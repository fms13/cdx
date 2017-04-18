/**
 * \file example-01.cpp
 *
 * \author Frank Schubert
 * \date Apr 18, 2017
 *
 * \brief Example 01 using C++
 *
 * If libcdx-dev is installed on your Debian 8 (Jessie) compile with:
 *
 *     g++ example-01.cpp -o example-01 -lcdx -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial
 *
 * Start the program with:
 *
 *     ./example-01
 *
 * If you would like to use the CDX library from a non-standard location, use for e.g. $HOME/workspace/cdx/trunk/CPP:
 *
 *     g++ example-01.cpp -o example-01 -lcdx -I$HOME/workspace/cdx/trunk/CPP -L$HOME/workspace/cdx/trunk/CPP/.libs \
 *     -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial
 *
 * Start the program then with:
 *
 *     LD_LIBRARY_PATH=$HOME/workspace/cdx/trunk/CPP/.libs ./example-01
 */

#include <cdx/WriteContinuousDelayFile.h>
#include <armadillo>

using namespace std;

int main() {
	cout << "example-01 start." << endl;

	const string cdx_file_name = "example-01.cdx";
	const double c0_m_s = 3e8;
	const double cir_rate_Hz = 100;
	const double transmitter_frequency_Hz = 1.5e9;
	const vector<string> link_names = { "satellite0", "satellite1" };

	// define the component types:
	CDX::component_types_t component_types = { { 0, "LOS" },
			{ 256, "Scatterer" } };

	// we're using the same component_types for all links:
	CDX::links_to_component_types_t links_to_component_types;
	links_to_component_types["satellite0"] = component_types;
	links_to_component_types["satellite1"] = component_types;

	// create the CDX file:
	CDX::WriteContinuousDelayFile cdx_out(cdx_file_name, c0_m_s, cir_rate_Hz,
			transmitter_frequency_Hz, link_names, links_to_component_types);

	// write some channel impulse responses:
	CDX::CIR_number_t CIRnum = 0;
	for (size_t k = 0; k < 10; k++) {
		CDX::impulse_t cir_los = { 0, 0, 0.0, complex<double>(1.0, 0.0) };
		CDX::impulse_t cir_scatterer =
				{ 256, 1, 0.1, complex<double>(0.5, 0.5) };

		CDX::CIR single_cir;

		single_cir.push_back(cir_los);
		single_cir.push_back(cir_scatterer);

		// we use the same CIR for both links:
		map<std::string, CDX::CIR> cirs;
		cirs["satellite0"] = single_cir;
		cirs["satellite1"] = single_cir;

		// reference delays:
		map<std::string, double> reference_delays;
		reference_delays["satellite0"] = 0.0;
		reference_delays["satellite1"] = 0.0;

		cdx_out.write_cir(cirs, reference_delays, CIRnum);

		CIRnum++;

	}

	cout << "all done." << endl;

	return 0;
}
