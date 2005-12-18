/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richedithtml.h
// Purpose:     HTML I/O for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RICHTEXTHTML_H_
#define _WX_RICHTEXTHTML_H_

/*!
 * Includes
 */

#include "wx/richtext/richtextbuffer.h"

/*!
 * wxRichTextHTMLHandler
 */

class WXDLLIMPEXP_ADV wxRichTextHTMLHandler: public wxRichTextFileHandler
{
    DECLARE_CLASS(wxRichTextHTMLHandler)
public:
    wxRichTextHTMLHandler(const wxString& name = wxT("HTML"), const wxString& ext = wxT("html"), int type = wxRICHTEXT_TYPE_HTML)
        : wxRichTextFileHandler(name, ext, type)
        { }

#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream);
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream);
#endif

    /// Can we save using this handler?
    virtual bool CanSave() const { return true; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return false; }

    /// Can we handle this filename (if using files)? By default, checks the extension.
    virtual bool CanHandle(const wxString& filename) const;

    /// Output character formatting
    virtual void OutputCharacterFormatting(const wxTextAttrEx& currentStyle, const wxTextAttrEx& thisStyle, wxOutputStream& stream, bool start);

    /// Output paragraph formatting
    virtual void OutputParagraphFormatting(const wxTextAttrEx& currentStyle, const wxTextAttrEx& thisStyle, wxOutputStream& stream, bool start);

protected:

};

#endif
    // _WX_RICHTEXTXML_H_
