/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.cpp
// Purpose:     wxTabCtrl
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "malloc.h"

#define INCL_PM
#include <os2.h>

//#include "wx/msw/dib.h"
#include "wx/os2/tabctrl.h"
#include "wx/app.h"
#include "wx/os2/private.h"
#include "wx/generic/imaglist.h"

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTabCtrl, wxControl)
END_EVENT_TABLE()

wxTabCtrl::wxTabCtrl()
{
    m_imageList = NULL;
}

bool wxTabCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
    m_imageList = NULL;

    m_backgroundColour = *wxWHITE; // TODO: wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
//        GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));
    m_foregroundColour = *wxBLACK ;

    SetName(name);

    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    m_windowStyle = style;

    SetFont(* (wxTheFontList->FindOrCreateFont(11, wxSWISS, wxNORMAL, wxNORMAL)));

    SetParent(parent);

    DWORD msflags = 0;

    if (width <= 0)
        width = 100;
    if (height <= 0)
        height = 30;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;

    m_windowId = (id < 0 ? NewControlId() : id);

    long tabStyle = 0;

    // Create the toolbar control.
    HWND hWndTabCtrl = 0;
    // TODO: create tab control

    m_hWnd = (WXHWND) hWndTabCtrl;
    if (parent) parent->AddChild(this);

    SubclassWin((WXHWND) hWndTabCtrl);

    return FALSE;
}

wxTabCtrl::~wxTabCtrl()
{
    UnsubclassWin();
}

bool wxTabCtrl::OS2OnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    wxTabEvent event(wxEVT_NULL, m_windowId);
    wxEventType eventType = wxEVT_NULL;
// TODO:
/*
    NMHDR* hdr1 = (NMHDR*) lParam;
    switch ( hdr1->code )
    {
        case TCN_SELCHANGE:
            eventType = wxEVT_COMMAND_TAB_SEL_CHANGED;
            break;

        case TCN_SELCHANGING:
            eventType = wxEVT_COMMAND_TAB_SEL_CHANGING;
            break;

        case TTN_NEEDTEXT:
        {
            // TODO
//            if (tool->m_shortHelpString != "")
//                ttText->lpszText = (char *) (const char *)tool->m_shortHelpString;
        }

        default :
            return wxControl::OS2OnNotify(idCtrl, lParam, result);
    }
*/
    event.SetEventObject( this );
    event.SetEventType(eventType);
    event.SetInt(idCtrl) ;

    return ProcessEvent(event);
}

// Responds to colour changes, and passes event on to children.
void wxTabCtrl::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    // TODO:
/*
    m_backgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
                                  GetGValue(GetSysColor(COLOR_BTNFACE)),
                                  GetBValue(GetSysColor(COLOR_BTNFACE)));

    Refresh();
*/
    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}


// Delete all items
bool wxTabCtrl::DeleteAllItems()
{
    // TODO
    return FALSE;
}

// Delete an item
bool wxTabCtrl::DeleteItem(int item)
{
    // TODO
    return FALSE;
}

// Get the selection
int wxTabCtrl::GetSelection() const
{
    // TODO
    return 0;
}

// Get the tab with the current keyboard focus
int wxTabCtrl::GetCurFocus() const
{
    // TODO
    return 0;
}

// Get the associated image list
wxImageList* wxTabCtrl::GetImageList() const
{
    return m_imageList;
}

// Get the number of items
int wxTabCtrl::GetItemCount() const
{
    // TODO
    return 0;
}

// Get the rect corresponding to the tab
bool wxTabCtrl::GetItemRect(int item, wxRect& wxrect) const
{
    // TODO
    return FALSE;
}

// Get the number of rows
int wxTabCtrl::GetRowCount() const
{
    // TODO
    return 0;
}

// Get the item text
wxString wxTabCtrl::GetItemText(int item) const
{
    // TODO
    return wxString("");
}

// Get the item image
int wxTabCtrl::GetItemImage(int item) const
{
    // TODO
    return 0;
}

