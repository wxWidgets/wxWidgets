/////////////////////////////////////////////////////////////////////////////
// Name:        msw/tbar95.cpp
// Purpose:     wxToolBar95
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tbar95.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/dynarray.h"
    #include "wx/settings.h"
    #include "wx/bitmap.h"
#endif

#if wxUSE_BUTTONBAR && wxUSE_TOOLBAR && defined(__WIN95__)

#if !defined(__GNUWIN32__) && !defined(__SALFORDC__)
    #include "malloc.h"
#endif

#include "wx/msw/private.h"

#ifndef __TWIN32__

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#else
    #include <commctrl.h>
#endif

#endif // __TWIN32__

#include "wx/msw/dib.h"
#include "wx/tbar95.h"
#include "wx/app.h"         // for GetComCtl32Version

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// these standard constants are not always defined in compilers headers

// Styles
#ifndef TBSTYLE_FLAT
    #define TBSTYLE_LIST            0x1000
    #define TBSTYLE_FLAT            0x0800
    #define TBSTYLE_TRANSPARENT     0x8000
#endif
 // use TBSTYLE_TRANSPARENT if you use TBSTYLE_FLAT

// Messages
#ifndef TB_GETSTYLE
    #define TB_GETSTYLE             (WM_USER + 57)
    #define TB_SETSTYLE             (WM_USER + 56)
#endif

// these values correspond to those used by comctl32.dll
#define DEFAULTBITMAPX   16
#define DEFAULTBITMAPY   15
#define DEFAULTBUTTONX   24
#define DEFAULTBUTTONY   24
#define DEFAULTBARHEIGHT 27

// ----------------------------------------------------------------------------
// function prototypes
// ----------------------------------------------------------------------------

static void wxMapBitmap(HBITMAP hBitmap, int width, int height);

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxToolBar, wxToolBarBase)
#endif

BEGIN_EVENT_TABLE(wxToolBar95, wxToolBarBase)
    EVT_MOUSE_EVENTS(wxToolBar95::OnMouseEvent)
    EVT_SYS_COLOUR_CHANGED(wxToolBar95::OnSysColourChanged)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxToolBar95 construction
// ----------------------------------------------------------------------------

void wxToolBar95::Init()
{
    m_maxWidth = -1;
    m_maxHeight = -1;
    m_hBitmap = 0;
    m_defaultWidth = DEFAULTBITMAPX;
    m_defaultHeight = DEFAULTBITMAPY;
}

