/////////////////////////////////////////////////////////////////////////////
// Name:        htmprint.cpp
// Purpose:     html printing classes
// Author:      Vaclav Slavik
// Created:     25/09/99
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik, 1999
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if wxUSE_PRINTING_ARCHITECTURE

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/html/htmprint.h"
#include "wx/wxhtml.h"
#include "wx/wfstream.h"


//--------------------------------------------------------------------------------
// wxHtmlDCRenderer
//--------------------------------------------------------------------------------


wxHtmlDCRenderer::wxHtmlDCRenderer() : wxObject()
{
    m_DC = NULL;
    m_Width = m_Height = 0;
    m_Cells = NULL;
    m_Parser = new wxHtmlWinParser(NULL);
    m_FS = new wxFileSystem();
    m_Parser -> SetFS(m_FS);
    m_Scale = 1.0;
}



wxHtmlDCRenderer::~wxHtmlDCRenderer()
{
    if (m_Cells) delete m_Cells;
    if (m_Parser) delete m_Parser;
    if (m_FS) delete m_FS;
}



void wxHtmlDCRenderer::SetDC(wxDC *dc, int maxwidth)
{
    int dx, dy;

    wxDisplaySize(&dx, &dy);
    m_MaxWidth = maxwidth;
#if 0
    m_Scale = (float)dx * 2 / 3 / (float)maxwidth;
            // let the width of A4 is approximately 2/3 the screen width
#endif
    m_Scale = (float)800 / (float)maxwidth;
            // for now, assume screen width = 800 => good results

    m_DC = dc;
    m_Parser -> SetDC(dc);
}



void wxHtmlDCRenderer::SetSize(int width, int height)
{
    m_Width = (int)(width * m_Scale);
    m_Height = (int)(height * m_Scale);
}



void wxHtmlDCRenderer::SetHtmlText(const wxString& html, const wxString& basepath, bool isdir)
{
    if (m_DC == NULL) return;

    if (m_Cells != NULL) delete m_Cells;
    
    m_FS -> ChangePathTo(basepath, isdir);
    m_DC -> SetUserScale(1.0, 1.0);
    m_Cells = (wxHtmlContainerCell*) m_Parser -> Parse(html);
    m_Cells -> SetIndent(0, wxHTML_INDENT_ALL, wxHTML_UNITS_PIXELS);
    m_Cells -> Layout(m_Width);
}



int wxHtmlDCRenderer::Render(int x, int y, int from, int dont_render)
{
    int pbreak;
    
    if (m_Cells == NULL || m_DC == NULL) return 0;
    
    pbreak = (int)(from * m_Scale + m_Height);
    while (m_Cells -> AdjustPagebreak(&pbreak)) {}
    
    if (!dont_render) {
        int w, h;
        m_DC -> GetSize(&w, &h);
        float overallScale = (float)(w/(float)m_MaxWidth) / m_Scale;
        m_DC -> SetUserScale(overallScale, overallScale);

        m_DC -> SetBrush(*wxWHITE_BRUSH);
        
        m_DC -> SetClippingRegion(x * m_Scale, y * m_Scale, m_Width, m_Height);
        m_Cells -> Draw(*m_DC, 
                        x * m_Scale, (y - from) * m_Scale, 
                        y * m_Scale, pbreak + (y - from) * m_Scale);
        m_DC -> DestroyClippingRegion();
    }
    
    if (pbreak < m_Cells -> GetHeight()) return (int)(pbreak / m_Scale);
    else return GetTotalHeight();
}



int wxHtmlDCRenderer::GetTotalHeight()
{
    if (m_Cells) return (int)(m_Cells -> GetHeight() / m_Scale);
    else return 0;
}
















//--------------------------------------------------------------------------------
// wxHtmlPrintout
//--------------------------------------------------------------------------------



wxHtmlPrintout::wxHtmlPrintout(const wxString& title) : wxPrintout(title)
{
    m_Renderer = new wxHtmlDCRenderer;
    m_RendererHdr = new wxHtmlDCRenderer;
    m_NumPages = wxHTML_PRINT_MAX_PAGES;
    m_Document = m_BasePath = wxEmptyString; m_BasePathIsDir = TRUE;
    m_Headers[0] = m_Headers[1] = wxEmptyString;
    m_Footers[0] = m_Footers[1] = wxEmptyString;
    m_HeaderHeight = m_FooterHeight = 0;
    SetMargins(); // to default values
}



