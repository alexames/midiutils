
Scale = {}
function Scale.new(interval)
    local obj = {}
    local meta = {}
    local len = #interval
    local octave = 0
    for i, v in ipairs(interval) do
        octave = octave + v
    end
    function obj.length()
        return len
    end
    function obj.octave()
        return octave
    end
    function meta.__index(self, index)
        local value = 0
        local count = 0
        while index < 0 do
            index = index + len
            count = count - 1
        end
        while index >= len do
            index = index - len
            count = count + 1
        end

        value = octave * count;

        local normalizedindex = math.fmod(index, len)
        for i = 1, normalizedindex do
            value = value + interval[i]
        end

        return value
    end
    setmetatable(obj, meta)
    return obj
end

Scale.major = Scale.new{ 2, 2, 1, 2, 2, 2, 1 }

return Scale