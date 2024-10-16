-- Retrieve current wxWidgets version
codeFileName = "../../../include/wx/version.h"

function trim(s)
   return s:match'^%s*(.*%S)' or ''
end

fo2 = io.open('temp/wx_loadversion.sql','w')
fo2:write('-- Current wxWidgets version\n-- Date: ' .. os.date("%Y-%m-%d %H:%M") .. '\n\n')
fo2:write("create table if not exists wx_version (version char, primary key (version));\n")
fo2:write('delete from wx_version;\nbegin;\n')

count = 0
for line in io.lines(codeFileName) do
  vkey = trim(string.sub(line,1,28))
  vval = trim(string.sub(line,30,35))
  if vkey == "#define wxMAJOR_VERSION" then
    vmajor = vval
  elseif vkey == "#define wxMINOR_VERSION" then
    vminor = vval
  elseif vkey == "#define wxRELEASE_NUMBER" then
    vrelno = vval
  elseif vkey == "#define wxSUBRELEASE_NUMBER" then
    vsubno = vval
  end
end
wxversion = vmajor .. "." .. vminor .. "." .. vrelno
print("  wxWidgets version number:", wxversion)
fo2:write("insert into wx_version values('" .. wxversion .. "');\n")

fo2:write('commit;\n')
fo2:close()
