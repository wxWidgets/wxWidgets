/////////////////////////////////////////////////////////////////////////////
// Name:        src/univ/toolbar.cpp
// Purpose:     implementation of wxToolBar for wxUniversal
// Author:      Robert Roebling, Vadim Zeitlin (universalization)
// Modified by:
// Created:     20.02.02
// Id:          $Id$
// Copyright:   (c) 2001 Robert Roebling,
//              (c) 2002 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univtoolbar.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/app.h"

    #include "wx/univ/renderer.h"
#endif

#include "wx/toolbar.h"
#include "wx/image.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// value meaning that m_widthSeparator is not initialized
static const wxCoord INVALID_WIDTH = -1;

// ----------------------------------------------------------------------------
// wxToolBarTool: our implementation of wxToolBarToolBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxToolBarTool : public wxToolBarToolBase
{
public:
    wxToolBarTool( wxToolBarBase *tbar = (wxToolBarBase *)NULL,
                   int id = wxID_SEPARATOR,
                   const wxBitmap& bitmap1 = wxNullBitmap,
                   const wxBitmap& bitmap2 = wxNullBitmap,
                   bool toggle = FALSE,
                   wxObject *clientData = (wxObject *) NULL,
                   const wxString& shortHelpString = wxEmptyString,
                   const wxString& longHelpString = wxEmptyString )
        : wxToolBarToolBase(tbar, id, bitmap1, bitmap2, toggle, clientData,
                            shortHelpString, longHelpString)
    {
        // no position yet
        m_x =
        m_y = -1;

        // not pressed yet
        m_isInverted = FALSE;
    }

    // is this tool pressed, even temporarily? (this is different from being
    // permanently toggled which is what IsToggled() returns)
    bool IsPressed() const
        { return CanBeToggled() ? IsToggled() != m_isInverted : m_isInverted; }

    // are we temporarily pressed/unpressed?
    bool IsInverted() const { return m_isInverted; }

    // press the tool temporarily by inverting its toggle state
    void Invert() { m_isInverted = !m_isInverted; }

public:
    // the tool position (the size is known by the toolbar itself)
    int m_x,
        m_y;

private:
    // TRUE if the tool is pressed
    bool m_isInverted;
};

// ============================================================================
// wxToolBar implementation
// ============================================================================

IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxControl);

// ----------------------------------------------------------------------------
// wxToolBar creation
// ----------------------------------------------------------------------------

void wxToolBar::Init()
{
    // no tools yet
    m_needsLayout = FALSE;

    // unknown widths for the tools and separators
    m_widthSeparator = INVALID_WIDTH;

    m_maxWidth =
    m_maxHeight = 0;

    m_toolPressed = NULL;
    m_toolCurrent = NULL;

    wxRenderer *renderer = GetRenderer();

    SetToolBitmapSize(renderer->GetToolBarButtonSize(&m_widthSeparator));
    SetMargins(renderer->GetToolBarMargin());
}

bool wxToolBar::Create(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
    if ( !wxToolBarBase::Create(parent, id, pos, size, style,
                                wxDefaultValidator, name) )
    {
        return FALSE;
    }

    CreateInputHandler(wxINP_HANDLER_TOOLBAR);

    SetBestSize(size);

    return TRUE;
}

wxToolBar::~wxToolBar()
{
}

// ----------------------------------------------------------------------------
// wxToolBar tool-related methods
// ----------------------------------------------------------------------------

wxToolBarToolBase *wxToolBar::FindToolForPosition(wxCoord x, wxCoord y) const
{
    // check the "other" direction first: it must be inside the toolbar or we
    // don't risk finding anything
    if ( IsVertical() )
    {
        if ( x < 0 || x > m_maxWidth )
            return NULL;

        // we always use x, even for a vertical toolbar, this makes the code
        // below simpler
        x = y;
    }
    else // horizontal
    {
        if ( y < 0 || y > m_maxHeight )
            return NULL;
    }

    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarToolBase *tool =  node->GetData();
        wxRect rectTool = GetToolRect(tool);

        wxCoord startTool, endTool;
        GetRectLimits(rectTool, &startTool, &endTool);

        if ( x >= startTool && x <= endTool )
        {
            // don't return the separators from here, they don't accept any
            // input anyhow
            return tool->IsSeparator() ? NULL : tool;
        }
    }

    return NULL;
}

