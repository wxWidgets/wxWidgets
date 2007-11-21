/////////////////////////////////////////////////////////////////////////////
// Name:        expended.cpp
// Purpose:     Layout/foldpanelbar sample
// Author:      Jorgen Bodde
// Modified by:
// Created:     24/07/2004
// Copyright:   (c) Jorgen Bodde based upon FoldPanelBarTest (c) Julian Smart
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wx/toolbar.h"
#include "wx/laywin.h"
#include "wx/spinctrl.h"
#include "wx/slider.h"

#include "sample.xpm"

#include "extended.h"

MyFrame *frame = NULL;
wxList my_children;

IMPLEMENT_APP(MyApp)

// For drawing lines in a canvas
long xpos = -1;
long ypos = -1;

int winNumber = 1;

// Initialise this in OnInit, not statically
bool MyApp::OnInit(void)
{
    // Create the main frame window

    frame = new MyFrame(NULL, wxID_ANY, _T("FoldPanelBar Extended Demo"), wxDefaultPosition, wxSize(500, 600),
                        wxDEFAULT_FRAME_STYLE |
                        wxNO_FULL_REPAINT_ON_RESIZE |
                        wxHSCROLL | wxVSCROLL);

    frame->SetIcon(wxIcon(sample_xpm));

    // Associate the menu bar with the frame
    frame->SetMenuBar(CreateMenuBar(false));

    frame->CreateStatusBar();

    frame->Show(true);

    SetTopWindow(frame);

    return true;
}

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
    EVT_MENU(FPBTEST_ABOUT, MyFrame::OnAbout)
    EVT_MENU(FPBTEST_NEW_WINDOW, MyFrame::OnNewWindow)
    EVT_SIZE(MyFrame::OnSize)
    EVT_MENU(FPBTEST_QUIT, MyFrame::OnQuit)
    EVT_MENU(FPBTEST_TOGGLE_WINDOW, MyFrame::OnToggleWindow)
    EVT_SASH_DRAGGED_RANGE(ID_WINDOW_TOP, ID_WINDOW_BOTTOM, MyFrame::OnFoldPanelBarDrag)
    EVT_MENU(FPB_BOTTOM_STICK, MyFrame::OnCreateBottomStyle)
    EVT_MENU(FPB_SINGLE_FOLD, MyFrame::OnCreateNormalStyle)
    EVT_BUTTON(ID_COLLAPSEME, MyFrame::OnCollapseMe)
    EVT_BUTTON(ID_APPLYTOALL, MyFrame::OnExpandMe)
    EVT_SCROLL(MyFrame::OnSlideColour)
    EVT_RADIOBUTTON(ID_USE_HGRADIENT, MyFrame::OnStyleChange)
    EVT_RADIOBUTTON(ID_USE_VGRADIENT, MyFrame::OnStyleChange)
    EVT_RADIOBUTTON(ID_USE_SINGLE, MyFrame::OnStyleChange)
    EVT_RADIOBUTTON(ID_USE_RECTANGLE, MyFrame::OnStyleChange)
    EVT_RADIOBUTTON(ID_USE_FILLED_RECTANGLE, MyFrame::OnStyleChange)
END_EVENT_TABLE()


// Define my frame constructor
MyFrame::MyFrame(wxWindow *parent, const wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size,
    const long style)
  : wxMDIParentFrame(parent, id, title, pos, size, style)
  , m_flags(0)
{
    m_leftWindow1 = new wxSashLayoutWindow(this, ID_WINDOW_LEFT1,
                                wxDefaultPosition, wxSize(200, 30),
                                wxNO_BORDER | wxSW_3D | wxCLIP_CHILDREN);

    m_leftWindow1->SetDefaultSize(wxSize(160, 1000));
    m_leftWindow1->SetOrientation(wxLAYOUT_VERTICAL);
    m_leftWindow1->SetAlignment(wxLAYOUT_LEFT);
    m_leftWindow1->SetSashVisible(wxSASH_RIGHT, true);
    m_leftWindow1->SetExtraBorderSize(0);

    m_pnl = NULL;
    ReCreateFoldPanel(0);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox(_T("wxWidgets 2.0 FoldPanelBar Demo\nAuthor: Julian Smart (c) 1998"), _T("About FoldPanelBar Demo"));
}

