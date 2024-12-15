-- Extract data from CLDR data file languageInfo.xml
-- Relevant tags: <languageMatch desired="xx"	supported="yy"	distance="30"	oneway="true"/>

-- Assign script arguments and check validity
xmlFileName = "unicode/languageInfo.xml"
assert(xmlFileName ~= nil, "Name of XML file missing!")
sqlFileName = "temp/uni_loadmatching.sql"
assert(sqlFileName ~= nil, "Name of SQL file missing!")

-- Open files
xmlInput = assert(io.open(xmlFileName, "r"))
xmlData = xmlInput:read("*all")
sqlFile = assert(io.open(sqlFileName, "w"))

-- Write SQL file header
sqlFile:write("create table if not exists uni_languagematch (desired char, supported char, distance int, oneway char, primary key (desired, supported));\n")
sqlFile:write("delete from uni_languagematch;\n")
sqlFile:write("begin;\n")

-- Extract data from XML
-- Replace underscore by hyphen
for w in string.gmatch(xmlData, ".-<languageMatches.->(.-)</languageMatches>") do
  for desired,supported,distance,oneopt in string.gmatch(w, ".-<languageMatch desired=\"(.-)\".-supported=\"(.-)\".-distance=\"(.-)\"(.-)/>") do
    oneway = oneopt:match("oneway=\"(.-)\"")
    -- Check optional parameter oneway
    if oneway == nil then
      oneway = "false"
    end
    sqlFile:write("insert into uni_languagematch values ('" .. desired:gsub('_','-') .. "', '" .. supported:gsub('_','-') .. "', " .. distance .. ", '" .. oneway .. "');\n")
  end
end

-- Write SQL file trailer
sqlFile:write("commit;\n")

-- Close files
xmlInput:close()
sqlFile:close()
