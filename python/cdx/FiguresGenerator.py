#!/usr/bin/env python
##
# \addtogroup python_implementation
# @{
#
# \file FiguresGenerator.py
# \date April 4, 2012
# \author Frank Schubert
#

import argparse
import os
import numpy as np
import matplotlib.pyplot as plt
from collections import defaultdict
from matplotlib.colors import LogNorm
import h5py
import glob
import ReadContinuousDelayFile
import ReadDiscreteDelayFile

component_colors_t = {0: 'r', 1: 'k', 2: 'b', 3: 'g', 4: 'y'}

##
# \brief Class which provides functionality to generate plots such as channel impulse
# responses, power over time, or the delay spread.
#
# The methods return matplotlib axis objects which can be added to matplotlib figure
# objects to display the plots.
class FiguresGenerator:
    def __init__(self, file_name):
        if file_name[-4:] == '.cdx':
            cdx_file_name = file_name
            cdd_file_name = file_name[:-4] + '.cdd'
        elif file_name[-4:] == '.cdd':
            cdx_file_name = file_name[:-4] + '.cdx'
            cdd_file_name = file_name
        else:
            raise Exception('Unkown file extension {0} of file {1}. Extension must be .cdx or .cdd.'.format(file_name[-4:], file_name))

        if os.path.isfile(cdx_file_name) == True:
            self.cdx_enabled = True

        else:
            self.cdx_enabled = False

        if os.path.isfile(cdd_file_name) == True:
            self.cdd_enabled = True
            # check if file can really be openend:
            try:
                cdd_f = h5py.File(cdd_file_name, 'r')
            except IOError:
                # No, we disable use of the cdd file:
                self.cdd_enabled = False

        else:
            self.cdd_enabled = False

        if self.cdd_enabled == False and self.cdx_enabled == False:
            raise Exception('Files {0} and {1} not found, at least one must be present.'.format(cdx_enabled, cdd_enabled))

        if self.cdx_enabled == True:
            self.cdx_file = ReadContinuousDelayFile.ReadContinuousDelayFile(cdx_file_name)

        if self.cdd_enabled == True:
            self.cdd_file = ReadDiscreteDelayFile.ReadDiscreteDelayFile(cdd_file_name)

        # TODO check if both files are present that their parameters match

        self.link_names = self.cdx_file.get_link_names()

        self.min_power_dB = -30
        self.max_power_dB = 5

        types, ids, delays, amplitudes, reference = self.cdx_file.get_cir(self.link_names[0], 0)

        partial_spread = (delays[0] - delays[-1]) / 10
        self.min_delay_s = delays[0] - partial_spread
        self.max_delay_s = delays[0] + partial_spread

        # check if there are scenery images available:
        result_dir_name = file_name[0:-4]

        # check if visualization is enabled in cdx file:
        self.visualization_enabled = False
        if "/visualization" in self.cdx_file.f:
            # is the fram_rate_Hz in the cdx file?
            if "frame_rate_Hz" in self.cdx_file.f["/visualization"]:
                self.frame_rate_Hz = self.cdx_file.f["/visualization/frame_rate_Hz"][0]
                # check if directories for each link are available:
                self.vis_link_dirs = {}
                for link_name in self.link_names:
                    vis_dir = "{}/{}".format(result_dir_name, link_name)
                    if os.path.isdir(vis_dir) == True:
                        # check if PNG files exist in this directory:
                        if len(glob.glob(vis_dir + "/*.png")) != 0:
                            print 'using directory {} to read visualization images of link {}'.format(vis_dir, link_name)
                            self.vis_link_dirs[link_name] = vis_dir
                            self.visualization_enabled = True
                        else:
                            print 'no PNG files in {} found: not displaying scenery images for link {}.'.format(vis_dir, link_name)

        print 'FiguresGenerator: self.visualization_enabled:', self.visualization_enabled

    def get_scenery_image_for_time(self, link_name, time):
        if self.visualization_enabled == False:
            raise Exception('get_scenery_image_for_cir_num: abort since visualization_enabled == False')

        # compute image number from time:
        image_num = round(time * self.frame_rate_Hz)

        return '{}/scenery{:07d}.png'.format(self.vis_link_dirs[link_name], int(image_num))

    def make_cir_axes(self, ax, link_name, nof_cir):
        ax.set_title('Channel Impulse Response, Link {}'.format(link_name))
        type_names = self.cdx_file.get_type_names(link_name)

        types, ids, delays, amplitudes, reference_delay = self.cdx_file.get_cir(link_name, nof_cir)