void MyFrame::OnToggleWindow(wxCommandEvent& WXUNUSED(event))
{
    m_leftWindow1->Show(!m_leftWindow1->IsShown());
#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE
}

void MyFrame::OnFoldPanelBarDrag(wxSashEvent& event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    if(event.GetId() == ID_WINDOW_LEFT1)
        m_leftWindow1->SetDefaultSize(wxSize(event.GetDragRect().width, 1000));

#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE

    // Leaves bits of itself behind sometimes
    GetClientWindow()->Refresh();
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event))
{
    // Make another frame, containing a canvas
    MyChild *subframe = new MyChild(frame, _T("Canvas Frame"),
                                    wxPoint(10, 10), wxSize(300, 300),
                                    wxDEFAULT_FRAME_STYLE |
                                    wxNO_FULL_REPAINT_ON_RESIZE);

    subframe->SetTitle(wxString::Format(_T("Canvas Frame %d"), winNumber));
    winNumber ++;

    // Give it a status line
    subframe->CreateStatusBar();

    // Associate the menu bar with the frame
    subframe->SetMenuBar(CreateMenuBar(true));

    int width, height;
    subframe->GetClientSize(&width, &height);
    MyCanvas *canvas = new MyCanvas(subframe, wxPoint(0, 0), wxSize(width, height));
    canvas->SetCursor(wxCursor(wxCURSOR_PENCIL));
    subframe->canvas = canvas;

    // Give it scrollbars
    canvas->SetScrollbars(20, 20, 50, 50);

    subframe->Show(true);
}

