/**
 * \file cdx-convert-continuous-to-discrete.cpp
 *
 * \date Nov 24, 2011
 * \author Frank M. Schubert
 *
 * \brief This file contains the implementation of a tool which converts continuous-
 * delay CDX files to discrete-delay CDX files.
 *
 */

#ifndef __APPLE__
#include <omp.h>
#endif

#include <boost/timer.hpp>
#include <boost/progress.hpp>
#include <boost/math/special_functions/sinc.hpp>
#include <boost/program_options.hpp> // for reading command line parameters
namespace po = boost::program_options;

#include <armadillo>

#include "cdx/WriteDiscreteDelayFile.h"
#include "cdx/ReadContinuousDelayFile.h"

/**
 * \brief Computes the Hamming window function
 *
 * \param[in] n number of coefficients
 * \return vector with window coefficients
 */
arma::vec hamming(const int n) {
	arma::vec t(n);

	if (n == 1)
		t(0) = 0.08;
	else
		for (int i = 0; i < n; i++)
			t[i] = (0.54 - 0.46 * std::cos(2.0 * M_PI * i / (n - 1)));

	return t;
}

/**
 * \brief Mirrors a signal after fft.
 *
 * \param[in] v incoming signal vector
 * \return mirrored vector
 */
arma::cx_vec fftshift(const arma::cx_vec &v) {
	const int nr = v.size() / 2;
	return arma::join_vert(v.rows(nr, v.size() - 1), v.rows(0, nr - 1));
}

using namespace std;

