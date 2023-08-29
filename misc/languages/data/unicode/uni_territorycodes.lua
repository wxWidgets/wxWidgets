-- Generate a script map based on ISO 15924 data
--
-- Data source: Unicode CLDR
-- URL: https://github.com/unicode-org/cldr/raw/main/tools/cldr-code/src/main/resources/org/unicode/cldr/util/data/territory_codes.txt
-- License: UNICODE, INC. LICENSE AGREEMENT - DATA FILES AND SOFTWARE

codeFileName = "unicode/territory_codes.txt"

function split(str)
  result = {}

  index = 1
  for s in string.gmatch(str, "%S+") do
    result[index] = s
    index = index + 1
  end

  return result
end

fo1 = io.open('temp/uni_loadterritorycodes.sql','w')
fo1:write('-- List of territory codes based on ISO 3166\n-- Date: ' .. os.date("%Y-%m-%d %H:%M") .. '\n\n')
fo1:write('-- Data source: Unicode CLDR (https://cldr.unicode.org)\n')
fo1:write('-- File: https://github.com/unicode-org/cldr/raw/main/tools/cldr-code/src/main/resources/org/unicode/cldr/util/data/territory_codes.txt\n')
fo1:write('-- License: UNICODE, INC. LICENSE AGREEMENT - DATA FILES AND SOFTWARE\n\n')

fo1:write("create table if not exists uni_territorycodes (iso2 char, iso3 char, primary key (iso2));\n")
fo1:write('delete from uni_territorycodes;\nbegin;\n')

count = 0
ccprev = ""
for line in io.lines(codeFileName) do
  rem = string.sub(line,1,1)
  if rem ~= "#" then
    values = split(line)

    -- 1 RFC3066
    -- 2 UN Numeric
    -- 3 ISO3166 Alpha-3
    -- 4 internet
    -- 5 FIPS-10

    iso2 = values[1]
    iso3 = values[3]

    if iso3 ~= nil then
      fo1:write("insert into uni_territorycodes values ('" .. iso2 .. "', '" .. iso3 .. "');\n")
    else
      print('*** Warning: no iso3 code for ' .. iso2)
    end
    count = count + 1
  else
    if line == "# Deprecated codes" then
      break
    end
  end
end
print("Number of countries=" .. count)

fo1:write('commit;\n')
fo1:close()
