function root = read_sim_output(config, output_file, debug)
%READ_SIM_OUTPUT Read a simulation output binary file
%   READ_SIM_OUTPUT(CONFIG, OUTPUT_FILE) reads the simulation output file
%   OUTPUT_FILE and returns a data structure organized according to the
%   telemetry configuration file CONFIG
%
%   READ_SIM_OUTPUT(CONFIG, OUTPUT_FILE, DEBUG) Additionally prints debug
%   statements
%
%   Jason Fernandez 6/17/2018

fid = fopen(config);
if fid == -1
    error('unable to open %s\n', filename)
end

cfgs   = struct('path', cell(1, 1000), ...
                'freq', cell(1, 1000), ...
                'type', cell(1, 1000));
n_cfgs = 0;

% The number of bytes of data per time step
sample_size = 0;

% If true, print debug info
dbg = false;
if nargin > 2
    dbg = debug;
end

while ~feof(fid)
    tline = fgetl(fid);
    if ~ischar(tline), break, end
    
    ind = strfind( tline,'#' );
    if ~isempty(ind)
        tline = tline(1:ind-1);
    end
    
    tline = strtrim( tline );
    if isempty(tline)
        continue
    end
    
    C=textscan(tline, '%s%d%s');
    
    n_cfgs = n_cfgs + 1;
    
    path = strrep(C{1}{1}, '.', '_');

    cfgs(n_cfgs).path = path;
    cfgs(n_cfgs).freq = C{2};

    if strcmp(C{3}{1}, 'float')
        cfgs(n_cfgs).type = 'single';
    else
        cfgs(n_cfgs).type =  C{3}{1};
    end
    
    sample_size = sample_size + ...
        class2size(C{3}{1});
end

fclose(fid);

if (dbg)
    fprintf('Record is %d bytes\n', ...
        sample_size);
end

fid = fopen(output_file);
if fid == -1
    error('unable to open %s\n', output_file)
end

% Preallocate space for each variable

s = dir(output_file);
n_samples = s.bytes / sample_size;

if dbg
    fprintf(1, 'Reading %d samples\n', n_samples);
end

data = cell(1, n_cfgs);

for i = 1:n_cfgs
    data{i} = zeros(1, n_samples);
end

% Load the data:

prev_percent = 0;

f = waitbar(0);

for n = 1:n_samples
    for i = 1:n_cfgs
        data{i}(n) = fread(fid, 1, cfgs(i).type);
    end
    
    frac = n / n_samples;
    percent = frac * 100;
    
    if percent >= prev_percent + 1
        prev_percent = percent;
        
        msg = sprintf( 'Loading %d/%d ...', n, n_samples );
        waitbar( frac, f, msg );
    end
end

close(f);

fclose(fid);

root = struct;

for i = 1:n_cfgs
    
    % Extract the variable name, which includes the numeric
    % suffix
    
    tokens = strsplit(cfgs(i).path, '/');
    var    = tokens{end};
    
    % Create the data structure
    
    path = strjoin(tokens(1:end-1), '.');
    
    if ~isempty(path)
        eval(['root.', path, '.', var, ' = data{i};'])
    else
        eval(['root.', var, ' = data{i};'])
    end
end

end

% ---------------------------------------------------------
% A helper function which maps a data type to its
% width, in bytes
% ---------------------------------------------------------
function y = class2size(type)

    c2b = containers.Map( ...
        {'double'; 'single'; 'int8'  ; 'int16' ; 'int32';
         'int64' ; 'uint8' ; 'uint16'; 'uint32'; 'uint64'}, ...
        [   8;         4;       1;         2;        4; 
            8;         1;       2;         4;        8]);

    y = c2b(type);
end
