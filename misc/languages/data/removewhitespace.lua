-- Assign script arguments and check validity
fileName = arg[1]
assert(fileName ~= nil, "Filename parameter missing!")

fileNameTemp = fileName .. ".tmp"

function trim(s)
   return s:match'^%s*(.*%S)' or ''
end

-- Remove whitespace from file
lastline = nil
prevlen = 0
fo2 = io.open(fileNameTemp,'wb')
for line in io.lines(fileName) do
  newline = trim(line)
  if lastline ~= nil then
    lastlen = string.len(lastline)
    if lastlen ~= 0 then
      fo2:write(lastline .. "\n")
    elseif prevlen ~=0 and lastlen == 0 then
      fo2:write("\n")
    end
    prevlen = lastlen
  end
  lastline = newline
end
if string.len(lastline) > 0 then
  fo2:write(lastline .. "\n")
end
fo2:close()

-- Remove original and replace it by the file with whitespace removed
os.remove(fileName)
os.rename(fileNameTemp, fileName)
