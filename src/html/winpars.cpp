/////////////////////////////////////////////////////////////////////////////
// Name:        winpars.cpp
// Purpose:     wxHtmlParser class (generic parser)
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
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/htmldefs.h"
#include "wx/html/winpars.h"
#include "wx/html/htmlwin.h"
#include "wx/fontmap.h"
#include "wx/log.h"


//-----------------------------------------------------------------------------
// wxHtmlWinParser
//-----------------------------------------------------------------------------


wxList wxHtmlWinParser::m_Modules;

wxHtmlWinParser::wxHtmlWinParser(wxWindow *wnd) : wxHtmlParser()
{
    m_Window = wnd;
    m_Container = NULL;
    m_DC = NULL;
    m_CharHeight = m_CharWidth = 0;
    m_UseLink = FALSE;
    m_EncConv = NULL;
    m_InputEnc = m_OutputEnc = wxFONTENCODING_DEFAULT;

    {
        int i, j, k, l, m;
        for (i = 0; i < 2; i++)
            for (j = 0; j < 2; j++)
                for (k = 0; k < 2; k++)
                    for (l = 0; l < 2; l++)
                        for (m = 0; m < 7; m++) {
                            m_FontsTable[i][j][k][l][m] = NULL;
                            m_FontsFacesTable[i][j][k][l][m] = wxEmptyString;
                            m_FontsEncTable[i][j][k][l][m] = wxFONTENCODING_DEFAULT;
                        }
#ifdef __WXMSW__
        static int default_sizes[7] = {7, 8, 10, 12, 16, 22, 30};
#elif defined(__WXMAC__)
        static int default_sizes[7] = {9, 12, 14, 18, 24, 30, 36};
#else
        static int default_sizes[7] = {10, 12, 14, 16, 19, 24, 32};
#endif
        SetFonts("", "", default_sizes);
    }

    // fill in wxHtmlParser's tables:
    wxNode *node = m_Modules.GetFirst();
    while (node) {
        wxHtmlTagsModule *mod = (wxHtmlTagsModule*) node -> GetData();
        mod -> FillHandlersTable(this);
        node = node -> GetNext();
    }
}


wxHtmlWinParser::~wxHtmlWinParser()
{
    int i, j, k, l, m;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++)
                for (l = 0; l < 2; l++)
                    for (m = 0; m < 7; m++) {
                        if (m_FontsTable[i][j][k][l][m] != NULL) 
                            delete m_FontsTable[i][j][k][l][m];
                    }
    if (m_EncConv) delete m_EncConv;
}


void wxHtmlWinParser::AddModule(wxHtmlTagsModule *module)
{
    m_Modules.Append(module);
}



void wxHtmlWinParser::SetFonts(wxString normal_face, wxString fixed_face, const int *sizes)
{
    int i, j, k, l, m;

    for (i = 0; i < 7; i++) m_FontsSizes[i] = sizes[i];
    m_FontFaceFixed = fixed_face;
    m_FontFaceNormal = normal_face;
    
    SetInputEncoding(m_InputEnc);

    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 2; k++)
                for (l = 0; l < 2; l++)
                    for (m = 0; m < 7; m++) {
                        if (m_FontsTable[i][j][k][l][m] != NULL) {
                            delete m_FontsTable[i][j][k][l][m];
                            m_FontsTable[i][j][k][l][m] = NULL;
                        }
                    }
}



void wxHtmlWinParser::InitParser(const wxString& source)
{
    wxHtmlParser::InitParser(source);
    wxASSERT_MSG(m_DC != NULL, _("no DC assigned to wxHtmlWinParser!!"));

    m_FontBold = m_FontItalic = m_FontUnderlined = m_FontFixed = FALSE;
    m_FontSize = 3; //default one
    CreateCurrentFont();           // we're selecting default font into
    m_DC -> GetTextExtent("H", &m_CharWidth, &m_CharHeight);
                /* NOTE : we're not using GetCharWidth/Height() because
                   of differences under X and win 
                 */

    m_UseLink = FALSE;
    m_Link = wxHtmlLinkInfo("", "");
    m_LinkColor.Set(0, 0, 0xFF);
    m_ActualColor.Set(0, 0, 0);
    m_Align = wxHTML_ALIGN_LEFT;
    m_tmpLastWasSpace = FALSE;

    OpenContainer();

    OpenContainer();
    m_Container -> InsertCell(new wxHtmlColourCell(m_ActualColor));
    m_Container -> InsertCell(new wxHtmlFontCell(CreateCurrentFont()));
}



