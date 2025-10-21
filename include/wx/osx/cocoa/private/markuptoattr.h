///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/cocoa/private/markuptoattr.h
// Purpose:     Class to convert markup to Cocoa attributed strings.
// Author:      Vadim Zeitlin
// Created:     2011-02-22
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OSX_COCOA_PRIVATE_MARKUPTOATTR_H_
#define _WX_OSX_COCOA_PRIVATE_MARKUPTOATTR_H_

#include "wx/private/markupparserattr.h"

// ----------------------------------------------------------------------------
// wxMarkupToAttrString: create NSAttributedString from markup.
// ----------------------------------------------------------------------------

class wxMarkupToAttrStringBase : public wxMarkupParserAttrOutput
{
protected:
    // We don't care about the original colours because we never use them but
    // we do need the correct initial font as we apply modifiers (e.g. create a
    // font larger than it) to it and so it must be valid.
    wxMarkupToAttrStringBase(const wxFont& font)
        : wxMarkupParserAttrOutput(font, wxColour(), wxColour()),
          m_attrString(nullptr)
    {}

    void Parse(const wxFont& WXUNUSED(font), const wxString& markup)
    {
        const wxCFStringRef label(PrepareText(wxMarkupParser::Strip(markup)));
        m_attrString = [[NSMutableAttributedString alloc]
                        initWithString: label.AsNSString()];

        m_pos = 0;

        [m_attrString beginEditing];

        // Now translate the markup tags to corresponding attributes.
        wxMarkupParser parser(*this);
        parser.Parse(markup);

        [m_attrString endEditing];
    }

    ~wxMarkupToAttrStringBase()
    {
        if ( m_attrString )
            [m_attrString release];
    }

    // prepare text chunk for display, e.g. strip mnemonics from it
    virtual wxString PrepareText(const wxString& text) = 0;

public:
    // Accessor for the users of this class.
    //
    // We keep ownership of the returned string.
    NSMutableAttributedString *GetNSAttributedString() const
    {
        return m_attrString;
    }

    // Implement base class pure virtual methods to process markup tags.
    virtual void OnText(const wxString& text) override
    {
        const Attr& attr = GetAttr();

        NSMutableDictionary *dict = [NSMutableDictionary dictionary];

        dict[NSFontAttributeName] = attr.effectiveFont.OSXGetNSFont();
        if ( attr.effectiveFont.GetStrikethrough() )
            dict[NSStrikethroughStyleAttributeName] = @(NSUnderlineStyleSingle);
        if ( attr.effectiveFont.GetUnderlined() )
            dict[NSUnderlineStyleAttributeName] = @(NSUnderlineStyleSingle);

        if ( attr.effectiveForeground.IsOk() )
            dict[NSForegroundColorAttributeName] = attr.effectiveForeground.OSXGetWXColor();

        if ( attr.effectiveBackground.IsOk() )
            dict[NSBackgroundColorAttributeName] = attr.effectiveBackground.OSXGetWXColor();

        const unsigned len = PrepareText(text).length();

        [m_attrString addAttributes:dict range:NSMakeRange(m_pos, len)];

        m_pos += len;
    }

    virtual void OnAttrStart(const Attr& WXUNUSED(attr)) override {}

    virtual void OnAttrEnd(const Attr& WXUNUSED(attr)) override {}

private:
    // The attributed string we're building.
    NSMutableAttributedString *m_attrString;

    // The current position in the output string.
    unsigned m_pos;
};


// for use with labels with mnemonics
class wxMarkupToAttrString : public wxMarkupToAttrStringBase
{
public:
    wxMarkupToAttrString(const wxFont& font, const wxString& markup)
        : wxMarkupToAttrStringBase(font)
    {
        Parse(font, markup);
    }

protected:
    virtual wxString PrepareText(const wxString& text) override
    {
        return wxControl::RemoveMnemonics(text);
    }

    wxDECLARE_NO_COPY_CLASS(wxMarkupToAttrString);
};


// for raw markup with no mnemonics
class wxItemMarkupToAttrString : public wxMarkupToAttrStringBase
{
public:
    wxItemMarkupToAttrString(const wxFont& font, const wxString& markup)
        : wxMarkupToAttrStringBase(font)
    {
        Parse(font, markup);
    }

protected:
    virtual wxString PrepareText(const wxString& text) override
    {
        return text;
    }

    wxDECLARE_NO_COPY_CLASS(wxItemMarkupToAttrString);
};

#endif // _WX_OSX_COCOA_PRIVATE_MARKUPTOATTR_H_
