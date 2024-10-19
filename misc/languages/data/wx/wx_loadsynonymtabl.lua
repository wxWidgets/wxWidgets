-- Assign script arguments and check validity
codeFileName = "temp/synonymtabl_current.txt"

function trim(s)
   return s:match'^%s*(.*%S)' or ''
end

function split(str, character)
  result = {}

  index = 1
-- (.-),
--  for s in string.gmatch(str, "[^"..character.."]+") do
  for s in string.gmatch(str .. character, "(.-)"..character) do
    result[index] = s
    index = index + 1
  end

  return result
end

fo2 = io.open('temp/wx_loadsynonymtabl.sql','w')
fo2:write('-- Known syonyms in wx\n-- Date: ' .. os.date("%Y-%m-%d %H:%M") .. '\n\n')
fo2:write("create table if not exists wx_synonyms (wxnamesyn char, wxnameprim char, winlocid char, wxversion char, primary key (wxnamesyn));\n")
fo2:write('delete from wx_synonyms;\nbegin;\n')

-- Code|N°|English Name|Nom français|Alias|Age|Date

count = 0
for line in io.lines(codeFileName) do
  rem = string.sub(line,1,1)
  if rem ~= "#" then
    wxnamesyn = trim(string.sub(line,1,55))
    wxnameprim = trim(string.sub(line,57,111))
    winlocid =  trim(string.sub(line,113,126))
    wxversion =  trim(string.sub(line,128,136))

    fo2:write("insert into wx_synonyms values ('" .. wxnamesyn .. "', '" .. wxnameprim .. "', '" .. winlocid .. "', '" .. wxversion .. "');\n")
    count = count + 1
  end
end
print("  Number of synonyms = " .. count)

fo2:write('commit;\n')
fo2:close()
