package.path = './?.lua;./?/init.lua'

require 'scales'
require 'messages'

context = { 
    note = 60;
    i = 0;
    time = 1;
    on = true;
    counter = 0;
}

function getnextevent(producer)

    local event = handlemessages(producer, context, print)
    if event then return event end
    if context.i >= 8 then
        context.i = 0
        print('lol')
        return nil 
    end
    if context.on then
        event = {
            time = context.time;
            command = commands.notebegin;
            channel = 0;
            notenumber = context.note + Scale.major[context.i];
            velocity = .9;
        }
        context.time = context.time + 1;
    else
        event = {
            time = context.time;
            command = commands.noteend;
            channel = 0;
            notenumber = context.note + Scale.major[context.i];
            velocity = .9;
        }
        context.i = context.i + 1
    end
    context.on = not context.on
    return event
end