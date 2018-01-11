%> \addtogroup matlab_implementation
%> @{
%>
%> \file plot_discrete_delay_cir_file.m
%>
%> \date Sep 08, 2010
%> \author Frank M. Schubert

%> \brief This function plots a certain length of a CDX file
function [ plot_data, dt, dd, reference_delays, f2 ] = plot_discrete_delay_cir_file(cdx_file, link, plot_params)

cir_rate = get_cir_rate(cdx_file);

start_cir = round(plot_params.start * cir_rate + 1);
end_cir = round((plot_params.start + plot_params.length) * cir_rate);
% fprintf('(channel_params.end_cir - channel_params.start_cir) : %f\n', (end_cir - start_cir) );
nof_cirs = end_cir - start_cir + 1;

fprintf('plot_discrete_delay_cir_file: Plotting CIRs %i to %i...\n', ...
    start_cir, end_cir);

total_cirs = get_number_of_cirs(cdx_file);

% do we have enough CIRs in file?
assert(end_cir <= total_cirs, 'Not enough cirs in file.');

nof_delay_samples = get_number_of_delay_samples(cdx_file);
delay_smpl_freq = get_delay_smpl_freq(cdx_file);

% time axis:
dt = 0:1/cir_rate:(nof_cirs-1) / cir_rate;

% delay axis:
Ts = 1 / delay_smpl_freq;
max_delay = nof_delay_samples / delay_smpl_freq;
dd = 0:Ts:max_delay-Ts;

reference_delays = zeros(nof_cirs, 1);
CIRs = zeros(nof_delay_samples, nof_cirs);

% get cirs
wb = waitbar(.1, 'Reading CIRs...','Name','Plotting CIRs of discrete-delay CDX file');

cir = get_discrete_delay_cirs(cdx_file, link, start_cir, end_cir);
nof_cirs = size(cir.samples, 1);

reference_delays = cir.reference_delays;

if plot_params.move_reference_delay_to_zero
    dd_off = reference_delays(1);
    dd = dd - dd_off;
    reference_delays = reference_delays - dd_off;

    % %     for k = 1:nof_cirs
% %         CIRs = circshift(cir.samples.', [-cir.reference_delays(k) / Ts, 0]);
% %     end
%     reference_delays = reference_delays - plot_params.plot_delay_before_los;
%     dd = dd - plot_params.plot_delay_before_los;
% else
end

CIRs = cir.samples.';

waitbar(0.3, wb, 'Windowing...');
%% windowing
if plot_params.enable_windowing
    window = fftshift(hanning(nof_delay_samples));
    window = window .* (size(window, 1) / sum(window));  % do some window normalization to keep power constant
    CIRs = ifft(repmat(window, [1 size(CIRs, 2)]) .* fft(CIRs, [], 1), [], 1);
end

%% plot
disp('plotting...');
waitbar(0.9, wb, 'Plotting...');
f2 = figure;
% fprintf('length time axis: %i\n', length(dt));
% fprintf('length delay axis: %i\n', length(dd));
% fprintf('size result: %i\n', size(CIRs));

s1 = surf(dt, dd / 1e-9, 20*log10(abs(CIRs)));
hold on;
title(plot_params.title);
caxis(plot_params.dB_range);
set(s1,'LineStyle','none');
set(s1,'EdgeLighting','phong');
set(s1,'FaceColor', plot_params.shading);
view(2);
if strcmp(plot_params.colormap, 'gray')
    c = colormap('gray');
    d = c(end:-1:1,:);
    colormap(d);
else
end
cb = colorbar;
ylabel(cb, '[dB]', 'FontSize', plot_params.font_size);
xlabel('Time [s]', 'FontSize', plot_params.font_size);
ylabel('Delay [ns]', 'FontSize', plot_params.font_size);

plot(dt, reference_delays / 1e-9, plot_params.reference_delay_color ,'LineWidth', plot_params.reference_delay_linewidth);

if plot_params.adjust_delay
    disp('adjusting delays');
    min_delay = min(reference_delays);
    max_delay = max(reference_delays);
    ylim([min_delay - plot_params.plot_delay_before_los, ...
        max_delay + plot_params.plot_delay_after_los] / 1e-9);
else
    ylim([dd(1), dd(end)] / 1e-9);
end

xlim([dt(1), dt(end)]);

if ~strcmp(plot_params.shading, 'interp')
    % legend not supported for interp shading
    legend(plot_params.legend_str);
end

%% save files:
if plot_params.save_pdf
    disp('saving plot as PDF...');
    set(gcf,'paperunits', 'inches');
    set(gcf,'paperpos', [ [0 0] , plot_params.save_format ]);
    set(gcf,'PaperSize', plot_params.save_format);
    print('-r300','-dpdf', [ get_file_name(cdx_file), '.pdf' ]);
    % % print('-r300','-djpeg', 'Figure10.jpg');
end
if plot_params.save_fig
    saveas(f2, [ get_file_name(cdx_file), '.fig' ]);
end

plot_data = CIRs.';

delete(wb);
disp('done.');

end
%> @}
