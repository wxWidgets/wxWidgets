/////////////////////////////////////////////////////////////////////////////
// Name:        m_pre.cpp
// Purpose:     wxHtml module for <PRE> ... </PRE> tag (code citation)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS
#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif


#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"

#include "wx/html/htmlcell.h"
#include "wx/tokenzr.h"
#include "wx/encconv.h"

FORCE_LINK_ME(m_pre)

//-----------------------------------------------------------------------------
// The list handler:
//-----------------------------------------------------------------------------


TAG_HANDLER_BEGIN(PRE, "PRE")

    TAG_HANDLER_PROC(tag)
    {
        wxHtmlContainerCell *c;

        int fixed = m_WParser->GetFontFixed(),
            italic = m_WParser->GetFontItalic(),
            underlined = m_WParser->GetFontUnderlined(),
            bold = m_WParser->GetFontBold(),
            fsize = m_WParser->GetFontSize();

        c = m_WParser->GetContainer();
        m_WParser->SetFontUnderlined(FALSE);
        m_WParser->SetFontBold(FALSE);
        m_WParser->SetFontItalic(FALSE);
        m_WParser->SetFontFixed(TRUE);
        m_WParser->SetFontSize(3);
        c->InsertCell(new wxHtmlFontCell(m_WParser->CreateCurrentFont()));

        m_WParser->CloseContainer();
        c = m_WParser->OpenContainer();
        c->SetAlignHor(wxHTML_ALIGN_LEFT);

        wxString src, srcMid;

        src = *m_WParser->GetSource();
        srcMid = src.Mid(tag.GetBeginPos(), 
                         tag.GetEndPos1() - tag.GetBeginPos());
        srcMid.Replace(wxT("\t"), wxT("        "));
        srcMid.Replace(wxT(" "), wxT("&nbsp;"));
        srcMid.Replace(wxT("\n"), wxT("<br>"));

        // It is safe to temporarily change the source being parsed,
        // provided we restore the state back after parsing
        m_Parser->SetSource(srcMid);
        m_Parser->DoParsing();
        m_Parser->SetSource(src);
        
        m_WParser->CloseContainer();
        c = m_WParser->OpenContainer();

        m_WParser->SetFontUnderlined(underlined);
        m_WParser->SetFontBold(bold);
        m_WParser->SetFontItalic(italic);
        m_WParser->SetFontFixed(fixed);
        m_WParser->SetFontSize(fsize);
        c->InsertCell(new wxHtmlFontCell(m_WParser->CreateCurrentFont()));

        return TRUE;
    }

TAG_HANDLER_END(PRE)





TAGS_MODULE_BEGIN(Pre)

    TAGS_MODULE_ADD(PRE)

TAGS_MODULE_END(Pre)

#endif