void wxToolBar::SetToolShortHelp(int id, const wxString& help)
{
    wxToolBarToolBase *tool = FindById(id);

    wxCHECK_RET( tool, _T("SetToolShortHelp: no such tool") );

    tool->SetShortHelp(help);
}

bool wxToolBar::DoInsertTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase * WXUNUSED(tool))
{
    // recalculate the toolbar geometry before redrawing it the next time
    m_needsLayout = TRUE;

    // and ensure that we indeed are going to redraw
    Refresh();

    return TRUE;
}

bool wxToolBar::DoDeleteTool(size_t WXUNUSED(pos),
                             wxToolBarToolBase * WXUNUSED(tool))
{
    // as above
    m_needsLayout = TRUE;

    Refresh();

    return TRUE;
}

void wxToolBar::DoEnableTool(wxToolBarToolBase *tool, bool enable)
{
    // created disabled-state bitmap on demand
    if ( !enable && !tool->GetDisabledBitmap().Ok() )
    {
        wxImage image( tool->GetNormalBitmap() );

        // TODO: don't hardcode 180
        unsigned char bg_red = 180;
        unsigned char bg_green = 180;
        unsigned char bg_blue = 180;

        unsigned char mask_red = image.GetMaskRed();
        unsigned char mask_green = image.GetMaskGreen();
        unsigned char mask_blue = image.GetMaskBlue();

        bool has_mask = image.HasMask();

        int x,y;
        for (y = 0; y < image.GetHeight(); y++)
        {
            for (x = 0; x < image.GetWidth(); x++)
            {
                unsigned char red = image.GetRed(x,y);
                unsigned char green = image.GetGreen(x,y);
                unsigned char blue = image.GetBlue(x,y);
                if (!has_mask || red != mask_red || green != mask_green || blue != mask_blue)
                {
                    red = (((wxInt32) red  - bg_red) >> 1) + bg_red;
                    green = (((wxInt32) green  - bg_green) >> 1) + bg_green;
                    blue = (((wxInt32) blue  - bg_blue) >> 1) + bg_blue;
                    image.SetRGB( x, y, red, green, blue );
                }
            }
        }

        for (y = 0; y < image.GetHeight(); y++)
        {
            for (x = y % 2; x < image.GetWidth(); x += 2)
            {
                unsigned char red = image.GetRed(x,y);
                unsigned char green = image.GetGreen(x,y);
                unsigned char blue = image.GetBlue(x,y);
                if (!has_mask || red != mask_red || green != mask_green || blue != mask_blue)
                {
                    red = (((wxInt32) red  - bg_red) >> 1) + bg_red;
                    green = (((wxInt32) green  - bg_green) >> 1) + bg_green;
                    blue = (((wxInt32) blue  - bg_blue) >> 1) + bg_blue;
                    image.SetRGB( x, y, red, green, blue );
                }
            }
        }

        tool->SetDisabledBitmap( image.ConvertToBitmap() );
    }

    RefreshTool(tool);
}

void wxToolBar::DoToggleTool(wxToolBarToolBase *tool, bool WXUNUSED(toggle))
{
    // note that if we're called the tool did change state (the base class
    // checks for it), so it's not necessary to check for this again here
    RefreshTool(tool);
}

void wxToolBar::DoSetToggle(wxToolBarToolBase *tool, bool WXUNUSED(toggle))
{
    RefreshTool(tool);
}

