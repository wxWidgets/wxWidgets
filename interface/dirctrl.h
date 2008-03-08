/////////////////////////////////////////////////////////////////////////////
// Name:        dirctrl.h
// Purpose:     documentation for wxGenericDirCtrl class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxGenericDirCtrl
    @wxheader{dirctrl.h}
    
    This control can  be used to place a directory listing (with optional files) on
    an arbitrary window.
    
    The control contains a wxTreeCtrl window representing the directory
    hierarchy, and optionally, a wxChoice window containing a list of filters.
    
    @library{wxbase}
    @category{ctrl}
    @appearance{genericdirctrl.png}
*/
class wxGenericDirCtrl : public wxControl
{
public:
    //@{
    /**
        Main constructor.
        
        @param parent 
        Parent window.
        
        @param id 
        Window identifier.
        
        @param dir 
        Initial folder.
        
        @param pos 
        Position.
        
        @param size 
        Size.
        
        @param style 
        Window style. Please see wxGenericDirCtrl for a list of possible styles.
        
        @param filter 
        A filter string, using the same syntax as that for wxFileDialog. This may be
        empty if filters
        are not being used.
        
        Example: "All files (*.*)|*.*|JPEG files (*.jpg)|*.jpg"
        
        @param defaultFilter 
        The zero-indexed default filter setting.
        
        @param name 
        The window name.
    */
    wxGenericDirCtrl();
        wxGenericDirCtrl(wxWindow* parent, const wxWindowID id = -1,
                         const wxString& dir = wxDirDialogDefaultFolderStr,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxDIRCTRL_3D_INTERNAL|wxBORDER_SUNKEN,
                         const wxString& filter = wxEmptyString,
                         int defaultFilter = 0,
                         const wxString& name = wxTreeCtrlNameStr);
    //@}

    /**
        Destructor.
    */
    ~wxGenericDirCtrl();

    /**
        Collapse the given path.
    */
    bool CollapsePath(const wxString& path);

    /**
        Collapses the entire tree.
    */
    void CollapseTree();

    /**
        Create function for two-step construction. See wxGenericDirCtrl() for details.
    */
    bool Create(wxWindow* parent, const wxWindowID id = -1,
                const wxString& dir = wxDirDialogDefaultFolderStr,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDIRCTRL_3D_INTERNAL|wxBORDER_SUNKEN,
                const wxString& filter = wxEmptyString,
                int defaultFilter = 0,
                const wxString& name = wxTreeCtrlNameStr);

    /**
        Tries to expand as much of the given path as possible, so that the filename or
        directory is visible in the tree control.
    */
    bool ExpandPath(const wxString& path);

    /**
        Gets the default path.
    */
    wxString GetDefaultPath();

    /**
        Gets selected filename path only (else empty string).
        
        This function doesn't count a directory as a selection.
    */
    wxString GetFilePath();

    /**
        Returns the filter string.
    */
    wxString GetFilter();

    /**
        Returns the current filter index (zero-based).
    */
    int GetFilterIndex();

    /**
        Returns a pointer to the filter list control (if present).
    */
    wxDirFilterListCtrl* GetFilterListCtrl();

    /**
        Gets the currently-selected directory or filename.
    */
    wxString GetPath();

    /**
        Returns the root id for the tree control.
    */
    wxTreeItemId GetRootId();

    /**
        Returns a pointer to the tree control.
    */
    wxTreeCtrl* GetTreeCtrl();

    /**
        Initializes variables.
    */
    void Init();

    /**
        Collapse and expand the tree, thus re-creating it from scratch.
        May be used to update the displayed directory content.
    */
    void ReCreateTree();

    /**
        Sets the default path.
    */
    void SetDefaultPath(const wxString& path);

    /**
        Sets the filter string.
    */
    void SetFilter(const wxString& filter);

    /**
        Sets the current filter index (zero-based).
    */
    void SetFilterIndex(int n);

    /**
        Sets the current path.
    */
    void SetPath(const wxString& path);

    /**
        @param show 
        If @true, hidden folders and files will be displayed by the
        control. If @false, they will not be displayed.
    */
    void ShowHidden(bool show);
};
