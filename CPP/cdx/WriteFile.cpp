/**
 * \file	WriteFile.cpp
 *
 * \author	Frank M. Schubert
 */

#include "WriteFile.h"

#include <iostream>
#include <vector>
#include <complex>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

using namespace std;

namespace CDX {

WriteFile::WriteFile(std::string _file_name, double _c0_m_s,
		double _cir_rate_Hz, double _transmitter_frequency_Hz,
		std::vector<std::string> _link_names) :
		File(_file_name, _c0_m_s, _cir_rate_Hz, _transmitter_frequency_Hz,
				_link_names) {

	// creating groups for all links:
	for (size_t k = 0; k < nof_links; k++) {
		if (link_names.at(k) == "") {
			throw runtime_error(
					"WriteCDXFile: link name for link #"
							+ boost::lexical_cast<string>(k) + " is empty.");
		}

		H5::Group *new_link_group = new H5::Group(
				links_group.createGroup(link_names.at(k)));
		link_groups[link_names.at(k)] = new_link_group;

		create_reference_delays_dataset(link_groups[link_names.at(k)]);
	}

	// write parameters to file:
	H5::Group group_parameters(h5file.createGroup("/parameters"));
	write("/parameters/c0_m_s", c0_m_s);
	write("/parameters/cir_rate_Hz", cir_rate_Hz);
	write("/parameters/transmitter_frequency_Hz", transmitter_frequency_Hz);

}

WriteFile::~WriteFile() {
}

void WriteFile::create_group(string path) {
	H5::Group group_links(h5file.createGroup(path.c_str()));
}

void WriteFile::write(string path, string data) {
	const char *data_out = data.c_str();

	hsize_t dimsf[1]; // dataset dimensions
	dimsf[0] = 1;
	H5::DataSpace dataspace(1, dimsf);

	H5::StrType str1(0, data.size());
	H5::DataSet dataset = h5file.createDataSet(path.c_str(), str1, dataspace);
	dataset.write((void *) data_out, str1);
}

void WriteFile::write(string path, double data) {
	hsize_t dimsf[1]; // dataset dimensions
	dimsf[0] = 1;
	H5::DataSpace dataspace(1, dimsf);
	H5::DataSet dataset = h5file.createDataSet(path.c_str(),
			H5::PredType::NATIVE_DOUBLE, dataspace);
	dataset.write(&data, H5::PredType::NATIVE_DOUBLE);
}

void WriteFile::write(string path, double *data, size_t num) {
	const size_t RANK = 2;

	hsize_t dimsf3[RANK]; // dataset dimensions
	dimsf3[0] = num;
	dimsf3[1] = 1;
	H5::DataSpace dspace3(RANK, dimsf3);
	H5::DataSet dset3 = h5file.createDataSet(path.c_str(),
			H5::PredType::NATIVE_DOUBLE, dspace3);
	dset3.write(data, H5::PredType::NATIVE_DOUBLE);
}

void WriteFile::write(string path, const vector<double> &data) {
	const size_t RANK = 2;

	// dataset dimensions: write vector as row vector:
	hsize_t dimsf3[RANK];
	dimsf3[0] = 1;
	dimsf3[1] = data.size();
	H5::DataSpace dspace3(RANK, dimsf3);

	H5::DataSet dset3 = h5file.createDataSet(path.c_str(),
			H5::PredType::NATIVE_DOUBLE, dspace3);
	dset3.write(&data[0], H5::PredType::NATIVE_DOUBLE);
}

void WriteFile::write(string path, const vector<vector<double> > &data) {
	// convert to double[][]:
	size_t dim1 = data.size();
	size_t dim2 = data.at(0).size();

	double wdata[dim1][dim2];
	for (size_t k = 0; k < dim1; k++) {
		for (size_t n = 0; n < dim2; n++) {
			wdata[k][n] = data.at(k).at(n);
		}
	}
	const size_t RANK = 2;
	hsize_t dimsf3[RANK]; // dataset dimensions
	dimsf3[0] = dim1;
	dimsf3[1] = dim2;
	H5::DataSpace dspace3(RANK, dimsf3);
	H5::DataSet dset3 = h5file.createDataSet(path.c_str(),
			H5::PredType::NATIVE_DOUBLE, dspace3);
	dset3.write(wdata, H5::PredType::NATIVE_DOUBLE);
}

#if H5_VERS_MAJOR >= 1 and H5_VERS_MINOR >= 10
void WriteFile::write(const H5::H5Location *h5file, const std::string &path,
		const std::vector<std::string> &data) {
#else
void WriteFile::write(const H5::CommonFG *h5file, const std::string &path,
		const std::vector<std::string> &data) {
#endif

	// based on
	// http://stackoverflow.com/questions/581209/how-to-best-write-out-a-stdvector-stdstring-container-to-a-hdf5-dataset

	const size_t nof_lines = data.size();

	const int RANK = 1;

	// make a string  dataspace:
	hsize_t str_dimsf[RANK] = { nof_lines };
	H5::DataSpace str_dataspace(RANK, str_dimsf);

	H5::StrType str_type(H5::PredType::C_S1, H5T_VARIABLE);

	H5::DataSet dset = h5file->createDataSet(path, str_type, str_dataspace);

	// HDF5 only understands vector of char*
	vector<const char*> arr_c_str;
	for (size_t n = 0; n < nof_lines; n++)
		arr_c_str.push_back(data.at(n).c_str());

	dset.write(arr_c_str.data(), str_type);
}

#if H5_VERS_MAJOR >= 1 and H5_VERS_MINOR >= 10
void WriteFile::write(const H5::H5Location* h5file, const std::string& path,
		const std::map<uint16_t, std::string>& data) {
#else
void WriteFile::write(const H5::CommonFG* h5file, const std::string& path,
		const std::map<uint16_t, std::string>& data) {
#endif

	// C struct for the compound data typo:
	typedef struct component_type_t {
		uint16_t id;
		const char *name;
	} component_type_t;

	// definition for a variable length string:
	H5::StrType str_type(H5::PredType::C_S1, H5T_VARIABLE);

	// create the HDF5 compound data type:
	H5::CompType comp_t(sizeof(component_type_t));

	comp_t.insertMember("id", HOFFSET(component_type_t, id),
			H5::PredType::NATIVE_UINT16);
	comp_t.insertMember("name", HOFFSET(component_type_t, name),
			str_type);

	// define dimensions:
    const int rank = 1;
    hsize_t dims[rank] = { data.size() }; // dataset dimensions

    H5::DataSpace dataspace(rank, dims);

    H5::DataSet dset = h5file->createDataSet(path,
    		comp_t, dataspace);

	// HDF5 only understands vector of char*
	vector<component_type_t> data_as_c_struct;
	vector<char *> component_names;
	for (auto name : data) {
		// dynamically allocate char arrays, otherwise writing does not work if *name in component_type_t points to name.second.c_str():
		component_names.push_back(new char[name.second.size()]);
		strcpy(component_names.back(), name.second.c_str());
		data_as_c_struct.push_back({name.first, component_names.back()});
	}

	dset.write(data_as_c_struct.data(), comp_t);

	// cleanup:
	for (auto component_name : component_names) {
		delete[] component_name;
	}
}

void WriteFile::create_reference_delays_dataset(H5::Group* group) {
	// creat dataset for reference delays:
	const int RANK = 1;

	// Create the data space with unlimited dimensions
	hsize_t dims_refdelay[RANK] = { 0 }; // dataset dimensions at creation
	hsize_t maxdims_refdelay[RANK] = { H5S_UNLIMITED };
	H5::DataSpace mspace1_refdelay(RANK, dims_refdelay, maxdims_refdelay);

	// Modify dataset creation properties, i.e. enable chunking.
	H5::DSetCreatPropList cparms;

	hsize_t chunk_dims_refdelay[RANK] = { 1 };
	cparms.setChunk(RANK, chunk_dims_refdelay);

	/*
	 // Set fill value for the dataset
	 double fill_val = 0;
	 cparms.setFillValue(H5::PredType::NATIVE_DOUBLE, &fill_val);
	 */

	// Create a new dataset within the file using cparms
	H5::DataSet dataset_refdelay = group->createDataSet("reference_delays",
			H5::PredType::NATIVE_DOUBLE, mspace1_refdelay, cparms);
}

void WriteFile::write(H5::Group *group, string path,
		const vector<double>& data) {
	const size_t RANK = 2;

	hsize_t dimsf3[RANK]; // dataset dimensions
	dimsf3[0] = data.size();
	dimsf3[1] = 1;
	H5::DataSpace dspace3(RANK, dimsf3);
	H5::DataSet dset3 = group->createDataSet(path.c_str(),
			H5::PredType::NATIVE_DOUBLE, dspace3);
	dset3.write(&data[0], H5::PredType::NATIVE_DOUBLE);
}

void WriteFile::append_reference_delay(H5::Group *group,
		double reference_delay) {
	const int RANK = 1;

	// Create the data space with unlimited dimensions
	hsize_t dims_refdelay[RANK] = { 1 }; // dataset dimensions at creation
	hsize_t maxdims_refdelay[RANK] = { H5S_UNLIMITED };
	H5::DataSpace mspace1_refdelay(RANK, dims_refdelay, maxdims_refdelay);

	H5::DataSet dataset = group->openDataSet("reference_delays");

	H5::DataSpace space = dataset.getSpace();
	const hsize_t actual_dims = space.getSimpleExtentNpoints();

	// Extend the dataset
	hsize_t new_size[RANK];
	new_size[0] = actual_dims + 1;
	dataset.extend(new_size);

	// Select a hyperslab:
	H5::DataSpace fspace1_refdelay = dataset.getSpace();
	hsize_t offset_refdelay[RANK] = { actual_dims };
	hsize_t dims1_refdelay[RANK] = { 1 }; // data1 dimensions
	fspace1_refdelay.selectHyperslab(H5S_SELECT_SET, dims1_refdelay,
			offset_refdelay);

	// Write the data to the hyperslab:
	double ref_delay_data[1] = { reference_delay };
	dataset.write(ref_delay_data, H5::PredType::NATIVE_DOUBLE, mspace1_refdelay,
			fspace1_refdelay);
}

} // end of namespace CDX
