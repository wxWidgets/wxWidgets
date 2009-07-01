///////////////////////////////////////////////////////////////////////////////
// Name:        ribbon/panel.h
// Purpose:     interface of wxRibbonPage
// Author:      Peter Cawley
// RCS-ID:      $Id$
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    @class wxRibbonPanel

    Serves as a container for a group of (ribbon) controls. A wxRibbonPage will
    typically have panels for children, with the controls for that page placed
    on the panels.
    
    A panel adds a border and label to a group of controls, and can be
    minimised (either automatically to conserve space, or manually by the user).
    
    @sa wxRibbonPage
    
    @beginStyleTable
    @style{wxRIBBON_PANEL_DEFAULT_STYLE}
           Defined as no other flags set.
    @style{wxRIBBON_PANEL_NO_AUTO_MINIMISE}
           Prevents the panel from automatically minimising to conserve screen
           space.
    @style{wxRIBBON_PANEL_EXT_BUTTON}
           Causes an extension button to be shown in the panel's chrome (if the
           bar in which it is contained has wxRIBBON_BAR_SHOW_PANEL_EXT_BUTTONS
           set). The behaviour of this button is application controlled, but
           typically will show an extended drop-down menu relating to the
           panel.
    @style{wxRIBBON_PANEL_MINIMISE_BUTTON}
           Causes a (de)minimise button to be shown in the panel's chrome (if
           the bar in which it is contained has the
           wxRIBBON_BAR_SHOW_PANEL_MINIMISE_BUTTONS style set). This flag is
           typically combined with wxRIBBON_PANEL_NO_AUTO_MINIMISE to make a
           panel which the user always has manual control over when it
           minimises.
    @endStyleTable

    @library{wxribbon}
    @category{ribbon}
*/
class wxRibbonPanel : public wxRibbonControl
{
public:
	wxRibbonPanel();

	wxRibbonPanel(wxWindow* parent,
				  wxWindowID id = wxID_ANY,
				  const wxString& label = wxEmptyString,
				  const wxBitmap& minimised_icon = wxNullBitmap,
				  const wxPoint& pos = wxDefaultPosition,
				  const wxSize& size = wxDefaultSize,
				  long style = wxRIBBON_PANEL_DEFAULT_STYLE);

	virtual ~wxRibbonPanel();

	bool Create(wxWindow* parent,
				wxWindowID id = wxID_ANY,
				const wxString& label = wxEmptyString,
				const wxBitmap& icon = wxNullBitmap,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxRIBBON_PANEL_DEFAULT_STYLE);

	wxBitmap& GetMinimisedIcon();

	void SetArtProvider(wxRibbonArtProvider* art);
};
