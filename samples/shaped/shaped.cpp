/////////////////////////////////////////////////////////////////////////////
// Name:        shaped.cpp
// Purpose:     Shaped Window sample
// Author:      Robin Dunn
// Modified by:
// Created:     28-Mar-2003
// Copyright:   (c) Robin Dunn
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/panel.h"
    #include "wx/stattext.h"
    #include "wx/menu.h"
    #include "wx/layout.h"
    #include "wx/msgdlg.h"
    #include "wx/image.h"
#endif

#include "wx/dcclient.h"
#include "wx/graphics.h"
#include "wx/image.h"
#include "wx/scopedptr.h"
#include "wx/sizer.h"
#include "wx/slider.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// menu ids
enum
{
    Show_Shaped = 100,
    Show_Transparent,
    Show_TransparentBg,

    // must be consecutive and in the same order as wxShowEffect enum elements
    Show_Effect_First,
    Show_Effect_Roll = Show_Effect_First,
    Show_Effect_Slide,
    Show_Effect_Blend,
    Show_Effect_Expand,
    Show_Effect_Last = Show_Effect_Expand
};

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    // override base class virtuals
    // ----------------------------

    // this one is called on application startup and is a good place for the app
    // initialization (doing it here and not in the ctor allows to have an error
    // return: if OnInit() returns false, the application terminates)
    virtual bool OnInit() wxOVERRIDE;
};


