/////////////////////////////////////////////////////////////////////////////
// Name:        prntbase.cpp
// Purpose:     Printing framework base class implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "prntbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_PRINTING_ARCHITECTURE

#ifndef WX_PRECOMP
#include "wx/utils.h"
#include "wx/dc.h"
#include "wx/app.h"
#include "wx/msgdlg.h"
#include "wx/layout.h"
#include "wx/choice.h"
#include "wx/button.h"
#include "wx/settings.h"
#include "wx/dcmemory.h"
#include "wx/stattext.h"
#include "wx/intl.h"
#endif

#include "wx/prntbase.h"
#include "wx/dcprint.h"
#include "wx/printdlg.h"
#include "wx/module.h"

#include <stdlib.h>
#include <string.h>

#ifdef __WXMSW__
    #include "wx/msw/private.h"
    #include <commdlg.h>

    #ifndef __WIN32__
        #include <print.h>
    #endif
#endif // __WXMSW__

IMPLEMENT_CLASS(wxPrinterBase, wxObject)
IMPLEMENT_ABSTRACT_CLASS(wxPrintout, wxObject)
IMPLEMENT_CLASS(wxPreviewCanvas, wxWindow)
IMPLEMENT_CLASS(wxPreviewControlBar, wxWindow)
IMPLEMENT_CLASS(wxPreviewFrame, wxFrame)
IMPLEMENT_CLASS(wxPrintPreviewBase, wxObject)

BEGIN_EVENT_TABLE(wxPrintAbortDialog, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxPrintAbortDialog::OnCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(wxPreviewCanvas, wxScrolledWindow)
    EVT_PAINT(wxPreviewCanvas::OnPaint)
    EVT_SYS_COLOUR_CHANGED(wxPreviewCanvas::OnSysColourChanged)
END_EVENT_TABLE()

/*
* Printer
*/

wxPrinterBase::wxPrinterBase(wxPrintDialogData *data)
{
    m_currentPrintout = (wxPrintout *) NULL;
    sm_abortWindow = (wxWindow *) NULL;
    sm_abortIt = FALSE;
    if (data)
        m_printDialogData = (*data);
}

wxWindow *wxPrinterBase::sm_abortWindow = (wxWindow *) NULL;
bool wxPrinterBase::sm_abortIt = FALSE;

wxPrinterBase::~wxPrinterBase()
{
}

void wxPrintAbortDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    wxPrinterBase::sm_abortIt = TRUE;
    wxPrinterBase::sm_abortWindow->Show(FALSE);
    wxPrinterBase::sm_abortWindow->Close(TRUE);
    wxPrinterBase::sm_abortWindow = (wxWindow *) NULL;
}

wxWindow *wxPrinterBase::CreateAbortWindow(wxWindow *parent, wxPrintout *WXUNUSED(printout))
{
    wxPrintAbortDialog *dialog = new wxPrintAbortDialog(parent, _("Printing"), wxPoint(0, 0), wxSize(400, 400), wxDEFAULT_DIALOG_STYLE);
    (void) new wxStaticText(dialog, -1, _("Please wait..."), wxPoint(5, 5));

    wxButton *button = new wxButton(dialog, wxID_CANCEL, _("Cancel"), wxPoint(5, 30));

    dialog->Fit();
    button->Centre(wxHORIZONTAL);

    dialog->Centre();
    return dialog;
}

void wxPrinterBase::ReportError(wxWindow *parent, wxPrintout *WXUNUSED(printout), char *message)
{
    wxMessageBox(message, _("Printing Error"), wxOK, parent);
}

/*
* Printout class
*/

wxPrintout::wxPrintout(const wxString& title)
{
    m_printoutTitle = title ;
    m_printoutDC = (wxDC *) NULL;
    m_pageWidthMM = 0;
    m_pageHeightMM = 0;
    m_pageWidthPixels = 0;
    m_pageHeightPixels = 0;
    m_PPIScreenX = 0;
    m_PPIScreenY = 0;
    m_PPIPrinterX = 0;
    m_PPIPrinterY = 0;
    m_isPreview = FALSE;
}

