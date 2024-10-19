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

.print Import likely subtags map
.read temp/uni_loadlikely.sql

.print Import matching subtags map
.read temp/uni_loadmatching.sql

.print Import region groups for main languages map
.read temp/uni_loadregiongroups.sql

.read temp/wx_loadversion.sql

.print Generate new tables (langtabl, scripttabl, synonymtabl, likelytabl, matchingtabl)
.read mkwxtables.sql
