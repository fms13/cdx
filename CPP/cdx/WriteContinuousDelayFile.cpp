/**
 * \file	WriteContinuousDelayFile.cpp
 * \author	Frank M. Schubert
 */

#include "WriteContinuousDelayFile.h"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace CDX {

WriteContinuousDelayFile::WriteContinuousDelayFile(std::string _file_name,
		double _c0_m_s, double _cir_rate_Hz, double _transmitter_frequency_Hz,
		const std::vector<std::string> &_link_names,
		links_to_component_types_t &_component_types) :
		WriteFile(_file_name, _c0_m_s, _cir_rate_Hz, _transmitter_frequency_Hz,
				_link_names), component_types(_component_types) {

	// write CDX file type to HDF5 file:
	write("/parameters/delay_type", "continuous-delay");

	// check that there are the same number of link names as component types:
	if (link_names.size() != component_types.size()) {
		stringstream msg;
		msg << "WriteContinuousDelayFile: number of provided component types ("
				<< component_types.size()
				<< ") does not match number of links in file (" << nof_links
				<< ").";
		throw runtime_error(msg.str());
	}

	// check for empty link_names:
	if (link_names.size() == 0)
		throw runtime_error(
				"WriteContinuousDelayFile: link_names.size() is zero");

	// creating groups for links and cirs:
	for (auto link_name : link_names) {
		H5::Group *new_cir_group = new H5::Group(
				link_groups[link_name]->createGroup("cirs"));
		group_cirs[link_name] = new_cir_group;

		// write component types to file for each link:
		write(link_groups[link_name], "component_types",
				component_types[link_name]);
	}

	cp_cmplx = new H5::CompType(sizeof(hdf5_impulse_t));
	cp_cmplx->insertMember("type", HOFFSET(hdf5_impulse_t, type),
			H5::PredType::NATIVE_INT16);
	cp_cmplx->insertMember("id", HOFFSET(hdf5_impulse_t, id),
			H5::PredType::NATIVE_UINT64);
	cp_cmplx->insertMember("delay", HOFFSET(hdf5_impulse_t, delay),
			H5::PredType::NATIVE_DOUBLE);
	cp_cmplx->insertMember("real", HOFFSET(hdf5_impulse_t, real),
			H5::PredType::NATIVE_DOUBLE);
	cp_cmplx->insertMember("imag", HOFFSET(hdf5_impulse_t, imag),
			H5::PredType::NATIVE_DOUBLE);
}

void WriteContinuousDelayFile::write_cir(
		std::map<std::string, components_t> cirs,
		std::map<std::string, double> reference_delays,
		cir_number_t cir_number) {

	if (cirs.size() != nof_links) {
		stringstream msg;
		msg << "error: Number of provided CIRs (" << cirs.size()
				<< ") does not match number of links in file (" << nof_links
				<< ").";
		throw runtime_error(msg.str());
	}

	if (reference_delays.size() != nof_links) {
		stringstream msg;
		msg << "error: Number of provided reference delays ("
				<< reference_delays.size()
				<< ") does not match number of links in file (" << nof_links
				<< ").";
		throw logic_error(msg.str());
	}

	for (auto cir : cirs) {
		const string link_name = cir.first;

		// write reference delay:
		append_reference_delay(link_groups[link_name],
				reference_delays[link_name]);

		// write CIR:
		const vector<CDX::impulse_t> impulses = cir.second;

		stringstream dsName;
		dsName << cir_number;

		hdf5_impulse_t wdata[impulses.size()];

		const size_t RANK = 1;
		hsize_t dimsf3[RANK]; // dataset dimensions
		dimsf3[0] = impulses.size();
		H5::DataSpace dspace3(RANK, dimsf3);

		H5::DataSet dset3 = group_cirs[link_name]->createDataSet(
				dsName.str().c_str(), *cp_cmplx, dspace3);

		for (size_t i = 0; i < impulses.size(); i++) {
			wdata[i].type = impulses.at(i).type;
			wdata[i].id = impulses.at(i).id;
			wdata[i].delay = impulses.at(i).delay;
			wdata[i].real = impulses.at(i).amplitude.real();
			wdata[i].imag = impulses.at(i).amplitude.imag();
		}

		dset3.write(wdata, *cp_cmplx);
	}
}

WriteContinuousDelayFile::~WriteContinuousDelayFile() {
	// close cirs groups which were opened in constructor
	for (const auto link_name : link_names)
		delete group_cirs[link_name];

	delete cp_cmplx;
}

} // end of namespace CDX