void MyFrame::ReCreateFoldPanel(int fpb_flags)
{
    // delete earlier panel
    m_leftWindow1->DestroyChildren();

    // recreate the foldpanelbar

    m_pnl = new wxFoldPanelBar(m_leftWindow1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFPB_DEFAULT_STYLE, fpb_flags);

    wxFoldPanel item = m_pnl->AddFoldPanel(_T("Caption colours"), false);

    m_pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), wxID_ANY, _T("Adjust the first colour")),
                              wxFPB_ALIGN_WIDTH, 5, 20);

    // RED color spin control
    m_rslider1 = new wxSlider(item.GetParent(), wxID_ANY, 0, 0, 255);
    m_pnl->AddFoldPanelWindow(item, m_rslider1, wxFPB_ALIGN_WIDTH,
                              2, 20);

    // GREEN color spin control
    m_gslider1 = new wxSlider(item.GetParent(), wxID_ANY, 0, 0, 255);
    m_pnl->AddFoldPanelWindow(item, m_gslider1, wxFPB_ALIGN_WIDTH,
                              0, 20);

    // BLUE color spin control
    m_bslider1 = new wxSlider(item.GetParent(), wxID_ANY, 0, 0, 255);
    m_pnl->AddFoldPanelWindow(item, m_bslider1, wxFPB_ALIGN_WIDTH,
                              0, 20);

    m_pnl->AddFoldPanelSeperator(item);

    m_pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), wxID_ANY, _T("Adjust the second colour")),
                              wxFPB_ALIGN_WIDTH, 5, 20);

    // RED color spin control
    m_rslider2 = new wxSlider(item.GetParent(), wxID_ANY, 0, 0, 255);
    m_pnl->AddFoldPanelWindow(item, m_rslider2, wxFPB_ALIGN_WIDTH,
                              2, 20);

    // GREEN color spin control
    m_gslider2 = new wxSlider(item.GetParent(), wxID_ANY, 0, 0, 255);
    m_pnl->AddFoldPanelWindow(item, m_gslider2, wxFPB_ALIGN_WIDTH,
                              0, 20);

    // BLUE color spin control
    m_bslider2 = new wxSlider(item.GetParent(), wxID_ANY, 0, 0, 255);
    m_pnl->AddFoldPanelWindow(item, m_bslider2, wxFPB_ALIGN_WIDTH,
                              0, 20);

    m_pnl->AddFoldPanelSeperator(item);

    m_btn = new wxButton(item.GetParent(), ID_APPLYTOALL, _T("Apply to all"));
    m_pnl->AddFoldPanelWindow(item, m_btn);

    // read back current gradients and set the sliders
    // for the colour which is now taken as default

    wxCaptionBarStyle style = m_pnl->GetCaptionStyle(item);
    wxColour col = style.GetFirstColour();
    m_rslider1->SetValue(col.Red());
    m_gslider1->SetValue(col.Green());
    m_bslider1->SetValue(col.Blue());

    col = style.GetSecondColour();
    m_rslider2->SetValue(col.Red());
    m_gslider2->SetValue(col.Green());
    m_bslider2->SetValue(col.Blue());

    // put down some caption styles from which the user can
    // select to show how the current or all caption bars will look like

    item = m_pnl->AddFoldPanel(_T("Caption style"), false);

    wxRadioButton *currStyle =  new wxRadioButton(item.GetParent(), ID_USE_VGRADIENT, _T("&Vertical gradient"));
    m_pnl->AddFoldPanelWindow(item, currStyle,  wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 10);
    currStyle->SetValue(true);

    m_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_HGRADIENT, _T("&Horizontal gradient")),
                              wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 10);
    m_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_SINGLE, _T("&Single colour")),
                              wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 10);
    m_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_RECTANGLE, _T("&Rectangle box")),
                              wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 10);
    m_pnl->AddFoldPanelWindow(item, new wxRadioButton(item.GetParent(), ID_USE_FILLED_RECTANGLE, _T("&Filled rectangle box")),
                              wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 10);

    m_pnl->AddFoldPanelSeperator(item);

    m_single = new wxCheckBox(item.GetParent(), wxID_ANY, _T("&Only this caption"));
    m_pnl->AddFoldPanelWindow(item, m_single, wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 10);


    // one more panel to finish it

    wxCaptionBarStyle cs;
    cs.SetCaptionStyle(wxCAPTIONBAR_RECTANGLE);

    item = m_pnl->AddFoldPanel(_T("Misc stuff"), true, cs);

    m_pnl->AddFoldPanelWindow(item, new wxButton(item.GetParent(), ID_COLLAPSEME, _T("Collapse All")));

    m_pnl->AddFoldPanelWindow(item, new wxStaticText(item.GetParent(), wxID_ANY, _T("Enter some comments")),
                             wxFPB_ALIGN_WIDTH, 5, 20);

    m_pnl->AddFoldPanelWindow(item, new wxTextCtrl(item.GetParent(), wxID_ANY, _T("Comments")),
                             wxFPB_ALIGN_WIDTH, wxFPB_DEFAULT_SPACING, 10);

    m_leftWindow1->SizeWindows();

}

void MyFrame::OnCreateBottomStyle(wxCommandEvent& event)
{
    // recreate with style collapse to bottom, which means
    // all panels that are collapsed are placed at the bottom,
    // or normal

    if(event.IsChecked())
        m_flags |= wxFPB_COLLAPSE_TO_BOTTOM;
    else
        m_flags &= ~wxFPB_COLLAPSE_TO_BOTTOM;

    ReCreateFoldPanel(m_flags);
}

void MyFrame::OnCreateNormalStyle(wxCommandEvent& event)
{
    // receate with style where only one panel at the time is
    // allowed to be opened

    // TODO: Not yet implemented!

    if(event.IsChecked())
        m_flags |= wxFPB_SINGLE_FOLD;
    else
        m_flags &= ~wxFPB_SINGLE_FOLD;

    ReCreateFoldPanel(m_flags);
}

