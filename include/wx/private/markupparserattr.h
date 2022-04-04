///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/markupparserattr.h
// Purpose:     Classes mapping markup attributes to wxFont/wxColour.
// Author:      Vadim Zeitlin
// Created:     2011-02-18
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_MARKUPPARSERATTR_H_
#define _WX_PRIVATE_MARKUPPARSERATTR_H_

#include "wx/private/markupparser.h"

#include "wx/stack.h"

#include "wx/colour.h"
#include "wx/font.h"

// ----------------------------------------------------------------------------
// wxMarkupParserAttrOutput: simplified wxFont-using version of the above.
// ----------------------------------------------------------------------------

// This class assumes that wxFont and wxColour are used to perform all the
// markup tags and implements the base class virtual functions in terms of
// OnAttr{Start,End}() only.
//
// Notice that you still must implement OnText() inherited from the base class
// when deriving from this one.
class wxMarkupParserAttrOutput : public wxMarkupParserOutput
{
public:
    // A container of font and colours with inheritance support. It holds two
    // sets of attributes:
    // 1. The currently specified ones from parsed tags that contain
    //    information on what should change in the output; some of them
    //    may be invalid if only the others are affected by a change.
    // 2. The _effective_ attributes that are always valid and accumulate
    //    all past changes as the markup is being parser; these are used
    //    to restore state when unwinding nested attributes.
    struct Attr
    {
        Attr(const Attr *attrInEffect,
             const wxFont& font_,
             const wxColour& foreground_ = wxColour(),
             const wxColour& background_ = wxColour())
            : font(font_), foreground(foreground_), background(background_)
        {
            if (attrInEffect)
            {
                effectiveFont = font.IsOk() ? font : attrInEffect->effectiveFont;
                effectiveForeground = foreground_.IsOk() ? foreground_ : attrInEffect->effectiveForeground;
                effectiveBackground = background.IsOk() ? background : attrInEffect->effectiveBackground;
            }
            else
            {
                effectiveFont = font;
                effectiveForeground = foreground;
                effectiveBackground = background;
            }
        }

        wxFont font;
        wxColour foreground,
                 background;
        wxFont   effectiveFont;
        wxColour effectiveForeground,
                 effectiveBackground;
    };


    // This object must be initialized with the font and colours to use
    // initially, i.e. the ones used before any tags in the string.
    wxMarkupParserAttrOutput(const wxFont& font,
                             const wxColour& foreground,
                             const wxColour& background)
    {
        m_attrs.push(Attr(NULL, font, foreground, background));
    }

    // Indicates the change of the font and/or colours used. Any of the
    // fields of the argument may be invalid indicating that the corresponding
    // attribute didn't actually change.
    virtual void OnAttrStart(const Attr& attr) = 0;

    // Indicates the end of the region affected by the given attributes
    // (the same ones that were passed to the matching OnAttrStart(), use
    // GetAttr() to get the ones that will be used from now on).
    virtual void OnAttrEnd(const Attr& attr) = 0;


    // Implement all pure virtual methods inherited from the base class in
    // terms of our own ones.
    virtual void OnBoldStart() wxOVERRIDE { DoChangeFont(&wxFont::Bold); }
    virtual void OnBoldEnd() wxOVERRIDE { DoEndAttr(); }

    virtual void OnItalicStart() wxOVERRIDE { DoChangeFont(&wxFont::Italic); }
    virtual void OnItalicEnd() wxOVERRIDE { DoEndAttr(); }

    virtual void OnUnderlinedStart() wxOVERRIDE { DoChangeFont(&wxFont::Underlined); }
    virtual void OnUnderlinedEnd() wxOVERRIDE { DoEndAttr(); }

    virtual void OnStrikethroughStart() wxOVERRIDE { DoChangeFont(&wxFont::Strikethrough); }
    virtual void OnStrikethroughEnd() wxOVERRIDE { DoEndAttr(); }

    virtual void OnBigStart() wxOVERRIDE { DoChangeFont(&wxFont::Larger); }
    virtual void OnBigEnd() wxOVERRIDE { DoEndAttr(); }

    virtual void OnSmallStart() wxOVERRIDE { DoChangeFont(&wxFont::Smaller); }
    virtual void OnSmallEnd() wxOVERRIDE { DoEndAttr(); }

    virtual void OnTeletypeStart() wxOVERRIDE
    {
        wxFont font(GetFont());
        font.SetFamily(wxFONTFAMILY_TELETYPE);
        DoSetFont(font);
    }
    virtual void OnTeletypeEnd() wxOVERRIDE { DoEndAttr(); }