wxPrintout::~wxPrintout()
{
}

bool wxPrintout::OnBeginDocument(int WXUNUSED(startPage), int WXUNUSED(endPage))
{
    return GetDC()->StartDoc(_("Printing"));
}

void wxPrintout::OnEndDocument()
{
    GetDC()->EndDoc();
}

void wxPrintout::OnBeginPrinting()
{
}

void wxPrintout::OnEndPrinting()
{
}

bool wxPrintout::HasPage(int page)
{
    return (page == 1);
}

void wxPrintout::GetPageInfo(int *minPage, int *maxPage, int *fromPage, int *toPage)
{
    *minPage = 1;
    *maxPage = 32000;
    *fromPage = 1;
    *toPage = 1;
}

/*
* Preview canvas
*/

wxPreviewCanvas::wxPreviewCanvas(wxPrintPreviewBase *preview, wxWindow *parent,
                                 const wxPoint& pos, const wxSize& size, long style, const wxString& name):
wxScrolledWindow(parent, -1, pos, size, style, name)
{
    m_printPreview = preview;
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));

    SetScrollbars(15, 18, 100, 100);
}

wxPreviewCanvas::~wxPreviewCanvas()
{
}

void wxPreviewCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    PrepareDC( dc );

/*
#ifdef __WXGTK__
    if (!GetUpdateRegion().IsEmpty())
        dc.SetClippingRegion( GetUpdateRegion() );
#endif
*/

    if (m_printPreview)
    {
        m_printPreview->PaintPage(this, dc);
    }
}

// Responds to colour changes, and passes event on to children.
void wxPreviewCanvas::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));
    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

/*
* Preview control bar
*/

BEGIN_EVENT_TABLE(wxPreviewControlBar, wxPanel)
    EVT_BUTTON(wxID_PREVIEW_CLOSE,    wxPreviewControlBar::OnWindowClose)
    EVT_BUTTON(wxID_PREVIEW_PRINT,    wxPreviewControlBar::OnPrint)
    EVT_BUTTON(wxID_PREVIEW_PREVIOUS, wxPreviewControlBar::OnPreviousButton)
    EVT_BUTTON(wxID_PREVIEW_NEXT,     wxPreviewControlBar::OnNextButton)
    EVT_CHAR(wxPreviewControlBar::OnChar)
    EVT_CHOICE(wxID_PREVIEW_ZOOM,     wxPreviewControlBar::OnZoom)
    EVT_PAINT(wxPreviewControlBar::OnPaint)
END_EVENT_TABLE()

wxPreviewControlBar::wxPreviewControlBar(wxPrintPreviewBase *preview, long buttons,
                                         wxWindow *parent, const wxPoint& pos, const wxSize& size,
                                         long style, const wxString& name):
wxPanel(parent, -1, pos, size, style, name)
{
    m_printPreview = preview;
    m_closeButton = (wxButton *) NULL;
    m_nextPageButton = (wxButton *) NULL;
    m_previousPageButton = (wxButton *) NULL;
    m_printButton = (wxButton *) NULL;
    m_zoomControl = (wxChoice *) NULL;
    m_buttonFlags = buttons;
}

wxPreviewControlBar::~wxPreviewControlBar()
{
}

void wxPreviewControlBar::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    int w, h;
    GetSize(&w, &h);
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawLine( 0, h-1, w, h-1 );
}

void wxPreviewControlBar::OnWindowClose(wxCommandEvent& WXUNUSED(event))
{
    wxPreviewFrame *frame = (wxPreviewFrame *)GetParent();
    frame->Close(TRUE);
}

void wxPreviewControlBar::OnPrint(wxCommandEvent& WXUNUSED(event))
{
    wxPrintPreviewBase *preview = GetPrintPreview();
    preview->Print(TRUE);
}

void wxPreviewControlBar::OnChar(wxKeyEvent &event)
{
   switch(event.KeyCode())
   {
   case WXK_NEXT:
      OnNext(); break;
   case WXK_PRIOR:
      OnPrevious(); break;
   default:
      event.Skip();
   }
}

