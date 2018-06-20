/////////////////////////////////////////////////////////////////////////////
// Name:        wx/stattext.h
// Purpose:     wxStaticText base header
// Author:      Julian Smart
// Modified by:
// Created:
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATTEXT_H_BASE_
#define _WX_STATTEXT_H_BASE_

#include "wx/defs.h"

#if wxUSE_STATTEXT

#include "wx/control.h"
#include "wx/datatransf.h"

/*
 * wxStaticText flags
 */
#define wxST_NO_AUTORESIZE         0x0001
// free 0x0002 bit
#define wxST_ELLIPSIZE_START       0x0004
#define wxST_ELLIPSIZE_MIDDLE      0x0008
#define wxST_ELLIPSIZE_END         0x0010

#define wxST_ELLIPSIZE_MASK \
    (wxST_ELLIPSIZE_START | wxST_ELLIPSIZE_MIDDLE | wxST_ELLIPSIZE_END)

extern WXDLLIMPEXP_DATA_CORE(const char) wxStaticTextNameStr[];

class WXDLLIMPEXP_CORE wxStaticTextBase : public wxControl
{
public:
    wxStaticTextBase() { }

    // wrap the text of the control so that no line is longer than the given
    // width (if possible: this function won't break words)
    // This function will modify the value returned by GetLabel()!
    void Wrap(int width);

    // overridden base virtuals
    virtual bool AcceptsFocus() const wxOVERRIDE { return false; }
    virtual bool HasTransparentBackground() wxOVERRIDE { return true; }

    bool IsEllipsized() const
    {
        return (GetWindowStyle() & wxST_ELLIPSIZE_MASK) != 0;
    }

protected:      // functions required for wxST_ELLIPSIZE_* support

    // choose the default border for this window
    virtual wxBorder GetDefaultBorder() const wxOVERRIDE { return wxBORDER_NONE; }

    // Calls Ellipsize() on the real label if necessary. Unlike GetLabelText(),
    // keeps the mnemonics instead of removing them.
    virtual wxString GetEllipsizedLabel() const;

    // Replaces parts of the string with ellipsis according to the ellipsize
    // style. Shouldn't be called if we don't have any.
    wxString Ellipsize(const wxString& label) const;

    // to be called when updating the size of the static text:
    // updates the label redoing ellipsization calculations
    void UpdateLabel();

    // These functions are platform-specific and must be overridden in ports
    // which do not natively support ellipsization and they must be implemented
    // in a way so that the m_labelOrig member of wxControl is not touched:

    // returns the real label currently displayed inside the control.
    virtual wxString DoGetLabel() const { return wxEmptyString; }

    // sets the real label currently displayed inside the control,
    // _without_ invalidating the size. The text passed is always markup-free
    // but may contain the mnemonic characters.
    virtual void DoSetLabel(const wxString& WXUNUSED(str)) { }

    // Update the current size to match the best size unless wxST_NO_AUTORESIZE
    // style is explicitly used.
    void AutoResizeIfNecessary();

private:
    wxDECLARE_NO_COPY_CLASS(wxStaticTextBase);
};

#if wxUSE_VALIDATORS

template<>
struct wxDataTransfer<wxStaticTextBase>
{
    static bool To(wxStaticTextBase* stxt, wxString* data)
    {
        stxt->SetLabel(*data);
        return true;
    }

    static bool From(wxStaticTextBase* stxt, wxString* data)
    {
        *data = stxt->GetLabel();
        return true;
    }
};

#endif // wxUSE_VALIDATORS

// see wx/generic/stattextg.h for the explanation
#ifndef wxNO_PORT_STATTEXT_INCLUDE

#if defined(__WXUNIVERSAL__)
    #include "wx/univ/stattext.h"
#elif defined(__WXMSW__)
    #include "wx/msw/stattext.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/stattext.h"
#elif defined(__WXGTK20__)
    #include "wx/gtk/stattext.h"
#elif defined(__WXGTK__)
    #include "wx/gtk1/stattext.h"
#elif defined(__WXMAC__)
    #include "wx/osx/stattext.h"
#elif defined(__WXQT__)
    #include "wx/qt/stattext.h"
#endif

#endif // !wxNO_PORT_STATTEXT_INCLUDE

#endif // wxUSE_STATTEXT

#endif // _WX_STATTEXT_H_BASE_
