/**
 * \file	WriteContinuousDelayFile.h
 * \author	Frank M. Schubert
 */

#ifndef WriteContinuousDelayCDXFile_H_
#define WriteContinuousDelayCDXFile_H_

#include "WriteFile.h"

namespace CDX {

typedef uint64_t cir_number_t;

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
			links_to_component_types_t &_component_types);

	virtual ~WriteContinuousDelayFile();

	/**
	 * \brief Write single CIR to file
	 */
	void write_cir(std::map<std::string, components_t> cirs,
			std::map<std::string, double> reference_delays,
			cir_number_t cir_number);

private:
	std::map<std::string, H5::Group *> group_cirs; ///< pointers to cir datasets in file
	links_to_component_types_t component_types; ///< holds the component's types for each link, link_name->component_types

	H5::CompType *cp_cmplx;
};

} // end of namespace CDX

#endif /* WriteContinuousDelayCDXFile_H_ */
