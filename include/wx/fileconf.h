///////////////////////////////////////////////////////////////////////////////
// Name:        fileconf.h
// Purpose:     wxFileConfig derivation of wxConfigBase
// Author:      Vadim Zeitlin
// Modified by:
// Created:     07.04.98 (adapted from appconf.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Karsten Ballüder   &  Vadim Zeitlin
//                       Ballueder@usa.net     <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _FILECONF_H
#define   _FILECONF_H

#ifdef __GNUG__
#pragma interface "fileconf.h"
#endif

#include "wx/defs.h"

#if wxUSE_CONFIG

#include "wx/textfile.h"
#include "wx/string.h"

// ----------------------------------------------------------------------------
// wxFileConfig
// ----------------------------------------------------------------------------

/*
  wxFileConfig derives from base Config and implements file based config class,
  i.e. it uses ASCII disk files to store the information. These files are
  alternatively called INI, .conf or .rc in the documentation. They are
  organized in groups or sections, which can nest (i.e. a group contains
  subgroups, which contain their own subgroups &c). Each group has some
  number of entries, which are "key = value" pairs. More precisely, the format
  is:

  # comments are allowed after either ';' or '#' (Win/UNIX standard)

  # blank lines (as above) are ignored

  # global entries are members of special (no name) top group
  written_for = Windows
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

  wxFileConfig manages not less than 2 config files for each program: global
  and local (or system and user if you prefer). Entries are read from both of
  them and the local entries override the global ones unless the latter is
  immutable (prefixed with '!') in which case a warning message is generated
  and local value is ignored. Of course, the changes are always written to local
  file only.

  The names of these files can be specified in a number of ways. First of all,
  you can use the standard convention: using the ctor which takes 'strAppName'
  parameter will probably be sufficient for 90% of cases. If, for whatever
  reason you wish to use the files with some other names, you can always use the
  second ctor.

  wxFileConfig also may automatically expand the values of environment variables
  in the entries it reads: for example, if you have an entry
    score_file = $HOME/.score
  a call to Read(&str, "score_file") will return a complete path to .score file
  unless the expansion was previousle disabled with SetExpandEnvVars(FALSE) call
  (it's on by default, the current status can be retrieved with
   IsExpandingEnvVars function).
*/
class wxFileConfig;
class ConfigGroup;
class ConfigEntry;

// we store all lines of the local config file as a linked list in memory
class LineList
{
public:
  void      SetNext(LineList *pNext)  { m_pNext = pNext; }
  void      SetPrev(LineList *pPrev)  { m_pPrev = pPrev; }

  // ctor
  LineList(const wxString& str, LineList *pNext = (LineList *) NULL) : m_strLine(str)
    { SetNext(pNext); SetPrev((LineList *) NULL); }

  //
  LineList *Next() const              { return m_pNext;  }
  LineList *Prev() const              { return m_pPrev;  }

  //
  void SetText(const wxString& str) { m_strLine = str;  }
  const wxString& Text() const      { return m_strLine; }

private:
  wxString  m_strLine;      // line contents
  LineList *m_pNext,        // next node
           *m_pPrev;        // previous one
};

class wxFileConfig : public wxConfigBase
{
public:
  // construct the "standard" full name for global (system-wide) and
  // local (user-specific) config files from the base file name.
  //
  // the following are the filenames returned by this functions:
  //            global                local
  // Unix   /etc/file.ext           ~/.file
  // Win    %windir%\file.ext   %USERPROFILE%\file.ext
  //
  // where file is the basename of szFile, ext is it's extension
  // or .conf (Unix) or .ini (Win) if it has none
  static wxString GetGlobalFileName(const wxChar *szFile);
  static wxString GetLocalFileName(const wxChar *szFile);

  // ctor & dtor
    // New constructor: one size fits all. Specify wxCONFIG_USE_LOCAL_FILE or
    // wxCONFIG_USE_GLOBAL_FILE to say which files should be used.
  wxFileConfig(const wxString& appName,
               const wxString& vendorName = _T(""),
               const wxString& localFilename = _T(""),
               const wxString& globalFilename = _T(""),
               long style = wxCONFIG_USE_LOCAL_FILE);

    // dtor will save unsaved data
  virtual ~wxFileConfig();

  // implement inherited pure virtual functions
  virtual void SetPath(const wxString& strPath);
  virtual const wxString& GetPath() const { return m_strPath; }

  virtual bool GetFirstGroup(wxString& str, long& lIndex) const;
  virtual bool GetNextGroup (wxString& str, long& lIndex) const;
  virtual bool GetFirstEntry(wxString& str, long& lIndex) const;
  virtual bool GetNextEntry (wxString& str, long& lIndex) const;

  virtual size_t GetNumberOfEntries(bool bRecursive = FALSE) const;
  virtual size_t GetNumberOfGroups(bool bRecursive = FALSE) const;

  virtual bool HasGroup(const wxString& strName) const;
  virtual bool HasEntry(const wxString& strName) const;

  virtual bool Read(const wxString& key, wxString *pStr) const;
  virtual bool Read(const wxString& key, wxString *pStr, const wxString& defValue) const;
  virtual bool Read(const wxString& key, long *pl) const;

