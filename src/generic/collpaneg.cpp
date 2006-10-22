/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/collpaneg.cpp
// Purpose:     wxGenericCollapsiblePane
// Author:      Francesco Montorsi
// Modified By:
// Created:     8/10/2006
// Id:          $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/button.h"
#endif // !WX_PRECOMP

#include "wx/collpane.h"
#include "wx/statline.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the number of pixels to leave between the button and the static line and
// between the button and the pane
#define wxCP_MARGIN         10

// ============================================================================
// implementation
// ============================================================================

const wxChar wxGenericCollapsiblePaneNameStr[] = wxT("genericCollapsiblePane");

//-----------------------------------------------------------------------------
// wxGenericCollapsiblePane
//-----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_COMMAND_COLLPANE_CHANGED)
IMPLEMENT_DYNAMIC_CLASS(wxGenericCollapsiblePane, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxCollapsiblePaneEvent, wxCommandEvent)

BEGIN_EVENT_TABLE(wxGenericCollapsiblePane, wxControl)
    EVT_BUTTON(wxID_ANY, wxGenericCollapsiblePane::OnButton)
    EVT_SIZE(wxGenericCollapsiblePane::OnSize)
END_EVENT_TABLE()


bool wxGenericCollapsiblePane::Create(wxWindow *parent,
                                      wxWindowID id,
                                      const wxString& label,
                                      const wxPoint& pos,
                                      const wxSize& size,
                                      long style,
                                      const wxValidator& val,
                                      const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, val, name) )
        return false;

    m_strLabel = label;

    // create children; their size & position is set in OnSize()
    m_pButton = new wxButton(this, wxID_ANY, GetBtnLabel(), wxPoint(0, 0),
                             wxDefaultSize, wxBU_EXACTFIT);
    m_pStatLine = new wxStaticLine(this, wxID_ANY);
    m_pPane = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                           wxNO_BORDER);

    // start as collapsed:
    m_pPane->Hide();

    //CacheBestSize(GetBestSize());
    return true;
}

wxSize wxGenericCollapsiblePane::DoGetBestSize() const
{
    wxSize sz = m_pButton->GetBestSize();

    // set width
    sz.SetWidth(sz.x + wxCP_MARGIN + m_pStatLine->GetBestSize().x);
    const wxCoord paneWidth = m_pPane->GetBestSize().x;
    if ( sz.x < paneWidth )
        sz.x = paneWidth;

    // when expanded, we need more vertical space
    if ( IsExpanded() )
        sz.SetHeight(sz.y + wxCP_MARGIN + m_pPane->GetBestSize().y);

    return sz;
}

wxString wxGenericCollapsiblePane::GetBtnLabel() const
{
    return m_strLabel + (IsCollapsed() ? wxT(" >>") : wxT(" <<"));
}

void wxGenericCollapsiblePane::Collapse(bool collapse)
{
    // optimization
    if ( IsCollapsed() == collapse )
        return;

    // update our state
    m_pPane->Show(!collapse);

    // update button label
    // NB: this must be done after updating our "state"
    m_pButton->SetLabel(GetBtnLabel());

    // minimal size has priority over the best size so set here our min size
    wxSize sz = GetBestSize();
    SetMinSize(sz);
    SetSize(sz);

    wxWindow *top = wxGetTopLevelParent(this);
    if (top)
    {
        // we've changed our size, thus our top level parent needs to relayout
        // itself
        top->Layout();

        // FIXME: this makes wxGenericCollapsiblePane behave as the user expect
        //        but maybe there are cases where this is unwanted!
        if (top->GetSizer())
#ifdef __WXGTK__
        // FIXME: the SetSizeHints() call would be required also for GTK+ for
        //        the expanded->collapsed transition. Unfortunately if we
        //        enable this line, then the GTK+ top window won't always be
        //        resized by the SetClientSize() call below! As a side effect
        //        of this dirty fix, the minimal size for the pane window is
        //        not set in GTK+ and the user can hide it shrinking the "top"
        //        window...
        if (IsCollapsed())
#endif
            top->GetSizer()->SetSizeHints(top);

        if (IsCollapsed())
        {
            // use SetClientSize() and not SetSize() otherwise the size for
            // e.g. a wxFrame with a menubar wouldn't be correctly set
            top->SetClientSize(sz);
        }
        else
        {
            // force our parent to "fit", i.e. expand so that it can honour
            // our minimal size
            top->Fit();
        }
    }
}

void wxGenericCollapsiblePane::SetLabel(const wxString &label)
{
    m_strLabel = label;
    m_pButton->SetLabel(GetBtnLabel());
    m_pButton->SetBestFittingSize();

    LayoutChildren();
}

void wxGenericCollapsiblePane::LayoutChildren()
{
    wxSize btnSz = m_pButton->GetSize();

    // the button position & size are always ok...

    // move & resize the static line
    m_pStatLine->SetSize(btnSz.x + wxCP_MARGIN, btnSz.y/2,
                         GetSize().x - btnSz.x - wxCP_MARGIN, -1,
                         wxSIZE_USE_EXISTING);

    // move & resize the container window
    m_pPane->SetSize(0, btnSz.y + wxCP_MARGIN,
                     GetSize().x, GetSize().y - btnSz.y - wxCP_MARGIN);
}



//-----------------------------------------------------------------------------
// wxGenericCollapsiblePane - event handlers
//-----------------------------------------------------------------------------

void wxGenericCollapsiblePane::OnButton(wxCommandEvent& event)
{
    if ( event.GetEventObject() != m_pButton )
    {
        event.Skip();
        return;
    }

    Collapse(!IsCollapsed());

    // this change was generated by the user - send the event
    wxCollapsiblePaneEvent ev(this, GetId(), IsCollapsed());
    GetEventHandler()->ProcessEvent(ev);
}

void wxGenericCollapsiblePane::OnSize(wxSizeEvent& WXUNUSED(event))
{
#if 0       // for debug only
    wxClientDC dc(this);
    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(wxPoint(0,0), GetSize());
    dc.SetPen(*wxRED_PEN);
    dc.DrawRectangle(wxPoint(0,0), GetBestSize());
#endif


    if (!m_pButton || !m_pStatLine || !m_pPane)
        return;     // we need to complete the creation first!

    LayoutChildren();

    // this is very important to make the pane window layout show correctly
    m_pPane->Layout();
}

