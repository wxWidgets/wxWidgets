/////////////////////////////////////////////////////////////////////////////
// Name:        html/helpctrl.h
// Purpose:     interface of wxHtmlHelpController
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHtmlHelpController
    @headerfile helpctrl.h wx/html/helpctrl.h

    This help controller provides an easy way of displaying HTML help in your
    application (see @e test sample). The help system is based on @b books
    (see wxHtmlHelpController::AddBook). A book is a logical
    section of documentation (for example "User's Guide" or "Programmer's Guide" or
    "C++ Reference" or "wxWidgets Reference"). The help controller can handle as
    many books as you want.

    Although this class has an API compatible with other wxWidgets
    help controllers as documented by wxHelpController, it
    is recommended that you use the enhanced capabilities of wxHtmlHelpController's
    API.

    wxHTML uses Microsoft's HTML Help Workshop project files (.hhp, .hhk, .hhc) as
    its
    native format. The file format is described here().
    Have a look at docs/html/ directory where sample project files are stored.

    You can use Tex2RTF to produce these files when generating HTML, if you set @b
    htmlWorkshopFiles to @b @true in
    your tex2rtf.ini file. The commercial tool HelpBlocks (www.helpblocks.com) can
    also create these files.

    @library{wxhtml}
    @category{help}

    @see @ref overview_wxhelpcontroller "Information about wxBestHelpController",
    wxHtmlHelpFrame, wxHtmlHelpDialog, wxHtmlHelpWindow, wxHtmlModalHelp
*/
class wxHtmlHelpController
{
public:
    /**
        Constructor.
    */
    wxHtmlHelpController(int style = wxHF_DEFAULT_STYLE,
                         wxWindow* parentWindow = NULL);

    //@{
    /**
        Adds book (@ref overview_helpformat ".hhp file" - HTML Help Workshop project
        file) into the list of loaded books.
        This must be called at least once before displaying  any help.
        @a bookFile or @a bookUrl  may be either .hhp file or ZIP archive
        that contains arbitrary number of .hhp files in
        top-level directory. This ZIP archive must have .zip or .htb extension
        (the latter stands for "HTML book"). In other words, @c
        AddBook(wxFileName("help.zip"))
        is possible and is the recommended way.
        
        @param showWaitMsg
            If @true then a decoration-less window with progress message is displayed.
        @param bookFile
            Help book filename. It is recommended to use this prototype
            instead of the one taking URL, because it is less error-prone.
        @param bookUrl
            Help book URL (note that syntax of filename and URL is
            different on most platforms)
    */
    bool AddBook(const wxFileName& bookFile, bool showWaitMsg);
    bool AddBook(const wxString& bookUrl, bool showWaitMsg);
    //@}

    /**
        This protected virtual method may be overridden so that when specifying the
        wxHF_DIALOG style, the controller
        uses a different dialog.
    */
    virtual wxHtmlHelpDialog* CreateHelpDialog(wxHtmlHelpData* data);

    /**
        This protected virtual method may be overridden so that the controller
        uses a different frame.
    */
    virtual wxHtmlHelpFrame* CreateHelpFrame(wxHtmlHelpData* data);

    //@{
    /**
        This alternative form is used to search help contents by numeric IDs.
    */
    void Display(const wxString& x);
    void Display(const int id);
    //@}

    /**
        Displays help window and focuses contents panel.
    */
    void DisplayContents();

    /**
        Displays help window and focuses index panel.
    */
    void DisplayIndex();

    /**
        Displays help window, focuses search panel and starts searching.  Returns @true
        if the keyword was found. Optionally it searches through the index (mode =
        wxHELP_SEARCH_INDEX), default the content (mode = wxHELP_SEARCH_ALL).
        @b Important: KeywordSearch searches only pages listed in .hhc file(s).
        You should list all pages in the contents file.
    */
    bool KeywordSearch(const wxString& keyword,
                       wxHelpSearchMode mode = wxHELP_SEARCH_ALL);

    /**
        Reads the controller's setting (position of window, etc.)
    */
    void ReadCustomization(wxConfigBase* cfg,
                           wxString path = wxEmptyString);

    /**
        Sets the path for storing temporary files - cached binary versions of index and
        contents files. These binary
        forms are much faster to read. Default value is empty string (empty string means
        that no cached data are stored). Note that these files are @e not
        deleted when program exits.
        Once created these cached files will be used in all subsequent executions
        of your application. If cached files become older than corresponding .hhp
        file (e.g. if you regenerate documentation) it will be refreshed.
    */
    void SetTempDir(const wxString& path);

    /**
        Sets format of title of the frame. Must contain exactly one "%s"
        (for title of displayed HTML page).
    */
    void SetTitleFormat(const wxString& format);

    /**
        Associates @a config object with the controller.
        If there is associated config object, wxHtmlHelpController automatically
        reads and writes settings (including wxHtmlWindow's settings) when needed.
        The only thing you must do is create wxConfig object and call UseConfig.
        If you do not use @e UseConfig, wxHtmlHelpController will use
        default wxConfig object if available (for details see
        wxConfigBase::Get and
        wxConfigBase::Set).
    */
    void UseConfig(wxConfigBase* config,
                   const wxString& rootpath = wxEmptyString);

    /**
        Stores controllers setting (position of window etc.)
    */
    void WriteCustomization(wxConfigBase* cfg,
                            wxString path = wxEmptyString);
};



/**
    @class wxHtmlModalHelp
    @headerfile helpctrl.h wx/html/helpctrl.h

    This class uses wxHtmlHelpController
    to display help in a modal dialog. This is useful on platforms such as wxMac
    where if you display help from a modal dialog, the help window must itself be a
    modal
    dialog.

    Create objects of this class on the stack, for example:

    @code
    // The help can be browsed during the lifetime of this object; when the user
    quits
        // the help, program execution will continue.
        wxHtmlModalHelp help(parent, wxT("help"), wxT("My topic"));
    @endcode

    @library{wxhtml}
    @category{FIXME}
*/
class wxHtmlModalHelp
{
public:
    /**
        @param parent
            is the parent of the dialog.
        @param helpFile
            is the HTML help file to show.
        @param topic
            is an optional topic. If this is empty, the help contents will be shown.
        @param style
            is a combination of the flags described in the wxHtmlHelpController
        documentation.
    */
    wxHtmlModalHelp(wxWindow* parent, const wxString& helpFile,
                    const wxString& topic = wxEmptyString,
                    int style = wxHF_DEFAULT_STYLE | wxHF_DIALOG | wxHF_MODAL);
};