  // The following are necessary to satisfy the compiler
  wxString Read(const wxString& key, const wxString& defVal) const
    { return wxConfigBase::Read(key, defVal); }
  bool Read(const wxString& key, long *pl, long defVal) const
    { return wxConfigBase::Read(key, pl, defVal); }
  long Read(const wxString& key, long defVal) const
    { return wxConfigBase::Read(key, defVal); }
  bool Read(const wxString& key, int *pi, int defVal) const
    { return wxConfigBase::Read(key, pi, defVal); }
  bool Read(const wxString& key, int *pi) const
    { return wxConfigBase::Read(key, pi); }
  bool Read(const wxString& key, double* val) const
    { return wxConfigBase::Read(key, val); }
  bool Read(const wxString& key, double* val, double defVal) const
    { return wxConfigBase::Read(key, val, defVal); }
  bool Read(const wxString& key, bool* val) const
    { return wxConfigBase::Read(key, val); }
  bool Read(const wxString& key, bool* val, bool defVal) const
    { return wxConfigBase::Read(key, val, defVal); }

  virtual bool Write(const wxString& key, const wxString& szValue);
  virtual bool Write(const wxString& key, long lValue);
  bool Write(const wxString& key, double value)
    { return wxConfigBase::Write(key, value); }
  bool Write(const wxString& key, bool value)
    { return wxConfigBase::Write(key, value); }

  virtual bool Flush(bool bCurrentOnly = FALSE);

  virtual bool RenameEntry(const wxString& oldName, const wxString& newName);
  virtual bool RenameGroup(const wxString& oldName, const wxString& newName);

  virtual bool DeleteEntry(const wxString& key, bool bGroupIfEmptyAlso);
  virtual bool DeleteGroup(const wxString& szKey);
  virtual bool DeleteAll();

public:
  // functions to work with this list
  LineList *LineListAppend(const wxString& str);
  LineList *LineListInsert(const wxString& str,
                           LineList *pLine);    // NULL => Prepend()
  void      LineListRemove(LineList *pLine);
  bool      LineListIsEmpty();

private:
  // GetXXXFileName helpers: return ('/' terminated) directory names
  static wxString GetGlobalDir();
  static wxString GetLocalDir();

  // common part of all ctors (assumes that m_str{Local|Global}File are already
  // initialized
  void Init();

  // common part of from dtor and DeleteAll
  void CleanUp();

  // parse the whole file
  void Parse(wxTextFile& file, bool bLocal);

  // the same as SetPath("/")
  void SetRootPath();

  // member variables
  // ----------------
  LineList   *m_linesHead,        // head of the linked list
             *m_linesTail;        // tail

  wxString    m_strLocalFile,     // local  file name passed to ctor
              m_strGlobalFile;    // global
  wxString    m_strPath;          // current path (not '/' terminated)

  ConfigGroup *m_pRootGroup,      // the top (unnamed) group
              *m_pCurrentGroup;   // the current group

public:
  WX_DEFINE_SORTED_ARRAY(ConfigEntry *, ArrayEntries);
  WX_DEFINE_SORTED_ARRAY(ConfigGroup *, ArrayGroups);
};

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

class ConfigGroup
{
private:
  wxFileConfig *m_pConfig;        // config object we belong to
  ConfigGroup  *m_pParent;        // parent group (NULL for root group)
  wxFileConfig::ArrayEntries  m_aEntries;       // entries in this group
  wxFileConfig::ArrayGroups   m_aSubgroups;     // subgroups
  wxString      m_strName;        // group's name
  bool          m_bDirty;         // if FALSE => all subgroups are not dirty
  LineList     *m_pLine;          // pointer to our line in the linked list
  ConfigEntry  *m_pLastEntry;     // last entry/subgroup of this group in the
  ConfigGroup  *m_pLastGroup;     // local file (we insert new ones after it)

  // DeleteSubgroupByName helper
  bool DeleteSubgroup(ConfigGroup *pGroup);

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

  const wxFileConfig::ArrayEntries& Entries() const { return m_aEntries;   }
  const wxFileConfig::ArrayGroups&  Groups()  const { return m_aSubgroups; }
  bool  IsEmpty() const { return Entries().IsEmpty() && Groups().IsEmpty(); }

  // find entry/subgroup (NULL if not found)
  ConfigGroup *FindSubgroup(const wxChar *szName) const;
  ConfigEntry *FindEntry   (const wxChar *szName) const;

  // delete entry/subgroup, return FALSE if doesn't exist
  bool DeleteSubgroupByName(const wxChar *szName);
  bool DeleteEntry(const wxChar *szName);

  // create new entry/subgroup returning pointer to newly created element
  ConfigGroup *AddSubgroup(const wxString& strName);
  ConfigEntry *AddEntry   (const wxString& strName, int nLine = wxNOT_FOUND);

  // will also recursively set parent's dirty flag
  void SetDirty();
  void SetLine(LineList *pLine);

  // rename: no checks are done to ensure that the name is unique!
  void Rename(const wxString& newName);

  //
  wxString GetFullName() const;

  // get the last line belonging to an entry/subgroup of this group
  LineList *GetGroupLine();     // line which contains [group]
  LineList *GetLastEntryLine(); // after which our subgroups start
  LineList *GetLastGroupLine(); // after which the next group starts

  // called by entries/subgroups when they're created/deleted
  void SetLastEntry(ConfigEntry *pEntry) { m_pLastEntry = pEntry; }
  void SetLastGroup(ConfigGroup *pGroup) { m_pLastGroup = pGroup; }
};

#endif
  // wxUSE_CONFIG

#endif  
  //_FILECONF_H

