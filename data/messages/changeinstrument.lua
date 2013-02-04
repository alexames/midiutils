function messages.changeinstrument(context, channel, nameorid)
    print 'changing instument!'
    context:pushevent
    {
        time = context.sortedevents[#context.sortedevents].time;
        command = commands.programchange;
        channel = channel;
        programnumber = instruments[nameorid];
    }
end