/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/htmprint.cpp
// Purpose:     html printing classes
// Author:      Vaclav Slavik
// Created:     25/09/99
// Copyright:   (c) Vaclav Slavik, 1999
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_HTML && wxUSE_PRINTING_ARCHITECTURE && wxUSE_STREAMS

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/module.h"
    #include "wx/sizer.h"
#endif

#include "wx/print.h"
#include "wx/printdlg.h"
#include "wx/html/htmprint.h"
#include "wx/wxhtml.h"
#include "wx/wfstream.h"
#include "wx/infobar.h"


// default font size of normal text (HTML font size 0) for printing, in points:
#define DEFAULT_PRINT_FONT_SIZE   12


// CSS specification offer following guidance on dealing with pixel sizes
// when printing at
// http://www.w3.org/TR/2004/CR-CSS21-20040225/syndata.html#length-units:
//
//      Pixel units are relative to the resolution of the viewing device, i.e.,
//      most often a computer display. If the pixel density of the output
//      device is very different from that of a typical computer display, the
//      user agent should rescale pixel values. It is recommended that the [
//      reference pixel] be the visual angle of one pixel on a device with a
//      pixel density of 96dpi and a distance from the reader of an arm's
//      length. For a nominal arm's length of 28 inches, the visual angle is
//      therefore about 0.0213 degrees.
//
//      For reading at arm's length, 1px thus corresponds to about 0.26 mm
//      (1/96 inch). When printed on a laser printer, meant for reading at a
//      little less than arm's length (55 cm, 21 inches), 1px is about 0.20 mm.
//      On a 300 dots-per-inch (dpi) printer, that may be rounded up to 3 dots
//      (0.25 mm); on a 600 dpi printer, it can be rounded to 5 dots.
//
// See also https://github.com/wxWidgets/wxWidgets/issues/10942.
#define TYPICAL_SCREEN_DPI  96.0

//--------------------------------------------------------------------------------
// wxHtmlDCRenderer
//--------------------------------------------------------------------------------


wxHtmlDCRenderer::wxHtmlDCRenderer() : wxObject()
{
    m_DC = NULL;
    m_Width = m_Height = 0;
    m_Cells = NULL;
    m_ownsCells = false;
    m_Parser.SetFS(&m_FS);
    SetStandardFonts(DEFAULT_PRINT_FONT_SIZE);
}



wxHtmlDCRenderer::~wxHtmlDCRenderer()
{
    if ( m_ownsCells )
        delete m_Cells;
}



void wxHtmlDCRenderer::SetDC(wxDC *dc, double pixel_scale, double font_scale)
{
    m_DC = dc;
    m_Parser.SetDC(m_DC, pixel_scale, font_scale);
}



void wxHtmlDCRenderer::SetSize(int width, int height)
{
    wxCHECK_RET( width, "width must be non-zero" );
    wxCHECK_RET( height, "height must be non-zero" );

    m_Width = width;
    m_Height = height;
}


void wxHtmlDCRenderer::SetHtmlText(const wxString& html, const wxString& basepath, bool isdir)
{
    wxCHECK_RET( m_DC, "SetDC() must be called before SetHtmlText()" );
    wxCHECK_RET( m_Width, "SetSize() must be called before SetHtmlText()" );

    m_FS.ChangePathTo(basepath, isdir);

    wxHtmlContainerCell* const cell = (wxHtmlContainerCell*) m_Parser.Parse(html);
    wxCHECK_RET( cell, "Failed to parse HTML" );

    DoSetHtmlCell(cell);

    m_ownsCells = true;
}

void wxHtmlDCRenderer::DoSetHtmlCell(wxHtmlContainerCell* cell)
{
    if ( m_ownsCells )
        delete m_Cells;

    m_Cells = cell;
    m_Cells->SetIndent(0, wxHTML_INDENT_ALL, wxHTML_UNITS_PIXELS);
    m_Cells->Layout(m_Width);
}

void wxHtmlDCRenderer::SetHtmlCell(wxHtmlContainerCell& cell)
{
    DoSetHtmlCell(&cell);

    m_ownsCells = false;
}


