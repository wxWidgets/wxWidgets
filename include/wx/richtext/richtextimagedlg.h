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

#include "wx/dialog.h"

#ifndef _RICHTEXTIMAGEDLG_H_
#define _RICHTEXTIMAGEDLG_H_

/*!
 * Includes
 */
#include "wx/richtext/richtextbuffer.h"

/*!
 * Forward declarations
 */

class WXDLLIMPEXP_FWD_CORE wxButton;
class WXDLLIMPEXP_FWD_CORE wxComboBox;
class WXDLLIMPEXP_FWD_CORE wxTextCtrl;

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

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RICHTEXTIMAGEDIALOG_PARA_UP
    void OnRichtextimagedialogParaUpClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RICHTEXTIMAGEDIALOG_DOWN
    void OnRichtextimagedialogDownClick( wxCommandEvent& event );

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
        ID_RICHTEXTIMAGEDIALOG_FLOATING_MODE = 10017,
        ID_RICHTEXTIMAGEDIALOG_WIDTH = 10018,
        ID_RICHTEXTIMAGEDIALOG_UNITS_W = 10019,
        ID_RICHTEXTIMAGEDIALOG_HEIGHT = 10020,
        ID_RICHTEXTIMAGEDIALOG_UNITS_H = 10021,
        ID_RICHTEXTIMAGEDIALOG_OFFSET = 10022,
        ID_RICHTEXTIMAGEDIALOG_OFFSET_UNITS = 10023,
        ID_RICHTEXTIMAGEDIALOG_PARA_UP = 10024,
        ID_RICHTEXTIMAGEDIALOG_DOWN = 10025
    };
////@end wxRichTextImageDialog member variables

    wxRichTextBuffer* m_buffer;
    wxRichTextObject* m_image;
    wxRichTextObject* m_parent;
};

#endif
    // _RICHTEXTIMAGEDLG_H_
