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
#pragma implementation "htmprint.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/dc.h"
#endif

#if wxUSE_HTML && wxUSE_PRINTING_ARCHITECTURE && wxUSE_STREAMS

#include "wx/dc.h"
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
    m_Parser->SetFS(m_FS);
}



wxHtmlDCRenderer::~wxHtmlDCRenderer()
{
    if (m_Cells) delete m_Cells;
    if (m_Parser) delete m_Parser;
    if (m_FS) delete m_FS;
}



void wxHtmlDCRenderer::SetDC(wxDC *dc, double pixel_scale)
{
    m_DC = dc;
    m_Parser->SetDC(m_DC, pixel_scale);
}



void wxHtmlDCRenderer::SetSize(int width, int height)
{
    m_Width = width;
    m_Height = height;
}


void wxHtmlDCRenderer::SetHtmlText(const wxString& html, const wxString& basepath, bool isdir)
{
    if (m_DC == NULL) return;

    if (m_Cells != NULL) delete m_Cells;

    m_FS->ChangePathTo(basepath, isdir);
    m_Cells = (wxHtmlContainerCell*) m_Parser->Parse(html);
    m_Cells->SetIndent(0, wxHTML_INDENT_ALL, wxHTML_UNITS_PIXELS);
    m_Cells->Layout(m_Width);
}


void wxHtmlDCRenderer::SetFonts(wxString normal_face, wxString fixed_face,
                                const int *sizes)
{
    m_Parser->SetFonts(normal_face, fixed_face, sizes);
    if (m_DC == NULL && m_Cells != NULL) m_Cells->Layout(m_Width);
}


int wxHtmlDCRenderer::Render(int x, int y, int from, int dont_render, int to, int *known_pagebreaks, int number_of_pages)
{
    int pbreak, hght;

    if (m_Cells == NULL || m_DC == NULL) return 0;

    pbreak = (int)(from + m_Height);
    while (m_Cells->AdjustPagebreak(&pbreak, known_pagebreaks, number_of_pages)) {}
    hght = pbreak - from;
    if(to < hght)
        hght = to;

    if (!dont_render)
    {
        wxHtmlRenderingInfo rinfo;
        wxDefaultHtmlRenderingStyle rstyle;
        rinfo.SetStyle(&rstyle);
        m_DC->SetBrush(*wxWHITE_BRUSH);
        m_DC->SetClippingRegion(x, y, m_Width, hght);
        m_Cells->Draw(*m_DC,
                      x, (y - from),
                      y, pbreak + (y /*- from*/),
                      rinfo);
        m_DC->DestroyClippingRegion();
    }

    if (pbreak < m_Cells->GetHeight()) return pbreak;
    else return GetTotalHeight();
}



int wxHtmlDCRenderer::GetTotalHeight()
{
    if (m_Cells) return m_Cells->GetHeight();
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



bool wxHtmlPrintout::OnBeginDocument(int startPage, int endPage)
{
    int pageWidth, pageHeight, mm_w, mm_h, scr_w, scr_h, dc_w, dc_h;
    float ppmm_h, ppmm_v;

    if (!wxPrintout::OnBeginDocument(startPage, endPage)) return FALSE;

    GetPageSizePixels(&pageWidth, &pageHeight);
    GetPageSizeMM(&mm_w, &mm_h);
    ppmm_h = (float)pageWidth / mm_w;
    ppmm_v = (float)pageHeight / mm_h;

    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);

    wxDisplaySize(&scr_w, &scr_h);
    GetDC()->GetSize(&dc_w, &dc_h);

    GetDC()->SetUserScale((double)dc_w / (double)pageWidth, (double)dc_w / (double)pageWidth);

    /* prepare headers/footers renderer: */

    m_RendererHdr->SetDC(GetDC(), (double)ppiPrinterY / (double)ppiScreenY);
    m_RendererHdr->SetSize((int) (ppmm_h * (mm_w - m_MarginLeft - m_MarginRight)),
                          (int) (ppmm_v * (mm_h - m_MarginTop - m_MarginBottom)));
    if (m_Headers[0] != wxEmptyString)
    {
        m_RendererHdr->SetHtmlText(TranslateHeader(m_Headers[0], 1));
        m_HeaderHeight = m_RendererHdr->GetTotalHeight();
    }
    else if (m_Headers[1] != wxEmptyString)
    {
        m_RendererHdr->SetHtmlText(TranslateHeader(m_Headers[1], 1));
        m_HeaderHeight = m_RendererHdr->GetTotalHeight();
    }
    if (m_Footers[0] != wxEmptyString)
    {
        m_RendererHdr->SetHtmlText(TranslateHeader(m_Footers[0], 1));
        m_FooterHeight = m_RendererHdr->GetTotalHeight();
    }
    else if (m_Footers[1] != wxEmptyString)
    {
        m_RendererHdr->SetHtmlText(TranslateHeader(m_Footers[1], 1));
        m_FooterHeight = m_RendererHdr->GetTotalHeight();
    }

    /* prepare main renderer: */
    m_Renderer->SetDC(GetDC(), (double)ppiPrinterY / (double)ppiScreenY);
    m_Renderer->SetSize((int) (ppmm_h * (mm_w - m_MarginLeft - m_MarginRight)),
                          (int) (ppmm_v * (mm_h - m_MarginTop - m_MarginBottom) -
                          m_FooterHeight - m_HeaderHeight -
                          ((m_HeaderHeight == 0) ? 0 : m_MarginSpace * ppmm_v) -
                          ((m_FooterHeight == 0) ? 0 : m_MarginSpace * ppmm_v)
                          ));
    m_Renderer->SetHtmlText(m_Document, m_BasePath, m_BasePathIsDir);
    CountPages();
    return TRUE;
}