#         print delays, len(delays)
#         print delays[0]
#         for idx in np.arange(len(delays)):

        # set amplitudes that are zero to 1e-9 to be able to compute the logarithm:
        amplitudes[amplitudes == 0] = 1e-9

        try:
            amplitudes_dB = 10 * np.log10(np.abs(amplitudes))
        except RuntimeWarning:
            raise ValueError('amplitude values are smaller than zero, logarithm cannot be computed for: {}'.format(amplitudes))

        min_amplitude_dB = np.min(amplitudes_dB)
        max_amplitude_dB = np.max(amplitudes_dB)

        if min_amplitude_dB < self.min_power_dB:
            self.min_power_dB = min_amplitude_dB - 5

        if max_amplitude_dB > self.max_power_dB:
            self.max_power_dB = max_amplitude_dB + 5

        # adjust x axis for delay
        min_delay_s = np.min(delays)
        max_delay_s = np.max(delays)
        partial_spread = (max_delay_s - min_delay_s) / 10
        if min_delay_s < self.min_delay_s:
            self.min_delay_s = min_delay_s - partial_spread

        if max_delay_s > self.max_delay_s:
            self.max_delay_s = max_delay_s + partial_spread

        # make an own group for each type of components:

        # generate a sorted set object from the types:
        types_set = sorted(set(types))

        # gather the components in list for these type sets:
        delays_dict = defaultdict(np.array)
        amplitudes_dB_dict = defaultdict(np.array)

        # get dict: type -> name:
        types_to_names = self.cdx_file.get_type_names(link_name)

        nof_processed_components = 0
        for type in types_set:
            # check if type from CIR exists at all in . TODO move to cdx python lib, for this move links_to_component_types to a base class
            if type not in types_to_names.keys():
                raise Exception("type {} as read from CDX file is not in types_to_names.keys().".format(type))

            # types==type is the list of indices where types equals type
            delays_dict[type] = delays[types == type]
            amplitudes_dB_dict[type] = amplitudes[types == type]
            nof_processed_components += len(delays_dict[type])

        if nof_processed_components != len(delays):
            raise Exception("CIR #{}: Total number of components ({}) does not match sum of types of components ({}): not all component types are defined in component_types".format(nof_cir, nof_processed_components, len(delays)))

        # dictionary that assigns type ids to color strings:
        component_colors = {}

        # insert a color for each types:
        k = 0
        for type in types_to_names:
            component_colors[type] = component_colors_t[k]
            k += 1

#         for component in
        for t in types_set:
            markerline, stemlines, baseline = ax.stem(delays_dict[t] / 1e-6, 10 * np.log10(np.abs(amplitudes_dB_dict[t])),
                    bottom=self.min_power_dB, label=types_to_names[t])
            # make the stems appear in the color defined by the t number and component_colors:
            plt.setp(stemlines, 'color', component_colors[t])
            plt.setp(markerline, 'markeredgecolor', component_colors[t])
            plt.setp(markerline, 'markerfacecolor', component_colors[t])
            plt.setp(baseline, 'color', 'w')

        # create vertical line for reference delay:
        ax.axvline(x=reference_delay / 1e-6, color='m', linestyle='--', label='Reference')
        ax.set_ylim((self.min_power_dB, self.max_power_dB))
        ax.set_xlim((self.min_delay_s / 1e-6, self.max_delay_s / 1e-6))

        # change baseline of stem plot:
        # http://stackoverflow.com/questions/13145218/stem-plot-in-matplotlib
#         cir = np.zeros((1, 1), dtype=cir_dtype)
#         cir[0].delays = delays

        ax.set_ylabel('Amplitude [dB]')
        ax.set_xlabel('Delay [$\\mu$s]')
#         pl_ref = ax.plot(x_axis, ref_delays/1e-9, 'm', linewidth=1.5, label='Reference delay')
#         ax.set_xlim((x_axis[0][0], x_axis[-1][0]))
        ax.legend()
        ax.grid()

    ##
    # \brief
    # If length is zero, go until end of the file.
    def make_discr_cir_axes(self, ax, link_name, start_time, length):
        cirs, times, delays, ref_delays = self.cdd_file.get_cirs(link_name, start_time, length)

        x_axis = times
        y_axis = delays

        nof_x = cirs.shape[1]
        nof_y = cirs.shape[0]

        nof_x_points = 300
        if nof_x_points == 0:
            # user didn't set nof_x_points, set it to maximum value:
            nof_x_points = 300
        else:
            if nof_x < nof_x_points:
                # we cannot print more points than are available:
                nof_x_points = nof_x

        x_step = np.floor(nof_x / nof_x_points)

        # for 2D plot:
        data = np.zeros((nof_y, nof_x_points), dtype=complex)

        # only print nof_x_points cirs:
        for x in np.arange(nof_x_points):
            idx_into_cirs = int(np.floor(float(x) / nof_x_points * nof_x))
            data[:, x] = cirs[:, idx_into_cirs]

        data_dB = 20 * np.log10(np.abs(data))

        ax.set_title('Discrete Channel Impulse Response, Link {}'.format(link_name))

        # plot cir:
        cax = ax.imshow(data_dB, interpolation='nearest', aspect='auto', origin='bottom',
                     extent=[x_axis[0][0], x_axis[-1][0], y_axis[0][0] / 1e-9, y_axis[-1][0] / 1e-9],
                     vmin=-60, vmax=0)

