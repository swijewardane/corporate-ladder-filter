classdef MoogLadderPlugin < audioPlugin
    properties

        Cutoff = 2000 % Hz
        Resonance = 0.0 % k [0-3.99]
        Gain = 0 % Input gain in db

        bypass = 0 % DSP Bypass
        ADAA_bypass = 1 % ADAA bypass

    end

    properties (Access = private)

        tol = 1e-6
        smoothingRate = 0.5
        kLimitingFactor = 0.5
        fs = 44100 % Default value

        CutoffSmoother 
        ResonanceSmoother
        GainSmoother

        bypass_flag = false
        ADAA_bypass_flag = false

        wnm1 = zeros(4, 2)
        xnm1 = zeros(4, 2)
        feedback = zeros(1, 2)
        drv_x_prev = zeros(1, 2)
        drv_F_prev = zeros(1, 2)
        fb_x_prev = zeros(1, 2)
        fb_F_prev = zeros(1, 2)

    end

    properties (Constant)
        PluginInterface = audioPluginInterface( ...
            'PluginName', 'Corporate Ladder Filter',...
            'VendorName', 'Sahan',...
            'UniqueId', 'SCLF', ...
            'BackgroundImage', 'CORPORATE LADDER FILTER.png',...
            audioPluginParameter('Cutoff', ...
            'DisplayName', 'Cutoff', ...
            'Label', 'Hz', ...
            'Mapping', {'log', 20, 20000}, ...
            'Style', 'rotaryknob', ...
            'Filmstrip', 'BOS_knob_AutofilterPan_72x72_128f.png', ...
            'FilmstripFrameSize', [72 72], ...
            'DisplayNameLocation', 'none', ...
            'EditBoxLocation', 'none',...
            'Layout', [2, 1]), ...
            audioPluginParameter('Resonance', ...
            'DisplayName', 'Resonance', ...
            'Label', 'k', ...
            'Mapping', {'pow', 3, 0, 3.99}, ...
            'Style', 'rotaryknob', ...
            'Filmstrip', 'BOS_knob_AutofilterPan_72x72_128f.png', ...
            'FilmstripFrameSize', [72 72],...
            'DisplayNameLocation', 'none', ...
            'EditBoxLocation', 'none',...
            'Layout', [2, 2]), ...
            audioPluginParameter('Gain', ...
            'DisplayName', 'Gain', ...
            'Label', 'db', ...
            'Mapping', {'lin', -60, 12}, ...
            'Style', 'vslider', ...
            'Filmstrip', 'ST_fader_10X198_128f.png', ...
            'FilmstripFrameSize', [10 128], ...
            'DisplayNameLocation', 'none', ...
            'EditBoxLocation', 'none',...
            'Layout', [2, 3; 4, 3]), ...
            audioPluginParameter('bypass', ...
            'DisplayName', 'Plugin Bypass', ...
            'DisplayNameLocation', 'none', ...
            'EditBoxLocation', 'none',...
            'Mapping', {'int', 0, 1}, ...
            'Style', 'vslider', ...
            'Filmstrip', 'ST_button_pwr_30x30_6f.png', ...
            'FilmstripFrameSize', [36 36],...
            'Layout', [4, 1]), ...
            audioPluginParameter('ADAA_bypass', ...
            'DisplayName', 'ADAA Bypass', ...
            'DisplayNameLocation', 'none', ...
            'EditBoxLocation', 'none',...
            'Mapping', {'int', 0, 1}, ...
            'Style', 'vslider', ...
            'Filmstrip', 'ST_button_pwr_30x30_6f.png', ...
            'FilmstripFrameSize', [36 36],...
            'Layout', [4, 2]), ...
            audioPluginGridLayout(...
            'RowHeight', [50, 85, 20, 85], ...
            'ColumnWidth', [135, 135, 135])...
            )
    end

    methods
        function plugin = MoogLadderPlugin()
            plugin.CutoffSmoother = ParameterSmoother(plugin.Cutoff, plugin.smoothingRate);
            plugin.ResonanceSmoother = ParameterSmoother(plugin.Resonance, plugin.smoothingRate);
            plugin.GainSmoother = ParameterSmoother(plugin.Gain, plugin.smoothingRate);
            
        end
        
        function out = process(plugin, in)

            % Initialize key properties and per buffer state variables
            fs_buffer = plugin.fs;
            fc = plugin.CutoffSmoother.step();
            k = plugin.ResonanceSmoother.step();
            kLimit = plugin.kLimitingFactor;
            gain = 10^(plugin.GainSmoother.step()/20);

            wnm1_buffer = plugin.wnm1;
            xnm1_buffer = plugin.xnm1;
            feedback_buffer = plugin.feedback;
            drv_x_prev_buffer = plugin.drv_x_prev;
            drv_F_prev_buffer = plugin.drv_F_prev;
            fb_x_prev_buffer = plugin.fb_x_prev;
            fb_F_prev_buffer = plugin.fb_F_prev;
            tol_buffer = plugin.tol;

            % moog_coeffs function + frequency-dependent k-limiting

            fc = min(fc, fs_buffer*0.45);
            fc_normalized = fc/(fs_buffer*0.45);
            k_lim = 3.99*(1 - kLimit*fc_normalized);
            k = min(k, k_lim);

            g = tan(pi .* fc ./ fs_buffer);
            
            num0 = g ./ (g + 1);
            num1 = num0;
            den1 = (g - 1) ./ (g + 1);

            % Define output and bypass checks

            [nFrames, nChs] = size(in);

            out_wet = zeros(nFrames, nChs);
            bypass_ADAA_buffer = plugin.ADAA_bypass_flag;
            bypass_plugin_buffer = plugin.bypass_flag;
            
            

            % Buffer loop begins

            for n = 1:nFrames
                for ch = 1:nChs

                    % Drive stage 
                    drive_in = gain*in(n, ch);

                    % Gain check (no drive saturation unless + gain)

                    if gain <= 1.0
                        drive_out = drive_in;

                    % ADAA check (drive stage)

                    elseif bypass_ADAA_buffer
                        drive_out = tanh(drive_in);
                    else
                        dx = drive_in - drv_x_prev_buffer(ch);
                        F_drive_in = log(cosh(drive_in));

                        % Tolerance check

                        if abs(dx) < tol_buffer
                            drive_out = tanh(0.5*(drive_in + drv_x_prev_buffer(ch)));
                        else
                            drive_out = (F_drive_in - drv_F_prev_buffer(ch))/dx;
                        end

                        drv_F_prev_buffer(ch) = F_drive_in;

                    end

                    drv_x_prev_buffer(ch) = drive_in;

                    % Ladder Filter

                    input = drive_out - k*feedback_buffer(ch);

                    for i = 1:4
                        output = num0*input + num1*xnm1_buffer(i, ch) - den1*wnm1_buffer(i, ch);
                        xnm1_buffer(i, ch) = input;
                        wnm1_buffer(i, ch) = output;
                        input = output;
                    end
                    
                    % out_wet(n, ch) = output;
                    feedback_in = output;
                    
                    % ADAA check (feedback stage)

                    if bypass_ADAA_buffer
                        feedback_out = tanh(feedback_in);
                    else
                        df = feedback_in - fb_x_prev_buffer(ch);
                        F_feedback_in = log(cosh(feedback_in));

                        % Tolerance check

                        if abs(df) < tol_buffer
                            feedback_out = tanh(0.5*(feedback_in + fb_x_prev_buffer(ch)));
                        else
                            feedback_out = (F_feedback_in - fb_F_prev_buffer(ch))/df;
                        end

                        fb_F_prev_buffer(ch) = F_feedback_in;

                    end

                    fb_x_prev_buffer(ch) = feedback_in;
                    feedback_buffer(ch) = feedback_out;
                    out_wet(n, ch) = feedback_out;

                end
            end

            % Plugin bypass check

            if bypass_plugin_buffer
                out = in;
            else
                out = out_wet;

            end

            % Write back all per buffer state variables

            plugin.wnm1 = wnm1_buffer;
            plugin.xnm1 = xnm1_buffer;
            plugin.drv_x_prev = drv_x_prev_buffer;
            plugin.drv_F_prev = drv_F_prev_buffer;
            plugin.fb_x_prev = fb_x_prev_buffer;
            plugin.fb_F_prev = fb_F_prev_buffer;
            plugin.feedback = feedback_buffer;
            
            
        end

        function reset(plugin)
            
            % DSP state reset
            plugin.wnm1 = zeros(4, 2);
            plugin.xnm1 = zeros(4, 2);
            plugin.drv_x_prev = zeros(1, 2);
            plugin.drv_F_prev = zeros(1, 2);
            plugin.fb_x_prev = zeros(1, 2);
            plugin.fb_F_prev = zeros(1, 2);
            plugin.feedback = zeros(1, 2);

            % Smoother reset
        
            plugin.fs = getSampleRate(plugin);
            plugin.CutoffSmoother.resetState(plugin.Cutoff, plugin.smoothingRate);
            plugin.ResonanceSmoother.resetState(plugin.Resonance, plugin.smoothingRate);
            plugin.GainSmoother.resetState(plugin.Gain, plugin.smoothingRate);
                        

        end

        function set.Cutoff(plugin, val)
            plugin.Cutoff = val;
            plugin.CutoffSmoother.setTargetValue(val);
            
        end

        function set.Resonance(plugin, val)
            plugin.Resonance = val;
            plugin.ResonanceSmoother.setTargetValue(val);
        end

        function set.Gain(plugin, val)
            plugin.Gain = val;
            plugin.GainSmoother.setTargetValue(val);
        end

        function set.bypass(plugin, val)
            plugin.bypass = val;
            plugin.bypass_flag = logical(val);
        end

        function set.ADAA_bypass(plugin, val)
            plugin.ADAA_bypass = val;
            plugin.ADAA_bypass_flag = logical(val);
            plugin.drv_x_prev = zeros(1, 2);
            plugin.drv_F_prev = zeros(1, 2);
            plugin.fb_x_prev = zeros(1, 2);
            plugin.fb_F_prev = zeros(1, 2);

        end

    end
end
