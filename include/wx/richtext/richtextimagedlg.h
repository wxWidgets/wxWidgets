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
#include "wx/statline.h"
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
#define SYMBOL_WXRICHTEXTIMAGEDIALOG_STYLE wxDEFAULT_DIALOG_STYLE|wxTAB_TRAVERSAL
#define SYMBOL_WXRICHTEXTIMAGEDIALOG_TITLE _("Image Properties")
#define SYMBOL_WXRICHTEXTIMAGEDIALOG_IDNAME ID_WXRICHTEXTIMAGEPAGE
#define SYMBOL_WXRICHTEXTIMAGEDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_WXRICHTEXTIMAGEDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * wxRichTextImageDialog class declaration
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextImageDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( wxRichTextImageDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxRichTextImageDialog();
    wxRichTextImageDialog( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTIMAGEDIALOG_IDNAME, const wxString& caption = SYMBOL_WXRICHTEXTIMAGEDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXRICHTEXTIMAGEDIALOG_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTIMAGEDIALOG_SIZE, long style = SYMBOL_WXRICHTEXTIMAGEDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTIMAGEDIALOG_IDNAME, const wxString& caption = SYMBOL_WXRICHTEXTIMAGEDIALOG_TITLE, const wxPoint& pos = SYMBOL_WXRICHTEXTIMAGEDIALOG_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTIMAGEDIALOG_SIZE, long style = SYMBOL_WXRICHTEXTIMAGEDIALOG_STYLE );

    /// Destructor
    ~wxRichTextImageDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin wxRichTextImageDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_UP
    void OnButtonParaUpClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_DOWN
    void OnButtonParaDownClick( wxCommandEvent& event );

////@end wxRichTextImageDialog event handler declarations

////@begin wxRichTextImageDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxRichTextImageDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

    /// Set the image attribute
    void SetImageAttr(const wxRichTextAnchoredObjectAttr& attr);
    wxRichTextImage* ApplyImageAttr();

    /// Set the anchored object
    void SetImageObject(wxRichTextImage *image, wxRichTextBuffer* buffer);

    virtual bool TransferDataFromWindow();
    virtual bool TransferDataToWindow();
private:
    /// Convert CM to MM
    bool ConvertFromString(const wxString& string, int& ret, int scale);
private:
    wxRichTextAnchoredObjectAttr m_attr;
////@begin wxRichTextImageDialog member variables
    wxComboBox* m_alignment;
    wxComboBox* m_float;
    wxTextCtrl* m_width;
    wxComboBox* m_unitsW;
    wxTextCtrl* m_height;
    wxComboBox* m_unitsH;
    wxTextCtrl* m_offset;
    wxComboBox* m_unitsOffset;
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
        ID_COMBOBOX_SCALE_H = 10002,
        ID_BUTTON_PARA_UP = 10020,
        ID_BUTTON_PARA_DOWN = 10021,
        ID_TEXTCTRL_OFFSET = 10022,
        ID_COMBOBOX_OFFSET = 10001
    };
////@end wxRichTextImageDialog member variables

    wxRichTextBuffer* m_buffer;
    wxRichTextObject* m_image;
    wxRichTextObject* m_parent;
};

#endif
    // _RICHTEXTIMAGEDLG_H_