void wxHtmlDCRenderer::SetFonts(const wxString& normal_face, const wxString& fixed_face,
                                const int *sizes)
{
    m_Parser.SetFonts(normal_face, fixed_face, sizes);

    if ( m_Cells )
        m_Cells->Layout(m_Width);
    // else: SetHtmlText() not yet called, no need for relayout
}

void wxHtmlDCRenderer::SetStandardFonts(int size,
                                        const wxString& normal_face,
                                        const wxString& fixed_face)
{
    m_Parser.SetStandardFonts(size, normal_face, fixed_face);

    if ( m_Cells )
        m_Cells->Layout(m_Width);
    // else: SetHtmlText() not yet called, no need for relayout
}

int wxHtmlDCRenderer::FindNextPageBreak(int pos) const
{
    // Stop looking for page breaks if the previous one was already at the end
    // of the last page.
    //
    // For an empty HTML document total height is 0, but we still must have at
    // least a single page in it, so handle the case of pos == 0 specially.
    if ( pos != 0 && pos >= GetTotalHeight() )
        return wxNOT_FOUND;

    int posNext = pos + m_Height;
    if ( m_Cells->AdjustPagebreak(&posNext, m_Height) )
    {
        // Check that AdjustPagebreak() returns the page break at a strictly
        // greater position than that of the previous page, otherwise
        // CountPages() would enter into an infinite loop.
        wxCHECK_MSG( posNext > pos, wxNOT_FOUND, "Bug in AdjustPagebreak()" );
    }

    return posNext;
}

void wxHtmlDCRenderer::Render(int x, int y, int from, int to)
{
    wxCHECK_RET( m_DC, "SetDC() must be called before Render()" );

    const int hght = to == INT_MAX ? m_Height : to - from;

    wxHtmlRenderingInfo rinfo;
    wxDefaultHtmlRenderingStyle rstyle;
    rinfo.SetStyle(&rstyle);
    m_DC->SetBrush(*wxWHITE_BRUSH);
    wxDCClipper clip(*m_DC, x, y, m_Width, hght);
    m_Cells->Draw(*m_DC,
                  x, (y - from),
                  y, y + hght,
                  rinfo);
}

int wxHtmlDCRenderer::GetTotalWidth() const
{
    return m_Cells ? m_Cells->GetWidth() : 0;
}

int wxHtmlDCRenderer::GetTotalHeight() const
{
    return m_Cells ? m_Cells->GetHeight() : 0;
}


//--------------------------------------------------------------------------------
// wxHtmlPrintout
//--------------------------------------------------------------------------------


wxVector<wxHtmlFilter*> wxHtmlPrintout::m_Filters;

wxHtmlPrintout::wxHtmlPrintout(const wxString& title) : wxPrintout(title)
{
    m_BasePathIsDir = true;
    m_HeaderHeight = m_FooterHeight = 0;
    SetMargins(); // to default values
    SetStandardFonts(DEFAULT_PRINT_FONT_SIZE);
}



void wxHtmlPrintout::CleanUpStatics()
{
    for ( size_t n = 0; n < m_Filters.size(); ++n )
        delete m_Filters[n];

    m_Filters.clear();
}

// Adds input filter
void wxHtmlPrintout::AddFilter(wxHtmlFilter *filter)
{
    m_Filters.push_back(filter);
}

