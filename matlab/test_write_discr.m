clc
clear all
close all

nof_cirs = 100;
nof_delays = 20;

link_cir(1).cirs = zeros(nof_cirs, nof_delays) + j*ones(nof_cirs, nof_delays);
link_cir(1).reference_delays = 1:nof_cirs;

% set parameters for CDX file:
        parameters.delay_type = 'discrete-delay';
        parameters.c0 = 3e8;
        parameters.cir_rate = 1/300;
        parameters.delay_smpl_freq = 100e6; % Hz
        parameters.nof_delay_samples = nof_delays;
        parameters.nof_links = 1;
        parameters.enable_compression = 1;

% create CDX file:
cdx_file = CDXWriteFile('test-discr.h5', parameters);

write_discrete_delay_cirs(cdx_file, link_cir);

delete(cdx_file);

cdx_file = CDXReadFile('test-discr.h5');

cirs = get_discrete_delay_cirs(cdx_file, 1);
delete(cdx_file);


