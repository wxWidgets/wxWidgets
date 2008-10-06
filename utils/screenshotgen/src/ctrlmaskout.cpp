/////////////////////////////////////////////////////////////////////////////
// Name:        ctrlmaskout.cpp
// Purpose:     Implement CtrlMaskOut class
// Author:      Utensil Candel (UtensilCandel@@gmail.com)
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __WXMAC__ // See CreateMask()
    #include <wx/minifram.h>
    #include <cstdlib>
#endif

#include <wx/filename.h>
#include <wx/hashset.h>

#include "ctrlmaskout.h"


// It's copied from src/aui/framemanager.cpp and modified, a failed attempt to
// visualize the process of taking the screenshot when wxTopLevelWindow::CanSetTransparent
// returns false. see CtrlMaskOut::CreateMask for more info
// now it shows nothing and does nothing
/////////////////////////////////////////////////////////////////////////////

class wxPseudoTransparentFrame : public wxFrame
{
public:
    wxPseudoTransparentFrame(wxWindow* parent = NULL,
                wxWindowID id = wxID_ANY,
                const wxString& title = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString &name = wxT("frame"))
                    : wxFrame(parent, id, title, pos, size, style | wxFRAME_SHAPED, name)
    {
        m_lastWidth=0;
        m_lastHeight=0;
    }

    virtual bool SetTransparent(wxByte alpha)
    {
        return true;
    }

    void OnPaint(wxPaintEvent& WXUNUSED(event)){}

    virtual void SetSize(const wxRect& rect){}
    virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO){}
    virtual void SetSize(int width, int height){}
    virtual void SetSize(const wxSize& size){}

    virtual bool Show(bool show = true){ return true;}

    void OnSize(wxSizeEvent& event)
    {
//        // We sometimes get surplus size events
//        if ((event.GetSize().GetWidth() == m_lastWidth) &&
//            (event.GetSize().GetHeight() == m_lastHeight))
//        {
//            event.Skip();
//            return;
//        }
//
// 		Show(false);
//
// 		GetParent()->Update();
//
// 		Show(true);
//
//
//        m_lastWidth = event.GetSize().GetWidth();
//        m_lastHeight = event.GetSize().GetHeight();
    }

private:
    int m_lastWidth,m_lastHeight;

    DECLARE_DYNAMIC_CLASS(wxPseudoTransparentFrame)
    DECLARE_EVENT_TABLE()
};

IMPLEMENT_DYNAMIC_CLASS(wxPseudoTransparentFrame, wxFrame)

BEGIN_EVENT_TABLE(wxPseudoTransparentFrame, wxFrame)
    EVT_PAINT(wxPseudoTransparentFrame::OnPaint)
    EVT_SIZE(wxPseudoTransparentFrame::OnSize)
END_EVENT_TABLE()



// ----------------------------------------------------------------------------
// CtrlMaskOut
// ----------------------------------------------------------------------------

CtrlMaskOut::CtrlMaskOut()
      : m_defaultDir(_T("screenshots")),
        m_controlName(_T("")),
        m_currentRect(0, 0, 0, 0),
        m_inflateBorder(5),
        m_mask(NULL),
        m_isTiming(false)
{
}

CtrlMaskOut::~CtrlMaskOut()
{
    if (m_mask != NULL)
    {
        m_mask->Destroy();
        m_mask = NULL;
    }
}

void CtrlMaskOut::OnLeftButtonDown(wxMouseEvent& event)
{
    if (m_isTiming) event.Skip();

    // Start draging at the left-top corner.
    wxWindow * thePanel = (wxWindow *)event.GetEventObject();
    m_currentRect.SetPosition(thePanel->ClientToScreen(event.GetPosition()));
    m_currentRect.SetSize(wxSize(1, 1));

    // Create a transparent mask to visulize the process of specifying a rect region..
    CreateMask(thePanel);
}

void CtrlMaskOut::OnMouseMoving(wxMouseEvent& event)
{
    if (!event.Dragging())
    {
        event.Skip();
        return;
    }

    wxWindow * thePanel = (wxWindow *)event.GetEventObject();

    // Determine the current rect region
    m_currentRect.SetBottomRight(thePanel->ClientToScreen(event.GetPosition()) - wxPoint(5, 5));
    // /Because Mac can't get the left btn up event fired if we don't minus wxPoint(5, 5),
    // /May fix it by making the panel handle this event later
    // /This is the current trick
    // /See also line 200 in CreateMask

    // Visualize the current rect region
    m_mask->SetSize(m_currentRect);
}

