/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtextimagedlg.h
// Purpose:     
// Author:      Mingquan Yang
// Modified by: 
// Created:     Wed 02 Jun 2010 11:27:23 CST
// RCS-ID:      
// Copyright:   (c) Mingquan Yang
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _RICHTEXTIMAGEDLG_H_
#define _RICHTEXTIMAGEDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "wx/richtext/richtextimagedlg.h"
#endif

/*!
 * Includes
 */
#include "wx/richtext/richtextbuffer.h"
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
#define SYMBOL_WXRICHTEXTIMAGEDLG_STYLE wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL
#define SYMBOL_WXRICHTEXTIMAGEDLG_TITLE _("wxRichTextImagePage")
#define SYMBOL_WXRICHTEXTIMAGEDLG_IDNAME ID_WXRICHTEXTIMAGEPAGE
#define SYMBOL_WXRICHTEXTIMAGEDLG_SIZE wxSize(400, 300)
#define SYMBOL_WXRICHTEXTIMAGEDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * wxRichTextImageDlg class declaration
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextImageDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( wxRichTextImageDlg )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxRichTextImageDlg();
    wxRichTextImageDlg( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTIMAGEDLG_IDNAME, const wxString& caption = SYMBOL_WXRICHTEXTIMAGEDLG_TITLE, const wxPoint& pos = SYMBOL_WXRICHTEXTIMAGEDLG_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTIMAGEDLG_SIZE, long style = SYMBOL_WXRICHTEXTIMAGEDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTIMAGEDLG_IDNAME, const wxString& caption = SYMBOL_WXRICHTEXTIMAGEDLG_TITLE, const wxPoint& pos = SYMBOL_WXRICHTEXTIMAGEDLG_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTIMAGEDLG_SIZE, long style = SYMBOL_WXRICHTEXTIMAGEDLG_STYLE );

    /// Destructor
    ~wxRichTextImageDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxRichTextImageDlg event handler declarations

////@end wxRichTextImageDlg event handler declarations

////@begin wxRichTextImageDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxRichTextImageDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    /// Set the image attribute
    void SetImageAttr(const wxRichTextImageAttr& attr);
    void ApplyImageAttr(wxRichTextImage* image);

    virtual bool TransferDataFromWindow();
    virtual bool TransferDataToWindow();
private:
    /// Convert CM to MM
    bool ConvertFromString(const wxString& string, int& ret, int scale);
private:
    wxRichTextImageAttr m_attr;
////@begin wxRichTextImageDlg member variables
    wxComboBox* m_alignment;
    wxComboBox* m_float;
    wxTextCtrl* m_width;
    wxComboBox* m_scaleW;
    wxTextCtrl* m_height;
    wxComboBox* m_scaleH;
    wxButton* m_saveButton;
    wxButton* m_cancelButton;
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
////@end wxRichTextImageDlg member variables
};

#endif
    // _RICHTEXTIMAGEDLG_H_
