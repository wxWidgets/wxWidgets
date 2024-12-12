///////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/private/markuptext.h
// Purpose:     Generic wx*MarkupText classes for managing text with markup.
// Author:      Vadim Zeitlin
// Created:     2011-02-21
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_PRIVATE_MARKUPTEXT_H_
#define _WX_GENERIC_PRIVATE_MARKUPTEXT_H_

#include "wx/defs.h"
#include "wx/gdicmn.h"

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxReadOnlyDC;

class wxMarkupParserOutput;

// ----------------------------------------------------------------------------
// wxMarkupText: allows to measure and draw the text containing markup.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMarkupTextBase
{
public:
    virtual ~wxMarkupTextBase() = default;

    // Update the markup string, return false if it didn't change.
    bool SetMarkup(const wxString& markup)
    {
        if ( markup == m_markup )
            return false;

        m_markup = markup;

        return true;
    }

    // Return the width and height required by the given string and optionally
    // the height of the visible part above the baseline (i.e. ascent minus
    // internal leading).
    //
    // The font currently selected into the DC is used for measuring (notice
    // that it is changed by this function but normally -- i.e. if markup is
    // valid -- restored to its original value when it returns).
    wxSize Measure(wxReadOnlyDC& dc, int *visibleHeight = nullptr) const;

protected:
    wxMarkupTextBase(const wxString& markup)
        : m_markup(markup)
    {
    }

    // Return m_markup suitable for measuring by Measure, i.e. stripped of
    // any mnenomics.
    virtual wxString GetMarkupForMeasuring() const = 0;

    wxString m_markup;
};


class WXDLLIMPEXP_CORE wxMarkupText : public wxMarkupTextBase
{
public:
    // Constants for Render() flags.
    enum
    {
        Render_Default = 0,     // Don't show mnemonics visually.
        Render_ShowAccels = 1   // Underline mnemonics.
    };


    // Initialize with the given string containing markup (which is supposed to
    // be valid, the caller must check for it before constructing this object).
    //
    // Notice that the usual rules for mnemonics apply to the markup text: if
    // it contains any '&' characters they must be escaped by doubling them,
    // otherwise they indicate that the next character is the mnemonic for this
    // field.
    //
    // TODO-MULTILINE-MARKUP: Currently only single line labels are supported,
    // search for other occurrences of this comment to find the places which
    // need to be updated to support multiline labels with markup.
    wxMarkupText(const wxString& markup) : wxMarkupTextBase(markup)
    {
    }

    // Default copy ctor, assignment operator and dtor are ok.

    // Render the markup string into the given DC in the specified rectangle.
    //
    // Notice that while the function uses the provided rectangle for alignment
    // (it centers the text in it), no clipping is done by it so use Measure()
    // and set the clipping region before rendering if necessary.
    void Render(wxDC& dc, const wxRect& rect, int flags);

protected:
    virtual wxString GetMarkupForMeasuring() const override;
};


// ----------------------------------------------------------------------------
// wxItemMarkupText: variant of wxMarkupText for items without mnemonics
// ----------------------------------------------------------------------------

// This class has similar interface to wxItemMarkup, but no strings contain
// mnemonics and no escaping is done.
class WXDLLIMPEXP_CORE wxItemMarkupText : public wxMarkupTextBase
{
public:
    // Initialize with the given string containing markup (which is supposed to
    // be valid, the caller must check for it before constructing this object).
    // Notice that mnemonics are not interpreted at all by this class, so
    // literal ampersands shouldn't be escaped/doubled.
    wxItemMarkupText(const wxString& markup) : wxMarkupTextBase(markup)
    {
    }

    // Default copy ctor, assignment operator and dtor are ok.

    // Similar to wxMarkupText::Render(), but uses wxRendererNative::DrawItemText()
    // instead of generic wxDC::DrawLabel(), so is more suitable for use in
    // controls that already use DrawItemText() for its items.
    //
    // The meaning of the flags here is different than in wxMarkupText too:
    // they're passed to DrawItemText().
    //
    // Currently the only supported ellipsize modes are wxELLIPSIZE_NONE and
    // wxELLIPSIZE_END, the others are treated as wxELLIPSIZE_END.
    void Render(wxWindow *win,
                wxDC& dc,
                const wxRect& rect,
                int rendererFlags,
                wxEllipsizeMode ellipsizeMode);

protected:
    virtual wxString GetMarkupForMeasuring() const override { return m_markup; }
};

#endif // _WX_GENERIC_PRIVATE_MARKUPTEXT_H_
