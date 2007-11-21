/////////////////////////////////////////////////////////////////////////////
// Name:        foldtestpanel.h
// Purpose:
// Author:      Jorgen Bodde
// Modified by: ABX - 19/12/2004 : possibility of horizontal orientation
//                               : wxWidgets coding standards
// Created:     18/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _FOLDTESTPANEL_H_
#define _FOLDTESTPANEL_H_

/*!
 * Includes
 */

#include "wx/foldbar/foldpanelbar.h"

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxBoxSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_TESTPANEL 10000
#define SYMBOL_FOLDTESTPANEL_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_FOLDTESTPANEL_TITLE _("FoldTestPanel")
#define SYMBOL_FOLDTESTPANEL_IDNAME ID_TESTPANEL
#define SYMBOL_FOLDTESTPANEL_SIZE wxSize(400, 300)
#define SYMBOL_FOLDTESTPANEL_POSITION wxDefaultPosition
#define ID_PANEL 10002
#define ID_CHOICE 10003
#define ID_TEXTCTRL 10016
#define ID_RADIOBUTTON 10004
#define ID_RADIOBUTTON1 10005
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * FoldTestPanel class declaration
 */

class FoldTestPanel: public wxPanel
{
    DECLARE_CLASS( FoldTestPanel )
    DECLARE_EVENT_TABLE()

private:
    wxImageList *m_images;
    wxRect m_oldsize;

    void OnCaptionPanel(wxCaptionBarEvent &event);

public:
    /// Constructors
    FoldTestPanel( );
    FoldTestPanel( wxWindow* parent, wxWindowID id = SYMBOL_FOLDTESTPANEL_IDNAME, const wxString& caption = SYMBOL_FOLDTESTPANEL_TITLE, const wxPoint& pos = SYMBOL_FOLDTESTPANEL_POSITION, const wxSize& size = SYMBOL_FOLDTESTPANEL_SIZE, long style = SYMBOL_FOLDTESTPANEL_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_FOLDTESTPANEL_IDNAME, const wxString& caption = SYMBOL_FOLDTESTPANEL_TITLE, const wxPoint& pos = SYMBOL_FOLDTESTPANEL_POSITION, const wxSize& size = SYMBOL_FOLDTESTPANEL_SIZE, long style = SYMBOL_FOLDTESTPANEL_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin FoldTestPanel event handler declarations

////@end FoldTestPanel event handler declarations

////@begin FoldTestPanel member function declarations


    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end FoldTestPanel member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin FoldTestPanel member variables
    wxBoxSizer* blaat;
////@end FoldTestPanel member variables
};

#endif
    // _FOLDTESTPANEL_H_
