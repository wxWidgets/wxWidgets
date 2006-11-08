/////////////////////////////////////////////////////////////////////////////
// Name:        src/richtext/richtexthtml.cpp
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
#endif

#include "wx/filename.h"
#include "wx/wfstream.h"
#include "wx/txtstrm.h"

#if wxUSE_FILESYSTEM
#include "wx/filesys.h"
#include "wx/fs_mem.h"
#endif

IMPLEMENT_DYNAMIC_CLASS(wxRichTextHTMLHandler, wxRichTextFileHandler)

int wxRichTextHTMLHandler::sm_fileCounter = 1;

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
    ClearTemporaryImageLocations();

    buffer->Defragment();

    wxTextOutputStream str(stream);

    wxTextAttrEx currentParaStyle = buffer->GetAttributes();
    wxTextAttrEx currentCharStyle = buffer->GetAttributes();

    str << wxT("<html><head></head><body>\n");

    str << wxT("<table border=0 cellpadding=0 cellspacing=0><tr><td width=\"100%\">");

    str << wxString::Format(wxT("<font face=\"%s\" size=\"%ld\" color=\"%s\" >"),
        currentParaStyle.GetFont().GetFaceName().c_str(), PtToSize(currentParaStyle.GetFont().GetPointSize()),
        currentParaStyle.GetTextColour().GetAsString(wxC2S_HTML_SYNTAX).c_str());

    m_font = false;
    m_indent = 0;
    m_list = false;

    wxRichTextObjectList::compatibility_iterator node = buffer->GetChildren().GetFirst();
    while (node)
    {
        wxRichTextParagraph* para = wxDynamicCast(node->GetData(), wxRichTextParagraph);
        wxASSERT (para != NULL);

        if (para)
        {
            wxTextAttrEx paraStyle(para->GetCombinedAttributes());
            
            OutputParagraphFormatting(currentParaStyle, paraStyle, stream);

            wxRichTextObjectList::compatibility_iterator node2 = para->GetChildren().GetFirst();
            while (node2)
            {
                wxRichTextObject* obj = node2->GetData();
                wxRichTextPlainText* textObj = wxDynamicCast(obj, wxRichTextPlainText);
                if (textObj && !textObj->IsEmpty())
                {
                    wxTextAttrEx charStyle(para->GetCombinedAttributes(obj->GetAttributes()));
                    BeginCharacterFormatting(currentCharStyle, charStyle, paraStyle, stream);
                    
                    wxString text = textObj->GetText();

                    if (charStyle.HasTextEffects() && (charStyle.GetTextEffects() & wxTEXT_ATTR_EFFECT_CAPITALS))
                        text.MakeUpper();

                    str << text;

                    EndCharacterFormatting(currentCharStyle, charStyle, paraStyle, stream);
                }

                wxRichTextImage* image = wxDynamicCast(obj, wxRichTextImage);
                if( image && !image->IsEmpty())
                    WriteImage( image, stream );

                node2 = node2->GetNext();
            }
            str << wxT("\n");
        }
        node = node->GetNext();
    }

    str << wxT("</font></td></tr></table></body></html>\n");

    return true;
}

