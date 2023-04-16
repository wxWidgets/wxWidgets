-- Assign script arguments and check validity
codeFileName = "temp/langtabl_current.txt"

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

fo2 = io.open('temp/wx_loadlangtabl.sql','w')
--fo2:write('-- Known syonyms in wx\n-- Date: ' .. os.date("%Y-%m-%d %H:%M") .. '\n\n')
fo2:write("create table if not exists wx_langmap (wxlangname char, wxlangid char, wxlocregion, wxversion char, primary key (wxlangname));\n")
fo2:write('delete from wx_langmap;\nbegin;\n')

-- '%-55s %-14s %-25s %-14s %-4s %-4s %s "%s","%s"', wxname, locid, wxlocid, wxlocidold, idlang, idsublang, layout, englishdisplay, nativedisplayhex) from win_localetable order by wxname; 

count = 0
for line in io.lines(codeFileName) do
  wxlangname = trim(string.sub(line,1,55))
  wxversion = trim(string.sub(line,57,65))
  wxlangid =  trim(string.sub(line,82,106))
  wxlocregion =  trim(string.sub(line,108,121))

  fo2:write("insert into wx_langmap values ('" .. wxlangname .. "', '" .. wxlangid .. "', '" .. wxlocregion .. "', '" .. wxversion .. "');\n")
  count = count + 1
end
print("Number of languages =" .. count)

fo2:write('commit;\n')
fo2:close()
