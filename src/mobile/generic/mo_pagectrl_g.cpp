/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_pagectrl_g.cpp
// Purpose:     wxMoPageCtrl class
// Author:      Julian Smart
// Modified by:
// Created:     16/05/09
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/dcclient.h"
#include "wx/dcscreen.h"
#include "wx/settings.h"
#include "wx/dcbuffer.h"

#include "wx/mobile/generic/pagectrl.h"
#include "wx/mobile/generic/utils.h"

IMPLEMENT_DYNAMIC_CLASS(wxMoPageCtrl, wxControl)

BEGIN_EVENT_TABLE(wxMoPageCtrl, wxControl)
    EVT_PAINT(wxMoPageCtrl::OnPaint)
    EVT_ERASE_BACKGROUND(wxMoPageCtrl::OnEraseBackground)    
    EVT_SIZE(wxMoPageCtrl::OnSize)
    EVT_MOUSE_EVENTS(wxMoPageCtrl::OnMouseEvent)
END_EVENT_TABLE()

bool wxMoPageCtrl::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    style &= ~wxBORDER_MASK;
    style |= wxBORDER_NONE;

    if ( !wxControl::Create(parent, id, pos, size, style, validator, name) )
        return false;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    SetForegroundColour(wxMoSystemSettings::GetColour(wxMO_COLOUR_PAGE_UNSELECTED));

    SetInitialSize(size);

    return true;
}

wxMoPageCtrl::~wxMoPageCtrl()
{
}

void wxMoPageCtrl::Init()
{
    m_pageCount = 0;
    m_currentPage = -1;
    m_dotSize = 8;
    m_dotMargin = 10;
}

wxSize wxMoPageCtrl::DoGetBestSize() const
{
    wxSize best = GetSizeForPageCount(GetPageCount());
    CacheBestSize(best);
    return best;
}

void wxMoPageCtrl::SetPageCount(int pageCount)
{
    InvalidateBestSize();

    m_pageCount = pageCount;

    Refresh();
}

void wxMoPageCtrl::SetCurrentPage(int page)
{
    m_currentPage = page;
    
    Refresh();
}

wxSize wxMoPageCtrl::GetSizeForPageCount(int pageCount) const
{
    wxSize sz(wxMax(m_dotMargin, (m_dotSize + m_dotMargin) * pageCount - m_dotMargin), m_dotSize + 4);
    return sz;
}

bool wxMoPageCtrl::SetBackgroundColour(const wxColour &colour)
{
    if ( !wxControl::SetBackgroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoPageCtrl::SetForegroundColour(const wxColour &colour)
{
    if ( !wxControl::SetForegroundColour(colour) )
    {
        // nothing to do
        return false;
    }

    Refresh();

    return true;
}

bool wxMoPageCtrl::Enable(bool enable)
{
    wxControl::Enable(enable);
    Refresh();
    return true;
}

bool wxMoPageCtrl::SetFont(const wxFont& font)
{
    wxControl::SetFont(font);

    Refresh();

    return true;
}

void wxMoPageCtrl::OnPaint(wxPaintEvent& WXUNUSED(event))
{
#ifdef __WXMSW__
    wxBufferedPaintDC dc(this);
#else
    wxPaintDC dc(this);
#endif

    wxSize clientSize(GetClientSize());

    wxColour bgColour = GetParent()->GetBackgroundColour();
    wxColour selColour = wxMoSystemSettings::GetColour(wxMO_COLOUR_PAGE_SELECTED);

    dc.SetBackground(wxBrush(bgColour));
    dc.Clear();

    dc.SetPen(wxPen(GetForegroundColour()));
    dc.SetBrush(wxBrush(GetForegroundColour()));

    int x = 0;
    if (GetPageCount() > 0)
    {
        wxSize sz = GetSizeForPageCount(GetPageCount());
        x = (clientSize.x - sz.x)/2;
    }

    int y = clientSize.y/2;

    for (int i = 0; i < GetPageCount(); i++)
    {
        if (GetCurrentPage() == i)
        {
            dc.SetPen(wxPen(* wxWHITE));
            dc.SetBrush(wxBrush(* wxWHITE));
        }
        else
        {
            dc.SetPen(wxPen(GetForegroundColour()));
            dc.SetBrush(wxBrush(GetForegroundColour()));
        }

        int dotX = x + m_dotSize/2;
        int dotY = y;

        dc.DrawCircle(dotX, dotY, m_dotSize/2);

        x += m_dotSize;
        x += m_dotMargin;
    }
}

int wxMoPageCtrl::HitTest(const wxPoint& pos)
{
    wxSize clientSize(GetClientSize());
    int x = 0;

    if (GetPageCount() > 0)
    {
        wxSize sz = GetSizeForPageCount(GetPageCount());
        x = (clientSize.x - sz.x)/2;
    }

    int y = clientSize.y/2;

    for (int i = 0; i < GetPageCount(); i++)
    {
        wxRect rect(x-m_dotMargin/2, y-m_dotSize-m_dotMargin/2, m_dotSize+m_dotMargin, m_dotSize+m_dotMargin);

        if (rect.Contains(pos))
            return i;

        x += m_dotSize;
        x += m_dotMargin;
    }

    return -1;
}

void wxMoPageCtrl::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
}

void wxMoPageCtrl::OnMouseEvent(wxMouseEvent& event)
{
    if (event.LeftDown())
    {
        int hit = HitTest(event.GetPosition());
        if (hit != -1)
        {
            SetCurrentPage(hit);
            Refresh();
            SendClickEvent();
        }
    }
}

void wxMoPageCtrl::OnSize(wxSizeEvent& WXUNUSED(event))
{
    Refresh();
}

void wxMoPageCtrl::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    event.SetEventObject(this);
    event.SetInt(GetCurrentPage());
    event.SetExtraLong(GetCurrentPage());
    
    GetEventHandler()->ProcessEvent(event);
}