void wxRichTextHTMLHandler::BeginCharacterFormatting(const wxTextAttrEx& currentStyle, const wxTextAttrEx& thisStyle, const wxTextAttrEx& paraStyle, wxOutputStream& stream)
{
    wxTextOutputStream str(stream);

    // Is the item a bulleted one?
    if ( paraStyle.GetBulletStyle() != wxTEXT_ATTR_BULLET_STYLE_NONE )
    {
        // Is there any opened list?
        if (m_list)
        {
            // Yes there is

            // Is the item among the previous ones?
            // Is the item one of the previous list tag's child items?            
            if ((paraStyle.GetLeftIndent() == (m_indent + 100)) || (paraStyle.GetLeftIndent() < 100))
                str << wxT("<li>"); //Yes it is
            else
            {
                // No it isn't, so we should close the list tag
                str << (m_is_ul ? wxT("</ul>") : wxT("</ol>"));

                // And renavigate to new list's horizontal position
                NavigateToListPosition(paraStyle, str);

                // Get the appropriate tag, an ol for numerical values, an ul for dot, square etc.
                wxString tag;
                TypeOfList(paraStyle, tag);
                str << tag << wxT("<li>");
            }
        }
        else
        {
            // No there isn't a list.
            // navigate to new list's horizontal position(indent)
            NavigateToListPosition(paraStyle, str);

            // Get the appropriate tag, an ol for numerical values, an ul for dot, square etc.
            wxString tag;
            TypeOfList(paraStyle, tag);
            str << tag << wxT("<li>");

            // Now we have a list, mark it.
            m_list = true;
        }
    }
    else if( m_list )
    {
        // The item is not bulleted and there is a list what should be closed now.
        // So close the list

        str << (m_is_ul ? wxT("</ul>") : wxT("</ol>"));

        // And mark as there is no an opened list
        m_list = false;
    }

    // does the item have an indentation ?
    if( paraStyle.GetLeftIndent() )
    {
        if (paraStyle.GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_NONE)
        {
            if (m_indent)
            {
                if ((paraStyle.GetLeftIndent() + paraStyle.GetLeftSubIndent()) == m_indent)
                {
                    if (paraStyle.GetLeftSubIndent() < 0)
                    {
                        str << SymbolicIndent(~paraStyle.GetLeftSubIndent());
                    }
                }
                else
                {
                    if (paraStyle.GetLeftIndent() + paraStyle.GetLeftSubIndent() > m_indent)
                    {
                        Indent(paraStyle, str);
                        m_indent = paraStyle.GetLeftIndent() + paraStyle.GetLeftSubIndent();
                        m_indents.Add( m_indent );
                    }
                    else
                    {
                        int i = m_indents.size() - 1;
                        for (; i > -1; i--)
                        {
                            if (m_indent < (paraStyle.GetLeftIndent() + paraStyle.GetLeftSubIndent()))
                            {
                                Indent(paraStyle, str);
                                m_indent = paraStyle.GetLeftIndent() + paraStyle.GetLeftSubIndent();
                                m_indents.Add( m_indent );

                                break;
                            }
                            else if (m_indent == (paraStyle.GetLeftIndent() + paraStyle.GetLeftSubIndent()))
                            {
                                if (paraStyle.GetLeftSubIndent() < 0)
                                {
                                    str << SymbolicIndent(~paraStyle.GetLeftSubIndent());
                                }
                                break;
                            }
                            else
                            {
                                str << wxT("</td></tr></table>");

                                m_indents.RemoveAt(i);

                                if(i < 1)
                                {
                                    m_indent=0; break;
                                }
                                m_indent = m_indents[i-1];
                            }
                        }
                    }
                }
            }
            else
            {
                Indent(paraStyle, str);
                m_indent = paraStyle.GetLeftIndent() + paraStyle.GetLeftSubIndent();
                m_indents.Add( m_indent );
            }
        }
    }
    else if (m_indent)
    {
        // The item is not indented and there is a table(s) that should be closed now.

        for (unsigned int i = 0; i < m_indents.size(); i++)
            str << wxT("</td></tr></table>");

        m_indent = 0;
        m_indents.Clear();
    }


    wxString style;

    // Is there any change in the font properties of the item?
    if (thisStyle.GetFont().GetFaceName() != currentStyle.GetFont().GetFaceName())
    {
        wxString faceName(thisStyle.GetFont().GetFaceName());
        style += wxString::Format(wxT(" face=\"%s\""), faceName.c_str());
    }
    if (thisStyle.GetFont().GetPointSize() != currentStyle.GetFont().GetPointSize())
        style += wxString::Format(wxT(" size=\"%ld\""), PtToSize(thisStyle.GetFont().GetPointSize()));
    if (thisStyle.GetTextColour() != currentStyle.GetTextColour() )
    {
        wxString color(thisStyle.GetTextColour().GetAsString(wxC2S_HTML_SYNTAX));
        style += wxString::Format(wxT(" color=\"%s\""), color.c_str());
    }

    if (style.size())
    {
        str << wxString::Format(wxT("<font %s >"), style.c_str());
        m_font = true;
    }

    if (thisStyle.GetFont().GetWeight() == wxBOLD)
        str << wxT("<b>");
    if (thisStyle.GetFont().GetStyle() == wxITALIC)
        str << wxT("<i>");
    if (thisStyle.GetFont().GetUnderlined())
        str << wxT("<u>");
    
    if (thisStyle.HasURL())
        str << wxT("<a href=\"") << thisStyle.GetURL() << wxT("\">");
}