bool wxToolBar95::Create(wxWindow *parent,
                         wxWindowID id,
                         const wxPoint& pos,
                         const wxSize& size,
                         long style,
                         const wxString& name)
{
    wxASSERT_MSG( (style & wxTB_VERTICAL) == 0,
                  wxT("Sorry, wxToolBar95 under Windows 95 only "
                      "supports horizontal orientation.") );

    // common initialisation
    if ( !CreateControl(parent, id, pos, size, style, name) )
        return FALSE;

    // prepare flags
    DWORD msflags = 0;      // WS_VISIBLE | WS_CHILD always included
    if (style & wxBORDER)
        msflags |= WS_BORDER;
    msflags |= TBSTYLE_TOOLTIPS;

    if (style & wxTB_FLAT)
    {
        if (wxTheApp->GetComCtl32Version() > 400)
            msflags |= TBSTYLE_FLAT;
    }

    // MSW-specific initialisation
    if ( !wxControl::MSWCreateControl(TOOLBARCLASSNAME, msflags) )
        return FALSE;

    // toolbar-specific post initialisation
    ::SendMessage(GetHwnd(), TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

    // set up the colors and fonts
    wxRGBToColour(m_backgroundColour, GetSysColor(COLOR_BTNFACE));
    m_foregroundColour = *wxBLACK;

    SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

    // position it
    int x = pos.x;
    int y = pos.y;
    int width = size.x;
    int height = size.y;

    if (width <= 0)
        width = 100;
    if (height <= 0)
        height = m_defaultHeight;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;

    SetSize(x, y, width, height);

    return TRUE;
}

wxToolBar95::~wxToolBar95()
{
    UnsubclassWin();

    if (m_hBitmap)
    {
        ::DeleteObject((HBITMAP) m_hBitmap);
    }
}

// ----------------------------------------------------------------------------
// adding/removing buttons
// ----------------------------------------------------------------------------

void wxToolBar95::ClearTools()
{
    // TODO: Don't know how to reset the toolbar bitmap, as yet.
    // But adding tools and calling CreateTools should probably
    // recreate a buttonbar OK.
    wxToolBarBase::ClearTools();
}

bool wxToolBar95::DeleteTool(int id)
{
    int index = GetIndexFromId(id);
    wxASSERT_MSG( index != wxNOT_FOUND, _T("invalid toolbar button id") );

    if ( !SendMessage(GetHwnd(), TB_DELETEBUTTON, index, 0) )
    {
        wxLogLastError("TB_DELETEBUTTON");

        return FALSE;
    }

    wxNode *node = m_tools.Nth(index);
    delete (wxToolBarTool *)node->Data();
    m_tools.DeleteNode(node);

    m_ids.RemoveAt(index);

    return TRUE;
}

bool wxToolBar95::AddControl(wxControl *control)
{
    wxCHECK_MSG( control, FALSE, _T("toolbar: can't insert NULL control") );

    wxCHECK_MSG( control->GetParent() == this, FALSE,
                 _T("control must have toolbar as parent") );

    wxToolBarTool *tool = new wxToolBarTool(control);

    m_tools.Append(control->GetId(), tool);
    m_ids.Add(control->GetId());

    return TRUE;
}

wxToolBarTool *wxToolBar95::AddTool(int index,
                                    const wxBitmap& bitmap,
                                    const wxBitmap& pushedBitmap,
                                    bool toggle,
                                    long xPos, long yPos,
                                    wxObject *clientData,
                                    const wxString& helpString1,
                                    const wxString& helpString2)
{
    wxToolBarTool *tool = new wxToolBarTool(index, bitmap, wxNullBitmap,
                                            toggle, xPos, yPos,
                                            helpString1, helpString2);
    tool->m_clientData = clientData;

    if (xPos > -1)
        tool->m_x = xPos;
    else
        tool->m_x = m_xMargin;

    if (yPos > -1)
        tool->m_y = yPos;
    else
        tool->m_y = m_yMargin;

    tool->SetSize(GetToolSize().x, GetToolSize().y);

    m_tools.Append((long)index, tool);
    m_ids.Add(index);

    return tool;
}

bool wxToolBar95::CreateTools()
{
    size_t nTools = m_tools.GetCount();
    if ( nTools == 0 )
        return FALSE;

    HBITMAP oldToolBarBitmap = (HBITMAP) m_hBitmap;

    int totalBitmapWidth = (int)(m_defaultWidth * nTools);
    int totalBitmapHeight = (int)m_defaultHeight;

    // Create a bitmap for all the tool bitmaps
    HDC dc = ::GetDC(NULL);
    m_hBitmap = (WXHBITMAP) ::CreateCompatibleBitmap(dc,
                                                     totalBitmapWidth,
                                                     totalBitmapHeight);
    ::ReleaseDC(NULL, dc);

    // Now blit all the tools onto this bitmap
    HDC memoryDC = ::CreateCompatibleDC(NULL);
    HBITMAP oldBitmap = (HBITMAP) ::SelectObject(memoryDC, (HBITMAP)m_hBitmap);

    HDC memoryDC2 = ::CreateCompatibleDC(NULL);

    // the button position
    wxCoord x = 0;

    // the number of buttons (not separators)
    int noButtons = 0;

    wxNode *node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        if ( tool->m_toolStyle == wxTOOL_STYLE_BUTTON && tool->m_bitmap1.Ok() )
        {
            HBITMAP hbmp = GetHbitmapOf(tool->m_bitmap1);
            if ( hbmp )
            {
                HBITMAP oldBitmap2 = (HBITMAP)::SelectObject(memoryDC2, hbmp);
                if ( !BitBlt(memoryDC, x, 0,  m_defaultWidth, m_defaultHeight,
                             memoryDC2, 0, 0, SRCCOPY) )
                {
                    wxLogLastError("BitBlt");
                }

                ::SelectObject(memoryDC2, oldBitmap2);

                x += m_defaultWidth;
                noButtons++;
            }
        }
        node = node->Next();
    }

    ::SelectObject(memoryDC, oldBitmap);
    ::DeleteDC(memoryDC);
    ::DeleteDC(memoryDC2);

    // Map to system colours
    wxMapBitmap((HBITMAP) m_hBitmap, totalBitmapWidth, totalBitmapHeight);

    if ( oldToolBarBitmap )
    {
        TBREPLACEBITMAP replaceBitmap;
        replaceBitmap.hInstOld = NULL;
        replaceBitmap.hInstNew = NULL;
        replaceBitmap.nIDOld = (UINT) oldToolBarBitmap;
        replaceBitmap.nIDNew = (UINT) (HBITMAP) m_hBitmap;
        replaceBitmap.nButtons = noButtons;
        if ( ::SendMessage(GetHwnd(), TB_REPLACEBITMAP,
                           0, (LPARAM) &replaceBitmap) == -1 )
        {
            wxFAIL_MSG(wxT("Could not add bitmap to toolbar"));
        }

        ::DeleteObject((HBITMAP) oldToolBarBitmap);

        // Now delete all the buttons
        int i = 0;
        while ( TRUE )
        {
            // TODO: What about separators???? They don't have an id!
            if ( ! ::SendMessage( GetHwnd(), TB_DELETEBUTTON, i, 0 ) )
                break;
        }
    }
    else
    {
        TBADDBITMAP addBitmap;
        addBitmap.hInst = 0;
        addBitmap.nID = (UINT)m_hBitmap;
        if ( ::SendMessage(GetHwnd(), TB_ADDBITMAP,
                           (WPARAM) noButtons, (LPARAM)&addBitmap) == -1 )
        {
            wxFAIL_MSG(wxT("Could not add bitmap to toolbar"));
        }
    }

    // Now add the buttons.
    TBBUTTON *buttons = new TBBUTTON[nTools];

    // this array will holds the indices of all controls in the toolbar
    wxArrayInt controlIds;

    int i = 0;
    int bitmapId = 0;

    node = m_tools.First();
    while (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        TBBUTTON& button = buttons[i];

        wxZeroMemory(button);

        switch ( tool->m_toolStyle )
        {
            case wxTOOL_STYLE_CONTROL:
                controlIds.Add(i);
                button.idCommand = tool->m_index;
                // fall through: create just a separator too

            case wxTOOL_STYLE_SEPARATOR:
                button.fsState = TBSTATE_ENABLED;
                button.fsStyle = TBSTYLE_SEP;
                break;

            case wxTOOL_STYLE_BUTTON:
                button.iBitmap = bitmapId;
                button.idCommand = tool->m_index;

                if (tool->m_enabled)
                    button.fsState |= TBSTATE_ENABLED;
                if (tool->m_toggleState)
                    button.fsState |= TBSTATE_CHECKED;
                button.fsStyle = tool->m_isToggle ? TBSTYLE_CHECK
                                                  : TBSTYLE_BUTTON;

                bitmapId++;
                break;
        }

        i++;
        node = node->Next();
    }

    if ( !::SendMessage(GetHwnd(), TB_ADDBUTTONS,
                        (WPARAM)i, (LPARAM)buttons) )
    {
        wxLogLastError("TB_ADDBUTTONS");
    }

    delete [] buttons;

    // adjust the controls size to fit nicely in the toolbar
    size_t nControls = controlIds.GetCount();
    for ( size_t nCtrl = 0; nCtrl < nControls; nCtrl++ )
    {
        wxToolBarTool *tool = (wxToolBarTool *)
                                m_tools.Nth(controlIds[nCtrl])->Data();
        wxControl *control = tool->GetControl();

        wxSize size = control->GetSize();

        // the position of the leftmost controls corner
        int left = -1;

        // TB_SETBUTTONINFO message is only supported by comctl32.dll 4.71+
        #if defined(_WIN32_IE) && (_WIN32_IE >= 0x400 )
            // available in headers, now check whether it is available now
            // (during run-time)
            if ( wxTheApp->GetComCtl32Version() >= 471 )
            {
                // set the (underlying) separators width to be that of the
                // control
                TBBUTTONINFO tbbi;
                tbbi.cbSize = sizeof(tbbi);
                tbbi.dwMask = TBIF_SIZE;
                tbbi.cx = size.x;
                if ( !SendMessage(GetHwnd(), TB_SETBUTTONINFO,
                                  tool->m_index, (LPARAM)&tbbi) )
                {
                    // the index is probably invalid
                    wxLogLastError("TB_SETBUTTONINFO");
                }

            }
            else
        #endif // comctl32.dll 4.71
            // TB_SETBUTTONINFO unavailable
            {
                int index = GetIndexFromId(tool->m_index);
                wxASSERT_MSG( index != wxNOT_FOUND,
                              _T("control wasn't added to the tbar?") );

                // try adding several separators to fit the controls width
                RECT r;
                if ( !SendMessage(GetHwnd(), TB_GETRECT,
                                  tool->m_index, (LPARAM)(LPRECT)&r) )
                {
                    wxLogLastError("TB_GETITEMRECT");
                }

                int widthSep = r.right - r.left;
                left = r.left;

                TBBUTTON tbb;
                wxZeroMemory(tbb);
                tbb.idCommand = 0;
                tbb.fsState = TBSTATE_ENABLED;
                tbb.fsStyle = TBSTYLE_SEP;

                size_t nSeparators = size.x / widthSep;
                for ( size_t nSep = 0; nSep < nSeparators; nSep++ )
                {
                    m_ids.Insert(0, (size_t)index);

                    if ( !SendMessage(GetHwnd(), TB_INSERTBUTTON,
                                      index, (LPARAM)&tbb) )
                    {
                        wxLogLastError("TB_INSERTBUTTON");
                    }
                }

                // adjust the controls width to exactly cover the separators
                control->SetSize((nSeparators + 1)*widthSep, -1);
            }

        // and position the control itself correctly vertically
        RECT r;
        if ( !SendMessage(GetHwnd(), TB_GETRECT,
                          tool->m_index, (LPARAM)(LPRECT)&r) )
        {
            wxLogLastError("TB_GETRECT");
        }

        int height = r.bottom - r.top;
        int diff = height - size.y;
        if ( diff < 0 )
        {
            // the control is too high, resize to fit
            control->SetSize(-1, height - 2);

            diff = 2;
        }

        control->Move(left == -1 ? r.left : left, r.top + diff / 2);
    }

    (void)::SendMessage(GetHwnd(), TB_AUTOSIZE, (WPARAM)0, (LPARAM) 0);

    SetRows(m_maxRows);

    return TRUE;
}

