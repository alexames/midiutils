package.cpath = '../bin/Debug/?.dll;' .. package.cpath

require 'luamidiutils'

--midifile = lmu.MidiFile.new()
--lmu.readfile(midifile, 'example.mid')
--producer = lmu.MidiFileEventProducer.new(midifile)

producer = lmu.LuaEventProducer.new('eventgenerator.lua')

stream = lmu.MidiStream.new(producer)
stream:play()
