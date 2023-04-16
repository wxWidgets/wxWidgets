-- Create unique wx language mapping
create table if not exists wx_langmapuni (wxlangname char, wxlangid char, wxlocregion char, primary key (wxlangname));
delete from wx_langmapuni;
insert into wx_langmapuni select wxlangname, wxlangid, wxlocregion from wx_langmap;

-- Create mapping between wx and Windows ids
create table if not exists wx2winmapping (wxlocidnew char, wxnamenew char, wxlocidold char, wxnameold char, primary key (wxlocidnew));
delete from wx2winmapping;

insert into wx2winmapping select wt.wxlocid, wt.wxname, ifnull(wm.wxlangid,'-'), ifnull(wm.wxlangname,'-') from win_localetable wt
  left join wx_langmapuni wm on wt.wxlocid=wm.wxlangid;

update wx2winmapping set wxlocidold = ifnull((select wxlangid from wx_langmapuni where wx_langmapuni.wxlangname=wx2winmapping.wxnamenew),'-') where wx2winmapping.wxlocidold='-';

-- Create mapping between language and country
create table if not exists lang2iso2 (lang char, iso2 char, wintag char, primary key (lang));
delete from lang2iso2;
insert into lang2iso2 select w.locid, w.locid||'_'||c.iso2, w.locid||'-'||c.iso2 from win_localetable w, uni_territorycodes c where w.locid not like '%-%' and w.country3=c.iso3;

-- Perform checks
.output temp/wxmissing.log
select '*** Missing via wx_langmapuni';
select wxlangname, wxlangid from wx_langmapuni where not exists (select wxnameold from wx2winmapping where wxnameold=wxlangname);
select '*** Missing via wx_langmap';
select wxlangname, wxlangid from wx_langmap where not exists (select wxname from win_localetable where wxname=wxlangname);
.output stdout

select '  #missing wxlangmapuni =', count(wxlangname) from wx_langmapuni where not exists (select wxnameold from wx2winmapping where wxnameold=wxlangname);
select '  #matched wxlangmapuni =', count(wxlangname) from wx_langmapuni where exists (select wxnameold from wx2winmapping where wxnameold=wxlangname);
select '  #matched wx2win       =', count(distinct wxnameold) from wx2winmapping where wxnameold <> '-';

select '  #missing wxlangmap    =', count(wxlangname) from wx_langmap where not exists (select wxname from win_localetable where wxname=wxlangname) and not exists (select wxnamesyn from wx_synonyms where wxnamesyn=wxlangname);
select wxlangname from wx_langmap where not exists (select wxname from win_localetable where wxname=wxlangname) and not exists (select wxnamesyn from wx_synonyms where wxnamesyn=wxlangname);

select wxname from win_localetable where exists (select wxnamesyn from wx_synonyms where wxnamesyn = win_localetable.wxname);

-- Generate references to previous wx mappings
update win_localetable set (wxlocidold, wxnameold) =
  (select wxlocregion, wxlangname from wx_langmap m1 where m1.wxlangid=win_localetable.wxlocid);

-- Replace NULL values with strings ('-')
update win_localetable set wxlocidold = '-' where wxlocidold is null;
update win_localetable set wxnameold = '-' where wxnameold is null;

-- Adjust Windows language ids
--.changes on
update win_localetable as t1 set (idlang,idsublang) =
    (t2.idlang, t2.idsublang) from win_localetable t2
    where t1.wxlocid <> '-' and t2.wxlocid = t1.wxlocidold and (t1.idlang <> t2.idlang or t1.idsublang <> t2.idsublang);
--.changes off

-- Adjust canonical reference for language-only entries
update win_localetable as w1 set wxlocidold =
  (select m.iso2 from win_localetable w, lang2iso2 m
    where w.locid=w1.locid and w.locid=m.lang
      and exists (select 1 from win_localetable w2 where w2.locid=m.wintag))
  where w1.locid not like '%-%' and (w1.wxlocidold='-' or w1.wxlocidold is null);

update win_localetable set wxlocidold = '-' where wxlocidold is null;

-- Initialize parameter holding the current wx version
.parameter init
.print Set wx version parameter
.parameter set @wxversion "(select version from wx_version)"
.parameter list

-- Generate new list of languages
.output langtabl.txt
select printf('%-55s %-9s %-14s %-25s %-14s %-4s %-4s %s "%s","%s"',
              w.wxname, ifnull(x.wxversion,@wxversion), w.locid, w.wxlocid, w.wxlocidold,
              w.idlang, w.idsublang, w.layout, w.englishdisplay, w.nativedisplayhex)
  from win_localetable w left join wx_langmap x on w.wxlocid = x.wxlangid order by w.wxname;
.output stdout

-- Generate new list of script mappings
.output scripttabl.txt
select printf('%-4s %s', sctag, lower(scalias)) from uni_scriptmap where scalias <> '-' order by sctag;
.output stdout

-- Generate new list of wxLANGUAGE synonyms
insert into wx_synonyms
  select wl.wxnameold, wl.wxname, wl.locid, ifnull(wm.wxversion,@wxversion)
    from win_localetable wl left join wx_langmap wm on wl.wxnameold = wm.wxlangname
    where wl.wxnameold <> '-' and wl.wxname <> wl.wxnameold;
delete from wx_synonyms where wxnamesyn in (select wxname from win_localetable);
.output synonymtabl.txt
select printf('%-55s %-55s %-14s %s', wxnamesyn, wxnameprim, winlocid, wxversion) from wx_synonyms order by wxnamesyn;
.output stdout