// ----------------------------------------------------------------------------
// message handlers
// ----------------------------------------------------------------------------

bool wxToolBar95::MSWCommand(WXUINT cmd, WXWORD id)
{
    wxNode *node = m_tools.Find((long)id);
    if (!node)
        return FALSE;

    wxToolBarTool *tool = (wxToolBarTool *)node->Data();
    if (tool->m_isToggle)
    {
        LRESULT state = ::SendMessage(GetHwnd(), TB_GETSTATE, id, 0);
        tool->m_toggleState = state & TBSTATE_CHECKED;
    }

    BOOL ret = OnLeftClick((int)id, tool->m_toggleState);
    if ( ret == FALSE && tool->m_isToggle )
    {
        tool->m_toggleState = !tool->m_toggleState;
        ::SendMessage(GetHwnd(), TB_CHECKBUTTON,
                      (WPARAM)id, (LPARAM)MAKELONG(tool->m_toggleState, 0));
    }

    return TRUE;
}

bool wxToolBar95::MSWOnNotify(int WXUNUSED(idCtrl),
                            WXLPARAM lParam,
                            WXLPARAM *result)
{
    // First check if this applies to us
    NMHDR *hdr = (NMHDR *)lParam;

    // the tooltips control created by the toolbar is sometimes Unicode, even
    // in an ANSI application - this seems to be a bug in comctl32.dll v5
    int code = (int)hdr->code;
    if ( (code != TTN_NEEDTEXTA) && (code != TTN_NEEDTEXTW) )
        return FALSE;

    HWND toolTipWnd = (HWND)::SendMessage((HWND)GetHWND(), TB_GETTOOLTIPS, 0, 0);
    if ( toolTipWnd != hdr->hwndFrom )
        return FALSE;

    LPTOOLTIPTEXT ttText = (LPTOOLTIPTEXT)lParam;
    int id = (int)ttText->hdr.idFrom;
    wxNode *node = m_tools.Find((long)id);
    if (!node)
        return FALSE;

    wxToolBarTool *tool = (wxToolBarTool *)node->Data();

    const wxString& help = tool->m_shortHelpString;

    if ( !help.IsEmpty() )
    {
        if ( code == TTN_NEEDTEXTA )
        {
            ttText->lpszText = (wxChar *)help.c_str();
        }
#if (_WIN32_IE >= 0x0300)
        else
        {
            // FIXME this is a temp hack only until I understand better what
            //       must be done in both ANSI and Unicode builds

            size_t lenAnsi = help.Len();
            #ifdef __MWERKS__
                // MetroWerks doesn't like calling mbstowcs with NULL argument
                size_t lenUnicode = 2*lenAnsi;
            #else
                size_t lenUnicode = mbstowcs(NULL, help, lenAnsi);
            #endif

            // using the pointer of right type avoids us doing all sorts of
            // pointer arithmetics ourselves
            wchar_t *dst = (wchar_t *)ttText->szText,
                    *pwz = new wchar_t[lenUnicode + 1];
            mbstowcs(pwz, help, lenAnsi + 1);
            memcpy(dst, pwz, lenUnicode*sizeof(wchar_t));

            // put the terminating _wide_ NUL
            dst[lenUnicode] = 0;

            delete [] pwz;
        }
#endif // _WIN32_IE >= 0x0300
    }

    // For backward compatibility...
    OnMouseEnter(tool->m_index);

    return TRUE;
}

