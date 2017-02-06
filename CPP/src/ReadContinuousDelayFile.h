/**
 * \file	ReadContinuousDelayFile.h
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

#ifndef SNREADCIRFILE_H_
#define SNREADCIRFILE_H_

#include <boost/shared_ptr.hpp>

#include "ReadFile.h"

namespace CDX {

/**
 * \brief	Reads CIRs from CDX file.
 *
 * \param	filename
 */
class ReadContinuousDelayFile: public ReadFile {
public:
	typedef boost::shared_ptr<ReadContinuousDelayFile> ptr;

	ReadContinuousDelayFile(std::string _filename);
	virtual ~ReadContinuousDelayFile();

	/**
	 * \brief	returns CIR with a given number
	 *
	 * \param	<cir_num> CIR number
	 *
	 * \return	CIR
	 */
	cir_t get_cir(std::string link, unsigned int cir_num);

	/**
	 * \brief	Return the number of CIRs in file
	 * \return	CIR amount
	 */
	unsigned int get_nof_cirs() const {
		return nof_cirs;
	}

protected:
	unsigned int nof_cirs;
	std::map<std::string, H5::Group *> cir_groups;

	// for function get_cir:
	H5::CompType *cp_echo;
};

} // end of namespace CDX

#endif /* SNREADCIRFILE_H_ */
