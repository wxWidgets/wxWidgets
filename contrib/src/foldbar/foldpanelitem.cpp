/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelitem.cpp
// Purpose:
// Author:      Jorgen Bodde
// Modified by: ABX - 19/12/2004 : possibility of horizontal orientation
//                               : wxWidgets coding standards
// Created:     22/06/2004
// RCS-ID:      $Id$
// Copyright:   (c) Jorgen Bodde
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/foldbar/foldpanelbar.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(wxFoldWindowItemArray);

//----------------------------------------------------------------------------
// wxFoldPanelItem
//----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxFoldPanelItem, wxPanel )

BEGIN_EVENT_TABLE(wxFoldPanelItem,wxPanel)
    EVT_CAPTIONBAR(wxID_ANY, wxFoldPanelItem::OnPressCaption)
    EVT_PAINT(wxFoldPanelItem::OnPaint)
    //EVT_SIZE(wxFoldPanelItem::OnSize)
END_EVENT_TABLE()

wxFoldPanelItem::wxFoldPanelItem( wxWindow *parent, const wxString &caption, wxImageList *icons, bool collapsedInitially,
                                  const wxCaptionBarStyle &style )
    : m_controlCreated(false)
    , m_userSize(0)
    , m_panelSize(0)
    , m_lastInsertPos(0)
    , m_itemPos(0)
    , m_userSized(false)
{
    wxCHECK2(parent, return);

    wxPanel::Create(parent, wxID_ANY);

    // create the caption bar, in collapsed or expanded state

    m_captionBar = new wxCaptionBar(this, caption, icons, wxID_ANY, style, wxPoint(0,0));
    //m_captionBar->SetBoldFont();

    if(collapsedInitially)
        m_captionBar->Collapse();

    m_controlCreated = true;

    // make initial size for component, if collapsed, the
    // size is determined on the panel height and won't change

    wxSize size = m_captionBar->GetSize();
    m_panelSize = IsVertical()?size.GetHeight():size.GetWidth();
    m_lastInsertPos = m_panelSize;
}

void wxFoldPanelItem::AddWindow(wxWindow *window, int flags, int ySpacing, int leftSpacing, int rightSpacing)
{
    wxASSERT(window);

    wxFoldWindowItem *wi = new wxFoldWindowItem(window, flags, ySpacing, leftSpacing, rightSpacing);
    m_items.Add(wi);

    bool vertical = this->IsVertical();

    window->SetSize( vertical ? leftSpacing : m_lastInsertPos + ySpacing,
                     vertical ? m_lastInsertPos + ySpacing : leftSpacing,
                     wxDefaultCoord,
                     wxDefaultCoord,
                     wxSIZE_USE_EXISTING);

    m_lastInsertPos += wi->GetWindowLength( vertical );

    ResizePanel();
}

void wxFoldPanelItem::AddSeparator(const wxColour &color, int ySpacing, int leftSpacing, int rightSpacing)
{
    wxFoldWindowItem *wi = new wxFoldWindowItem(m_lastInsertPos, color, ySpacing, leftSpacing, rightSpacing);
    m_items.Add(wi);

    m_lastInsertPos += wi->GetWindowLength( this->IsVertical() );

    ResizePanel();
}


wxFoldPanelItem::~wxFoldPanelItem()
{
    m_items.Clear();
}

void wxFoldPanelItem::OnPressCaption(wxCaptionBarEvent &event)
{
    // tell the upper container we are responsible
    // for this event, so it can fold the panel item
    // and do a refresh

    event.SetTag((void *)this);
    event.Skip();
}

/* Inactive */
void wxFoldPanelItem::OnSize(wxSizeEvent &event)
{
    // deny access to pointers (yet)

    if(!m_controlCreated)
    {
        event.Skip();
        return;
    }

    // calculate the size needed for this window, so
    // we get the parent size, and determine the size for the caption and the panel

    //wxRect rect = GetRect();

    //wxSize size(0,wxDefaultCoord);
    //size.SetWidth(rect.GetWidth());
    //m_captionBar->SetSize(size);

}

int wxFoldPanelItem::Reposition(int pos)
{
    // NOTE: Call Resize before Reposition when an item is added, because the new
    // size needed will be calculated by Resize. Ofcourse the relative position
    // of the controls have to be correct in respect to the caption bar

    Freeze();

    bool vertical = this->IsVertical();

    SetSize( vertical ? wxDefaultCoord : pos,
             vertical ? pos : wxDefaultCoord,
             wxDefaultCoord,
             wxDefaultCoord,
             wxSIZE_USE_EXISTING);

    m_itemPos = pos;

    Thaw();

    return GetPanelLength();
}

void wxFoldPanelItem::ResizePanel()
{
    bool vertical = IsVertical();

    // prevent unnecessary updates by blocking repaints for a sec

    Freeze();

    // force this panel to take the width of the parent panel and the y of the
    // user or calulated width (which will be recalculated by the contents here

    wxSize size;
    if(m_captionBar->IsCollapsed())
    {
        size = m_captionBar->GetSize();
        m_panelSize = vertical ? size.GetHeight() : size.GetWidth();
    }
    else
    {
        size = GetBestSize();
        m_panelSize = vertical ? size.GetHeight() : size.GetWidth();

        if(m_userSized)
        {
            if ( vertical )
                size.SetHeight(m_userSize);
            else
                size.SetWidth(m_userSize);
        }
    }

    wxSize pnlsize = GetParent()->GetSize();
    if ( vertical )
        size.SetWidth(pnlsize.GetWidth());
    else
        size.SetHeight(pnlsize.GetHeight());

    // resize caption bar
    m_captionBar->SetSize( vertical ? size.GetWidth() : wxDefaultCoord,
                          vertical ? wxDefaultCoord : size.GetHeight());

    // resize the panel
    SetSize(size);

    // go by all the controls and call Layout

    for(size_t i = 0; i < m_items.GetCount(); i++)
        m_items.Item(i).ResizeItem( vertical ? size.GetWidth() : size.GetHeight() , vertical );

    // and draw all

    Thaw();
}

void wxFoldPanelItem::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // draw all the items that are lines
    wxPaintDC dc(this);
    bool vertical = IsVertical();

    for(size_t i = 0; i < m_items.GetCount(); i++)
    {
        wxFoldWindowItem &item = m_items.Item(i);
        wxPen pen(item.GetLineColour(), 1, wxSOLID);
        if(item.GetType() == wxFoldWindowItem::SEPARATOR)
        {
            dc.SetPen(pen);
            int a = item.GetLeftSpacing();
            int b = item.GetLineY() + item.GetSpacing();
            int c = item.GetLineLength();
            int d = a + c;
            if (vertical)
                dc.DrawLine(a, b, d, b);
            else
                dc.DrawLine(b, a, b, d);
        }
    }
}

bool wxFoldPanelItem::IsVertical() const
{
    // grandparent of wxFoldPanelItem is wxFoldPanelBar
    // default is vertical
    wxPanel *panel = wxDynamicCast(GetParent(), wxPanel);
    wxCHECK_MSG( panel, true, _T("wrong parent") );
    wxFoldPanelBar *bar = wxDynamicCast(panel->GetParent(), wxFoldPanelBar);
    wxCHECK_MSG( bar, true, _T("wrong parent") );
    return bar->IsVertical();
}