wxHtmlPrintout::~wxHtmlPrintout()
{
    delete m_Renderer;
    delete m_RendererHdr;
}



void wxHtmlPrintout::OnBeginPrinting()
{
    int pageWidth, pageHeight, mm_w, mm_h;
    float ppmm_h, ppmm_v;
    
    wxPrintout::OnBeginPrinting();

    GetPageSizePixels(&pageWidth, &pageHeight);
    GetPageSizeMM(&mm_w, &mm_h);
    ppmm_h = (float)pageWidth / mm_w;
    ppmm_v = (float)pageHeight / mm_h;

    /* prepare headers/footers renderer: */
    
    m_RendererHdr -> SetDC(GetDC(), pageWidth);
    m_RendererHdr -> SetSize(ppmm_h * (mm_w - m_MarginLeft - m_MarginTop), 
                          ppmm_v * (mm_h - m_MarginTop - m_MarginBottom));
    if (m_Headers[0] != wxEmptyString) {
        m_RendererHdr -> SetHtmlText(TranslateHeader(m_Headers[0], 1));
        m_HeaderHeight = m_RendererHdr -> GetTotalHeight();
    }
    else if (m_Headers[1] != wxEmptyString) {
        m_RendererHdr -> SetHtmlText(TranslateHeader(m_Headers[1], 1));
        m_HeaderHeight = m_RendererHdr -> GetTotalHeight();
    }
    if (m_Footers[0] != wxEmptyString) {
        m_RendererHdr -> SetHtmlText(TranslateHeader(m_Footers[0], 1));
        m_FooterHeight = m_RendererHdr -> GetTotalHeight();
    }
    else if (m_Footers[1] != wxEmptyString) {
        m_RendererHdr -> SetHtmlText(TranslateHeader(m_Footers[1], 1));
        m_FooterHeight = m_RendererHdr -> GetTotalHeight();
    }
    
    /* prepare main renderer: */
    m_Renderer -> SetDC(GetDC(), pageWidth);
    m_Renderer -> SetSize(ppmm_h * (mm_w - m_MarginLeft - m_MarginTop), 
                          ppmm_v * (mm_h - m_MarginTop - m_MarginBottom) - 
                          m_FooterHeight - m_HeaderHeight -
                          ((m_HeaderHeight == 0) ? 0 : m_MarginSpace * ppmm_v) -
                          ((m_FooterHeight == 0) ? 0 : m_MarginSpace * ppmm_v)
                          );
    m_Renderer -> SetHtmlText(m_Document, m_BasePath, m_BasePathIsDir);
    CountPages();
}


bool wxHtmlPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc) {
        if (HasPage(page))
            RenderPage(dc, page);
        return TRUE;
    } else
        return FALSE;
}


void wxHtmlPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    *maxPage = wxHTML_PRINT_MAX_PAGES;
    *selPageFrom = 1;
    *selPageTo = wxHTML_PRINT_MAX_PAGES;
}



bool wxHtmlPrintout::HasPage(int pageNum)
{
    return (pageNum >= 1 && pageNum <= m_NumPages);
}



void wxHtmlPrintout::SetHtmlText(const wxString& html, const wxString &basepath, bool isdir)
{
    m_Document = html;
    m_BasePath = basepath;
    m_BasePathIsDir = isdir;
}



void wxHtmlPrintout::SetHtmlFile(const wxString& htmlfile)
{
    wxFileSystem fs;
    wxFSFile *ff = fs.OpenFile(htmlfile);
    wxInputStream *st = ff -> GetStream();
    char *t = new char[st -> GetSize() + 1];
    st -> Read(t, st -> GetSize());
    t[st -> GetSize()] = 0;
    
    wxString doc = wxString(t);
    delete t;
    delete ff;
    
    SetHtmlText(doc, htmlfile, FALSE);
}



void wxHtmlPrintout::SetHeader(const wxString& header, int pg)
{
    if (pg == wxPAGE_ALL || pg == wxPAGE_EVEN) 
        m_Headers[0] = header;
    if (pg == wxPAGE_ALL || pg == wxPAGE_ODD) 
        m_Headers[1] = header;
}



void wxHtmlPrintout::SetFooter(const wxString& footer, int pg)
{
    if (pg == wxPAGE_ALL || pg == wxPAGE_EVEN) 
        m_Footers[0] = footer;
    if (pg == wxPAGE_ALL || pg == wxPAGE_ODD) 
        m_Footers[1] = footer;
}



