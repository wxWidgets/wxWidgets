/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelitem.cpp
// Purpose:
// Author:      Jorgen Bodde
// Modified by:
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

#include "wx/foldbar/foldpanelitem.h"

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
    : _controlCreated(false)
    , _yUserSize(0)
    , _yPanelSize(0)
    , _yLastInsertPos(0)
    , _yPos(0)
    , _userSized(false)
{
    wxCHECK2(parent, return);

    wxPanel::Create(parent, wxID_ANY);

    // create the caption bar, in collapsed or expanded state

    _captionBar = new wxCaptionBar(this, caption, icons, wxID_ANY, style, wxPoint(0,0));
    //_captionBar->SetBoldFont();

    if(collapsedInitially)
        _captionBar->Collapse();

    _controlCreated = true;

    // make initial size for component, if collapsed, the
    // size is determined on the panel height and won't change

    wxSize size = _captionBar->GetSize();
    _yPanelSize = size.GetHeight();
    _yLastInsertPos = _yPanelSize;
}

void wxFoldPanelItem::AddWindow(wxWindow *window, int flags, int ySpacing, int leftSpacing, int rightSpacing)
{
    wxASSERT(window);

    wxFoldWindowItem *wi = new wxFoldWindowItem(window, flags, ySpacing, leftSpacing, rightSpacing);
    _items.Add(wi);

    window->SetSize(leftSpacing, _yLastInsertPos + ySpacing, wxDefaultCoord, wxDefaultCoord, wxSIZE_USE_EXISTING);
    _yLastInsertPos += wi->GetWindowHeight();

    ResizePanel();
}

void wxFoldPanelItem::AddSeparator(const wxColour &color, int ySpacing, int leftSpacing, int rightSpacing)
{
    wxFoldWindowItem *wi = new wxFoldWindowItem(_yLastInsertPos, color, ySpacing, leftSpacing, rightSpacing);
    _items.Add(wi);

    _yLastInsertPos += wi->GetWindowHeight();

    ResizePanel();
}


wxFoldPanelItem::~wxFoldPanelItem()
{
    _items.Clear();
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

    if(!_controlCreated)
    {
        event.Skip();
        return;
    }

    // calculate the size needed for this window, so
    // we get the parent size, and determine the size for the caption and the panel

    //wxRect rect = GetRect();

    //wxSize size(0,wxDefaultCoord);
    //size.SetWidth(rect.GetWidth());
    //_captionBar->SetSize(size);

}

int wxFoldPanelItem::Reposition(int y)
{
    // NOTE: Call Resize before Reposition when an item is added, because the new
    // size needed will be calculated by Resize. Ofcourse the relative position
    // of the controls have to be correct in respect to the caption bar

    Freeze();

    SetSize(wxDefaultCoord, y, wxDefaultCoord, wxDefaultCoord, wxSIZE_USE_EXISTING);
    _yPos = y;

    Thaw();

    return GetPanelHeight();
}

void wxFoldPanelItem::ResizePanel()
{
    // prevent unnecessary updates by blocking repaints for a sec

    Freeze();

    // force this panel to take the width of the parent panel and the y of the
    // user or calulated width (which will be recalculated by the contents here

    wxSize size;
    if(_captionBar->IsCollapsed())
    {
        size = _captionBar->GetSize();
        _yPanelSize = size.GetHeight();
    }
    else
    {
        size = GetBestSize();
        _yPanelSize = size.GetHeight();

        if(_userSized)
            size.SetHeight(_yUserSize);
    }

    wxSize pnlsize = GetParent()->GetSize();
    size.SetWidth(pnlsize.GetWidth());

    // resize caption bar
    _captionBar->SetSize(wxSize(size.GetWidth(), wxDefaultCoord));

    // resize the panel
    SetSize(size);

    // go by all the controls and call Layout

    for(size_t i = 0; i < _items.GetCount(); i++)
        _items.Item(i).ResizeItem(size.GetWidth());

    // and draw all

    Thaw();
}

void wxFoldPanelItem::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    // draw all the items that are lines

    wxPaintDC dc(this);

    for(size_t i = 0; i < _items.GetCount(); i++)
    {
        wxFoldWindowItem &item = _items.Item(i);
        wxPen pen(item.GetLineColour(), 1, wxSOLID);
        if(item.GetType() == wxFoldWindowItem::SEPARATOR)
        {
            dc.SetPen(pen);
            dc.DrawLine(item.GetLeftSpacing(), item.GetLineY() + item.GetYSpacing(),
                        item.GetLineWidth() + item.GetLeftSpacing(), item.GetLineY() + item.GetYSpacing());
        }
    }
}
