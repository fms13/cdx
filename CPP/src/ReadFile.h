/**
 * \file	ReadFile.h
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

#ifndef READCDXFILE_H_
#define READCDXFILE_H_

#include "File.h"

namespace CDX {

/**
 * \brief	Base class for reading CDX files.
 */
class ReadFile: public File {
public:
	ReadFile(std::string _file_name);
	virtual ~ReadFile();

protected:
	/** return reference delays for a specific link */
	std::vector<double> get_reference_delays(std::string link);

	std::map<std::string, std::vector<double> > ref_delays;
};

} // end of namespace CDX

#endif /* READCDXFILE_H_ */
