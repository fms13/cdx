/**
 * \file	ReadFile.cpp
 * \author	Frank M. Schubert
 */

#include "ReadFile.h"

namespace CDX {

using namespace std;

ReadFile::ReadFile(string _file_name) :
		File(_file_name) {

}

ReadFile::~ReadFile() {
}

vector<double> ReadFile::get_reference_delays(std::string link) {

	H5::DataSet dataset = H5::DataSet(
			link_groups[link]->openDataSet("reference_delays"));

	H5::DataSpace dataspace = H5::DataSpace(dataset.getSpace());

	size_t nof_cirs = dataspace.getSimpleExtentNpoints();

	double ref_delays_h5[nof_cirs];
	dataset.read(ref_delays_h5, H5::PredType::NATIVE_DOUBLE);

	// convert to vector:
	vector<double> ref_delays(nof_cirs);

	for (size_t m = 0; m < ref_delays.size(); m++) {
		ref_delays.at(m) = ref_delays_h5[m];
	}

	return ref_delays;
}

} // end of namespace CDX