// ----------------------------------------------------------------------------
// sizing stuff
// ----------------------------------------------------------------------------

void wxToolBar95::SetToolBitmapSize(const wxSize& size)
{
    wxToolBarBase::SetToolBitmapSize(size);

    ::SendMessage(GetHwnd(), TB_SETBITMAPSIZE, 0, MAKELONG(size.x, size.y));
}

void wxToolBar95::SetRows(int nRows)
{
    // TRUE in wParam means to create at least as many rows
    RECT rect;
    ::SendMessage(GetHwnd(), TB_SETROWS,
                  MAKEWPARAM(nRows, TRUE), (LPARAM) &rect);

    m_maxWidth = (rect.right - rect.left + 2);
    m_maxHeight = (rect.bottom - rect.top + 2);

    m_maxRows = nRows;
}

wxSize wxToolBar95::GetMaxSize() const
{
    if ( (m_maxWidth == -1) || (m_maxHeight == -1) )
    {
        // it has a side effect of filling m_maxWidth/Height variables
        ((wxToolBar95 *)this)->SetRows(m_maxRows);  // const_cast
    }

    return wxSize(m_maxWidth, m_maxHeight);
}

// The button size is bigger than the bitmap size
wxSize wxToolBar95::GetToolSize() const
{
    // FIXME: this is completely bogus (VZ)
    return wxSize(m_defaultWidth + 8, m_defaultHeight + 7);
}

