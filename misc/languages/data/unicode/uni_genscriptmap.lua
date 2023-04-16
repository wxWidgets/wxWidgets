-- Generate a script map based on ISO 15924 data
--
-- Data source: Unicode
-- URL: https://www.unicode.org/iso15924/iso15924.txt
-- License: UNICODE, INC. LICENSE AGREEMENT - DATA FILES AND SOFTWARE

codeFileName = "unicode/iso15924.txt"

function split(str, character)
  result = {}

  index = 1
  for s in string.gmatch(str .. character, "(.-)"..character) do
    result[index] = s
    index = index + 1
  end

  return result
end

fo2 = io.open('temp/uni_loadscriptmap.sql','w')
fo2:write('-- Mapping of script codes based on ISO 15924\n-- Date: ' .. os.date("%Y-%m-%d %H:%M") .. '\n\n')
fo2:write('-- Data source: Unicode Org (https://www.unicode.org)\n')
fo2:write('-- URL: https://www.unicode.org/iso15924/iso15924.txt\n')
fo2:write('-- License: UNICODE, INC. LICENSE AGREEMENT - DATA FILES AND SOFTWARE\n\n')

fo2:write("create table if not exists uni_scriptmap (sctag char, scname char, scalias char, primary key (sctag));\n")
fo2:write('delete from uni_scriptmap;\nbegin;\n')

-- Names and order of data columns
-- Code|N°|English Name|Nom français|Alias|Age|Date

count = 0
for line in io.lines(codeFileName) do
  rem = string.sub(line,1,1)
  if rem ~= "#"  and rem ~= "" then
    values = split(line, ";")
    sctag = values[1]
    scnum = values[2]
    scname = values[3]
    scname_fr = values[4]
    scalias = values[5]
    scage = values[6]
    scdate = values[7]

    scname = scname:gsub("'", "''")
    if scalias == "" then
      scalias = "-"
    end

    fo2:write("insert into uni_scriptmap values ('" .. sctag .. "', '" .. scname .. "', '" .. scalias .. "');\n")
    count = count + 1
  end
end
print("Number of script mappings=" .. count)

fo2:write('commit;\n')
fo2:close()