int main(int argc, char **argv) {
	cout
			<< "\n==== Convert continuous-delay to discrete-delay CDX file ====\n\n";

	cout << "Copyright (C) 2011-2016  Frank M. Schubert\n"
			<< "This program comes with ABSOLUTELY NO WARRANTY.\n"
			<< "This is free software, and you are welcome to redistribute it\n"
			<< "under certain conditions. See the licenses/gpl-3.0.txt file for more information.\n";

	// Declare the supported command line options:
	// from http://www.boost.org/doc/libs/1_55_0b1/doc/html/program_options/tutorial.html
	po::options_description desc("Allowed options");
	desc.add_options()("help", "produce help message")("input-file,i",
			po::value<string>(), "input continuous-delay CDX file")(
			"output-file,o", po::value<string>(),
			"output discrete-delay CDX file")("sampling-frequency,s",
			po::value<double>(),
			"sampling frequency of the delay axis in Hz of the resulting CDX file")(
			"delay-before-min,b", po::value<double>(),
			"delay to add before minimum delay")("delay-after-max,a",
			po::value<double>(), "delay to add after maximum delay")(
			"enable-filtering,e", po::bool_switch(), "enable filtering")(
			"subtract-reference-delay,r", po::bool_switch(),
			"subtract the reference for each CIR")("type,t",
			po::value<unsigned>(),
			"only process components with types equal to this number.");

	// parse command line options:
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("input-file") != 1)
		throw std::runtime_error("no input file name given.");

	const string input_file = vm["input-file"].as<string>();

	if (vm.count("output-file") != 1)
		throw std::runtime_error("no input file name given.");

	const string output_file = vm["output-file"].as<string>();

	const double smpl_freq = vm["sampling-frequency"].as<double>();

	const double delay_before_min = vm["delay-before-min"].as<double>();
	const double delay_after_max = vm["delay-after-max"].as<double>();
	const bool filter_enabled = vm["enable-filtering"].as<bool>();
	const bool subtract_ref_delay_from_components =
			vm["subtract-reference-delay"].as<bool>();

	const bool filter_by_types = vm.count("type") == 0 ? false : true;
	unsigned type_to_process = 0;
	if (filter_by_types == true) {
		cout << "info: filtering by types, only components with type equal to "
				<< type_to_process << "will be processed.\n";
		type_to_process = vm["type"].as<unsigned>();
	}

	cout << "info: open input file: " << input_file << "...\n";

	CDX::ReadContinuousDelayFile cdx_in(input_file);

	const vector<string> link_names = cdx_in.get_link_names();

	// open CDX output file for interpolation results:
	const double param_c0_m_s = cdx_in.get_c0_m_s();
	cout << "info: c0 = " << param_c0_m_s << " m/s\n";

	const double cir_rate_Hz = cdx_in.get_cir_rate_Hz();
	cout << "info: cir_rate_Hz = " << cir_rate_Hz << " Hz\n";

	const unsigned int nof_cirs = cdx_in.get_nof_cirs();
	cout << "info: number of CIRs in file: " << nof_cirs << "\n";

	const double transmitter_frequency_Hz =
			cdx_in.get_transmitter_frequency_Hz();
	cout << "info: transmitter_frequency: " << transmitter_frequency_Hz
			<< " Hz\n";

	cout << "info: opening output_file: " << output_file << "...\n";
	CDX::WriteDiscreteDelayFile cdx_out = CDX::WriteDiscreteDelayFile(
			output_file, param_c0_m_s, cir_rate_Hz, transmitter_frequency_Hz,
			link_names, smpl_freq);

	for (size_t link = 0; link < link_names.size(); link++) {
		cout << "process: reading CIRs of link " << link_names.at(link)
				<< " ... ";
		cout.flush();

		// read in whole input file
		vector<CDX::cir_t> cirs(nof_cirs);
		vector<double> reference_delays(nof_cirs);

		// find minimum and maximum delay in file:
		double delay_min = std::numeric_limits<double>::max();
		double delay_max = std::numeric_limits<double>::min();

		for (unsigned int k = 0; k < nof_cirs; k++) {
			cirs.at(k) = cdx_in.get_cir(link_names.at(link), k);
			reference_delays.at(k) = cirs.at(k).ref_delay;

			// read components
			for (size_t n = 0; n < cirs.at(k).components.size(); n++) {
				if (subtract_ref_delay_from_components == true)
					cirs.at(k).components.at(n).delay -= reference_delays.at(k);

				if (cirs.at(k).components.at(n).delay < delay_min) {
					delay_min = cirs.at(k).components.at(n).delay;
				}
				if (cirs.at(k).components.at(n).delay > delay_max) {
					delay_max = cirs.at(k).components.at(n).delay;
				}
			}
		}

		// correct reference delay if necessary:
		if (subtract_ref_delay_from_components == true)
			for (size_t k = 0; k < reference_delays.size(); k++)
				reference_delays.at(k) = 0.0;

		cout << "done.\n";
		cout << "process: minimum delay in file: " << delay_min << "\n";
		cout << "process: maximum delay in file: " << delay_max << "\n";
		cout << "info: delay before min delay: " << delay_before_min << "\n";
		cout << "info: delay after max delay: " << delay_after_max << "\n";

		// compute delay length of interpolation interval:
		const double interval_length = (delay_max + delay_after_max)
				- (delay_min - delay_before_min);
		cout << "process: interpolation interval: " << interval_length << "\n";

		// calculate interpolated CIR length:
		const size_t nof_coeffs = round(interval_length * smpl_freq);
		cout << "process: number of coefficients: " << nof_coeffs << "\n";

		cout << "info: smpl_freq = " << smpl_freq << "\n";

		cout << "info: allocating "
				<< nof_cirs * nof_coeffs * 16 / 1024.0 / 1024.0 << " MB... ";
		cout.flush();

		arma::cx_mat interp_cirs(nof_cirs, nof_coeffs);
		interp_cirs.zeros();
		cout << "done.\n";

		cout << "process: interpolating CIRs... " << endl;
		cout.flush();
		boost::progress_display show_progress(nof_cirs);

		const double interpolation_bandwidth = smpl_freq / 2.0;
		const double Om = 2.0 * M_PI * (interpolation_bandwidth);

		const arma::cx_rowvec window = fftshift(
				arma::conv_to<arma::cx_vec>::from(hamming(nof_coeffs))).st();

		if (window.size() != nof_coeffs)
			throw std::runtime_error("window.size() != nof_coeffs");

		size_t k, c;
		size_t n;
#pragma omp parallel for private (k, c, n)
		// for all CIRs
		for (k = 0; k < nof_cirs; k++) {
			for (c = 0; c < cirs.at(k).components.size(); c++) { // for all components
				if (filter_by_types == true
						and type_to_process != cirs.at(k).components.at(c).type)
					continue;

				for (n = 0; n < nof_coeffs; n++) { // for all coefficients
					const double fac = Om
							* (static_cast<double>(n) / smpl_freq
									- (cirs.at(k).components.at(c).delay
											- delay_min + delay_before_min));
					const double sin_term = boost::math::sinc_pi(fac);

					interp_cirs(k, n) += cirs.at(k).components.at(c).amplitude
							* sin_term;

					/*
					 const double fac = Om
					 * (static_cast<double>(n) / smpl_freq
					 - (cirs[k].components[c].delay
					 + delay_before_min));
					 const double sin_term = sin(fac) / fac;

					 interp_cirs(k, n) += cirs[k].components[c].weight
					 * sin_term;
					 */
				}
			}

			if (filter_enabled == true) {
				// filter the CIR:

				// fft is not thread-safe, so we must do:
#pragma omp critical
				{
					const arma::cx_rowvec cir_fft = arma::fft(
							interp_cirs.row(k));
					const arma::cx_rowvec cir_fft_window = cir_fft % window;
					const arma::cx_rowvec filtered_cir = arma::ifft(
							cir_fft_window);
					interp_cirs.row(k) = filtered_cir;
				}
			}

			++show_progress;
		}
		cout << endl << "process: writing CIRs... ";
		cout.flush();

		// setup link in CDX file:
		cdx_out.setup_link(link_names.at(link), nof_coeffs, delay_min);

		for (size_t n = 0; n < nof_cirs; n++) { // for all CIRs

			// convert armadillo vec to stdlib vec:
			vector<complex<double> > std_cir = arma::conv_to<
					vector<complex<double> > >::from(interp_cirs.row(n));

			cdx_out.append_cir_snapshot(link_names.at(link), std_cir,
					reference_delays.at(n) + delay_before_min);
		}
		cout << "done.\n";
	}
	cout << "all done. exit.\n";
	return 0;
}
