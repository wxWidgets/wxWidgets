/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtextsizepage.h
// Purpose:
// Author:      Julian Smart
// Modified by:
// Created:     20/10/2010 10:23:24
// RCS-ID:
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _RICHTEXTSIZEPAGE_H_
#define _RICHTEXTSIZEPAGE_H_

/*!
 * Includes
 */

#include "wx/richtext/richtextdialogpage.h"
#include "wx/sizer.h"

////@begin includes
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */


/*!
 * Control identifiers
 */

////@begin control identifiers
#define SYMBOL_WXRICHTEXTSIZEPAGE_STYLE wxTAB_TRAVERSAL
#define SYMBOL_WXRICHTEXTSIZEPAGE_TITLE wxEmptyString
#define SYMBOL_WXRICHTEXTSIZEPAGE_IDNAME ID_WXRICHTEXTSIZEPAGE
#define SYMBOL_WXRICHTEXTSIZEPAGE_SIZE wxSize(400, 300)
#define SYMBOL_WXRICHTEXTSIZEPAGE_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * wxRichTextSizePage class declaration
 */

class WXDLLIMPEXP_RICHTEXT wxRichTextSizePage: public wxRichTextDialogPage
{
    DECLARE_DYNAMIC_CLASS( wxRichTextSizePage )
    DECLARE_EVENT_TABLE()
    DECLARE_HELP_PROVISION()

public:
    /// Constructors
    wxRichTextSizePage();
    wxRichTextSizePage( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTSIZEPAGE_IDNAME, const wxPoint& pos = SYMBOL_WXRICHTEXTSIZEPAGE_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTSIZEPAGE_SIZE, long style = SYMBOL_WXRICHTEXTSIZEPAGE_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXRICHTEXTSIZEPAGE_IDNAME, const wxPoint& pos = SYMBOL_WXRICHTEXTSIZEPAGE_POSITION, const wxSize& size = SYMBOL_WXRICHTEXTSIZEPAGE_SIZE, long style = SYMBOL_WXRICHTEXTSIZEPAGE_STYLE );

    /// Destructor
    ~wxRichTextSizePage();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    /// Gets the attributes from the formatting dialog
    wxRichTextAttr* GetAttributes();

    /// Data transfer
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();

    /// Show/hide position controls
    void ShowPositionControls(bool show);

    /// Show/hide floating controls
    void ShowFloatingControls(bool show);

////@begin wxRichTextSizePage event handler declarations

    /// wxEVT_UPDATE_UI event handler for ID_RICHTEXT_WIDTH
    void OnRichtextWidthUpdate( wxUpdateUIEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_RICHTEXT_HEIGHT
    void OnRichtextHeightUpdate( wxUpdateUIEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX
    void OnRichtextVerticalAlignmentComboboxUpdate( wxUpdateUIEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_RICHTEXT_OFFSET
    void OnRichtextOffsetUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RICHTEXT_PARA_UP
    void OnRichtextParaUpClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_RICHTEXT_PARA_DOWN
    void OnRichtextParaDownClick( wxCommandEvent& event );

////@end wxRichTextSizePage event handler declarations

////@begin wxRichTextSizePage member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxRichTextSizePage member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxRichTextSizePage member variables
    wxBoxSizer* m_parentSizer;
    wxBoxSizer* m_floatingControls;
    wxComboBox* m_float;
    wxCheckBox* m_widthCheckbox;
    wxTextCtrl* m_width;
    wxComboBox* m_unitsW;
    wxCheckBox* m_heightCheckbox;
    wxTextCtrl* m_height;
    wxComboBox* m_unitsH;
    wxBoxSizer* m_alignmentControls;
    wxCheckBox* m_verticalAlignmentCheckbox;
    wxComboBox* m_verticalAlignmentComboBox;
    wxBoxSizer* m_positionControls;
    wxBoxSizer* m_moveObjectParentSizer;
    wxCheckBox* m_offsetYCheckbox;
    wxTextCtrl* m_offset;
    wxComboBox* m_unitsOffset;
    wxBoxSizer* m_moveObjectSizer;
    /// Control identifiers
    enum {
        ID_WXRICHTEXTSIZEPAGE = 10700,
        ID_RICHTEXT_FLOATING_MODE = 10701,
        ID_RICHTEXT_WIDTH_CHECKBOX = 10702,
        ID_RICHTEXT_WIDTH = 10703,
        ID_RICHTEXT_UNITS_W = 10704,
        ID_RICHTEXT_HEIGHT_CHECKBOX = 10705,
        ID_RICHTEXT_HEIGHT = 10706,
        ID_RICHTEXT_UNITS_H = 10707,
        ID_RICHTEXT_VERTICAL_ALIGNMENT_CHECKBOX = 10708,
        ID_RICHTEXT_VERTICAL_ALIGNMENT_COMBOBOX = 10709,
        ID_RICHTEXT_OFFSET_CHECKBOX = 10710,
        ID_RICHTEXT_OFFSET = 10711,
        ID_RICHTEXT_OFFSET_UNITS = 10712,
        ID_RICHTEXT_PARA_UP = 10713,
        ID_RICHTEXT_PARA_DOWN = 10714
    };
////@end wxRichTextSizePage member variables
};

#endif
    // _RICHTEXTSIZEPAGE_H_
