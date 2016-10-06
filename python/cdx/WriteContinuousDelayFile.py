#!/usr/bin/env python
#
# \file WriteContinuousDelayFile.py
# \date April 4, 2012
# \author Frank Schubert
#
#   CDX Library
#
#   As part of
#
#   SNACS - The Satellite Navigation Radio Channel Simulator
#
#   Class to write continuous-delay CDX files.
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

cir_dtype = np.dtype([('type', np.int16), ('delays', np.float64), ('real', np.float64), ('imag', np.float64)])

class WriteContinuousDelayFile:
    """Write a continuous-delay CDX file"""
    def __init__(self, file_name, parameters):
        # expects parameters as dict: { 'delay_type', 'c0', 'cir_rate', 'nof_links', 'enable_compression', 'transmitter_frequency' }
        self.file_name = file_name
        print "Open CDX file", self.file_name
        self.f = h5py.File(self.file_name, 'w')
        parameters_group = self.f.create_group("parameters")
        para_c0_m_s = parameters_group.create_dataset('c0_m_s', data=parameters['c0_m_s'], dtype=np.float64)

        if parameters['delay_type'] != 'continuous-delay':
            raise SystemExit("Error: WriteContinuousDelayFile: Can only handle continuous-delay data but you provided {} in parameters.".format(parameters['delay_type']))

        # write fixed-length string with h5py (http://docs.h5py.org/en/latest/strings.html):
        para_delay_type = parameters_group.create_dataset('delay_type', data=['continuous-delay'], shape=(1,), dtype="S16")

        para_cir_rate_Hz = parameters_group.create_dataset('cir_rate_Hz', data=parameters['cir_rate_Hz'], dtype=np.float64)
        para_transmitter_frequency_Hz = parameters_group.create_dataset('transmitter_frequency_Hz', data=parameters['transmitter_frequency_Hz'], dtype=np.float64)

        self.nof_links = len(parameters['link_names'])
        self.link_names = parameters['link_names']

        if len(parameters['link_names']) < 1:
            raise SystemExit('Error: WriteContinuousDelayFile: parameters["link_names"] is empty.')
        if len(parameters['link_names']) != len(parameters['component_types']):
            raise SystemExit('Error: WriteContinuousDelayFile: parameters["link_names"] has a different number of entries than parameters["component_types"].')

        self.link_groups = {} # contains data for a link
        self.cirs_groups = {} # contains the CIRs for a link
        self.ref_delays = {} # contains the reference delays for a link
        self.component_types = parameters['component_types']

        for link_name, component_types in zip(parameters['link_names'], parameters['component_types']):
            self.link_groups[link_name] = self.f.create_group("links/{}".format(link_name))
            self.link_groups[link_name].create_dataset('component_types', data=parameters['component_types'][link_name])
            self.cirs_groups[link_name] = self.link_groups[link_name].create_group("cirs")
            self.ref_delays[link_name] = np.empty(0, np.float64)

        self.current_cir_number = 0 # identical for all links, used when appending cirs

    def append_cir(self, cirs, ref_delays):
        # cirs and ref_delays must be lists with one entry per link:
        if len(cirs) != self.nof_links:
            raise SystemExit('Error: WriteContinuousDelayFile: append_cir: number of cirs ({}) does not match number of links in file ({}).'.format(len(cirs), self.nof_links))
        if len(ref_delays) != self.nof_links:
            raise SystemExit('Error: WriteContinuousDelayFile: append_cir: number of reference_delays ({}) does not match number of links in file ({}).'.format(len(ref_delays), self.nof_links))

        # go through all links:
        for k, link_name in enumerate(self.link_names):
            # component type must only contain entries from component_types:
            for idx in range(len(cirs[link_name])):
                if cirs[link_name][idx]['type'] > len(self.component_types):
                    raise SystemExit('Error: WriteContinuousDelayFile: append_cir: link_name {}: component type ({}) is larger than number of entries in component_types list ({}).'.format(link_name, cirs[link_name][idx]['type'], len(self.component_types)))

            dset_name = '{}'.format(self.current_cir_number)
            dset = self.cirs_groups[link_name].create_dataset(dset_name, data=cirs[link_name], dtype=cir_dtype)
            self.ref_delays[link_name] = np.hstack((self.ref_delays[link_name], ref_delays[link_name]))

        self.current_cir_number = self.current_cir_number + 1
    def __del__(self):
        # write all reference delays of all links to the file:
        for k, link_name in enumerate(self.link_names):
            self.link_groups[link_name].create_dataset("reference_delays", data=self.ref_delays[link_name])
#        print "dset.shape:", dset.shape
#        print "dset.dtype", dset.dtype
        print "closing CDX file {}.".format(self.file_name)
        self.f.close()
