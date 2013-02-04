package.path = './?.lua;./?/init.lua'

require 'utils'
require 'scales'
require 'messages'
require 'context'

function getnextevent(context)
    return context:getnextevent()
end

function prepareevents(context)
    context:prepareevents()
end