wxToolBarToolBase *wxToolBar::CreateTool(int id,
                                         const wxBitmap& bitmap1,
                                         const wxBitmap& bitmap2,
                                         bool toggle,
                                         wxObject *clientData,
                                         const wxString& shortHelpString,
                                         const wxString& longHelpString)
{
    return new wxToolBarTool( this, id, bitmap1, bitmap2, toggle,
                              clientData, shortHelpString, longHelpString);
}

wxToolBarToolBase *wxToolBar::CreateTool(wxControl *control)
{
    wxFAIL_MSG( wxT("Toolbar doesn't support controls yet (TODO)") );

    return NULL;
}

// ----------------------------------------------------------------------------
// wxToolBar geometry
// ----------------------------------------------------------------------------

wxRect wxToolBar::GetToolRect(wxToolBarToolBase *toolBase) const
{
    const wxToolBarTool *tool = (wxToolBarTool *)toolBase;

    wxRect rect;

    wxCHECK_MSG( tool, rect, _T("GetToolRect: NULL tool") );

    // ensure that we always have the valid tool position
    if ( m_needsLayout )
    {
        wxConstCast(this, wxToolBar)->DoLayout();
    }

    rect.x = tool->m_x - m_xMargin;
    rect.y = tool->m_y - m_yMargin;

    if ( IsVertical() )
    {
        rect.width = m_defaultWidth;
        rect.height = tool->IsSeparator() ? m_widthSeparator : m_defaultHeight;
    }
    else // horizontal
    {
        rect.width = tool->IsSeparator() ? m_widthSeparator : m_defaultWidth;
        rect.height = m_defaultHeight;
    }

    rect.width += 2*m_xMargin;
    rect.height += 2*m_yMargin;

    return rect;
}

bool wxToolBar::Realize()
{
    if ( !wxToolBarBase::Realize() )
        return FALSE;

    m_needsLayout = TRUE;
    DoLayout();

    SetBestSize(wxDefaultSize);

    return TRUE;
}

void wxToolBar::DoLayout()
{
    wxASSERT_MSG( m_needsLayout, _T("why are we called?") );

    m_needsLayout = FALSE;

    wxCoord x = m_xMargin,
            y = m_yMargin;

    const wxCoord widthTool = IsVertical() ? m_defaultHeight : m_defaultWidth;
    wxCoord margin = IsVertical() ? m_xMargin : m_yMargin,
           *pCur = IsVertical() ? &y : &x;

    // calculate the positions of all elements
    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarTool *tool = (wxToolBarTool *) node->GetData();

        tool->m_x = x;
        tool->m_y = y;

        *pCur += (tool->IsSeparator() ? m_widthSeparator : widthTool) + margin;
    }

    // calculate the total toolbar size
    wxCoord xMin = m_defaultWidth + 2*m_xMargin,
            yMin = m_defaultHeight + 2*m_yMargin;

    m_maxWidth = x < xMin ? xMin : x;
    m_maxHeight = y < yMin ? yMin : y;
}

wxSize wxToolBar::DoGetBestClientSize() const
{
    return wxSize(m_maxWidth, m_maxHeight);
}

// ----------------------------------------------------------------------------
// wxToolBar drawing
// ----------------------------------------------------------------------------

void wxToolBar::RefreshTool(wxToolBarToolBase *tool)
{
    RefreshRect(GetToolRect(tool));
}

void wxToolBar::GetRectLimits(const wxRect& rect,
                              wxCoord *start,
                              wxCoord *end) const
{
    wxCHECK_RET( start && end, _T("NULL pointer in GetRectLimits") );

    if ( IsVertical() )
    {
        *start = rect.GetTop();
        *end = rect.GetBottom();
    }
    else // horizontal
    {
        *start = rect.GetLeft();
        *end = rect.GetRight();
    }
}

