%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function get_discrete_delay_cir
%
% This function reads a cir with discrete delay data to a CDX file
% 
% based on http://www.hdfgroup.org/ftp/HDF5/examples/examples-by-api/matlab/HDF5_M_Examples/h5ex_t_string.m
%
% Author: F. Schubert
% Date: 08-09-2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function cir = get_discrete_delay_cir(obj, link_num, cir_num)
dset = H5D.open(obj.cir_group_ids(link_num), sprintf('%i', cir_num-1));
rdata = H5D.read(dset, obj.memtype, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT');
H5D.close (dset);

cir.samples = rdata.real + 1i .* rdata.imag;

% read reference delay:
dset = H5D.open(obj.link_group_ids(link_num), 'reference_delays');
space = H5D.get_space(dset);
start = [ cir_num-1 0 ]';
count = [ 1 1 ];
H5S.select_hyperslab(space, 'H5S_SELECT_SET', fliplr(start), [], fliplr(count), []);
rdata = H5D.read(dset, 'H5T_NATIVE_DOUBLE', 'H5S_ALL', space, 'H5P_DEFAULT');
cir.reference_delay = rdata(cir_num-1+1);
end