void CtrlMaskOut::OnLeftButtonUp(wxMouseEvent& event)
{
    if (m_mask == NULL)// Which means it's not after specifying a rect region
    {
        event.Skip();
        return;
    }

    DestroyMask();

    // End dragging at the right-bottom corner.
    wxWindow * thePanel = (wxWindow *)event.GetEventObject();
    m_currentRect.SetRightBottom(thePanel->ClientToScreen(event.GetPosition()));

    if (event.ControlDown())
    {
        m_isTiming = true;
        (new wxTimer(this))->Start(3000, wxTIMER_ONE_SHOT);
        this->Connect(wxEVT_TIMER, wxTimerEventHandler( CtrlMaskOut::OnTimingFinished ), NULL, this);
    }
    else
    {
        // The final rect region is determined.
        DetermineCtrlNameAndRect();

        Capture(m_currentRect, m_controlName);
    }
}

void CtrlMaskOut::OnTimingFinished(wxTimerEvent& event)
{
    // The final rect region is determined.
    DetermineCtrlNameAndRect();

    Capture(m_currentRect, m_controlName);

    m_isTiming = false;
    this->Disconnect(wxEVT_TIMER, wxTimerEventHandler( CtrlMaskOut::OnTimingFinished ), NULL, this);
}

void CtrlMaskOut::Capture(int x, int y, int width, int height, wxString fileName)
{
    // Somehow wxScreenDC.Blit() doesn't work under Mac for now. Here is a trick.
#ifdef __WXMAC__

    // wxExecute(_T("screencapture -x ") + tempfile, wxEXEC_SYNC);

    system("screencapture -x /tmp/wx_screen_capture.png");

    wxBitmap fullscreen;

    do
    {
        fullscreen = wxBitmap(_T("/tmp/wx_screen_capture.png"), wxBITMAP_TYPE_PNG);
    }
    while(!fullscreen.IsOk());

    wxBitmap screenshot = fullscreen.GetSubBitmap(wxRect(x,y,width,height));

    #else // Under other paltforms, take a real screenshot

    // Create a DC for the whole screen area
    wxScreenDC dcScreen;

    // Create a Bitmap that will later on hold the screenshot image
    // Note that the Bitmap must have a size big enough to hold the screenshot
    // -1 means using the current default colour depth
    wxBitmap screenshot(width, height, -1);

    // Create a memory DC that will be used for actually taking the screenshot
    wxMemoryDC memDC;
    // Tell the memory DC to use our Bitmap
    // all drawing action on the memory DC will go to the Bitmap now
    memDC.SelectObject(screenshot);
    // Blit (in this case copy) the actual screen on the memory DC
    // and thus the Bitmap
    memDC.Blit( 0, // Copy to this X coordinate
                0, // Copy to this Y coordinate
                width, // Copy this width
                height, // Copy this height
                &dcScreen, // From where do we copy?
                x, // What's the X offset in the original DC?
                y  // What's the Y offset in the original DC?
                    );
    // Select the Bitmap out of the memory DC by selecting a new
    // uninitialized Bitmap
    memDC.SelectObject(wxNullBitmap);

#endif // #ifdef __WXMAC__

    // Check if m_defaultDir already existed
    if (!wxDirExists(m_defaultDir))
        wxMkdir(m_defaultDir);

    // Our Bitmap now has the screenshot, so let's save it as an png
    // The filename itself is without extension.
    screenshot.SaveFile(m_defaultDir + wxFileName::GetPathSeparator() +
    fileName + _T(".png"), wxBITMAP_TYPE_PNG);
}

void CtrlMaskOut::Capture(wxRect rect, wxString fileName)
{
    wxPoint origin = rect.GetPosition();
    Capture(origin.x, origin.y, rect.GetWidth(), rect.GetHeight(), fileName);
}

void CtrlMaskOut::DetermineCtrlNameAndRect()
{
    // Detect windows using (n-1)*(n-1) points
    const int n = 5;

    wxPoint pt0 = m_currentRect.GetPosition();

    int dx = m_currentRect.GetWidth() / n;
    int dy = m_currentRect.GetHeight() / n;

    wxPoint pt;
    wxWindow * ctrl;
    wxString ctrlName;
    wxRect ctrlSize;

    // use a set to make sure the same control won't be inserted twice
    WX_DECLARE_HASH_SET(wxWindow*, wxPointerHash, wxPointerEqual, CtrlSet);

    CtrlSet ctrls;

    int i, j;
    for (i = 1; i <= n - 1; ++i)
    {
        for (j = 1; j <= n - 1; ++j)
        {
            pt = wxPoint(pt0.x + i * dx, pt0.y + j * dy);
            ctrls.insert(wxFindWindowAtPoint(pt));
        }
    }

    // Store the manual specified rect region, we might need it
    wxRect backup = m_currentRect;

    // Reset the control region and control name
    m_currentRect = wxRect(0, 0, 0, 0);
    m_controlName = _T("");

    for (CtrlSet::iterator it = ctrls.begin(); it != ctrls.end(); ++it )
    {
        ctrl = *it;
        if (ctrl == NULL)
        // which means no control was detected.
        // This seldom happens, but without dealing with it, it will CRASH the program
        {
            static int count = 0;
            ++count;
            m_controlName = _T("nothing_");
            m_controlName << count;
            m_currentRect = backup;
            return;
        }
        ctrlName = ctrl->GetClassInfo()->GetClassName();

        if (ctrlName != _T("wxPanel")) // Avoid detecting "wxPanel", which is only the backgroud
        {
            // Get a rect region to contain every window detected by unioning one by one.
            m_currentRect.Union(ctrl->GetScreenRect());

            // Get the name of the control, cut off "wx" and change them into lowercase.
            // e.g. wxButton will have a filename of "button" and be saved as "button.png"
            ctrlName.StartsWith(_T("wx"), &ctrlName);
            ctrlName.MakeLower();

            if (m_controlName.IsEmpty() || m_controlName == ctrlName)
            {
                m_controlName = ctrlName;
            }
            else // more than one types of controls are in the region selected
            {
                m_controlName += _T("_");
                m_controlName += ctrlName;
            }
        }
    }

    if (m_controlName.IsEmpty() && backup.GetHeight() > 15 && backup.GetWidth() > 15)
    // which means no control other than the wxPanel was detected.
    // if the size of the specified rect(backup) is smaller than 15x15,
    // then it could be just an accident
    {
        static int count = 0;
        ++count;
        m_controlName = _T("unknown_");
        m_controlName << count;
        m_currentRect = backup;
        return;
    }

    // Increase its border so that it looks nicer.
    m_currentRect.Inflate(m_inflateBorder);
}