void wxRichTextHTMLHandler::EndCharacterFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, const wxTextAttrEx& WXUNUSED(paraStyle), wxOutputStream& stream)
{
    wxTextOutputStream str(stream);

    if (thisStyle.HasURL())
        str << wxT("</a>");

    if (thisStyle.GetFont().GetUnderlined())
        str << wxT("</u>");
    if (thisStyle.GetFont().GetStyle() == wxITALIC)
        str << wxT("</i>");
    if (thisStyle.GetFont().GetWeight() == wxBOLD)
        str << wxT("</b>");

    if (m_font)
    {
        m_font = false;
        str << wxT("</font>");
    }
}

/// Output paragraph formatting
void wxRichTextHTMLHandler::OutputParagraphFormatting(const wxTextAttrEx& WXUNUSED(currentStyle), const wxTextAttrEx& thisStyle, wxOutputStream& stream)
{
    // If there is no opened list currently, insert a <p> after every paragraph
    if (!m_list)
    {
        wxTextOutputStream str(stream);
        wxString align = GetAlignment(thisStyle);
        str << wxString::Format(wxT("<p align=\"%s\">"), align.c_str());
    }
    
    if (thisStyle.HasPageBreak())
    {
        wxTextOutputStream str(stream);
        str << wxT("<div style=\"page-break-after:always\"></div>\n");
    }
}

void wxRichTextHTMLHandler::NavigateToListPosition(const wxTextAttrEx& thisStyle, wxTextOutputStream& str)
{
    // indenting an item using an ul/ol tag is equal to inserting 5 x &nbsp; on its left side.
    // so we should start from 100 point left

    // Is the second td's left wall of the current indentaion table at the 100+ point-left-side
    // of the item, horizontally?
    if (m_indent + 100 < thisStyle.GetLeftIndent())
    {
        // yes it is
        LIndent(thisStyle, str);
        m_indent = thisStyle.GetLeftIndent() - 100;
        m_indents.Add( m_indent );
        return;
    }
    // No it isn't

    int i = m_indents.size() - 1;
    for (; i > -1; i--)
    {
        //Is the second td's left wall of the current indentaion table at the 100+ point-left-side
        //of the item ?
        if (m_indent + 100 < thisStyle.GetLeftIndent())
        {
            // Yes it is
            LIndent(thisStyle, str);
            m_indent = thisStyle.GetLeftIndent() - 100;
            m_indents.Add( m_indent );
            break;
        }
        else if (m_indent + 100 == thisStyle.GetLeftIndent())
            break; //exact match
        else
        {
            // No it is not, the second td's left wall of the current indentaion table is at the
            //right side of the current item horizontally, so close it.
            str << wxT("</td></tr></table>");

            m_indents.RemoveAt(i);

            if (i < 1)
            {
                m_indent=0; break;
            }
            m_indent = m_indents[i-1];
        }
    }
}
void wxRichTextHTMLHandler::Indent( const wxTextAttrEx& thisStyle, wxTextOutputStream& str )
{
    //There is no way to indent an item in HTML, but we can use tables.

    // Item -> "Hello world"
    // Its Left Indentation -> 100
    // Its Left Sub-Indentation ->40
    // A typical indentation-table for the item will be construct as the following

    // 3 x nbsp = 60
    // 2 x nbsp = 40
    // LSI = Left Sub Indent
    // LI = Left Indent - LSI
    //
    // -------------------------------------------
    // |&nbsp;&nbsp;nbsp;|nbsp;nbsp;Hello World  |
    // |      |          |    |                  |
    // |        V        |    V                  |
    // |      --LI--     | --LSI--               |
    // -------------------------------------------

    str << wxT("<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr>");

    wxString symbolic_indent = SymbolicIndent( (thisStyle.GetLeftIndent() + thisStyle.GetLeftSubIndent()) - m_indent );
    str << wxString::Format( wxT("<td>%s</td>"), symbolic_indent.c_str() );
    str << wxT("<td width=\"100%\">");

    if (thisStyle.GetLeftSubIndent() < 0)
    {
        str << SymbolicIndent(~thisStyle.GetLeftSubIndent());
    }
}

