/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtextimagepage.h
// Purpose:     
// Author:      Mingquan Yang
// Modified by: 
// Created:     Wed 02 Jun 2010 11:27:23 CST
// RCS-ID:      
// Copyright:   (c) Mingquan Yang
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _RICHTEXTIMAGEPAGE_H_
#define _RICHTEXTIMAGEPAGE_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx/richtext/richtextimagepage.h"
#endif

/*!
 * Includes
 */

////@begin includes
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
#define SYMBOL_WXRICHTEXTIMAGEPAGE_STYLE wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxTAB_TRAVERSAL
#define SYMBOL_WXRICHTEXTIMAGEPAGE_TITLE _("wxRichTextImagePage")
#define SYMBOL_WXRICHTEXTIMAGEPAGE_IDNAME ID_WXRICHTEXTIMAGEPAGE
#define SYMBOL_WXRICHTEXTIMAGEPAGE_SIZE wxSize(400, 300)
#define SYMBOL_WXRICHTEXTIMAGEPAGE_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * wxRichTextImagePage class declaration
 */

class wxRichTextImagePage: public wxPanel
{    
    DECLARE_DYNAMIC_CLASS( wxRichTextImagePage )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxRichTextImagePage();
    wxRichTextImagePage( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTIMAGEPAGE_IDNAME, const wxPoint& pos = SYMBOL_WXRICHTEXTIMAGEPAGE_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTIMAGEPAGE_SIZE, long style = SYMBOL_WXRICHTEXTIMAGEPAGE_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTIMAGEPAGE_IDNAME, const wxPoint& pos = SYMBOL_WXRICHTEXTIMAGEPAGE_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTIMAGEPAGE_SIZE, long style = SYMBOL_WXRICHTEXTIMAGEPAGE_STYLE );

    /// Destructor
    ~wxRichTextImagePage();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxRichTextImagePage event handler declarations

////@end wxRichTextImagePage event handler declarations

////@begin wxRichTextImagePage member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxRichTextImagePage member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxRichTextImagePage member variables
    wxComboBox* m_alignment;
    wxComboBox* m_float;
    wxTextCtrl* m_width;
    wxComboBox* m_scaleW;
    wxTextCtrl* m_height;
    wxComboBox* m_scaleH;
    /// Control identifiers
    enum {
        ID_WXRICHTEXTIMAGEPAGE = 10015,
        ID_COMBOBOX_ALIGN = 10016,
        ID_COMBOBOX = 10019,
        ID_TEXTCTRL_WIDTH = 10017,
        ID_COMBOBOX_SCALE_W = 10018,
        ID_TEXTCTRL_HEIGHT = 10000,
        ID_COMBOBOX_SCALE_H = 10002
    };
////@end wxRichTextImagePage member variables
};

#endif
    // _RICHTEXTIMAGEPAGE_H_
