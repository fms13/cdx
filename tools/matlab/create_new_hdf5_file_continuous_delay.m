%> \addtogroup matlab_examples
%> @{
%> \addtogroup matlab_example_create_new_hdf5_file_continuous_delay create_new_hdf5_file_continuous_delay
%> @{
%>
%> \file create_new_hdf5_file_continuous_delay.m
%>
%> \brief This script creates a new CDX (HDF5 CIR) sample file with continuous delay data.
%
%> \date Sep 08, 2010
%> \author Frank M. Schubert

% the most interesting parameters are probably:
% plot_params.length = 1; % length to plot in seconds
% interp_params.skip_step = 1; % only every skip_step-th CIR is plotted

% if waitbars are still open from a previous program interruption, call:
% set(0,'ShowHiddenHandles','on')
% delete(get(0,'Children'))

clear all
close all
clear classes
clc

addpath('../CDX_library_functions');

file_name = 'test-CDX-file_continuous-delay.h5';

% set parameters for CDX file:
parameters.delay_type = 'continuous-delay';
parameters.c0 = 3e8;
parameters.cir_rate = 500;
parameters.nof_links = 2;
parameters.enable_compression = 0;

% create CDX file:
cdx_file = CDXWriteFile(file_name, parameters);

% create one CIR per link...
cir_link(1).delays = [ 1 2 3 ];
cir_link(1).weights = [ 4+7j 5+8j 6+9j ];
cir_link(1).reference_delay = [ 1.1 ];

cir_link(2).delays = [ 1 2 3 ];
cir_link(2).weights = [ 4+5j 5+6j 6+7j ];
cir_link(2).reference_delay = [ 2.1 ];

% ...and append the CIR to the file:
append_cir(cdx_file, cir_link);

% create another CIR per link...
cir_link(1).delays = [ 1 2 3 4 ];
cir_link(1).weights = [ 4+7j 5+8j 6+9j 7+8j];
cir_link(1).reference_delay = [ 1.2 ];

cir_link(2).delays = [ 1 2  ];
cir_link(2).weights = [ 4+5j 5+6j  ];
cir_link(2).reference_delay = [ 2.2 ];

% ...and append the CIR to the file:
append_cir(cdx_file, cir_link);

% IMPORTANT: close the file
delete(cdx_file);

% read the second CIR of link 1 from file:
cdx_file = CDXReadFile(file_name);
[ cir ] = get_cir(cdx_file, 1, 2);
fprintf('delays:');
cir.delays'
fprintf('weights:');
cir.weights
fprintf('reference delay:');
cir.reference_delay

%> @}
%> @}