bool
wxHtmlPrintout::CheckFit(const wxSize& pageArea, const wxSize& docArea) const
{
    // Nothing to do if the contents fits horizontally.
    if ( docArea.x <= pageArea.x )
        return true;

    // Otherwise warn the user more or less intrusively depending on whether
    // we're previewing or printing:
    if ( wxPrintPreview * const preview = GetPreview() )
    {
        // Don't annoy the user too much when previewing by using info bar
        // instead of a dialog box.
#if wxUSE_INFOBAR
        wxFrame * const parent = preview->GetFrame();
        wxCHECK_MSG( parent, false, "No parent preview frame?" );

        wxSizer * const sizer = parent->GetSizer();
        wxCHECK_MSG( sizer, false, "Preview frame should be using sizers" );

        wxInfoBar * const bar = new wxInfoBar(parent);
        sizer->Add(bar, wxSizerFlags().Expand());

        // Note that the message here is similar to the one below but not
        // exactly the same, notably we don't use the document title here
        // because it's already clear which document it pertains to and the
        // title may be long enough to make the text not fit in the window.
        bar->ShowMessage
             (
              _("This document doesn't fit on the page horizontally and "
                "will be truncated when it is printed."),
              wxICON_WARNING
             );
#endif // wxUSE_INFOBAR
    }
    else // We're going to really print and not just preview.
    {
        // This is our last chance to warn the user that the output will be
        // mangled so do show a message box.
        wxMessageDialog
            dlg
            (
                NULL,
                wxString::Format
                (
                 _("The document \"%s\" doesn't fit on the page "
                   "horizontally and will be truncated if printed.\n"
                   "\n"
                   "Would you like to proceed with printing it nevertheless?"),
                 GetTitle()
                ),
                _("Printing"),
                wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_QUESTION
            );
        dlg.SetExtendedMessage
            (
                _("If possible, try changing the layout parameters to "
                  "make the printout more narrow.")
            );
        dlg.SetOKLabel(wxID_PRINT);

        if ( dlg.ShowModal() == wxID_CANCEL )
            return false;
    }

    return true;
}

void wxHtmlPrintout::OnPreparePrinting()
{
    int pageWidth, pageHeight, mm_w, mm_h, dc_w, dc_h;
    float ppmm_h, ppmm_v;

    GetPageSizePixels(&pageWidth, &pageHeight);
    GetPageSizeMM(&mm_w, &mm_h);
    ppmm_h = (float)pageWidth / mm_w;
    ppmm_v = (float)pageHeight / mm_h;

    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    wxUnusedVar(ppiPrinterX);
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    wxUnusedVar(ppiScreenX);

    GetDC()->GetSize(&dc_w, &dc_h);

    GetDC()->SetUserScale((double)dc_w / (double)pageWidth,
                          (double)dc_h / (double)pageHeight);

    /* prepare headers/footers renderer: */

    m_RendererHdr.SetDC(GetDC(),
                         (double)ppiPrinterY / TYPICAL_SCREEN_DPI,
                         (double)ppiPrinterY / (double)ppiScreenY);
    m_RendererHdr.SetSize((int) (ppmm_h * (mm_w - m_MarginLeft - m_MarginRight)),
                          (int) (ppmm_v * (mm_h - m_MarginTop - m_MarginBottom)));
    if (!m_Headers[0].empty())
    {
        m_RendererHdr.SetHtmlText(TranslateHeader(m_Headers[0], 1));
        m_HeaderHeight = m_RendererHdr.GetTotalHeight();
    }
    else if (!m_Headers[1].empty())
    {
        m_RendererHdr.SetHtmlText(TranslateHeader(m_Headers[1], 1));
        m_HeaderHeight = m_RendererHdr.GetTotalHeight();
    }
    if (!m_Footers[0].empty())
    {
        m_RendererHdr.SetHtmlText(TranslateHeader(m_Footers[0], 1));
        m_FooterHeight = m_RendererHdr.GetTotalHeight();
    }
    else if (!m_Footers[1].empty())
    {
        m_RendererHdr.SetHtmlText(TranslateHeader(m_Footers[1], 1));
        m_FooterHeight = m_RendererHdr.GetTotalHeight();
    }

    /* prepare main renderer: */
    m_Renderer.SetDC(GetDC(),
                      (double)ppiPrinterY / TYPICAL_SCREEN_DPI,
                      (double)ppiPrinterY / (double)ppiScreenY);

    const int printAreaW = int(ppmm_h * (mm_w - m_MarginLeft - m_MarginRight));
    int printAreaH = int(ppmm_v * (mm_h - m_MarginTop - m_MarginBottom));
    if ( m_HeaderHeight )
        printAreaH -= int(m_HeaderHeight + m_MarginSpace * ppmm_v);
    if ( m_FooterHeight )
        printAreaH -= int(m_FooterHeight + m_MarginSpace * ppmm_v);

    m_Renderer.SetSize(printAreaW, printAreaH);
    m_Renderer.SetHtmlText(m_Document, m_BasePath, m_BasePathIsDir);

    if ( CheckFit(wxSize(printAreaW, printAreaH),
                  wxSize(m_Renderer.GetTotalWidth(),
                         m_Renderer.GetTotalHeight())) || IsPreview() )
    {
        // do paginate the document
        CountPages();
    }
    //else: if we don't call CountPages() m_PageBreaks remains empty and our
    //      GetPageInfo() will return 0 as max page and so nothing will be
    //      printed
}

