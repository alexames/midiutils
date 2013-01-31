
Midi = {}
Midi.metatable = {}
Midi.metatable.__index = Midi.metatable

function Midi.new()
    local obj = {}
    setmetatable(obj, Midi.metatable)
    obj.tracks = {}
    return obj
end

Track = {}
Track.metatable = {}
Track.metatable.__index = Track.metatable

function Track.new()
    local obj = {}
    setmetatable(obj, Midi.metatable)
    return obj
end