void wxHtmlWinParser::DoneParser()
{
    m_Container = NULL;
    SetInputEncoding(wxFONTENCODING_DEFAULT); // for next call
    wxHtmlParser::DoneParser();
}



wxObject* wxHtmlWinParser::GetProduct()
{
    wxHtmlContainerCell *top;

    CloseContainer();
    OpenContainer();

    top = m_Container;
    while (top -> GetParent()) top = top -> GetParent();
    return top;
}



void wxHtmlWinParser::AddText(const char* txt)
{
    wxHtmlCell *c;
    int i = 0, x, lng = strlen(txt);
    char temp[wxHTML_BUFLEN];
    register char d;
    int templen = 0;

    if (m_tmpLastWasSpace) {
        while ((i < lng) && ((txt[i] == '\n') || (txt[i] == '\r') || (txt[i] == ' ') || (txt[i] == '\t'))) i++;
    }

    while (i < lng) {
        x = 0;
        d = temp[templen++] = txt[i];
        if ((d == '\n') || (d == '\r') || (d == ' ') || (d == '\t')) {
            i++, x++;
            while ((i < lng) && ((txt[i] == '\n') || (txt[i] == '\r') || (txt[i] == ' ') || (txt[i] == '\t'))) i++, x++;
        }
        else i++;

        if (x) {
            temp[templen-1] = ' ';
            temp[templen] = 0;
            templen = 0;
            if (m_EncConv) m_EncConv -> Convert(temp);
            c = new wxHtmlWordCell(temp, *(GetDC()));
            if (m_UseLink) c -> SetLink(m_Link);
            m_Container -> InsertCell(c);
            m_tmpLastWasSpace = TRUE;
        }
    }
    if (templen) {
        temp[templen] = 0;
        if (m_EncConv) m_EncConv -> Convert(temp);
        c = new wxHtmlWordCell(temp, *(GetDC()));
        if (m_UseLink) c -> SetLink(m_Link);
        m_Container -> InsertCell(c);
        m_tmpLastWasSpace = FALSE;
    }
}



wxHtmlContainerCell* wxHtmlWinParser::OpenContainer()
{
    m_Container = new wxHtmlContainerCell(m_Container);
    m_Container -> SetAlignHor(m_Align);
    m_tmpLastWasSpace = TRUE;
        /* to avoid space being first character in paragraph */
    return m_Container;
}



wxHtmlContainerCell* wxHtmlWinParser::SetContainer(wxHtmlContainerCell *c)
{
    m_tmpLastWasSpace = TRUE;
        /* to avoid space being first character in paragraph */
    return m_Container = c;
}



wxHtmlContainerCell* wxHtmlWinParser::CloseContainer()
{
    m_Container = m_Container -> GetParent();
    return m_Container;
}


void wxHtmlWinParser::SetFontSize(int s)
{
    if (s < 1) s = 1;
    else if (s > 7) s = 7;
    m_FontSize = s;
}



