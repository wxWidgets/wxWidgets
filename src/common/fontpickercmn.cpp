///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/fontpickercmn.cpp
// Purpose:     wxFontPickerCtrl class implementation
// Author:      Francesco Montorsi
// Created:     15/04/2006
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_FONTPICKERCTRL

#include "wx/fontpicker.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
#endif

#include "wx/fontenum.h"
#include "wx/tokenzr.h"
#include "wx/weakref.h"

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXGTK__) && !defined(__WXUNIVERSAL__)
    #define SetMinMaxPointSize(min, max)
#else
    #define SetMinMaxPointSize(min, max)  GetPickerWidget()->GetFontData()->SetRange((min), (max))
#endif

const char wxFontPickerCtrlNameStr[] = "fontpicker";
const char wxFontPickerWidgetNameStr[] = "fontpickerwidget";

wxDEFINE_EVENT(wxEVT_FONTPICKER_CHANGED, wxFontPickerEvent);
wxIMPLEMENT_DYNAMIC_CLASS(wxFontPickerCtrl, wxPickerBase);
wxIMPLEMENT_DYNAMIC_CLASS(wxFontPickerEvent, wxCommandEvent);

#ifdef __WXWINUI__
namespace
{

void RefitFontPickerGrowOnly(wxFontPickerCtrl *picker)
{
    const wxWeakRef<wxFontPickerCtrl> weakPicker(picker);
    if ( wxControl * const button = picker->GetPickerCtrl() )
        button->InvalidateBestSize();
    picker->InvalidateBestSize();

    const wxSize desired = picker->GetBestSize();
    picker = weakPicker.get();
    if ( !picker )
        return;

    wxSize minimum = picker->GetMinSize();
    if ( minimum.x < 0 )
        minimum.x = 0;
    if ( minimum.y < 0 )
        minimum.y = 0;
    minimum.IncTo(desired);
    picker->SetMinSize(minimum);

    wxSize grown = picker->GetSize();
    grown.IncTo(desired);
    if ( grown != picker->GetSize() )
        picker->SetSize(grown);

    // A realized WinUI button can gain width when its font description or
    // label font changes. Invalidate the complete composite and let its owner
    // consume the monotonic minimum immediately; no timer or private layout
    // loop is introduced here.
    picker = weakPicker.get();
    if ( picker )
    {
        wxWindow * const parent = picker->GetParent();
        if ( parent && parent->GetSizer() )
            parent->Layout();
    }
}

} // anonymous namespace
#endif // __WXWINUI__

// ----------------------------------------------------------------------------
// wxFontPickerCtrl
// ----------------------------------------------------------------------------

bool wxFontPickerCtrl::Create( wxWindow *parent, wxWindowID id,
                        const wxFont &initial,
                        const wxPoint &pos, const wxSize &size,
                        long style, const wxValidator& validator,
                        const wxString &name )
{
    if (!wxPickerBase::CreateBase(parent, id,
                                  Font2String(initial.IsOk() ? initial
                                                             : *wxNORMAL_FONT),
                                  pos, size, style, validator, name))
        return false;

    // the picker of a wxFontPickerCtrl is a wxFontPickerWidget
    m_picker = new wxFontPickerWidget(this, wxID_ANY, initial,
                                      wxDefaultPosition, wxDefaultSize,
                                      GetPickerStyle(style));
    // complete sizer creation
    wxPickerBase::PostCreation();

    m_picker->Bind(wxEVT_FONTPICKER_CHANGED, &wxFontPickerCtrl::OnFontChange, this);

    return true;
}

wxString wxFontPickerCtrl::Font2String(const wxFont &f)
{
    wxString ret = f.GetNativeFontInfoUserDesc();
#if defined(__WXMSW__) || defined(__WXOSX__)
    // on wxMSW and wxOSX the encoding of the font is appended at the end of the string;
    // since encoding is not very user-friendly we remove it.
    wxFontEncoding enc = f.GetEncoding();
    if ( enc != wxFONTENCODING_DEFAULT && enc != wxFONTENCODING_SYSTEM )
        ret = ret.BeforeLast(wxT(' '));
#endif
    return ret;
}

