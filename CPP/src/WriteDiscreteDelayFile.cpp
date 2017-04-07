/**
 * \file	WriteDiscreteDelayFile.cpp
 * \author	Frank M. Schubert
 */

#include "WriteDiscreteDelayFile.h"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace CDX {

WriteDiscreteDelayFile::WriteDiscreteDelayFile(std::string _file_name,
		double _c0_m_s, double _cir_rate_Hz, double _transmitter_frequency_Hz,
		const std::vector<std::string> &_link_names, double _delay_smpl_freq_Hz) :
		WriteFile(_file_name, _c0_m_s, _cir_rate_Hz,
				_transmitter_frequency_Hz, _link_names), delay_smpl_freq_Hz(
				_delay_smpl_freq_Hz) {

	write("/parameters/delay_type", "discrete-delay");
	write("/parameters/delay_smpl_freq_Hz", delay_smpl_freq_Hz);

	for (size_t k = 0; k < nof_links; k++) {
		numbers_of_delay_samples[link_names.at(k)] = 0;
		min_delays[link_names.at(k)] = 0;
		act_cirs[link_names.at(k)] = 0;
	}
}

WriteDiscreteDelayFile::~WriteDiscreteDelayFile() {
	for (size_t k = 0; k < link_names.size(); k++) {
		const size_t nof_cirs = act_cirs[link_names.at(k)];
		// calculate x-axis:
		vector<double> x_axis(nof_cirs);
		const double cir_rate_Hz = get_cir_rate_Hz();
		for (size_t n = 0; n < nof_cirs; n++)
			x_axis.at(n) = static_cast<double>(n) / cir_rate_Hz;

		write(link_groups[link_names.at(k)], "x_axis", x_axis);
	}
}

void WriteDiscreteDelayFile::setup_link(std::string link_name,
		size_t number_of_delay_samples, double min_delay) {
	numbers_of_delay_samples[link_name] = number_of_delay_samples;
	min_delays[link_name] = min_delay;

	// calculate y-axis:
	vector<double> y_axis(number_of_delay_samples);
	for (size_t k = 0; k < number_of_delay_samples; k++)
		y_axis.at(k) = min_delay + static_cast<double>(k) / delay_smpl_freq_Hz;
	write(link_groups[link_name], "y_axis", y_axis);

	const int RANK = 2;
	// Create the data space with unlimited dimensions.
	hsize_t dims[2] = { number_of_delay_samples, 1 }; // dataset dimensions at creation
	hsize_t maxdims[2] = { H5S_UNLIMITED, H5S_UNLIMITED };
	H5::DataSpace mspace1(RANK, dims, maxdims);

	// Modify dataset creation properties, i.e. enable chunking.
	H5::DSetCreatPropList cparms;

	hsize_t chunk_dims[2] = { number_of_delay_samples, 1 };
	cparms.setChunk(RANK, chunk_dims);

	/*
	 * Set fill value for the dataset
	 */
	double fill_val = 0;
	cparms.setFillValue(H5::PredType::NATIVE_DOUBLE, &fill_val);
	/*
	 * Create a new dataset within the file using cparms
	 * creation properties. */
	H5::DataSet dataset_r = link_groups[link_name]->createDataSet("cirs_real",
			H5::PredType::NATIVE_DOUBLE, mspace1, cparms);
	H5::DataSet dataset_i = link_groups[link_name]->createDataSet("cirs_imag",
			H5::PredType::NATIVE_DOUBLE, mspace1, cparms);

}

void WriteDiscreteDelayFile::append_2d_dataset(H5::Group *group, string path,
		double *data, size_t length, size_t act_cir) {
	const int RANK = 2;

	// Create the data space with unlimited dimensions.
	hsize_t dims[RANK] = { length, 1 }; // dataset dimensions at creation
	hsize_t maxdims[RANK] = { H5S_UNLIMITED, H5S_UNLIMITED };
	H5::DataSpace mspace1(RANK, dims, maxdims);

	// open dataset
	H5::DataSet dataset = group->openDataSet(path);
	// Extend the dataset.
	hsize_t size[RANK];
	size[0] = length;
	size[1] = act_cir + 1;
	dataset.extend(size);

	// Select a hyperslab.
	H5::DataSpace fspace1 = dataset.getSpace();
	hsize_t offset[RANK];
	offset[0] = 0;
	offset[1] = act_cir;
	hsize_t dims1[RANK] = { length, 1 }; /* data1 dimensions */
	fspace1.selectHyperslab(H5S_SELECT_SET, dims1, offset);

	// Write the data to the hyperslab.
	dataset.write(data, H5::PredType::NATIVE_DOUBLE, mspace1, fspace1);
}

void WriteDiscreteDelayFile::append_cir_snapshot(std::string link_name,
		const vector<complex<double> > &data, double ref_delay) {
	// check if setup_link() has been called already:

	if (numbers_of_delay_samples[link_name] == 0)
		throw logic_error(
				"WriteDiscreteDelayCDXFile::append_cir_snapshot: setup_link() needs to be called before calling this function.");

	// partly from http://www.hdfgroup.org/HDF5/doc/cpplus_RM/extend__ds_8cpp-example.html
	// consistency check:
	if (data.size() != numbers_of_delay_samples[link_name]) {
		stringstream msg;
		msg << "error: link: " << link_name << ", data.size() (" << data.size()
				<< ") != number_of_delay_samples ("
				<< numbers_of_delay_samples[link_name] << ") !\n";
		throw logic_error(msg.str());
	}

	const size_t nof_samples = numbers_of_delay_samples[link_name];

	// convert vector to double[]
	double data_r[nof_samples];
	double data_i[nof_samples];
	for (size_t k = 0; k < nof_samples; k++) {
		data_r[k] = data.at(k).real();
		data_i[k] = data.at(k).imag();
	}

	append_2d_dataset(link_groups[link_name], "cirs_real", data_r, nof_samples,
			act_cirs[link_name]);

	append_2d_dataset(link_groups[link_name], "cirs_imag", data_i, nof_samples,
			act_cirs[link_name]);

	// append reference delay: ///////////////
	append_reference_delay(link_groups[link_name], ref_delay);

	act_cirs[link_name]++;
}

} // end of namespace CDX
