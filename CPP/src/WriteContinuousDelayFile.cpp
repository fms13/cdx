/**
 * \file	WriteContinuousDelayFile.cpp
 * \author	Frank M. Schubert
 */

/*
 *  This file is part of the program
 *
 *  SNACS - The Satellite Navigation Radio Channel Simulator
 *
 *  Copyright (C) 2012-2013  Frank M. Schubert
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "WriteContinuousDelayFile.h"

#include <iostream>
#include <stdexcept>

using namespace std;

namespace CDX {

WriteContinuousDelayFile::WriteContinuousDelayFile(std::string _file_name,
		double _c0_m_s, double _cir_rate_Hz, double _transmitter_frequency_Hz,
		const std::vector<std::string> &_link_names,
		const std::vector<component_types_t> &_component_types) :
		WriteFile(_file_name, _c0_m_s, _cir_rate_Hz,
				_transmitter_frequency_Hz, _link_names), component_types(
				_component_types) {

	// write CDX file type to HDF5 file:
	write("/parameters/delay_type", "continuous-delay");

	// check that their are the same number of link names as component types:
	if (link_names.size() != component_types.size()) {
		stringstream msg;
		msg << "Number of provided component types (" << component_types.size()
				<< ") does not match number of links in file (" << nof_links
				<< ").";
		throw runtime_error(msg.str());
	}

	// creating groups for links and cirs:
	for (size_t k = 0; k < nof_links; k++) {
		H5::Group *new_cir_group = new H5::Group(
				link_groups[link_names.at(k)]->createGroup("cirs"));
		group_cirs[link_names.at(k)] = new_cir_group;

		// write component types to file for each link:
		write(link_groups[link_names.at(k)], "component_types",
				component_types.at(k));
	}
}

void WriteContinuousDelayFile::write_cir(std::map<std::string, CIR> cirs,
		std::map<std::string, double> reference_delays, CIR_number_t CIRnum) {

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

	for (std::map<std::string, CIR>::iterator it = cirs.begin();
			it != cirs.end(); ++it) {
		const string link_name = it->first;

		// write reference delay:
		append_reference_delay(link_groups[link_name],
				reference_delays[link_name]);

		// write CIR:
		vector<CDX::impulse_t> impulses = it->second;

		stringstream dsName;
		dsName << CIRnum;

		H5::CompType cp_cmplx(sizeof(hdf5_impulse_t));

		cp_cmplx.insertMember("type", HOFFSET(hdf5_impulse_t, type),
				H5::PredType::NATIVE_INT16);
		cp_cmplx.insertMember("delays", HOFFSET(hdf5_impulse_t, delays),
				H5::PredType::NATIVE_DOUBLE);
		cp_cmplx.insertMember("real", HOFFSET(hdf5_impulse_t, real),
				H5::PredType::NATIVE_DOUBLE);
		cp_cmplx.insertMember("imag", HOFFSET(hdf5_impulse_t, imag),
				H5::PredType::NATIVE_DOUBLE);

		hdf5_impulse_t wdata[impulses.size()];

		const size_t RANK = 1;
		hsize_t dimsf3[RANK]; // dataset dimensions
		dimsf3[0] = impulses.size();
		H5::DataSpace dspace3(RANK, dimsf3);

		H5::DataSet dset3 = group_cirs[link_name]->createDataSet(
				dsName.str().c_str(), cp_cmplx, dspace3);

		for (size_t i = 0; i < impulses.size(); i++) {
			wdata[i].type = impulses.at(i).type;
			wdata[i].delays = impulses.at(i).delay;
			wdata[i].real = impulses.at(i).amplitude.real();
			wdata[i].imag = impulses.at(i).amplitude.imag();
		}
		dset3.write(wdata, cp_cmplx);
	}
}

WriteContinuousDelayFile::~WriteContinuousDelayFile() {
	// close cirs groups which were opened in constructor
	for (size_t k = 0; k < link_names.size(); k++) {
		delete group_cirs[link_names.at(k)];
	}
}

} // end of namespace CDX
