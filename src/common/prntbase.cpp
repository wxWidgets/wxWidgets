/////////////////////////////////////////////////////////////////////////////
// Name:        prntbase.cpp
// Purpose:     Printing framework base class implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
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

#ifdef __WXMSW__
#define __GOOD_COMPILER__
#endif

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

#include <stdlib.h>
#include <string.h>

#ifdef __WXMSW__
#include <windows.h>
#include <commdlg.h>

// Clash with Windows header files
#ifdef StartDoc
#undef StartDoc
#endif

#ifndef __WIN32__
#include <print.h>
#endif

#endif
 // End __WXMSW__

#if !USE_SHARED_LIBRARY
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
#endif

/*
 * Printer
 */
 
wxPrinterBase::wxPrinterBase(wxPrintData *data)
{
  m_currentPrintout = (wxPrintout *) NULL;
  sm_abortWindow = (wxWindow *) NULL;
  sm_abortIt = FALSE;
  if (data)
    m_printData = (*data);
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

  SetScrollbars(40, 40, 100, 100);
}

wxPreviewCanvas::~wxPreviewCanvas()
{
}

void wxPreviewCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);

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
	EVT_BUTTON(wxID_PREVIEW_CLOSE, 		wxPreviewControlBar::OnClose)
	EVT_BUTTON(wxID_PREVIEW_PRINT, 		wxPreviewControlBar::OnPrint)
	EVT_BUTTON(wxID_PREVIEW_PREVIOUS, 	wxPreviewControlBar::OnPrevious)
	EVT_BUTTON(wxID_PREVIEW_NEXT, 		wxPreviewControlBar::OnNext)
	EVT_CHOICE(wxID_PREVIEW_ZOOM, 		wxPreviewControlBar::OnZoom)
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

void wxPreviewControlBar::OnClose(wxCommandEvent& WXUNUSED(event))
{
  wxPreviewFrame *frame = (wxPreviewFrame *)GetParent();
  frame->Close(TRUE);
}

void wxPreviewControlBar::OnPrint(wxCommandEvent& WXUNUSED(event))
{
  wxPrintPreviewBase *preview = GetPrintPreview();
  preview->Print(TRUE);
}

void wxPreviewControlBar::OnNext(wxCommandEvent& WXUNUSED(event))
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

void wxPreviewControlBar::OnPrevious(wxCommandEvent& WXUNUSED(event))
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
#ifdef __GOOD_COMPILER__ // Robert Roebling

  SetSize(0, 0, 400, 40);

#ifdef __WXMSW__
  int fontSize = 9;
#else
  int fontSize = 10;
#endif

  wxFont buttonFont(fontSize, wxSWISS, wxNORMAL, wxBOLD);
  SetButtonFont(buttonFont);

  int buttonWidth = 65;
  int buttonHeight = 24;

  int x = 5;
  int y = 5;
  int gap = 5;

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
    m_previousPageButton = new wxButton(this, wxID_PREVIEW_PREVIOUS, "<<", wxPoint(x, y),
		wxSize(buttonWidth, buttonHeight));
	x += gap + buttonWidth;
  }

  if (m_buttonFlags & wxPREVIEW_NEXT)
  {
    m_nextPageButton = new wxButton(this, wxID_PREVIEW_NEXT, ">>",
	 	wxPoint(x, y), wxSize(buttonWidth, buttonHeight));
	x += gap + buttonWidth;
  }

  static wxString choices[] = { "10%", "20%", "25%", "30%", "35%", "40%", "45%", "50%", "55%", "60%",
    "65%", "70%", "75%", "80%", "85%", "90%", "95%", "100%", "110%", "120%", "150%", "200%" };
  int n = 22;
  if (m_buttonFlags & wxPREVIEW_ZOOM)
  {
    m_zoomControl = new wxChoice(this, wxID_PREVIEW_ZOOM, wxPoint(x, y),
		wxSize(100, -1), n, (wxString *)choices);
    SetZoomControl(m_printPreview->GetZoom());
  }

  m_closeButton->SetDefault();
  
#endif
}

void wxPreviewControlBar::SetZoomControl(int zoom)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
  char buf[20];
  sprintf(buf, "%d%%", zoom);
  if (m_zoomControl)
    m_zoomControl->SetStringSelection(buf);
#endif
}

int wxPreviewControlBar::GetZoomControl()
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
  char buf[20];
  if (m_zoomControl && m_zoomControl->GetStringSelection())
  {
    strcpy(buf, m_zoomControl->GetStringSelection());
    buf[strlen(buf) - 1] = 0;
    return (int)atoi(buf);
  }
  else return 0;
#else
  return 0;
#endif
}


/*
 * Preview frame
 */

