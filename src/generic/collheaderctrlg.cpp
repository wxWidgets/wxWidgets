/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/collheaderctrlg.cpp
// Purpose:     Generic wxCollapsibleHeaderCtrl implementation
// Author:      Tobias Taschner
// Created:     2015-09-19
// Copyright:   (c) 2015 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#include "wx/defs.h"

#if wxUSE_COLLPANE

#include "wx/collheaderctrl.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/sizer.h"
    #include "wx/intl.h"
#endif // !WX_PRECOMP

#include "wx/renderer.h"

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif // __WXMSW__

// if we have another implementation of this class we should extract
// the lines below to a common file

const char wxCollapsibleHeaderCtrlNameStr[] = "collapsibleHeader";

wxDEFINE_EVENT(wxEVT_COLLAPSIBLEHEADER_CHANGED, wxCommandEvent);

#if wxUSE_ACCESSIBILITY

class wxCollapsibleHeaderCtrlAccessible : public wxWindowAccessible
{
public:
    explicit wxCollapsibleHeaderCtrlAccessible(wxGenericCollapsibleHeaderCtrl* win)
        : wxWindowAccessible(win)
    {
    }

    virtual wxAccStatus GetRole(int childId, wxAccRole* role) override
    {
        if ( childId != wxACC_SELF )
            return wxACC_NOT_IMPLEMENTED;

        *role = wxROLE_SYSTEM_PUSHBUTTON;
        return wxACC_OK;
    }

    virtual wxAccStatus GetState(int childId, long* state) override
    {
        wxGenericCollapsibleHeaderCtrl* const header = GetHeader();
        wxCHECK( header, wxACC_FAIL );

        if ( childId != wxACC_SELF )
            return wxACC_NOT_IMPLEMENTED;

        long st = header->IsCollapsed() ? wxACC_STATE_SYSTEM_COLLAPSED
                                        : wxACC_STATE_SYSTEM_EXPANDED;
        if ( !header->IsEnabled() )
            st |= wxACC_STATE_SYSTEM_UNAVAILABLE;
        if ( !header->IsShownOnScreen() )
            st |= wxACC_STATE_SYSTEM_INVISIBLE;
        if ( header->IsFocusable() )
            st |= wxACC_STATE_SYSTEM_FOCUSABLE;
        if ( header->HasFocus() )
            st |= wxACC_STATE_SYSTEM_FOCUSED;

        *state = st;
        return wxACC_OK;
    }

    virtual wxAccStatus GetDefaultAction(int childId, wxString* actionName) override
    {
        wxGenericCollapsibleHeaderCtrl* const header = GetHeader();
        wxCHECK( header, wxACC_FAIL );

        if ( childId != wxACC_SELF )
            return wxACC_NOT_IMPLEMENTED;

        *actionName = header->IsCollapsed() ? _("Expand") : _("Collapse");
        return wxACC_OK;
    }

    virtual wxAccStatus DoDefaultAction(int childId) override
    {
        wxGenericCollapsibleHeaderCtrl* const header = GetHeader();
        wxCHECK( header, wxACC_FAIL );

        if ( childId != wxACC_SELF )
            return wxACC_NOT_IMPLEMENTED;

        header->DoSetCollapsed(!header->IsCollapsed());
        return wxACC_OK;
    }

private:
    wxGenericCollapsibleHeaderCtrl* GetHeader()
    {
        return static_cast<wxGenericCollapsibleHeaderCtrl*>(GetWindow());
    }
};

#endif // wxUSE_ACCESSIBILITY

// ============================================================================
// implementation
// ============================================================================

void wxGenericCollapsibleHeaderCtrl::Init()
{
    m_collapsed = true;
    m_inWindow = false;
    m_mouseDown = false;
}

bool wxGenericCollapsibleHeaderCtrl::Create(wxWindow *parent,
    wxWindowID id,
    const wxString& label,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name)
{
    if ( !wxCollapsibleHeaderCtrlBase::Create(parent, id, label, pos, size,
                                              style | wxWANTS_CHARS,
                                              validator, name) )
    {
        return false;
    }

    Bind(wxEVT_PAINT, &wxGenericCollapsibleHeaderCtrl::OnPaint, this);
    Bind(wxEVT_LEFT_DOWN, &wxGenericCollapsibleHeaderCtrl::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &wxGenericCollapsibleHeaderCtrl::OnLeftUp, this);
    Bind(wxEVT_ENTER_WINDOW, &wxGenericCollapsibleHeaderCtrl::OnEnterWindow, this);
    Bind(wxEVT_LEAVE_WINDOW, &wxGenericCollapsibleHeaderCtrl::OnLeaveWindow, this);
    Bind(wxEVT_CHAR, &wxGenericCollapsibleHeaderCtrl::OnChar, this);
    Bind(wxEVT_SET_FOCUS, &wxGenericCollapsibleHeaderCtrl::OnFocus, this);
    Bind(wxEVT_KILL_FOCUS, &wxGenericCollapsibleHeaderCtrl::OnFocus, this);

    return true;
}

