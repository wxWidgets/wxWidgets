/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelbar.cpp
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

#include "wx/foldbar/foldpanelbar.h"
#include "icon_collapsed.xpm"
#include "icon_expanded.xpm"
#include "icon_theresmore.xpm"

//----------------------------------------------------------------------------
// wxFoldPanelBar
//----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxFoldPanelBar, wxPanel )

BEGIN_EVENT_TABLE(wxFoldPanelBar,wxPanel)
    EVT_SIZE(wxFoldPanelBar::OnSizePanel)
    //EVT_PAINT(wxFoldPanelBar::OnPaint)
    EVT_CAPTIONBAR(wxID_ANY, wxFoldPanelBar::OnPressCaption)
END_EVENT_TABLE()

wxFoldPanelBar::wxFoldPanelBar()
{

}

wxFoldPanelBar::wxFoldPanelBar( wxWindow *parent, wxWindowID id, const wxPoint &position,
                                const wxSize& size, long style, long extraStyle)
    : _foldPanel(0)
    , _bottomPanel(0)
    , _controlCreated(false)
{
    Create( parent, id, position, size, style, extraStyle);
}

void wxFoldPanelBar::Create( wxWindow *parent, wxWindowID id, const wxPoint &position,
                             const wxSize& size, long style, long extraStyle )
{

    _extraStyle = extraStyle;

    // create the panel (duh!). This causes a size event, which we are going
    // to skip when we are not initialised

    wxPanel::Create(parent, id, position, size, style);

    // the fold panel area

    _foldPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxTAB_TRAVERSAL);

    // the extra area for some icons / context menu etc

#if 0
    _bottomPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(wxDefaultCoord,22), wxNO_BORDER|wxTAB_TRAVERSAL);
    _bottomPanel->SetBackgroundColour(*wxWHITE);
#endif

    // create the fold icons to be used in the captions

    _images = new wxImageList(16, 16);

    wxBitmap *bmp = new wxBitmap(icon_expanded);
    _images->Add(*bmp);
    delete bmp;

    bmp = new wxBitmap(icon_collapsed);
    _images->Add(*bmp);
    delete bmp;

    _moreBmp = new wxBitmap(icon_theresmore);

    // do this as last, to check if create is already called

    _controlCreated = true;
}

wxFoldPanelBar::~wxFoldPanelBar()
{
    delete _images;
    delete _moreBmp;
}

wxFoldPanel wxFoldPanelBar::AddFoldPanel(const wxString &caption, bool collapsedInitially, const wxCaptionBarStyle &style)
{
    wxASSERT(_controlCreated);

    // create a fold panel item, which is first only the caption.
    // the user can now add a panel area which will be folded in
    // when pressed.

    wxFoldPanelItem *item = new wxFoldPanelItem(_foldPanel, caption, _images, collapsedInitially, style);

    // look at the last added one and reposition this one
    int y = 0;
    if(_panels.GetCount() > 0)
        y = _panels.Last()->GetY() + _panels.Last()->GetPanelHeight();

    item->Reposition(y);
    _panels.Add(item);

    //return wxFoldPanel(item);
    return wxFoldPanel(item);
}

int wxFoldPanelBar::AddFoldPanelWindow(const wxFoldPanel &panel, wxWindow *window, int flags, int ySpacing, int leftSpacing,
                                       int rightSpacing)
{
    wxCHECK(panel.IsOk(), -1);
    panel.GetItem()->AddWindow(window, flags, ySpacing, leftSpacing, rightSpacing);

    // TODO: Take old and new height, and if difference, reposition all the lower panels
    // this is because the user can add new wxWindow controls somewhere in between
    // when other panels are already present.

    return 0;
}

int wxFoldPanelBar::AddFoldPanelSeperator(const wxFoldPanel &panel, const wxColour &color, int ySpacing, int leftSpacing,
                                          int rightSpacing)
{
    wxCHECK(panel.IsOk(), -1);
    panel.GetItem()->AddSeparator(color, ySpacing, leftSpacing, rightSpacing);

    return 0;
}

void wxFoldPanelBar::OnSizePanel(wxSizeEvent &event)
{
    // skip all stuff when we are not initialised yet

    if(!_controlCreated)
    {
        event.Skip();
        return;
    }

    // now size the fold panel area and the
    // lower bar in such a way that the bar is always
    // visible

    wxRect foldrect = GetRect();

    // fold panel itself. If too little space,
    // don't show it

#if 0
    if(foldrect.GetHeight() < 23)
        foldrect.SetHeight(0);
    else
        foldrect.SetHeight(foldrect.GetHeight() - 22);
#endif

    foldrect.SetX(0);
    foldrect.SetY(0);
    _foldPanel->SetSize(foldrect);

    if(_extraStyle & wxFPB_COLLAPSE_TO_BOTTOM)
    {
        wxRect rect = RepositionCollapsedToBottom();
        if(rect.GetHeight() > 0)
            RefreshRect(rect);
    }

    // TODO: A smart way to check wether the old - new width of the
    // panel changed, if so no need to resize the fold panel items

    RedisplayFoldPanelItems();

    // tool panel for icons and other stuff

#if 0
    wxRect bottomrect = GetRect();
    if(bottomrect.GetHeight() < 22)
        bottomrect.SetY(0);
    else
        bottomrect.SetY(bottomrect.GetHeight() - 22);

    bottomrect.SetHeight(22);
    bottomrect.SetX(0);
    _bottomPanel->SetSize(bottomrect);

    // TODO: redraw the bitmap properly
    // use the captionbar algorithm for that

    _bottomPanel->Refresh();
#endif
}

