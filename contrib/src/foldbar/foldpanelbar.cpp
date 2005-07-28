/////////////////////////////////////////////////////////////////////////////
// Name:        foldpanelbar.cpp
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
#include "icon_collapsed.xpm"
#include "icon_expanded.xpm"
#include "icon_theresmore.xpm"

//----------------------------------------------------------------------------
// wxFoldPanelBar
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxFoldPanelBar, wxPanel )

BEGIN_EVENT_TABLE(wxFoldPanelBar,wxPanel)
    EVT_SIZE(wxFoldPanelBar::OnSizePanel)
    //EVT_PAINT(wxFoldPanelBar::OnPaint)
    EVT_CAPTIONBAR(wxID_ANY, wxFoldPanelBar::OnPressCaption)
END_EVENT_TABLE()

wxFoldPanelBar::wxFoldPanelBar()
    : wxPanel()
    , m_foldPanel(NULL)
    , m_bottomPanel(NULL)
    , m_controlCreated(false)
{

}

wxFoldPanelBar::wxFoldPanelBar( wxWindow *parent, wxWindowID id, const wxPoint &position,
                                const wxSize& size, long style, long extraStyle)
    : wxPanel()
    , m_foldPanel(NULL)
    , m_bottomPanel(NULL)
    , m_controlCreated(false)
{
    Create( parent, id, position, size, style, extraStyle);
}

void wxFoldPanelBar::Create( wxWindow *parent, wxWindowID id, const wxPoint &position,
                             const wxSize& size, long style, long extraStyle )
{

    m_extraStyle = extraStyle;

    // make sure there is any orientation
    if ( ( style & wxFPB_HORIZONTAL ) != wxFPB_HORIZONTAL )
        style |= wxFPB_VERTICAL;

    // create the panel (duh!). This causes a size event, which we are going
    // to skip when we are not initialised

    wxPanel::Create(parent, id, position, size, style);

    // the fold panel area

    m_foldPanel = new wxPanel(this, wxID_ANY, position, size, wxNO_BORDER|wxTAB_TRAVERSAL);

    // the extra area for some icons / context menu etc

#if 0
    m_bottomPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(wxDefaultCoord,22), wxNO_BORDER|wxTAB_TRAVERSAL);
    m_bottomPanel->SetBackgroundColour(*wxWHITE);
#endif

    // create the fold icons to be used in the captions

    m_images = new wxImageList(16, 16);

    wxBitmap *bmp = new wxBitmap(icon_expanded);
    m_images->Add(*bmp);
    delete bmp;

    bmp = new wxBitmap(icon_collapsed);
    m_images->Add(*bmp);
    delete bmp;

    m_moreBmp = new wxBitmap(icon_theresmore);

    // do this as last, to check if create is already called

    m_controlCreated = true;
}

wxFoldPanelBar::~wxFoldPanelBar()
{
    delete m_images;
    delete m_moreBmp;
}