void MyFrame::OnCollapseMe(wxCommandEvent &WXUNUSED(event))
{
    wxFoldPanel item(0);
    for(size_t i = 0; i < m_pnl->GetCount(); i++)
    {
        item = m_pnl->Item(i);
        m_pnl->Collapse(item);
    }
}

void MyFrame::OnExpandMe(wxCommandEvent &WXUNUSED(event))
{
    wxColour col1((unsigned char)m_rslider1->GetValue(),
                  (unsigned char)m_gslider1->GetValue(),
                  (unsigned char)m_bslider1->GetValue()),
             col2((unsigned char)m_rslider2->GetValue(),
                  (unsigned char)m_gslider2->GetValue(),
                  (unsigned char)m_bslider2->GetValue());

    wxCaptionBarStyle style;

    style.SetFirstColour(col1);
    style.SetSecondColour(col2);

    m_pnl->ApplyCaptionStyleAll(style);
}

wxMenuBar *CreateMenuBar(bool with_window)
{
    // Make a menubar
    wxMenu *file_menu = new wxMenu;

    file_menu->Append(FPBTEST_NEW_WINDOW, _T("&New window"));
    if(with_window)
        file_menu->Append(FPBTEST_CHILD_QUIT, _T("&Close child"));

    file_menu->AppendSeparator();
    file_menu->Append(FPBTEST_QUIT, _T("&Exit"));

    wxMenu *option_menu = 0;
    if(with_window)
    {
        // Dummy option
        option_menu = new wxMenu;
        option_menu->Append(FPBTEST_REFRESH, _T("&Refresh picture"));
    }

    // make fold panel menu

    wxMenu *fpb_menu = new wxMenu;
    fpb_menu->AppendCheckItem(FPB_BOTTOM_STICK, _T("Create with &wxFPB_COLLAPSE_TO_BOTTOM"));
    //fpb_menu->AppendCheckItem(FPB_SINGLE_FOLD, _T("Create with &wxFPB_SINGLE_FOLD"));

    fpb_menu->AppendSeparator();
    fpb_menu->Append(FPBTEST_TOGGLE_WINDOW, _T("&Toggle FoldPanelBar"));

    wxMenu *help_menu = new wxMenu;
    help_menu->Append(FPBTEST_ABOUT, _T("&About"));

    wxMenuBar *menu_bar = new wxMenuBar;

    menu_bar->Append(file_menu, _T("&File"));
    menu_bar->Append(fpb_menu, _T("&FoldPanel"));
    if(option_menu)
        menu_bar->Append(option_menu, _T("&Options"));
    menu_bar->Append(help_menu, _T("&Help"));

    return menu_bar;
}

void MyFrame::OnSlideColour(wxScrollEvent &WXUNUSED(event))
{
    wxColour col1((unsigned char)m_rslider1->GetValue(),
                  (unsigned char)m_gslider1->GetValue(),
                  (unsigned char)m_bslider1->GetValue()),
             col2((unsigned char)m_rslider2->GetValue(),
                  (unsigned char)m_gslider2->GetValue(),
                  (unsigned char)m_bslider2->GetValue());
    //m_btn->SetBackgroundColour(col);

    wxCaptionBarStyle style;

    style.SetFirstColour(col1);
    style.SetSecondColour(col2);

    wxFoldPanel item = m_pnl->Item(0);
    m_pnl->ApplyCaptionStyle(item, style);
}

