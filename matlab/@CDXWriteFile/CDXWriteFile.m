%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Class CDXWriteFile
%
% Writes CIRs to Commond Channel Data Exchange (CDX) files
%
% Author: F. Schubert
% Date: 08-09-2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% opens always a new file.
% TODO: append data to an existing file.

classdef CDXWriteFile < handle
    % Property data is private to the class
    properties (SetAccess = private, GetAccess = private)
        filename
        file_id
        link_id
        nof_links
        cir_group_ids = H5ML.id;
        link_group_ids = H5ML.id;
        nof_cirs
        memtype % HDF5 memory type for continuous data type
        memtype_discr_delay; % HDF5 memory type for discrete delay samples
        filetype
        filetype_discr_delay
        cir_rate
        c0
        transmitter_frequency
        frame_rate;
        compression_enabled = 0;
        delay_type;
        delay_smpl_freq;
        nof_delay_samples;
        links_name;
    end % properties

    methods
        % Construct an object and
        % save the file ID
        function obj = CDXWriteFile(filename, parameters)

            if parameters.enable_compression
                % check if compression is available
                % from
                % http://www.hdfgroup.uiuc.edu/UserSupport/examples-by-api/matlab/HDF5_M_Ex
                % amples/h5ex_d_gzip.m
                avail = H5Z.filter_avail('H5Z_FILTER_DEFLATE');
                if ~avail
                    error ('gzip filter not available.');
                end

                % check that it can be used.
                H5Z_FILTER_CONFIG_ENCODE_ENABLED = H5ML.get_constant_value('H5Z_FILTER_CONFIG_ENCODE_ENABLED');
                H5Z_FILTER_CONFIG_DECODE_ENABLED = H5ML.get_constant_value('H5Z_FILTER_CONFIG_DECODE_ENABLED');
                filter_info = H5Z.get_filter_info('H5Z_FILTER_DEFLATE');
                if ( ~bitand(filter_info,H5Z_FILTER_CONFIG_ENCODE_ENABLED) || ...
                        ~bitand(filter_info,H5Z_FILTER_CONFIG_DECODE_ENABLED) )
                    error ('gzip filter not available for encoding and decoding.');
                end
            end

            % type of CDX file:
            assert(strcmp(parameters.delay_type, 'continuous-delay') || strcmp(parameters.delay_type, 'discrete-delay'), ...
                'parameters.delay_type must either be continuous-delay or discrete-delay');
            obj.delay_type = parameters.delay_type;

            if strcmp(obj.delay_type, 'discrete-delay')
                assert(parameters.delay_smpl_freq > 0, ...
                'for parameters.delay_type == discrete-delay, parameters.delay_smpl_freq must be greater than zero.');
            obj.delay_smpl_freq = parameters.delay_smpl_freq;
            obj.nof_delay_samples = parameters.nof_delay_samples;
            end

            % c0 must be greater than 0:
            assert(parameters.c0 > 0, 'parameters.c0 must be greater than zero.');
            obj.c0 = parameters.c0;

            obj.transmitter_frequency = parameters.transmitter_frequency;

            % number of links must be greater than 0:
            assert(parameters.nof_links > 0, 'parameters.nof_links must be greater than zero.');
            obj.nof_links = parameters.nof_links;

            assert(parameters.cir_rate > 0, 'parameters.cir_rate must be greater than zero');
            obj.cir_rate = parameters.cir_rate;

            obj.filename = filename;
            fprintf('open file %s for writing\n', obj.filename);

            time = clock; % [year month day hour minute seconds]
            creation_time = sprintf('%02d-%02d-%02d %02d:%02d:%2.2f', time(1), time(2), time(3), time(4), time(5), time(6));

            hdf5write(obj.filename, '/parameters/creation_time', creation_time);
            hdf5write(obj.filename, '/parameters/number_of_links' , obj.nof_links, 'WriteMode', 'append');
            hdf5write(obj.filename, '/parameters/c0_m_s' , obj.c0, 'WriteMode', 'append');
            hdf5write(obj.filename, '/parameters/cir_rate_Hz' , obj.cir_rate, 'WriteMode', 'append');
            hdf5write(obj.filename, '/parameters/number_of_delay_samples' , obj.nof_delay_samples, 'WriteMode', 'append');
            hdf5write(obj.filename, '/parameters/delay_smpl_freq' , obj.delay_smpl_freq, 'WriteMode', 'append');
            hdf5write(obj.filename, '/parameters/transmitter_frequency_Hz' , obj.transmitter_frequency, 'WriteMode', 'append');

            % open file for HDF5 low level functions:
            obj.file_id = H5F.open (obj.filename, 'H5F_ACC_RDWR', 'H5P_DEFAULT');

            obj.frame_rate = parameters.frame_rate;

            %if visualization is enabled
            if(obj.frame_rate  > 10^(-10))
                hdf5write(obj.filename, '/visualization/frame_rate_Hz' , obj.frame_rate, 'WriteMode', 'append');
                vizualization_id = H5G.open(obj.file_id, '/visualization') ;
            end

            % write delay_type string:
            param_group_id = H5G.open(obj.file_id, '/parameters') ;
            write_string(obj, param_group_id, 'delay_type', obj.delay_type);
            H5G.close(param_group_id);
            fprintf('parameters.nof_links %i \n', parameters.nof_links);
            obj.link_id = H5G.create(obj.file_id, '/links', 16) ;
            obj.links_name = parameters.links_name;
            % create groups for the links:
            for i = 0:parameters.nof_links-1
                % create group for link
                obj.link_group_ids(i+1) = H5G.create(obj.file_id, ['/links/', obj.links_name{i+1}] , 16) ;
                 %if visualization is enabled
                if(obj.frame_rate  > 10^(-10))
                    vizualization_link_id(i+1) = H5G.create(obj.file_id, sprintf('/visualization/link%i', i), 16) ;
                end
                % create group for cirs, only for continuous-delay:
                if strcmp(parameters.delay_type, 'continuous-delay')
                    obj.cir_group_ids(i+1) = H5G.create(obj.file_id, ['/links/', obj.links_name{i+1},'/cirs'], 16) ;

                    % Create dataspace with unlimited dimensions for reference_delays
                    dims = 0;
                    chunk = 1;
                    H5S_UNLIMITED = H5ML.get_constant_value('H5S_UNLIMITED');
                    maxdims = H5S_UNLIMITED;
                    space = H5S.create_simple(1, fliplr(dims), fliplr(maxdims));

                    % Create the dataset creation property list, and set the chunk
                    % size.
                    dcpl = H5P.create('H5P_DATASET_CREATE');
                    H5P.set_chunk (dcpl, fliplr(chunk));

                    % Create the unlimited dataset.
                    dset = H5D.create(obj.link_group_ids(i+1), 'reference_delays' ,'H5T_IEEE_F64BE',space,dcpl);

                    % Write the data to the dataset.
                    wdata = [];
                    H5D.write(dset,'H5T_NATIVE_DOUBLE','H5S_ALL','H5S_ALL','H5P_DEFAULT', wdata);

                    wdata1 = {'Geometrical LOS'; 'Diffracted LOS'; 'Multipath Component'};

                    % Add the component_types
                    hdf5write(obj.filename, ['/links/', obj.links_name{i+1}, '/component_types'], wdata1, 'WriteMode', 'append');
                end
            end

            %if visualization is enabled
            if(obj.frame_rate  > 10^(-10))
                for k = 1:obj.nof_links
                    H5G.close(vizualization_link_id(k));
                end
                H5G.close(vizualization_id);
            end

            obj.nof_cirs = 0;

            % define memory type and filetype for continuous delay data
            % type:
            obj.memtype = H5T.create('H5T_COMPOUND',  4 * 8);
            H5T.insert(obj.memtype, 'type', 0, 'H5T_NATIVE_DOUBLE');
            H5T.insert(obj.memtype, 'delay', 8, 'H5T_NATIVE_DOUBLE');
            H5T.insert(obj.memtype, 'real', 8 + 8,'H5T_NATIVE_DOUBLE');
            H5T.insert(obj.memtype, 'imag', 16 + 8, 'H5T_NATIVE_DOUBLE');

            obj.filetype = H5T.create('H5T_COMPOUND', 8 + 8 + 8 + 8);
            H5T.insert(obj.filetype, 'type', 0, 'H5T_NATIVE_DOUBLE');
            H5T.insert(obj.filetype, 'delay', 8, 'H5T_IEEE_F64BE');
            H5T.insert(obj.filetype, 'real', 8 + 8,'H5T_IEEE_F64BE');
            H5T.insert(obj.filetype, 'imag', 16 + 8,'H5T_IEEE_F64BE');

            % define memory type and filetype for discrete delay data
            % type:
            obj.memtype_discr_delay = H5T.create('H5T_COMPOUND', 2 * 8);
            H5T.insert(obj.memtype_discr_delay, 'real', 0,'H5T_NATIVE_DOUBLE');
            H5T.insert(obj.memtype_discr_delay, 'imag', 8, 'H5T_NATIVE_DOUBLE');

            obj.filetype_discr_delay = H5T.create('H5T_COMPOUND', 8 + 8);
            H5T.insert(obj.filetype_discr_delay, 'real', 0 ,'H5T_IEEE_F64BE');
            H5T.insert(obj.filetype_discr_delay, 'imag', 8,'H5T_IEEE_F64BE');
        end

        function r = get_file_name(obj)
            r = obj.filename;
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

        function r = get_delay_type(obj)
            r = obj.delay_type;
        end

        function append_cir(obj,cir)
            if strcmp(obj.delay_type, 'continuous-delay')
                append_continuous_delay_cir(obj,cir);
            elseif strcmp(obj.delay_type, 'discrete-delay')
                append_discrete_delay_cir(obj,cir);
            else
                error('unknown CDX data type');
            end
        end

        % Delete methods are always called before a object
        % of the class is destroyed
        function delete(obj)
            H5T.close (obj.memtype);
            for k = 1:obj.nof_links
                % only for continuous-delay:
                if strcmp(obj.delay_type, 'continuous-delay')
                    H5G.close(obj.cir_group_ids(k));
                end
                H5G.close(obj.link_group_ids(k));
            end
            H5G.close(obj.link_id);
            H5F.close(obj.file_id);
        end
    end  % methods
end % class
