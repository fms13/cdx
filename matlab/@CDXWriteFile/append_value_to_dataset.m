%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function append_value_to_dataset
%
% This function appends a single value to a HDF5 dataset
%
% Author: F. Schubert
% Date: 08-09-2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function append_value_to_dataset(obj, group_id, DATASET, value)

dset = H5D.open(group_id, DATASET);

% get dimensions
space = H5D.get_space (dset);
[ndims dims]  = H5S.get_simple_extent_dims (space);
dims = fliplr(dims');

% disp(sprintf('dimensions in file: %i\n', dims));
extdims = 1 + dims;

% Extend the dataset.
H5D.extend(dset,fliplr(extdims));

% Retrieve the dataspace for the newly extended dataset.
space = H5D.get_space(dset);

% The selection now contains only the newly extended
% portions of the dataset.
start = extdims - 1;
count = 1;
H5S.select_hyperslab(space, 'H5S_SELECT_SET', fliplr(start), [], fliplr(count), []);

% Write the data to the selected portion of the dataset.
mem_space = H5S.create_simple(1, fliplr(1), fliplr(1));
H5D.write(dset, 'H5T_NATIVE_DOUBLE', mem_space, space, 'H5P_DEFAULT', value);

H5D.close(dset);
H5S.close(space);

end