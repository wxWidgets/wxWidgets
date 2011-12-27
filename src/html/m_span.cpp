/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/m_span.cpp
// Purpose:     wxHtml module for span handling
// Author:      Nigel Paton
// RCS-ID:      $Id$
// Copyright:   wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HTML

#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"
#include "wx/fontenum.h"
#include "wx/tokenzr.h"
#include "wx/html/styleparams.h"

FORCE_LINK_ME(m_span)


TAG_HANDLER_BEGIN(SPAN, "SPAN" )

    TAG_HANDLER_VARS
        wxArrayString m_Faces;

    TAG_HANDLER_CONSTR(SPAN) { }

    TAG_HANDLER_PROC(tag)
    {
        wxColour oldclr = m_WParser->GetActualColor();
        int oldsize = m_WParser->GetFontSize();
        int oldbold = m_WParser->GetFontBold();
        int olditalic = m_WParser->GetFontItalic();
        int oldunderlined = m_WParser->GetFontUnderlined();
        wxString oldfontface = m_WParser->GetFontFace();

        // Load any style parameters
        wxHtmlStyleParams styleParams(tag);

        wxString str;

        str = styleParams.GetParam(wxS("color"));
        if ( !str.empty() )
        {
            wxColour clr;
            if ( wxHtmlTag::ParseAsColour(str, &clr) )
            {
                m_WParser->SetActualColor(clr);
                m_WParser->GetContainer()->InsertCell(new wxHtmlColourCell(clr));
            }
        }

        str = styleParams.GetParam(wxS("font-size"));
        if ( !str.empty() )
        {
            // Point size
            int foundIndex = str.Find(wxS("pt"));
            if (foundIndex != wxNOT_FOUND)
            {
                str.Truncate(foundIndex);

                long sizeValue;
                if (str.ToLong(&sizeValue) == true)
                {
                    // Set point size
                    m_WParser->SetFontPointSize(sizeValue);
                    m_WParser->GetContainer()->InsertCell(
                         new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
                }
            }
            // else: check for other ways of specifying size (TODO)
        }

        str = styleParams.GetParam(wxS("font-weight"));
        if ( !str.empty() )
        {
            // Only bold and normal supported just now
            if ( str == wxS("bold") )
            {
                m_WParser->SetFontBold(true);
                m_WParser->GetContainer()->InsertCell(
                     new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            }
            else if ( str == wxS("normal") )
            {
                m_WParser->SetFontBold(false);
                m_WParser->GetContainer()->InsertCell(
                     new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            }
        }

        str = styleParams.GetParam(wxS("font-style"));
        if ( !str.empty() )
        {
            // "oblique" and "italic" are more or less the same.
            // "inherit" (using the parent font) is not supported.
            if ( str == wxS("oblique") || str == wxS("italic") )
            {
                m_WParser->SetFontItalic(true);
                m_WParser->GetContainer()->InsertCell(
                     new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            }
            else if ( str == wxS("normal") )
            {
                m_WParser->SetFontItalic(false);
                m_WParser->GetContainer()->InsertCell(
                     new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            }
        }

        str = styleParams.GetParam(wxS("text-decoration"));
        if ( !str.empty() )
        {
            // Only underline is supported.
            if ( str == wxS("underline") )
            {
                m_WParser->SetFontUnderlined(true);
                m_WParser->GetContainer()->InsertCell(
                     new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
            }
        }

        str = styleParams.GetParam(wxS("font-family"));
        if ( !str.empty() )
        {
            m_WParser->SetFontFace(str);
            m_WParser->GetContainer()->InsertCell(
                 new wxHtmlFontCell(m_WParser->CreateCurrentFont()));
        }

        ParseInner(tag);

        m_WParser->SetFontSize(oldsize);
        m_WParser->SetFontBold(oldbold);
        m_WParser->SetFontUnderlined(oldunderlined);
        m_WParser->SetFontFace(oldfontface);
        m_WParser->SetFontItalic(olditalic);
        m_WParser->GetContainer()->InsertCell(
                new wxHtmlFontCell(m_WParser->CreateCurrentFont()));

        if (oldclr != m_WParser->GetActualColor())
        {
            m_WParser->SetActualColor(oldclr);
            m_WParser->GetContainer()->InsertCell(
                new wxHtmlColourCell(oldclr));
        }

        return true;
    }

TAG_HANDLER_END(SPAN)


TAGS_MODULE_BEGIN(Spans)

    TAGS_MODULE_ADD(SPAN)

TAGS_MODULE_END(Spans)

#endif // wxUSE_HTML
