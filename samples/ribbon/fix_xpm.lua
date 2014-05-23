#!/usr/bin/lua
-------------------------------------------------------------------------------
-- Name:        fix_xpm.lua
-- Purpose:     Fix XPM files for use in Ribbon sample
-- Author:      Peter Cawley
-- Modified by:
-- Created:     2009-07-06
-- Copyright:   (C) Copyright 2009, Peter Cawley
-- Licence:     wxWindows Library Licence
-------------------------------------------------------------------------------
-- My preferred image editor (Paint Shop Pro 9) spits out XPM files, but with
-- some deficiencies:
-- 1) Specifies a 256 colour palette, even when less than 256 colours are used
-- 2) Transparency is replaced by a non-transparent colour
-- 3) Does not name the C array appropriately
-- 4) Array and strings not marked const

assert(_VERSION == "Lua 5.1", "Lua 5.1 is required")
local lpeg = require "lpeg"

-- Parse command line
local args = {...}
local filename = assert(...,"Expected filename as first command line argument")
local arg_transparent
local arg_name
local arg_out
for i = 2, select('#', ...) do
  local arg = args[i]
  if arg == "/?" or arg == "-?" or arg == "--help" then
    print("Usage: filename [transparent=<colour>|(x,y)] [name=<array_name>] "..
          "[out=<filename>]")
    print("In addition to the transparent colour and name changes, the "..
          "palette will be also be optimised")
    print "Examples:"
    print("  in.xpm transparent=Gray100 -- Modifies in.xpm, replacing "..
          "Gray100 with transparent")
    print("  in.xpm transparent=(0,0) -- Modifies in.xpm, replacing "..
          "whichever colour is at (0,0) with transparent")
    print("  in.xpm name=out_xpm out=out.xpm -- Copies in.xpm to out.xpm, "..
          "and changes the array name to out_xpm")
    return
  end
  arg_transparent = arg:match"transparent=(.*)" or arg_transparent
  arg_name = arg:match"name=(.*)" or arg_name
  arg_out = arg:match"out=(.*)" or arg_out
end

