%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Class CDXReadFile
%
% Reads CIRs from Commond Channel Data Exchange (CDX) files
%
% Author: F. Schubert
% Date: 08-09-2010
%
% Updated: Ioana Gulie
% Date: 21-07-2016
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

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
            % nof groups in file:
            info_struct = h5info(obj.filename); 
            groups = info_struct.Groups;
            nof_groups = size(groups, 1);
            fprintf('nof_groups is  %i \n', nof_groups);
            for k = 1:nof_groups
                group_name = groups(k).Name;
                if strcmp(group_name(1:6), '/links') 
                    info_links = h5info(obj.filename, group_name);
                    links = info_links.Groups;
                    % nof subgroups in the group /links:
                    nof_links = size(links, 1);
                    obj.nof_links = obj.nof_links + nof_links;
                    % get the names of the links
                    if obj.nof_links >= 1
                        obj.links_name= {links(1).Name};
                        if obj.nof_links > 1
                            for h = 2:nof_links
                                obj.links_name = [obj.links_name, links(h).Name];
                            end
                        end
                    end
                end
                if strcmp(group_name(1:6), '/visua') 
                    obj.frame_rate_Hz=hdf5read(obj.filename, '/visualization/frame_rate_Hz');
                end
            end
            assert(obj.nof_links > 0, 'number_of_links must be greater then 0!');
            fprintf('file contains %i link(s)\n', obj.nof_links);
            
            % open file for hdf5 low level access
            obj.file_id = H5F.open(obj.filename, 'H5F_ACC_RDONLY', 'H5P_DEFAULT');
            
            % read delay_type string:
            param_group_id = H5G.open(obj.file_id, '/parameters') ;
            obj.delay_type = read_string(obj, param_group_id, 'delay_type');
            H5G.close(param_group_id);
            
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
                obj.link_group_ids(k) = H5G.open(obj.file_id, obj.links_name{k}); % TODO fix the name
                % only for continuous-delay:
                if strcmp(obj.delay_type, 'continuous-delay')
                    obj.cir_group_ids(k) = H5G.open(obj.file_id, [obj.links_name{k}, '/cirs']);
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
            H5T.insert (obj.memtype, 'delays', 8, 'H5T_NATIVE_DOUBLE');
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

        % Delete methods are always called before a object
        % of the class is destroyed
        function delete(obj)
            H5T.close (obj.memtype);
            %             H5G.close(obj.group_id);
            % only for continuous-delay:
            if strcmp(obj.delay_type, 'continuous-delay')
                for k = 1:obj.nof_links
                    H5G.close(obj.cir_group_ids(k));
                end
            end
            H5F.close (obj.file_id);
        end
    end  % methods
end % class
