/////////////////////////////////////////////////////////////////////////////
// Name:        printing.h
// Purpose:     Printing demo for wxWindows
// Author:      Julian Smart
// Modified by:
// Created:     1995
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface
#endif

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp() ;
    bool OnInit();
    int OnExit();

    wxFont* m_testFont;
};

DECLARE_APP(MyApp)

class MyCanvas;

// Define a new canvas and frame
class MyFrame: public wxFrame
{
  public:
    MyCanvas *canvas;
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);

    void Draw(wxDC& dc);

    void OnSize(wxSizeEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnPrintPreview(wxCommandEvent& event);
    void OnPrintSetup(wxCommandEvent& event);
    void OnPageSetup(wxCommandEvent& event);
#if defined(__WXMSW__) && wxTEST_POSTSCRIPT_IN_MSW
    void OnPrintPS(wxCommandEvent& event);
    void OnPrintPreviewPS(wxCommandEvent& event);
    void OnPrintSetupPS(wxCommandEvent& event);
    void OnPageSetupPS(wxCommandEvent& event);
#endif

    void OnExit(wxCommandEvent& event);
    void OnPrintAbout(wxCommandEvent& event);
DECLARE_EVENT_TABLE()
};

// Define a new canvas which can receive some events
class MyCanvas: public wxScrolledWindow
{
  public:
    MyCanvas(wxFrame *frame, const wxPoint& pos, const wxSize& size, long style = wxRETAINED);
    ~MyCanvas(void) ;

    virtual void OnDraw(wxDC& dc);
    void OnEvent(wxMouseEvent& event);

DECLARE_EVENT_TABLE()
};

class MyPrintout: public wxPrintout
{
 public:
  MyPrintout(char *title = "My printout"):wxPrintout(title) {}
  bool OnPrintPage(int page);
  bool HasPage(int page);
  bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

  void DrawPageOne(wxDC *dc);
  void DrawPageTwo(wxDC *dc);
};

#define WXPRINT_QUIT            100
#define WXPRINT_PRINT           101
#define WXPRINT_PRINT_SETUP     102
#define WXPRINT_PAGE_SETUP      103
#define WXPRINT_PREVIEW         104

#define WXPRINT_PRINT_PS        105
#define WXPRINT_PRINT_SETUP_PS  106
#define WXPRINT_PAGE_SETUP_PS   107
#define WXPRINT_PREVIEW_PS      108

#define WXPRINT_ABOUT           109

