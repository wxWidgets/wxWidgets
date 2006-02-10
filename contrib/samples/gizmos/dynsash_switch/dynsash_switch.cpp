/////////////////////////////////////////////////////////////////////////////
// Name:        contrib/samples/gizmos/dynsash_switch/dynsash_switch.cpp
// Purpose:     Test custom scrollbar handling of wxDynamicSashWindow by
//              creating a dynamic sash window where the client scrolls a
//              a subwindow of the client window by responding to scroll
//              events itself
// Author:      Matt Kimball
// Modified by:
// Created:     7/15/2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Matt Kimball
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/app.h"
#include "wx/frame.h"
#include "wx/choice.h"
#include "wx/dcclient.h"
#include "wx/gizmos/dynamicsash.h"
#include "wx/layout.h"
#include "wx/scrolbar.h"

class SwitchDemo : public wxApp {
public:
    bool OnInit();
};


class SwitchView : public wxWindow {
public:
    SwitchView(wxDynamicSashWindow *parent);

    wxSize DoGetBestSize() const;

private:
    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnChoice(wxCommandEvent& event);
    void OnScroll(wxScrollEvent& event);
    void OnFocus(wxFocusEvent& event);
    void OnErase(wxEraseEvent& event);
    void OnSplit(wxDynamicSashSplitEvent& event);
    void OnUnify(wxDynamicSashUnifyEvent& event);

    wxDynamicSashWindow *m_dyn_sash;
    wxWindow *m_bar;
    wxChoice *m_choice;
    wxWindow *m_view;
};

IMPLEMENT_APP(SwitchDemo)