void wxFoldPanelBar::OnPaint(wxPaintEvent &event)
{
    if(!_controlCreated)
        return;
#if 0
    // paint the bottom panel only, where the
    // arrow is shown when there is more to show the user
    // just as informative icon

    wxPaintDC dc(_bottomPanel);

    wxSize size = _bottomPanel->GetSize();
    int offset = (size.GetHeight() - _moreBmp->GetHeight()) / 2;

    dc.DrawBitmap(*_moreBmp, size.GetWidth() - _moreBmp->GetWidth() - 2, offset, true);
#endif

    event.Skip();
}

void wxFoldPanelBar::OnPressCaption(wxCaptionBarEvent &event)
{
    // act upon the folding or expanding status of the bar
    // to expand or collapse the panel(s)

    if(event.GetFoldStatus())
        Collapse(wxFoldPanel((wxFoldPanelItem *)event.GetTag()));
    else
        Expand(wxFoldPanel((wxFoldPanelItem *)event.GetTag()));
}

void wxFoldPanelBar::RefreshPanelsFrom(wxFoldPanelItem *item)
{
    wxASSERT(item);

    int i = _panels.Index(item);
    if(i != wxNOT_FOUND)
        RefreshPanelsFrom(i);
}

void wxFoldPanelBar::RefreshPanelsFrom(size_t i)
{
    Freeze();

    // if collapse to bottom is on, the panels that are not expanded
    // should be drawn at the bottom. All panels that are expanded
    // are drawn on top. The last expanded panel gets all the extra space

    if(_extraStyle & wxFPB_COLLAPSE_TO_BOTTOM)
    {
        int offset = 0;

        for(size_t j = 0; j < _panels.GetCount(); j++)
        {
            if(_panels.Item(j)->IsExpanded())
                offset += _panels.Item(j)->Reposition(offset);
        }

        // put all non collapsed panels at the bottom where there is space, else
        // put them right behind the expanded ones

        RepositionCollapsedToBottom();
    }
    else
    {
        int y = _panels.Item(i)->GetY() + _panels.Item(i)->GetPanelHeight();
        for(i++; i < _panels.GetCount(); i++)
            y += _panels.Item(i)->Reposition(y);
    }
    Thaw();
}

void wxFoldPanelBar::RedisplayFoldPanelItems()
{
    // resize them all. No need to reposition

    wxFoldPanelItem *item;
    for(size_t i = 0; i < _panels.GetCount(); i++)
    {
        item = _panels.Item(i);
        wxASSERT(item);

        item->ResizePanel();
    }
}

wxRect wxFoldPanelBar::RepositionCollapsedToBottom()
{
    wxRect value(0,0,0,0);

    // determine wether the number of panels left
    // times the size of their captions is enough
    // to be placed in the left over space

    int expanded = 0, collapsed = 0, offset;
    GetPanelsHeight(collapsed, expanded);

    // if no room stick them behind the normal ones, else
    // at the bottom

    if((GetSize().GetHeight() - expanded - collapsed) < 0)
        offset = expanded;
    else
    {
        // value is the region which is left unpainted
        // I will send it back as 'slack' so it does not need to
        // be recalulated.

        value.SetX(0);
        value.SetY(expanded);
        value.SetHeight(GetSize().GetHeight() - expanded);
        value.SetWidth(GetSize().GetWidth());

        offset = GetSize().GetHeight() - collapsed;
    }


    // go reposition

    for(size_t i = 0; i < _panels.GetCount(); i++)
    {
        if(!_panels.Item(i)->IsExpanded())
            offset += _panels.Item(i)->Reposition(offset);
    }

    return value;
}

int wxFoldPanelBar::GetPanelsHeight(int &collapsed, int &expanded)
{
    int value = 0;

    // assumed here that all the panels that are expanded
    // are positioned after eachother from 0,0 to end.

    for(size_t j = 0; j < _panels.GetCount(); j++)
    {
        int offset = _panels.Item(j)->GetPanelHeight();
        value += offset;
        if(_panels.Item(j)->IsExpanded())
            expanded += offset;
        else
            collapsed += offset;
    }

    return value;

}