void wxHtmlPrintout::CountPages()
{
    wxBusyCursor wait;
    int pageWidth, pageHeight, mm_w, mm_h;
    float ppmm_h, ppmm_v;

    GetPageSizePixels(&pageWidth, &pageHeight);
    GetPageSizeMM(&mm_w, &mm_h);
    ppmm_h = (float)pageWidth / mm_w;
    ppmm_v = (float)pageHeight / mm_h;

    int pos = 0;

    m_NumPages = 0;
    
    m_PageBreaks[0] = 0;
    do {
        pos = m_Renderer -> Render(ppmm_h * m_MarginLeft, 
                                   ppmm_v * (m_MarginTop + (m_HeaderHeight == 0 ? 0 : m_MarginSpace)) + m_HeaderHeight,
                                   pos, TRUE);
        m_PageBreaks[++m_NumPages] = pos;
    } while (pos < m_Renderer -> GetTotalHeight());
}



void wxHtmlPrintout::RenderPage(wxDC *dc, int page)
{
    wxBusyCursor wait;

    int pageWidth, pageHeight, mm_w, mm_h;
    float ppmm_h, ppmm_v;

    GetPageSizePixels(&pageWidth, &pageHeight);
    GetPageSizeMM(&mm_w, &mm_h);
    ppmm_h = (float)pageWidth / mm_w;
    ppmm_v = (float)pageHeight / mm_h;
    
    m_Renderer -> SetDC(dc, pageWidth);

    dc -> SetBackgroundMode(wxTRANSPARENT);

    m_Renderer -> Render(ppmm_h * m_MarginLeft, 
                         ppmm_v * (m_MarginTop + (m_HeaderHeight == 0 ? 0 : m_MarginSpace)) + m_HeaderHeight,
                         m_PageBreaks[page-1]);
    
    m_RendererHdr -> SetDC(dc, pageWidth);
    if (m_Headers[page % 2] != wxEmptyString) {
        m_RendererHdr -> SetHtmlText(TranslateHeader(m_Headers[page % 2], page));
        m_RendererHdr -> Render(ppmm_h * m_MarginLeft, ppmm_v * m_MarginTop);
    }
    if (m_Footers[page % 2] != wxEmptyString) {
        m_RendererHdr -> SetHtmlText(TranslateHeader(m_Footers[page % 2], page));
        m_RendererHdr -> Render(ppmm_h * m_MarginLeft, pageHeight - ppmm_v * m_MarginBottom - m_FooterHeight);
    }
}



wxString wxHtmlPrintout::TranslateHeader(const wxString& instr, int page)
{
    wxString r = instr;
    wxString num;
    
    num.Printf("%i", page);
    r.Replace("@PAGENUM@", num);

    num.Printf("%i", m_NumPages);
    r.Replace("@PAGESCNT@", num);

    return r;
}



void wxHtmlPrintout::SetMargins(float top, float bottom, float left, float right, float spaces)
{
    m_MarginTop = top;
    m_MarginBottom = bottom;
    m_MarginLeft = left;
    m_MarginRight = right;
    m_MarginSpace = spaces;
}






//--------------------------------------------------------------------------------
// wxHtmlEasyPrinting
//--------------------------------------------------------------------------------


wxHtmlEasyPrinting::wxHtmlEasyPrinting(const wxString& name, wxFrame *parent_frame)
{
    m_Frame = parent_frame;
    m_Name = name;
    m_PrintData = new wxPrintData;
    m_PageSetupData = new wxPageSetupDialogData;
    m_Headers[0] = m_Headers[1] = m_Footers[0] = m_Footers[1] = wxEmptyString;
    
    m_PageSetupData -> EnableMargins(TRUE);
    m_PageSetupData -> SetMarginTopLeft(wxPoint(25, 25));    
    m_PageSetupData -> SetMarginBottomRight(wxPoint(25, 25));

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    m_PrintData -> SetPrinterCommand("lpr");
#endif
}



wxHtmlEasyPrinting::~wxHtmlEasyPrinting()
{
    delete m_PrintData;
    delete m_PageSetupData;
}



void wxHtmlEasyPrinting::PreviewFile(const wxString &htmlfile)
{
    wxHtmlPrintout *p1 = CreatePrintout();
    p1 -> SetHtmlFile(htmlfile);
    wxHtmlPrintout *p2 = CreatePrintout();
    p2 -> SetHtmlFile(htmlfile);
    DoPreview(p1, p2);
}



