#!/usr/bin/env python
#
# \file ReadContinuousDelayFile.py
# \date April 4, 2012
# \author Frank Schubert
#
#   CDX Library
#
#   As part of
#
#   SNACS - The Satellite Navigation Radio Channel Simulator
#
#   Class to read continuous-delay CDX files.
#
#   Copyright (C) 2012-2013  F. M. Schubert
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

import numpy as np
import h5py
import warnings

warnings.filterwarnings('error')

cir_dtype = np.dtype([('type', np.uint16), ('id', np.uint64), ('delay', np.float64),
                      ('real', np.float64), ('imag', np.float64)])

##
# \brief Class that provides functions to read from a continuous-delay CDX file
#
# Moreover, operations such as computation of power-delay profiles are provided.
#
class ReadContinuousDelayFile:
    """Read a continuous-delay CDX file"""
    def __init__(self, file_name):
        # expects parameters as dict: { 'delay_type', 'c0', 'cir_rate_Hz', 'nof_links', 'enable_compression', 'transmitter_frequency_Hz' }
        print "Open CDX file", file_name
        self.f = h5py.File(file_name, 'r')

        parameters_group = self.f['parameters']

        delay_type = parameters_group['delay_type'][...]
        if delay_type != "continuous-delay":
            raise SystemExit("Error: ReadContinuousDelayFile: Cannot open file because it is not a continuous-delay CDX file but of type {}.".format(delay_type))

        # get number of links in file:
        self.nof_links = len(self.f['/links'])
        print "found {0} links in file.".format(self.nof_links)

        # read link_names and reference delays:
        self.link_names = []
        self.reference_delays = {}
        for group_name in self.f['/links']:
            self.link_names.append(group_name)
            self.reference_delays[group_name] = self.f['/links/{}/reference_delays'.format(group_name)][...]

        self.nof_cirs = len(self.f['/links/{}/cirs'.format(self.link_names[0])])


        # read remaining parameters:
        self.c0 = parameters_group['c0_m_s'][0]
        self.cir_rate_Hz = parameters_group['cir_rate_Hz'][0]
        self.cir_interval = 1.0 / self.cir_rate_Hz
        self.transmitter_frequency_Hz = parameters_group['transmitter_frequency_Hz'][0]

    def get_cir_rate(self):
        return self.cir_rate_Hz

    def get_nof_links(self):
        return self.nof_links

    def get_nof_cirs(self):
        return self.nof_cirs

    def get_link_names(self):
        return self.link_names

    def get_type_names(self, link_name):
        type_names_dataset = self.f['/links/{}/component_types'.format(link_name)]

        # we transform the dataset to a dictionary now:
        # type_name[id] -> name
        types_to_names = {}
        for dataset in type_names_dataset:
            #print "dataset['id']:", dataset['id'], ", dataset['name']:", dataset['name']
            id = int(dataset['id'])
            name = str(dataset['name'])
            types_to_names[id] = name

        return types_to_names

    def get_cir(self, link_name, n):
        cir_raw = self.f['/links/{}/cirs/{}'.format(link_name, n)]
        types = cir_raw['type']
        ids = cir_raw['id']
        delays = cir_raw['delay']
        amplitudes = cir_raw['real'] + 1j * cir_raw['imag']
