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

#if !defined(APIENTRY)	// NT defines APIENTRY, 3.x not
#define APIENTRY FAR PASCAL
#endif
 
#ifdef __WIN32__
#define _EXPORT /**/
#else
#define _EXPORT _export
typedef signed short int SHORT ;
#endif
 
#if !defined(__WIN32__)	// 3.x uses FARPROC for dialogs
#define DLGPROC FARPROC
#endif

LONG APIENTRY _EXPORT wxAbortProc(HDC hPr, int Code);
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
  currentPrintout = NULL;
  abortWindow = NULL;
  abortIt = FALSE;
  if (data)
    printData = (*data);
}

wxWindow *wxPrinterBase::abortWindow = NULL;
bool wxPrinterBase::abortIt = FALSE;

wxPrinterBase::~wxPrinterBase(void)
{
}

void wxPrintAbortDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
  wxPrinterBase::abortIt = TRUE;
  wxPrinterBase::abortWindow->Show(FALSE);
  wxPrinterBase::abortWindow->Close(TRUE);
  wxPrinterBase::abortWindow = NULL;
}

wxWindow *wxPrinterBase::CreateAbortWindow(wxWindow *parent, wxPrintout *WXUNUSED(printout))
{
  wxPrintAbortDialog *dialog = new wxPrintAbortDialog(parent, _("Printing"), wxPoint(0, 0), wxSize(400, 400));
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
 
wxPrintout::wxPrintout(const char *title)
{
  printoutTitle = title ? copystring(title) : NULL;
  printoutDC = NULL;
  pageWidthMM = 0;
  pageHeightMM = 0;
  pageWidthPixels = 0;
  pageHeightPixels = 0;
  PPIScreenX = 0;
  PPIScreenY = 0;
  PPIPrinterX = 0;
  PPIPrinterY = 0;
  isPreview = FALSE;
}

wxPrintout::~wxPrintout(void)
{
  if (printoutTitle)
    delete[] printoutTitle;
}

bool wxPrintout::OnBeginDocument(int WXUNUSED(startPage), int WXUNUSED(endPage))
{
  return GetDC()->StartDoc(_("Printing"));
}

void wxPrintout::OnEndDocument(void)
{
  GetDC()->EndDoc();
}

void wxPrintout::OnBeginPrinting(void)
{
}

void wxPrintout::OnEndPrinting(void)
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
  printPreview = preview;
  SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_APPWORKSPACE));

  SetScrollbars(40, 40, 100, 100);
}

wxPreviewCanvas::~wxPreviewCanvas(void)
{
}

void wxPreviewCanvas::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);

  if (printPreview)
  {
    printPreview->PaintPage(this, dc);
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
  printPreview = preview;
  closeButton = NULL;
  nextPageButton = NULL;
  previousPageButton = NULL;
  printButton = NULL;
  zoomControl = NULL;
  buttonFlags = buttons;
}

wxFont *wxPreviewControlBar::buttonFont = NULL;

wxPreviewControlBar::~wxPreviewControlBar(void)
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

void wxPreviewControlBar::CreateButtons(void)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  SetSize(0, 0, 400, 40);

#ifdef __WXMSW__
  int fontSize = 9;
#else
  int fontSize = 10;
#endif

  if (!buttonFont)
    buttonFont = wxTheFontList->FindOrCreateFont(fontSize, wxSWISS, wxNORMAL, wxBOLD);
  SetButtonFont(*buttonFont);

  int buttonWidth = 65;
  int buttonHeight = 24;

  int x = 5;
  int y = 5;
  int gap = 5;

  closeButton = new wxButton(this, wxID_PREVIEW_CLOSE, _("Close"),
  	wxPoint(x, y), wxSize(buttonWidth, buttonHeight));

  x += gap + buttonWidth;
  
  if (buttonFlags & wxPREVIEW_PRINT)
  {
    printButton =  new wxButton(this, wxID_PREVIEW_PRINT, _("Print..."), wxPoint(x, y),
		wxSize(buttonWidth, buttonHeight));
	x += gap + buttonWidth;
  }

  if (buttonFlags & wxPREVIEW_PREVIOUS)
  {
    previousPageButton = new wxButton(this, wxID_PREVIEW_PREVIOUS, "<<", wxPoint(x, y),
		wxSize(buttonWidth, buttonHeight));
	x += gap + buttonWidth;
  }

  if (buttonFlags & wxPREVIEW_NEXT)
  {
    nextPageButton = new wxButton(this, wxID_PREVIEW_NEXT, ">>",
	 	wxPoint(x, y), wxSize(buttonWidth, buttonHeight));
	x += gap + buttonWidth;
  }

  static wxString choices[] = { "10%", "20%", "25%", "30%", "35%", "40%", "45%", "50%", "55%", "60%",
    "65%", "70%", "75%", "80%", "85%", "90%", "95%", "100%", "110%", "120%", "150%", "200%" };
  int n = 22;
  if (buttonFlags & wxPREVIEW_ZOOM)
  {
    zoomControl = new wxChoice(this, wxID_PREVIEW_ZOOM, wxPoint(x, y),
		wxSize(100, -1), n, (wxString *)choices);
    SetZoomControl(printPreview->GetZoom());
  }

  closeButton->SetDefault();
  
