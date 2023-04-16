-- Assign script arguments and check validity
codeFileName = "temp/langtabl_current.txt"

function trim(s)
   return s:match'^%s*(.*%S)' or ''
end

fo2 = io.open('temp/wx_loadlangtabl.sql','w')
fo2:write("create table if not exists wx_langmap (wxlangname char, wxlangid char, wxlocregion, wxversion char, primary key (wxlangname));\n")
fo2:write('delete from wx_langmap;\nbegin;\n')

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
