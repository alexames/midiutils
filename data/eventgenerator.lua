require 'scales'
--require 'midi'

note = 60
i = 0
time = 100
on = true

counter = 0

commands = {}

function commands.changeinstrument(channel, instrumentname)
    return {
        time = time;
        command = command.programchange;
        channel = channel;
        programnumber = instrument[instrumentname]
    }
end

function getnextevent(producer)
    counter = counter + 1

    if counter >= 10 then
        counter = 0;
        return nil;
    else
        local message = producer:getnextmessage()
        if message then
            print("message", message)
            local cmd, param1, param2 = message:match('(%w+)%s+(%w+)%s+(%w+)')
            if cmd then
                if commands[cmd] then
                    print("running command")
                    return commands[cmd](param1, param2)
                else
                    print('unknown command ' .. cmd)
                end
            end
        end
        
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
end