#endif
}

void wxPreviewControlBar::SetZoomControl(int zoom)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
  char buf[20];
  sprintf(buf, "%d%%", zoom);
  if (zoomControl)
    zoomControl->SetStringSelection(buf);
#endif
}

int wxPreviewControlBar::GetZoomControl(void)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
  char buf[20];
  if (zoomControl && zoomControl->GetStringSelection())
  {
    strcpy(buf, zoomControl->GetStringSelection());
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

  printPreview = preview;
  controlBar = NULL;
  previewCanvas = NULL;
#endif
}

wxPreviewFrame::~wxPreviewFrame(void)
{
}

bool wxPreviewFrame::OnClose(void)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  MakeModal(FALSE);
  
  // Need to delete the printout and the print preview
  wxPrintout *printout = printPreview->GetPrintout();
  if (printout)
  {
    delete printout;
    printPreview->SetPrintout(NULL);
    printPreview->SetCanvas(NULL);
    printPreview->SetFrame(NULL);
  }
  delete printPreview;
  return TRUE;
#else
  return FALSE;
#endif
}

void wxPreviewFrame::Initialize(void)
{

#ifdef __GOOD_COMPILER__ // Robert Roebling

  CreateStatusBar();
  
  CreateCanvas();
  CreateControlBar();

  printPreview->SetCanvas(previewCanvas);
  printPreview->SetFrame(this);

  // Set layout constraints here

  // Control bar constraints
  wxLayoutConstraints *c1 = new wxLayoutConstraints;
//  int w, h;
//  controlBar->GetSize(&w, &h);
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

  controlBar->SetConstraints(c1);

  // Canvas constraints
  wxLayoutConstraints *c2 = new wxLayoutConstraints;

  c2->left.SameAs       (this, wxLeft);
  c2->top.Below         (controlBar);
  c2->right.SameAs      (this, wxRight);
  c2->bottom.SameAs     (this, wxBottom);

  previewCanvas->SetConstraints(c2);

  SetAutoLayout(TRUE);

  MakeModal(TRUE);

  Layout();
  
#endif  
}

void wxPreviewFrame::CreateCanvas(void)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  previewCanvas = new wxPreviewCanvas(printPreview, this);
  
#endif
}

void wxPreviewFrame::CreateControlBar(void)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  long buttons = wxPREVIEW_DEFAULT;
  if (printPreview->GetPrintoutForPrinting())
    buttons |= wxPREVIEW_PRINT;
    
  controlBar = new wxPreviewControlBar(printPreview, buttons, this, wxPoint(0, 0), wxSize(400, 40));
  controlBar->CreateButtons();
#endif
}
 
/*
 * Print preview
 */

wxPrintPreviewBase::wxPrintPreviewBase(wxPrintout *printout, wxPrintout *printoutForPrinting, wxPrintData *data)
{

#ifdef __GOOD_COMPILER__ // Robert Roebling

  isOk = TRUE;
  previewPrintout = printout;
  if (previewPrintout)
    previewPrintout->SetIsPreview(TRUE);
    
  printPrintout = printoutForPrinting;
  if (data)
    printData = (*data);

  previewCanvas = NULL;
  previewFrame = NULL;
  previewBitmap = NULL;
  currentPage = 1;
  currentZoom = 30;
  topMargin = 40;
  leftMargin = 40;
  pageWidth = 0;
  pageHeight = 0;

  printout->OnPreparePrinting();

  // Get some parameters from the printout, if defined
  int selFrom, selTo;
  printout->GetPageInfo(&minPage, &maxPage, &selFrom, &selTo);
  
#endif  
}

wxPrintPreviewBase::~wxPrintPreviewBase(void)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling

  if (previewPrintout)
    delete previewPrintout;
  if (previewBitmap)
    delete previewBitmap;
  if (printPrintout)
    delete printPrintout;
    
#endif
}

