/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/dirctrlg.h
// Purpose:     wxGenericDirCtrl class
//              Builds on wxDirCtrl class written by Robert Roebling for the
//              wxFile application, modified by Harm van der Heijden.
//              Further modified for Windows.
// Author:      Robert Roebling, Harm van der Heijden, Julian Smart et al
// Modified by:
// Created:     21/3/2000
// Copyright:   (c) Robert Roebling, Harm van der Heijden, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRCTRL_H_
#define _WX_DIRCTRL_H_

#if wxUSE_DIRDLG || wxUSE_FILEDLG

#include "wx/treectrl.h"
#include "wx/dialog.h"
#include "wx/dirdlg.h"
#include "wx/choice.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxTextCtrl;
class WXDLLIMPEXP_FWD_BASE wxHashTable;

extern WXDLLIMPEXP_DATA_CORE(const char) wxDirDialogDefaultFolderStr[];

//-----------------------------------------------------------------------------
// Extra styles for wxGenericDirCtrl
//-----------------------------------------------------------------------------

enum
{
    // Only allow directory viewing/selection, no files
    wxDIRCTRL_DIR_ONLY          = 0x0010,
    // When setting the default path, select the first file in the directory
    wxDIRCTRL_SELECT_FIRST      = 0x0020,
    // Show the filter list
    wxDIRCTRL_SHOW_FILTERS      = 0x0040,
    // Use 3D borders on internal controls
    wxDIRCTRL_3D_INTERNAL       = 0x0080,
    // Editable labels
    wxDIRCTRL_EDIT_LABELS       = 0x0100,
    // Allow multiple selection
    wxDIRCTRL_MULTIPLE          = 0x0200,
    // On selection show the entire row, not just the text
    wxDIRCTRL_ROW_HIGHLIGHT     = 0x0400,
    // Use themed representation where possible
    wxDIRCTRL_THEMED            = 0x0800,
    // Use mouse hover highlighting
    wxDIRCTRL_HOVER_HIGHLIGHT   = 0x1000,
    // Show bookbarks/favorites at top
    wxDIRCTRL_BOOKMARKS         = 0x2000,
    // When editing labels, peg the editor width to the maximum
    wxDIRCTRL_FULL_WIDTH_EDITOR = 0x4000,

    wxDIRCTRL_DEFAULT_STYLE  = wxDIRCTRL_3D_INTERNAL
};

//-----------------------------------------------------------------------------
// wxDirItemData
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFavItemData
{
public:
    wxFavItemData(const wxString& path, const wxString& name) : m_path(path), m_name(name) {}
    wxFavItemData(const wxFavItemData& other) : m_path(other.m_path.Clone()), m_name(other.m_name.Clone()) {}
    wxString GetPath() { return m_path; }
    wxString GetName() { return m_name; }
    void SetName(const wxString& name) { m_name = name; }
private:
    wxString m_path, m_name;
};

class WXDLLIMPEXP_CORE wxDirItemData : public wxTreeItemData
{
public:
    wxDirItemData(const wxString& path, const wxString& name, bool isDir, bool isFav, wxFavItemData* fav = nullptr);
    virtual ~wxDirItemData(){}
    void SetNewDirName(const wxString& path);

    bool HasSubDirs() const;
    bool HasFiles(const wxString& spec = wxEmptyString) const;

    bool IsDirectory() { return m_isDir && !m_isFav; }
    bool IsFavorite() { return m_isFav && (m_fav != nullptr); }

    wxString m_path, m_name;
    bool m_isHidden;
    bool m_isExpanded;
    bool m_isDir;
    bool m_isFav;
    wxFavItemData* m_fav;
};

//-----------------------------------------------------------------------------
// wxDirCtrl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxDirFilterListCtrl;

WX_DEFINE_SORTED_ARRAY(wxFavItemData*, wxFavArray);