// ----------------------------------------------------------------------------
// tool state
// ----------------------------------------------------------------------------

void wxToolBar95::EnableTool(int toolIndex, bool enable)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        tool->m_enabled = enable;
        ::SendMessage(GetHwnd(), TB_ENABLEBUTTON,
                      (WPARAM)toolIndex, (LPARAM)MAKELONG(enable, 0));
    }
}

void wxToolBar95::ToggleTool(int toolIndex, bool toggle)
{
    wxNode *node = m_tools.Find((long)toolIndex);
    if (node)
    {
        wxToolBarTool *tool = (wxToolBarTool *)node->Data();
        if (tool->m_isToggle)
        {
            tool->m_toggleState = toggle;
            ::SendMessage(GetHwnd(), TB_CHECKBUTTON,
                          (WPARAM)toolIndex, (LPARAM)MAKELONG(toggle, 0));
        }
    }
}

bool wxToolBar95::GetToolState(int toolIndex) const
{
    wxASSERT_MSG( GetIndexFromId(toolIndex) != wxNOT_FOUND,
                  _T("invalid toolbar button id") );

    return ::SendMessage(GetHwnd(), TB_ISBUTTONCHECKED,
                         (WPARAM)toolIndex, (LPARAM)0) != 0;
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

// Responds to colour changes, and passes event on to children.
void wxToolBar95::OnSysColourChanged(wxSysColourChangedEvent& event)
{
    m_backgroundColour = wxColour(GetRValue(GetSysColor(COLOR_BTNFACE)),
          GetGValue(GetSysColor(COLOR_BTNFACE)), GetBValue(GetSysColor(COLOR_BTNFACE)));

    // Remap the buttons
    CreateTools();

    Refresh();

    // Propagate the event to the non-top-level children
    wxWindow::OnSysColourChanged(event);
}

void wxToolBar95::OnMouseEvent(wxMouseEvent& event)
{
    if (event.RightDown())
    {
        // For now, we don't have an id. Later we could
        // try finding the tool.
        OnRightClick((int)-1, event.GetX(), event.GetY());
    }
    else
    {
        event.Skip();
    }
}


// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

int wxToolBar95::GetIndexFromId(int id) const
{
    size_t count = m_ids.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if ( m_ids[n] == id )
            return n;
    }

    return wxNOT_FOUND;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// These are the default colors used to map the bitmap colors to the current
// system colors. Note that they are in BGR format because this is what Windows
// wants (and not RGB)

#define BGR_BUTTONTEXT      (RGB(000,000,000))  // black
#define BGR_BUTTONSHADOW    (RGB(128,128,128))  // dark grey
#define BGR_BUTTONFACE      (RGB(192,192,192))  // bright grey
#define BGR_BUTTONHILIGHT   (RGB(255,255,255))  // white
#define BGR_BACKGROUNDSEL   (RGB(255,000,000))  // blue
#define BGR_BACKGROUND      (RGB(255,000,255))  // magenta

void wxMapBitmap(HBITMAP hBitmap, int width, int height)
{
  COLORMAP ColorMap[] =
  {
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
    hbmOld = (HBITMAP) SelectObject(hdcMem, hBitmap);

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

// Some experiments...
#if 0
  // What we want to do is create another bitmap which has a depth of 4,
  // and set the bits. So probably we want to convert this HBITMAP into a
  // DIB, then call SetDIBits.
  // AAAGH. The stupid thing is that if newBitmap has a depth of 4 (less than that of
  // the screen), then SetDIBits fails.
  HBITMAP newBitmap = ::CreateBitmap(totalBitmapWidth, totalBitmapHeight, 1, 4, NULL);
  HANDLE newDIB = ::BitmapToDIB((HBITMAP) m_hBitmap, NULL);
  LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER) GlobalLock(newDIB);

  dc = ::GetDC(NULL);
//  LPBITMAPINFOHEADER lpbmi = (LPBITMAPINFOHEADER) newDIB;

  int result = ::SetDIBits(dc, newBitmap, 0, lpbmi->biHeight, FindDIBBits((LPSTR)lpbmi), (LPBITMAPINFO)lpbmi,
    DIB_PAL_COLORS);
  DWORD err = GetLastError();

  ::ReleaseDC(NULL, dc);

  // Delete the DIB
  GlobalUnlock (newDIB);
  GlobalFree (newDIB);

//  WXHBITMAP hBitmap2 = wxCreateMappedBitmap((WXHINSTANCE) wxGetInstance(), (WXHBITMAP) m_hBitmap);
  // Substitute our new bitmap for the old one
  ::DeleteObject((HBITMAP) m_hBitmap);
  m_hBitmap = (WXHBITMAP) newBitmap;
#endif


#endif // !(wxUSE_TOOLBAR && Win95)
