%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function read string
%
% This function reads a string from a HDF5 dataset
% 
% based on http://www.hdfgroup.org/ftp/HDF5/examples/examples-by-api/matlab/HDF5_M_Examples/h5ex_t_string.m
%
% Author: F. Schubert
% Date: 08-09-2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function value = read_string(obj, group_id, DATASET)

dset = H5D.open(group_id, DATASET);

% Get the datatype and its size.
filetype = H5D.get_type (dset);
sdim = H5T.get_size (filetype);
sdim = sdim+1;                   % Make room for null terminator %

% Get dataspace
space = H5D.get_space (dset);
[~, dims, ~] = H5S.get_simple_extent_dims (space);
dims=fliplr(dims);

% Create the memory datatype.
memtype = H5T.copy ('H5T_C_S1');
H5T.set_size (memtype, sdim);

% Read the data.
value = H5D.read (dset, memtype, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');
value = value(1:end-1)'; % get rid of trailing \0

% Close and release resources.
H5D.close (dset);
H5S.close (space);
H5T.close (filetype);
H5T.close (memtype);

end