%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function append_discrete_delay_cir
%
% This function appends a cir with discrete delay data to a CDX file
%
% Author: F. Schubert
% Date: 08-09-2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function write_discrete_delay_cirs(obj, link_cir)
assert(size(link_cir, 2) == obj.nof_links, 'Number of links in cir differs from number of links in HDF5 CDX file.');

% format: link_cir(cirs, delays)

% consistency check:
for i = 1:obj.nof_links
    cir.real = real(link_cir(i).samples);
    cir.imag = imag(link_cir(i).samples);
    
    % reference delay must be one value:
    assert(numel(link_cir(i).reference_delays) == size(cir.real, 1), 'Number of reference delays must equal number of cirs in link_cir.cirs.');
    
    assert(strcmp(obj.delay_type, 'discrete-delay'), 'can only be called on discrete-delay type files.');
end

% write data
for i = 1:obj.nof_links
    
    % writing all cirs:
    DATASET        = 'cirs';
    DIM0           = size(cir.real, 1);
    DIM1           = size(cir.real, 2);
    
    dims  = [DIM0 DIM1];
    
    % Create dataspace.  Setting maximum size to [] sets the maximum
    % size to be the current size.
    space = H5S.create_simple (2, fliplr(dims), []);
    
    % Create the dataset and write the array data to it.
    dset = H5D.create(obj.link_group_ids(i), 'cirs_real', 'H5T_IEEE_F64BE', space, 'H5P_DEFAULT');
    H5D.write (dset, 'H5T_NATIVE_DOUBLE', 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', cir.real);
    H5D.close (dset);
    
    dset = H5D.create(obj.link_group_ids(i), 'cirs_imag', 'H5T_IEEE_F64BE', space, 'H5P_DEFAULT');
    H5D.write (dset, 'H5T_NATIVE_DOUBLE', 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', cir.imag);
    H5D.close (dset);
    
    H5S.close (space);
    
    % write reference delays
    DATASET        = 'reference_delays';
    DIM0           = 1;
    DIM1           = numel(link_cir(i).reference_delays);
    
    dims  = [DIM0 DIM1];
    
    % Create dataspace.  Setting maximum size to [] sets the maximum
    % size to be the current size.
    space = H5S.create_simple (2, fliplr(dims), []);
    
    % Create the dataset and write the array data to it.
    dset = H5D.create(obj.link_group_ids(i), 'reference_delays', 'H5T_IEEE_F64BE', space, 'H5P_DEFAULT');
    H5D.write (dset, 'H5T_NATIVE_DOUBLE', 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', link_cir(i).reference_delays);
    H5D.close (dset);
   
    H5S.close (space);
    
    % for i = 1:obj.nof_links
    %
    %     space = H5S.create_simple (1,fliplr(dims), []);
    %     if obj.compression_enabled
    %         % activate compression:
    %         dcpl = H5P.create('H5P_DATASET_CREATE');
    %         H5P.set_deflate(dcpl, 9);
    %         H5P.set_chunk(dcpl, fliplr([ 8 ])); % chunksize = 8
    %     else
    %         dcpl = 'H5P_DEFAULT';
    %     end
    
    %     append_value_to_dataset(obj, obj.link_group_ids(i), 'reference_delays', link_cir(i).reference_delay);
    % end
end
end
