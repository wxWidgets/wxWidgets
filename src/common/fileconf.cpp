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
#include  <wx/fileconf.h>

// _WINDOWS_ is defined when windows.h is included,
// __WINDOWS__ is defined for MS Windows compilation
#if       defined(__WINDOWS__) && !defined(_WINDOWS_)
  #include  <windows.h>
#endif  //windows.h

#include  <stdlib.h>
#include  <ctype.h>

// ----------------------------------------------------------------------------
// global functions declarations
// ----------------------------------------------------------------------------

// is 'c' a valid character in group name?
// NB: APPCONF_IMMUTABLE_PREFIX and APPCONF_PATH_SEPARATOR must be valid chars,
//     but _not_ ']' (group name delimiter)
inline bool IsValid(char c) { return isalnum(c) || strchr("_/-!.*%", c); }

// filter strings
static wxString FilterIn(const wxString& str);
static wxString FilterOut(const wxString& str);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// static functions
// ----------------------------------------------------------------------------
wxString wxFileConfig::GetGlobalFileName(const char *szFile)
{
  wxString str;

  bool bNoExt = strchr(szFile, '.') == NULL;

  #ifdef  __UNIX__
    str << "/etc/" << szFile;
    if ( bNoExt )
      str << ".conf";
  #else   // Windows
    #ifndef _MAX_PATH
      #define _MAX_PATH 512
    #endif

    char szWinDir[_MAX_PATH];
    ::GetWindowsDirectory(szWinDir, _MAX_PATH);
    str << szWinDir <<  "\\" << szFile;
    if ( bNoExt )
      str << ".ini";
  #endif  // UNIX/Win

  return str;
}

wxString wxFileConfig::GetLocalFileName(const char *szFile)
{
  wxString str;

  #ifdef  __UNIX__
    const char *szHome = getenv("HOME");
    if ( szHome == NULL ) {
      // we're homeless...
      wxLogWarning("can't find user's HOME, using current directory.");
      szHome = ".";
    }
    str << szHome << "/." << szFile;
  #else   // Windows
    #ifdef  __WIN32__
      const char *szHome = getenv("HOMEDRIVE");
      if ( szHome != NULL )
        str << szHome;
      szHome = getenv("HOMEPATH");
      if ( szHome != NULL )
        str << szHome;
      str << szFile;
      if ( strchr(szFile, '.') == NULL )
        str << ".ini";
    #else   // Win16
      // Win16 has no idea about home, so use the current directory instead
      str << ".\\" << szFile;
    #endif  // WIN16/32
  #endif  // UNIX/Win

  return str;
}

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

wxFileConfig::wxFileConfig(const wxString& strLocal, const wxString& strGlobal)
            : m_strLocalFile(strLocal), m_strGlobalFile(strGlobal)
{
  Init();

  // it's not an error if (one of the) file(s) doesn't exist

  // parse the global file
  if ( !strGlobal.IsEmpty() ) {
    if ( wxFile::Exists(strGlobal) ) {
      wxTextFile fileGlobal(strGlobal);

      if ( fileGlobal.Open() ) {
        Parse(fileGlobal, FALSE /* global */);
        SetRootPath();
      }
      else
        wxLogWarning("Can't open global configuration file '%s'.",
                     strGlobal.c_str());
    }
  }

  // parse the local file
  if ( wxFile::Exists(strLocal) ) {
    wxTextFile fileLocal(strLocal);
    if ( fileLocal.Open() ) {
      Parse(fileLocal, TRUE /* local */);
      SetRootPath();
    }
    else
      wxLogWarning("Can't open user configuration file '%s'.",
                   strLocal.c_str());
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

const char *wxFileConfig::Read(const char *szKey,
                               const char *szDefault) const
{
  PathChanger path(this, szKey);

  ConfigEntry *pEntry = m_pCurrentGroup->FindEntry(path.Name());
  return pEntry == NULL ? szDefault : pEntry->Value().c_str();
}

bool wxFileConfig::Read(wxString   *pstr,
                        const char *szKey,
                        const char *szDefault) const
{
  PathChanger path(this, szKey);

  ConfigEntry *pEntry = m_pCurrentGroup->FindEntry(path.Name());
  if (pEntry == NULL) {
    *pstr = szDefault;
    return FALSE;
  }
  else {
    *pstr = pEntry->Value();
    return TRUE;
  }
}

bool wxFileConfig::Read(long *pl, const char *szKey, long lDefault) const
{
  wxString str;
  if ( Read(&str, szKey) ) {
    *pl = atol(str);
    return TRUE;
  }
  else {
    *pl = lDefault;
    return FALSE;
  }
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

  wxTempFile file(m_strLocalFile);

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
  const char *szFile = m_strLocalFile;
  delete m_pRootGroup;
  Init();

  if ( remove(szFile) == -1 )
    wxLogSysError("Can't delete user configuration file '%s'", szFile);

  szFile = m_strGlobalFile;
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

// insert a new line after the given one or in the very beginning if !pLine
wxFileConfig::LineList *wxFileConfig::LineListInsert(const wxString& str, 
                                                     LineList *pLine)
{
  if ( pLine == m_linesTail )
    return LineListAppend(str);

  LineList *pNewLine;
  
  if ( pLine == NULL ) {
    pNewLine = new LineList(str, m_linesHead);
    m_linesHead = pNewLine;
  }
  else {
    pNewLine = new LineList(str, pLine->Next());
    pLine->SetNext(pNewLine);
  }

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
  m_pLastEntry = NULL;
  m_pLastGroup = NULL;
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
      Parent()->SetLastGroup(this);
    }
    else {
      // we return NULL, so that LineListInsert() will insert us in the
      // very beginning
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
  if ( m_pLastGroup != NULL )
    return m_pLastGroup->GetLastGroupLine();

  // if we have any entries, our last line is the last entry
  if ( m_pLastEntry != NULL )
    return m_pLastEntry->GetLine();

  // nothing at all: last line is the first one
  return GetGroupLine();
}

// return the last line belonging to the entries of this group
// (after which we can add a new entry)
wxFileConfig::LineList *wxFileConfig::ConfigGroup::GetLastEntryLine()
{
  if ( m_pLastEntry != NULL ) {
    wxFileConfig::LineList *pLine = m_pLastEntry->GetLine();

    wxASSERT( pLine != NULL );  // last entry must have !NULL associated line
    return pLine;
  }

  // no entrues: insert after the group header
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
  if ( m_pLine != NULL ) {
    wxLogWarning("Entry '%s' appears more than once in group '%s'",
                 Name().c_str(), m_pParent->GetFullName().c_str());
  }

  m_pLine = pLine;
  Group()->SetLastEntry(this);
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

      m_pLine = Group()->Config()->LineListInsert(strLine, 
                                                  Group()->GetLastEntryLine());
      Group()->SetLastEntry(this);
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
// global functions
// ============================================================================

// undo FilterOut
wxString FilterIn(const wxString& str)
{
  wxString strResult;

  bool bQuoted = !str.IsEmpty() && str[0] == '"';

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