SwitchView::SwitchView(wxDynamicSashWindow *win) {
    Create(win, wxID_ANY);

    m_dyn_sash = win;

    m_bar = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxRAISED_BORDER, wxT("bar"));
    m_choice = new wxChoice(m_bar, wxID_ANY);
    m_choice->SetEventHandler(this);
    m_view = new wxWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, wxT("view"));
    m_view->SetBackgroundColour(*wxWHITE);
    m_view->SetEventHandler(this);

    m_choice->Append(wxT("Triangle"));
    m_choice->Append(wxT("Square"));
    m_choice->SetSelection(0);

    wxLayoutConstraints *layout;

    layout = new wxLayoutConstraints();
    layout->left.Absolute(0);
    layout->top.Absolute(0);
    layout->height.Absolute(36);
    layout->width.SameAs(this, wxWidth);
    m_bar->SetConstraints(layout);

    layout = new wxLayoutConstraints();
    layout->left.Absolute(3);
    layout->width.AsIs();
    layout->height.AsIs();
    layout->top.Absolute(3);
    m_choice->SetConstraints(layout);

    layout = new wxLayoutConstraints();
    layout->left.Absolute(0);
    layout->width.SameAs(this, wxWidth);
    layout->top.Below(m_bar);
    layout->bottom.SameAs(this, wxBottom);
    m_view->SetConstraints(layout);

    wxScrollBar *hscroll = m_dyn_sash->GetHScrollBar(this);
    wxScrollBar *vscroll = m_dyn_sash->GetVScrollBar(this);

    hscroll->SetEventHandler(this);
    vscroll->SetEventHandler(this);

    Connect(GetId(), wxEVT_SIZE, (wxObjectEventFunction)
                                 (wxEventFunction)
                                 (wxSizeEventFunction)&SwitchView::OnSize);
    Connect(m_choice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, (wxObjectEventFunction)
                                                              (wxEventFunction)
                                                              (wxCommandEventFunction)&SwitchView::OnChoice);
    Connect(m_view->GetId(), wxEVT_PAINT, (wxObjectEventFunction)
                                          (wxEventFunction)
                                          (wxPaintEventFunction)&SwitchView::OnPaint);

    Connect(wxID_ANY, wxEVT_SET_FOCUS, (wxObjectEventFunction)
                                 (wxEventFunction)
                                 (wxFocusEventFunction)&SwitchView::OnFocus);
    Connect(wxID_ANY, wxEVT_SCROLL_TOP, (wxObjectEventFunction)
                                  (wxEventFunction)
                                  (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_SCROLL_BOTTOM, (wxObjectEventFunction)
                                     (wxEventFunction)
                                     (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_SCROLL_LINEUP, (wxObjectEventFunction)
                                     (wxEventFunction)
                                     (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_SCROLL_LINEDOWN, (wxObjectEventFunction)
                                       (wxEventFunction)
                                       (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_SCROLL_PAGEUP, (wxObjectEventFunction)
                                     (wxEventFunction)
                                     (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_SCROLL_PAGEDOWN, (wxObjectEventFunction)
                                       (wxEventFunction)
                                       (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_SCROLL_THUMBTRACK, (wxObjectEventFunction)
                                         (wxEventFunction)
                                         (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_SCROLL_THUMBRELEASE, (wxObjectEventFunction)
                                           (wxEventFunction)
                                           (wxScrollEventFunction)&SwitchView::OnScroll);
    Connect(wxID_ANY, wxEVT_ERASE_BACKGROUND, (wxObjectEventFunction)
                                        (wxEventFunction)
                                        (wxEraseEventFunction)&SwitchView::OnErase);

    Connect(wxID_ANY, wxEVT_DYNAMIC_SASH_SPLIT, (wxObjectEventFunction)
                                          (wxEventFunction)
                                          (wxDynamicSashSplitEventFunction)&SwitchView::OnSplit);
    Connect(wxID_ANY, wxEVT_DYNAMIC_SASH_UNIFY, (wxObjectEventFunction)
                                          (wxEventFunction)
                                          (wxDynamicSashUnifyEventFunction)&SwitchView::OnUnify);
}

wxSize SwitchView::DoGetBestSize() const {
    return wxSize(64, 64);
}

void SwitchView::OnSize(wxSizeEvent& WXUNUSED(event)) {
    Layout();

    wxScrollBar *hscroll = m_dyn_sash->GetHScrollBar(this);
    wxScrollBar *vscroll = m_dyn_sash->GetVScrollBar(this);

    if (hscroll && vscroll) {
        int hpos = hscroll->GetThumbPosition();
        int vpos = vscroll->GetThumbPosition();

        wxSize size = m_view->GetSize();
        if (hpos + size.GetWidth() > 300)
            hpos = 300 - size.GetWidth();
        if (vpos + size.GetHeight() > 300)
            vpos = 300 - size.GetHeight();

        hscroll->SetScrollbar(hpos, size.GetWidth(), 300, size.GetWidth());
        vscroll->SetScrollbar(vpos, size.GetHeight(), 300, size.GetHeight());
    }
}

void SwitchView::OnPaint(wxPaintEvent& WXUNUSED(event)) {
    wxPaintDC dc(m_view);

    wxScrollBar *hscroll = m_dyn_sash->GetHScrollBar(this);
    wxScrollBar *vscroll = m_dyn_sash->GetVScrollBar(this);

    dc.Clear();
    dc.SetDeviceOrigin(-hscroll->GetThumbPosition(), -vscroll->GetThumbPosition());

    if (m_choice->GetSelection()) {
        dc.DrawLine(20, 20, 280, 20);
        dc.DrawLine(280, 20, 280, 280);
        dc.DrawLine(280, 280, 20, 280);
        dc.DrawLine(20, 280, 20, 20);
    } else {
        dc.DrawLine(150, 20, 280, 280);
        dc.DrawLine(280, 280, 20, 280);
        dc.DrawLine(20, 280, 150, 20);
    }
}

void SwitchView::OnErase(wxEraseEvent& WXUNUSED(event)) {
    // Do nothing
}

void SwitchView::OnSplit(wxDynamicSashSplitEvent& WXUNUSED(event)) {
    SwitchView *view = new SwitchView(m_dyn_sash);
    view->m_choice->SetSelection(m_choice->GetSelection());

    wxScrollBar *hscroll = m_dyn_sash->GetHScrollBar(this);
    wxScrollBar *vscroll = m_dyn_sash->GetVScrollBar(this);

    hscroll->SetEventHandler(this);
    vscroll->SetEventHandler(this);
}

void SwitchView::OnUnify(wxDynamicSashUnifyEvent& WXUNUSED(event)) {
    wxScrollBar *hscroll = m_dyn_sash->GetHScrollBar(this);
    wxScrollBar *vscroll = m_dyn_sash->GetVScrollBar(this);

    hscroll->SetEventHandler(this);
    vscroll->SetEventHandler(this);
}

void SwitchView::OnChoice(wxCommandEvent& WXUNUSED(event)) {
    m_view->Refresh();
}

void SwitchView::OnScroll(wxScrollEvent& WXUNUSED(event)) {
    m_view->Refresh();
}

void SwitchView::OnFocus(wxFocusEvent& event) {
    wxScrollBar *hscroll = m_dyn_sash->GetHScrollBar(this);
    wxScrollBar *vscroll = m_dyn_sash->GetVScrollBar(this);

    if (event.GetEventObject() == hscroll || event.GetEventObject() == vscroll) {
        m_view->SetFocus();
    } else {
        event.Skip();
    }
}



bool SwitchDemo::OnInit() {
    wxFrame *frame;
    wxDynamicSashWindow *dyn;

    frame = new wxFrame(NULL, wxID_ANY, wxT("Dynamic Sash Window Switch Demo"));
    dyn = new wxDynamicSashWindow(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN);
    new SwitchView(dyn);

    frame->SetSize(480, 480);
    frame->Show();

    return true;
}
