-- Extract data from CLDR data file likelySubtags.xml
-- Relevant tags: <likelySubtag from="xx" to="xx_Xxxx_XX"/>

-- Assign script arguments and check validity
xmlFileName = "unicode/likelySubtags.xml"
sqlFileName = "temp/uni_loadlikely.sql"

-- Open files
xmlInput = assert(io.open(xmlFileName, "r"))
xmlData = xmlInput:read("*all")
sqlFile = assert(io.open(sqlFileName, "w"))

-- Write SQL file header
sqlFile:write("create table if not exists uni_likelysubtags (tagfrom char, tagto char, primary key (tagfrom));\n")
sqlFile:write("delete from uni_likelysubtags;\n")
sqlFile:write("begin;\n")

-- Extract data from XML
-- Replace underscore by hyphen
for w in string.gmatch(xmlData, "<likelySubtags>(.-)</likelySubtags>") do
  for tagFrom,tagTo in string.gmatch(w, ".-<likelySubtag from=\"(.-)\" to=\"(.-)\"") do
    sqlFile:write("insert into uni_likelysubtags values ('" .. tagFrom:gsub('_','-') .. "', '" .. tagTo:gsub('_','-') .. "');\n")
  end
end

-- Write SQL file trailer
sqlFile:write("commit;\n")

-- Close files
xmlInput:close()
sqlFile:close()
