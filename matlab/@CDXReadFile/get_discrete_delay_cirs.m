%> \addtogroup matlab_implementation
%> @{
%>
%> \file get_discrete_delay_cirs.m
%>
%> \date Sep 08, 2010
%> \author Frank M. Schubert

%> \brief This function reads a cir with discrete delay data to a CDX file
%> Based on http://www.hdfgroup.org/ftp/HDF5/examples/examples-by-api/matlab/HDF5_M_Examples/h5ex_t_string.m
function cir = get_discrete_delay_cirs(obj, link_num, cir_s, cir_e)

dset = H5D.open(obj.link_group_ids(link_num), 'cirs_real');

%% Real data
% Get dataspace
space = H5D.get_space(dset);
[~, dims,~]= H5S.get_simple_extent_dims (space);
dims = fliplr(dims);

% Read the data.
rdata.real = H5D.read(dset, 'H5T_NATIVE_DOUBLE', 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');
H5D.close (dset);
H5S.close (space);

%% Imag data
dset = H5D.open(obj.link_group_ids(link_num), 'cirs_imag');

% Get dataspace
space = H5D.get_space(dset);
[~, dims,~]= H5S.get_simple_extent_dims (space);
dims = fliplr(dims);

% Read the data.
rdata.imag = H5D.read(dset, 'H5T_NATIVE_DOUBLE', 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');
H5D.close (dset);
H5S.close (space);

cir.samples = rdata.real + 1i .* rdata.imag;

% read reference delay:
dset = H5D.open(obj.link_group_ids(link_num), 'reference_delays');

% Get dataspace
space = H5D.get_space(dset);
[~, dims,~]= H5S.get_simple_extent_dims(space);
dims = fliplr(dims);

% Read the data.
cir.reference_delays = H5D.read(dset, 'H5T_NATIVE_DOUBLE', 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');

H5D.close (dset);
H5S.close (space);

cir.samples = cir.samples(cir_s:cir_e, :);
cir.reference_delays = cir.reference_delays(cir_s:cir_e);

end

%> @}
