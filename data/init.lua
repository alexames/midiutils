-- package.path = './?.lua;./?/init.lua'

-- require 'utils'
-- require 'scales'
-- require 'messages'
-- require 'context'

beat = 2 * 2 * 2 * 2 * 3 * 5 * 7 * 11

ticksperbeat = beat

print "init.lua"

local getsetup1
local getstart1
local getend1
local getsetup2
local getstart2
local getend2

notenumber = 60

function getsetup1()
  -- print "getsetup"
  getnextevent = getsetup2
  return {
    timedelta = beat;
    channel = 0;
    command = midi.commands.programchange;
    programnumber = midi.instruments.acousticgrand;
  }
end
function getsetup2()
  -- print "getsetup"
  getnextevent = getstart1
  return {
    timedelta = beat;
    channel = 1;
    command = midi.commands.programchange;
    programnumber = midi.instruments.electricgrand;
  }
end
function getstart1()
  -- print "getstart"
  getnextevent = getstart2
  return {
    timedelta = beat;
    command = midi.commands.notebegin;
    channel = 0;
    notenumber = 60;
    velocity = 64;
  }
end
c = 2
function getstart2()
  -- print "getstart"
  getnextevent = getend1
  b = beat * c
  c = 1
  return {
    timedelta = b;
    command = midi.commands.notebegin;
    channel = 1;
    notenumber = 60;
    velocity = 64;
  }
end
function getend1()
  -- print "getend"
  getnextevent = getend2
  return  {
    timedelta = beat;
    command = midi.commands.noteend;
    channel = 0;
    notenumber = 60;
    velocity = 64;
  }
end
function getend2()
  -- print "getend"
  getnextevent = getstart1
  return  {
    timedelta = beat;
    command = midi.commands.noteend;
    channel = 1;
    notenumber = 60;
    velocity = 64;
  }
end

getnextevent = getsetup1

function prepareevents()
end