/**
 * \file cdx-test-write-read-continuous-delay-cdx-file
 *
 * \date Feb 16, 2018
 * \author Frank Schubert
 *
 * \copyright All rights reserved.
 *
 * \brief Creates a CDX file using WriteContinuousDelayFile and writes test data. Two links are created, link0 and link1. For
 * each link, two CIRs are created and written.
 *
 * The data is then read back with ReadContinuousDelayFile and compared to the original data written.
 */

#include "../../cdx/WriteContinuousDelayFile.h"
#include "../../cdx/ReadContinuousDelayFile.h"

#include <iostream>

using namespace std;

int main(void) {
	cout << "cdx-test-write-read-continuous-delay-cdx-file start." << endl;

	const string file_name = "cdx-test-write-read-continuous-delay-cdx-file.cdx";

	// constant parameters:
	const double c0_m_s = 3e8;
	const double cir_rate_Hz = 100.0;
	const double transmitter_frequency_Hz = 1e6;

	// we create two links in the file:
	const vector<string> link_names { "link0", "link1" };

	// assignment of integer numbers to component type names:
	CDX::component_types_t component_types = { { 0, "component-type-0" }, { 1,
			"component-type-1" } };

	// assignment of link names to component types map:
	CDX::links_to_component_types_t links_to_component_types = { { "link0",
			component_types }, { "link1", component_types } };

	const size_t nof_cirs = 5000;

	{
		// create the CDX file:
		CDX::WriteContinuousDelayFile cdx_out(file_name, c0_m_s, cir_rate_Hz,
				transmitter_frequency_Hz, link_names, links_to_component_types);

		cout << "writing " << nof_cirs << " CIRs... ";
		cout.flush();

		// write some CIRs:
		for (CDX::cir_number_t cir_number = 0; cir_number < nof_cirs;
				cir_number++) {

			// we create two impulses with test data for link0...
			CDX::impulse_t impulse_link0_1;
			impulse_link0_1.type = 0;
			impulse_link0_1.id = 1 * cir_number;
			impulse_link0_1.delay = 2.0 * cir_number;
			impulse_link0_1.amplitude = complex<double>(3.0 * cir_number,
					4.0 * cir_number);

			CDX::impulse_t impulse_link0_2;
			impulse_link0_2.type = 1;
			impulse_link0_2.id = 5 * cir_number;
			impulse_link0_2.delay = 6.0 * cir_number;
			impulse_link0_2.amplitude = complex<double>(7.0 * cir_number,
					8.0 * cir_number);

			// ...and two with test data for link1:
			CDX::impulse_t impulse_link1_1;
			impulse_link1_1.type = 0;
			impulse_link1_1.id = 9 * cir_number;
			impulse_link1_1.delay = 10.0 * cir_number;
			impulse_link1_1.amplitude = complex<double>(11.0 * cir_number,
					12.0 * cir_number);

			CDX::impulse_t impulse_link1_2;
			impulse_link1_2.type = 1;
			impulse_link1_2.id = 13 * cir_number;
			impulse_link1_2.delay = 14.0 * cir_number;
			impulse_link1_2.amplitude = complex<double>(15.0 * cir_number,
					16.0 * cir_number);

			// gather the two impulses for link0...
			CDX::components_t components_link0 = { impulse_link0_1,
					impulse_link0_2 };

			// ...gather the two impulses for link1...
			CDX::components_t components_link1 = { impulse_link1_1,
					impulse_link1_2 };

			// ...and combine them to build the complete CIR:
			map<string, CDX::components_t> cirs =
					{ { "link0", components_link0 },
							{ "link1", components_link1 } };

			// define the map that assigns link names to reference delays:
			const map<string, double> reference_delays = { { "link0", 17.0
					* cir_number }, { "link1", 18.0 * cir_number } };

			cdx_out.write_cir(cirs, reference_delays, cir_number);
		}
		cout << "done." << endl;

		// file is written and closed here.

		// read it back in:
		{
			// create the CDX file:
			CDX::ReadContinuousDelayFile cdx_in(file_name);

			// test constant data:
			if (c0_m_s != cdx_in.get_c0_m_s()) {
				stringstream ss;
				ss << "c0_m_s read from file (" << cdx_in.get_c0_m_s()
						<< ")does not match input (" << c0_m_s << ").";
				throw runtime_error(ss.str());
			}

			if (cir_rate_Hz != cdx_in.get_cir_rate_Hz()) {
				stringstream ss;
				ss << "cir_rate_Hz read from file (" << cdx_in.get_cir_rate_Hz()
						<< ")does not match input (" << cir_rate_Hz << ").";
				throw runtime_error(ss.str());
			}

			if (transmitter_frequency_Hz
					!= cdx_in.get_transmitter_frequency_Hz()) {
				stringstream ss;
				ss << "transmitter_frequency_Hz read from file ("
						<< cdx_in.get_transmitter_frequency_Hz()
						<< ")does not match input (" << transmitter_frequency_Hz
						<< ").";
				throw runtime_error(ss.str());
			}

			if (nof_cirs != cdx_in.get_nof_cirs()) {
				stringstream ss;
				ss << "nof_cirs read from file (" << cdx_in.get_nof_cirs()
						<< ")does not match input (" << nof_cirs << ").";
				throw runtime_error(ss.str());
			}

			cout << "constant parameters in file match the input data." << endl;

			cout << "reading " << nof_cirs << " CIRs... ";
			cout.flush();

			// write some CIRs:
			for (CDX::cir_number_t cir_number = 0; cir_number < nof_cirs;
					cir_number++) {

				// get CIR for link0:
				const CDX::cir_t cir_link0 = cdx_in.get_cir("link0",
						cir_number);

				// link0: test first CIR:
				if (cir_link0.components.at(0).type != 0) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(0).type does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link0.components.at(0).id != 1 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(0).id does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link0.components.at(0).delay != 2.0 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(0).delay does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link0.components.at(0).amplitude
						!= complex<double>(3.0 * cir_number,
								4.0 * cir_number)) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(0).amplitude does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				// link0: test second CIR:
				if (cir_link0.components.at(1).type != 1) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(1).type does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link0.components.at(1).id != 5 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(1).id does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link0.components.at(1).delay != 6.0 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(1).delay does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link0.components.at(1).amplitude
						!= complex<double>(7.0 * cir_number,
								8.0 * cir_number)) {
					stringstream ss;
					ss
							<< "cir_link0.components.at(1).amplitude does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				// get CIR for link1:
				const CDX::cir_t cir_link1 = cdx_in.get_cir("link1",
						cir_number);

				// link1: test first CIR:
				if (cir_link1.components.at(0).type != 0) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(0).type does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link1.components.at(0).id != 9 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(0).id does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link1.components.at(0).delay != 10.0 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(0).delay does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link1.components.at(0).amplitude
						!= complex<double>(11.0 * cir_number,
								12.0 * cir_number)) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(0).amplitude does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				// link1: test second CIR:
				if (cir_link1.components.at(1).type != 1) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(1).type does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link1.components.at(1).id != 13 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(1).id does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link1.components.at(1).delay != 14.0 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(1).delay does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				if (cir_link1.components.at(1).amplitude
						!= complex<double>(15.0 * cir_number,
								16.0 * cir_number)) {
					stringstream ss;
					ss
							<< "cir_link1.components.at(1).amplitude does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				// link0: check reference delay:
				if (cir_link0.ref_delay != 17.0 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link0.ref_delay does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}

				// link1: check reference delay:
				if (cir_link1.ref_delay != 18.0 * cir_number) {
					stringstream ss;
					ss
							<< "cir_link1.ref_delay does not match input data. cir_number: "
							<< cir_number;
					throw runtime_error(ss.str());
				}
			}
		}
	}

	cout << "all values match." << endl;

	cout << "all done." << endl;
}