void wxToolBar::DoDraw(wxControlRenderer *renderer)
{
    // prepare the variables used below
    wxDC& dc = renderer->GetDC();
    wxRenderer *rend = renderer->GetRenderer();
    // dc.SetFont(GetFont()); -- uncomment when we support labels

    // draw the border separating us from the menubar (if there is no menubar
    // we probably shouldn't draw it?)
    if ( !IsVertical() )
    {
        rend->DrawHorizontalLine(dc, 0, 0, GetClientSize().x);
    }

    // get the update rect and its limits depending on the orientation
    wxRect rectUpdate = GetUpdateClientRect();
    wxCoord start, end;
    GetRectLimits(rectUpdate, &start, &end);

    // and redraw all the tools intersecting it
    for ( wxToolBarToolsList::Node *node = m_tools.GetFirst();
          node;
          node = node->GetNext() )
    {
        wxToolBarToolBase *tool = node->GetData();
        wxRect rectTool = GetToolRect(tool);
        wxCoord startTool, endTool;
        GetRectLimits(rectTool, &startTool, &endTool);

        if ( endTool < start )
        {
            // we're still to the left of the area to redraw
            continue;
        }

        if ( startTool > end )
        {
            // we're beyond the area to redraw, nothing left to do
            break;
        }

        // deal with the flags
        int flags = 0;

        if ( tool->IsEnabled() )
        {
            // the toolbars without wxTB_FLAT don't react to the mouse hovering
            if ( HasFlag(wxTB_FLAT) && (tool == m_toolCurrent) )
                flags |= wxCONTROL_CURRENT;
        }
        else // disabled tool
        {
            flags |= wxCONTROL_DISABLED;
        }

        if ( tool == m_toolPressed )
            flags |= wxCONTROL_FOCUSED;

        if ( ((wxToolBarTool *)tool)->IsPressed() )
            flags |= wxCONTROL_PRESSED;

        wxString label;
        wxBitmap bitmap;
        if ( !tool->IsSeparator() )
        {
            label = tool->GetLabel();
            bitmap = tool->GetBitmap();
        }
        //else: leave both the label and the bitmap invalid to draw a separator

        rend->DrawToolBarButton(dc, label, bitmap, rectTool, flags);
    }
}

// ----------------------------------------------------------------------------
// wxToolBar actions
// ----------------------------------------------------------------------------

void wxToolBar::Press()
{
    wxCHECK_RET( m_toolCurrent, _T("no tool to press?") );

    wxLogTrace(_T("toolbar"),
               _T("Button '%s' pressed."),
               m_toolCurrent->GetShortHelp().c_str());

    // this is the tool whose state is going to change
    m_toolPressed = (wxToolBarTool *)m_toolCurrent;

    // we must toggle it regardless of whether it is a checkable tool or not,
    // so use Invert() and not Toggle() here
    m_toolPressed->Invert();

    RefreshTool(m_toolPressed);
}

void wxToolBar::Release()
{
    wxCHECK_RET( m_toolPressed, _T("no tool to release?") );

    wxLogTrace(_T("toolbar"),
               _T("Button '%s' released."),
               m_toolCurrent->GetShortHelp().c_str());

    wxASSERT_MSG( m_toolPressed->IsInverted(), _T("release unpressed button?") );

    m_toolPressed->Invert();

    RefreshTool(m_toolPressed);
}

void wxToolBar::Toggle()
{
    m_toolCurrent = m_toolPressed;

    Release();

    Click();
}

void wxToolBar::Click()
{
    wxCHECK_RET( m_toolCurrent, _T("no tool to click?") );

    bool isToggled;
    if ( m_toolCurrent->CanBeToggled() )
    {
        m_toolCurrent->Toggle();

        RefreshTool(m_toolCurrent);

        isToggled = m_toolCurrent->IsToggled();
    }
    else // simple non-checkable tool
    {
        isToggled = FALSE;
    }

    OnLeftClick(m_toolCurrent->GetId(), isToggled);
}