void wxPreviewControlBar::OnNext(void)
{
    wxPrintPreviewBase *preview = GetPrintPreview();
    if (preview)
    {
        int currentPage = preview->GetCurrentPage();
        if ((preview->GetMaxPage() > 0) &&
            (currentPage < preview->GetMaxPage()) &&
            preview->GetPrintout()->HasPage(currentPage + 1))
        {
            preview->SetCurrentPage(currentPage + 1);
        }
    }
}

void wxPreviewControlBar::OnPrevious(void)
{
    wxPrintPreviewBase *preview = GetPrintPreview();
    if (preview)
    {
        int currentPage = preview->GetCurrentPage();
        if ((preview->GetMinPage() > 0) &&
            (currentPage > preview->GetMinPage()) &&
            preview->GetPrintout()->HasPage(currentPage - 1))
        {
            preview->SetCurrentPage(currentPage - 1);
        }
    }
}

void wxPreviewControlBar::OnZoom(wxCommandEvent& WXUNUSED(event))
{
    int zoom = GetZoomControl();
    if (GetPrintPreview())
        GetPrintPreview()->SetZoom(zoom);
}

void wxPreviewControlBar::CreateButtons()
{
    SetSize(0, 0, 400, 40);

    /*
    #ifdef __WXMSW__
    int fontSize = 9;
    #else
    int fontSize = 10;
    #endif

      wxFont buttonFont(fontSize, wxSWISS, wxNORMAL, wxBOLD);
      SetFont(buttonFont);
    */

    int buttonWidth = 65;
#ifdef __WXGTK__
    int buttonHeight = -1;
#else
    int buttonHeight = 24;
#endif

    int x = 5;
    int y = 5;

#ifdef __WXMOTIF__
    int gap = 15;
#else
    int gap = 5;
#endif

    m_closeButton = new wxButton(this, wxID_PREVIEW_CLOSE, _("Close"),
        wxPoint(x, y), wxSize(buttonWidth, buttonHeight));

    x += gap + buttonWidth;

    if (m_buttonFlags & wxPREVIEW_PRINT)
    {
        m_printButton =  new wxButton(this, wxID_PREVIEW_PRINT, _("Print..."), wxPoint(x, y),
            wxSize(buttonWidth, buttonHeight));
        x += gap + buttonWidth;
    }

    if (m_buttonFlags & wxPREVIEW_PREVIOUS)
    {
        m_previousPageButton = new wxButton(this, wxID_PREVIEW_PREVIOUS, wxT("<<"), wxPoint(x, y),
            wxSize(buttonWidth, buttonHeight));
        x += gap + buttonWidth;
    }

    if (m_buttonFlags & wxPREVIEW_NEXT)
    {
        m_nextPageButton = new wxButton(this, wxID_PREVIEW_NEXT, wxT(">>"),
            wxPoint(x, y), wxSize(buttonWidth, buttonHeight));
        x += gap + buttonWidth;
    }

    if (m_buttonFlags & wxPREVIEW_ZOOM)
    {
        static const char *choices[] =
        {
            "10%", "15%", "20%", "25%", "30%", "35%", "40%", "45%", "50%", "55%",
            "60%", "65%", "70%", "75%", "80%", "85%", "90%", "95%", "100%", "110%",
            "120%", "150%", "200%"
        };

        int n = WXSIZEOF(choices);

        wxString* strings = new wxString[n];
        int i;
        for (i = 0; i < n; i++ )
           strings[i] = choices[i];

        m_zoomControl = new wxChoice(this, wxID_PREVIEW_ZOOM,
                                     wxPoint(x, y),
                                     wxSize(100, -1),
                                     n,
                                     strings
                                    );
        delete[] strings;

        SetZoomControl(m_printPreview->GetZoom());
    }

    //  m_closeButton->SetDefault();
}

