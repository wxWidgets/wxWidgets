/////////////////////////////////////////////////////////////////////////////
// Name:        _ditctrl.i
// Purpose:     SWIG interface file for wxGenericDirCtrl
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2002 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(DirDialogDefaultFolderStr);

//---------------------------------------------------------------------------
%newgroup

// Extra styles for wxGenericDirCtrl
enum
{
    // Only allow directory viewing/selection, no files
    wxDIRCTRL_DIR_ONLY       = 0x0010,
    // When setting the default path, select the first file in the directory
    wxDIRCTRL_SELECT_FIRST   = 0x0020,
    // Show the filter list
    wxDIRCTRL_SHOW_FILTERS   = 0x0040,
    // Use 3D borders on internal controls
    wxDIRCTRL_3D_INTERNAL    = 0x0080,
    // Editable labels
    wxDIRCTRL_EDIT_LABELS    = 0x0100
};



#if 0
class wxDirItemData : public wxObject // wxTreeItemData
{
public:
  wxDirItemData(const wxString& path, const wxString& name, bool isDir);
//  ~wxDirItemDataEx();
  void SetNewDirName( wxString path );
  wxString m_path, m_name;
  bool m_isHidden;
  bool m_isExpanded;
  bool m_isDir;
};
#endif




MustHaveApp(wxGenericDirCtrl);

class wxGenericDirCtrl: public wxControl
{
public:
    %pythonAppend wxGenericDirCtrl         "self._setOORInfo(self)"
    %pythonAppend wxGenericDirCtrl()       ""

    wxGenericDirCtrl(wxWindow *parent, const wxWindowID id = -1,
                     const wxString& dir = wxPyDirDialogDefaultFolderStr,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER,
                     const wxString& filter = wxPyEmptyString,
                     int defaultFilter = 0,
                     const wxString& name = wxPyTreeCtrlNameStr);
    %RenameCtor(PreGenericDirCtrl, wxGenericDirCtrl());


    bool Create(wxWindow *parent, const wxWindowID id = -1,
                const wxString& dir = wxPyDirDialogDefaultFolderStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDIRCTRL_3D_INTERNAL|wxSUNKEN_BORDER,
                const wxString& filter = wxPyEmptyString,
                int defaultFilter = 0,
                const wxString& name = wxPyTreeCtrlNameStr);


    
    // Try to expand as much of the given path as possible.
    virtual bool ExpandPath(const wxString& path);
    // collapse the path
    virtual bool CollapsePath(const wxString& path);

    virtual inline wxString GetDefaultPath() const;
    virtual void SetDefaultPath(const wxString& path);

    // Get dir or filename
    virtual wxString GetPath() const;
    
    // Get selected filename path only (else empty string).
    // I.e. don't count a directory as a selection
    virtual wxString GetFilePath() const;
    virtual void SetPath(const wxString& path);
    
    virtual void ShowHidden( bool show );
    virtual bool GetShowHidden();

    virtual wxString GetFilter() const;
    virtual void SetFilter(const wxString& filter);

    virtual int GetFilterIndex() const;
    virtual void SetFilterIndex(int n);

    virtual wxTreeItemId GetRootId();

    virtual wxPyTreeCtrl* GetTreeCtrl() const;
    virtual wxDirFilterListCtrl* GetFilterListCtrl() const;

    
    // Parse the filter into an array of filters and an array of descriptions
//     virtual int ParseFilter(const wxString& filterStr, wxArrayString& filters, wxArrayString& descriptions);
    
    DocDeclAStr(
        virtual wxTreeItemId, FindChild(wxTreeItemId parentId, const wxString& path, bool& OUTPUT),
        "FindChild(wxTreeItemId parentId, wxString path) -> (item, done)",
        "Find the child that matches the first part of 'path'.  E.g. if a child
path is \"/usr\" and 'path' is \"/usr/include\" then the child for
/usr is returned.  If the path string has been used (we're at the
leaf), done is set to True.
", "");
        
    
    // Resize the components of the control
    virtual void DoResize();
    
    // Collapse & expand the tree, thus re-creating it from scratch:
    virtual void ReCreateTree();

    %property(DefaultPath, GetDefaultPath, SetDefaultPath, doc="See `GetDefaultPath` and `SetDefaultPath`");
    %property(FilePath, GetFilePath, doc="See `GetFilePath`");
    %property(Filter, GetFilter, SetFilter, doc="See `GetFilter` and `SetFilter`");
    %property(FilterIndex, GetFilterIndex, SetFilterIndex, doc="See `GetFilterIndex` and `SetFilterIndex`");
    %property(FilterListCtrl, GetFilterListCtrl, doc="See `GetFilterListCtrl`");
    %property(Path, GetPath, SetPath, doc="See `GetPath` and `SetPath`");
    %property(RootId, GetRootId, doc="See `GetRootId`");
    %property(TreeCtrl, GetTreeCtrl, doc="See `GetTreeCtrl`");
};




MustHaveApp(wxDirFilterListCtrl);

class wxDirFilterListCtrl: public wxChoice
{
public:
    %pythonAppend wxDirFilterListCtrl         "self._setOORInfo(self)"
    %pythonAppend wxDirFilterListCtrl()       ""

    wxDirFilterListCtrl(wxGenericDirCtrl* parent, const wxWindowID id = -1,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0);
    %RenameCtor(PreDirFilterListCtrl, wxDirFilterListCtrl());

    bool Create(wxGenericDirCtrl* parent, const wxWindowID id = -1,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0);

//// Operations
    void FillFilterList(const wxString& filter, int defaultFilter);
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