// Main frame just contains the menu items invoking the other tests
class MainFrame : public wxFrame
{
public:
    MainFrame();

private:
    void OnShowShaped(wxCommandEvent& event);
    void OnShowTransparent(wxCommandEvent& event);
    void OnShowEffect(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

// Define a new frame type: this is going to the frame showing the
// effect of wxFRAME_SHAPED
class ShapedFrame : public wxFrame
{
public:
    // ctor(s)
    ShapedFrame(wxFrame *parent);
    void SetWindowShape();

    // event handlers (these functions should _not_ be virtual)
    void OnDoubleClick(wxMouseEvent& evt);
    void OnLeftDown(wxMouseEvent& evt);
    void OnLeftUp(wxMouseEvent& evt);
    void OnMouseMove(wxMouseEvent& evt);
    void OnExit(wxMouseEvent& evt);
    void OnPaint(wxPaintEvent& evt);

private:
    enum ShapeKind
    {
        Shape_None,
        Shape_Star,
#if wxUSE_GRAPHICS_CONTEXT
        Shape_Circle,
#endif // wxUSE_GRAPHICS_CONTEXT
        Shape_Max
    } m_shapeKind;

    wxBitmap m_bmp;
    wxPoint  m_delta;

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

// Define a new frame type: this is going to the frame showing the
// effect of wxWindow::SetTransparent and of
// wxWindow::SetBackgroundStyle(wxBG_STYLE_TRANSPARENT)
class SeeThroughFrame : public wxFrame
{
public:
    void Create(wxWindow* parent);

private:
    // event handlers (these functions should _not_ be virtual)
    void OnPaint(wxPaintEvent& evt);

    void OnAlpha(wxCommandEvent& event);

    // any class wishing to process wxWidgets events must use this macro
    wxDECLARE_EVENT_TABLE();
};

class EffectFrame : public wxFrame
{
public:
    EffectFrame(wxWindow *parent,
                wxShowEffect effect,
                // TODO: add menu command to the main frame to allow changing
                //       these parameters
                unsigned timeout = 1000)
        : wxFrame(parent, wxID_ANY,
                  wxString::Format("Frame shown with %s effect",
                                   GetEffectName(effect)),
                  wxDefaultPosition, wxSize(450, 300)),
          m_effect(effect),
          m_timeout(timeout)
    {
        new wxStaticText(this, wxID_ANY,
                         wxString::Format("Effect: %s", GetEffectName(effect)),
                         wxPoint(20, 20));
        new wxStaticText(this, wxID_ANY,
                         wxString::Format("Timeout: %ums", m_timeout),
                         wxPoint(20, 60));

        ShowWithEffect(m_effect, m_timeout);

        Bind(wxEVT_CLOSE_WINDOW, &EffectFrame::OnClose, this);
    }

private:
    static const char *GetEffectName(wxShowEffect effect)
    {
        static const char *names[] =
        {
            "none",
            "roll to left",
            "roll to right",
            "roll to top",
            "roll to bottom",
            "slide to left",
            "slide to right",
            "slide to top",
            "slide to bottom",
            "fade",
            "expand",
        };
        wxCOMPILE_TIME_ASSERT( WXSIZEOF(names) == wxSHOW_EFFECT_MAX,
                                EffectNamesMismatch );

        return names[effect];
    }

    void OnClose(wxCloseEvent& event)
    {
        HideWithEffect(m_effect, m_timeout);

        event.Skip();
    }

    wxShowEffect m_effect;
    unsigned m_timeout;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

// `Main program' equivalent: the program execution "starts" here
bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    wxInitAllImageHandlers();

    new MainFrame;

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_MENU(Show_Shaped, MainFrame::OnShowShaped)
    EVT_MENU(Show_Transparent, MainFrame::OnShowTransparent)
    EVT_MENU(Show_TransparentBg, MainFrame::OnShowTransparent)
    EVT_MENU_RANGE(Show_Effect_First, Show_Effect_Last, MainFrame::OnShowEffect)
    EVT_MENU(wxID_EXIT, MainFrame::OnExit)
wxEND_EVENT_TABLE()

MainFrame::MainFrame()
         : wxFrame(NULL, wxID_ANY, "wxWidgets Shaped Sample",
                   wxDefaultPosition, wxSize(200, 100))
{
    SetIcon(wxICON(sample));

    wxMenuBar * const mbar = new wxMenuBar;
    wxMenu * const menuFrames = new wxMenu;
    menuFrames->Append(Show_Shaped, "Show &shaped window\tCtrl-S");
    menuFrames->Append(Show_Transparent, "Show &transparent window\tCtrl-T");
    menuFrames->Append(Show_TransparentBg,
                       "Show with &transparent background\tShift-Ctrl-T");
    menuFrames->AppendSeparator();
    menuFrames->Append(Show_Effect_Roll, "Show &rolled effect\tCtrl-R");
    menuFrames->Append(Show_Effect_Slide, "Show s&lide effect\tCtrl-L");
    menuFrames->Append(Show_Effect_Blend, "Show &fade effect\tCtrl-F");
    menuFrames->Append(Show_Effect_Expand, "Show &expand effect\tCtrl-E");
    menuFrames->AppendSeparator();
    menuFrames->Append(wxID_EXIT, "E&xit");

    mbar->Append(menuFrames, "&Show");
    SetMenuBar(mbar);

    Show();
}

void MainFrame::OnShowShaped(wxCommandEvent& WXUNUSED(event))
{
    ShapedFrame *shapedFrame = new ShapedFrame(this);
    shapedFrame->Show(true);
}

void MainFrame::OnShowTransparent(wxCommandEvent& event)
{
    SeeThroughFrame *seeThroughFrame = new SeeThroughFrame;

    if ( event.GetId() == Show_TransparentBg )
    {
        wxString reason;
        if (IsTransparentBackgroundSupported(&reason))
        {
            seeThroughFrame->SetBackgroundStyle(wxBG_STYLE_TRANSPARENT);
        }
        else
        {
            wxLogError("Can't use transparent background: %s", reason);
            delete seeThroughFrame;
            return;
        }
    }

    seeThroughFrame->Create(this);
    seeThroughFrame->Show(true);
}

void MainFrame::OnShowEffect(wxCommandEvent& event)
{
    int effect = event.GetId();
    static wxDirection direction = wxLEFT;
    direction = (wxDirection)(((int)direction)<< 1);
    if ( direction > wxDOWN )
        direction = wxLEFT ;

    wxShowEffect eff;
    switch ( effect )
    {
        case Show_Effect_Roll:
            switch ( direction )
            {
                case wxLEFT:
                    eff = wxSHOW_EFFECT_ROLL_TO_LEFT;
                    break;
                case wxRIGHT:
                    eff = wxSHOW_EFFECT_ROLL_TO_RIGHT;
                    break;
                case wxTOP:
                    eff = wxSHOW_EFFECT_ROLL_TO_TOP;
                    break;
                case wxBOTTOM:
                    eff = wxSHOW_EFFECT_ROLL_TO_BOTTOM;
                    break;
                default:
                    wxFAIL_MSG( "invalid direction" );
                    return;
            }
            break;
        case Show_Effect_Slide:
            switch ( direction )
            {
                case wxLEFT:
                    eff = wxSHOW_EFFECT_SLIDE_TO_LEFT;
                    break;
                case wxRIGHT:
                    eff = wxSHOW_EFFECT_SLIDE_TO_RIGHT;
                    break;
                case wxTOP:
                    eff = wxSHOW_EFFECT_SLIDE_TO_TOP;
                    break;
                case wxBOTTOM:
                    eff = wxSHOW_EFFECT_SLIDE_TO_BOTTOM;
                    break;
                default:
                    wxFAIL_MSG( "invalid direction" );
                    return;
            }
            break;

        case Show_Effect_Blend:
            eff = wxSHOW_EFFECT_BLEND;
            break;

        case Show_Effect_Expand:
            eff = wxSHOW_EFFECT_EXPAND;
            break;

        default:
            wxFAIL_MSG( "invalid effect" );
            return;
    }

    new EffectFrame(this,  eff, 1000);
}

void MainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

// ----------------------------------------------------------------------------
// shaped frame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(ShapedFrame, wxFrame)
    EVT_LEFT_DCLICK(ShapedFrame::OnDoubleClick)
    EVT_LEFT_DOWN(ShapedFrame::OnLeftDown)
    EVT_LEFT_UP(ShapedFrame::OnLeftUp)
    EVT_MOTION(ShapedFrame::OnMouseMove)
    EVT_RIGHT_UP(ShapedFrame::OnExit)
    EVT_PAINT(ShapedFrame::OnPaint)
wxEND_EVENT_TABLE()


// frame constructor
ShapedFrame::ShapedFrame(wxFrame *parent)
       : wxFrame(parent, wxID_ANY, wxEmptyString,
                  wxDefaultPosition, wxSize(100, 100),
                  0
                  | wxFRAME_SHAPED
                  | wxSIMPLE_BORDER
                  | wxFRAME_NO_TASKBAR
                  | wxSTAY_ON_TOP
            )
{
    m_shapeKind = Shape_Star;
    m_bmp = wxBitmap("star.png", wxBITMAP_TYPE_PNG);
    SetSize(wxSize(m_bmp.GetWidth(), m_bmp.GetHeight()));
    SetToolTip("Right-click to close, double click to cycle shape");
    SetWindowShape();
}

void ShapedFrame::SetWindowShape()
{
    switch ( m_shapeKind )
    {
        case Shape_None:
            SetShape(wxRegion());
            break;

        case Shape_Star:
            SetShape(wxRegion(m_bmp, *wxWHITE));
            break;

#if wxUSE_GRAPHICS_CONTEXT
        case Shape_Circle:
            {
                wxGraphicsPath
                    path = wxGraphicsRenderer::GetDefaultRenderer()->CreatePath();
                path.AddCircle(m_bmp.GetWidth()/2, m_bmp.GetHeight()/2, 30);
                SetShape(path);
            }
            break;
#endif // wxUSE_GRAPHICS_CONTEXT

        case Shape_Max:
            wxFAIL_MSG( "invalid shape kind" );
            break;
    }
}

void ShapedFrame::OnDoubleClick(wxMouseEvent& WXUNUSED(evt))
{
    m_shapeKind = static_cast<ShapeKind>((m_shapeKind + 1) % Shape_Max);
    SetWindowShape();
}

void ShapedFrame::OnLeftDown(wxMouseEvent& evt)
{
    CaptureMouse();
    wxPoint pos = ClientToScreen(evt.GetPosition());
    wxPoint origin = GetPosition();
    int dx =  pos.x - origin.x;
    int dy = pos.y - origin.y;
    m_delta = wxPoint(dx, dy);
}

void ShapedFrame::OnLeftUp(wxMouseEvent& WXUNUSED(evt))
{
    if (HasCapture())
    {
        ReleaseMouse();
    }
}

void ShapedFrame::OnMouseMove(wxMouseEvent& evt)
{
    wxPoint pt = evt.GetPosition();
    if (evt.Dragging() && evt.LeftIsDown())
    {
        wxPoint pos = ClientToScreen(pt);
        Move(wxPoint(pos.x - m_delta.x, pos.y - m_delta.y));
    }
}

void ShapedFrame::OnExit(wxMouseEvent& WXUNUSED(evt))
{
    Close();
}

void ShapedFrame::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
    dc.DrawBitmap(m_bmp, 0, 0, true);
}

// ----------------------------------------------------------------------------
// see-through frame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(SeeThroughFrame, wxFrame)
    EVT_PAINT(SeeThroughFrame::OnPaint)
wxEND_EVENT_TABLE()

void
SeeThroughFrame::Create(wxWindow* parent)
{
    wxFrame::Create(parent, wxID_ANY, "Transparency test",
           wxDefaultPosition, wxWindow::FromDIP(wxSize(300, 300), parent),
           wxDEFAULT_FRAME_STYLE |
           wxFULL_REPAINT_ON_RESIZE |
           wxSTAY_ON_TOP);
    SetBackgroundColour(*wxWHITE);

    const int initialAlpha = wxALPHA_OPAQUE / 2;

    // Create control for choosing alpha and put it in the middle of the window.
    wxPanel* panel = new wxPanel(this);
    wxSlider* slider = new wxSlider
                           (
                                panel,
                                wxID_ANY,
                                initialAlpha,
                                wxALPHA_TRANSPARENT,
                                wxALPHA_OPAQUE,
                                wxDefaultPosition,
                                FromDIP(wxSize(256, -1)),
                                wxSL_HORIZONTAL | wxSL_LABELS
                           );
    slider->Bind(wxEVT_SLIDER, &SeeThroughFrame::OnAlpha, this);

    const wxSizerFlags center = wxSizerFlags().Center().Border();

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(new wxStaticText(panel, wxID_ANY, "Window opacity:"), center);
    sizer->Add(slider, center);
    panel->SetSizer(sizer);

    wxSizer* sizerTop = new wxBoxSizer(wxVERTICAL);
    sizerTop->AddStretchSpacer();
    sizerTop->Add(panel, center);
    sizerTop->AddStretchSpacer();
    SetSizer(sizerTop);

    // Note that this must be called before the frame is shown.
    SetTransparent(initialAlpha);
}

// Paints a grid of varying hue and alpha
void SeeThroughFrame::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
    wxPaintDC dc(this);
    wxScopedPtr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

    // Draw 3 bands: one opaque, one semi-transparent and one transparent.
    const wxSize size = GetClientSize();
    const double h = size.y / 3.0;

    wxGraphicsPath path = gc->CreatePath();
    path.AddRectangle(0, 0, size.x, h);
    gc->SetBrush(wxBrush(wxColour(0, 0, 255, wxALPHA_OPAQUE)));
    gc->FillPath(path);

    gc->SetBrush(wxBrush(wxColour(0, 0, 255, wxALPHA_OPAQUE / 2)));
    gc->Translate(0, h);
    gc->FillPath(path);

    // This blue won't actually be seen and instead the white background will
    // be visible, because this brush is fully transparent.
    gc->SetBrush(wxBrush(wxColour(0, 0, 255, wxALPHA_TRANSPARENT)));
    gc->Translate(0, h);
    gc->FillPath(path);
}

void SeeThroughFrame::OnAlpha(wxCommandEvent& event)
{
    SetTransparent(event.GetInt());
}