wxSize wxGenericCollapsibleHeaderCtrl::DoGetBestClientSize() const
{
    wxGenericCollapsibleHeaderCtrl* const
        self = const_cast<wxGenericCollapsibleHeaderCtrl*>(this);

    // The code here parallels that of OnPaint() -- except without drawing.
    wxInfoDC dc(self);

    wxSize size = wxRendererNative::Get().GetCollapseButtonSize(self, dc);

    wxString text;
    wxControl::FindAccelIndex(GetLabel(), &text);

    const wxSize textSize = dc.GetTextExtent(text);

    size.x += FromDIP(2) + textSize.x;
    if ( textSize.y > size.y )
        size.y = textSize.y;

#ifdef __WXMSW__
    size.IncBy(wxGetSystemMetrics(SM_CXFOCUSBORDER, this),
               wxGetSystemMetrics(SM_CYFOCUSBORDER, this));
#endif // __WXMSW__

    return size;
}

void wxGenericCollapsibleHeaderCtrl::SetCollapsed(bool collapsed)
{
    m_collapsed = collapsed;
    Refresh();

#if wxUSE_ACCESSIBILITY
    wxAccessible::NotifyEvent(wxACC_EVENT_OBJECT_STATECHANGE, this,
                              wxOBJID_CLIENT, wxACC_SELF);
#endif // wxUSE_ACCESSIBILITY
}

void wxGenericCollapsibleHeaderCtrl::DoSetCollapsed(bool collapsed)
{
    SetCollapsed(collapsed);

    wxCommandEvent evt(wxEVT_COLLAPSIBLEHEADER_CHANGED, GetId());
    evt.SetEventObject(this);
    ProcessEvent(evt);
}

#if wxUSE_ACCESSIBILITY

wxAccessible* wxGenericCollapsibleHeaderCtrl::CreateAccessible()
{
    return new wxCollapsibleHeaderCtrlAccessible(this);
}

#endif // wxUSE_ACCESSIBILITY

void wxGenericCollapsibleHeaderCtrl::OnFocus(wxFocusEvent& event)
{
    Refresh();
    event.Skip();
}

void wxGenericCollapsibleHeaderCtrl::OnChar(wxKeyEvent& event)
{
    switch (event.GetKeyCode())
    {
    case WXK_SPACE:
    case WXK_RETURN:
        DoSetCollapsed(!m_collapsed);
        break;
    default:
        if ( !HandleAsNavigationKey(event) )
            event.Skip();
        break;
    }
}

void wxGenericCollapsibleHeaderCtrl::OnEnterWindow(wxMouseEvent& event)
{
    m_inWindow = true;
    Refresh();
    event.Skip();
}

void wxGenericCollapsibleHeaderCtrl::OnLeaveWindow(wxMouseEvent& event)
{
    m_inWindow = false;
    Refresh();
    event.Skip();
}

void wxGenericCollapsibleHeaderCtrl::OnLeftUp(wxMouseEvent& event)
{
    m_mouseDown = false;
    DoSetCollapsed(!m_collapsed);
    event.Skip();
}

void wxGenericCollapsibleHeaderCtrl::OnLeftDown(wxMouseEvent& event)
{
    m_mouseDown = true;
    Refresh();
    event.Skip();
}

void wxGenericCollapsibleHeaderCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);

    wxRect rect(wxPoint(0, 0), GetClientSize());

    wxSize btnSize = wxRendererNative::Get().GetCollapseButtonSize(this, dc);

    wxRect btnRect(wxPoint(0, 0), btnSize);
    btnRect = btnRect.CenterIn(rect, wxVERTICAL);

    int flags = 0;

    if ( m_inWindow )
        flags |= wxCONTROL_CURRENT;

    if ( m_mouseDown )
        flags |= wxCONTROL_PRESSED;

    if ( !m_collapsed )
        flags |= wxCONTROL_EXPANDED;

    wxRendererNative::Get().DrawCollapseButton(this, dc, btnRect, flags);

    wxString text;
    int indexAccel = wxControl::FindAccelIndex(GetLabel(), &text);

    wxSize textSize = dc.GetTextExtent(text);

    wxRect textRect(wxPoint(btnSize.x + FromDIP(2), 0), textSize);
    textRect = textRect.CenterIn(rect, wxVERTICAL);

    dc.DrawLabel(text, textRect, wxALIGN_CENTRE_VERTICAL, indexAccel);

#ifdef __WXMSW__
    if ( HasFocus() )
        wxRendererNative::Get().DrawFocusRect(this, dc, textRect.Inflate(1), flags);
#endif
}


#endif // wxUSE_COLLPANE
