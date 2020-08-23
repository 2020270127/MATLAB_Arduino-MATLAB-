clear all;
delete(instrfindall)
clc;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Communication setup (Arduino & DSO) %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
PORT_NUM   = 'COM5';
BAUD_RATE  = 230400;
IP_ADDRESS = '163.152.207.242';
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Power analysis setup                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
NUM_TRACE  = 1000;
DELAY_TIME = 0.5;
FILE_PATH  = 'C:\Users\win7\Desktop\matlab_sca_dso_arduino';
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%======================================
%Arduino setting
%======================================
% COM setting
arduino = serial(PORT_NUM, 'Baudrate', BAUD_RATE, 'Parity', 'none', 'Databit', 8, 'Terminator', 'LF');
fopen(arduino);
pause(2);
file_path1 = sprintf('%s/plaintext.txt', FILE_PATH);
file_path2 = sprintf('%s/ciphertext.txt', FILE_PATH);
path1 = fopen(file_path1, 'w');
path2 = fopen(file_path2, 'w');
fclose(path1);
fclose(path2);

%======================================
% DSO setting
%======================================
interfaceObj = instrfind('Type', 'tcpip', 'RemoteHost', IP_ADDRESS, 'RemotePort', 1861, 'Tag', '');

if isempty(interfaceObj)
    interfaceObj = tcpip(IP_ADDRESS, 1861);
else
    fclose(interfaceObj);
    interfaceObj = interfaceObj(1);
end

deviceObj = icdevice('lecroy_basic_driver.mdd', interfaceObj);
connect(deviceObj);

% Trigger mode setting
groupObj = get(deviceObj, 'Trigger');
groupObj = groupObj(1);
set(groupObj, 'Mode', 'run');
pause(DELAY_TIME);

% Channel coupling mode setting
groupObj = get(deviceObj, 'Channel');
groupObj = groupObj(1);
set(groupObj, 'Coupling', 'ac');
pause(DELAY_TIME);

fprintf(arduino, '1');

groupObj = get(deviceObj, 'Waveform');
groupObj = groupObj(1);
[Y, T, XUNIT, YUNIT, HEADER] = invoke(groupObj, 'readwaveform', 'channel1');
pause(DELAY_TIME);
ref = Y;

%======================================
% File open for cihertext
%======================================
fileID3 = fopen(file_path1,'a');
fileID2 = fopen(file_path2,'a');

%======================================
% Collecting power trace
%======================================
for i=1:NUM_TRACE
    file_path = sprintf('%s/signal/waveform%05d.txt', FILE_PATH, i);
    fileID1 = fopen(file_path,'w');

    groupObj = get(deviceObj, 'Waveform');
    groupObj = groupObj(1);

    while 1
        fprintf(arduino, '1');
        out = fscanf(arduino);
        input = out(1:37);
        output = out(38:74);
        [Y, T, XUNIT, YUNIT, HEADER] = invoke(groupObj, 'readwaveform', 'channel1');
        pause(DELAY_TIME);

        if ~isequal(ref, Y)
            ref = Y;
            fprintf(fileID3, '%s', input);
            fprintf(fileID2, '%s', output);
            fprintf(fileID1, '%d\n', Y);
            fprintf('Waveform %d captured.\n', i);
            break;
        else
            fprintf('Duplication Waveform captured!!!\n');
        end
    end
    
    fclose(fileID1);
    
end

fprintf('Waveform capture complete!!!\n');

fclose(fileID2);
disconnect(deviceObj);
delete([deviceObj interfaceObj]);
fclose(arduino);
delete(arduino);
