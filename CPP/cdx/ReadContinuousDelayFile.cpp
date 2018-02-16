/**
 * \file	ReadContinuousDelayCDXFile.cpp
 * \brief	Implementation of reading channel impulse responses from a HDF5 file.
 *
 * \author	Frank M. Schubert
 */

/**
 * \addtogroup cpp_implementation
 * @{
 */

#include "ReadContinuousDelayFile.h"

#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace std;

namespace CDX {

const bool sdebug = false;

ReadContinuousDelayFile::ReadContinuousDelayFile(string _file_name) :
		ReadFile(_file_name) {

	// delay-type has to be continuous-delay:
	if (delay_type != "continuous-delay") {
		stringstream err_msg;
		err_msg << "delay_type (" << delay_type
				<< ") must be 'continuous-delay'!";
		throw logic_error(err_msg.str());
	}

	// open group for cirs for each link:
	for (size_t k = 0; k < link_names.size(); k++) {
		cir_groups[link_names.at(k)] = new H5::Group(
				link_groups[link_names.at(k)]->openGroup("cirs"));
	}

	// read number of cirs
	// for link 0:
	nof_cirs = cir_groups[link_names.at(0)]->getNumObjs();

	for (size_t k = 0; k < link_names.size(); k++) {
		const size_t nof_cirs_in_link = cir_groups[link_names.at(k)]->getNumObjs();

		if (nof_cirs != nof_cirs_in_link) {
			stringstream err_msg;
			err_msg << "snReadCIRFile: number of cirs for link 0 (" << nof_cirs
					<< ") differs from number of cirs for link("
					<< nof_cirs_in_link << ")!";
			throw logic_error(err_msg.str());
		}
	}

	// prepare echo compound type for for function get_cir:
	cp_echo = new H5::CompType(sizeof(hdf5_impulse_t));
	cp_echo->insertMember("type", HOFFSET(hdf5_impulse_t, type),
			H5::PredType::NATIVE_INT16);
	cp_echo->insertMember("id", HOFFSET(hdf5_impulse_t, id),
			H5::PredType::NATIVE_UINT64);
	cp_echo->insertMember("delay", HOFFSET(hdf5_impulse_t, delay),
			H5::PredType::NATIVE_DOUBLE);
	cp_echo->insertMember("real", HOFFSET(hdf5_impulse_t, real),
			H5::PredType::NATIVE_DOUBLE);
	cp_echo->insertMember("imag", HOFFSET(hdf5_impulse_t, imag),
			H5::PredType::NATIVE_DOUBLE);
}

cir_t ReadContinuousDelayFile::get_cir(std::string link,
		unsigned int cir_num) {
	cir_t result_cir;

	if (cir_groups.count(link) < 1) {
		throw logic_error(
				"ReadContinuousDelayCDXFile::get_cir: did not find link in file.");
	}

	if (cir_num > nof_cirs - 1) {
		throw logic_error(
				"ReadContinuousDelayCDXFile::get_cir: parameter cir_num is greater than number of cirs in file.");
	}

	H5::DataSet dataset = H5::DataSet(
			cir_groups[link]->openDataSet("/links/" + link + "/cirs/" + boost::lexical_cast<string>(cir_num)));

	H5::DataSpace dataspace = H5::DataSpace(dataset.getSpace());

	int echoes_amount = dataspace.getSimpleExtentNpoints();

	hdf5_impulse_t echoes[echoes_amount];
	dataset.read(echoes, *cp_echo);

	result_cir.components.resize(echoes_amount);

	for (int i = 0; i < echoes_amount; i++) {
		result_cir.components.at(i).type = echoes[i].type;
		result_cir.components.at(i).id = echoes[i].id;
		result_cir.components.at(i).delay = echoes[i].delay;
		result_cir.components.at(i).amplitude = complex<double>(echoes[i].real,
				echoes[i].imag);
	}

	result_cir.ref_delay = get_reference_delay(link, cir_num);

	return result_cir;
}

ReadContinuousDelayFile::~ReadContinuousDelayFile() {
	for (auto link_name : link_names)
		delete cir_groups[link_name];

	delete cp_echo;
}

} // end of namespace CDX

/**
 * @}
 */
