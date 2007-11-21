/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/fontpicker.h
// Purpose:     wxFontButton header
// Author:      Francesco Montorsi
// Modified by:
// Created:     14/4/2006
// Copyright:   (c) Francesco Montorsi
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_FONTPICKER_H_
#define _WX_GTK_FONTPICKER_H_

// since GtkFontButton is available only for GTK+ >= 2.4,
// we need to use generic version if we detect (at runtime)
// that GTK+ < 2.4
#include "wx/generic/fontpickerg.h"

//-----------------------------------------------------------------------------
// wxFontButton
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFontButton : public wxGenericFontButton
{
public:
    wxFontButton() {}
    wxFontButton(wxWindow *parent,
                 wxWindowID id,
                 const wxFont& initial = wxNullFont,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxFONTBTN_DEFAULT_STYLE,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxFontPickerWidgetNameStr)
    {
       Create(parent, id, initial, pos, size, style, validator, name);
    }

    virtual ~wxFontButton();


public:     // overrides

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxFont& initial = wxNullFont,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxFONTBTN_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxFontPickerWidgetNameStr);

protected:
    void UpdateFont();


public:     // used by the GTK callback only

    void SetNativeFontInfo(const gchar *gtkdescription)
        { m_selectedFont.SetNativeFontInfo(wxString::FromAscii(gtkdescription)); }

private:
    DECLARE_DYNAMIC_CLASS(wxFontButton)
};

#endif // _WX_GTK_FONTPICKER_H_