void wxPreviewControlBar::SetZoomControl(int zoom)
{
    char buf[20];
    sprintf(buf, "%d%%", zoom);
// Someone is calling methods that do no exist in wxChoice!! So I'll just comment out for VA for now
    if (m_zoomControl)
        m_zoomControl->SetStringSelection(buf);
}

int wxPreviewControlBar::GetZoomControl()
{
    wxChar buf[20];
    if (m_zoomControl && (m_zoomControl->GetStringSelection() != wxT("")))
    {
        wxStrcpy(buf, m_zoomControl->GetStringSelection());
        buf[wxStrlen(buf) - 1] = 0;
        return (int)wxAtoi(buf);
    }
    else return 0;
}


/*
* Preview frame
*/

BEGIN_EVENT_TABLE(wxPreviewFrame, wxFrame)
    EVT_CLOSE(wxPreviewFrame::OnCloseWindow)
END_EVENT_TABLE()

wxPreviewFrame::wxPreviewFrame(wxPrintPreviewBase *preview, wxFrame *parent, const wxString& title,
                               const wxPoint& pos, const wxSize& size, long style, const wxString& name):
wxFrame(parent, -1, title, pos, size, style, name)
{
    m_printPreview = preview;
    m_controlBar = NULL;
    m_previewCanvas = NULL;
}

wxPreviewFrame::~wxPreviewFrame()
{
}

void wxPreviewFrame::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    MakeModal(FALSE);

    // Need to delete the printout and the print preview
    wxPrintout *printout = m_printPreview->GetPrintout();
    if (printout)
    {
        delete printout;
        m_printPreview->SetPrintout(NULL);
        m_printPreview->SetCanvas(NULL);
        m_printPreview->SetFrame(NULL);
    }
    delete m_printPreview;

    Destroy();
}

void wxPreviewFrame::Initialize()
{
    CreateStatusBar();

    CreateCanvas();
    CreateControlBar();

    m_printPreview->SetCanvas(m_previewCanvas);
    m_printPreview->SetFrame(this);

    // Set layout constraints here

    // Control bar constraints
    wxLayoutConstraints *c1 = new wxLayoutConstraints;
    //  int w, h;
    //  m_controlBar->GetSize(&w, &h);
    int h;
#if (defined(__WXMSW__) || defined(__WXGTK__))
    h = 40;
#else
    h = 60;
#endif

    c1->left.SameAs       (this, wxLeft);
    c1->top.SameAs        (this, wxTop);
    c1->right.SameAs      (this, wxRight);
    c1->height.Absolute   (h);

    m_controlBar->SetConstraints(c1);

    // Canvas constraints
    wxLayoutConstraints *c2 = new wxLayoutConstraints;

    c2->left.SameAs       (this, wxLeft);
    c2->top.Below         (m_controlBar);
    c2->right.SameAs      (this, wxRight);
    c2->bottom.SameAs     (this, wxBottom);

    m_previewCanvas->SetConstraints(c2);

    SetAutoLayout(TRUE);

    MakeModal(TRUE);

    Layout();
}

void wxPreviewFrame::CreateCanvas()
{
    m_previewCanvas = new wxPreviewCanvas(m_printPreview, this);
}

void wxPreviewFrame::CreateControlBar()
{
    long buttons = wxPREVIEW_DEFAULT;
    if (m_printPreview->GetPrintoutForPrinting())
        buttons |= wxPREVIEW_PRINT;

    m_controlBar = new wxPreviewControlBar(m_printPreview, buttons, this, wxPoint(0, 0), wxSize(400, 40));
    m_controlBar->CreateButtons();
}

/*
* Print preview
*/

wxPrintPreviewBase::wxPrintPreviewBase(wxPrintout *printout,
                                       wxPrintout *printoutForPrinting,
                                       wxPrintData *data)
{
    if (data)
        m_printDialogData = (*data);

    Init(printout, printoutForPrinting);
}

wxPrintPreviewBase::wxPrintPreviewBase(wxPrintout *printout,
                                       wxPrintout *printoutForPrinting,
                                       wxPrintDialogData *data)
{
    if (data)
        m_printDialogData = (*data);

    Init(printout, printoutForPrinting);
}