-- XPM parsing
local grammars = {}
do
  local C, P, R, S, V = lpeg.C, lpeg.P, lpeg.R, lpeg.S, lpeg.V
  local Ct = lpeg.Ct
  local comment = P"/*" * (1 - P"*/") ^ 0 * P"*/"
  local ws = (S" \r\n\t" + comment) ^ 0
  local function tokens(...)
    local result = ws
    for i, arg in ipairs{...} do
      if type(arg) == "table" then
        arg = P(arg[1]) ^ -1
      end
      result = result * P(arg) * ws
    end
    return result
  end
  grammars.file = P { "xpm";
    xpm = P"/* XPM */" * ws *
         tokens("static",{"const"},"char","*",{"const"}) * V"name" *
         tokens("[","]","=","{") * V"lines",
    name = C(R("az","AZ","__") * R("az","AZ","09","__") ^ 0),
    lines = Ct(V"line" ^ 1),
    line = ws * P'"' * C((1 - P'"') ^ 0) * P'"' * (tokens"," + V"eof"),
    eof = tokens("}",";") * P(1) ^ 0,
  }
  grammars.values = P { "values";
    values = Ct(V"value" * (S" \r\n\t" ^ 1 * V"value") ^ 3),
    value = C(R"09" ^ 1) / tonumber,
  }
  function make_remaining_grammars(cpp)
    local char = R"\32\126" - S[['"\]] -- Most of lower ASCII
    local colour = char
    for i = 2, cpp do
      colour = colour * char
    end
    grammars.colour = P { "line";
      line = C(colour) * Ct(Ct(ws * V"key" * ws * V"col") ^ 1),
      key = C(P"g4" + S"msgc"),
      col = V"name" + V"hex",
      name = C(R("az","AZ","__") * R("az","AZ","09","__") ^ 0),
      hex = C(P"#" * R("09","af","AF") ^ 3),
    }
    grammars.pixels = P { "line";
      line = Ct(C(colour) ^ 1),
    }
  end
end

-- Load file
local file = assert(io.open(filename,"rt"))
local filedata = file:read"*a"
file:close()

local xpm = {}
xpm.name, xpm.lines = grammars.file:match(filedata)
local values_table = assert(grammars.values:match(xpm.lines[1]))
xpm.width, xpm.height, xpm.ncolours, xpm.cpp = unpack(values_table)
make_remaining_grammars(xpm.cpp)
xpm.colours = {}
xpm.colours_full = {}
for i = 1, xpm.ncolours do
  local name, data = grammars.colour:match(xpm.lines[1 + i])
  local colour = ""
  for _, datum in ipairs(data) do
    if datum[1] == "c" then
      colour = datum[2]
      break
    end
  end
  assert(colour, "No colour data for " .. name)
  xpm.colours[name] = colour
  xpm.colours_full[i] = {name = name, unpack(data)}
end
xpm.pixels = {}
for y = 1, xpm.height do
  xpm.pixels[y] = grammars.pixels:match(xpm.lines[1 + xpm.ncolours + y])
  if not xpm.pixels[y] or #xpm.pixels[y] ~= xpm.width then
    error("Line " .. y .. " is invalid")
  end
end

-- Fix palette
repeat
  local n_colours_used = 0
  local colours_used = setmetatable({}, {__newindex = function(t, k, v)
    n_colours_used = n_colours_used + 1
    rawset(t, k, v)
  end})
  for y = 1, xpm.height do
    for x = 1, xpm.width do
      colours_used[xpm.pixels[y][x]] = true
    end
  end
  if n_colours_used == xpm.ncolours then
    break
  end
  local chars =" .abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567"
  local cpp = (n_colours_used > #chars) and 2 or 1
  local nalloc = 0
  local colour_map = setmetatable({}, {__index = function(t, k)
    nalloc = nalloc + 1
    local v
    if cpp == 1 then
      v = chars:sub(nalloc, nalloc)
    else
      local a, b = math.floor(nalloc / #chars) + 1, (nalloc % #chars) + 1
      v = chars:sub(a, a) .. chars:sub(b, b)
    end
    t[k] = v
    return v
  end})
  for y = 1, xpm.height do
    for x = 1, xpm.width do
      xpm.pixels[y][x] = colour_map[xpm.pixels[y][x]]
    end
  end
  local new_colours_full = {}
  for i, colour in ipairs(xpm.colours_full) do
    if colours_used[colour.name] then
      colour.name = colour_map[colour.name]
      new_colours_full[#new_colours_full + 1] = colour
    end
  end
  xpm.colours_full = new_colours_full
  local new_colours = {}
  for name, value in pairs(xpm.colours) do
    if colours_used[name] then
      new_colours[colour_map[name]] = value
    end
  end
  xpm.colours = new_colours
  xpm.cpp = cpp
  xpm.ncolours = nalloc
until true

-- Fix transparency
if arg_transparent then
  local name
  local x, y = arg_transparent:match"[(](%d+),(%d+)[)]"
  if x and y then
    name = xpm.pixels[y + 1][x + 1]
  else
    for n, c in pairs(xpm.colours) do
      if c == arg_transparent then
        name = n
        break
      end
    end
  end
  if not name then
    error("Cannot convert " .. arg_transparent .. " to transparent as the "..
          "colour is not present in the file")
  end
  xpm.colours[name] = "None"
  for i, colour in ipairs(xpm.colours_full) do
    if colour.name == name then
      for i, data in ipairs(colour) do
        if data[1] == "c" then
          data[2] = "None"
          break
        end
      end
      break
    end
  end
end

-- Fix name
xpm.name = arg_name or xpm.name

-- Save
local file = assert(io.open(arg_out or filename, "wt"))
file:write"/* XPM */\n"
file:write("static const char *const " .. xpm.name .. "[] = {\n")
file:write(('"%i %i %i %i",\n'):format(xpm.width, xpm.height, xpm.ncolours,
                                       xpm.cpp))
for _, colour in ipairs(xpm.colours_full) do
  file:write('"' .. colour.name)
  for _, data in ipairs(colour) do
    file:write(" " .. data[1] .. " " .. data[2])
  end
  file:write('",\n')
end
for i, row in ipairs(xpm.pixels) do
  file:write('"' .. table.concat(row) .. (i == xpm.height and '"\n' or '",\n'))
end
file:write("};\n")
