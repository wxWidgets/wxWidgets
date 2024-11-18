-- Extract data from CLDR data file supplementalData.xml and languageInfo.xml
-- Relevant tags in supplementalData:
--   <territoryContainment>
--     <group type="nnn" contains="aaa bbb cccc ..."/>
-- Relevant tags in languageInfo:
--   <matchVariable id="$yyy" value="zzz"/>
--   <languageMatch desired="xx_*_$yyy"	[...] />

-- Assign script arguments and check validity
xmlFileName1 = "unicode/supplementalData.xml"
xmlFileName2 = "unicode/languageInfo.xml"
sqlFileName = "temp/uni_loadregiongroups.sql"

-- Open files
xmlInput1 = assert(io.open(xmlFileName1, "r"))
xmlData1 = xmlInput1:read("*all")

xmlInput2 = assert(io.open(xmlFileName2, "r"))
xmlData2 = xmlInput2:read("*all")

sqlFile = assert(io.open(sqlFileName, "w"))

-- Extract data from XML
-- Replace underscore by hyphen

local regions = {}

-- Extract territory containments
print("  -> Extract territory containments")
for w in string.gmatch(xmlData1, "<territoryContainment>(.-)</territoryContainment>") do
  for grtype,grcontains,grother in string.gmatch(w, "<group type=\"(.-)\" contains=\"(.-)\"(.-)/>") do
    -- Extract value of status attribute
    status = grother:match("status=\"(.-)\"")
    if status == nil then
      status = ""
    end

    -- Extract value of grouping attribute
    grouping = grother:match("grouping=\"(.-)\"")
    if grouping == nil then
      grouping = "false"
    end

    -- Use only entries with status NOT equal to "deprecated" or "grouping"
    if status ~= "deprecated" and status ~= "grouping" then
      regions[grtype] = grcontains
    end
  end
end

-- Function for building the country list of a region
function addcountries(regions, countries, countrylist)
  local nn = #countries
  for id in countrylist:gmatch("%w+") do
    region = regions[id]
    if region ~= nil then
      addcountries(regions, countries, regions[id])
    else
      nn = #countries + 1
      countries[nn] = id
    end
  end
end

-- Extract matching region
print("  -> Extract languages and matching regions")
local languages = {}
local matchids = {}
for w in string.gmatch(xmlData2, ".-<languageMatches.->(.-)</languageMatches>") do
  for matchVarId,matchVarValue in string.gmatch(w, "<matchVariable id=\"(.-)\".-value=\"(.-)\"") do
    matchVarValue = matchVarValue:gsub('+',' ')
    matchVarValue = matchVarValue:gsub('-',' ')
--    print("Id=" .. matchVarId .. ", " .. matchVarValue)
    matchids[matchVarId] = matchVarValue
    regions[matchVarId] = matchVarValue
    for langMatch in string.gmatch(w, ".-<languageMatch desired=\"(.-)\".-/>") do
      found = string.find(langMatch, matchVarId)
      if found ~= nil then
        uscorestar = string.find(langMatch, "%_%*")
        if uscorestar ~= nil then
          language = string.sub(langMatch,1,uscorestar-1)
        else
          language = string.sub(langMatch,1,found-2)
        end
        language = language:gsub('_','-')
        languages[language] = matchVarId
        print("    Language: " .. language, "Region: " .. string.sub(matchVarId,2))
      end
    end
  end
end

-- Extract territory containments
local regionlist = {}
print('  -> Extract (and  expand) regions')
for k,v in pairs(regions) do
  regionlist[k] = {}
  addcountries(regions, regionlist[k], v)
  print("    Region:", k, " Size:", #regionlist[k])
-- Print for debugging if necessary
--  for _, cid in ipairs(regionlist[k]) do
--    io.write(cid .. " ")
--  end
--  io.write("\n")
end

print("  -> Generate SQL file")
-- Write SQL file header
sqlFile:write("create table if not exists uni_regiongroups (language char, country char, primary key(language, country));\n")
sqlFile:write("delete from uni_regiongroups;\n")
sqlFile:write("begin;\n")

-- Sort language keys
local tkeys = {}
for k in pairs(languages) do
  table.insert(tkeys, k)
end
table.sort(tkeys)

-- Write data to SQL file
for _, k in ipairs(tkeys) do
  v = languages[k]
  sqlFile:write("\n-- Region group '" .. string.sub(v,2) .. "' for language '" .. k .. "'\n")
  for _, cid in ipairs(regionlist[v]) do
    sqlFile:write("insert into uni_regiongroups values ('" .. k .. "', '" .. cid .. "');\n")
  end
end

-- Write SQL file trailer
sqlFile:write("commit;\n")

-- Close files
xmlInput1:close()
xmlInput2:close()
sqlFile:close()
