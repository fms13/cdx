/**
 * \file	WriteContinuousDelayFile.h
 * \author	Frank M. Schubert
 */

#ifndef WriteContinuousDelayCDXFile_H_
#define WriteContinuousDelayCDXFile_H_

#include "WriteFile.h"

namespace CDX {

typedef uint64_t CIR_number_t;

/**
 * \brief Class for writing continuous-delay CDX files.
 */
class WriteContinuousDelayFile: public WriteFile {
public:
	/**
	 * \brief Constructor.
	 *
	 * \param _component_types A map of component types for each link
	 */
	WriteContinuousDelayFile(std::string _file_name, double _c0_m_s,
			double _cir_rate_Hz, double _transmitter_frequency_Hz,
			const std::vector<std::string> &_link_names,
			const std::vector<component_types_t> &_component_types);

	virtual ~WriteContinuousDelayFile();

	/**
	 * \brief Write single CIR to file
	 */
	void write_cir(std::map<std::string, CIR> cirs,
			std::map<std::string, double> reference_delays,
			CIR_number_t CIRnum);

private:
	std::map<std::string, H5::Group *> group_cirs; ///< pointers to cir datasets in file
	std::vector<component_types_t> component_types; ///< holds the component's types for each link

	H5::CompType *cp_cmplx;
};

} // end of namespace CDX

#endif /* WriteContinuousDelayCDXFile_H_ */