    virtual void OnSpanStart(const wxMarkupSpanAttributes& spanAttr) wxOVERRIDE
    {
        wxFont font(GetFont());
        if ( !spanAttr.m_fontFace.empty() )
            font.SetFaceName(spanAttr.m_fontFace);

        FontModifier<wxFontWeight>()(spanAttr.m_isBold,
                                     font, &wxFont::SetWeight,
                                     wxFONTWEIGHT_NORMAL, wxFONTWEIGHT_BOLD);

        FontModifier<wxFontStyle>()(spanAttr.m_isItalic,
                                    font, &wxFont::SetStyle,
                                    wxFONTSTYLE_NORMAL, wxFONTSTYLE_ITALIC);

        FontModifier<bool>()(spanAttr.m_isUnderlined,
                             font, &wxFont::SetUnderlined,
                             false, true);

        FontModifier<bool>()(spanAttr.m_isStrikethrough,
                             font, &wxFont::SetStrikethrough,
                             false, true);

        switch ( spanAttr.m_sizeKind )
        {
            case wxMarkupSpanAttributes::Size_Unspecified:
                break;

            case wxMarkupSpanAttributes::Size_Relative:
                if ( spanAttr.m_fontSize > 0 )
                    font.MakeLarger();
                else
                    font.MakeSmaller();
                break;

            case wxMarkupSpanAttributes::Size_Symbolic:
                // The values of font size intentionally coincide with the
                // values of wxFontSymbolicSize enum elements so simply cast
                // one to the other.
                font.SetSymbolicSize(
                    static_cast<wxFontSymbolicSize>(spanAttr.m_fontSize)
                );
                break;

            case wxMarkupSpanAttributes::Size_PointParts:
                font.SetFractionalPointSize(spanAttr.m_fontSize/1024.);
                break;
        }


        const Attr attr(&m_attrs.top(), font, spanAttr.m_fgCol, spanAttr.m_bgCol);
        OnAttrStart(attr);

        m_attrs.push(attr);
    }

    virtual void OnSpanEnd(const wxMarkupSpanAttributes& WXUNUSED(spanAttr)) wxOVERRIDE
    {
        DoEndAttr();
    }

protected:
    // Get the current attributes, i.e. the ones that should be used for
    // rendering (or measuring or whatever) the text at the current position in
    // the string.
    //
    // It may be called from OnAttrStart() to get the old attributes used
    // before and from OnAttrEnd() to get the new attributes that will be used
    // from now on but is mostly meant to be used from overridden OnText()
    // implementations.
    const Attr& GetAttr() const { return m_attrs.top(); }

    // A shortcut for accessing the font of the current attribute.
    const wxFont& GetFont() const { return GetAttr().font; }

private:
    // Change only the font to the given one. Call OnAttrStart() to notify
    // about the change and update the attributes stack.
    void DoSetFont(const wxFont& font)
    {
        const Attr attr(&m_attrs.top(), font);

        OnAttrStart(attr);

        m_attrs.push(attr);
    }

    // Apply the given function to the font currently on top of the font stack,
    // push the new font on the stack and call OnAttrStart() with it.
    void DoChangeFont(wxFont (wxFont::*func)() const)
    {
        DoSetFont((GetFont().*func)());
    }

    void DoEndAttr()
    {
        const Attr attr(m_attrs.top());
        m_attrs.pop();

        OnAttrEnd(attr);
    }

    // A helper class used to apply the given function to a wxFont object
    // depending on the value of an OptionalBool.
    template <typename T>
    struct FontModifier
    {
        FontModifier() { }

        void operator()(wxMarkupSpanAttributes::OptionalBool isIt,
                        wxFont& font,
                        void (wxFont::*func)(T),
                        T noValue,
                        T yesValue)
        {
            switch ( isIt )
            {
                case wxMarkupSpanAttributes::Unspecified:
                    break;

                case wxMarkupSpanAttributes::No:
                    (font.*func)(noValue);
                    break;

                case wxMarkupSpanAttributes::Yes:
                    (font.*func)(yesValue);
                    break;
            }
        }
    };


    wxStack<Attr> m_attrs;

    wxDECLARE_NO_COPY_CLASS(wxMarkupParserAttrOutput);
};

#endif // _WX_PRIVATE_MARKUPPARSERATTR_H_
