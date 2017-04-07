/**
 * \file	WriteDiscreteDelayFile.h
 * \author	Frank M. Schubert
 */

#ifndef WRITEDISCRETEDELAYCDXFILE_H_
#define WRITEDISCRETEDELAYCDXFILE_H_

#include "WriteFile.h"

namespace CDX {

/**
 * \brief Class for writing discrete-delay data to a CDX file
 */
class WriteDiscreteDelayFile: public WriteFile {
public:
	/**
	 * \param min_delay delay value of first delay bin
	 */
	WriteDiscreteDelayFile(std::string _file_name, double _c0_m_s,
			double _cir_rate_Hz, double _transmitter_frequency_Hz,
			const std::vector<std::string> &_link_names,
			double _delay_smpl_freq_Hz);

	virtual ~WriteDiscreteDelayFile();

	/**
	 * \b Configures a link.
	 *
	 * \param[in] link Number of the link.
	 * \param[in] number_of_delay_samples Number of samples in the delay dimension.
	 * \param[in] min_delay Minimal delay, this serves as the lower value on the vertical axis.
	 */
	void setup_link(std::string link_name, size_t number_of_delay_samples,
			double min_delay);

	/** This function appends a cir with discrete delay data to a CDX file*/
	void append_cir_snapshot(std::string link_name,
			const std::vector<std::complex<double> > &data, double ref_delay);

	void append_2d_dataset(H5::Group *group, std::string path, double *data,
			size_t length, size_t act_cir);

private:
	std::map<std::string, size_t> numbers_of_delay_samples;
	std::map<std::string, double> min_delays;
	double delay_smpl_freq_Hz;

	std::map<std::string, size_t> act_cirs;
};

} // end of namespace CDX

#endif /* WRITEDISCRETEDELAYCDXFILE_H_ */
