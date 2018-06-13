%> \addtogroup matlab_implementation
%> @{
%>
%> \file write_string.m
%>
%> \date Sep 08, 2010
%> \author Frank M. Schubert

%> \brief This function appends a single value to a HDF5 dataset
%> Based on http://www.hdfgroup.org/ftp/HDF5/examples/examples-by-api/matlab/HDF5_M_Examples/h5ex_t_string.m
function write_string(obj, group_id, DATASET, value)

SDIM = length(value);
dims = 1;

% Create file and memory datatypes. MATLAB strings do not have \0's.
filetype = H5T.copy ('H5T_FORTRAN_S1');
H5T.set_size (filetype, SDIM);
memtype = H5T.copy ('H5T_C_S1');
H5T.set_size (memtype, SDIM);

% Create dataspace.  Setting maximum size to [] sets the maximum
% size to be the current size.
space = H5S.create_simple (1,fliplr(dims), []);

% Create the dataset and write the string data to it.
dset = H5D.create (group_id, DATASET, filetype, space, 'H5P_DEFAULT');
% Transpose the data to match the layout in the H5 file to match C
% generated H5 file.
H5D.write(dset, memtype, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', value');

H5D.close (dset);
H5S.close (space);
H5T.close (filetype);
H5T.close (memtype);

end
%> @}
