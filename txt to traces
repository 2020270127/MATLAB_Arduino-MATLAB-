trace = load('waveform00001.txt');

for i=2:9
    
  data = load(sprintf('waveform0000%d.txt', i));
  trace= horzcat(trace, data); %updating trace var
  disp(i)
 
end

for i=10:99
   
  data = load(sprintf('waveform000%d.txt', i));
  trace= horzcat(trace, data);  
  disp(i)
end

for i=100:999
  data = load(sprintf('waveform00%d.txt', i));
  trace= horzcat(trace, data); 
  disp(i)
end


  data = load('waveform01000.txt');
  trace= horzcat(trace, data);  
  
 save('AES.traces', 'trace', '-ascii'); %save as ascii
 
 %MATLAB
