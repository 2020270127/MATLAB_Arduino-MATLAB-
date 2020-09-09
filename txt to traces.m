trace = load('waveform05d.txt');

for i=1:1000
    {
    data = load(sprintf('waveform%05d.txt', i));
    trace= horzcat(trace, data);  
    save('AES.traces', 'trace', '-ascii'); %save as ascii
} 
 %MATLAB
