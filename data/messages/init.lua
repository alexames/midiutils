messages = { }

function handlemessages(producer, context)
    local message = producer:getnextmessage()
    if message then
        local cmd, param1, param2 = message:match('(%w+)%s+(%w+)%s+(%w+)')
        if cmd then
            if messages[cmd] then
                return messages[cmd](context, param1, param2)
            end
        end
    end
end

require 'messages/changeinstrument'
