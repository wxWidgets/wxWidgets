/////////////////////////////////////////////////////////////////////////////
// Name:        tabctrl.cpp
// Purpose:     wxTabCtrl
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:       wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "tabctrl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined(__WIN95__)

#if !defined(__GNUWIN32__) && !defined(__SALFORDC__)
#include "malloc.h"
#endif

#include <windows.h>

#if (defined(__WIN95__) && !defined(__GNUWIN32__)) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
#include <commctrl.h>
#endif

#ifndef __TWIN32__
#ifdef __GNUWIN32__
#ifndef wxUSE_NORLANDER_HEADERS
#include "wx/msw/gnuwin32/extra.h"
#endif
#endif
#endif

#include "wx/msw/dib.h"
#include "wx/msw/tabctrl.h"
#include "wx/app.h"
#include "wx/msw/private.h"
#include "wx/msw/imaglist.h"

IMPLEMENT_DYNAMIC_CLASS(wxTabCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTabCtrl, wxControl)
    EVT_SYS_COLOUR_CHANGED(wxTabCtrl::OnSysColourChanged)
END_EVENT_TABLE()

wxTabCtrl::wxTabCtrl()
{
    m_imageList = NULL;
}

bool wxTabCtrl::Create(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
            long style, const wxString& name)
{
  m_imageList = NULL;

  m_backgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
      GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));
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
  if (style & wxBORDER)
    msflags |= WS_BORDER;
  msflags |= WS_CHILD | WS_VISIBLE;
  
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
  if (m_windowStyle & wxTC_MULTILINE)
    tabStyle |= TCS_MULTILINE;
  if (m_windowStyle & wxTC_RIGHTJUSTIFY)
    tabStyle |= TCS_RIGHTJUSTIFY;
  if (m_windowStyle & wxTC_FIXEDWIDTH)
    tabStyle |= TCS_FIXEDWIDTH;
  if (m_windowStyle & wxTC_OWNERDRAW)
    tabStyle |= TCS_OWNERDRAWFIXED;

  tabStyle |= TCS_TOOLTIPS;

  // Create the toolbar control.
  HWND hWndTabCtrl = CreateWindowEx(0L,     // No extended styles.
    WC_TABCONTROL,                          // Class name for the tab control
    wxT(""),                                 // No default text.
    WS_CHILD | WS_BORDER | WS_VISIBLE | tabStyle,    // Styles and defaults.
    x, y, width, height,                    // Standard size and position.
    (HWND) parent->GetHWND(),               // Parent window
    (HMENU)m_windowId,                      // ID.
    wxGetInstance(),                        // Current instance.
    NULL );                                 // No class data.

  m_hWnd = (WXHWND) hWndTabCtrl;
  if (parent) parent->AddChild(this);
  
  SubclassWin((WXHWND) hWndTabCtrl);

  return TRUE;
}

wxTabCtrl::~wxTabCtrl()
{
  UnsubclassWin();
}

bool wxTabCtrl::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM *result)
{
    wxTabEvent event(wxEVT_NULL, m_windowId);
    wxEventType eventType = wxEVT_NULL;
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
            return wxControl::MSWOnNotify(idCtrl, lParam, result);
    }

    event.SetEventObject( this );
    event.SetEventType(eventType);
    event.SetInt(idCtrl) ;

    return ProcessEvent(event);
}

// Responds to colour changes, and passes event on to children.
void wxTabCtrl::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    m_backgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
                                  GetGValue(GetSysColor(COLOR_BTNFACE)),
                                  GetBValue(GetSysColor(COLOR_BTNFACE)));

    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

// Delete all items
bool wxTabCtrl::DeleteAllItems()
{
    return ( TabCtrl_DeleteAllItems( (HWND) GetHWND() ) != FALSE );
}

// Delete an item
bool wxTabCtrl::DeleteItem(int item)
{
    return ( TabCtrl_DeleteItem( (HWND) GetHWND(), item) != FALSE );
}

// Get the selection
int wxTabCtrl::GetSelection() const
{
    return (int) TabCtrl_GetCurSel( (HWND) GetHWND() );
}

// Get the tab with the current keyboard focus
int wxTabCtrl::GetCurFocus() const
{
    return (int) TabCtrl_GetCurFocus( (HWND) GetHWND() );
}

// Get the associated image list
wxImageList* wxTabCtrl::GetImageList() const
{
    return m_imageList;
}

// Get the number of items
int wxTabCtrl::GetItemCount() const
{
    return (int) TabCtrl_GetItemCount( (HWND) GetHWND() );
}

// Get the rect corresponding to the tab
bool wxTabCtrl::GetItemRect(int item, wxRect& wxrect) const
{
    RECT rect;
    if ( !TabCtrl_GetItemRect( (HWND) GetHWND(), item, & rect) )
        return FALSE;
    else
    {
        wxrect.x = rect.left; wxrect.y = rect.top;
        wxrect.width = rect.right - rect.left;
        wxrect.height = rect.bottom - rect.top;
        return TRUE;
    }
}

