/**
 * \file	WriteFile.h
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

#ifndef WRITECDXFILE_H_
#define WRITECDXFILE_H_

#include "File.h"

namespace CDX {

/**
 * \brief Base class for writing continuous-delay and discrete-delay CDX files.
 */
class WriteFile: public File {
public:
	WriteFile(std::string _file_name, double _c0_m_s, double _cir_rate_Hz,
			double _transmitter_frequency, std::vector<std::string> _link_names);

	virtual ~WriteFile();

	/** creates group at path */
	void create_group(std::string path);

	/**
	 * \brief Writes string data to path.
	 */
	void write(std::string path, std::string data);

	/**
	 * \brief Writes double value to path.
	 */
	void write(std::string path, double data);

	/**
	 * \brief Writes double array to path.
	 */
	void write(std::string path, double *data, size_t num);

	/**
	 * \brief Writes vector of doubles to path.
	 */
	void write(std::string path, const std::vector<double> &data);

	/**
	 * \brief Writes vector of strings to path.
	 */
	void write(const H5::CommonFG *h5file, const std::string &path,
			const std::vector<std::string> &data);

	/**
	 * \brief Writes double value to path into group.
	 */
	void write(H5::Group *group, std::string path,
			const std::vector<double> &data);

	/**
	 * \brief Writes vector of vector of doubles to path.
	 */
	void write(std::string path, const std::vector<std::vector<double> > &data);

protected:
	/**
	 * \brief Creates 1D dataset for reference delays in file.
	 * Initial size is zero and dataset has unlimited dimension.
	 */
	void create_reference_delays_dataset(H5::Group *group);

	/**
	 * \brief Append single value to reference delay dataset.
	 */
	void append_reference_delay(H5::Group *group, double reference_delay);
};

} // end of namespace CDX

#endif /* WRITECDXFILE_H_ */