bool wxHtmlPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage)) return false;

    return true;
}


bool wxHtmlPrintout::OnPrintPage(int page)
{
    wxDC *dc = GetDC();
    if (dc && dc->IsOk())
    {
        if (HasPage(page))
            RenderPage(dc, page);
        return true;
    }
    else return false;
}


void wxHtmlPrintout::GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo)
{
    *minPage = 1;
    if ( m_PageBreaks.empty() )
        *maxPage = INT_MAX;
    else
        *maxPage = (signed)m_PageBreaks.size()-1;
    *selPageFrom = 1;
    *selPageTo = (signed)m_PageBreaks.size()-1;
}



bool wxHtmlPrintout::HasPage(int pageNum)
{
    return pageNum > 0 && (unsigned)pageNum < m_PageBreaks.size();
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
    wxFSFile *ff;

    if (wxFileExists(htmlfile))
        ff = fs.OpenFile(wxFileSystem::FileNameToURL(htmlfile));
    else
        ff = fs.OpenFile(htmlfile);

    if (ff == NULL)
    {
        wxLogError(htmlfile + _(": file does not exist!"));
        return;
    }

    bool done = false;
    wxHtmlFilterHTML defaultFilter;
    wxString doc;

    for ( size_t n = 0; n < m_Filters.size(); ++n )
    {
        wxHtmlFilter* const h = m_Filters[n];
        if (h->CanRead(*ff))
        {
            doc = h->ReadFile(*ff);
            done = true;
            break;
        }
    }

    if (!done)
        doc = defaultFilter.ReadFile(*ff);

    SetHtmlText(doc, htmlfile, false);
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

    m_PageBreaks.clear();

    for ( int pos = 0; pos != wxNOT_FOUND; )
    {
        m_PageBreaks.push_back(pos);
        pos = m_Renderer.FindNextPageBreak(pos);
    }
}



void wxHtmlPrintout::RenderPage(wxDC *dc, int page)
{
    wxBusyCursor wait;

    int pageWidth, pageHeight, mm_w, mm_h, dc_w, dc_h;
    float ppmm_h, ppmm_v;

    GetPageSizePixels(&pageWidth, &pageHeight);
    GetPageSizeMM(&mm_w, &mm_h);
    ppmm_h = (float)pageWidth / mm_w;
    ppmm_v = (float)pageHeight / mm_h;
    dc->GetSize(&dc_w, &dc_h);

    int ppiPrinterX, ppiPrinterY;
    GetPPIPrinter(&ppiPrinterX, &ppiPrinterY);
    wxUnusedVar(ppiPrinterX);
    int ppiScreenX, ppiScreenY;
    GetPPIScreen(&ppiScreenX, &ppiScreenY);
    wxUnusedVar(ppiScreenX);

    dc->SetUserScale((double)dc_w / (double)pageWidth,
                     (double)dc_h / (double)pageHeight);

    m_Renderer.SetDC(dc,
                      (double)ppiPrinterY / TYPICAL_SCREEN_DPI,
                      (double)ppiPrinterY / (double)ppiScreenY);

    dc->SetBackgroundMode(wxBRUSHSTYLE_TRANSPARENT);

    m_Renderer.Render((int) (ppmm_h * m_MarginLeft),
                         (int) (ppmm_v * (m_MarginTop + (m_HeaderHeight == 0 ? 0 : m_MarginSpace)) + m_HeaderHeight),
                         m_PageBreaks[page-1], m_PageBreaks[page]);


    m_RendererHdr.SetDC(dc,
                         (double)ppiPrinterY / TYPICAL_SCREEN_DPI,
                         (double)ppiPrinterY / (double)ppiScreenY);
    if (!m_Headers[page % 2].empty())
    {
        m_RendererHdr.SetHtmlText(TranslateHeader(m_Headers[page % 2], page));
        m_RendererHdr.Render((int) (ppmm_h * m_MarginLeft), (int) (ppmm_v * m_MarginTop));
    }
    if (!m_Footers[page % 2].empty())
    {
        m_RendererHdr.SetHtmlText(TranslateHeader(m_Footers[page % 2], page));
        m_RendererHdr.Render((int) (ppmm_h * m_MarginLeft), (int) (pageHeight - ppmm_v * m_MarginBottom - m_FooterHeight));
    }
}



