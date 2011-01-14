/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtextimagedlg.cpp
// Purpose:
// Author:      Mingquan Yang
// Modified by: Julian Smart
// Created:     Wed 02 Jun 2010 11:27:23 CST
// RCS-ID:
// Copyright:   (c) Mingquan Yang, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/combobox.h"
    #include "wx/sizer.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif

#include "wx/statline.h"

#include "wx/richtext/richtextimagedlg.h"
#include "wx/richtext/richtextctrl.h"

////@begin XPM images
////@end XPM images


/*!
 * wxRichTextObjectPropertiesDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxRichTextObjectPropertiesDialog, wxRichTextFormattingDialog )


/*!
 * wxRichTextObjectPropertiesDialog event table definition
 */

BEGIN_EVENT_TABLE( wxRichTextObjectPropertiesDialog, wxRichTextFormattingDialog )

////@begin wxRichTextObjectPropertiesDialog event table entries
////@end wxRichTextObjectPropertiesDialog event table entries

END_EVENT_TABLE()


/*!
 * wxRichTextObjectPropertiesDialog constructors
 */

wxRichTextObjectPropertiesDialog::wxRichTextObjectPropertiesDialog()
{
    Init();
}

wxRichTextObjectPropertiesDialog::wxRichTextObjectPropertiesDialog( wxRichTextObject* obj, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(obj, parent, id, caption, pos, size, style);
}


/*!
 * wxRichTextImageDlg creator
 */

bool wxRichTextObjectPropertiesDialog::Create( wxRichTextObject* obj, wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    SetObject(obj);
    SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
    long flags = wxRICHTEXT_FORMAT_SIZE|wxRICHTEXT_FORMAT_MARGINS|wxRICHTEXT_FORMAT_BORDERS|wxRICHTEXT_FORMAT_BACKGROUND;
    wxRichTextFormattingDialog::Create( flags, parent, caption, id, pos, size, style );

    CreateControls();

    return true;
}


/*!
 * wxRichTextObjectPropertiesDialog destructor
 */

wxRichTextObjectPropertiesDialog::~wxRichTextObjectPropertiesDialog()
{
////@begin wxRichTextObjectPropertiesDialog destruction
////@end wxRichTextObjectPropertiesDialog destruction
}


/*!
 * Member initialisation
 */

void wxRichTextObjectPropertiesDialog::Init()
{
////@begin wxRichTextObjectPropertiesDialog member initialisation
////@end wxRichTextObjectPropertiesDialog member initialisation
}


/*!
 * Control creation for wxRichTextImageDlg
 */

void wxRichTextObjectPropertiesDialog::CreateControls()
{
}


/*!
 * Should we show tooltips?
 */

bool wxRichTextObjectPropertiesDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap wxRichTextObjectPropertiesDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxRichTextObjectPropertiesDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxRichTextObjectPropertiesDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon wxRichTextObjectPropertiesDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxRichTextObjectPropertiesDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxRichTextObjectPropertiesDialog icon retrieval
}

#if 0
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_UP
 */
void wxRichTextObjectPropertiesDialog::OnRichtextParaUpClick( wxCommandEvent& WXUNUSED(event))
{
    // Before editing this code, remove the block markers.
    wxRichTextRange range = m_object->GetRange();
    wxRichTextObjectList::compatibility_iterator iter = m_buffer->GetChildren().GetFirst();
    if (!iter)
        return;

    while (iter)
    {
        if (iter->GetData() == m_parent)
            break;
        iter = iter->GetNext();
    }

    iter = iter->GetPrevious();
    if (!iter)
        return;

    wxRichTextObject *obj = iter->GetData();
    wxRichTextRange rg = obj->GetRange();
    m_object = m_object->Clone();

    m_buffer->DeleteRangeWithUndo(range, m_buffer->GetRichTextCtrl());
    m_buffer->InsertObjectWithUndo(rg.GetEnd(), m_object, m_buffer->GetRichTextCtrl(), 0);
    m_parent = obj;
    m_object->SetRange(wxRichTextRange(rg.GetEnd(), rg.GetEnd()));
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PARA_DOWN
 */

void wxRichTextObjectPropertiesDialog::OnRichtextDownClick( wxCommandEvent& WXUNUSED(event))
{
    // Before editing this code, remove the block markers.
    wxRichTextRange range = m_object->GetRange();
    wxRichTextObjectList::compatibility_iterator iter = m_buffer->GetChildren().GetFirst();
    if (!iter)
        return;

    while (iter)
    {
        if (iter->GetData() == m_parent)
            break;
        iter = iter->GetNext();
    }

    iter = iter->GetNext();
    if (!iter)
        return;

    wxRichTextObject *obj = iter->GetData();
    wxRichTextRange rg = obj->GetRange();
    m_object = m_object->Clone();

    m_buffer->DeleteRangeWithUndo(range, m_buffer->GetRichTextCtrl());
    m_buffer->InsertObjectWithUndo(rg.GetEnd(), m_object, m_buffer->GetRichTextCtrl(), 0);
    m_parent = obj;
    m_object->SetRange(wxRichTextRange(rg.GetEnd(), rg.GetEnd()));
}

#endif

#endif
    // wxUSE_RICHTEXT
