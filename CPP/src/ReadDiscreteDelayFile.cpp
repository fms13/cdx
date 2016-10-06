/**
 * \file	ReadDiscreteDelayFile.h
 * \brief	Implementation of reading channel impulse responses from a HDF5 CDX file.
 *
 * \author Frank M. Schubert
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

#include "ReadDiscreteDelayFile.h"

using namespace std;

namespace CDX {

ReadDiscreteDelayFile::ReadDiscreteDelayFile(string _file_name) :
		ReadFile(_file_name) {
	// Try block to detect exceptions raised by any of the calls inside it

	// read simulation parameters
	H5::DataSet(h5file.openDataSet("/parameters/delay_smpl_freq_Hz")).read(
			&delay_smpl_freq, H5::PredType::NATIVE_DOUBLE);

}

vector<vector<complex<double> > > ReadDiscreteDelayFile::get_cirs(
		std::string link) {

	if (cir_groups.count(link) < 1) {
		throw "ReadDiscreteDelayCDXFile::get_cir: did not find link in file.";
	}

	vector<vector<complex<double> > > cirs;

	// read data
	H5::DataSet dataset_real = H5::DataSet(
			link_groups[link]->openDataSet("cirs_real"));
	H5::DataSet dataset_imag = H5::DataSet(
			link_groups[link]->openDataSet("cirs_imag"));

	H5::DataSpace dataspace_real = H5::DataSpace(dataset_real.getSpace());
	H5::DataSpace dataspace_imag = H5::DataSpace(dataset_imag.getSpace());

	int rank_real = dataspace_real.getSimpleExtentNdims();
//		int rank_imag = dataspace_imag.getSimpleExtentNdims();

	/*
	 * Get the dimension size of each dimension in the dataspace_real and
	 * display them.
	 */
	hsize_t dims_out_real[rank_real];
//		int ndims_real =
	dataspace_real.getSimpleExtentDims(dims_out_real, NULL);

	hsize_t dims_out_imag[rank_real];
//		int ndims_imag =
	dataspace_imag.getSimpleExtentDims(dims_out_imag, NULL);

	if (sdebug)
		cout << "rank " << rank_real << ", dimensions "
				<< (unsigned long) (dims_out_real[0]) << " x "
				<< (unsigned long) (dims_out_real[1]) << endl;

	if (dims_out_real[0] != dims_out_imag[0]) {
		throw("dimension 0 of cirs_real does not match dimension 0 of cirs_imag");
	}
	if (dims_out_real[1] != dims_out_imag[1]) {
		throw("dimension 1 of cirs_real does not match dimension 1 of cirs_imag");
	}

	const unsigned int nof_delay_smpls = dims_out_real[0];
	const unsigned int nof_cirs = dims_out_real[1];

	if (sdebug) {
		cout << "nof_delay_smpls: " << nof_delay_smpls << endl;
		cout << "nof_cirs: " << nof_cirs << endl;
		cout << "reading real data..." << endl;
	}
	double cirs_real[nof_delay_smpls][nof_cirs];
	/*
	 // Define the memory dataspace.
	 hsize_t dimsm[2];  memory space dimensions
	 dimsm[0] = nof_delay_smpls;
	 dimsm[1] = nof_cirs;
	 DataSpace memspace(2, dimsm);
	 dataset_real.read(cirs_real, H5::PredType::NATIVE_DOUBLE, memspace, dataspace_real);
	 */
	dataset_real.read(cirs_real, H5::PredType::NATIVE_DOUBLE);

	if (sdebug)
		cout << "reading imag data..." << endl;
	double cirs_imag[nof_delay_smpls][nof_cirs];
	dataset_imag.read(cirs_imag, H5::PredType::NATIVE_DOUBLE);

	cirs.resize(nof_cirs);
	for (size_t k = 0; k < cirs.size(); k++) {
		cirs.at(k).resize(nof_delay_smpls);
		for (size_t n = 0; n < cirs.at(k).size(); n++) {
			cirs.at(k).at(n) = complex<double>(cirs_real[n][k],
					cirs_imag[n][k]);
		}
	}

//		result_cir.ref_delay = ref_delays.at(link).at(cir_num);

	return cirs;
}

ReadDiscreteDelayFile::~ReadDiscreteDelayFile() {
}

} // end of namespace CDX