class WXDLLIMPEXP_CORE wxGenericDirCtrl: public wxPanel
{
public:
    wxGenericDirCtrl();
    wxGenericDirCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
              const wxString &dir = wxASCII_STR(wxDirDialogDefaultFolderStr),
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDIRCTRL_DEFAULT_STYLE,
              const wxString& filter = wxEmptyString,
              int defaultFilter = 0,
              const wxString& name = wxASCII_STR(wxTreeCtrlNameStr));

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
              const wxString &dir = wxASCII_STR(wxDirDialogDefaultFolderStr),
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = wxDIRCTRL_DEFAULT_STYLE,
              const wxString& filter = wxEmptyString,
              int defaultFilter = 0,
              const wxString& name = wxASCII_STR(wxTreeCtrlNameStr) );

    virtual void Init();

    virtual ~wxGenericDirCtrl();

    void OnExpandItem(wxTreeEvent &event );
    void OnCollapseItem(wxTreeEvent &event );
    void OnBeginEditItem(wxTreeEvent &event );
    void OnDoingEditItem(wxTreeEvent & event );
    void OnEndEditItem(wxTreeEvent &event );
    void OnTreeSelChange(wxTreeEvent &event);
    void OnItemActivated(wxTreeEvent &event);
    void OnSize(wxSizeEvent &event );

    // Try to expand as much of the given path as possible.
    virtual bool ExpandPath(const wxString& path);
    // collapse the path
    virtual bool CollapsePath(const wxString& path);

    // Accessors

    virtual inline wxString GetDefaultPath() const { return m_defaultPath; }
    virtual void SetDefaultPath(const wxString& path) { m_defaultPath = path; }

    // Get dir or filename
    virtual wxString GetPath() const;
    virtual void GetPaths(wxArrayString& paths) const;

    // Get selected filename path only (else empty string).
    // I.e. don't count a directory as a selection
    virtual wxString GetFilePath() const;
    virtual void GetFilePaths(wxArrayString& paths) const;
    virtual void SetPath(const wxString& path);

    virtual void SelectPath(const wxString& path, bool select = true);
    virtual void SelectPaths(const wxArrayString& paths);

    virtual void ShowHidden( bool show );
    virtual bool GetShowHidden() { return m_showHidden; }

    virtual wxString GetFilter() const { return m_filter; }
    virtual void SetFilter(const wxString& filter);

    virtual int GetFilterIndex() const { return m_currentFilter; }
    virtual void SetFilterIndex(int n);

    virtual wxTreeItemId GetRootId() { return m_rootId; }

    virtual wxTreeCtrl* GetTreeCtrl() const { return m_treeCtrl; }
    virtual wxDirFilterListCtrl* GetFilterListCtrl() const { return m_filterListCtrl; }

    virtual void UnselectAll();

    // Helper
    virtual void SetupSections();

    // Find the child that matches the first part of 'path'.
    // E.g. if a child path is "/usr" and 'path' is "/usr/include"
    // then the child for /usr is returned.
    // If the path string has been used (we're at the leaf), done is set to true
    virtual wxTreeItemId FindChild(wxTreeItemId parentId, const wxString& path, bool& done);

    wxString GetPath(wxTreeItemId itemId) const;

    // Resize the components of the control
    virtual void DoResize();

    // Collapse & expand the tree, thus re-creating it from scratch:
    virtual void ReCreateTree();

    // Collapse the entire tree
    virtual void CollapseTree();

    // Scroll the tree control so as to show the selected dir on top
    virtual void SelectedToTop();

    // Add a favorite to the tree
    wxFavItemData* AddFavorite(const wxString& path, const wxString& name);

    // Start editing a favorite
    void EditFavorite(wxFavItemData* item);

    // Get the path from a favorite
    wxString GetFavoritePath(size_t index);

    // Get the name from a favorite
    wxString GetFavoriteName(size_t index);

    // True if the item is a favorite
    bool IsFavorite(const wxTreeItemId& item);

    // True if the item is a directory
    bool IsDirectory(const wxTreeItemId& item);

    // Get the favorite count
    size_t GetFavoriteCount();

    // Remove a favorite
    void RemoveFavorite(wxFavItemData* item);

    // Get the currently selected favorite, if any; nullptr otherwise
    wxFavItemData* GetFavorite();
protected:
    virtual void ExpandRoot();
    virtual void ExpandDir(wxTreeItemId parentId);
    virtual void CollapseDir(wxTreeItemId parentId);
    virtual const wxTreeItemId AddSection(const wxString& path, const wxString& name, int imageId = 0, bool isFav = false);
    virtual wxTreeItemId AppendItem (const wxTreeItemId & parent,
                const wxString & text,
                int image = -1, int selectedImage = -1,
                wxTreeItemData * data = NULL);
    //void FindChildFiles(wxTreeItemId id, int dirFlags, wxArrayString& filenames);
    virtual wxTreeCtrl* CreateTreeCtrl(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long treeStyle);

    // Extract description and actual filter from overall filter string
    bool ExtractWildcard(const wxString& filterStr, int n, wxString& filter, wxString& description);

    // Find a favorite tree id
    wxTreeItemId FindFavoriteInTree(wxFavItemData* item);
    // Update the tree with the new favorites
    void UpdateFavoriteBranch();
private:
    void PopulateNode(wxTreeItemId node);
    wxDirItemData* GetItemData(wxTreeItemId itemId) const;

    bool            m_showHidden;
    wxTreeItemId    m_rootId;
    wxTreeItemId    m_favId;
    wxString        m_defaultPath; // Starting path
    long            m_styleEx; // Extended style
    wxString        m_filter;  // Wildcards in same format as per wxFileDialog
    int             m_currentFilter; // The current filter index
    wxString        m_currentFilterStr; // Current filter string
    wxTreeCtrl*     m_treeCtrl;
    wxDirFilterListCtrl* m_filterListCtrl;
    wxFavArray      m_favs;
    wxFavItemData*  m_favInEdit;

private:
    wxDECLARE_EVENT_TABLE();
    wxDECLARE_DYNAMIC_CLASS(wxGenericDirCtrl);
    wxDECLARE_NO_COPY_CLASS(wxGenericDirCtrl);
};

wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_DIRCTRL_SELECTIONCHANGED, wxTreeEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_DIRCTRL_FILEACTIVATED, wxTreeEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_DIRCTRL_DIRACTIVATED, wxTreeEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_DIRCTRL_FAVACTIVATED, wxTreeEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_DIRCTRL_FAVEDITED, wxTreeEvent );
wxDECLARE_EXPORTED_EVENT( WXDLLIMPEXP_CORE, wxEVT_DIRCTRL_EDITING, wxTreeEvent );

#define wx__DECLARE_DIRCTRL_EVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_DIRCTRL_ ## evt, id, wxTreeEventHandler(fn))

#define EVT_DIRCTRL_SELECTIONCHANGED(id, fn) wx__DECLARE_DIRCTRL_EVT(SELECTIONCHANGED, id, fn)
#define EVT_DIRCTRL_FILEACTIVATED(id, fn) wx__DECLARE_DIRCTRL_EVT(FILEACTIVATED, id, fn)
#define EVT_DIRCTRL_DIRACTIVATED(id, fn) wx__DECLARE_DIRCTRL_EVT(DIRACTIVATED, id, fn)
#define EVT_DIRCTRL_FAVACTIVATED(id, fn) wx__DECLARE_DIRCTRL_EVT(FAVACTIVATED, id, fn)
#define EVT_DIRCTRL_FAVEDITED(id, fn) wx__DECLARE_DIRCTRL_EVT(FAVEDITED, id, fn)
#define EVT_DIRCTRL_EDITING(id, fn) wx__DECLARE_DIRCTRL_EVT(EDITING, id, fn)

//-----------------------------------------------------------------------------
// wxDirFilterListCtrl
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDirFilterListCtrl: public wxChoice
{
public:
    wxDirFilterListCtrl() { Init(); }
    wxDirFilterListCtrl(wxGenericDirCtrl* parent, wxWindowID id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0)
    {
        Init();
        Create(parent, id, pos, size, style);
    }

    bool Create(wxGenericDirCtrl* parent, wxWindowID id = wxID_ANY,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              long style = 0);

    void Init();

    virtual ~wxDirFilterListCtrl() {}

    //// Operations
    void FillFilterList(const wxString& filter, int defaultFilter);

    //// Events
    void OnSelFilter(wxCommandEvent& event);

protected:
    wxGenericDirCtrl*    m_dirCtrl;

    wxDECLARE_EVENT_TABLE();
    wxDECLARE_CLASS(wxDirFilterListCtrl);
    wxDECLARE_NO_COPY_CLASS(wxDirFilterListCtrl);
};

#if !defined(__WXMSW__) && !defined(__WXMAC__)
    #define wxDirCtrl wxGenericDirCtrl
#endif

// Symbols for accessing individual controls
#define wxID_TREECTRL          7000
#define wxID_FILTERLISTCTRL    7001

#endif // wxUSE_DIRDLG

//-------------------------------------------------------------------------
// wxFileIconsTable - use wxTheFileIconsTable which is created as necessary
//-------------------------------------------------------------------------

#if wxUSE_DIRDLG || wxUSE_FILEDLG || wxUSE_FILECTRL

class WXDLLIMPEXP_FWD_CORE wxImageList;

class WXDLLIMPEXP_CORE wxFileIconsTable
{
public:
    wxFileIconsTable();
    ~wxFileIconsTable();

    enum iconId_Type
    {
        folder,
        folder_open,
        computer,
        drive,
        cdrom,
        floppy,
        removeable,
        file,
        executable,
        bookmark,
        bookmark_folder
    };

    int GetIconID(const wxString& extension, const wxString& mime = wxEmptyString);
    wxImageList *GetSmallImageList();

    const wxSize& GetSize() const { return m_size; }
    void SetSize(const wxSize& sz) { m_size = sz; }

    bool IsOk() const { return m_smallImageList != NULL; }

protected:
    void Create(const wxSize& sz);  // create on first use

    wxImageList *m_smallImageList;
    wxHashTable *m_HashTable;
    wxSize  m_size;
};

// The global fileicons table
extern WXDLLIMPEXP_DATA_CORE(wxFileIconsTable *) wxTheFileIconsTable;

#endif // wxUSE_DIRDLG || wxUSE_FILEDLG || wxUSE_FILECTRL

// old wxEVT_COMMAND_* constants
#define wxEVT_COMMAND_DIRCTRL_SELECTIONCHANGED  wxEVT_DIRCTRL_SELECTIONCHANGED
#define wxEVT_COMMAND_DIRCTRL_FILEACTIVATED     wxEVT_DIRCTRL_FILEACTIVATED
#define wxEVT_COMMAND_DIRCTRL_DIRACTIVATED      wxEVT_DIRCTRL_DIRACTIVATED
#define wxEVT_COMMAND_DIRCTRL_FAVACTIVATED      wxEVT_DIRCTRL_FAVACTIVATED
#define wxEVT_COMMAND_DIRCTRL_FAVEDITED         wxEVT_DIRCTRL_FAVEDITED
#define wxEVT_COMMAND_DIRCTRL_EDITING           wxEVT_DIRCTRL_EDITING

#endif // _WX_DIRCTRLG_H_