wxFont wxFontPickerCtrl::String2Font(const wxString &s)
{
    wxString str(s);
    wxFont ret;
    double n;

    // put a limit on the maximum point size which the user can enter
    // NOTE: we suppose the last word of given string is the pointsize
    wxString size = str.AfterLast(wxT(' '));
    if (size.ToDouble(&n))
    {
        if (n < 1)
            str = str.Left(str.length() - size.length()) + wxT("1");
        else if (n >= m_nMaxPointSize)
            str = str.Left(str.length() - size.length()) +
                  wxString::Format(wxT("%d"), m_nMaxPointSize);
    }

    if (!ret.SetNativeFontInfoUserDesc(str))
        return wxNullFont;

    return ret;
}

void wxFontPickerCtrl::SetSelectedFont(const wxFont &f)
{
    const wxWeakRef<wxFontPickerCtrl> weakThis(this);
    GetPickerWidget()->SetSelectedFont(f);
    wxFontPickerCtrl *live = weakThis.get();
    if ( !live )
        return;

    live->UpdateTextCtrlFromPicker();
#ifdef __WXWINUI__
    live = weakThis.get();
    if ( !live )
        return;
    RefitFontPickerGrowOnly(live);
#endif
}

void wxFontPickerCtrl::UpdatePickerFromTextCtrl()
{
    wxASSERT(m_text);

    // NB: we don't use the wxFont::wxFont(const wxString &) constructor
    //     since that constructor expects the native font description
    //     string returned by wxFont::GetNativeFontInfoDesc() and not
    //     the user-friendly one returned by wxFont::GetNativeFontInfoUserDesc()
    wxFont f = String2Font(m_text->GetValue());
    if (!f.IsOk())
        return;     // invalid user input

    if (GetPickerWidget()->GetSelectedFont() != f)
    {
        const wxWeakRef<wxFontPickerCtrl> weakThis(this);
        GetPickerWidget()->SetSelectedFont(f);
        wxFontPickerCtrl *live = weakThis.get();
        if ( !live )
            return;

#ifdef __WXWINUI__
        RefitFontPickerGrowOnly(live);
        live = weakThis.get();
        if ( !live )
            return;
#endif

        // fire an event
        wxFontPickerEvent event(live, live->GetId(), f);
        live->GetEventHandler()->ProcessEvent(event);
    }
}

void wxFontPickerCtrl::UpdateTextCtrlFromPicker()
{
    if (!m_text)
        return;     // no textctrl to update

    // Take care to use ChangeValue() here and not SetValue() to avoid
    // infinite recursion.
    m_text->ChangeValue(Font2String(GetPickerWidget()->GetSelectedFont()));
}

void wxFontPickerCtrl::SetMinPointSize(unsigned int min)
{
    m_nMinPointSize = min;
    SetMinMaxPointSize(m_nMinPointSize, m_nMaxPointSize);
}

void wxFontPickerCtrl::SetMaxPointSize(unsigned int max)
{
    m_nMaxPointSize = max;
    SetMinMaxPointSize(m_nMinPointSize, m_nMaxPointSize);
}

// ----------------------------------------------------------------------------
// wxFontPickerCtrl - event handlers
// ----------------------------------------------------------------------------

void wxFontPickerCtrl::OnFontChange(wxFontPickerEvent &ev)
{
    const wxWeakRef<wxFontPickerCtrl> weakThis(this);
    UpdateTextCtrlFromPicker();
    wxFontPickerCtrl *live = weakThis.get();
    if ( !live )
        return;

    // the wxFontPickerWidget sent us a colour-change notification.
    // forward this event to our parent
#ifdef __WXWINUI__
    RefitFontPickerGrowOnly(live);
    live = weakThis.get();
    if ( !live )
        return;
#endif
    wxFontPickerEvent event(live, live->GetId(), ev.GetFont());
    live->GetEventHandler()->ProcessEvent(event);
}

#endif  // wxUSE_FONTPICKERCTRL