bool wxPrintPreviewBase::SetCurrentPage(int pageNum)
{
#ifdef __GOOD_COMPILER__ // Robert Roebling
  if (currentPage == pageNum)
    return TRUE;

  currentPage = pageNum;
  if (previewBitmap)
  {
    delete previewBitmap;
    previewBitmap = NULL;
  }

  if (previewCanvas)
  {
    RenderPage(pageNum);
    previewCanvas->Refresh();
  }
  
#endif
  return TRUE;
}

bool wxPrintPreviewBase::PaintPage(wxWindow *canvas, wxDC& dc)
{

#ifdef __GOOD_COMPILER__  // Robert Roebling

  DrawBlankPage(canvas, dc);

  if (!previewBitmap)
    RenderPage(currentPage);
    
  if (!previewBitmap)
    return FALSE;

  if (!canvas)
    return FALSE;

  int canvasWidth, canvasHeight;
  canvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = (float)(zoomScale*pageWidth*previewScale);
//  float actualHeight = (float)(zoomScale*pageHeight*previewScale);

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
  float y = (float)topMargin;

  wxMemoryDC temp_dc;
  temp_dc.SelectObject(*previewBitmap);

  dc.Blit((float)x, (float)y, (float)previewBitmap->GetWidth(), (float)previewBitmap->GetHeight(), &temp_dc, (float)0, (float)0);

  temp_dc.SelectObject(wxNullBitmap);

#endif  
  
  return TRUE;
}

bool wxPrintPreviewBase::RenderPage(int pageNum)
{
  int canvasWidth, canvasHeight;
  
#ifdef __GOOD_COMPILER__  // Robert Roebling

  if (!previewCanvas)
  {
    wxMessageBox(_("wxPrintPreviewBase::RenderPage: must use wxPrintPreviewBase::SetCanvas to let me know about the canvas!"),
      _("Print Preview Failure"), wxOK);
    return FALSE;
  }
  previewCanvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = (float)(zoomScale*pageWidth*previewScale);
  float actualHeight = (float)(zoomScale*pageHeight*previewScale);

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
//  float y = topMargin;


  if (!previewBitmap)
  {
    previewBitmap = new wxBitmap((int)actualWidth, (int)actualHeight);
    if (!previewBitmap || !previewBitmap->Ok())
    {
      if (previewBitmap)
        delete previewBitmap;
      wxMessageBox(_("Sorry, not enough memory to create a preview."), _("Print Preview Failure"), wxOK);
      return FALSE;
    }
  }
  
  wxMemoryDC memoryDC;
  memoryDC.SelectObject(*previewBitmap);

  memoryDC.Clear();

  previewPrintout->SetDC(&memoryDC);
  previewPrintout->SetPageSizePixels(pageWidth, pageHeight);

  previewPrintout->OnBeginPrinting();
  

  if (!previewPrintout->OnBeginDocument(printData.GetFromPage(), printData.GetToPage()))
  {
    wxMessageBox(_("Could not start document preview."), _("Print Preview Failure"), wxOK);
    
    memoryDC.SelectObject(wxNullBitmap);

    delete previewBitmap;
    return FALSE;
  }
  
  previewPrintout->OnPrintPage(pageNum);
  previewPrintout->OnEndDocument();
  previewPrintout->OnEndPrinting();

  previewPrintout->SetDC(NULL);
  
  memoryDC.SelectObject(wxNullBitmap);
#endif

  char buf[200];
  if (maxPage != 0)
    sprintf(buf, _("Page %d of %d"), pageNum, maxPage);
  else
    sprintf(buf, _("Page %d"), pageNum);

  if (previewFrame)
    previewFrame->SetStatusText(buf);

  return TRUE;
}


bool wxPrintPreviewBase::DrawBlankPage(wxWindow *canvas, wxDC& dc)
{

#ifdef __GOOD_COMPILER__ // Robert Roebling

  int canvasWidth, canvasHeight;
  canvas->GetSize(&canvasWidth, &canvasHeight);
  
  float zoomScale = (float)((float)currentZoom/(float)100);
  float actualWidth = zoomScale*pageWidth*previewScale;
  float actualHeight = zoomScale*pageHeight*previewScale;

  float x = (float)((canvasWidth - actualWidth)/2.0);
  if (x < leftMargin)
    x = (float)leftMargin;
  float y = (float)topMargin;

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
  if (currentZoom == percent)
    return;
    
  currentZoom = percent;
  if (previewBitmap)
  {
    delete previewBitmap;
    previewBitmap = NULL;
  }
  RenderPage(currentPage);
  
  if (previewCanvas)
  {
    previewCanvas->Clear();
    previewCanvas->Refresh();
  }
#endif  
  
}