void wxHtmlEasyPrinting::PreviewText(const wxString &htmltext, const wxString &basepath)
{
    wxHtmlPrintout *p1 = CreatePrintout();
    p1 -> SetHtmlText(htmltext, basepath, TRUE);
    wxHtmlPrintout *p2 = CreatePrintout();
    p2 -> SetHtmlText(htmltext, basepath, TRUE);
    DoPreview(p1, p2);
}



void wxHtmlEasyPrinting::PrintFile(const wxString &htmlfile)
{
    wxHtmlPrintout *p = CreatePrintout();
    p -> SetHtmlFile(htmlfile);
    DoPrint(p);
}



void wxHtmlEasyPrinting::PrintText(const wxString &htmltext, const wxString &basepath)
{
    wxHtmlPrintout *p = CreatePrintout();
    p -> SetHtmlText(htmltext, basepath, TRUE);
    DoPrint(p);
}



void wxHtmlEasyPrinting::DoPreview(wxHtmlPrintout *printout1, wxHtmlPrintout *printout2)
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(*m_PrintData);
    wxPrintPreview *preview = new wxPrintPreview(printout1, printout2, &printDialogData);
    if (!preview -> Ok()) {
        delete preview;
        wxMessageBox(_("There was a problem previewing.\nPerhaps your current printer is not set correctly?"), _("Previewing"), wxOK);
    }
    
    else {
        wxPreviewFrame *frame = new wxPreviewFrame(preview, m_Frame, 
                                                   m_Name + _(" Preview"), 
                                                   wxPoint(100, 100), wxSize(650, 500));
        frame -> Centre(wxBOTH);
        frame -> Initialize();
        frame -> Show(TRUE);
    }
}



void wxHtmlEasyPrinting::DoPrint(wxHtmlPrintout *printout)
{
    wxPrintDialogData printDialogData(*m_PrintData);
    wxPrinter printer(&printDialogData);

    if (!printer.Print(m_Frame, printout, TRUE))
        wxMessageBox(_("There was a problem printing.\nPerhaps your current printer is not set correctly?"), _("Printing"), wxOK);
    else
        (*m_PrintData) = printer.GetPrintDialogData().GetPrintData();
}



void wxHtmlEasyPrinting::PrinterSetup()
{
    wxPrintDialogData printDialogData(*m_PrintData);
    wxPrintDialog printerDialog(m_Frame, &printDialogData);
    
    printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);

    if (printerDialog.ShowModal() == wxID_OK)
        (*m_PrintData) = printerDialog.GetPrintDialogData().GetPrintData();
}



void wxHtmlEasyPrinting::PageSetup()
{
    m_PageSetupData -> SetPrintData(*m_PrintData);
    wxPageSetupDialog pageSetupDialog(m_Frame, m_PageSetupData);

    if (pageSetupDialog.ShowModal() == wxID_OK) {
        (*m_PrintData) = pageSetupDialog.GetPageSetupData().GetPrintData();
        (*m_PageSetupData) = pageSetupDialog.GetPageSetupData();
    }
}



void wxHtmlEasyPrinting::SetHeader(const wxString& header, int pg)
{
    if (pg == wxPAGE_ALL || pg == wxPAGE_EVEN) 
        m_Headers[0] = header;
    if (pg == wxPAGE_ALL || pg == wxPAGE_ODD) 
        m_Headers[1] = header;
}



void wxHtmlEasyPrinting::SetFooter(const wxString& footer, int pg)
{
    if (pg == wxPAGE_ALL || pg == wxPAGE_EVEN) 
        m_Footers[0] = footer;
    if (pg == wxPAGE_ALL || pg == wxPAGE_ODD) 
        m_Footers[1] = footer;
}



wxHtmlPrintout *wxHtmlEasyPrinting::CreatePrintout()
{
    wxHtmlPrintout *p = new wxHtmlPrintout(m_Name);
    
    p -> SetHeader(m_Headers[0], wxPAGE_EVEN);
    p -> SetHeader(m_Headers[1], wxPAGE_ODD);
    p -> SetFooter(m_Footers[0], wxPAGE_EVEN);
    p -> SetFooter(m_Footers[1], wxPAGE_ODD);

    p -> SetMargins(m_PageSetupData -> GetMarginTopLeft().y,
                    m_PageSetupData -> GetMarginBottomRight().y,
                    m_PageSetupData -> GetMarginTopLeft().x,
                    m_PageSetupData -> GetMarginBottomRight().x);
    
    return p;
}



#endif // wxUSE_PRINTING_ARCHITECTURE
