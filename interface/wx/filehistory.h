/////////////////////////////////////////////////////////////////////////////
// Name:        filehistory.h
// Purpose:     wxFileHistory class
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    Styles for the paths shown in wxFileHistory menus.

    The default style is wxFH_HIDE_CURRENT_PATH, i.e. the path of the file is
    only shown in the menu if it's different from the path of the first file.

    @since 3.1.5
*/
enum wxFileHistoryMenuPathStyle
{
    /** Hide the file path if it matches the path of the first item */
    wxFH_HIDE_CURRENT_PATH,

    /** Hide all paths and show only filenames */
    wxFH_HIDE_ALL_PATHS,

    /** Show the full path for all files */
    wxFH_SHOW_FULL_PATH
};

/**
    @class wxFileHistory

    The wxFileHistory encapsulates a user interface convenience, the list of
    most recently visited files as shown on a menu (usually the File menu).

    wxFileHistory can manage one or more file menus. More than one menu may be
    required in an MDI application, where the file history should appear on
    each MDI child menu as well as the MDI parent frame.

    @library{wxcore}
    @category{docview}

    @see @ref overview_docview, wxDocManager
*/
class wxFileHistory : public wxObject
{
public:
    /**
        Constructor. Pass the maximum number of files that should be stored and
        displayed.

        @a idBase defaults to wxID_FILE1 and represents the id given to the
        first history menu item. Since menu items can't share the same ID you
        should change @a idBase (to one of your own defined IDs) when using
        more than one wxFileHistory in your application.
    */
    wxFileHistory(size_t maxFiles = 9, wxWindowID idBase = wxID_FILE1);

    /**
        Destructor.
    */
    virtual ~wxFileHistory();

    /**
        Adds a file to the file history list, if the object has a pointer to an
        appropriate file menu.
    */
    virtual void AddFileToHistory(const wxString& filename);

    /**
        Appends the files in the history list, to all menus managed by the file
        history object.
    */
    virtual void AddFilesToMenu();
    /**
        Appends the files in the history list, to the given menu only.
    */
    virtual void AddFilesToMenu(wxMenu* menu);

    /**
        Returns the base identifier for the range used for appending items.
    */
    wxWindowID GetBaseId() const;

    /**
        Returns the number of files currently stored in the file history.
    */
    virtual size_t GetCount() const;

    /**
        Returns the file at this index (zero-based).
    */
    virtual wxString GetHistoryFile(size_t index) const;

    /**
        Returns the maximum number of files that can be stored.
    */
    virtual int GetMaxFiles() const;

    /**
        Returns the list of menus that are managed by this file history object.

        @see UseMenu()
    */
    const wxList& GetMenus() const;

    /**
        Loads the file history from the given config object. This function
        should be called explicitly by the application.

        @see wxConfigBase
    */
    virtual void Load(const wxConfigBase& config);

    /**
        Refresh the labels on all the menu items in the menus used by this
        file history.

        @since 3.1.5
    */
    void RefreshLabels();

    /**
        Removes the specified file from the history.
    */
    virtual void RemoveFileFromHistory(size_t i);

    /**
        Removes this menu from the list of those managed by this object.
    */
    virtual void RemoveMenu(wxMenu* menu);

    /**
        Saves the file history into the given config object. This must be
        called explicitly by the application.

        @see wxConfigBase
    */
    virtual void Save(wxConfigBase& config);

    /**
        Sets the base identifier for the range used for appending items.
    */
    void SetBaseId(wxWindowID baseId);

    /**
        Adds this menu to the list of those menus that are managed by this file
        history object. Also see AddFilesToMenu() for initializing the menu
        with filenames that are already in the history when this function is
        called, as this is not done automatically.
    */
    virtual void UseMenu(wxMenu* menu);

    /**
        Set the style of the menu item labels.

        By default, the menu item label style is ::wxFH_HIDE_CURRENT_PATH.

        @remarks Use RefreshLabels() to update any existing menu items to the new style.
        @since 3.1.5
    */
    void SetMenuPathStyle(wxFileHistoryMenuPathStyle style);

    /**
        Get the current style of the menu item labels.

        Initially returns ::wxFH_HIDE_CURRENT_PATH.

        @see SetMenuPathStyle()

        @since 3.1.5
    */
    wxFileHistoryMenuPathStyle GetMenuPathStyle() const;


};
