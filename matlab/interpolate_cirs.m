%> \addtogroup matlab_implementation
%> @{
%>
%> \file interpolate_cirs.m
%>
%> \date Sep 08, 2010
%> \author Frank M. Schubert, G. Steinböck

%> \brief This function reads continuous CIRs from a CDX file and interpolates them.
function [ Result, time_axis, delay_axis, ref_delays ] = interpolate_cirs(cdx_file, link, interp, sim_params)

fsmpl = 2 * interp.bandwidth;
Ts = 1 / fsmpl;
Om = 2 * interp.bandwidth;

interp.nof_delay_samples = floor(interp.max_delay * fsmpl)

Result = zeros(interp.nof_delay_samples, sim_params.nof_cirs);

ref_delays = zeros(sim_params.nof_cirs, 1);

add_absolute_delay = interp.add_delay_offset;

wb = waitbar(0, 'CIR interpolation...', 'CreateCancelBtn',...
    'setappdata(gcbf,''canceling'',1)');

x = 0:Ts:interp.max_delay-Ts;

act_cir = 1;
for i = sim_params.start_cir:interp.skip_step:sim_params.end_cir

    waitbar(act_cir / (sim_params.nof_cirs), wb);

    cir = get_cir(cdx_file, link, i);
    ref_delays(act_cir) = cir.reference_delay;

    % interpolate

    %         for o = 1:numel(delays)
    %                 fac = Om .* (x - (delays(o) + add_absolute_delay));
    %                 Result(:, indx) = Result(:, indx) + weights(o) .* sinc(fac)';
    %         end

    % loop was substituted by
    if interp.move_to_delay0 == true
%         add_absolute_delay = add_absolute_delay - ref_delays(i);
        cir.delays = cir.delays - ref_delays(act_cir);
        ref_delays(act_cir) = 0;
    end
    fac = repmat(x, numel(cir.delays), 1) - ...
        repmat(cir.delays + add_absolute_delay, 1, interp.nof_delay_samples);
    Result(:, act_cir) = cir.weights' * sinc(Om * fac);
    act_cir = act_cir + 1;

    % Check for Cancel button press
    if getappdata(wb, 'canceling')
        break
    end
end

if interp.enable_windowing
    waitbar(1, wb, 'windowing...');
    window = fftshift(hanning(interp.nof_delay_samples));
    window = window .* (size(window, 1) / sum(window));  % do some window normalization to keep power constant
    Result = ifft(repmat(window, [1 size(Result, 2)]) .* fft(Result, [], 1), [], 1);
end

delete(wb);
start_time = (sim_params.start_cir-1) / sim_params.cir_rate;
end_time = sim_params.end_cir / sim_params.cir_rate;
time_axis = start_time:1/sim_params.cir_rate*interp.skip_step:end_time;
time_axis = time_axis(1:end-1);
delay_axis = (0:interp.nof_delay_samples - 1) / fsmpl;

end
%> @}
