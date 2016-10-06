/**
 * \file	ReadDiscreteDelayFile.h
 *
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


#ifndef SNREADDISCRDELAYCIRFILE_H_
#define SNREADDISCRDELAYCIRFILE_H_

#include <string>

#include "ReadFile.h"

namespace CDX {

/**
 * \brief	Reads CIRs from CIRs from discrete-delay HDF5 file.
 *
 * \param	filename
 */
class ReadDiscreteDelayFile: public ReadFile {
public:
	ReadDiscreteDelayFile(std::string filename);
	virtual ~ReadDiscreteDelayFile();

	/**
	 * \brief	returns all CIRs
	 * \return	CIRs
	 */
	std::vector<std::vector<std::complex<double> > > get_cirs(std::string link);

	//	cir_struct get_cir(unsigned int link, unsigned int cir_num);

	/** returns sampling rate in delay direction */
	double get_delay_smpl_freq() {
		return delay_smpl_freq;
	}

protected:
	unsigned int nof_cirs;

	double delay_smpl_freq;
};

} // end of namespace CDX

#endif /* SNREADDISCRDELAYCIRFILE_H_ */
