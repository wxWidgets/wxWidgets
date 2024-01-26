.print Initialize database
.print Import territory codes
.read temp/uni_loadterritorycodes.sql

.print Import locales supported by Windows
.read temp/win_loadlocaletable.sql

.print Import wxWidgets language list
.read temp/wx_loadlangtabl.sql

.print Import wxWidgets language synonyms
.read temp/wx_loadsynonymtabl.sql

.print Import script map
.read temp/uni_loadscriptmap.sql

.read temp/wx_loadversion.sql

.print Generate new tables (langtabl, scripttabl, synonymtabl)
.read mkwxtables.sql