void CtrlMaskOut::CreateMask(wxWindow* parent)
{
    if (m_mask != NULL)
        m_mask->Destroy();

    // Make a frame to visualize the process of specifying the rect region.
#if defined(__WXMSW__) || defined(__WXGTK__)
    m_mask = new wxFrame(parent, wxID_ANY, wxEmptyString,
                                m_currentRect.GetPosition(), m_currentRect.GetSize(),
                                wxFRAME_TOOL_WINDOW |
                                wxFRAME_FLOAT_ON_PARENT |
                                wxFRAME_NO_TASKBAR);
//                                 wxNO_BORDER);
// with the border, it looks better under windows

    m_mask->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));

#elif defined(__WXMAC__)
    // Using a miniframe with float and tool styles keeps the parent
    // frame activated and highlighted as such...
    m_mask = new wxMiniFrame(parent, wxID_ANY, wxEmptyString,
                                m_currentRect.GetPosition(), m_currentRect.GetSize(),
                                wxFRAME_FLOAT_ON_PARENT
                                | wxFRAME_TOOL_WINDOW );

    // Can't set the bg colour of a Frame in wxMac
    wxPanel* p = new wxPanel(m_mask);

    // The default wxSYS_COLOUR_ACTIVECAPTION colour is a light silver
    // color that is really hard to see, especially transparent.
    // Until a better system color is decided upon we'll just use
    // blue.
    p->SetBackgroundColour(*wxBLUE);

    // So that even if the cursor run into the mask, the events are still correctly processed.
    p->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( CtrlMaskOut::OnLeftButtonDown ), NULL, this);
    p->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( CtrlMaskOut::OnLeftButtonUp ), NULL, this);
    p->Connect( wxEVT_MOTION, wxMouseEventHandler( CtrlMaskOut::OnMouseMoving ), NULL, this);
#endif

    // If the platform doesn't support SetTransparent()
    // we will use a dummy mask(which does and shows nothing)
    // /this is the current trick, shall make it visible later
    if (!m_mask->CanSetTransparent())
    {
        m_mask->Destroy();

        m_mask = new wxPseudoTransparentFrame(parent,
                                    wxID_ANY,
                                    wxEmptyString,
                                    wxDefaultPosition,
                                    wxSize(1,1),
                                            wxFRAME_TOOL_WINDOW |
                                            wxFRAME_FLOAT_ON_PARENT |
                                            wxFRAME_NO_TASKBAR
                                            );

    }

    m_mask->SetTransparent(100); // Full value is 255
    m_mask->Show(true);

    // So that even if the cursor run into the mask, the events are still correctly processed.
    m_mask->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( CtrlMaskOut::OnLeftButtonDown ), NULL, this);
    m_mask->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( CtrlMaskOut::OnLeftButtonUp ), NULL, this);
    m_mask->Connect( wxEVT_MOTION, wxMouseEventHandler( CtrlMaskOut::OnMouseMoving ), NULL, this);
}

void CtrlMaskOut::DestroyMask()
{
    m_mask->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( CtrlMaskOut::OnLeftButtonDown ), NULL, this);
    m_mask->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( CtrlMaskOut::OnLeftButtonUp ), NULL, this);
    m_mask->Disconnect( wxEVT_MOTION, wxMouseEventHandler( CtrlMaskOut::OnMouseMoving ), NULL, this);
    wxWindow * parent = m_mask->GetParent();
//    m_mask->Destroy();
    delete m_mask;

    parent->Update();
    m_mask = NULL;
}

