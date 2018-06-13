/**
 * \file	WriteFile.h
 *
 * \author	Frank M. Schubert
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
	void write(const H5::H5Location *h5file, const std::string &path,
			const std::vector<std::string> &data);

	/**
	 * \brief Writes a std::map<uint16_t, std::string> to path.
	 */
	void write(const H5::H5Location *h5file, const std::string &path,
			const std::map<uint16_t, std::string> &data);

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
