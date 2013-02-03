function messages.changeinstrument(context, channel, nameorid)
    return {
        time = context.time;
        command = commands.programchange;
        channel = channel;
        programnumber = instruments[nameorid];
    }
end