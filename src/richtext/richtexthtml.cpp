/////////////////////////////////////////////////////////////////////////////
// Name:        richtext/richtexthtml.cpp
// Purpose:     HTML I/O for wxRichTextCtrl
// Author:      Julian Smart
// Modified by:
// Created:     2005-09-30
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#if wxUSE_RICHTEXT

#include "wx/richtext/richtexthtml.h"

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/txtstrm.h"

IMPLEMENT_DYNAMIC_CLASS(wxRichTextHTMLHandler, wxRichTextFileHandler)

/// Can we handle this filename (if using files)? By default, checks the extension.
bool wxRichTextHTMLHandler::CanHandle(const wxString& filename) const
{
    wxString path, file, ext;
    wxSplitPath(filename, & path, & file, & ext);

    return (ext.Lower() == wxT("html") || ext.Lower() == wxT("htm"));
}


#if wxUSE_STREAMS
bool wxRichTextHTMLHandler::DoLoadFile(wxRichTextBuffer *WXUNUSED(buffer), wxInputStream& WXUNUSED(stream))
{
    return false;
}

/*
 * We need to output only _changes_ in character formatting.
 */

bool wxRichTextHTMLHandler::DoSaveFile(wxRichTextBuffer *buffer, wxOutputStream& stream)
{
    buffer->Defragment();

    wxTextOutputStream str(stream);

    wxTextAttrEx currentParaStyle = buffer->GetAttributes();
    wxTextAttrEx currentCharStyle = buffer->GetAttributes();

    str << wxT("<html><head></head><body>\n");

    wxRichTextObjectList::compatibility_iterator node = buffer->GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para)
        {
            OutputParagraphFormatting(currentParaStyle, para->GetAttributes(), stream, true);

            wxRichTextObjectList::compatibility_iterator node2 = para->GetChildren().GetFirst();
            while (node2)
            {
                wxRichTextObject* obj = node2->GetData();
                wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
                if (textObj && !textObj->IsEmpty())
                {
                    OutputCharacterFormatting(currentCharStyle, obj->GetAttributes(), stream, true);

                    str << textObj->GetText();

                    OutputCharacterFormatting(currentCharStyle, obj->GetAttributes(), stream, false);
                }

                node2 = node2->GetNext();
            }

            OutputParagraphFormatting(currentParaStyle, para->GetAttributes(), stream, false);

            str << wxT("<P>\n");
        }

        node = node->GetNext();
    }

    str << wxT("</body></html>\n");

    return true;
}

/// Output character formatting
void wxRichTextHTMLHandler::OutputCharacterFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, wxOutputStream& stream, bool start)
{
    wxTextOutputStream str(stream);

    bool isBold = false;
    bool isItalic = false;
    bool isUnderline = false;
    wxString faceName;

    if (thisStyle.GetFont().Ok())
    {
        if (thisStyle.GetFont().GetWeight() == wxBOLD)
            isBold = true;
        if (thisStyle.GetFont().GetStyle() == wxITALIC)
            isItalic = true;
        if (thisStyle.GetFont().GetUnderlined())
            isUnderline = true;

        faceName = thisStyle.GetFont().GetFaceName();
    }

    if (start)
    {
        if (isBold)
            str << wxT("<b>");
        if (isItalic)
            str << wxT("<i>");
        if (isUnderline)
            str << wxT("<u>");
    }
    else
    {
        if (isUnderline)
            str << wxT("</u>");
        if (isItalic)
            str << wxT("</i>");
        if (isBold)
            str << wxT("</b>");
    }
}

/// Output paragraph formatting
void wxRichTextHTMLHandler::OutputParagraphFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, wxOutputStream& stream, bool start)
{
    // TODO: lists, indentation (using tables), fonts, right-align, ...

    wxTextOutputStream str(stream);
    bool isCentered = false;

    if (thisStyle.GetAlignment() == wxTEXT_ALIGNMENT_CENTRE)
    {
        isCentered = true;
    }

    if (start)
    {
        if (isCentered)
            str << wxT("<center>");
    }
    else
    {
        if (isCentered)
            str << wxT("</center>");
    }
}

#endif
    // wxUSE_STREAMS

#endif
    // wxUSE_RICHTEXT
    
