/**
 * \file	ReadFile.cpp
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
