/*****************************************************************************\
 * Project:   CppLib: C++ library for Windows/UNIX platfroms                 *
 * File:      fileconf.h - file based implementation of Config               *
 *---------------------------------------------------------------------------*
 * Language:  C++                                                            *
 * Platfrom:  Any                                                            *
 *---------------------------------------------------------------------------*
 * Classes:                                                                  *
 *---------------------------------------------------------------------------*
 * Author:    Vadim Zeitlin zeitlin@dptmaths.ens-cachan.fr>                  *
 *            adapted from earlier class by VZ & Karsten Ballüder            *
 * History:                                                                  *
 *  27.04.98  created                                                        *
\*****************************************************************************/

#ifndef   _FILECONF_H
#define   _FILECONF_H

#ifdef __GNUG__
#pragma interface "fileconf.h"
#endif

// ----------------------------------------------------------------------------
// compile options
// ----------------------------------------------------------------------------

// it won't compile without it anyhow
#ifndef USE_WXCONFIG
  #error "Please define USE_WXCONFIG or remove fileconf.cpp from your makefile"
#endif // USE_WXCONFIG

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

  @@@@ describe environment variable expansion
*/

class wxFileConfig : public wxConfig
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
  static wxString GetGlobalFileName(const char *szFile);
  static wxString GetLocalFileName(const char *szFile);

  // ctor & dtor
    // if strGlobal is empty, only local config file is used
  wxFileConfig(const wxString& strLocal,
               const wxString& strGlobal = "");
    // dtor will save unsaved data
  virtual ~wxFileConfig();

  // implement inherited pure virtual functions
  virtual void SetPath(const wxString& strPath);
  virtual const wxString& GetPath() const { return m_strPath; }

  virtual bool GetFirstGroup(wxString& str, long& lIndex);
  virtual bool GetNextGroup (wxString& str, long& lIndex);
  virtual bool GetFirstEntry(wxString& str, long& lIndex);
  virtual bool GetNextEntry (wxString& str, long& lIndex);

  virtual bool HasGroup(const wxString& strName) const;
  virtual bool HasEntry(const wxString& strName) const;

  virtual bool Read(wxString *pstr, const char *szKey,
                    const char *szDefault = 0) const;
  virtual const char *Read(const char *szKey,
                           const char *szDefault = 0) const;
  virtual bool Read(long *pl, const char *szKey, long lDefault) const;
  virtual long Read(const char *szKey, long lDefault) const
    { return wxConfig::Read(szKey, lDefault); }
  virtual bool Write(const char *szKey, const char *szValue);
  virtual bool Write(const char *szKey, long lValue);
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
      { SetNext(pNext); SetPrev(NULL); }

    //
    LineList *Next() const              { return m_pNext;  }
    LineList *Prev() const              { return m_pPrev;  }
    void      SetNext(LineList *pNext)  { m_pNext = pNext; }
    void      SetPrev(LineList *pPrev)  { m_pPrev = pPrev; }

    //
    void SetText(const wxString& str) { m_strLine = str;  }
    const wxString& Text() const      { return m_strLine; }

  private:
    wxString  m_strLine;      // line contents
    LineList *m_pNext,        // next node
             *m_pPrev;        // previous one
  };

  // functions to work with this list
  LineList *LineListAppend(const wxString& str);
  LineList *LineListInsert(const wxString& str,
                           LineList *pLine);    // NULL => Prepend()
  void      LineListRemove(LineList *pLine);
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

  wxString    m_strLocalFile,     // local  file name passed to ctor
              m_strGlobalFile;    // global
  wxString    m_strPath;          // current path (not '/' terminated)

  ConfigGroup *m_pRootGroup,      // the top (unnamed) group
              *m_pCurrentGroup;   // the current group

//protected: --- if wxFileConfig::ConfigEntry is not public, functions in
//               ConfigGroup such as Find/AddEntry can't return "ConfigEntry *"
public:
  WX_DEFINE_SORTED_ARRAY(ConfigEntry *, ArrayEntries);
  WX_DEFINE_SORTED_ARRAY(ConfigGroup *, ArrayGroups);

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
    wxFileConfig *m_pConfig;      // config object we belong to
    ConfigGroup  *m_pParent;      // parent group (NULL for root group)
    ArrayEntries  m_aEntries;     // entries in this group
    ArrayGroups   m_aSubgroups;   // subgroups
    wxString      m_strName;      // group's name
    bool          m_bDirty;       // if FALSE => all subgroups are not dirty
    LineList     *m_pLine;        // pointer to our line in the linked list
    ConfigEntry  *m_pLastEntry;   // last entry of this group in the local file
    ConfigGroup  *m_pLastGroup;   // last subgroup

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

    // the new entries in this subgroup will be inserted after the last subgroup
    // or, if there is none, after the last entry
    void SetLastEntry(ConfigEntry *pLastEntry) { m_pLastEntry = pLastEntry; }
    void SetLastGroup(ConfigGroup *pLastGroup) { m_pLastGroup = pLastGroup; }

    wxString GetFullName() const;

    // get the last line belonging to an entry/subgroup of this group
    LineList *GetGroupLine();
    LineList *GetLastEntryLine();
    LineList *GetLastGroupLine();
  };
};

#endif  //_FILECONF_H