bool wxHtmlPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc)
    {
        if (HasPage(page))
            RenderPage(dc, page);
        return TRUE;
    }
    else return FALSE;
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

    if (ff == NULL)
    {
        wxLogError(htmlfile + _(": file does not exist!"));
        return;
    }

    wxHtmlFilterHTML filter;
    wxString doc = filter.ReadFile(*ff);
    
    SetHtmlText(doc, htmlfile, FALSE);
    delete ff;
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
    do
    {
        pos = m_Renderer->Render((int)( ppmm_h * m_MarginLeft),
                                   (int) (ppmm_v * (m_MarginTop + (m_HeaderHeight == 0 ? 0 : m_MarginSpace)) + m_HeaderHeight),
                                   pos, TRUE, INT_MAX, m_PageBreaks, m_NumPages);
        m_PageBreaks[++m_NumPages] = pos;
    } while (pos < m_Renderer->GetTotalHeight());
}



void wxHtmlPrintout::RenderPage(wxDC *dc, int page)
{
    wxBusyCursor wait;

    int pageWidth, pageHeight, mm_w, mm_h, scr_w, scr_h, dc_w, dc_h;
    float ppmm_h, ppmm_v;

    GetPageSizePixels(&pageWidth, &pageHeight);
    GetPageSizeMM(&mm_w, &mm_h);
    ppmm_h = (float)pageWidth / mm_w;
    ppmm_v = (float)pageHeight / mm_h;
    wxDisplaySize(&scr_w, &scr_h);
    dc->GetSize(&dc_w, &dc_h);

    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);

    dc->SetUserScale((double)dc_w / (double)pageWidth, (double)dc_w / (double)pageWidth);

    m_Renderer->SetDC(dc, (double)ppiPrinterY / (double)ppiScreenY);

    dc->SetBackgroundMode(wxTRANSPARENT);

    m_Renderer->Render((int) (ppmm_h * m_MarginLeft),
                         (int) (ppmm_v * (m_MarginTop + (m_HeaderHeight == 0 ? 0 : m_MarginSpace)) + m_HeaderHeight),
                         m_PageBreaks[page-1], FALSE, m_PageBreaks[page]-m_PageBreaks[page-1]);

    m_RendererHdr->SetDC(dc, (double)ppiPrinterY / (double)ppiScreenY);
    if (m_Headers[page % 2] != wxEmptyString)
    {
        m_RendererHdr->SetHtmlText(TranslateHeader(m_Headers[page % 2], page));
        m_RendererHdr->Render((int) (ppmm_h * m_MarginLeft), (int) (ppmm_v * m_MarginTop));
    }
    if (m_Footers[page % 2] != wxEmptyString)
    {
        m_RendererHdr->SetHtmlText(TranslateHeader(m_Footers[page % 2], page));
        m_RendererHdr->Render((int) (ppmm_h * m_MarginLeft), (int) (pageHeight - ppmm_v * m_MarginBottom - m_FooterHeight));
    }
}



