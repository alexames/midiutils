package.cpath = '../bin/Debug/?.dll;' .. package.cpath

require 'luamidiutils'

--midifile = lmu.MidiFile.new()
--lmu.readfile(midifile, 'example.mid')
--prod = lmu.MidiFileEventProducer.new(midifile)

prod = lmu.LuaEventProducer.new('eventgenerator.lua')

stream = lmu.MidiStream.new(prod)
stream:play()