void wxPrintPreviewBase::Init(wxPrintout *printout,
                              wxPrintout *printoutForPrinting)
{
    m_isOk = TRUE;
    m_previewPrintout = printout;
    if (m_previewPrintout)
        m_previewPrintout->SetIsPreview(TRUE);

    m_printPrintout = printoutForPrinting;

    m_previewCanvas = NULL;
    m_previewFrame = NULL;
    m_previewBitmap = NULL;
    m_currentPage = 1;
    m_currentZoom = 70;
    m_topMargin = 40;
    m_leftMargin = 40;
    m_pageWidth = 0;
    m_pageHeight = 0;
    m_printingPrepared = FALSE;

    // Too soon! Moved to RenderPage.
    // printout->OnPreparePrinting();

    // Get some parameters from the printout, if defined
    int selFrom, selTo;
    printout->GetPageInfo(&m_minPage, &m_maxPage, &selFrom, &selTo);
}

wxPrintPreviewBase::~wxPrintPreviewBase()
{
    if (m_previewPrintout)
        delete m_previewPrintout;
    if (m_previewBitmap)
        delete m_previewBitmap;
    if (m_printPrintout)
        delete m_printPrintout;
}

bool wxPrintPreviewBase::SetCurrentPage(int pageNum)
{
    if (m_currentPage == pageNum)
        return TRUE;

    m_currentPage = pageNum;
    if (m_previewBitmap)
    {
        delete m_previewBitmap;
        m_previewBitmap = NULL;
    }

    if (m_previewCanvas)
    {
        RenderPage(pageNum);
        m_previewCanvas->Refresh();
    }
    return TRUE;
}

bool wxPrintPreviewBase::PaintPage(wxWindow *canvas, wxDC& dc)
{
    DrawBlankPage(canvas, dc);

    if (!m_previewBitmap)
        RenderPage(m_currentPage);

    if (!m_previewBitmap)
        return FALSE;

    if (!canvas)
        return FALSE;

    int canvasWidth, canvasHeight;
    canvas->GetSize(&canvasWidth, &canvasHeight);

    double zoomScale = ((float)m_currentZoom/(float)100);
    double actualWidth = (zoomScale*m_pageWidth*m_previewScale);
    //  float actualHeight = (float)(zoomScale*m_pageHeight*m_previewScale);

    int x = (int) ((canvasWidth - actualWidth)/2.0);
    if (x < m_leftMargin)
        x = m_leftMargin;
    int y = m_topMargin;

    wxMemoryDC temp_dc;
    temp_dc.SelectObject(*m_previewBitmap);

    dc.Blit(x, y, m_previewBitmap->GetWidth(), m_previewBitmap->GetHeight(), &temp_dc, 0, 0);

    temp_dc.SelectObject(wxNullBitmap);

    return TRUE;
}