bool wxToolBar::PerformAction(const wxControlAction& action,
                              long numArg,
                              const wxString& strArg)
{
    if ( action == wxACTION_TOOLBAR_TOGGLE )
        Toggle();
    else if ( action == wxACTION_TOOLBAR_PRESS )
        Press();
    else if ( action == wxACTION_TOOLBAR_RELEASE )
        Release();
    else if ( action == wxACTION_TOOLBAR_CLICK )
        Click();
    else if ( action == wxACTION_TOOLBAR_ENTER )
    {
        wxToolBarToolBase *toolCurrentOld = m_toolCurrent;
        m_toolCurrent = FindById((int)numArg);

        if ( m_toolCurrent != toolCurrentOld )
        {
            // the appearance of the current tool only changes for the flat
            // toolbars
            if ( HasFlag(wxTB_FLAT) )
            {
                // and only if the tool was/is enabled
                if ( toolCurrentOld && toolCurrentOld->IsEnabled() )
                    RefreshTool(toolCurrentOld);

                if ( m_toolCurrent )
                {
                    if ( m_toolCurrent->IsEnabled() )
                        RefreshTool(m_toolCurrent);
                }
                else
                {
                    wxFAIL_MSG( _T("no current tool in wxACTION_TOOLBAR_ENTER?") );
                }
            }
        }
    }
    else if ( action == wxACTION_TOOLBAR_LEAVE )
    {
        if ( m_toolCurrent )
        {
            wxToolBarToolBase *toolCurrentOld = m_toolCurrent;
            m_toolCurrent = NULL;

            RefreshTool(toolCurrentOld);
        }
    }
    else
        return wxControl::PerformAction(action, numArg, strArg);

    return TRUE;
}

// ============================================================================
// wxStdToolbarInputHandler implementation
// ============================================================================

wxStdToolbarInputHandler::wxStdToolbarInputHandler(wxInputHandler *handler)
                        : wxStdButtonInputHandler(handler)
{
}

bool wxStdToolbarInputHandler::HandleKey(wxInputConsumer *consumer,
                                         const wxKeyEvent& event,
                                         bool pressed)
{
    // TODO: when we have a current button we should allow the arrow
    //       keys to move it
    return wxStdInputHandler::HandleKey(consumer, event, pressed);
}

bool wxStdToolbarInputHandler::HandleMouse(wxInputConsumer *consumer,
                                           const wxMouseEvent& event)
{
    // don't let the base class press the disabled buttons but simply ignore
    // all events on them
    wxToolBar *tbar = wxStaticCast(consumer->GetInputWindow(), wxToolBar);
    wxToolBarToolBase *tool = tbar->FindToolForPosition(event.GetX(), event.GetY());

    if ( tool && !tool->IsEnabled() )
        return TRUE;

    return wxStdButtonInputHandler::HandleMouse(consumer, event);
}

bool wxStdToolbarInputHandler::HandleMouseMove(wxInputConsumer *consumer,
                                               const wxMouseEvent& event)
{
    if ( !wxStdButtonInputHandler::HandleMouseMove(consumer, event) )
    {
        wxToolBarToolBase *tool;

        if ( event.Leaving() )
        {
            tool = NULL;
        }
        else
        {
            wxToolBar *tbar = wxStaticCast(consumer->GetInputWindow(), wxToolBar);
            tool = tbar->FindToolForPosition(event.GetX(), event.GetY());
        }

        if ( tool )
            consumer->PerformAction(wxACTION_TOOLBAR_ENTER, tool->GetId());
        else
            consumer->PerformAction(wxACTION_TOOLBAR_LEAVE);

        return TRUE;
    }

    return FALSE;
}

bool wxStdToolbarInputHandler::HandleFocus(wxInputConsumer *consumer,
                                           const wxFocusEvent& event)
{
    // we shouldn't be left with a highlighted button
    consumer->PerformAction(wxACTION_TOOLBAR_LEAVE);

    return TRUE;
}

bool wxStdToolbarInputHandler::HandleActivation(wxInputConsumer *consumer,
                                                bool activated)
{
    // as above
    if ( !activated )
        consumer->PerformAction(wxACTION_TOOLBAR_LEAVE);

    return TRUE;
}

