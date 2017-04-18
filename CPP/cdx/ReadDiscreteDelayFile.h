/**
 * \file	ReadDiscreteDelayFile.h
 *
 * \author	Frank M. Schubert
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