void MyFrame::OnStyleChange(wxCommandEvent &event)
{
    wxCaptionBarStyle style;
    switch(event.GetId())
    {
    case ID_USE_HGRADIENT:
        style.SetCaptionStyle(wxCAPTIONBAR_GRADIENT_H);
        break;

    case ID_USE_VGRADIENT:
        style.SetCaptionStyle(wxCAPTIONBAR_GRADIENT_V);
        break;

    case ID_USE_SINGLE:
        style.SetCaptionStyle(wxCAPTIONBAR_SINGLE);
        break;

    case ID_USE_RECTANGLE:
        style.SetCaptionStyle(wxCAPTIONBAR_RECTANGLE);
        break;

    case ID_USE_FILLED_RECTANGLE:
        style.SetCaptionStyle(wxCAPTIONBAR_FILLED_RECTANGLE);
        break;

    default:
        break;
    }

    if(m_single->GetValue())
    {
        wxFoldPanel item = m_pnl->Item(1);
        m_pnl->ApplyCaptionStyle(item, style);
    }
    else
    {
        m_pnl->ApplyCaptionStyleAll(style);
    }
}

/* ----------------------------------------------------------------------------------------------- */

BEGIN_EVENT_TABLE(MyCanvas, wxScrolledWindow)
    EVT_MOUSE_EVENTS(MyCanvas::OnEvent)
END_EVENT_TABLE()

// Define a constructor for my canvas
MyCanvas::MyCanvas(wxWindow *parent, const wxPoint& pos, const wxSize& size)
         :wxScrolledWindow(parent, wxID_ANY, pos, size,
                           wxSUNKEN_BORDER | wxNO_FULL_REPAINT_ON_RESIZE)
{
    SetBackgroundColour(* wxWHITE);
}

// Define the repainting behaviour
void MyCanvas::OnDraw(wxDC& dc)
{
    dc.SetFont(*wxSWISS_FONT);
    dc.SetPen(*wxGREEN_PEN);
    dc.DrawLine(0, 0, 200, 200);
    dc.DrawLine(200, 0, 0, 200);

    dc.SetBrush(*wxCYAN_BRUSH);
    dc.SetPen(*wxRED_PEN);
    dc.DrawRectangle(100, 100, 100, 50);
    dc.DrawRoundedRectangle(150, 150, 100, 50, 20);

    dc.DrawEllipse(250, 250, 100, 50);
#if wxUSE_SPLINES
    dc.DrawSpline(50, 200, 50, 100, 200, 10);
#endif // wxUSE_SPLINES
    dc.DrawLine(50, 230, 200, 230);
    dc.DrawText(_T("This is a test string"), 50, 230);

    wxPoint points[3];
    points[0].x = 200; points[0].y = 300;
    points[1].x = 100; points[1].y = 400;
    points[2].x = 300; points[2].y = 400;

    dc.DrawPolygon(3, points);
}

// This implements a tiny doodling program! Drag the mouse using
// the left button.
void MyCanvas::OnEvent(wxMouseEvent& event)
{
    wxClientDC dc(this);
    PrepareDC(dc);

    wxPoint pt(event.GetLogicalPosition(dc));

    if (xpos > -1 && ypos > -1 && event.Dragging())
    {
        dc.SetPen(*wxBLACK_PEN);
        dc.DrawLine(xpos, ypos, pt.x, pt.y);
    }
    xpos = pt.x;
    ypos = pt.y;
}

void MyFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if wxUSE_MDI_ARCHITECTURE
    wxLayoutAlgorithm layout;
    layout.LayoutMDIFrame(this);
#endif // wxUSE_MDI_ARCHITECTURE
}

// Note that FPBTEST_NEW_WINDOW and FPBTEST_ABOUT commands get passed
// to the parent window for processing, so no need to
// duplicate event handlers here.

BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
    EVT_MENU(FPBTEST_CHILD_QUIT, MyChild::OnQuit)
END_EVENT_TABLE()

MyChild::MyChild(wxMDIParentFrame *parent, const wxString& title,
                 const wxPoint& pos, const wxSize& size,
                 const long style)
        :wxMDIChildFrame(parent, wxID_ANY, title, pos, size, style)
{
    canvas = NULL;
    my_children.Append(this);
}

MyChild::~MyChild(void)
{
    my_children.DeleteObject(this);
}

void MyChild::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyChild::OnActivate(wxActivateEvent& event)
{
    if (event.GetActive() && canvas)
        canvas->SetFocus();
}
