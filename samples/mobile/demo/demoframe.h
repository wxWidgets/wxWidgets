/////////////////////////////////////////////////////////////////////////////
// Name:        demoframe.h
// Purpose:     
// Author:      Julian Smart
// Modified by: 
// Created:     12/05/2009 12:30:14
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _DEMOFRAME_H_
#define _DEMOFRAME_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DEMOFRAME 10000
#define ID_PANEL 10001
#define ID_TEXTCTRL 10002
#define ID_BUTTON 10003
#define ID_BITMAPBUTTON 10004
#define SYMBOL_DEMOFRAME_STYLE 0
#define SYMBOL_DEMOFRAME_TITLE _("DemoFrame")
#define SYMBOL_DEMOFRAME_IDNAME ID_DEMOFRAME
#define SYMBOL_DEMOFRAME_SIZE wxSize(400, 300)
#define SYMBOL_DEMOFRAME_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * DemoFrame class declaration
 */

class DemoFrame: public wxFrame
{
public:
    /// Constructors
    DemoFrame();
    DemoFrame(wxWindow* parent,
              wxWindowID id = SYMBOL_DEMOFRAME_IDNAME,
              const wxString& caption = SYMBOL_DEMOFRAME_TITLE,
              const wxPoint& pos = SYMBOL_DEMOFRAME_POSITION,
              const wxSize& size = SYMBOL_DEMOFRAME_SIZE,
              long style = SYMBOL_DEMOFRAME_STYLE );

    bool Create(wxWindow* parent,
                wxWindowID id = SYMBOL_DEMOFRAME_IDNAME,
                const wxString& caption = SYMBOL_DEMOFRAME_TITLE,
                const wxPoint& pos = SYMBOL_DEMOFRAME_POSITION,
                const wxSize& size = SYMBOL_DEMOFRAME_SIZE,
                long style = SYMBOL_DEMOFRAME_STYLE );

    /// Destructor
    ~DemoFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

#if 0
    /// Show an action sheet
    void OnShowActionSheet(wxCommandEvent& event);

    /// Show an alert sheet
    void OnShowAlertSheet(wxCommandEvent& event);

    /// Respond to action sheet events
    void OnActionSheet(wxSheetEvent& event);

    /// Respond to table events
    void OnAccessoryClick(wxTableCtrlEvent& event);
    void OnSelectRow(wxTableCtrlEvent& event);
#endif  // 0

////@begin DemoFrame event handler declarations

////@end DemoFrame event handler declarations

////@begin DemoFrame member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DemoFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();
    
private:
    DECLARE_EVENT_TABLE()

////@begin DemoFrame member variables
////@end DemoFrame member variables
};

#endif
    // _DEMOFRAME_H_