// Get the item data
void* wxTabCtrl::GetItemData(int item) const
{
    // TODO
    return NULL;
}

// Hit test
int wxTabCtrl::HitTest(const wxPoint& pt, long& flags)
{
    // TODO
    return 0;
}

// Insert an item
bool wxTabCtrl::InsertItem(int item, const wxString& text, int imageId, void* data)
{
    // TODO
    return FALSE;
}

// Set the selection
int wxTabCtrl::SetSelection(int item)
{
    // TODO
    return 0;
}

// Set the image list
void wxTabCtrl::SetImageList(wxImageList* imageList)
{
    // TODO
}

// Set the text for an item
bool wxTabCtrl::SetItemText(int item, const wxString& text)
{
    // TODO
    return FALSE;
}

// Set the image for an item
bool wxTabCtrl::SetItemImage(int item, int image)
{
    // TODO
    return FALSE;
}

// Set the data for an item
bool wxTabCtrl::SetItemData(int item, void* data)
{
    // TODO
    return FALSE;
}

// Set the size for a fixed-width tab control
void wxTabCtrl::SetItemSize(const wxSize& size)
{
    // TODO
}

// Set the padding between tabs
void wxTabCtrl::SetPadding(const wxSize& padding)
{
    // TODO
}

#if 0
// These are the default colors used to map the bitmap colors
// to the current system colors

#define BGR_BUTTONTEXT      (RGB(000,000,000))  // black
#define BGR_BUTTONSHADOW    (RGB(128,128,128))  // dark grey
#define BGR_BUTTONFACE      (RGB(192,192,192))  // bright grey
#define BGR_BUTTONHILIGHT   (RGB(255,255,255))  // white
#define BGR_BACKGROUNDSEL   (RGB(255,000,000))  // blue
#define BGR_BACKGROUND      (RGB(255,000,255))  // magenta

void wxMapBitmap(HBITMAP hBitmap, int width, int height)
{
  COLORMAP ColorMap[] = {
    {BGR_BUTTONTEXT,    COLOR_BTNTEXT},     // black
    {BGR_BUTTONSHADOW,  COLOR_BTNSHADOW},   // dark grey
    {BGR_BUTTONFACE,    COLOR_BTNFACE},     // bright grey
    {BGR_BUTTONHILIGHT, COLOR_BTNHIGHLIGHT},// white
    {BGR_BACKGROUNDSEL, COLOR_HIGHLIGHT},   // blue
    {BGR_BACKGROUND,    COLOR_WINDOW}       // magenta
  };

  int NUM_MAPS = (sizeof(ColorMap)/sizeof(COLORMAP));
  int n;
  for ( n = 0; n < NUM_MAPS; n++)
  {
    ColorMap[n].to = ::GetSysColor(ColorMap[n].to);
  }

  HBITMAP hbmOld;
  HDC hdcMem = CreateCompatibleDC(NULL);

  if (hdcMem)
  {
    hbmOld = SelectObject(hdcMem, hBitmap);

    int i, j, k;
    for ( i = 0; i < width; i++)
    {
        for ( j = 0; j < height; j++)
        {
            COLORREF pixel = ::GetPixel(hdcMem, i, j);
/*
            BYTE red = GetRValue(pixel);
            BYTE green = GetGValue(pixel);
            BYTE blue = GetBValue(pixel);
*/

            for ( k = 0; k < NUM_MAPS; k ++)
            {
                if ( ColorMap[k].from == pixel )
                {
                    /* COLORREF actualPixel = */ ::SetPixel(hdcMem, i, j, ColorMap[k].to);
                    break;
                }
            }
        }
    }


    SelectObject(hdcMem, hbmOld);
    DeleteObject(hdcMem);
  }

}
#endif

// Tab event
IMPLEMENT_DYNAMIC_CLASS(wxTabEvent, wxCommandEvent)

wxTabEvent::wxTabEvent(wxEventType commandType, int id):
  wxCommandEvent(commandType, id)
{
}