#         cbar = fig.colorbar(cax)
#         cbar.set_label('dB')
        ax.set_xlabel('Time [s]')
        ax.set_ylabel('Delay [ns]')

        # plot reference delay:
        # check if there is a line for the reference delay already:
        ref_line_found = False
        for line in ax.lines:
            if line.get_label() == 'Reference Delay':
                ref_line_found = True
                # just update data:
                line.set_data(x_axis, ref_delays / 1e-9)

        if ref_line_found == False:
            # no ref line yet, create new one:
            pl_ref = ax.plot(x_axis, ref_delays / 1e-9, 'm', linewidth=1.5, alpha=0.7, label='Reference Delay')

        ax.legend()
        ax.set_xlim((x_axis[0][0], x_axis[-1][0]))

#         if args.margin_before_min_ref_delay != None:
#             before_min_delay = float(args.margin_before_min_ref_delay)
#             after_max_delay = float(args.margin_after_max_ref_delay)
#             ax.set_ylim(( (min(ref_delays) - before_min_delay) / 1e-9, (max(ref_delays) + after_max_delay) / 1e-9))
#         else:
        ax.set_ylim((delays[0][0] / 1e-9, delays[-1][0] / 1e-9))

    ##
    # \brief
    # If length is zero, go until end of the file.
    def make_delay_doppler_spectrum(self, ax, link_name, start_time=0.0, length=0.0):
        cirs, times, delays, ref_delays = self.cdd_file.get_cirs(link_name, start_time, length)

        # compute delay-Doppler spectrum:
        dds = np.fft.fftshift(np.fft.fft(cirs, cirs.shape[1], 1) / self.cdd_file.cir_rate_Hz, 1)

        nof_x = cirs.shape[1]
        nof_y = cirs.shape[0]

        # for 2D plot:
        nof_x_points = 0
        if nof_x_points == 0:
            # user didn't set nof_x_points, set it to maximum value:
            data = dds
        else:
            if nof_x < nof_x_points:
                # we cannot print more points than are available:
                nof_x_points = nof_x

            # only print nof_x_points cirs:
            data = np.zeros((nof_y, nof_x_points), dtype=complex)
            for x in arange(nof_x_points):
                idx_into_cirs = np.floor(np.float(x) / nof_x_points * nof_x)
                data[:, x] = dds[:, idx_into_cirs]


        # replace values close to zero:
        data[data < 10**-13 ] = 10**-13

        data_dB = 20 * np.log10(np.abs(data))

        # compute frequency axis:
        nof_cirs = float(cirs.shape[1])
        deltaf = self.cdd_file.cir_rate_Hz / nof_cirs
        #print "Frequency resolution: ", deltaf, " Hz"
        ymin = -nof_cirs / 2.0 * deltaf
        ymax = nof_cirs / 2.0 * deltaf - deltaf
        x_axis = np.arange(ymin, ymax, deltaf)

        ax.set_title('Doppler-Delay Spectrum, Link {}'.format(link_name))

        # plot cir:
        im = ax.imshow(data_dB, interpolation='nearest', aspect='auto', origin='bottom',
                     extent=[x_axis[0], x_axis[-1], delays[0][0] / 1e-9, delays[-1][0] / 1e-9],
                     vmin=-60, vmax=0)

#         cbar = fig.colorbar(cax)
#         cbar.set_label('dB')
        ax.set_xlabel('Frequency [Hz]')
        ax.set_ylabel('Delay [ns]')

        # plot reference delay:
        c0_m_s = 3e8  # speed of light
        # relative velocity receiver-transmitter in m/s:
        rx_velocity_m_p_s = (np.diff(ref_delays) * c0_m_s) / (1.0 / self.cdd_file.cir_rate_Hz)
        # compute doppler from relative velocity:
        reference_dopplers_Hz = self.cdd_file.transmitter_frequency_Hz - self.cdx_file.transmitter_frequency_Hz * np.sqrt((c0_m_s + rx_velocity_m_p_s) / (c0_m_s - rx_velocity_m_p_s))

        # check if there is a line for the reference delay already:
        ref_line_found = False
        for line in ax.lines:
            if line.get_label() == 'Reference Delay':
                ref_line_found = True
                # just update data:
                line.set_data(reference_dopplers_Hz, ref_delays[0:len(ref_delays) - 1] / 1e-9)

        if ref_line_found == False:
            # no ref line yet, create new one:
            ax.plot(reference_dopplers_Hz, ref_delays[0:len(ref_delays) - 1] / 1e-9, 'm',
                    linewidth=1.5, alpha=0.7, label='Reference Delay')

        ax.legend()
        ax.set_xlim((x_axis[0], x_axis[-1]))
