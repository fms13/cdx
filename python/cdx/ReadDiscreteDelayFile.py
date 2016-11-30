#!/usr/bin/env python
#
# \file ReadContinuousDelayFile.py
# \date April 4, 2012
# \author Frank Schubert
#
#   CDX Library
#

import numpy as np
import h5py
import os

cir_dtype = np.dtype([('type', np.uint16), ('id', np.uint64), ('delay', np.float64),
                      ('real', np.float64), ('imag', np.float64)])

class ReadDiscreteDelayFile:
    """Read a discrete-delay CDX file"""
    def __init__(self, file_name):
        # expects parameters as dict: { 'delay_type', 'c0_m_s', 'cir_rate_Hz', 'nof_links', 'enable_compression', 'transmitter_frequency_Hz' }
        print "Open CDX file", file_name
        self.f = h5py.File(file_name, 'r')

        parameters_group = self.f['parameters']

        delay_type = parameters_group['delay_type'][...]
        if delay_type != "discrete-delay":
            raise SystemExit("Error: ReadContinuousDelayCDXFile: Cannot open file because it is not a discrete-delay CDX file but of type {}.".format(delay_type))

        # get number of links in file:
        self.nof_links = len(self.f['/links'])
        print "found {0} links in file.".format(self.nof_links)

        # read link_names:
        self.link_names = []
        for group_name in self.f['/links']:
            self.link_names.append(group_name)

        self.nof_cirs = len(self.f['/links/{}/reference_delays'.format(self.link_names[0])])

        # read remaining parameters:
        self.c0_m_s = parameters_group['c0_m_s'][0]
        self.cir_rate_Hz = parameters_group['cir_rate_Hz'][0]
        self.cir_interval = 1.0 / self.cir_rate_Hz
        self.delay_smpl_freq_Hz = parameters_group['delay_smpl_freq_Hz'][0]
        self.transmitter_frequency_Hz = parameters_group['transmitter_frequency_Hz'][0]
        self.length_s = self.nof_cirs / self.cir_rate_Hz

    def get_length_s(self):
        return self.length_s

    def get_nof_links(self):
        return self.nof_links

    def get_link_names(self):
        return self.link_names

    def get_cir_start_end_numbers_from_times(self, start_time, length):
        cir_start = int(start_time * self.cir_rate_Hz)
        cir_end = int((start_time + length) * self.cir_rate_Hz)

        return cir_start, cir_end

    def get_cirs(self, link_name, start_time, length):
        g = self.f['/links/{}'.format(link_name)];
        total_nof_cirs = g['cirs_real'].shape[1]

        # check if start_time and length can be processed:
        if length != 0.0:
            if start_time + length > total_nof_cirs / self.cir_rate_Hz:
                raise SystemExit("Error: start_time + length ({}) exceeds file length ({}) (start_time + length > total_nof_cirs / cir_rate_Hz).".format(start_time + length, total_nof_cirs / self.cir_rate_Hz))
            cir_start, cir_end = self.get_cir_start_end_numbers_from_times(start_time, length)

            print 'processing cirs {} to {}'.format(cir_start, cir_end)

            reference_delays = g['reference_delays'][cir_start:cir_end]
            times = np.arange(cir_start * self.cir_interval, cir_end * self.cir_interval, self.cir_interval)
            nof_cirs = len(reference_delays)

            cirs_real = g['cirs_real'][:, cir_start:cir_end]
            cirs_imag = g['cirs_imag'][:, cir_start:cir_end]
            ref_delays = g['reference_delays'][cir_start:cir_end]

            times = g['x_axis'][cir_start:cir_end]
            delays = g['y_axis']

        else:
            reference_delays = g['reference_delays'][...]
            nof_cirs = len(reference_delays)

            cirs_real = g['cirs_real'][...]
            cirs_imag = g['cirs_imag'][...]
            ref_delays = g['reference_delays'][...]

            times = g['x_axis'][...]
            delays = g['y_axis']

        cirs = cirs_real + 1j * cirs_imag

        return cirs, times, delays, ref_delays

    def compute_power(self, link_name, start_time = 0.0, length = 0.0):

        cirs, times, delays, ref_delays = self.get_cirs(link_name, start_time, length)

        channel_power = np.abs(np.sum(cirs, axis=0))**2

        return times, channel_power


    def __del__(self):
        # close CDX file if it is open:
        try:
            self.f.close()
        except AttributeError:
            # file was not openend
            pass
