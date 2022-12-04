/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/stattextcmn.cpp
// Purpose:     common (to all ports) wxStaticText functions
// Author:      Vadim Zeitlin, Francesco Montorsi
// Created:     2007-01-07 (extracted from dlgcmn.cpp)
// Copyright:   (c) 1999-2006 Vadim Zeitlin
//              (c) 2007 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_STATTEXT

#ifndef WX_PRECOMP
    #include "wx/stattext.h"
    #include "wx/button.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/settings.h"
    #include "wx/sizer.h"
    #include "wx/containr.h"
#endif

#include "wx/textwrapper.h"

#include "wx/private/markupparser.h"

#include <algorithm>

extern WXDLLEXPORT_DATA(const char) wxStaticTextNameStr[] = "staticText";

// ----------------------------------------------------------------------------
// XTI
// ----------------------------------------------------------------------------

wxDEFINE_FLAGS( wxStaticTextStyle )
wxBEGIN_FLAGS( wxStaticTextStyle )
// new style border flags, we put them first to
// use them for streaming out
wxFLAGS_MEMBER(wxBORDER_SIMPLE)
wxFLAGS_MEMBER(wxBORDER_SUNKEN)
wxFLAGS_MEMBER(wxBORDER_DOUBLE)
wxFLAGS_MEMBER(wxBORDER_RAISED)
wxFLAGS_MEMBER(wxBORDER_STATIC)
wxFLAGS_MEMBER(wxBORDER_NONE)

// old style border flags
wxFLAGS_MEMBER(wxSIMPLE_BORDER)
wxFLAGS_MEMBER(wxSUNKEN_BORDER)
wxFLAGS_MEMBER(wxDOUBLE_BORDER)
wxFLAGS_MEMBER(wxRAISED_BORDER)
wxFLAGS_MEMBER(wxSTATIC_BORDER)
wxFLAGS_MEMBER(wxBORDER)

// standard window styles
wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
wxFLAGS_MEMBER(wxCLIP_CHILDREN)
wxFLAGS_MEMBER(wxWANTS_CHARS)
wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
wxFLAGS_MEMBER(wxVSCROLL)
wxFLAGS_MEMBER(wxHSCROLL)

wxFLAGS_MEMBER(wxST_NO_AUTORESIZE)
wxFLAGS_MEMBER(wxALIGN_LEFT)
wxFLAGS_MEMBER(wxALIGN_RIGHT)
wxFLAGS_MEMBER(wxALIGN_CENTRE)
wxEND_FLAGS( wxStaticTextStyle )

wxIMPLEMENT_DYNAMIC_CLASS_XTI(wxStaticText, wxControl, "wx/stattext.h");

wxBEGIN_PROPERTIES_TABLE(wxStaticText)
wxPROPERTY( Label,wxString, SetLabel, GetLabel, wxString(), 0 /*flags*/, \
           wxT("Helpstring"), wxT("group"))
