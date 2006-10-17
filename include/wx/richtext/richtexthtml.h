/////////////////////////////////////////////////////////////////////////////
// Name:        wx/richtext/richtexthtml.h
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

class WXDLLIMPEXP_RICHTEXT wxRichTextHTMLHandler: public wxRichTextFileHandler
{
    DECLARE_CLASS(wxRichTextHTMLHandler)
public:
    wxRichTextHTMLHandler(const wxString& name = wxT("HTML"), const wxString& ext = wxT("html"), int type = wxRICHTEXT_TYPE_HTML)
        : wxRichTextFileHandler(name, ext, type)
        { }

    /// Can we save using this handler?
    virtual bool CanSave() const { return true; }

    /// Can we load using this handler?
    virtual bool CanLoad() const { return false; }

    /// Can we handle this filename (if using files)? By default, checks the extension.
    virtual bool CanHandle(const wxString& filename) const;

protected:

#if wxUSE_STREAMS
    virtual bool DoLoadFile(wxRichTextBuffer *buffer, wxInputStream& stream);
    virtual bool DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream);
#endif

    /// Output character formatting
    virtual void BeginCharacterFormatting(const wxTextAttrEx& currentStyle, const wxTextAttrEx& thisStyle, const wxTextAttrEx& paraStyle, wxOutputStream& stream );
    virtual void EndCharacterFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, const wxTextAttrEx& paraStyle, wxOutputStream& stream );

    /// Output paragraph formatting
    virtual void OutputParagraphFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, wxOutputStream& stream/*, bool start*/);

    /// Converts an image to its base64 equivalent
    void Image_to_Base64(wxRichTextImage* image, wxOutputStream& stream);

    /// Builds required indentation
    void Indent( const wxTextAttrEx& thisStyle, wxTextOutputStream& str );

    /// Left indent
    void LIndent( const wxTextAttrEx& thisStyle, wxTextOutputStream& str );

    /// Converts from pt to size property compatible height
    long Pt_To_Size(long size);

    /// Typical base64 encoder
    wxChar* b64enc( unsigned char* input, size_t in_len );

    /// Gets the mime type of the given wxBITMAP_TYPE
    const wxChar* GetMimeType(int imageType);

    /// Gets the html equivalent of the specified value
    wxString GetAlignment( const wxTextAttrEx& thisStyle );

    /// Generates &nbsp; array for indentations
    wxString SymbolicIndent(long indent);

    /// Finds the html equivalent of the specified bullet
    void TypeOfList( const wxTextAttrEx& thisStyle, wxString& tag );

    /// Closes existings or Opens new tables for navigation to an item's horizontal position.
    void NavigateToListPosition( const wxTextAttrEx& thisStyle, wxTextOutputStream& str );

    /// Indentation values of the table tags
    wxArrayInt m_indents;

    /// Horizontal position of the current table
    long m_indent;

    /// Is there any opened font tag
    bool m_font;

    /// Is there any opened ul/ol tag
    bool m_list;

    /// type of list, ul or ol?
    bool m_is_ul;

};

#endif
    // _WX_RICHTEXTXML_H_