#         if args.margin_before_min_ref_delay != None:
#             before_min_delay = float(args.margin_before_min_ref_delay)
#             after_max_delay = float(args.margin_after_max_ref_delay)
#             ax.set_ylim(( (min(ref_delays) - before_min_delay) / 1e-9, (max(ref_delays) + after_max_delay) / 1e-9))
#         else:
        ax.set_ylim((delays[0][0] / 1e-9, delays[-1][0] / 1e-9))
        return im

    def make_power_axes_magnitude(self, ax, link_name):
        print 'computing power plot for link ', link_name

        times, channel_power = self.cdx_file.compute_power_magnitude(link_name)

        ax.set_title('Power, Link {}'.format(link_name))
        ax.plot(times, 20 * np.log10(abs(channel_power)))
        ax.set_xlim((times[0], times[-1]))
        ax.set_xlabel('Time [s]')
        ax.set_ylabel('Power [dB]')
        ax.grid()

    def make_power_axes_coherent_sum(self, ax, link_name):
        print 'computing power plot for link ', link_name

        times, channel_power = self.cdx_file.compute_power_coherent_sum(link_name)

        ax.set_title('Power, Link {}'.format(link_name))
        ax.plot(times, 20 * np.log10(abs(channel_power)))
        ax.set_xlim((times[0], times[-1]))
        ax.set_xlabel('Time [s]')
        ax.set_ylabel('Power [dB]')
        ax.grid()

    def make_delay_spread_axes(self, ax, link_name):
        print 'computing delay spread plot for link ', link_name


        times, mp_spread = self.cdx_file.compute_multipath_spread(link_name)

        ax.set_title('Multipath Delay Spread, Link {}'.format(link_name))
        ax.plot(times, mp_spread / 1e-9)
        ax.set_xlim((times[0], times[-1]))
        ax.set_xlabel('Time [s]')
        ax.set_ylabel('Delay Spread [ns]')
        ax.grid()

    def make_nof_components_axes(self, ax, link_name):
        print 'computing number of components for link ', link_name

        times, nof_components = self.cdx_file.compute_nof_components(link_name)

        ax.set_title('Number of Components, Link {}'.format(link_name))
        ax.plot(times, nof_components)
        ax.set_xlim((times[0], times[-1]))
        ax.set_xlabel('Time [s]')
        ax.set_ylabel('Number')
        ax.grid()

    def make_pdp_axes(self, ax, link_name, lower_prob=10e-7, upper_prob=10e0, figure=None):
        print 'computing power delay profile plot for link ', link_name

        pdp, del_ax, pwr_ax = self.cdx_file.compute_pdp(link_name, lower_prob, upper_prob)

        ax.set_title('Power Delay Profile, Link {}'.format(link_name))
        ax.set_xlabel('Delay [ns]')
        ax.set_ylabel('Power [dB]')

    # test points to see correct orientation of plot:
#         pdp[round(0 / -pwr_step), round(0 / del_step)] = 0.1
#         pdp[round(-10 / -pwr_step), round(100e-9 / del_step)] = 0.1

        # plot PDP:
        im = ax.matshow(pdp, norm=LogNorm(vmin=lower_prob, vmax=upper_prob),  #
                        extent=[del_ax[0] / 1e-9, del_ax[-1] / 1e-9, pwr_ax[0], pwr_ax[-1] ],
                        interpolation='bicubic', aspect='auto')
        ax.xaxis.set_ticks_position('bottom')

        if figure != None:
            figure.colorbar(im)

        #return im

def main():
    # if called as standalone script, display one CIR:
    # parse command line arguments
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", help="input CDX file", action="store")
    parser.add_argument("-n", "--cir-number", help="number of CIR to plot, default = 0", default=0, action="store")
    parser.add_argument("-l", "--link-name", help="name of the link", action="store")
    args = parser.parse_args()

    if args.input == None:
        raise SystemExit("Error: no input file given (command line parameter -i is missing).")
    else:
        file_name = args.input
        print "plotting CIR #{0} of CDX file {1} ...".format(args.cir_number, args.input)

    cdx_figures = FiguresGenerator(args.input)

    fig = plt.figure(dpi=100)
    ax = fig.add_subplot(1, 1, 1)
    cdx_figures.make_cir_axes(ax, 'satellite01', 0)

    plt.show()

if __name__ == "__main__":
     main()

print 'all done.'

## @} #
