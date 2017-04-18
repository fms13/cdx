/**
 * \file	ReadContinuousDelayFile.h
 *
 * \author	Frank M. Schubert
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
