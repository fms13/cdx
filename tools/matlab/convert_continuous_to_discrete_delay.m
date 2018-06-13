%> \addtogroup matlab_tools
%> @{
%> \addtogroup matlab_tools_convert_continuous_to_discrete_delay convert_continuous_to_discrete_delay
%> @{
%>
%> \file convert_continuous_to_discrete_delay.m
%>
%> \brief This script converts a continuous-delay CDX file to a discrete delay CDX file through interpolation of the data.
%>
%> \date Sep 08, 2010
%> \author Frank M. Schubert

% if waitbars are still open from a previous program interruption, call:
% set(0,'ShowHiddenHandles','on')
% delete(get(0,'Children'))

clear all
close all
clear classes
clc

addpath('../CDX_library_functions');

%% set parameters
in_file_name = '../Channel-Data-Examples/DLRLMS_urban_continuous-delay.h5';
out_file_name = 'DLRLMS_urban_discrete-delay.h5';

% interpolation parameters:
interp_bandwidth = 100e6; % Hz
interp_max_delay = 1.8e-6; % s
add_delay_offset = .1e-6; % s
enable_windowing = true;

% calculate remaining interpolation parameters:
delay_smpl_freq = 2 * interp_bandwidth; % Hz
Ts = 1 / delay_smpl_freq;
interp_nof_delay_samples = interp_max_delay * delay_smpl_freq;

%% open input file
cdx_in = CDXReadFile(in_file_name);

%% set parameters for output CDX file...
parameters.delay_type = 'discrete-delay';
parameters.delay_smpl_freq = delay_smpl_freq;
parameters.nof_delay_samples = interp_max_delay * delay_smpl_freq;
% ...partly from the input file:
parameters.c0 = get_c0(cdx_in);
parameters.cir_rate = get_cir_rate(cdx_in);
parameters.nof_links = get_number_of_links(cdx_in);
parameters.enable_compression = 0;

%% open output file
% create CDX file:
cdx_out = CDXWriteFile(out_file_name, parameters);

%% for all CIRs:
nof_cirs = get_number_of_cirs(cdx_in);
nof_links = get_number_of_links(cdx_in);

% delay vector
x = 0:Ts:interp_max_delay-Ts;

if enable_windowing
    window = fftshift(hanning(parameters.nof_delay_samples));
    window = window .* (size(window, 1) / sum(window));  % normalize
end

wb = waitbar(0, 'Converting...', 'CreateCancelBtn',...
    'setappdata(gcbf,''canceling'',1)');
for k = 1:nof_cirs
    if (mod(k, 100) == 0) waitbar(k/nof_cirs, wb, ...
            sprintf('%i CIRs of a total of %i CIRs processed...', k, nof_cirs)); end

    % for all links:
    for link = 1:nof_links
        cir = get_cir(cdx_in, link, k);

        actual_max_delay = max(cir.delays) + add_delay_offset;
        if (actual_max_delay > interp_max_delay)
           fprintf('WARNING: max. delay in file (%e, including add_delay_offset)\n is greater than interp_max_delay (%e). Not all multipath components are taken into account!\n', ...
               actual_max_delay, interp_max_delay);
        end

        % interpolate
        fac = repmat(x, numel(cir.delays), 1) - ...
            repmat(cir.delays + add_delay_offset, 1, interp_nof_delay_samples);
        result_cir(link).samples = cir.weights' * sinc(delay_smpl_freq * fac);
        if enable_windowing
            result_cir(link).samples = ifft(window .* fft(result_cir(link).samples'))';
        end
        result_cir(link).reference_delay = cir.reference_delay + add_delay_offset;
    end
    append_cir(cdx_out, result_cir);

    % Check for Cancel button press
    if getappdata(wb, 'canceling')
        break
    end
end
delete(wb);


%% IMPORTANT: close the files
delete(cdx_in);
delete(cdx_out);

%> @}
%> @}