wxPROPERTY_FLAGS( WindowStyle, wxStaticTextStyle, long, SetWindowStyleFlag, \
                 GetWindowStyleFlag, wxEMPTY_PARAMETER_VALUE, 0 /*flags*/, \
                 wxT("Helpstring"), wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxEMPTY_HANDLERS_TABLE(wxStaticText)

wxCONSTRUCTOR_6( wxStaticText, wxWindow*, Parent, wxWindowID, Id, \
                wxString, Label, wxPoint, Position, wxSize, Size, long, WindowStyle )


// ----------------------------------------------------------------------------
// wxTextWrapper
// ----------------------------------------------------------------------------

void wxTextWrapper::Wrap(wxWindow *win, const wxString& text, int widthMax)
{
    const wxClientDC dc(win);

    const wxArrayString ls = wxSplit(text, '\n', '\0');
    for ( wxArrayString::const_iterator i = ls.begin(); i != ls.end(); ++i )
    {
        wxString line = *i;

        if ( i != ls.begin() )
        {
            // Do this even if the line is empty, except if it's the first one.
            OnNewLine();
        }

        // Is this a special case when wrapping is disabled?
        if ( widthMax < 0 )
        {
            DoOutputLine(line);
            continue;
        }

        for ( bool newLine = false; !line.empty(); newLine = true )
        {
            if ( newLine )
                OnNewLine();

            wxArrayInt widths;
            dc.GetPartialTextExtents(line, widths);

            const size_t posEnd = std::lower_bound(widths.begin(),
                                                   widths.end(),
                                                   widthMax) - widths.begin();

            // Does the entire remaining line fit?
            if ( posEnd == line.length() )
            {
                DoOutputLine(line);
                break;
            }

            // Find the last word to chop off.
            const size_t lastSpace = line.rfind(' ', posEnd);
            if ( lastSpace == wxString::npos )
            {
                // No spaces, so can't wrap.
                DoOutputLine(line);
                break;
            }

            // Output the part that fits.
            DoOutputLine(line.substr(0, lastSpace));

            // And redo the layout with the rest.
            line = line.substr(lastSpace + 1);
        }
    }
}


// ----------------------------------------------------------------------------
// wxLabelWrapper: helper class for wxStaticTextBase::Wrap()
// ----------------------------------------------------------------------------

class wxLabelWrapper : public wxTextWrapper
{
public:
    void WrapLabel(wxWindow *text, int widthMax)
    {
        m_text.clear();
        Wrap(text, text->GetLabel(), widthMax);
        text->SetLabel(m_text);
    }

protected:
    virtual void OnOutputLine(const wxString& line) override
    {
        m_text += line;
    }

    virtual void OnNewLine() override
    {
        m_text += wxT('\n');
    }

private:
    wxString m_text;
};


// ----------------------------------------------------------------------------
// wxStaticTextBase
// ----------------------------------------------------------------------------

void wxStaticTextBase::Wrap(int width)
{
    wxLabelWrapper wrapper;
    wrapper.WrapLabel(this, width);
}

void wxStaticTextBase::AutoResizeIfNecessary()
{
    // This flag is specifically used to prevent the control from resizing even
    // when its label changes.
    if ( HasFlag(wxST_NO_AUTORESIZE) )
        return;

    // This method is only called if either the label or the font changed, i.e.
    // if the label extent changed, so the best size is not the same either
    // any more.
    //
    // Note that we don't invalidate it when wxST_NO_AUTORESIZE is on because
    // this would result in the control being effectively resized during the
    // next Layout() and this style is used expressly to prevent this from
    // happening.
    InvalidateBestSize();

    SetSize(GetBestSize());
}

// ----------------------------------------------------------------------------
// wxStaticTextBase - generic implementation for wxST_ELLIPSIZE_* support
// ----------------------------------------------------------------------------

void wxStaticTextBase::UpdateLabel()
{
    if (!IsEllipsized())
        return;

    const wxString& newlabel = GetEllipsizedLabel();

    // we need to touch the "real" label (i.e. the text set inside the control,
    // using port-specific functions) instead of the string returned by GetLabel().
    //
    // In fact, we must be careful not to touch the original label passed to
    // SetLabel() otherwise GetLabel() will behave in a strange way to the user
    // (e.g. returning a "Ver...ing" instead of "Very long string") !
    if (newlabel == WXGetVisibleLabel())
        return;
    WXSetVisibleLabel(newlabel);
}

wxString wxStaticTextBase::GetEllipsizedLabel() const
{
    // this function should be used only by ports which do not support
    // ellipsis in static texts: we first remove markup (which cannot
    // be handled safely by Ellipsize()) and then ellipsize the result.

    wxString ret(m_labelOrig);

    if (IsEllipsized())
        ret = Ellipsize(ret);

    return ret;
}

wxString wxStaticTextBase::Ellipsize(const wxString& label) const
{
    wxSize sz(GetClientSize());
    if (sz.GetWidth() < 2 || sz.GetHeight() < 2)
    {
        // the size of this window is not valid (yet)
        return label;
    }

    wxClientDC dc(const_cast<wxStaticTextBase*>(this));

    wxEllipsizeMode mode;
    if ( HasFlag(wxST_ELLIPSIZE_START) )
        mode = wxELLIPSIZE_START;
    else if ( HasFlag(wxST_ELLIPSIZE_MIDDLE) )
        mode = wxELLIPSIZE_MIDDLE;
    else if ( HasFlag(wxST_ELLIPSIZE_END) )
        mode = wxELLIPSIZE_END;
    else
    {
        wxFAIL_MSG( "should only be called if have one of wxST_ELLIPSIZE_XXX" );

        return label;
    }

    return wxControl::Ellipsize(label, dc, mode, sz.GetWidth());
}

#endif // wxUSE_STATTEXT
