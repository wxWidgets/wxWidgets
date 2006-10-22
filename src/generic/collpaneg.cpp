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
#include "wx/collpane.h"
#include "wx/statline.h"

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
    EVT_BUTTON(wxCP_BUTTON_ID, wxGenericCollapsiblePane::OnButton)
    EVT_SIZE(wxGenericCollapsiblePane::OnSize)
END_EVENT_TABLE()


bool wxGenericCollapsiblePane::Create( wxWindow *parent, wxWindowID id,
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
    m_pButton = new wxButton(this, wxCP_BUTTON_ID, GetBtnLabel(), wxPoint(0, 0),
                             wxDefaultSize, wxBU_EXACTFIT);
    m_pStatLine = new wxStaticLine(this, wxID_ANY);
    m_pPane = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

    // start as collapsed:
    m_pPane->Hide();

    //CacheBestSize(GetBestSize());
    return true;
}

wxSize wxGenericCollapsiblePane::DoGetBestSize() const
{
    wxSize sz = m_pButton->GetBestSize();

    // set width
    sz.SetWidth( sz.GetWidth() + wxCP_MARGIN + m_pStatLine->GetBestSize().GetWidth() );
    sz.SetWidth( wxMax(sz.GetWidth(), m_pPane->GetBestSize().GetWidth()) );

    // when expanded, we need more vertical space
    if (!IsCollapsed())
        sz.SetHeight( sz.GetHeight() + wxCP_MARGIN + m_pPane->GetBestSize().GetHeight() );

    return sz;
}

wxString wxGenericCollapsiblePane::GetBtnLabel() const
{
    if (IsCollapsed())
        return m_strLabel + wxT(" >>");
    return m_strLabel + wxT(" <<");
}

void wxGenericCollapsiblePane::Collapse(bool collapse)
{
    // optimization
    if (IsCollapsed() == collapse)
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

    wxWindow *top = GetTopLevelParent();
    if (top)
    {
        // we've changed our size, thus our top level parent needs to relayout itself
        top->Layout();

        // FIXME: this makes wxGenericCollapsiblePane behave as the user expect but
        //        maybe there are cases where this is unwanted!
        if (top->GetSizer())
#ifdef __WXGTK__
        // FIXME: the SetSizeHints() call would be required also for GTK+ for the
        //        expanded->collapsed transition.
        //        Unfortunately if we enable this line, then the GTK+ top window
        //        won't always be resized by the SetClientSize() call below!
        //        As a side effect of this dirty fix, the minimal size for the
        //        pane window is not set in GTK+ and the user can hide it shrinking
        //        the "top" window...
        if (IsCollapsed())
#endif
            top->GetSizer()->SetSizeHints(top);

        if (IsCollapsed())
        {
            // NB: we need to use SetClientSize() and not SetSize() otherwise the size for
            //     windows like e.g. wxFrames with wxMenubars won't be correctly set
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

wxWindow *wxGenericCollapsiblePane::GetTopLevelParent()
{
    wxWindow *parent = GetParent();
    while (parent && !parent->IsTopLevel())
        parent = parent->GetParent();

    return parent;
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
    m_pStatLine->SetSize(btnSz.GetWidth() + wxCP_MARGIN, btnSz.GetHeight()/2,
                         GetSize().GetWidth() - btnSz.GetWidth() - wxCP_MARGIN, -1,
                         wxSIZE_USE_EXISTING);

    // move & resize the container window
    m_pPane->SetSize(0, btnSz.GetHeight() + wxCP_MARGIN,
                     GetSize().GetWidth(), GetSize().GetHeight() - btnSz.GetHeight() - wxCP_MARGIN);
}



//-----------------------------------------------------------------------------
// wxGenericCollapsiblePane - event handlers
//-----------------------------------------------------------------------------

void wxGenericCollapsiblePane::OnButton(wxCommandEvent &WXUNUSED(event))
{
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

