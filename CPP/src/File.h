/**
 * \file File.h
 *
 * \date Mar 18, 2015
 * \author Frank Schubert
 */

#ifndef CDX_FILE_H_
#define CDX_FILE_H_

#include <string>
#include <vector>
#include <complex>
#include <map>

#include "H5Cpp.h"

/**
 * \brief Contains all CDX classes and types.
 */
namespace CDX {

/**
 * \brief Assigns integer values to component type names.
 *
 * The integer values are written to continuous-delay CDX files. Plotting tools
 * can then process and display the different multipath component depending on
 * their types, e.g. "diffracted components" "components caused by vegetation", etc.
 */
typedef std::map<uint16_t, std::string> component_types_t;


/**
 * \brief Holds a single multipath component as input value for WriteContinuousDelayCDXFile::write_cir.
 */
struct impulse_t {
	uint16_t type; ///< the multipath component's type as defined in component_types_t
	uint64_t id; ///< unique identifier for each component
	double delay; ///< the multipath component's delay in s
	std::complex<double> amplitude; ///< the multipath component's complex amplitude
};

/**
 * \brief Defines a vector of CDX_Impulses representing one channel impulse response (CIR).
 */
typedef std::vector<impulse_t> CIR;

struct hdf5_impulse_t {
	uint16_t type;
	uint64_t id;
	double delay;
	double real;
	double imag;
};

/**
 * \brief Struct used as return value for function ReadContinuousDelayCDXFile::get_cir.
 */
struct cir_t {
	double ref_delay; ///< the reference delay in s, i.e. the time the signal travels from the transmitter to the receiver on a straight line
	std::vector<impulse_t> components; ///< the multipath components
};

/**
 * \brief Base class for the processing of Channel Data Exchange (CDX) files.
 *
 * The CDX format is designed to store radio channel responses in terms of
 * multipath components. Each component has a delay and a complex amplitude. The
 * channel response is time-variant, i.e. a channel impulse response is stored
 * for each time step.
 *
 * A CDX file can contain multiple links and store a channel reponse for each link.
 *
 * A channel response can be stored with continuous-delay or discrete-delay data.
 *
 * Continuous-delay data stores a vector of multipath components using the floating
 * point format for their delay. Radio channel models usually generate channel
 * responses in this format.
 *
 * A discrete-delay CDX file stores the channel response data on a fix delay-time
 * plane. It can be interpreted as measurements stemming from a channel sounding
 * measurement campaign.
 *
 * A continuous-delay CDX file can be converted to a discrete-delay CDX file with
 * the tool convert-continuous-to-discrete-cdx in the support_programs/ directory.
 *
 */
class File {
public:
	/**
	 * \brief Construction from a file name.
	 *
	 * \param[in] _file_name File name
	 */
	File(std::string _file_name);

	/**
	 * \brief Construction from file name and parameters.
	 *
	 * \param[in] _file_name File name
	 * \param[in] _c0_m_s Speed of light in m/s
	 * \param[in] _cir_rate_Hz CIR rate in Hz
	 * \param[in] _transmitter_frequency_Hz Transmitter Frequency in Hz
	 * \param[in] _link_names Vector of strings for the link names
	 */
	File(std::string _file_name, double _c0_m_s, double _cir_rate_Hz,
			double _transmitter_frequency_Hz,
			const std::vector<std::string> &_link_names);

	/**
	 * \brief Destructor.
	 */
	virtual ~File();

	/**
	 * \brief Reads a double from an HDF5 file.
	 *
	 * \param[in] file File name
	 * \param[in] dataset_name Dataset name
	 * \return value of Dataset member
	 */
	static double read_double_h5(H5::H5File file, std::string dataset_name);

	/**
	 * \brief Reads string of given path from an CDX file.
	 *
	 * \param[in] file File name
	 * \param[in] path Dataset name
	 * \return value of Dataset member
	 */
	static std::string read_string_h5(H5::H5File file, std::string path);

	/**
	 * \brief Returns the file handle to the HDF5 file.
	 *
	 * \return File handle
	 */
	H5::H5File get_file_handle() const {
		return h5file;
	}

	/**
	 * \brief Returns number of links.
	 *
	 * \return Number of links
	 */
	size_t get_nof_links() const {
		return nof_links;
	}

	/**
	 * \brief Return the CIR rate of the file.
	 *
	 * \return CIR rate in Hz
	 */
	double get_cir_rate_Hz() const {
		return cir_rate_Hz;
	}

	/**
	 * \brief Returns the speed of light in m/s as defined in the CDX file.
	 *
	 * \return Speed of light in Hz
	 */
	double get_c0_m_s() const {
		return c0_m_s;
	}

	/**
	 * \brief Returns the transmitter frequency in Hz.
	 *
	 * \return Transmitter Frequency in Hz
	 */
	double get_transmitter_frequency_Hz() const {
		return transmitter_frequency_Hz;
	}

	/**
	 * \brief Returns the link names.
	 *
	 * \return Vector of strings with the link names
	 */
	std::vector<std::string> get_link_names() const {
		return link_names;
	}

protected:
	const std::string file_name; ///< the CDX file's name
	H5::H5File h5file; ///< the handle to the HDF5 file

	double c0_m_s; ///< the speed of light in m/s
	double cir_rate_Hz; ///< the CIR update rate in Hz
	double transmitter_frequency_Hz; ///< the transmitter frequency in Hz
	std::string delay_type; ///< the delay type, either "continuous-delay" or "discrete-delay"

	std::vector<std::string> link_names; ///< vector of the link names
	H5::Group links_group; ///< handle to the group in the HDF5 file that stores the links
	const size_t nof_links; ///< the number of the links
	std::map<std::string, H5::Group *> link_groups; ///< vector of pointers to HDF5 groups which contain data for each link in file
};

} // end of namespace CDX

#endif /* CDX_FILE_H_ */
