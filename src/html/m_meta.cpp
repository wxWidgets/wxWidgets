/////////////////////////////////////////////////////////////////////////////
// Name:        m_meta.cpp
// Purpose:     wxHtml module for parsing <meta> tag
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vaclav Slavik
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

#include "wx/fontmap.h"
#include "wx/html/forcelnk.h"
#include "wx/html/m_templ.h"
#include "wx/html/htmlcell.h"

FORCE_LINK_ME(m_meta)




TAG_HANDLER_BEGIN(META, "META")

    TAG_HANDLER_PROC(tag)
    {
        if (tag.HasParam(_T("HTTP-EQUIV")) && 
            tag.GetParam(_T("HTTP-EQUIV")) == _T("Content-Type") &&
            tag.HasParam(_T("CONTENT")))
        {
            wxString content = tag.GetParam(_T("CONTENT"));
            if (content.Left(19) == _T("text/html; charset="))
            {
                wxFontEncoding enc = 
                    wxTheFontMapper -> CharsetToEncoding(content.Mid(19));
                if (enc == wxFONTENCODING_SYSTEM) return FALSE;
                if (enc == m_WParser -> GetInputEncoding()) return FALSE;

                m_WParser -> SetInputEncoding(enc);
                m_WParser -> GetContainer() -> InsertCell(
                    new wxHtmlFontCell(m_WParser -> CreateCurrentFont()));
            }
        }
        return FALSE;
    }

TAG_HANDLER_END(META)


TAGS_MODULE_BEGIN(MetaTag)

    TAGS_MODULE_ADD(META)

TAGS_MODULE_END(MetaTag)

#endif
