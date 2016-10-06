%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% function plot_continuous_delay_cir_file
%
% This function plots a certain length of a CDX file
%
% Author: F. Schubert
% Date: 08-09-2010
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [ f, channel_params, dt ] = plot_continuous_delay_cir_file(cdx_file, link, plot_params, interp_params)

channel_params.cir_rate = get_cir_rate(cdx_file);
channel_params.start_cir = plot_params.start * channel_params.cir_rate + 1;
channel_params.end_cir = (plot_params.start + plot_params.length) * channel_params.cir_rate;
% fprintf('(channel_params.end_cir - channel_params.start_cir) : %f\n', (channel_params.end_cir - channel_params.start_cir) );
channel_params.nof_cirs = (channel_params.end_cir - channel_params.start_cir + 1) / interp_params.skip_step;

channel_params

% determine remaining parameters for interpolation:
assert(channel_params.nof_cirs == floor(channel_params.nof_cirs), ...
    sprintf('channel_params.nof_cirs (=%i) / interp_params.skip (=%i) step must be integer.', channel_params.nof_cirs, interp_params.skip_step));

channel_params.total_cirs = get_number_of_cirs(cdx_file);

% do we have enough CIRs in file?
assert(channel_params.end_cir <= channel_params.total_cirs, 'Not enough cirs in file.');


%% create interpolated CIRs:
disp('reading cirs and interpolating...');
[ interp_result, dt, dd, reference_delays ] = interpolate_cirs(cdx_file, link, interp_params, channel_params);

%% plot
disp('plotting...');
f = figure(2);
% fprintf('length time axis: %i\n', length(dt));
% fprintf('length delay axis: %i\n', length(dd));
% fprintf('size result delay axis: %i\n', size(interp_result));

s1 = surf(dt, dd, 20*log10(abs(interp_result)));
hold on;
title(plot_params.title);
caxis(plot_params.dB_range);
set(s1,'LineStyle','none');
set(s1,'EdgeLighting','phong');
if ~strcmp(plot_params.shading, 'faceted') set(s1,'FaceColor', plot_params.shading); end
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
ylabel('Delay [s]', 'FontSize', plot_params.font_size);

plot(dt, reference_delays + interp_params.add_delay_offset, plot_params.reference_delay_color ,'LineWidth', plot_params.reference_delay_linewidth);

ylim([dd(1), dd(end)]);
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
    saveas(f, [ get_file_name(cdx_file), '.fig' ]);
end
disp('done.');

end