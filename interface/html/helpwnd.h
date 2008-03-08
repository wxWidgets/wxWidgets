/////////////////////////////////////////////////////////////////////////////
// Name:        html/helpwnd.h
// Purpose:     documentation for wxHtmlHelpWindow class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHtmlHelpWindow
    @headerfile helpwnd.h wx/html/helpwnd.h

    This class is used by wxHtmlHelpController
    to display help within a frame or dialog, but you can use it yourself to create
    an embedded HTML help window.

    For example:

    @code
    // m_embeddedHelpWindow is a wxHtmlHelpWindow
        // m_embeddedHtmlHelp is a wxHtmlHelpController

        // Create embedded HTML Help window
        m_embeddedHelpWindow = new wxHtmlHelpWindow;
        m_embeddedHtmlHelp.UseConfig(config, rootPath); // Set your own config
    object here
        m_embeddedHtmlHelp.SetHelpWindow(m_embeddedHelpWindow);
        m_embeddedHelpWindow-Create(this,
            wxID_ANY, wxDefaultPosition, GetClientSize(),
    wxTAB_TRAVERSAL|wxBORDER_NONE, wxHF_DEFAULT_STYLE);
        m_embeddedHtmlHelp.AddBook(wxFileName(_T("doc.zip")));
    @endcode

    You should pass the style wxHF_EMBEDDED to the style parameter of
    wxHtmlHelpController to allow
    the embedded window to be destroyed independently of the help controller.

    @library{wxhtml}
    @category{FIXME}
*/
class wxHtmlHelpWindow : public wxWindow
{
public:
    //@{
    /**
        Constructor.
        
        Constructor. For the values of @e helpStyle, please see the documentation for
        wxHtmlHelpController.
    */
    wxHtmlHelpWindow(wxHtmlHelpData* data = @NULL);
    wxHtmlHelpWindow(wxWindow* parent, int wxWindowID,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& pos = wxDefaultSize,
                     int style = wxTAB_TRAVERSAL|wxBORDER_NONE,
                     int helpStyle = wxHF_DEFAULT_STYLE,
                     wxHtmlHelpData* data = @NULL);
    //@}

    /**
        You may override this virtual method to add more buttons to the help window's
        toolbar. @e toolBar is a pointer to the toolbar and @e style is the style
        flag as passed to the Create method.
        
        wxToolBar::Realize is called immediately after returning from this function.
        
        See @e samples/html/helpview for an example.
    */
    virtual void AddToolbarButtons(wxToolBar * toolBar, int style);

    /**
        Creates the help window. See @ref wxhtmlhelpwindow() "the constructor"
        for a description of the parameters.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& pos = wxDefaultSize,
                int style = wxTAB_TRAVERSAL|wxBORDER_NONE,
                int helpStyle = wxHF_DEFAULT_STYLE,
                wxHtmlHelpData* data = @NULL);

    /**
        Creates contents panel. (May take some time.)
        
        Protected.
    */
    void CreateContents();

    /**
        Creates index panel. (May take some time.)
        
        Protected.
    */
    void CreateIndex();

    /**
        Creates search panel.
    */
    void CreateSearch();

    //@{
    /**
        Displays page x. If not found it will give the user the choice of
        searching books.
        Looking for the page runs in these steps:
        
         try to locate file named x (if x is for example "doc/howto.htm")
         try to open starting page of book x
         try to find x in contents (if x is for example "How To ...")
         try to find x in index (if x is for example "How To ...")
        
        The second form takes numeric ID as the parameter.
        (uses extension to MS format, param name="ID" value=id)
    */
    bool Display(const wxString& x);
    bool Display(const int id);
    //@}

    /**
        Displays contents panel.
    */
    bool DisplayContents();

    /**
        Displays index panel.
    */
    bool DisplayIndex();

    /**
        Returns the wxHtmlHelpData object, which is usually a pointer to the
        controller's data.
    */
    wxHtmlHelpData* GetData();

    /**
        Search for given keyword. Optionally it searches through the index (mode =
        wxHELP_SEARCH_INDEX), default the content (mode = wxHELP_SEARCH_ALL).
    */
    bool KeywordSearch(const wxString& keyword,
                       wxHelpSearchMode mode = wxHELP_SEARCH_ALL);

    /**
        Reads the user's settings for this window (see
        wxHtmlHelpController::ReadCustomization)
    */
    void ReadCustomization(wxConfigBase* cfg,
                           const wxString& path = wxEmptyString);

    /**
        Refresh all panels. This is necessary if a new book was added.
        
        Protected.
    */
    void RefreshLists();

    /**
        Sets the frame's title format. @e format must contain exactly one "%s"
        (it will be replaced by the page title).
    */
    void SetTitleFormat(const wxString& format);

    /**
        Associates a wxConfig object with the help window. It is recommended that you
        use wxHtmlHelpController::UseConfig instead.
    */
    void UseConfig(wxConfigBase* config,
                   const wxString& rootpath = wxEmptyString);

    /**
        Saves the user's settings for this window(see
        wxHtmlHelpController::WriteCustomization).
    */
    void WriteCustomization(wxConfigBase* cfg,
                            const wxString& path = wxEmptyString);
};