void wxRichTextHTMLHandler::LIndent( const wxTextAttrEx& thisStyle, wxTextOutputStream& str )
{
    // Code:
    // r.BeginNumberedBullet(1, 200, 60);
    // r.Newline();
    // r.WriteText(wxT("first item"));
    // r.EndNumberedBullet();
    // r.BeginNumberedBullet(2, 200, 60);
    // r.Newline();
    // r.WriteText(wxT("second item."));
    // r.EndNumberedBullet();
    //
    // A typical indentation-table for the item will be construct as the following

    // 1 x nbsp = 20 point
    // ULI -> 100pt (UL/OL tag indents its sub element by 100 point)
    // <--------- 100 pt ---------->|
    // ------------------------------------------------------
    // |&nbsp;&nbsp;nbsp;&nbsp;nbsp;|<ul>                   |
    // |                            |<-ULI-><li>first item  |
    // |                            |<-ULI-><li>second item |
    // |                            |</ul>                  |
    // ------------------------------------------------------
    //                              |<-100->|


    str << wxT("<table width=\"100%\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\"><tr>");

    wxString symbolic_indent = SymbolicIndent( (thisStyle.GetLeftIndent() - m_indent) - 100);
    str << wxString::Format( wxT("<td>%s</td>"), symbolic_indent.c_str() );
    str << wxT("<td width=\"100%\">");
}

void wxRichTextHTMLHandler::TypeOfList( const wxTextAttrEx& thisStyle, wxString& tag )
{
    // We can use number attribute of li tag but not all the browsers support it.
    // also wxHtmlWindow doesn't support type attribute.

    m_is_ul = false;
    if (thisStyle.GetBulletStyle() == (wxTEXT_ATTR_BULLET_STYLE_ARABIC|wxTEXT_ATTR_BULLET_STYLE_PERIOD))
        tag = wxT("<ol type=\"1\">");
    else if (thisStyle.GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_LETTERS_UPPER)
        tag = wxT("<ol type=\"A\">");
    else if (thisStyle.GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_LETTERS_LOWER)
        tag = wxT("<ol type=\"a\">");
    else if (thisStyle.GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_ROMAN_UPPER)
        tag = wxT("<ol type=\"I\">");
    else if (thisStyle.GetBulletStyle() == wxTEXT_ATTR_BULLET_STYLE_ROMAN_LOWER)
        tag = wxT("<ol type=\"i\">");
    else
    {
        tag = wxT("<ul>");
        m_is_ul = true;
    }
}

wxString wxRichTextHTMLHandler::GetAlignment( const wxTextAttrEx& thisStyle )
{
    switch( thisStyle.GetAlignment() )
    {
    case wxTEXT_ALIGNMENT_LEFT:
        return  wxT("left");
    case wxTEXT_ALIGNMENT_RIGHT:
        return wxT("right");
    case wxTEXT_ALIGNMENT_CENTER:
        return wxT("center");
    case wxTEXT_ALIGNMENT_JUSTIFIED:
        return wxT("justify");
    default:
        return wxT("left");
    }
}

void wxRichTextHTMLHandler::WriteImage(wxRichTextImage* image, wxOutputStream& stream)
{
    wxTextOutputStream str(stream);

    str << wxT("<img src=\"");

#if wxUSE_FILESYSTEM
    if (GetFlags() & wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_MEMORY)
    {
        if (!image->GetImage().Ok() && image->GetImageBlock().GetData())
            image->LoadFromBlock();
        if (image->GetImage().Ok() && !image->GetImageBlock().GetData())
            image->MakeBlock();

        if (image->GetImage().Ok())
        {        
            wxString ext(image->GetImageBlock().GetExtension());
            wxString tempFilename(wxString::Format(wxT("image%d.%s"), sm_fileCounter, (const wxChar*) ext));
            wxMemoryFSHandler::AddFile(tempFilename, image->GetImage(), image->GetImageBlock().GetImageType());
            
            m_imageLocations.Add(tempFilename);
            
            str << wxT("memory:") << tempFilename;
        }
        else
            str << wxT("memory:?");

        sm_fileCounter ++;
    }
    else if (GetFlags() & wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_FILES)
    {
        if (!image->GetImage().Ok() && image->GetImageBlock().GetData())
            image->LoadFromBlock();
        if (image->GetImage().Ok() && !image->GetImageBlock().GetData())
            image->MakeBlock();

        if (image->GetImage().Ok())
        {        
            wxString tempDir(GetTempDir());
            if (tempDir.IsEmpty())
                tempDir = wxFileName::GetTempDir();
            
            wxString ext(image->GetImageBlock().GetExtension());
            wxString tempFilename(wxString::Format(wxT("%s/image%d.%s"), (const wxChar*) tempDir, sm_fileCounter, (const wxChar*) ext));
            image->GetImageBlock().Write(tempFilename);
            
            m_imageLocations.Add(tempFilename);
            
            str << wxFileSystem::FileNameToURL(tempFilename);            
        }
        else
            str << wxT("file:?");

        sm_fileCounter ++;
    }
    else // if (GetFlags() & wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_BASE64) // this is implied
#endif
    {
        str << wxT("data:");
        str << GetMimeType(image->GetImageBlock().GetImageType());
        str << wxT(";base64,");

        if (image->GetImage().Ok() && !image->GetImageBlock().GetData())
            image->MakeBlock();

        wxChar* data = b64enc( image->GetImageBlock().GetData(), image->GetImageBlock().GetDataSize() );
        str << data;

        delete[] data;
    }

    str << wxT("\" />");
}

