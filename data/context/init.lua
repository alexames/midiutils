context = { }

context.note = 60
context.time = 0

context.sortedevents = { }
context.latestsortedevent = 0

context.pendingevents = { }
context.earliestpendingevent = 0

function context:pushevent(event)
    table.insert(self.pendingevents, event)
    if event.time < self.earliestpendingevent then
        self.earliestpendingevent = event.time
    end
end

function context:getnextevent()
    self.index = self.index + 1
    return self.sortedevents[self.index]
end

function context:fillnextmeasure()
    local rhythm = { 4/5, 4/5, 4/5, 4/5, 4/5 }
    for i = 0, 4 do
        local notebegin = {
            time = self.time;
            command = commands.notebegin;
            channel = 0;
            notenumber = self.note + Scale.major[i];
            velocity = .9;
        }
        local noteend = {
            time = self.time + 1;
            command = commands.noteend;
            channel = 0;
            notenumber = self.note + Scale.major[i];
            velocity = .9;
        }
        table.insert(self.pendingevents, notebegin)
        table.insert(self.pendingevents, noteend)
        self.time = self.time + rhythm[i+1]
    end
end

function context:prepareevents()
    self.index = 0
    self:fillnextmeasure()
    table.sort(self.pendingevents, function (e1, e2) return e1.time < e2.time end)
    self.sortedevents = self.pendingevents
    self.pendingevents = { }
end