bool wxPrintPreviewBase::RenderPage(int pageNum)
{
    int canvasWidth, canvasHeight;

    if (!m_previewCanvas)
    {
        wxMessageBox(_("wxPrintPreviewBase::RenderPage: must use wxPrintPreviewBase::SetCanvas to let me know about the canvas!"),
            _("Print Preview Failure"), wxOK);
        return FALSE;
    }
    m_previewCanvas->GetSize(&canvasWidth, &canvasHeight);

    double zoomScale = (m_currentZoom/100.0);
    int actualWidth = (int)(zoomScale*m_pageWidth*m_previewScale);
    int actualHeight = (int)(zoomScale*m_pageHeight*m_previewScale);

    int x = (int)((canvasWidth - actualWidth)/2.0);
    if (x < m_leftMargin)
        x = m_leftMargin;
    //  int y = m_topMargin;


    if (!m_previewBitmap)
    {
        m_previewBitmap = new wxBitmap((int)actualWidth, (int)actualHeight);
        if (!m_previewBitmap || !m_previewBitmap->Ok())
        {
            if (m_previewBitmap)
                delete m_previewBitmap;
            wxMessageBox(_("Sorry, not enough memory to create a preview."), _("Print Preview Failure"), wxOK);
            return FALSE;
        }
    }

    wxMemoryDC memoryDC;
    memoryDC.SelectObject(*m_previewBitmap);

    memoryDC.Clear();

    m_previewPrintout->SetDC(&memoryDC);
    m_previewPrintout->SetPageSizePixels(m_pageWidth, m_pageHeight);

    // Need to delay OnPreparePrinting until here, so we have enough information.
    if (!m_printingPrepared)
    {
        m_previewPrintout->OnPreparePrinting();
        m_printingPrepared = TRUE;
    }

    m_previewPrintout->OnBeginPrinting();

    if (!m_previewPrintout->OnBeginDocument(m_printDialogData.GetFromPage(), m_printDialogData.GetToPage()))
    {
        wxMessageBox(_("Could not start document preview."), _("Print Preview Failure"), wxOK);

        memoryDC.SelectObject(wxNullBitmap);

        delete m_previewBitmap;
        return FALSE;
    }

    m_previewPrintout->OnPrintPage(pageNum);
    m_previewPrintout->OnEndDocument();
    m_previewPrintout->OnEndPrinting();

    m_previewPrintout->SetDC(NULL);

    memoryDC.SelectObject(wxNullBitmap);

    wxChar buf[200];
    if (m_maxPage != 0)
        wxSprintf(buf, _("Page %d of %d"), pageNum, m_maxPage);
    else
        wxSprintf(buf, _("Page %d"), pageNum);

    if (m_previewFrame)
        m_previewFrame->SetStatusText(buf);

    return TRUE;
}


bool wxPrintPreviewBase::DrawBlankPage(wxWindow *canvas, wxDC& dc)
{
    int canvasWidth, canvasHeight;
    canvas->GetSize(&canvasWidth, &canvasHeight);

    float zoomScale = (float)((float)m_currentZoom/(float)100);
    float actualWidth = zoomScale*m_pageWidth*m_previewScale;
    float actualHeight = zoomScale*m_pageHeight*m_previewScale;

    float x = (float)((canvasWidth - actualWidth)/2.0);
    if (x < m_leftMargin)
        x = (float)m_leftMargin;
    float y = (float)m_topMargin;

    // Draw shadow, allowing for 1-pixel border AROUND the actual page
    int shadowOffset = 4;
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxBLACK_BRUSH);
    /*
    dc.DrawRectangle((int)(x-1 + shadowOffset), (int)(y-1 + shadowOffset), (int)(actualWidth+2), (int)(actualHeight+2));
    */
    dc.DrawRectangle((int)(x + shadowOffset), (int)(y + actualHeight+1), (int)(actualWidth), shadowOffset);
    dc.DrawRectangle((int)(x + actualWidth), (int)(y + shadowOffset), shadowOffset, (int)(actualHeight));

    // Draw blank page allowing for 1-pixel border AROUND the actual page
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);

    /*
    wxRegion update_region = canvas->GetUpdateRegion();
    wxRect r = update_region.GetBox();

      printf( "x: %d y: %d w: %d h: %d.\n", (int)r.x, (int)r.y, (int)r.width, (int)r.height );
    */

    dc.DrawRectangle((int)(x-2), (int)(y-1), (int)(actualWidth+3), (int)(actualHeight+2));

    return TRUE;
}

void wxPrintPreviewBase::SetZoom(int percent)
{
    if (m_currentZoom == percent)
        return;

    m_currentZoom = percent;
    if (m_previewBitmap)
    {
        delete m_previewBitmap;
        m_previewBitmap = NULL;
    }

    if (m_previewCanvas)
    {
        RenderPage(m_currentPage);
        ((wxScrolledWindow *) m_previewCanvas)->Scroll(0, 0);
        m_previewCanvas->Clear();
        m_previewCanvas->Refresh();
    }
}

#endif // wxUSE_PRINTING_ARCHITECTURE
