
note = 80

gnote = {
        timedelta = 1000;
        command = command.notebegin;
        channel = 0;
        notenumber = note;
        velocity = 100;
    };

i = 5

function getnextevent(producer)
    note = note + 1
    if note > 110 then note = 80 end
    return {
        timedelta = 100;
        command = command.notebegin;
        channel = 0;
        notenumber = note;
        velocity = 100;
    }
end