wxFoldPanel wxFoldPanelBar::AddFoldPanel(const wxString &caption, bool collapsedInitially, const wxCaptionBarStyle &style)
{
    wxASSERT(m_controlCreated);

    // create a fold panel item, which is first only the caption.
    // the user can now add a panel area which will be folded in
    // when pressed.

    wxFoldPanelItem *item = new wxFoldPanelItem(m_foldPanel, caption, m_images, collapsedInitially, style);

    // look at the last added one and reposition this one
    int pos = 0;
    if(m_panels.GetCount() > 0)
        pos = m_panels.Last()->GetItemPos() + m_panels.Last()->GetPanelLength();

    item->Reposition(pos);
    m_panels.Add(item);

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

    if(!m_controlCreated)
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
    m_foldPanel->SetSize(foldrect);

    if(m_extraStyle & wxFPB_COLLAPSE_TO_BOTTOM)
    {
        wxRect rect = RepositionCollapsedToBottom();
        bool vertical = IsVertical();
        if((vertical && rect.GetHeight() > 0) || (!vertical && rect.GetWidth() > 0))
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
    m_bottomPanel->SetSize(bottomrect);

    // TODO: redraw the bitmap properly
    // use the captionbar algorithm for that

    m_bottomPanel->Refresh();
#endif
}

void wxFoldPanelBar::OnPaint(wxPaintEvent &event)
{
    if(!m_controlCreated)
        return;
#if 0
    // paint the bottom panel only, where the
    // arrow is shown when there is more to show the user
    // just as informative icon

    wxPaintDC dc(m_bottomPanel);

    wxSize size = m_bottomPanel->GetSize();
    int offset = (size.GetHeight() - m_moreBmp->GetHeight()) / 2;

    dc.DrawBitmap(*m_moreBmp, size.GetWidth() - m_moreBmp->GetWidth() - 2, offset, true);
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

    int i = m_panels.Index(item);
    if(i != wxNOT_FOUND)
        RefreshPanelsFrom(i);
}

void wxFoldPanelBar::RefreshPanelsFrom(size_t i)
{
    Freeze();

    // if collapse to bottom is on, the panels that are not expanded
    // should be drawn at the bottom. All panels that are expanded
    // are drawn on top. The last expanded panel gets all the extra space

    if(m_extraStyle & wxFPB_COLLAPSE_TO_BOTTOM)
    {
        int offset = 0;

        for(size_t j = 0; j < m_panels.GetCount(); j++)
        {
            if(m_panels.Item(j)->IsExpanded())
                offset += m_panels.Item(j)->Reposition(offset);
        }

        // put all non collapsed panels at the bottom where there is space, else
        // put them right behind the expanded ones

        RepositionCollapsedToBottom();
    }
    else
    {
        int pos = m_panels.Item(i)->GetItemPos() + m_panels.Item(i)->GetPanelLength();
        for(i++; i < m_panels.GetCount(); i++)
            pos += m_panels.Item(i)->Reposition(pos);
    }
    Thaw();
}

void wxFoldPanelBar::RedisplayFoldPanelItems()
{
    // resize them all. No need to reposition

    wxFoldPanelItem *item;
    for(size_t i = 0; i < m_panels.GetCount(); i++)
    {
        item = m_panels.Item(i);
        wxASSERT(item);

        item->ResizePanel();
    }
}

wxRect wxFoldPanelBar::RepositionCollapsedToBottom()
{
    wxRect value(0,0,0,0);
    bool vertical = IsVertical();

    // determine wether the number of panels left
    // times the size of their captions is enough
    // to be placed in the left over space

    int expanded = 0, collapsed = 0, offset;
    GetPanelsLength(collapsed, expanded);

    // if no room stick them behind the normal ones, else
    // at the bottom

    if(((vertical ? GetSize().GetHeight() : GetSize().GetWidth()) - expanded - collapsed) < 0)
        offset = expanded;
    else
    {
        // value is the region which is left unpainted
        // I will send it back as 'slack' so it does not need to
        // be recalulated.

        value.SetHeight(GetSize().GetHeight());
        value.SetWidth(GetSize().GetWidth());

        if(vertical)
        {
            value.SetY(expanded);
            value.SetHeight(value.GetHeight() - expanded);
        }
        else
        {
            value.SetX(expanded);
            value.SetWidth(value.GetWidth() - expanded);
        }

        offset = (vertical ? GetSize().GetHeight() : GetSize().GetWidth()) - collapsed;
    }

    // go reposition

    for(size_t i = 0; i < m_panels.GetCount(); i++)
    {
        if(!m_panels.Item(i)->IsExpanded())
            offset += m_panels.Item(i)->Reposition(offset);
    }

    return value;
}

int wxFoldPanelBar::GetPanelsLength(int &collapsed, int &expanded)
{
    int value = 0;

    // assumed here that all the panels that are expanded
    // are positioned after each other from 0,0 to end.

    for(size_t j = 0; j < m_panels.GetCount(); j++)
    {
        int offset = m_panels.Item(j)->GetPanelLength();
        value += offset;
        if(m_panels.Item(j)->IsExpanded())
            expanded += offset;
        else
            collapsed += offset;
    }

    return value;
}
