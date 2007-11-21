/////////////////////////////////////////////////////////////////////////////
// Name:        samples/printing.h

// Purpose:     Printing demo for wxWidgets
// Author:      Julian Smart
// Modified by:
// Created:     1995
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(){};
    bool OnInit();
    int OnExit();

    wxFont m_testFont;
};

DECLARE_APP(MyApp)

class MyCanvas;

// Define a new canvas and frame
class MyFrame: public wxFrame
{
  public:
    MyCanvas *canvas;
    wxBitmap m_bitmap;
    int      m_angle;
    MyFrame(wxFrame *frame, const wxString& title, const wxPoint& pos, const wxSize& size);

    void Draw(wxDC& dc);
    void OnAngleUp(wxCommandEvent& event);
    void OnAngleDown(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event);
    void OnPrint(wxCommandEvent& event);
    void OnPrintPreview(wxCommandEvent& event);
    void OnPageSetup(wxCommandEvent& event);
#if defined(__WXMSW__) && wxTEST_POSTSCRIPT_IN_MSW
    void OnPrintPS(wxCommandEvent& event);
    void OnPrintPreviewPS(wxCommandEvent& event);
    void OnPageSetupPS(wxCommandEvent& event);
#endif
#ifdef __WXMAC__
    void OnPageMargins(wxCommandEvent& event);
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
    ~MyCanvas(void){};

    virtual void OnDraw(wxDC& dc);
    void OnEvent(wxMouseEvent& event);

DECLARE_EVENT_TABLE()
};

class MyPrintout: public wxPrintout
{
 public:
  MyPrintout(const wxChar *title = _T("My printout")):wxPrintout(title) {}
  bool OnPrintPage(int page);
  bool HasPage(int page);
  bool OnBeginDocument(int startPage, int endPage);
  void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);

  void DrawPageOne();

  void DrawPageTwo();

};

#define WXPRINT_QUIT            100
#define WXPRINT_PRINT           101
#define WXPRINT_PAGE_SETUP      103
#define WXPRINT_PREVIEW         104

#define WXPRINT_PRINT_PS        105
#define WXPRINT_PAGE_SETUP_PS   107
#define WXPRINT_PREVIEW_PS      108

#define WXPRINT_ABOUT           109

#define WXPRINT_ANGLEUP         110
#define WXPRINT_ANGLEDOWN       111

#ifdef __WXMAC__
    #define WXPRINT_PAGE_MARGINS 112
#endif