wxString wxHtmlPrintout::TranslateHeader(const wxString& instr, int page)
{
    wxString r = instr;
    wxString num;

    num.Printf(wxT("%i"), page);
    r.Replace(wxT("@PAGENUM@"), num);

    num.Printf(wxT("%lu"), (unsigned long)(m_PageBreaks.size() - 1));
    r.Replace(wxT("@PAGESCNT@"), num);

#if wxUSE_DATETIME
    const wxDateTime now = wxDateTime::Now();
    r.Replace(wxT("@DATE@"), now.FormatDate());
    r.Replace(wxT("@TIME@"), now.FormatTime());
#else
    r.Replace(wxT("@DATE@"), wxEmptyString);
    r.Replace(wxT("@TIME@"), wxEmptyString);
#endif

    r.Replace(wxT("@TITLE@"), GetTitle());

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

void wxHtmlPrintout::SetMargins(const wxPageSetupDialogData& pageSetupData)
{
    SetMargins(pageSetupData.GetMarginTopLeft().y,
               pageSetupData.GetMarginBottomRight().y,
               pageSetupData.GetMarginTopLeft().x,
               pageSetupData.GetMarginBottomRight().x);
}

void wxHtmlPrintout::SetFonts(const wxString& normal_face, const wxString& fixed_face,
                              const int *sizes)
{
    m_Renderer.SetFonts(normal_face, fixed_face, sizes);
    m_RendererHdr.SetFonts(normal_face, fixed_face, sizes);
}

void wxHtmlPrintout::SetStandardFonts(int size,
                                      const wxString& normal_face,
                                      const wxString& fixed_face)
{
    m_Renderer.SetStandardFonts(size, normal_face, fixed_face);
    m_RendererHdr.SetStandardFonts(size, normal_face, fixed_face);
}



//----------------------------------------------------------------------------
// wxHtmlEasyPrinting
//----------------------------------------------------------------------------


wxHtmlEasyPrinting::wxHtmlEasyPrinting(const wxString& name, wxWindow *parentWindow)
{
    m_ParentWindow = parentWindow;
    m_Name = name;
    m_PrintData = NULL;
    m_PageSetupData = new wxPageSetupDialogData;

    m_PageSetupData->EnableMargins(true);
    m_PageSetupData->SetMarginTopLeft(wxPoint(25, 25));
    m_PageSetupData->SetMarginBottomRight(wxPoint(25, 25));

    SetStandardFonts(DEFAULT_PRINT_FONT_SIZE);

    m_promptMode = Prompt_Always;
}



wxHtmlEasyPrinting::~wxHtmlEasyPrinting()
{
    delete m_PrintData;
    delete m_PageSetupData;
}


wxPrintData *wxHtmlEasyPrinting::GetPrintData()
{
    if (m_PrintData == NULL)
        m_PrintData = new wxPrintData();
    return m_PrintData;
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
    p1->SetHtmlText(htmltext, basepath, true);
    wxHtmlPrintout *p2 = CreatePrintout();
    p2->SetHtmlText(htmltext, basepath, true);
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
    p->SetHtmlText(htmltext, basepath, true);
    bool ret = DoPrint(p);
    delete p;
    return ret;
}



bool wxHtmlEasyPrinting::DoPreview(wxHtmlPrintout *printout1, wxHtmlPrintout *printout2)
{
    // Pass two printout objects: for preview, and possible printing.
    wxPrintDialogData printDialogData(*GetPrintData());
    wxPrintPreview *preview = new wxPrintPreview(printout1, printout2, &printDialogData);
    if (!preview->IsOk())
    {
        delete preview;
        return false;
    }

    wxPreviewFrame *frame = new wxPreviewFrame(preview, m_ParentWindow,
                                               m_Name + _(" Preview"),
                                               wxPoint(100, 100), wxSize(650, 500));
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
    return true;
}



bool wxHtmlEasyPrinting::DoPrint(wxHtmlPrintout *printout)
{
    wxPrintDialogData printDialogData(*GetPrintData());
    wxPrinter printer(&printDialogData);

    const bool prompt = m_promptMode != Prompt_Never;
    if (m_promptMode == Prompt_Once)
    {
        m_promptMode = Prompt_Never;
    }

    if (!printer.Print(m_ParentWindow, printout, prompt))
    {
        return false;
    }

    (*GetPrintData()) = printer.GetPrintDialogData().GetPrintData();
    return true;
}




void wxHtmlEasyPrinting::PageSetup()
{
    if (!GetPrintData()->IsOk())
    {
        wxLogError(_("There was a problem during page setup: you may need to set a default printer."));
        return;
    }

    m_PageSetupData->SetPrintData(*GetPrintData());
    wxPageSetupDialog pageSetupDialog(m_ParentWindow, m_PageSetupData);

    if (pageSetupDialog.ShowModal() == wxID_OK)
    {
        (*GetPrintData()) = pageSetupDialog.GetPageSetupData().GetPrintData();
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


void wxHtmlEasyPrinting::SetFonts(const wxString& normal_face, const wxString& fixed_face,
                                  const int *sizes)
{
    m_fontMode = FontMode_Explicit;
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

void wxHtmlEasyPrinting::SetStandardFonts(int size,
                                          const wxString& normal_face,
                                          const wxString& fixed_face)
{
    m_fontMode = FontMode_Standard;
    m_FontFaceNormal = normal_face;
    m_FontFaceFixed = fixed_face;
    m_FontsSizesArr[0] = size;
}


wxHtmlPrintout *wxHtmlEasyPrinting::CreatePrintout()
{
    wxHtmlPrintout *p = new wxHtmlPrintout(m_Name);

    if (m_fontMode == FontMode_Explicit)
    {
        p->SetFonts(m_FontFaceNormal, m_FontFaceFixed, m_FontsSizes);
    }
    else // FontMode_Standard
    {
        p->SetStandardFonts(m_FontsSizesArr[0],
                            m_FontFaceNormal, m_FontFaceFixed);
    }

    p->SetHeader(m_Headers[0], wxPAGE_EVEN);
    p->SetHeader(m_Headers[1], wxPAGE_ODD);
    p->SetFooter(m_Footers[0], wxPAGE_EVEN);
    p->SetFooter(m_Footers[1], wxPAGE_ODD);

    p->SetMargins(*m_PageSetupData);

    return p;
}

// A module to allow initialization/cleanup
// without calling these functions from app.cpp or from
// the user's application.

class wxHtmlPrintingModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxHtmlPrintingModule);
public:
    wxHtmlPrintingModule() : wxModule() {}
    bool OnInit() wxOVERRIDE { return true; }
    void OnExit() wxOVERRIDE { wxHtmlPrintout::CleanUpStatics(); }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxHtmlPrintingModule, wxModule);


// This hack forces the linker to always link in m_* files
// (wxHTML doesn't work without handlers from these files)
#include "wx/html/forcelnk.h"
FORCE_WXHTML_MODULES()

#endif // wxUSE_HTML & wxUSE_PRINTING_ARCHITECTURE
