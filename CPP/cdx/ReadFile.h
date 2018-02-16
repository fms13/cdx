/**
 * \file	ReadFile.h
 * \author	Frank M. Schubert
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
	double get_reference_delay(std::string link, size_t number);

	/** return reference delays for a specific link */
	std::vector<double> get_reference_delays(std::string link);
};

} // end of namespace CDX

#endif /* READCDXFILE_H_ */