#         cir = np.zeros((1, 1), dtype=cir_dtype)
#         cir[0].delays = delays

        reference = self.reference_delays[link_name][n]

        return types, ids, delays, amplitudes, reference

    def get_cir_start_end_numbers_from_times(self, start_time, length):
        cir_start = int(start_time * self.cir_rate_Hz)
        cir_end = int((start_time + length) * self.cir_rate_Hz)

        return cir_start, cir_end

    def compute_multipath_spread(self, link_name, start_time = 0.0, length = 0.0):
        g = self.f['/links/{}'.format(link_name)];
        total_nof_cirs = len(g['cirs'])

        # check if start_time and length can be processed:
        if length != 0.0:
            if start_time + length > total_nof_cirs / self.cir_rate_Hz:
                raise SystemExit("Error: start_time + length ({}) exceeds file length ({}) (start_time + length > total_nof_cirs / cir_rate_Hz).".format(start_time + length, nof_cirs / cir_rate))
            cir_start, cir_end = self.get_cir_start_end_numbers_from_times(start_time, length)

            #print 'processing cirs {} to {}'.format(cir_start, cir_end)

            reference_delays = g['reference_delays'][cir_start:cir_end]
            times = np.arange(cir_start * self.cir_interval, cir_end * self.cir_interval, self.cir_interval)
            nof_cirs = len(reference_delays)
        else:
            reference_delays = g['reference_delays'][...]
            nof_cirs = len(reference_delays)
            cir_start = 0
            cir_end = nof_cirs
            times = np.arange(0, nof_cirs * self.cir_interval, self.cir_interval)

        mp_spread = np.zeros((nof_cirs, 1))

        # for all cirs
        for cir_n in np.arange(nof_cirs):
            cir = g['cirs/{0}'.format(cir_start + cir_n)]

            delays = cir['delay']
            if len(delays) > 1: # there must be at least two components to compute the difference:
                comp_min_delay = min(delays)
                comp_max_delay = max(delays)
                mp_spread[cir_n] = comp_max_delay - comp_min_delay
            else:
                mp_spread[cir_n] = 0.0

        return times, mp_spread

    def compute_power(self, link_name, start_time = 0.0, length = 0.0):
        g = self.f['/links/{}'.format(link_name)];
        total_nof_cirs = len(g['cirs'])

        # check if start_time and length can be processed:
        if length != 0.0:
            if start_time + length > total_nof_cirs / self.cir_rate_Hz:
                raise SystemExit("Error: start_time + length ({}) exceeds file length ({}) (start_time + length > total_nof_cirs / cir_rate_Hz).".format(start_time + length, nof_cirs / cir_rate))
            cir_start, cir_end = self.get_cir_start_end_numbers_from_times(start_time, length)

            #print 'processing cirs {} to {}'.format(cir_start, cir_end)

            reference_delays = g['reference_delays'][cir_start:cir_end]
            times = np.arange(cir_start * self.cir_interval, cir_end * self.cir_interval, self.cir_interval)
            nof_cirs = len(reference_delays)
        else:
            reference_delays = g['reference_delays'][...]
            nof_cirs = len(reference_delays)
            cir_start = 0
            cir_end = nof_cirs
            times = np.arange(0, nof_cirs * self.cir_interval, self.cir_interval)

        channel_power = np.zeros((nof_cirs, 1), dtype=complex)

        # for all cirs
        for cir_n in np.arange(nof_cirs):
            cir = g['cirs/{0}'.format(cir_start + cir_n)]
            amplitudes = cir['real'] + 1j * cir['imag']
            channel_power[cir_n] = np.sum(np.abs(amplitudes))

        return times, channel_power

    def get_min_max_power(self, link_name):
        g = self.f['/links/{}'.format(link_name)];
        cir = g['cirs/0']
        min_power = np.abs(cir['real'] + 1j * cir['imag'])
        max_power = np.abs(cir['real'] + 1j * cir['imag'])

        # for all cirs
        for cir_n in np.arange(nof_cirs):
            cir = g['cirs/{0}'.format(cir_start + cir_n)]
            amplitudes = abs(cir['real'] + 1j * cir['imag'])

            min_amplitudes = np.min(amplitudes)
            if min_amplitudes < min_power:
                min_power = min_amplitudes

            max_amplitudes = np.max(amplitudes)
            if max_amplitudes > max_power:
                max_power = max_amplitudes

        return min_power, max_power

    def compute_pdp(self, link_name, lower_prob=10e-7, upper_prob=10e0):
        # power axis ( = power bins):
        pwr_ax = np.arange(-30, 1, 1)  # in dB, highest power must be zero!
        nof_pwr_bins = len(pwr_ax)
        # delay axis ( = delay bins):
        del_ax = np.arange(0, 500e-9, 10e-9);  # in s
        nof_del_bins = len(del_ax)
        # power step:
        pwr_step = (pwr_ax[-1] - pwr_ax[0]) / (len(pwr_ax) - 1)
        # delay step:
        del_step = (del_ax[-1] - del_ax[0]) / (len(del_ax) - 1)

        # pdp will be plotted in dB, initialize with min values:
        pdp = lower_prob * np.ones((len(pwr_ax), len(del_ax)))

        g = self.f['/links/{}'.format(link_name)];
        reference_delays = g['reference_delays'][...]

        # for all cirs
        for cir_n in np.arange(len(reference_delays)):
            types, ids, delays, amplitudes, reference = self.get_cir(link_name, cir_n)

            #delays = delays - reference_delays[cir_n]
            try:
                powers = 20 * np.log10(np.abs(amplitudes))  # TODO
            except RuntimeWarning:
                print 'amplitudes:', amplitudes, ' abs amplitudes:', np.abs(amplitudes)
                sys.os.exit(1)

            # for all components:
            for k, val in enumerate(delays):
                # compute component delay:
                del_bin = int(round(delays[k] / del_step))
                if del_bin < 0:
                    raise SystemError('del_bin < 0. delays[k]: {0}, reference_delays[k]: {1}'.format(delays[k], reference_delays[k]))
                pwr_bin = int(round(powers[k] / -pwr_step))
                # only bin this component if it is inside power/delay axes:
                if del_bin <= (nof_del_bins - 1) and pwr_bin <= (nof_pwr_bins - 1) and pwr_bin >= 0:
                    # print "powers[k]: ", powers[k], ", pwr_bin" , pwr_bin, ", del_bin: ", del_bin
                    pdp[pwr_bin, del_bin] = pdp[pwr_bin, del_bin] + 1

        return pdp / sum(sum(pdp)), del_ax, pwr_ax

    def __del__(self):
        # close CDX file if it is open:
        try:
            self.f.close()
        except AttributeError:
            # file was not openend
            pass
