/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/stattextcmn.cpp
// Purpose:     common (to all ports) wxStaticText functions
// Author:      Vadim Zeitlin, Francesco Montorsi
// Created:     2007-01-07 (extracted from dlgcmn.cpp)
// RCS-ID:      $Id$
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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/private/stattext.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
    #include "wx/dcclient.h"
    #include "wx/intl.h"
    #include "wx/settings.h"
    #include "wx/stattext.h"
    #include "wx/sizer.h"
    #include "wx/containr.h"
#endif

#if wxUSE_STATTEXT

const wxChar *wxMarkupEntities[][wxMARKUP_ENTITY_MAX] =
{
    // the entities handled by SetLabel() when wxST_MARKUP is used and their referenced string

    { wxT("&amp;"), wxT("&lt;"), wxT("&gt;"), wxT("&apos;"), wxT("&quot;") },
    { wxT("&"),     wxT("<"),    wxT(">"),    wxT("'"),      wxT("\"")     }
};


// ----------------------------------------------------------------------------
// wxTextWrapper
// ----------------------------------------------------------------------------

void wxTextWrapper::Wrap(wxWindow *win, const wxString& text, int widthMax)
{
    const wxChar *lastSpace = NULL;
    wxString line;

    const wxChar *lineStart = text.c_str();
    for ( const wxChar *p = lineStart; ; p++ )
    {
        if ( IsStartOfNewLine() )
        {
            OnNewLine();

            lastSpace = NULL;
            line.clear();
            lineStart = p;
        }

        if ( *p == _T('\n') || *p == _T('\0') )
        {
            DoOutputLine(line);

            if ( *p == _T('\0') )
                break;
        }
        else // not EOL
        {
            if ( *p == _T(' ') )
                lastSpace = p;

            line += *p;

            if ( widthMax >= 0 && lastSpace )
            {
                int width;
                win->GetTextExtent(line, &width, NULL);

                if ( width > widthMax )
                {
                    // remove the last word from this line
                    line.erase(lastSpace - lineStart, p + 1 - lineStart);
                    DoOutputLine(line);

                    // go back to the last word of this line which we didn't
                    // output yet
                    p = lastSpace;
                }
            }
            //else: no wrapping at all or impossible to wrap
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
    virtual void OnOutputLine(const wxString& line)
    {
        m_text += line;
    }

    virtual void OnNewLine()
    {
        m_text += _T('\n');
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

wxString wxStaticTextBase::GetLabelText() const
{
    wxString ret(GetLabel());

    if (HasFlag(wxST_MARKUP))
        ret = RemoveMarkup(ret);
    return RemoveMnemonics(ret);
}

/*static*/
wxString wxStaticTextBase::RemoveMarkup(const wxString& text)
{
    // strip out of "text" the markup for platforms which don't support it natively
    bool inside_tag = false;

    wxString label;
    const wxChar *source = text;
    for (size_t i=0, max=text.length(); i<max; i++)
    {
        switch (source[i])
        {
            case wxT('<'):
                if (inside_tag)
                {
                    wxLogDebug(wxT("Invalid markup !"));
                    return wxEmptyString;
                }
                inside_tag = true;
                break;

            case wxT('>'):
                if (!inside_tag)
                {
                    wxLogDebug(wxT("Invalid markup !"));
                    return wxEmptyString;
                }
                inside_tag = false;
                break;

            case wxT('&'):
                {
                    if (i == max-1)
                    {
                        wxLogDebug(wxT("Cannot use & as last character of the string '%s'"),
                                   text.c_str());
                        return wxEmptyString;
                    }

                    // is this ampersand introducing a mnemonic or rather an entity?
                    bool isMnemonic = true;
                    for (size_t j=0; j < wxMARKUP_ENTITY_MAX; j++)
                    {
                        const wxChar *entity = wxMarkupEntities[wxMARKUP_ELEMENT_NAME][j];
                        size_t entityLen = wxStrlen(entity);

                        if (max - i >= entityLen &&
                            wxStrncmp(entity, &source[i], entityLen) == 0)
                        {
                            // replace the &entity; string with the entity reference
                            label << wxMarkupEntities[wxMARKUP_ELEMENT_VALUE][j];
                            
                            // little exception: when the entity reference is "&"
                            // (i.e. when entity is "&amp;"), substitute it with &&
                            // instead of a single ampersand:
                            if (*wxMarkupEntities[wxMARKUP_ELEMENT_VALUE][j] == wxT('&'))
                                label << wxT('&');
                            i += entityLen - 1;     // the -1 is because main for()
                                                    // loop already increments i
                            isMnemonic = false;
                            break;
                        }
                    }

                    if (isMnemonic)
                        label << text[i];
                }
                break;


            default:
                if (!inside_tag)
                    label << text[i];
        }
    }

    return label;
}

/* static */
wxString wxStaticTextBase::EscapeMarkup(const wxString& text)
{
    wxString ret;

    for (const wxChar *source = text; *source != wxT('\0'); source++)
    {
        bool isEntity = false;

        // search in the list of the entities and eventually escape this character
        for (size_t j=0; j < wxMARKUP_ENTITY_MAX; j++)
        {
            if (*source == *wxMarkupEntities[wxMARKUP_ELEMENT_VALUE][j])
            {
                ret << wxMarkupEntities[wxMARKUP_ELEMENT_NAME][j];
                isEntity = true;
                break;
            }
        }

        if (!isEntity)
            ret << *source;     // this character does not need to be escaped
    }

    return ret;
}



// ----------------------------------------------------------------------------
// wxStaticTextBase - generic implementation for wxST_ELLIPSIZE_* support
// ----------------------------------------------------------------------------

void wxStaticTextBase::UpdateLabel()
{
    if (!IsEllipsized())
        return;

    wxString newlabel = GetEllipsizedLabelWithoutMarkup();

    // we need to touch the "real" label (i.e. the text set inside the control,
    // using port-specific functions) instead of the string returned by GetLabel().
    //
    // In fact, we must be careful not to touch the original label passed to 
    // SetLabel() otherwise GetLabel() will behave in a strange way to the user
    // (e.g. returning a "Ver...ing" instead of "Very long string") !
    if (newlabel == DoGetLabel())
        return;
    DoSetLabel(newlabel);
}

wxString wxStaticTextBase::GetEllipsizedLabelWithoutMarkup() const
{
    // this function should be used only by ports which do not support
    // ellipsis in static texts: we first remove markup (which cannot
    // be handled safely by Ellipsize()) and then ellipsize the result.

    wxString ret(m_labelOrig);

    // the order of the following two blocks is important!

    if (HasFlag(wxST_MARKUP))
        ret = RemoveMarkup(ret);

    if (IsEllipsized())
        ret = Ellipsize(ret);

    return ret;
}

#define wxELLIPSE_REPLACEMENT       wxT("...")

wxString wxStaticTextBase::Ellipsize(const wxString& label) const
{
    wxSize sz(GetSize());
    if (sz.GetWidth() < 2 || sz.GetHeight() < 2)
    {
        // the size of this window is not valid (yet)
        return label;
    }

    wxClientDC dc(wx_const_cast(wxStaticTextBase*, this));
    dc.SetFont(GetFont());

    wxArrayInt charOffsets;
    wxString ret;

    // these cannot be cached as they can change because of e.g. a font change
    int replacementWidth = dc.GetTextExtent(wxELLIPSE_REPLACEMENT).GetWidth();
    int marginWidth = dc.GetCharWidth()*2;

    // handle correctly labels with newlines
    wxString curLine;
    wxSize reqsize;
    size_t len;
    for ( const wxChar *pc = label; ; pc++ )
    {
        switch ( *pc )
        {
        case _T('\n'):
        case _T('\0'):
            len = curLine.length();
            if (len > 0 &&
                dc.GetPartialTextExtents(curLine, charOffsets))
            {
                wxASSERT(charOffsets.GetCount() == len);

                size_t totalWidth = charOffsets.Last();
                if ( totalWidth > (size_t)sz.GetWidth() )
                {
                    // we need to ellipsize this row
                    int excessPixels = totalWidth - sz.GetWidth() + 
                                       replacementWidth +
                                       marginWidth;     // security margin (NEEDED!)

                    // remove characters in excess
                    size_t initialChar,     // index of first char to erase
                           nChars;          // how many chars do we need to erase?
                    if (HasFlag(wxST_ELLIPSIZE_START))
                    {
                        initialChar = 0;
                        for (nChars=0;
                             nChars < len && charOffsets[nChars] < excessPixels;
                             nChars++)
                            ;
                    }
                    else if (HasFlag(wxST_ELLIPSIZE_MIDDLE))
                    {
                        // the start & end of the removed span of chars
                        initialChar = len/2;
                        size_t endChar = len/2;

                        int removed = 0;
                        for ( ; removed < excessPixels; )
                        {
                            if (initialChar > 0)
                            {
                                // width of the initialChar-th character
                                int width = charOffsets[initialChar] -
                                            charOffsets[initialChar-1];

                                // remove the initialChar-th character
                                removed += width;
                                initialChar--;
                            }

                            if (endChar < len - 1 &&
                                removed < excessPixels)
                            {
                                // width of the (endChar+1)-th character
                                int width = charOffsets[endChar+1] -
                                            charOffsets[endChar];

                                // remove the endChar-th character
                                removed += width;
                                endChar++;
                            }

                            if (initialChar == 0 && endChar == len-1)
                            {
                                nChars = len+1;
                                break;
                            }
                        }

                        initialChar++;
                        nChars = endChar - initialChar + 1;
                    }
                    else
                    {
                        wxASSERT(HasFlag(wxST_ELLIPSIZE_END));
                        wxASSERT(len > 0);

                        int maxWidth = totalWidth - excessPixels;
                        for (initialChar=0; 
                             initialChar < len && 
                             charOffsets[initialChar] < maxWidth; 
                             initialChar++)
                            ;

                        if (initialChar == 0)
                        {
                            nChars = len;
                        }
                        else
                        {
                            initialChar--;      // go back one character 
                            nChars = len - initialChar;
                        }
                    }

                    if (nChars > len)
                    {
                        // need to remove the entire row!
                        curLine.clear();
                    }
                    else
                    {
                        // erase nChars characters after initialChar (included):
                        curLine.erase(initialChar, nChars+1);

                        // if there is space for the replacement dots, add them
                        if (sz.GetWidth() > replacementWidth)
                            curLine.insert(initialChar, wxELLIPSE_REPLACEMENT);
                    }

                    // if everything was ok, we should have shortened this line
                    // enough to make it fit in sz.GetWidth():
                    wxASSERT(dc.GetTextExtent(curLine).GetWidth() < sz.GetWidth());
                }
            }

            // add this (ellipsized) row to the rest of the label
            ret << curLine << *pc;
            curLine.clear();

            if ( *pc == _T('\0') )
                return ret;

            break;

            // we need to remove mnemonics from the label for
            // correct calculations
        case _T('&'):
            // pc+1 is safe: at worst we'll hit the \0
            if (*(pc+1) == _T('&'))
                curLine += _T('&');          // && becomes &
            //else: remove this ampersand

            break;

            // we need also to expand tabs to properly calc their size
        case _T('\t'):
            // Windows natively expands the TABs to 6 spaces. Do the same:
            curLine += wxT("      ");
            break;

        default:
            curLine += *pc;
        }
    }

    //return ret;
}

#endif // wxUSE_STATTEXT