long wxRichTextHTMLHandler::PtToSize(long size)
{
    // return approximate size
    if (size < 9 ) return 1;
    else if( size < 11 ) return 2;
    else if( size < 14 ) return 3;
    else if( size < 18 ) return 4;
    else if( size < 23 ) return 5;
    else if( size < 30 ) return 6;
    else return 7;
}

wxString wxRichTextHTMLHandler::SymbolicIndent(long indent)
{
    wxString in;
    for(;indent > 0; indent -= 20)
        in.Append( wxT("&nbsp;") );
    return in;
}

const wxChar* wxRichTextHTMLHandler::GetMimeType(int imageType)
{
    switch(imageType)
    {
    case wxBITMAP_TYPE_BMP:
        return wxT("image/bmp");
    case wxBITMAP_TYPE_TIF:
        return wxT("image/tiff");
    case wxBITMAP_TYPE_GIF:
        return wxT("image/gif");
    case wxBITMAP_TYPE_PNG:
        return wxT("image/png");
    case wxBITMAP_TYPE_JPEG:
        return wxT("image/jpeg");
    default:
        return wxT("image/unknown");
    }
}

// exim-style base64 encoder
wxChar* wxRichTextHTMLHandler::b64enc( unsigned char* input, size_t in_len )
{
    // elements of enc64 array must be 8 bit values
    // otherwise encoder will fail
    // hmmm.. Does wxT macro define a char as 16 bit value
    // when compiling with UNICODE option?
    static const wxChar enc64[] = wxT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
    wxChar* output = new wxChar[4*((in_len+2)/3)+1];
    wxChar* p = output;

    while( in_len-- > 0 )
    {
        register wxChar a, b;

        a = *input++;

        *p++ = enc64[ (a >> 2) & 0x3f ];

        if( in_len-- == 0 )
        {
            *p++ = enc64[ (a << 4 ) & 0x30 ];
            *p++ = '=';
            *p++ = '=';
            break;
        }

        b = *input++;

        *p++ = enc64[(( a << 4 ) | ((b >> 4) &0xf )) & 0x3f];

        if( in_len-- == 0 )
        {
            *p++ = enc64[ (b << 2) & 0x3f ];
            *p++ = '=';
            break;
        }

        a = *input++;

        *p++ = enc64[ ((( b << 2 ) & 0x3f ) | ((a >> 6)& 0x3)) & 0x3f ];

        *p++ = enc64[ a & 0x3f ];
    }
    *p = 0;

    return output;
}
#endif
// wxUSE_STREAMS

/// Delete the in-memory or temporary files generated by the last operation
bool wxRichTextHTMLHandler::DeleteTemporaryImages()
{
    return DeleteTemporaryImages(GetFlags(), m_imageLocations);
}

/// Delete the in-memory or temporary files generated by the last operation
bool wxRichTextHTMLHandler::DeleteTemporaryImages(int flags, const wxArrayString& imageLocations)
{
    size_t i;
    for (i = 0; i < imageLocations.GetCount(); i++)
    {
        wxString location = imageLocations[i];
        
        if (flags & wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_MEMORY)
        {
#if wxUSE_FILESYSTEM
            wxMemoryFSHandler::RemoveFile(location);
#endif
        }
        else if (flags & wxRICHTEXT_HANDLER_SAVE_IMAGES_TO_FILES)
        {
            if (wxFileExists(location))
                wxRemoveFile(location);
        }
    }
    
    return true;
}


#endif
// wxUSE_RICHTEXT

