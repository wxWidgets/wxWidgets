/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/private/stattext.h
// Purpose:     Internal declarations for dlgcmn.cpp and stattextcmn.cpp
// Author:      Francesco Montorsi
// Created:     2007-01-07 (extracted from dlgcmn.cpp)
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vadim Zeitlin
//              (c) 2007 wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_STATTEXT_H_
#define _WX_PRIVATE_STATTEXT_H_

#include "wx/window.h"

#if wxUSE_STATTEXT

// ----------------------------------------------------------------------------
// wxTextWrapper
// ----------------------------------------------------------------------------

// this class is used to wrap the text on word boundary: wrapping is done by
// calling OnStartLine() and OnOutputLine() functions
class wxTextWrapper
{
public:
    wxTextWrapper() { m_eol = false; }

    // win is used for getting the font, text is the text to wrap, width is the
    // max line width or -1 to disable wrapping
    void Wrap(wxWindow *win, const wxString& text, int widthMax);

    // we don't need it, but just to avoid compiler warnings
    virtual ~wxTextWrapper() { }

protected:
    // line may be empty
    virtual void OnOutputLine(const wxString& line) = 0;

    // called at the start of every new line (except the very first one)
    virtual void OnNewLine() { }

private:
    // call OnOutputLine() and set m_eol to true
    void DoOutputLine(const wxString& line)
    {
        OnOutputLine(line);

        m_eol = true;
    }

    // this function is a destructive inspector: when it returns true it also
    // resets the flag to false so calling it again woulnd't return true any
    // more
    bool IsStartOfNewLine()
    {
        if ( !m_eol )
            return false;

        m_eol = false;

        return true;
    }


    bool m_eol;
};

enum
{
    wxMARKUP_ENTITY_AMP,
    wxMARKUP_ENTITY_LT,
    wxMARKUP_ENTITY_GT,
    wxMARKUP_ENTITY_APOS,
    wxMARKUP_ENTITY_QUOT,
    wxMARKUP_ENTITY_MAX
};

enum
{
    wxMARKUP_ELEMENT_NAME,
    wxMARKUP_ELEMENT_VALUE,
    wxMARKUP_ELEMENT_MAX
};

// these are the only entities treated in a special way by wxStaticText::SetLabel()
// when the wxST_MARKUP style is used; use as:
//
//    wxMarkupEntities[wxMARKUP_ELEMENT_NAME][wxMARKUP_ENTITY_GT]  == "&gt;"
//    wxMarkupEntities[wxMARKUP_ELEMENT_VALUE][wxMARKUP_ENTITY_GT] == ">"
//
extern const wxChar *wxMarkupEntities[wxMARKUP_ELEMENT_MAX][wxMARKUP_ENTITY_MAX];

#endif // wxUSE_STATTEXT

#endif // _WX_PRIVATE_STATTEXT_H_
