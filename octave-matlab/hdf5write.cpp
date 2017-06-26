/**
 * \file hdf5write.cpp
 *
 * \date Apr 17, 2017
 * \author Frank Schubert
 *
 * \brief hdf5write() implementation for Octave.
 *
 * Writes arrays to a HDF5 file.
 *
 * Compilation for Octave:
 *
 *   clear hdf5write; mkoctfile hdf5write.cpp -lhdf5 -lhdf5_cpp -std=c++11
 *
 * Test:
 *
 *   test_variable = [ 1, 2, 3 ];
 *   hdf5write('test-file.h5', '/test-group/test-dataset', test_variable)
 *
 */

#include <string>
#include <vector>

#include <octave/oct.h>

#include "H5Cpp.h"

using namespace std;

DEFUN_DLD (hdf5write, args, nargout,
		"hdf5write() implementation for Octave. Writes data to an HDF5 file.")
{
	int nargin = args.length ();

	if (nargin != 3 and nargin != 5) {
		error("hdf5write: either 3 or 5 arguments are required.");
		std::cout << "LALALA\n";
	}

	const std::string file_name = args(0).string_value();
	std::string dataset_name = args(1).string_value();
	const Matrix matrix = args(2).matrix_value();

	unsigned int flags;

	// either 3 or 5 arguments are required:
	if (nargin == 3) {
		// overwrite file by default:
		flags = H5F_ACC_TRUNC;
	} else if (nargin == 5) {
		// if 'WriteMode' is 'append, open file for appending:
		const std::string parameter_name = args(3).string_value();
		const std::string parameter_value = args(4).string_value();

		if (parameter_name == "WriteMode") {
			if (parameter_value == "append") {
				flags = H5F_ACC_RDWR;
			} else {
				error("hdf5write: Only 'append' as parameter value is supported.");
			}
		} else {
			error("hdf5write: Only 'WriteMode' as parameter is supported.");
		}
	}

	// disable HDF5 warnings, we're handling exceptions:
	H5::Exception::dontPrint();

	try {
		H5::H5File h5file(file_name, flags);

		// open root group:
		H5::Group h5_group = h5file.openGroup("/");

		// check if all groups in the path exist:
		// dataset_name is e.g. "/test/lala"
		// is a slash in the string?:
		while (dataset_name.find("/") != string::npos) {
			// if first character is a slash, remove it:
			if (dataset_name[0] == '/') {
				dataset_name = dataset_name.substr(1, dataset_name.size());
			}

			// get first group name:
			const auto slash_pos = dataset_name.find("/");
			string group_name = dataset_name.substr(0, slash_pos);
			dataset_name = dataset_name.substr(slash_pos + 1, dataset_name.size());
			// cout << "group_name: " << group_name << ", dataset_name: " << dataset_name << endl;
			// check if group exists:
			try {
				h5_group = h5_group.openGroup(group_name);
			} catch(H5::Exception& e) {
				// group does not exists, create it:
				// cout << "group does not exist, creating it" << endl;
				h5_group = h5_group.createGroup(group_name);
			}
		}

		// dataset dimensions: write vector as row vector:
		const int rank = 2;
		hsize_t dimsf3[rank];
		dimsf3[0] = matrix.columns();
		dimsf3[1] = matrix.rows();
		H5::DataSpace dspace3(rank, dimsf3);

		H5::DataSet dset3 = h5_group.createDataSet(dataset_name.c_str(),
				H5::PredType::NATIVE_DOUBLE, dspace3);
		dset3.write(matrix.data(), H5::PredType::NATIVE_DOUBLE);
	} catch (H5::FileIException &e) {
		std::stringstream ss;
		ss << "hdf5write: H5::FileIException, file name: " << file_name;
		error(ss.str().c_str());
	}

	return octave_value_list ();
}
