require 'scales'
--require 'midi'

note = 60
i = 0
time = 100
on = true

function getnextevent()
    local event
    if on then
        event = {
            time = time;
            command = command.notebegin;
            channel = 0;
            notenumber = note + Scale.major[i];
            velocity = .9;
        }
        time = time + 100;
    else
        event = {
            time = time;
            command = command.noteend;
            channel = 0;
            notenumber = note + Scale.major[i];
            velocity = .9;
        }
        i = i + 1
        if i >= 8 then i = 0 end
    end
    on = not on
    return event
end