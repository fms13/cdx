%> \addtogroup matlab_implementation
%> @{
%>
%> \file CDXReadFile.m
%>
%> \date Sep 08, 2010
%> \author Frank M. Schubert
%>
%> \date Jul 21, 2016
%> \author Ioana Gulie

%> \brief Class for reading the channel response from Commond Channel Data Exchange (CDX) files.
classdef CDXReadFile < handle
    % Property data is private to the class
    properties (SetAccess = private, GetAccess = private)
        filename;
        file_id;
        nof_links;
        cir_group_ids = H5ML.id;
        link_group_ids = H5ML.id;
        nof_cirs;
        memtype; % HDF5 memory type for continuous data type
        memtype_discr_delay; % HDF5 memory type for discrete delay samples
        %         filetype
        %         filetype_discr_delay
        cir_rate;
        c0;
        transmitter_frequency;
        delay_type;
        delay_smpl_freq;
        nof_delay_samples;
        frame_rate_Hz;
        links_name;
    end % properties

    methods
        % Construct an object and
        % save the file ID
        function obj = CDXReadFile(filename)
            obj.filename = filename;
            fprintf('open file %s for reading\n', obj.filename);

            obj.cir_rate = hdf5read(obj.filename, '/parameters/cir_rate_Hz');
            obj.c0 = hdf5read(obj.filename, '/parameters/c0_m_s');
            obj.transmitter_frequency = hdf5read(obj.filename, '/parameters/transmitter_frequency_Hz');

            %initialize the frame rate
            obj.frame_rate_Hz=0;

            % get number of links
            obj.nof_links = 0;
            fprintf('hai ca mai e un pic');

            % nof groups in file:
            % info_struct = h5info(obj.filename);
           % fprintf('inca un pic');

            idx_type = 'H5_INDEX_NAME';
            order = 'H5_ITER_DEC';
            lapl_id = 'H5P_DEFAULT';

           % open file for hdf5 low level access
            obj.file_id = H5F.open(obj.filename, 'H5F_ACC_RDONLY', lapl_id);

            % read delay_type string:
            param_group_id = H5G.open(obj.file_id, '/parameters') ;
            obj.delay_type = read_string(obj, param_group_id, 'delay_type');
            H5G.close(param_group_id);

           % find how many groups are in the file
           % if nof_groups = 3: visualization is present
            info_struct = H5G.get_info(obj.file_id);
            nof_groups = info_struct.nlinks;
            fprintf('nof_groups is  %i \n', nof_groups);

            if nof_groups == 3
                obj.frame_rate_Hz=hdf5read(obj.filename, '/visualization/frame_rate_Hz');
            end

            % read and store the names of the links
            links_id = H5G.open(obj.file_id, '/links');
            info_links = H5G.get_info(links_id);
            % nof subgroups in the group /links:
            obj.nof_links = info_links.nlinks;
            H5G.close(links_id);

            % get the names of each link
            if obj.nof_links >= 1
                obj.links_name= {H5L.get_name_by_idx(obj.file_id,'/links',idx_type,order,0,lapl_id)};
                if obj.nof_links > 1
                    for h = 2:obj.nof_links
                        obj.links_name = [obj.links_name, H5L.get_name_by_idx(obj.file_id,'/links',idx_type,order,h-1,lapl_id)];
                    end
                end
            end

            assert(obj.nof_links > 0, 'number_of_links must be greater then 0!');
            fprintf('file contains %i link(s)\n', obj.nof_links)

            if strcmp(obj.delay_type, 'continuous-delay')
                fprintf('data type is %s\n', obj.delay_type);
            elseif strcmp(obj.delay_type, 'discrete-delay')
                %TODO This loop needs to be updated
                obj.delay_smpl_freq = hdf5read(obj.filename, '/parameters/delay_smpl_freq');

                % read number of delay samples. TODO: check that this is
                % the same number for all links
                obj.nof_delay_samples = size(hdf5read(obj.filename, '/link0/cirs_real'), 2);

                fprintf('data type is %s, delay_smpl_freq is %f\n', obj.delay_type, obj.delay_smpl_freq);
            else
                fprintf('data type in CDX file: %s (length: %i)\n', obj.delay_type, length(obj.delay_type));
                error('unknown CDX data type');
            end

            % open all groups:
            nof_cirs_per_link = zeros(obj.nof_links, 1);
          %  test = H5G.get_info(obj.file_id, '/link')
           % print test
            for k = 1:obj.nof_links
                obj.link_group_ids(k) = H5G.open(obj.file_id, ['links/', obj.links_name{k}]); % TODO fix the name
                % only for continuous-delay:
                if strcmp(obj.delay_type, 'continuous-delay')
                    obj.cir_group_ids(k) = H5G.open(obj.file_id, ['links/', obj.links_name{k}, '/cirs']);
                    nof_cirs_per_link(k) = H5G.get_num_objs(obj.cir_group_ids(k));
                else
                    % discrete delay:
                    dset = H5D.open(obj.link_group_ids(k), 'cirs_real');

                    % Get dataspace
                    space = H5D.get_space(dset);
                    [~, dims,~]= H5S.get_simple_extent_dims(space);
                    dims = fliplr(dims);

                    H5D.close (dset);
                    H5S.close (space);

                    nof_cirs_per_link(k) = dims(1);
                end
            end

            obj.nof_cirs = nof_cirs_per_link(1);
            % check if every link has the same number of cirs:
            for k = 1:obj.nof_links
                assert(obj.nof_cirs == nof_cirs_per_link(k), 'HDF5 CDX file is inconsistent: links do not have the same number of cirs.');
            end

            fprintf('file contains %i cir(s)\n', obj.nof_cirs);

            % define memory type and filetype for continuous delay data
            % type:
            obj.memtype = H5T.create ('H5T_COMPOUND', 4 * 8);
            H5T.insert (obj.memtype, 'type', 0, 'H5T_NATIVE_DOUBLE');
            H5T.insert (obj.memtype, 'delay', 8, 'H5T_NATIVE_DOUBLE');
            H5T.insert (obj.memtype, 'real', 8 + 8,'H5T_NATIVE_DOUBLE');
            H5T.insert (obj.memtype, 'imag', 16 + 8, 'H5T_NATIVE_DOUBLE');

            % define memory type and filetype for discrete delay data
            % type:
            obj.memtype_discr_delay = H5T.create('H5T_COMPOUND', 2 * 8);
            H5T.insert(obj.memtype_discr_delay, 'real', 0,'H5T_NATIVE_DOUBLE');
            H5T.insert(obj.memtype_discr_delay, 'imag', 8, 'H5T_NATIVE_DOUBLE');
        end

        function r = get_file_name(obj)
            r = obj.filename;
        end

        function r = get_c0(obj)
            r = obj.c0;
        end

        function r = get_transmitter_frequency(obj)
            r = obj.transmitter_frequency;
        end

        function r = get_frame_rate(obj)
            r = obj.frame_rate_Hz;
        end

        function r = get_number_of_links(obj)
            r = obj.nof_links;
        end

        function r = get_cir_rate(obj)
            r = obj.cir_rate;
        end

        function r = get_number_of_cirs(obj)
            r = obj.nof_cirs;
        end

        function r = get_number_of_delay_samples(obj)
            r = obj.nof_delay_samples;
        end

        function r = get_delay_smpl_freq(obj)
            r = obj.delay_smpl_freq;
        end

        function r = get_links_name(obj)
            r = obj.links_name;
        end

        function cir = get_cir(obj, link_num, cir_num)
            assert(cir_num <= obj.nof_cirs, sprintf('cir_num (%i) must be smaller than nof_cirs (%i)', cir_num, obj.nof_cirs));
            if strcmp(obj.delay_type, 'continuous-delay')
                cir = get_continuous_delay_cir(obj, link_num, cir_num);
            elseif strcmp(obj.delay_type, 'discrete-delay')
                cir = get_discrete_delay_cirs(obj, link_num, cir_num);
            else
                error('unknown CDX data type');
            end
        end

        %> \brief Desctructor
        %>
        %> Delete methods are always called before an object is destroyed.
        function delete(obj)
            H5T.close (obj.memtype);
            % only for continuous-delay:
            if strcmp(obj.delay_type, 'continuous-delay')
                for k = 1:obj.nof_links
                    H5G.close(obj.cir_group_ids(k));
                end
            end
            H5F.close(obj.file_id);
        end
    end  % methods
end % class

%> @}