// Get the number of rows
int wxTabCtrl::GetRowCount() const
{
    return (int) TabCtrl_GetRowCount( (HWND) GetHWND() );
}

// Get the item text
wxString wxTabCtrl::GetItemText(int item) const
{
    wxChar buf[256];
    wxString str(wxT(""));
    TC_ITEM tcItem;
    tcItem.mask = TCIF_TEXT;
    tcItem.pszText = buf;
    tcItem.cchTextMax = 256;

    if (TabCtrl_GetItem( (HWND) GetHWND(), item, & tcItem) )
        str = tcItem.pszText;

    return str;
}

// Get the item image
int wxTabCtrl::GetItemImage(int item) const
{
    TC_ITEM tcItem;
    tcItem.mask = TCIF_IMAGE;

    if (TabCtrl_GetItem( (HWND) GetHWND(), item, & tcItem) )
        return tcItem.iImage;
    else
        return -1;
}

// Get the item data
void* wxTabCtrl::GetItemData(int item) const
{
    TC_ITEM tcItem;
    tcItem.mask = TCIF_PARAM;

    if (TabCtrl_GetItem( (HWND) GetHWND(), item, & tcItem) )
        return (void*) tcItem.lParam;
    else
        return 0;
}

// Hit test
int wxTabCtrl::HitTest(const wxPoint& pt, long& flags)
{
    TC_HITTESTINFO hitTestInfo;
    hitTestInfo.pt.x = pt.x;
    hitTestInfo.pt.y = pt.y;
    int item = TabCtrl_HitTest( (HWND) GetHWND(), & hitTestInfo ) ;
    flags = 0;

    if ((hitTestInfo.flags & TCHT_NOWHERE) == TCHT_NOWHERE)
        flags |= wxTAB_HITTEST_NOWHERE;
    if ((hitTestInfo.flags & TCHT_ONITEMICON) == TCHT_ONITEMICON)
        flags |= wxTAB_HITTEST_ONICON;
    if ((hitTestInfo.flags & TCHT_ONITEMLABEL) == TCHT_ONITEMLABEL)
        flags |= wxTAB_HITTEST_ONLABEL;

    return item;
}

// Insert an item
bool wxTabCtrl::InsertItem(int item, const wxString& text, int imageId, void* data)
{
    wxChar buf[256];
    TC_ITEM tcItem;
    tcItem.mask = TCIF_PARAM;
    tcItem.lParam = (long) data;
    if (text != wxT(""))
    {
        tcItem.mask |= TCIF_TEXT;
        wxStrcpy(buf, (const wxChar*) text);
        tcItem.pszText = buf;
        tcItem.cchTextMax = 256;
    }
    if (imageId != -1)
    {
        tcItem.mask |= TCIF_IMAGE;
        tcItem.iImage = imageId;
    }

    return (TabCtrl_InsertItem( (HWND) GetHWND(), item, & tcItem) != -1);
}

// Set the selection
int wxTabCtrl::SetSelection(int item)
{
    return (int) TabCtrl_SetCurSel( (HWND) GetHWND(), item );
}

// Set the image list
void wxTabCtrl::SetImageList(wxImageList* imageList)
{
    m_imageList = imageList;
    TabCtrl_SetImageList( (HWND) GetHWND(), (HIMAGELIST) imageList->GetHIMAGELIST() );
}

// Set the text for an item
bool wxTabCtrl::SetItemText(int item, const wxString& text)
{
    wxChar buf[256];
    TC_ITEM tcItem;
    tcItem.mask = TCIF_TEXT;
    wxStrcpy(buf, (const wxChar*) text);
    tcItem.pszText = buf;
    tcItem.cchTextMax = 256;

    return ( TabCtrl_SetItem( (HWND) GetHWND(), item, & tcItem) != 0 );
}

// Set the image for an item
bool wxTabCtrl::SetItemImage(int item, int image)
{
    TC_ITEM tcItem;
    tcItem.mask = TCIF_IMAGE;
    tcItem.iImage = image;

    return ( TabCtrl_SetItem( (HWND) GetHWND(), item, & tcItem) != 0 );
}

// Set the data for an item
bool wxTabCtrl::SetItemData(int item, void* data)
{
    TC_ITEM tcItem;
    tcItem.mask = TCIF_PARAM;
    tcItem.lParam = (long) data;

    return ( TabCtrl_SetItem( (HWND) GetHWND(), item, & tcItem) != 0 );
}

// Set the size for a fixed-width tab control
void wxTabCtrl::SetItemSize(const wxSize& size)
{
    TabCtrl_SetItemSize( (HWND) GetHWND(), size.x, size.y );
}

// Set the padding between tabs
void wxTabCtrl::SetPadding(const wxSize& padding)
{
    TabCtrl_SetPadding( (HWND) GetHWND(), padding.x, padding.y );
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


#endif
    // __WIN95__
