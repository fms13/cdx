%> \addtogroup matlab_implementation
%> @{
%>
%> \file append_continuous_delay_cir.m
%>
%> \date Sep 08, 2010
%> \author Frank M. Schubert

%> \brief This function appends a cir with continuous delay data to a CDX file.
function append_continuous_delay_cir(obj, link_cir)
    assert(size(link_cir, 2) == obj.nof_links, 'Number of links in cir differs from number of links in HDF5 CDX file.');
    assert(numel(link_cir.reference_delay) == 1, 'Only one reference_delay value per CIR allowed.');

    % consistency check:
    for i = 1:obj.nof_links
        % vectors must have same lengths:
        assert(numel(link_cir(i).delays) == numel(link_cir(i).weights), 'Delays and weights vectors do not have the same lengths.');
        % reference delay must be one value:
        assert(numel(link_cir(i).reference_delay) == 1, 'Number of reference delays must equal 1.');
    end

    % number of the new cir:
    new_cir_num = obj.nof_cirs;

    % writing cir
    for i = 1:obj.nof_links
        cir.type = link_cir(i).type;
        cir.delays = link_cir(i).delays;
        cir.real = real(link_cir(i).weights);
        cir.imag = imag(link_cir(i).weights);

        assert(numel(cir.delays) == numel(cir.real), 'Delays and weights vectors must have same length.');
        assert(numel(cir.delays) == numel(cir.imag), 'Delays and weights vectors must have same length.');

        dims = numel(cir.delays);

        space = H5S.create_simple (1,fliplr(dims), []);
        if obj.compression_enabled
            % activate compression:
            dcpl = H5P.create('H5P_DATASET_CREATE');
            H5P.set_deflate(dcpl, 9);
            H5P.set_chunk(dcpl, fliplr([ 8 ])); % chunksize = 8
        else
            dcpl = 'H5P_DEFAULT';
        end

        dset = H5D.create(obj.cir_group_ids(i), sprintf('%i', new_cir_num), obj.filetype, space, dcpl);

        H5D.write (dset, obj.memtype, 'H5S_ALL', 'H5S_ALL', 'H5P_DEFAULT', cir);

        % Close and release resources.
        H5D.close (dset);
        H5S.close (space);

        append_value_to_dataset(obj, obj.link_group_ids(i), 'reference_delays', link_cir(i).reference_delay);
    end
    obj.nof_cirs = obj.nof_cirs + 1;
end
%> @}
