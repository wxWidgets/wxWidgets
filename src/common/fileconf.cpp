///////////////////////////////////////////////////////////////////////////////
// Name:        fileconf.cpp
// Purpose:     implementation of wxFileConfig derivation of wxConfig
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     07.04.98 (adapted from appconf.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Karsten Ballüder   &  Vadim Zeitlin
//                       Ballueder@usa.net     <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include  "wx/wxprec.h"

#ifdef    __BORLANDC__
  #pragma hdrstop
#endif  //__BORLANDC__

#ifndef   WX_PRECOMP
  #include  <wx/string.h>
  #include  <wx/intl.h>
#endif  //WX_PRECOMP

#include  <wx/dynarray.h>
#include  <wx/file.h>
#include  <wx/log.h>
#include  <wx/textfile.h>
#include  <wx/config.h>

#ifdef __WINDOWS__
#include <windows.h>
#endif

#include <stdlib.h>
#include <ctype.h>

// ----------------------------------------------------------------------------
// global functions declarations
// ----------------------------------------------------------------------------

// is 'c' a valid character in group name?
// NB: APPCONF_IMMUTABLE_PREFIX and APPCONF_PATH_SEPARATOR must be valid chars,
//     but _not_ ']' (group name delimiter)
inline bool IsValid(char c) { return isalnum(c) || strchr("_/-!.*%", c); }

// get the system wide and user configuration file full path
static const char *GetGlobalFileName(const char *szFile);
static const char *GetLocalFileName(const char *szFile);

// split path into parts removing '..' in progress
static void SplitPath(wxArrayString& aParts, const char *sz);

// filter strings
static wxString FilterIn(const wxString& str);
static wxString FilterOut(const wxString& str);

// ----------------------------------------------------------------------------
// wxFileConfig
// ----------------------------------------------------------------------------

/*
  FileConfig derives from BaseConfig and implements file based config class, 
  i.e. it uses ASCII disk files to store the information. These files are
  alternatively called INI, .conf or .rc in the documentation. They are 
  organized in groups or sections, which can nest (i.e. a group contains
  subgroups, which contain their own subgroups &c). Each group has some
  number of entries, which are "key = value" pairs. More precisely, the format 
  is:

  # comments are allowed after either ';' or '#' (Win/UNIX standard)

  # blank lines (as above) are ignored

  # global entries are members of special (no name) top group
  written_for = wxWindows
  platform    = Linux

  # the start of the group 'Foo'
  [Foo]                           # may put comments like this also
  # following 3 lines are entries
  key = value
  another_key = "  strings with spaces in the beginning should be quoted, \
                   otherwise the spaces are lost"
  last_key = but you don't have to put " normally (nor quote them, like here)

  # subgroup of the group 'Foo'
  # (order is not important, only the name is: separator is '/', as in paths)
  [Foo/Bar]
  # entries prefixed with "!" are immutable, i.e. can't be changed if they are
  # set in the system-wide config file
  !special_key = value
  bar_entry = whatever

  [Foo/Bar/Fubar]   # depth is (theoretically :-) unlimited
  # may have the same name as key in another section
  bar_entry = whatever not

  You have {read/write/delete}Entry functions (guess what they do) and also
  setCurrentPath to select current group. enum{Subgroups/Entries} allow you
  to get all entries in the config file (in the current group). Finally,
  flush() writes immediately all changed entries to disk (otherwise it would
  be done automatically in dtor)

  FileConfig manages not less than 2 config files for each program: global
  and local (or system and user if you prefer). Entries are read from both of
  them and the local entries override the global ones unless the latter is
  immutable (prefixed with '!') in which case a warning message is generated
  and local value is ignored. Of course, the changes are always written to local
  file only.
*/

class wxFileConfig : public wxConfig
{
public:
  // ctor & dtor
    // the config file is searched in the following locations
    //            global                local
    // Unix   /etc/file.ext           ~/.file
    // Win    %windir%\file.ext   %USERPROFILE%\file.ext
    //
    // where file is the basename of strFile, ext is it's extension
    // or .conf (Unix) or .ini (Win) if it has none
  wxFileConfig(const wxString& strFile, bool bLocalOnly = FALSE);
    // dtor will save unsaved data
  virtual ~wxFileConfig();

  // implement inherited pure virtual functions
  virtual void SetPath(const wxString& strPath);
  virtual const wxString& GetPath() const { return m_strPath; }

  virtual bool GetFirstGroup(wxString& str, long& lIndex);
  virtual bool GetNextGroup (wxString& str, long& lIndex);
  virtual bool GetFirstEntry(wxString& str, long& lIndex);
  virtual bool GetNextEntry (wxString& str, long& lIndex);

  virtual const char *Read(const char *szKey, const char *szDefault = 0) const;
  virtual long        Read(const char *szKey, long lDefault) const;
  virtual bool Write(const char *szKey, const char *szValue);
  virtual bool Write(const char *szKey, long Value);
  virtual bool Flush(bool bCurrentOnly = FALSE);

  virtual bool DeleteEntry(const char *szKey, bool bGroupIfEmptyAlso);
  virtual bool DeleteGroup(const char *szKey);
  virtual bool DeleteAll();

public:
  // fwd decl
  class ConfigGroup;
  class ConfigEntry;

  // we store all lines of the local config file as a linked list in memory
  class LineList
  {
  public:
    // ctor
    LineList(const wxString& str, LineList *pNext = NULL) : m_strLine(str) 
      { SetNext(pNext); }
    
    // 
    LineList *Next() const              { return m_pNext;  }
    void      SetNext(LineList *pNext)  { m_pNext = pNext; }

    //
    void SetText(const wxString& str) { m_strLine = str;  }
    const wxString& Text() const      { return m_strLine; }

  private:
    wxString  m_strLine;      // line contents
    LineList *m_pNext;        // next node
  };
  
  // functions to work with this list
  LineList *LineListAppend(const wxString& str);
  LineList *LineListInsert(const wxString& str, 
                           LineList *pLine);    // NULL => Append()
  bool      LineListIsEmpty();

private:
  // put the object in the initial state
  void Init();

  // parse the whole file
  void Parse(wxTextFile& file, bool bLocal);

  // the same as SetPath("/")
  void SetRootPath();

  // member variables
  // ----------------
  LineList   *m_linesHead,        // head of the linked list
             *m_linesTail;        // tail

  wxString    m_strFile;          // file name passed to ctor
  wxString    m_strPath;          // current path (not '/' terminated)

  ConfigGroup *m_pRootGroup,      // the top (unnamed) group
              *m_pCurrentGroup;   // the current group

  // a handy little class which changes current path to the path of given entry
  // and restores it in dtor: so if you declare a local variable of this type,
  // you work in the entry directory and the path is automatically restored
  // when function returns
  class PathChanger
  {
  public:
    // ctor/dtor do path changing/restorin
    PathChanger(const wxFileConfig *pContainer, const wxString& strEntry);
   ~PathChanger();

    // get the key name
   const wxString& Name() const { return m_strName; }

  private:
    wxFileConfig *m_pContainer;   // object we live in
    wxString      m_strName,      // name of entry (i.e. name only)
                  m_strOldPath;   // saved path
    bool          m_bChanged;     // was the path changed?
  };

//protected: --- if FileConfig::ConfigEntry is not public, functions in
//               ConfigGroup such as Find/AddEntry can't return "ConfigEntry *"
public:
  WX_DEFINE_ARRAY(ConfigEntry *, ArrayEntries);
  WX_DEFINE_ARRAY(ConfigGroup *, ArrayGroups);

  class ConfigEntry
  {
  private:
    ConfigGroup  *m_pParent;      // group that contains us
    wxString      m_strName,      // entry name
                  m_strValue;     //       value
    bool          m_bDirty,       // changed since last read?
                  m_bImmutable;   // can be overriden locally?
    int           m_nLine;        // used if m_pLine == NULL only
    LineList     *m_pLine;        // pointer to our line in the linked list
                                  // or NULL if it was found in global file

  public:
    ConfigEntry(ConfigGroup *pParent, const wxString& strName, int nLine);

    // simple accessors
    const wxString& Name()        const { return m_strName;    }
    const wxString& Value()       const { return m_strValue;   }
    ConfigGroup    *Group()       const { return m_pParent;    }
    bool            IsDirty()     const { return m_bDirty;     }
    bool            IsImmutable() const { return m_bImmutable; }
    bool            IsLocal()     const { return m_pLine != 0; }
    int             Line()        const { return m_nLine;      }
    LineList       *GetLine()     const { return m_pLine;      }

    // modify entry attributes
    void SetValue(const wxString& strValue, bool bUser = TRUE);
    void SetDirty();
    void SetLine(LineList *pLine);
  };

protected:
  class ConfigGroup
  {
  private:
    wxFileConfig *m_pConfig;      // config object we belong to
    ConfigGroup  *m_pParent;      // parent group (NULL for root group)
    ArrayEntries  m_aEntries;     // entries in this group
    ArrayGroups   m_aSubgroups;   // subgroups
    wxString      m_strName;      // group's name
    bool          m_bDirty;       // if FALSE => all subgroups are not dirty
    LineList     *m_pLine;        // pointer to our line in the linked list
    int           m_nLastEntry,   // last here means "last added"
                  m_nLastGroup;   // 

  public:
    // ctor
    ConfigGroup(ConfigGroup *pParent, const wxString& strName, wxFileConfig *);

    // dtor deletes all entries and subgroups also
    ~ConfigGroup();

    // simple accessors
    const wxString& Name()    const { return m_strName; }
    ConfigGroup    *Parent()  const { return m_pParent; }
    wxFileConfig   *Config()  const { return m_pConfig; }
    bool            IsDirty() const { return m_bDirty;  }

    bool  IsEmpty() const { return Entries().IsEmpty() && Groups().IsEmpty(); }
    const ArrayEntries& Entries() const { return m_aEntries;   }
    const ArrayGroups&  Groups()  const { return m_aSubgroups; }

    // find entry/subgroup (NULL if not found)
    ConfigGroup *FindSubgroup(const char *szName) const;
    ConfigEntry *FindEntry   (const char *szName) const;

    // delete entry/subgroup, return FALSE if doesn't exist
    bool DeleteSubgroup(const char *szName);
    bool DeleteEntry(const char *szName);

    // create new entry/subgroup returning pointer to newly created element
    ConfigGroup *AddSubgroup(const wxString& strName);
    ConfigEntry *AddEntry   (const wxString& strName, int nLine = NOT_FOUND);

    // will also recursively set parent's dirty flag
    void SetDirty();
    void SetLine(LineList *pLine);

    wxString GetFullName() const;

    // get the last line belonging to an entry/subgroup of this group
    LineList *GetGroupLine();
    LineList *GetLastEntryLine();
    LineList *GetLastGroupLine();
  };
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------

void wxFileConfig::Init()
{
  m_pCurrentGroup = 
  m_pRootGroup    = new ConfigGroup(NULL, "", this);

  m_linesHead =
  m_linesTail = NULL;

  m_bExpandEnvVars = TRUE;

  m_strPath.Empty();
}

wxFileConfig::wxFileConfig(const wxString& strFile, bool bLocalOnly) 
            : m_strFile(strFile)
{
  Init();

  const char *szFile;

  // it's not an error if (one of the) file(s) doesn't exist

  // parse the global file
  if ( !bLocalOnly ) {
    szFile = GetGlobalFileName(strFile);
    if ( wxFile::Exists(szFile) ) {
      wxTextFile fileGlobal(szFile);

      if ( fileGlobal.Open() ) {
        Parse(fileGlobal, FALSE /* global */);
        SetRootPath();
      }
      else
        wxLogWarning("Can't open global configuration file.");
    }
  }

  // parse the local file
  szFile = GetLocalFileName(strFile);
  if ( wxFile::Exists(szFile) ) {
    wxTextFile fileLocal(szFile);
    if ( fileLocal.Open() ) {
      Parse(fileLocal, TRUE /* local */);
      SetRootPath();
    }
    else
      wxLogWarning("Can't open user configuration file.");
  }
}

wxFileConfig::~wxFileConfig()
{
  Flush();
  delete m_pRootGroup;
}

// ----------------------------------------------------------------------------
// parse a config file
// ----------------------------------------------------------------------------

void wxFileConfig::Parse(wxTextFile& file, bool bLocal)
{
  const char *pStart;
  const char *pEnd;
  
  for ( uint n = 0; n < file.GetLineCount(); n++ ) {
    // add the line to linked list
    if ( bLocal )
      LineListAppend(file[n]);

    // skip leading spaces
    for ( pStart = file[n]; isspace(*pStart); pStart++ )
      ;

    // skip blank/comment lines
    if ( *pStart == '\0'|| *pStart == ';' || *pStart == '#' )
      continue;

    if ( *pStart == '[' ) {          // a new group
      pEnd = pStart;

      while ( *++pEnd != ']' ) {
        if ( !IsValid(*pEnd) && *pEnd != ' ' )  // allow spaces in group names
          break;
      }

      if ( *pEnd != ']' ) {
        wxLogError("file '%s': unexpected character at line %d (missing ']'?)",
		                file.GetName(), n + 1);
        continue; // skip this line
      }

      // group name here is always considered as abs path
      wxString strGroup;
      pStart++;
      strGroup << APPCONF_PATH_SEPARATOR << wxString(pStart, pEnd - pStart);

      // will create it if doesn't yet exist
      SetPath(strGroup);

      if ( bLocal )
        m_pCurrentGroup->SetLine(m_linesTail);

      // check that there is nothing except comments left on this line
      bool bCont = TRUE;
      while ( *++pEnd != '\0' && bCont ) {
        switch ( *pEnd ) {
          case '#':
          case ';':
            bCont = FALSE;
            break;
        
          case ' ':
          case '\t':
            // ignore whitespace ('\n' impossible here)
            break;
          
          default:
            wxLogWarning("file '%s', line %d: '%s' ignored after group header.",
                         file.GetName(), n + 1, pEnd);
            bCont = FALSE;
        }
      }
    }
    else {                        // a key
      const char *pEnd = pStart;
      while ( IsValid(*pEnd) )
        pEnd++;

      wxString strKey(pStart, pEnd);

      // skip whitespace
      while ( isspace(*pEnd) ) 
        pEnd++;

      if ( *pEnd++ != '=' ) {
        wxLogError("file '%s', line %d: '=' expected.", file.GetName(), n + 1);
      }
      else {
        ConfigEntry *pEntry = m_pCurrentGroup->FindEntry(strKey);

        if ( pEntry == NULL ) {
          // new entry
          pEntry = m_pCurrentGroup->AddEntry(strKey, n);

          if ( bLocal )
            pEntry->SetLine(m_linesTail);
        }
        else {
          if ( bLocal && pEntry->IsImmutable() ) {
            // immutable keys can't be changed by user
            wxLogWarning("file '%s', line %d: value for immutable key '%s' ignored.",
                         file.GetName(), n + 1, strKey.c_str());
            continue;
          }
          // the condition below catches the cases (a) and (b) but not (c):
          //  (a) global key found second time in global file
          //  (b) key found second (or more) time in local file
          //  (c) key from global file now found in local one
          // which is exactly what we want.
          else if ( !bLocal || pEntry->IsLocal() ) {
            wxLogWarning("file '%s', line %d: key '%s' was first found at line %d.", 
                         file.GetName(), n + 1, strKey.c_str(), pEntry->Line());

            if ( bLocal )
              pEntry->SetLine(m_linesTail);
          }
        }

        // skip whitespace
        while ( isspace(*pEnd) ) 
          pEnd++;

        wxString strValue;
        if (m_bExpandEnvVars)
            strValue = ExpandEnvVars(FilterIn(pEnd));
        else
            strValue = FilterIn(pEnd);
        pEntry->SetValue(strValue, FALSE);
      }
    }
  }
}

// ----------------------------------------------------------------------------
// set/retrieve path
// ----------------------------------------------------------------------------

void wxFileConfig::SetRootPath()
{
  m_strPath.Empty();
  m_pCurrentGroup = m_pRootGroup;
}

void wxFileConfig::SetPath(const wxString& strPath)
{
  wxArrayString aParts;

  if ( strPath.IsEmpty() )
    return;

  if ( strPath[0] == APPCONF_PATH_SEPARATOR ) {
    // absolute path
    SplitPath(aParts, strPath);
  }
  else {
    // relative path, combine with current one
    wxString strFullPath = m_strPath;
    strFullPath << APPCONF_PATH_SEPARATOR << strPath;
    SplitPath(aParts, strFullPath);
  }

  // change current group
  uint n;
  m_pCurrentGroup = m_pRootGroup;
  for ( n = 0; n < aParts.Count(); n++ ) {
    ConfigGroup *pNextGroup = m_pCurrentGroup->FindSubgroup(aParts[n]);
    if ( pNextGroup == NULL )
      pNextGroup = m_pCurrentGroup->AddSubgroup(aParts[n]);
    m_pCurrentGroup = pNextGroup;
  }

  // recombine path parts in one variable
  m_strPath.Empty();
  for ( n = 0; n < aParts.Count(); n++ ) {
    m_strPath << APPCONF_PATH_SEPARATOR << aParts[n];
  }
}

// ----------------------------------------------------------------------------
// enumeration
// ----------------------------------------------------------------------------

bool wxFileConfig::GetFirstGroup(wxString& str, long& lIndex)
{
  lIndex = 0;
  return GetNextGroup(str, lIndex);
}

bool wxFileConfig::GetNextGroup (wxString& str, long& lIndex)
{
  if ( uint(lIndex) < m_pCurrentGroup->Groups().Count() ) {
    str = m_pCurrentGroup->Groups()[lIndex++]->Name();
    return TRUE;
  }
  else
    return FALSE;
}

bool wxFileConfig::GetFirstEntry(wxString& str, long& lIndex)
{
  lIndex = 0;
  return GetNextEntry(str, lIndex);
}

bool wxFileConfig::GetNextEntry (wxString& str, long& lIndex)
{
  if ( uint(lIndex) < m_pCurrentGroup->Entries().Count() ) {
    str = m_pCurrentGroup->Entries()[lIndex++]->Name();
    return TRUE;
  }
  else
    return FALSE;
}

// ----------------------------------------------------------------------------
// read/write values
// ----------------------------------------------------------------------------

const char *wxFileConfig::Read(const char *szKey, const char *szDefault) const
{
  PathChanger path(this, szKey);

  ConfigEntry *pEntry = m_pCurrentGroup->FindEntry(path.Name());
  if (pEntry == NULL)
    return szDefault;
  else
    return pEntry->Value();
//  return pEntry == NULL ? szDefault : pEntry->Value();
}

long wxFileConfig::Read(const char *szKey, long lDefault) const
{
  const char *pc = Read(szKey);
  return pc == NULL ? lDefault : atol(pc);
}

bool wxFileConfig::Write(const char *szKey, const char *szValue)
{
  PathChanger path(this, szKey);

  ConfigEntry *pEntry = m_pCurrentGroup->FindEntry(path.Name());
  if ( pEntry == NULL )
    pEntry = m_pCurrentGroup->AddEntry(path.Name());
  pEntry->SetValue(szValue);

  return TRUE;
}

bool wxFileConfig::Write(const char *szKey, long lValue)
{
  // ltoa() is not ANSI :-(
  char szBuf[40];   // should be good for sizeof(long) <= 16 (128 bits)
  sprintf(szBuf, "%ld", lValue);
  return Write(szKey, szBuf);
}

bool wxFileConfig::Flush(bool /* bCurrentOnly */)
{
  if ( LineListIsEmpty() || !m_pRootGroup->IsDirty() )
    return TRUE;

  wxTempFile file(GetLocalFileName(m_strFile));

  if ( !file.IsOpened() ) {
    wxLogError("Can't open user configuration file.");
    return FALSE;
  }

  // write all strings to file
  for ( LineList *p = m_linesHead; p != NULL; p = p->Next() ) {
    if ( !file.Write(p->Text() + wxTextFile::GetEOL()) ) {
      wxLogError("Can't write user configuration file.");
      return FALSE;
    }
  }

  return file.Commit();
}

// ----------------------------------------------------------------------------
// delete groups/entries
// ----------------------------------------------------------------------------

bool wxFileConfig::DeleteEntry(const char *szKey, bool bGroupIfEmptyAlso)
{
  PathChanger path(this, szKey);

  if ( !m_pCurrentGroup->DeleteEntry(path.Name()) )
    return FALSE;

  if ( bGroupIfEmptyAlso && m_pCurrentGroup->IsEmpty() ) {
    if ( m_pCurrentGroup != m_pRootGroup ) {
      ConfigGroup *pGroup = m_pCurrentGroup;
      SetPath("..");  // changes m_pCurrentGroup!
      m_pCurrentGroup->DeleteSubgroup(pGroup->Name());
    }
    //else: never delete the root group
  }

  return TRUE;
}

bool wxFileConfig::DeleteGroup(const char *szKey)
{
  PathChanger path(this, szKey);

  return m_pCurrentGroup->DeleteSubgroup(path.Name());
}

bool wxFileConfig::DeleteAll()
{
  const char *szFile = GetLocalFileName(m_strFile);
  delete m_pRootGroup;
  Init();

  if ( remove(szFile) == -1 )
    wxLogSysError("Can't delete user configuration file '%s'", szFile);

  szFile = GetGlobalFileName(m_strFile);
  if ( remove(szFile) )
    wxLogSysError("Can't delete system configuration file '%s'", szFile);

  return TRUE;
}

// ----------------------------------------------------------------------------
// linked list functions
// ----------------------------------------------------------------------------

// append a new line to the end of the list
wxFileConfig::LineList *wxFileConfig::LineListAppend(const wxString& str)
{
  LineList *pLine = new LineList(str);

  if ( m_linesTail == NULL ) {
    // list is empty
    m_linesHead = pLine;
  }
  else {
    // adjust pointers
    m_linesTail->SetNext(pLine);
  }

  m_linesTail = pLine;
  return m_linesTail;
}

// insert a new line after the given one
wxFileConfig::LineList *wxFileConfig::LineListInsert(const wxString& str, 
                                                     LineList *pLine)
{
  if ( pLine == NULL )
    return LineListAppend(str);

  LineList *pNewLine = new LineList(str, pLine->Next());
  pLine->SetNext(pNewLine);

  return pNewLine;
}

bool wxFileConfig::LineListIsEmpty()
{
  return m_linesHead == NULL;
}

// ============================================================================
// wxFileConfig::ConfigGroup
// ============================================================================

// ----------------------------------------------------------------------------
// ctor/dtor
// ----------------------------------------------------------------------------

// ctor
wxFileConfig::ConfigGroup::ConfigGroup(wxFileConfig::ConfigGroup *pParent,
                                       const wxString& strName,
                                       wxFileConfig *pConfig)
                         : m_strName(strName)
{
  m_pConfig = pConfig;
  m_pParent = pParent;
  m_pLine   = NULL;
  m_bDirty  = FALSE;

  m_nLastEntry = 
  m_nLastGroup = NOT_FOUND;
}

// dtor deletes all children
wxFileConfig::ConfigGroup::~ConfigGroup()
{
  // entries
  uint n, nCount = m_aEntries.Count();
  for ( n = 0; n < nCount; n++ )
    delete m_aEntries[n];

  // subgroups
  nCount = m_aSubgroups.Count();
  for ( n = 0; n < nCount; n++ )
    delete m_aSubgroups[n];
}

// ----------------------------------------------------------------------------
// line
// ----------------------------------------------------------------------------

void wxFileConfig::ConfigGroup::SetLine(LineList *pLine)
{
  wxASSERT( m_pLine == NULL ); // shouldn't be called twice

  m_pLine = pLine;
}

// return the line which contains "[our name]"
wxFileConfig::LineList *wxFileConfig::ConfigGroup::GetGroupLine()
{
  if ( m_pLine == NULL ) {
    // this group wasn't present in local config file, add it now
    if ( Parent() != NULL ) {
      wxString strFullName;
      strFullName << "[" << GetFullName().c_str() + 1 << "]"; // +1: no '/'
      m_pLine = m_pConfig->LineListInsert(strFullName, 
                                          Parent()->GetLastGroupLine());
    }
    else {
      // we're the root group, yet we were not in the local file => there were
      // only comments and blank lines in config file or nothing at all
      // we return NULL, so that LineListInsert() will do Append()
    }
  }

  return m_pLine;
}

// return the last line belonging to the subgroups of this group
// (after which we can add a new subgroup)
wxFileConfig::LineList *wxFileConfig::ConfigGroup::GetLastGroupLine()
{
  // if we have any subgroups, our last line is the last line of the last
  // subgroup
  if ( m_nLastGroup != NOT_FOUND ) {
    return m_aSubgroups[m_nLastGroup]->GetLastGroupLine();
  }

  // if we have any entries, our last line is the last entry
  if ( m_nLastEntry != NOT_FOUND ) {
    return m_aEntries[m_nLastEntry]->GetLine();
  }

  // nothing at all: last line is the first one
  return GetGroupLine();
}

// return the last line belonging to the entries of this group
// (after which we can add a new entry)
wxFileConfig::LineList *wxFileConfig::ConfigGroup::GetLastEntryLine()
{
  if ( m_nLastEntry != NOT_FOUND )
    return m_aEntries[m_nLastEntry]->GetLine();
  else
    return GetGroupLine();
}

// ----------------------------------------------------------------------------
// group name
// ----------------------------------------------------------------------------

wxString wxFileConfig::ConfigGroup::GetFullName() const
{
  if ( Parent() )
    return Parent()->GetFullName() + APPCONF_PATH_SEPARATOR + Name();
  else
    return "";
}

// ----------------------------------------------------------------------------
// find an item
// ----------------------------------------------------------------------------

wxFileConfig::ConfigEntry *
wxFileConfig::ConfigGroup::FindEntry(const char *szName) const
{
  uint nCount = m_aEntries.Count();
  for ( uint n = 0; n < nCount; n++ ) {
    if ( m_aEntries[n]->Name().IsSameAs(szName, APPCONF_CASE_SENSITIVE) )
      return m_aEntries[n];
  }

  return NULL;
}

wxFileConfig::ConfigGroup *
wxFileConfig::ConfigGroup::FindSubgroup(const char *szName) const
{
  uint nCount = m_aSubgroups.Count();
  for ( uint n = 0; n < nCount; n++ ) {
    if ( m_aSubgroups[n]->Name().IsSameAs(szName, APPCONF_CASE_SENSITIVE) )
      return m_aSubgroups[n];
  }

  return NULL;
}

// ----------------------------------------------------------------------------
// create a new item
// ----------------------------------------------------------------------------

// create a new entry and add it to the current group
wxFileConfig::ConfigEntry *
wxFileConfig::ConfigGroup::AddEntry(const wxString& strName, int nLine)
{
  wxASSERT( FindEntry(strName) == NULL );

  ConfigEntry *pEntry = new ConfigEntry(this, strName, nLine);
  m_aEntries.Add(pEntry);

  return pEntry;
}

// create a new group and add it to the current group
wxFileConfig::ConfigGroup *
wxFileConfig::ConfigGroup::AddSubgroup(const wxString& strName)
{
  wxASSERT( FindSubgroup(strName) == NULL );

  ConfigGroup *pGroup = new ConfigGroup(this, strName, m_pConfig);
  m_aSubgroups.Add(pGroup);

  return pGroup;
}

// ----------------------------------------------------------------------------
// delete an item
// ----------------------------------------------------------------------------

bool wxFileConfig::ConfigGroup::DeleteSubgroup(const char *szName)
{
  uint n, nCount = m_aSubgroups.Count();
  for ( n = 0; n < nCount; n++ ) {
    if ( m_aSubgroups[n]->Name().IsSameAs(szName, APPCONF_CASE_SENSITIVE) )
      break;
  }

  if ( n == nCount )
    return FALSE;

  delete m_aSubgroups[n];
  m_aSubgroups.Remove(n);
  return TRUE;
}

bool wxFileConfig::ConfigGroup::DeleteEntry(const char *szName)
{
  uint n, nCount = m_aEntries.Count();
  for ( n = 0; n < nCount; n++ ) {
    if ( m_aEntries[n]->Name().IsSameAs(szName, APPCONF_CASE_SENSITIVE) )
      break;
  }

  if ( n == nCount )
    return FALSE;

  delete m_aEntries[n];
  m_aEntries.Remove(n);
  return TRUE;
}

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
void wxFileConfig::ConfigGroup::SetDirty()
{
  m_bDirty = TRUE;
  if ( Parent() != NULL )             // propagate upwards
    Parent()->SetDirty();
}

// ============================================================================
// wxFileConfig::ConfigEntry
// ============================================================================

// ----------------------------------------------------------------------------
// ctor
// ----------------------------------------------------------------------------
wxFileConfig::ConfigEntry::ConfigEntry(wxFileConfig::ConfigGroup *pParent, 
                                       const wxString& strName,
                                       int nLine)
                         : m_strName(strName)
{
  m_pParent = pParent;
  m_nLine   = nLine;
  m_pLine   = NULL;

  m_bDirty  = FALSE;

  m_bImmutable = strName[0] == APPCONF_IMMUTABLE_PREFIX;
  if ( m_bImmutable )
    m_strName.erase(0, 1);  // remove first character
}

// ----------------------------------------------------------------------------
// set value
// ----------------------------------------------------------------------------

void wxFileConfig::ConfigEntry::SetLine(LineList *pLine)
{
  wxASSERT( m_pLine == NULL );

  m_pLine = pLine;
}

// second parameter is FALSE if we read the value from file and prevents the
// entry from being marked as 'dirty'
void wxFileConfig::ConfigEntry::SetValue(const wxString& strValue, bool bUser)
{
  if ( bUser && IsImmutable() ) {
    wxLogWarning("Attempt to change immutable key '%s' ignored.", 
                 Name().c_str());
    return;
  }

  // do nothing if it's the same value
  if ( strValue == m_strValue )
    return;

  m_strValue = strValue;

  if ( bUser ) {
    wxString strVal = FilterOut(strValue);
    wxString strLine;
    strLine << m_strName << " = " << strVal;

    if ( m_pLine != NULL ) {
      // entry was read from the local config file, just modify the line
      m_pLine->SetText(strLine);
    }
    else {
      // add a new line to the file
      wxASSERT( m_nLine == NOT_FOUND );   // consistency check

      Group()->Config()->LineListInsert(strLine, Group()->GetLastEntryLine());
    }

    SetDirty();
  }
}

void wxFileConfig::ConfigEntry::SetDirty()
{
  m_bDirty = TRUE;
  Group()->SetDirty();
}

// ============================================================================
// wxFileConfig::PathChanger
// ============================================================================

wxFileConfig::PathChanger::PathChanger(const wxFileConfig *pContainer,
                                       const wxString& strEntry)
{
  m_pContainer = (wxFileConfig *)pContainer;
  wxString strPath = strEntry.Before(APPCONF_PATH_SEPARATOR);
  if ( !strPath.IsEmpty() ) {
    // do change the path
    m_bChanged = TRUE;
    m_strName = strEntry.Right(APPCONF_PATH_SEPARATOR);
    m_strOldPath = m_pContainer->GetPath();
    m_strOldPath += APPCONF_PATH_SEPARATOR;
    m_pContainer->SetPath(strPath);
  }
  else {
    // it's a name only, without path - nothing to do
    m_bChanged = FALSE;
    m_strName = strEntry;
  }
}

wxFileConfig::PathChanger::~PathChanger()
{
  // only restore path if it was changed
  if ( m_bChanged ) {
    m_pContainer->SetPath(m_strOldPath);
  }
}

// ============================================================================
// global functions
// ============================================================================

const char *GetGlobalFileName(const char *szFile)
{
  static wxString s_str;
  s_str.Empty();

  bool bNoExt = strchr(szFile, '.') == NULL;

  #ifdef  __UNIX__
    s_str << "/etc/" << szFile;
    if ( bNoExt )
      s_str << ".conf";
  #else   // Windows
#ifndef _MAX_PATH
#define _MAX_PATH 512
#endif
    char szWinDir[_MAX_PATH];
    ::GetWindowsDirectory(szWinDir, _MAX_PATH);
    s_str << szWinDir <<  "\\" << szFile;
    if ( bNoExt )
      s_str << ".INI";
  #endif  // UNIX/Win

  return s_str.c_str();
}

const char *GetLocalFileName(const char *szFile)
{
  static wxString s_str;
  s_str.Empty();

  #ifdef  __UNIX__
    const char *szHome = getenv("HOME");
    if ( szHome == NULL ) {
      // we're homeless...
      wxLogWarning("can't find user's HOME, using current directory.");
      szHome = ".";
    }
    s_str << szHome << "/." << szFile;
  #else   // Windows
    #ifdef  __WIN32__
      const char *szHome = getenv("HOMEDRIVE");
      if ( szHome == NULL )
        szHome = "";
      s_str << szHome;
      szHome = getenv("HOMEPATH");
      s_str << ( szHome == NULL ? "." : szHome ) << szFile;
      if ( strchr(szFile, '.') == NULL )
        s_str << ".INI";
    #else   // Win16
      // Win16 has no idea about home, so use the current directory instead
      s_str << ".\\" << szFile;
    #endif  // WIN16/32
  #endif  // UNIX/Win

  return s_str.c_str();
}

void SplitPath(wxArrayString& aParts, const char *sz)
{
  aParts.Empty();

  wxString strCurrent;
  const char *pc = sz;
  for ( ;; ) {
    if ( *pc == '\0' || *pc == APPCONF_PATH_SEPARATOR ) {
      if ( strCurrent == "." ) {
        // ignore
      }
      else if ( strCurrent == ".." ) {
        // go up one level
        if ( aParts.IsEmpty() )
          wxLogWarning("'%s' has extra '..', ignored.", sz);
        else
          aParts.Remove(aParts.Count() - 1);
      }
      else if ( !strCurrent.IsEmpty() ) {
        aParts.Add(strCurrent);
        strCurrent.Empty();
      }
      //else:
        // could log an error here, but we prefer to ignore extra '/'

      if ( *pc == '\0' )
        return;
    }
    else
      strCurrent += *pc;

    pc++;
  }
}

// undo FilterOut
wxString FilterIn(const wxString& str)
{
  wxString strResult;

  bool bQuoted = str[0] == '"';

  for ( uint n = bQuoted ? 1 : 0; n < str.Len(); n++ ) {
    if ( str[n] == '\\' ) {
      switch ( str[++n] ) {
        case 'n':
          strResult += '\n';
          break;

        case 't':
          strResult += '\t';
          break;

        case '\\':
          strResult += '\\';
          break;

        case '"':
          strResult += '"';
          break;
      }
    }
    else {
      if ( str[n] != '"' || !bQuoted )
        strResult += str[n];
      else if ( n != str.Len() - 1 )
        wxLogWarning("unexpected \" at position %d in '%s'.", n, str.c_str());
      //else: it's the last quote of a quoted string, ok
    }
  }

  return strResult;
}

// quote the string before writing it to file
wxString FilterOut(const wxString& str)
{
  wxString strResult;

  // quoting is necessary to preserve spaces in the beginning of the string
  bool bQuote = isspace(str[0]) || str[0] == '"';

  if ( bQuote )
    strResult += '"';

  char c;
  for ( uint n = 0; n < str.Len(); n++ ) {
    switch ( str[n] ) {
      case '\n':
        c = 'n';
        break;

      case '\t':
        c = 't';
        break;

      case '\\':
        c = '\\';
        break;

      case '"':
        if ( bQuote )
          c = '"';
        //else: fall through

      default:
        strResult += str[n];
        continue;   // nothing special to do
    }

    // we get here only for special characters
    strResult << '\\' << c;
  }

  if ( bQuote )
    strResult += '"';

  return strResult;
}

wxConfig *CreateFileConfig(const wxString& strFile, bool bLocalOnly)
{
  return new wxFileConfig(strFile, bLocalOnly);
}