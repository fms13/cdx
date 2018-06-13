/**
 * \file	ReadFile.cpp
 * \author	Frank M. Schubert
 */

#include "ReadFile.h"
#include <iostream>

namespace CDX {

using namespace std;

ReadFile::ReadFile(string _file_name) :
		File(_file_name) {

}

ReadFile::~ReadFile() {
}

double ReadFile::get_reference_delay(std::string link, size_t number) {
	H5::DataSet dataset = H5::DataSet(
			link_groups[link]->openDataSet("reference_delays"));

	H5::DataSpace dataspace = H5::DataSpace(dataset.getSpace());

	const int RANK = 1;

	// select hyperslab in the dataset (both stride and block are zero):
	const hsize_t count[RANK] = { 1 };
	const hsize_t offset[RANK] = { number };
	dataspace.selectHyperslab(H5S_SELECT_SET, count, offset);

	// define memory dataspace:
	hsize_t dimsm[RANK] = { 1 };
	H5::DataSpace memspace(RANK, dimsm);

	// define memory hyperslab:
	hsize_t offset_out[RANK] = { 0 };   // hyperslab offset in memory
	hsize_t count_out[RANK] = { 1 };
	memspace.selectHyperslab(H5S_SELECT_SET, count_out, offset_out);

	double ref_delay_h5[1];
	dataset.read(ref_delay_h5, H5::PredType::NATIVE_DOUBLE, memspace,
			dataspace);

	return ref_delay_h5[0];
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
