function y = gen_wave(tone, rhythm)
    Fs = 8192;
    freqs = [523, 587, 659, 698, 783, 880, 988, 1046, 1175, 1318, 1397, 1568];
    x = linspace(0, 2 * pi * rhythm, floor(Fs * rhythm));
    m = sin(freqs(tone) * x) .* (1 - x / (2 * pi));
    y = smoothdata(m);
end