wxFont* wxHtmlWinParser::CreateCurrentFont()
{
    int fb = GetFontBold(),
        fi = GetFontItalic(),
        fu = GetFontUnderlined(),
        ff = GetFontFixed(),
        fs = GetFontSize() - 1 /*remap from <1;7> to <0;6>*/ ;

    wxString face = ff ? m_FontFaceFixed : m_FontFaceNormal;
    wxString *faceptr = &(m_FontsFacesTable[fb][fi][fu][ff][fs]);
    wxFont **fontptr = &(m_FontsTable[fb][fi][fu][ff][fs]);
    wxFontEncoding *encptr = &(m_FontsEncTable[fb][fi][fu][ff][fs]);

    if (*fontptr != NULL && (*faceptr != face || *encptr != m_OutputEnc)) {
        delete *fontptr;
        *fontptr = NULL;
    }

    if (*fontptr == NULL) {
        *faceptr = face;
        *encptr = m_OutputEnc;
        *fontptr = new wxFont(
                       m_FontsSizes[fs] * m_PixelScale,
                       ff ? wxMODERN : wxSWISS,
                       fi ? wxITALIC : wxNORMAL,
                       fb ? wxBOLD : wxNORMAL,
                       fu ? TRUE : FALSE, face,
                       m_OutputEnc);
    }
    m_DC -> SetFont(**fontptr);
    return (*fontptr);
}



void wxHtmlWinParser::SetLink(const wxHtmlLinkInfo& link)
{
    m_Link = link; 
    m_UseLink = (link.GetHref() != wxEmptyString);
}


void wxHtmlWinParser::SetFontFace(const wxString& face) 
{
    if (GetFontFixed()) m_FontFaceFixed = face; 
    else m_FontFaceNormal = face;

    if (m_InputEnc != wxFONTENCODING_DEFAULT)
        SetInputEncoding(m_InputEnc);
}



void wxHtmlWinParser::SetInputEncoding(wxFontEncoding enc)
{
    m_InputEnc = m_OutputEnc = wxFONTENCODING_DEFAULT;
    if (m_EncConv) {delete m_EncConv; m_EncConv = NULL;}

    if (enc == wxFONTENCODING_DEFAULT) return;

    wxFontEncoding altfix, altnorm;
    bool availfix, availnorm;
    
    // exact match?    
    availnorm = wxTheFontMapper -> IsEncodingAvailable(enc, m_FontFaceNormal);
    availfix = wxTheFontMapper -> IsEncodingAvailable(enc, m_FontFaceFixed);
    if (availnorm && availfix) 
        m_OutputEnc = enc;
    
    // alternatives?
    else if (wxTheFontMapper -> GetAltForEncoding(enc, &altnorm, m_FontFaceNormal, FALSE) &&
             wxTheFontMapper -> GetAltForEncoding(enc, &altfix, m_FontFaceFixed, FALSE) &&
             altnorm == altfix)
        m_OutputEnc = altnorm;
    
    // at least normal face?
    else if (availnorm)
        m_OutputEnc = enc;
    else if (wxTheFontMapper -> GetAltForEncoding(enc, &altnorm, m_FontFaceNormal, FALSE))
        m_OutputEnc = altnorm;
        
    // okay, let convert to ISO_8859-1, available always
    else
        m_OutputEnc = wxFONTENCODING_DEFAULT;
        
    m_InputEnc = enc;
    
    if (m_InputEnc == m_OutputEnc) return;

    m_EncConv = new wxEncodingConverter();
    if (!m_EncConv -> Init(m_InputEnc, 
                           (m_OutputEnc == wxFONTENCODING_DEFAULT) ?
                                      wxFONTENCODING_ISO8859_1 : m_OutputEnc,
                           wxCONVERT_SUBSTITUTE))  
    { // total failture :-(
        wxLogError(_("Failed to display HTML document in %s encoding"), 
	           wxFontMapper::GetEncodingName(enc).mb_str());
        m_InputEnc = m_OutputEnc = wxFONTENCODING_DEFAULT;
        delete m_EncConv;
        m_EncConv = NULL;
    }
}





//-----------------------------------------------------------------------------
// wxHtmlWinTagHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlWinTagHandler, wxHtmlTagHandler)



//-----------------------------------------------------------------------------
// wxHtmlTagsModule
//-----------------------------------------------------------------------------


IMPLEMENT_DYNAMIC_CLASS(wxHtmlTagsModule, wxModule)


bool wxHtmlTagsModule::OnInit()
{
    wxHtmlWinParser::AddModule(this);
    return TRUE;
}



void wxHtmlTagsModule::OnExit()
{
}
#endif

