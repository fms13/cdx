/**
 * \file File.cpp
 *
 * \date Mar 18, 2015
 * \author: Frank Schubert
 */

#include "File.h"

#include <stdexcept>

using namespace std;

/**
 * \addtogroup cpp_implementation
 * @{
 */

namespace CDX {

File::File(std::string _file_name) :
		file_name(_file_name), h5file(file_name.c_str(), H5F_ACC_RDONLY), c0_m_s(
				read_double_h5(h5file, "/parameters/c0_m_s")), cir_rate_Hz(
				read_double_h5(h5file, "/parameters/cir_rate_Hz")), transmitter_frequency_Hz(
				read_double_h5(h5file, "/parameters/transmitter_frequency_Hz")), delay_type(
				read_string_h5(h5file, "/parameters/delay_type")), links_group(
				h5file.openGroup("/links")), nof_links(links_group.getNumObjs()) {

	// open groups, one for each link:
	for (size_t k = 0; k < nof_links; k++) {
		std::string link_name = links_group.getObjnameByIdx(k);
		link_names.push_back(link_name);
		link_groups[link_name] = new H5::Group(
				links_group.openGroup(link_name));
	}

}

File::File(std::string _file_name, double _c0_m_s, double _cir_rate_Hz,
		double _transmitter_frequency_Hz,
		const std::vector<std::string> &_link_names) :
		file_name(_file_name), h5file(file_name.c_str(), H5F_ACC_TRUNC), c0_m_s(
				_c0_m_s), cir_rate_Hz(_cir_rate_Hz), transmitter_frequency_Hz(
				_transmitter_frequency_Hz), link_names(_link_names), links_group(
				h5file.createGroup("/links")), nof_links(link_names.size()) {

}

File::~File() {
	// close link groups which were openend in constructor:
	for (size_t k = 0; k < link_names.size(); k++) {
		delete link_groups[link_names.at(k)];
	}
}

double File::read_double_h5(H5::H5File file, std::string dataset_name) {

	H5::DataSet dataset = file.openDataSet(dataset_name);

	H5::DataSpace dataspace(dataset.getSpace());

	// get rank of data:
	const int rank = dataspace.getSimpleExtentNdims();

	if (rank > 1) {
		std::stringstream ss;
		ss << "read_double_h5: error: Dataset " << dataset_name
				<< " has rank > 1 (rank = " << rank << ").";
		throw std::runtime_error(ss.str());
	}

	double data = 0.0;
	dataset.read(&data, H5::PredType::NATIVE_DOUBLE);

	return data;
}

std::string File::read_string_h5(H5::H5File file, std::string path) {
	H5::DataSet delay_type_set = H5::DataSet(file.openDataSet(path));
	H5::DataType delay_type_type = delay_type_set.getDataType();
	int delay_type_len = delay_type_type.getSize();
	H5::DataSpace delay_type_space = H5::DataSpace(delay_type_set.getSpace());

	char delay_type[delay_type_len + 1];
	delay_type_set.read(delay_type, delay_type_type, delay_type_space);

	delay_type[delay_type_len] = 0;

	const std::string out = delay_type;
	return out;
}

} // end of namespace CDX

/**
 * @}
 */