wxPreviewFrame::wxPreviewFrame(wxPrintPreviewBase *preview, wxFrame *parent, const wxString& title,
    const wxPoint& pos, const wxSize& size, long style, const wxString& name):
 wxFrame(parent, -1, title, pos, size, style, name)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
  m_printPreview = preview;
  m_controlBar = NULL;
  m_previewCanvas = NULL;
#endif
}

wxPreviewFrame::~wxPreviewFrame()
{
}

bool wxPreviewFrame::OnClose()
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

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
  return TRUE;
#else
  return FALSE;
#endif
}

void wxPreviewFrame::Initialize()
{

#ifdef __GOOD_COMPILER__ // Robert Roebling

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
#ifdef __WXMSW__
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
  
#endif  
}

void wxPreviewFrame::CreateCanvas()
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  m_previewCanvas = new wxPreviewCanvas(m_printPreview, this);
  
#endif
}

void wxPreviewFrame::CreateControlBar()
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  long buttons = wxPREVIEW_DEFAULT;
  if (m_printPreview->GetPrintoutForPrinting())
    buttons |= wxPREVIEW_PRINT;
    
  m_controlBar = new wxPreviewControlBar(m_printPreview, buttons, this, wxPoint(0, 0), wxSize(400, 40));
  m_controlBar->CreateButtons();
#endif
}
 
/*
 * Print preview
 */

wxPrintPreviewBase::wxPrintPreviewBase(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data)
{

#ifdef __GOOD_COMPILER__ // Robert Roebling

  m_isOk = TRUE;
  m_previewPrintout = printout;
  if (m_previewPrintout)
    m_previewPrintout->SetIsPreview(TRUE);
    
  m_printPrintout = printoutForPrinting;
  if (data)
    m_printData = (*data);

  m_previewCanvas = NULL;
  m_previewFrame = NULL;
  m_previewBitmap = NULL;
  m_currentPage = 1;
  m_currentZoom = 30;
  m_topMargin = 40;
  m_leftMargin = 40;
  m_pageWidth = 0;
  m_pageHeight = 0;

  printout->OnPreparePrinting();

  // Get some parameters from the printout, if defined
  int selFrom, selTo;
  printout->GetPageInfo(&m_minPage, &m_maxPage, &selFrom, &selTo);
  
#endif  
}

wxPrintPreviewBase::~wxPrintPreviewBase()
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  if (m_previewPrintout)
    delete m_previewPrintout;
  if (m_previewBitmap)
    delete m_previewBitmap;
  if (m_printPrintout)
    delete m_printPrintout;
    
#endif
}

bool wxPrintPreviewBase::SetCurrentPage(int pageNum)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
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
  
#endif
  return TRUE;
}

bool wxPrintPreviewBase::PaintPage(wxWindow *canvas, wxDC& dc)
{

#ifdef __GOOD_COMPILER__  // Robert Roebling

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

#endif  
  
  return TRUE;
}

bool wxPrintPreviewBase::RenderPage(int pageNum)
{
  int canvasWidth, canvasHeight;
  
#ifdef __GOOD_COMPILER__  // Robert Roebling

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

  m_previewPrintout->OnBeginPrinting();
  

  if (!m_previewPrintout->OnBeginDocument(m_printData.GetFromPage(), m_printData.GetToPage()))
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
#endif

  char buf[200];
  if (m_maxPage != 0)
    sprintf(buf, _("Page %d of %d"), pageNum, m_maxPage);
  else
    sprintf(buf, _("Page %d"), pageNum);

  if (m_previewFrame)
    m_previewFrame->SetStatusText(buf);

  return TRUE;
}


bool wxPrintPreviewBase::DrawBlankPage(wxWindow *canvas, wxDC& dc)
{

#ifdef __GOOD_COMPILER__ // Robert Roebling

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
  dc.DrawRectangle(x-1 + shadowOffset, y-1 + shadowOffset, actualWidth+2, actualHeight+2);

  // Draw blank page allowing for 1-pixel border AROUND the actual page
  dc.SetPen(*wxBLACK_PEN);
  dc.SetBrush(*wxWHITE_BRUSH);
  

  dc.DrawRectangle(x-1, y-1, actualWidth+2, actualHeight+2);
  
#endif

  return TRUE;
}

void wxPrintPreviewBase::SetZoom(int percent)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
  if (m_currentZoom == percent)
    return;
    
  m_currentZoom = percent;
  if (m_previewBitmap)
  {
    delete m_previewBitmap;
    m_previewBitmap = NULL;
  }
  RenderPage(m_currentPage);
  
  if (m_previewCanvas)
  {
    m_previewCanvas->Clear();
    m_previewCanvas->Refresh();
  }
#endif  
  
}
