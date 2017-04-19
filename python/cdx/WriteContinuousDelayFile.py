#!/usr/bin/env python
#
# \file WriteContinuousDelayFile.py
# \date April 4, 2012
# \author Frank Schubert
#
#   CDX Library

import numpy as np
import h5py

cir_dtype = np.dtype([('type', np.uint16), ('id', np.uint64), ('delay', np.float64),
                      ('real', np.float64), ('imag', np.float64)])

vlen_string_dtype = h5py.special_dtype(vlen=bytes)
component_types_dtype = np.dtype([ ('id', np.uint16), ('name', vlen_string_dtype) ])

class WriteContinuousDelayFile:
    """Write a continuous-delay CDX file"""
    def __init__(self, file_name, c0_m_s, cir_rate_Hz, transmitter_frequency_Hz, link_names, component_types):
        self.file_name = file_name
        self.c0_m_s = c0_m_s
        self.cir_rate_Hz = cir_rate_Hz
        self.link_names = link_names
        self.nof_links = len(self.link_names)
        self.transmitter_frequency_Hz = transmitter_frequency_Hz
        self.component_types = component_types

        self.f = h5py.File(self.file_name, 'w')

        parameters_group = self.f.create_group("parameters")

        # write parameters to CDX file:
        # write CDX file type to HDF5 file:
        # write fixed-length string with h5py (http://docs.h5py.org/en/latest/strings.html):
        parameters_group.create_dataset('delay_type', data=['continuous-delay'], shape=(1,), dtype="S16")
        parameters_group.create_dataset('c0_m_s', data=c0_m_s, dtype=np.float64)
        parameters_group.create_dataset('cir_rate_Hz', data=self.cir_rate_Hz, dtype=np.float64)
        parameters_group.create_dataset('transmitter_frequency_Hz', data=self.transmitter_frequency_Hz, dtype=np.float64)

        # check that there are the same number of link names as component types:
        if len(self.link_names) != len(self.component_types):
            raise SystemExit('WriteContinuousDelayFile: number of provided component types ({}) does not match number of links in file ({}).'.format(len(self.component_types, len(self.link_names))))

        # check for empty link_names:
        if len(self.link_names) < 1:
            raise SystemExit('WriteContinuousDelayFile: link_names is empty.')

        self.link_groups = {}  # contains data for a link
        self.cirs_groups = {}  # contains the CIRs for a link
        self.ref_delays = {}  # contains the reference delays for a link

        for link_name, component_types in zip(self.link_names, self.component_types):
            self.link_groups[link_name] = self.f.create_group("links/{}".format(link_name))

            # component types: convert dictionary to hdf5 compound type:
            nof_component_types = len(self.component_types[link_name])
            component_types_h5 = np.empty(nof_component_types, dtype=component_types_dtype)
            for idx, key in enumerate(self.component_types[link_name]):
                component_types_h5[idx] = (key, self.component_types[link_name][key])
            # create dataset for component types, dtype is component_types_dtype:
            self.link_groups[link_name].create_dataset('component_types', data=component_types_h5, dtype=component_types_dtype)

            self.cirs_groups[link_name] = self.link_groups[link_name].create_group("cirs")
            self.ref_delays[link_name] = np.empty(0, np.float64)

        self.current_cir_number = 0  # identical for all links, used when appending cirs

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
                if cirs[link_name][idx]['type'] not in self.component_types[link_name].keys():
                    raise SystemExit('WriteContinuousDelayFile: append_cir: link_name {}: component type ({}) is not contained in component_types list ({}).'.format(link_name, cirs[link_name][idx]['type'], self.component_types[link_name]))

            dset_name = '{}'.format(self.current_cir_number)
            dset = self.cirs_groups[link_name].create_dataset(dset_name, data=cirs[link_name], dtype=cir_dtype)
            self.ref_delays[link_name] = np.hstack((self.ref_delays[link_name], ref_delays[link_name]))

        self.current_cir_number = self.current_cir_number + 1
    def __del__(self):
        # write all reference delays of all links to the file:
        for k, link_name in enumerate(self.link_names):
            self.link_groups[link_name].create_dataset("reference_delays", data=self.ref_delays[link_name])
        self.f.close()
