/////////////////////////////////////////////////////////////////////////////
// Name:        html/helpfrm.h
// Purpose:     documentation for wxHtmlHelpFrame class
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxHtmlHelpFrame
    @headerfile helpfrm.h wx/html/helpfrm.h
    
    This class is used by wxHtmlHelpController 
    to display help.
    It is an internal class and should not be used directly - except for the case
    when you're writing your own HTML help controller.
    
    @library{wxhtml}
    @category{FIXME}
*/
class wxHtmlHelpFrame : public wxFrame
{
public:
    //@{
    /**
        Constructor. For the values of @e style, please see the documentation for
        wxHtmlHelpController.
    */
    wxHtmlHelpFrame(wxHtmlHelpData* data = @NULL);
        wxHtmlHelpFrame(wxWindow* parent, int wxWindowID,
                        const wxString& title = wxEmptyString,
                        int style = wxHF_DEFAULT_STYLE,
                        wxHtmlHelpData* data = @NULL);
    //@}

    /**
        You may override this virtual method to add more buttons to the help window's
        toolbar. @e toolBar is a pointer to the toolbar and @e style is the style
        flag as passed to the Create method.
        
        wxToolBar::Realize is called immediately after returning from this function.
    */
    virtual void AddToolbarButtons(wxToolBar * toolBar, int style);

    /**
        Creates the frame. See @ref wxhtmlhelpframe() "the constructor"
        for a description of the parameters.
    */
    bool Create(wxWindow* parent, wxWindowID id,
                const wxString& title = wxEmptyString,
                int style = wxHF_DEFAULT_STYLE);

    /**
        Returns the help controller associated with the frame.
    */
    wxHtmlHelpController* GetController();

    /**
        Reads the user's settings for this frame see
        wxHtmlHelpController::ReadCustomization)
    */
    void ReadCustomization(wxConfigBase* cfg,
                           const wxString& path = wxEmptyString);

    /**
        Sets the help controller associated with the frame.
    */
    void SetController(wxHtmlHelpController* contoller);

    /**
        Sets the frame's title format. @e format must contain exactly one "%s"
        (it will be replaced by the page title).
    */
    void SetTitleFormat(const wxString& format);

    /**
        Saves the user's settings for this frame (see
        wxHtmlHelpController::WriteCustomization).
    */
    void WriteCustomization(wxConfigBase* cfg,
                            const wxString& path = wxEmptyString);
};