wxString wxHtmlPrintout::TranslateHeader(const wxString& instr, int page)
{
    wxString r = instr;
    wxString num;

    num.Printf(wxT("%i"), page);
    r.Replace(wxT("@PAGENUM@"), num);

    num.Printf(wxT("%i"), m_NumPages);
    r.Replace(wxT("@PAGESCNT@"), num);

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




void wxHtmlPrintout::SetFonts(wxString normal_face, wxString fixed_face,
                              const int *sizes)
{
    m_Renderer->SetFonts(normal_face, fixed_face, sizes);
    m_RendererHdr->SetFonts(normal_face, fixed_face, sizes);
}



//----------------------------------------------------------------------------
// wxHtmlEasyPrinting
//----------------------------------------------------------------------------


wxHtmlEasyPrinting::wxHtmlEasyPrinting(const wxString& name, wxWindow *parentWindow)
{
    m_ParentWindow = parentWindow;
    m_Name = name;
    m_PrintData = new wxPrintData;
    m_PageSetupData = new wxPageSetupDialogData;
    m_Headers[0] = m_Headers[1] = m_Footers[0] = m_Footers[1] = wxEmptyString;

    m_PageSetupData->EnableMargins(TRUE);
    m_PageSetupData->SetMarginTopLeft(wxPoint(25, 25));
    m_PageSetupData->SetMarginBottomRight(wxPoint(25, 25));

    SetFonts(wxEmptyString, wxEmptyString, NULL);
}



wxHtmlEasyPrinting::~wxHtmlEasyPrinting()
{
    delete m_PrintData;
    delete m_PageSetupData;
}



bool wxHtmlEasyPrinting::PreviewFile(const wxString &htmlfile)
{
    wxHtmlPrintout *p1 = CreatePrintout();
    p1->SetHtmlFile(htmlfile);
    wxHtmlPrintout *p2 = CreatePrintout();
    p2->SetHtmlFile(htmlfile);
    return DoPreview(p1, p2);
}



bool wxHtmlEasyPrinting::PreviewText(const wxString &htmltext, const wxString &basepath)
{
    wxHtmlPrintout *p1 = CreatePrintout();
    p1->SetHtmlText(htmltext, basepath, TRUE);
    wxHtmlPrintout *p2 = CreatePrintout();
    p2->SetHtmlText(htmltext, basepath, TRUE);
    return DoPreview(p1, p2);
}



bool wxHtmlEasyPrinting::PrintFile(const wxString &htmlfile)
{
    wxHtmlPrintout *p = CreatePrintout();
    p->SetHtmlFile(htmlfile);
    bool ret = DoPrint(p);
    delete p;
    return ret;
}



bool wxHtmlEasyPrinting::PrintText(const wxString &htmltext, const wxString &basepath)
{
    wxHtmlPrintout *p = CreatePrintout();
    p->SetHtmlText(htmltext, basepath, TRUE);
    bool ret = DoPrint(p);
    delete p;
    return ret;
}



bool wxHtmlEasyPrinting::DoPreview(wxHtmlPrintout *printout1, wxHtmlPrintout *printout2)
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(*m_PrintData);
    wxPrintPreview *preview = new wxPrintPreview(printout1, printout2, &printDialogData);
    if (!preview->Ok())
    {
        delete preview;
        return FALSE;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, m_ParentWindow,
                                               m_Name + _(" Preview"),
                                               wxPoint(100, 100), wxSize(650, 500));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(TRUE);
    return TRUE;
}



bool wxHtmlEasyPrinting::DoPrint(wxHtmlPrintout *printout)
{
    wxPrintDialogData printDialogData(*m_PrintData);
    wxPrinter printer(&printDialogData);

    if (!printer.Print(m_ParentWindow, printout, TRUE))
    {
        return FALSE;
    }

    (*m_PrintData) = printer.GetPrintDialogData().GetPrintData();
    return TRUE;
}



void wxHtmlEasyPrinting::PrinterSetup()
{
    wxPrintDialogData printDialogData(*m_PrintData);
    wxPrintDialog printerDialog(m_ParentWindow, &printDialogData);

    printerDialog.GetPrintDialogData().SetSetupDialog(TRUE);

    if (printerDialog.ShowModal() == wxID_OK)
        (*m_PrintData) = printerDialog.GetPrintDialogData().GetPrintData();
}



void wxHtmlEasyPrinting::PageSetup()
{
    if (!m_PrintData->Ok())
    {
        wxLogError(_("There was a problem during page setup: you may need to set a default printer."));
        return;
    }

    m_PageSetupData->SetPrintData(*m_PrintData);
    wxPageSetupDialog pageSetupDialog(m_ParentWindow, m_PageSetupData);

    if (pageSetupDialog.ShowModal() == wxID_OK)
    {
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


void wxHtmlEasyPrinting::SetFonts(wxString normal_face, wxString fixed_face,
                                  const int *sizes)
{
    m_FontFaceNormal = normal_face;
    m_FontFaceFixed = fixed_face;

    if (sizes)
    {
        m_FontsSizes = m_FontsSizesArr;
        for (int i = 0; i < 7; i++) m_FontsSizes[i] = sizes[i];
    }
    else
        m_FontsSizes = NULL;
}


wxHtmlPrintout *wxHtmlEasyPrinting::CreatePrintout()
{
    wxHtmlPrintout *p = new wxHtmlPrintout(m_Name);

    p->SetFonts(m_FontFaceNormal, m_FontFaceFixed, m_FontsSizes);

    p->SetHeader(m_Headers[0], wxPAGE_EVEN);
    p->SetHeader(m_Headers[1], wxPAGE_ODD);
    p->SetFooter(m_Footers[0], wxPAGE_EVEN);
    p->SetFooter(m_Footers[1], wxPAGE_ODD);

    p->SetMargins(m_PageSetupData->GetMarginTopLeft().y,
                    m_PageSetupData->GetMarginBottomRight().y,
                    m_PageSetupData->GetMarginTopLeft().x,
                    m_PageSetupData->GetMarginBottomRight().x);

    return p;
}


// This hack forces the linker to always link in m_* files
// (wxHTML doesn't work without handlers from these files)
#include "wx/html/forcelnk.h"
FORCE_WXHTML_MODULES()

#endif // wxUSE_HTML & wxUSE_PRINTING_ARCHITECTURE
