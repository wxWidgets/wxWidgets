///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.06.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_NOTEBOOK

#include "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcctl.h" // include <commctrl.h> "properly"
    #include "wx/string.h"
    #include "wx/dc.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/app.h"
    #include "wx/dcclient.h"
    #include "wx/dcmemory.h"
    #include "wx/settings.h"
#endif  // WX_PRECOMP

#include "wx/imaglist.h"
#include "wx/renderer.h"
#include "wx/sysopt.h"

#include "wx/msw/private.h"
#include "wx/msw/dc.h"

#include <windowsx.h>
#include "wx/msw/winundef.h"

#include "wx/msw/private/darkmode.h"

#if wxUSE_UXTHEME
    #include "wx/msw/uxtheme.h"
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())

// you can set USE_NOTEBOOK_ANTIFLICKER to 0 for desktop Windows versions too
// to disable code whih results in flicker-less notebook redrawing at the
// expense of some extra GDI resource consumption
#define USE_NOTEBOOK_ANTIFLICKER    1

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// This is a work-around for missing defines in gcc-2.95 headers
#ifndef TCS_RIGHT
    #define TCS_RIGHT       0x0002
#endif

#ifndef TCS_VERTICAL
    #define TCS_VERTICAL    0x0080
#endif

#ifndef TCS_BOTTOM
    #define TCS_BOTTOM      TCS_RIGHT
#endif

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

#if USE_NOTEBOOK_ANTIFLICKER

// the pointer to standard spin button wnd proc
static WXWNDPROC gs_wndprocNotebookSpinBtn = nullptr;

// the pointer to standard tab control wnd proc
static WXWNDPROC gs_wndprocNotebook = nullptr;

LRESULT APIENTRY
wxNotebookWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#endif // USE_NOTEBOOK_ANTIFLICKER

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

static bool HasTroubleWithNonTopTabs()
{
    const int verComCtl32 = wxApp::GetComCtl32Version();

    // 600 is XP, 616 is Vista -- and both have a problem with tabs not on top
    // (but don't just test for >= 600 as Microsoft might decide to fix it in
    // later versions, who knows...)
    return verComCtl32 >= 600 && verComCtl32 <= 616;
}

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxNotebook, wxBookCtrlBase)
    EVT_SIZE(wxNotebook::OnSize)
    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
wxEND_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

// common part of all ctors
void wxNotebook::Init()
{
#if wxUSE_UXTHEME
    m_hbrBackground = nullptr;
#endif // wxUSE_UXTHEME

#if USE_NOTEBOOK_ANTIFLICKER
    m_hasSubclassedUpdown = false;
#endif // USE_NOTEBOOK_ANTIFLICKER
}

// default for dynamic class
wxNotebook::wxNotebook()
{
  Init();
}

// the same arguments as for wxControl
wxNotebook::wxNotebook(wxWindow *parent,
                       wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxString& name)
{
  Init();

  Create(parent, id, pos, size, style, name);
}

// Create() function
bool wxNotebook::Create(wxWindow *parent,
                        wxWindowID id,
                        const wxPoint& pos,
                        const wxSize& size,
                        long style,
                        const wxString& name)
{
    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
    {
        style |= wxBK_TOP;
    }

#if !wxUSE_UXTHEME
    // ComCtl32 notebook tabs simply don't work unless they're on top if we
    // have uxtheme, we can work around it later (after control creation), but
    // if we have been compiled without uxtheme support, we have to clear those
    // styles
    if ( HasTroubleWithNonTopTabs() )
    {
        style &= ~(wxBK_BOTTOM | wxBK_LEFT | wxBK_RIGHT);
    }
#endif //wxUSE_UXTHEME

    LPCTSTR className = WC_TABCONTROL;

#if USE_NOTEBOOK_ANTIFLICKER
    // SysTabCtl32 class has natively CS_HREDRAW and CS_VREDRAW enabled and it
    // causes horrible flicker when resizing notebook, so get rid of it by
    // using a class without these styles (but otherwise identical to it)
    if ( !HasFlag(wxFULL_REPAINT_ON_RESIZE) )
    {
        static ClassRegistrar s_clsNotebook;
        if ( !s_clsNotebook.IsInitialized() )
        {
            // get a copy of standard class and modify it
            WNDCLASS wc;

            if ( ::GetClassInfo(nullptr, WC_TABCONTROL, &wc) )
            {
                gs_wndprocNotebook = wc.lpfnWndProc;
                wc.lpszClassName = wxT("_wx_SysTabCtl32");
                wc.style &= ~(CS_HREDRAW | CS_VREDRAW);
                wc.hInstance = wxGetInstance();
                wc.lpfnWndProc = wxNotebookWndProc;
                s_clsNotebook.Register(wc);
            }
            else
            {
                wxLogLastError(wxT("GetClassInfoEx(SysTabCtl32)"));
            }
        }

        // use our custom class if available but fall back to the standard
        // notebook if we failed to register it
        if ( s_clsNotebook.IsRegistered() )
        {
            // it's ok to use c_str() here as the static s_clsNotebook object
            // has sufficiently long lifetime
            className = s_clsNotebook.GetName().c_str();
        }
    }
#endif // USE_NOTEBOOK_ANTIFLICKER

    if ( !CreateControl(parent, id, pos, size, style | wxTAB_TRAVERSAL,
                        wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(className, wxEmptyString, pos, size) )
        return false;

    Bind(wxEVT_PAINT, &wxNotebook::OnPaint, this);

    // Inherit parent attributes and, unlike the default, also inherit the
    // parent background colour in order to blend in with its background if
    // it's set to a non-default value -- or if we're using dark mode, in which
    // the default colour always needs to be changed.
    InheritAttributes();
    if ( !UseBgCol() )
    {
        wxColour colBg;
        if ( parent->InheritsBackgroundColour() )
        {
            colBg = parent->GetBackgroundColour();
        }
        else if ( wxMSWDarkMode::IsActive() )
        {
            colBg = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);

            // We also need to change the foreground in this case to ensure a
            // good contrast with the dark background.
            SetForegroundColour(
                wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)
            );
        }

        if ( colBg.IsOk() )
            SetBackgroundColour(colBg);
    }

#if wxUSE_UXTHEME
    if ( HasFlag(wxNB_NOPAGETHEME) ||
            wxSystemOptions::IsFalse(wxT("msw.notebook.themed-background")) )
    {
        SetBackgroundColour(GetThemeBackgroundColour());
    }
    else // use themed background by default
    {
        // create backing store
        UpdateBgBrush();
    }

    // comctl32.dll 6.0 doesn't support non-top tabs with visual styles (the
    // control is simply not rendered correctly), so we disable themes
    // if possible, otherwise we simply clear the styles.
    if ( HasTroubleWithNonTopTabs() &&
            (style & (wxBK_BOTTOM | wxBK_LEFT | wxBK_RIGHT)) )
    {
        // check if we use themes at all -- if we don't, we're still okay
        if ( wxUxThemeIsActive() )
        {
            ::SetWindowTheme(GetHwnd(), L"", L"");

            // correct the background color for the new non-themed control
            SetBackgroundColour(GetThemeBackgroundColour());
        }
    }
#endif // wxUSE_UXTHEME

    return true;
}

WXDWORD wxNotebook::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    WXDWORD tabStyle = wxControl::MSWGetStyle(style, exstyle);

    tabStyle |= WS_TABSTOP | TCS_TABS;

    if ( style & wxNB_MULTILINE )
        tabStyle |= TCS_MULTILINE;
    if ( style & wxNB_FIXEDWIDTH )
        tabStyle |= TCS_FIXEDWIDTH;

    if ( style & wxBK_BOTTOM )
        tabStyle |= TCS_RIGHT;
    else if ( style & wxBK_LEFT )
        tabStyle |= TCS_VERTICAL;
    else if ( style & wxBK_RIGHT )
        tabStyle |= TCS_VERTICAL | TCS_RIGHT;

    return tabStyle;
}

int wxNotebook::MSWGetToolTipMessage() const
{
    return TCM_GETTOOLTIPS;
}

wxNotebook::~wxNotebook()
{
#if wxUSE_UXTHEME
    if ( m_hbrBackground )
        ::DeleteObject((HBRUSH)m_hbrBackground);
#endif // wxUSE_UXTHEME
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------

size_t wxNotebook::GetPageCount() const
{
    // consistency check
    wxASSERT( (int)m_pages.size() == TabCtrl_GetItemCount(GetHwnd()) );

    return m_pages.size();
}

int wxNotebook::GetRowCount() const
{
    return TabCtrl_GetRowCount(GetHwnd());
}

int wxNotebook::SetSelection(size_t nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), wxNOT_FOUND, wxT("notebook page out of range") );

    if ( m_selection == wxNOT_FOUND || nPage != (size_t)m_selection )
    {
        if ( SendPageChangingEvent(nPage) )
        {
            // program allows the page change
            const int selectionOld = m_selection;

            UpdateSelection(nPage);

            (void)TabCtrl_SetCurSel(GetHwnd(), nPage);

            SendPageChangedEvent(selectionOld, nPage);
        }
    }

    return m_selection;
}

void wxNotebook::UpdateSelection(int selNew)
{
    if ( m_selection != wxNOT_FOUND )
        m_pages[m_selection]->Show(false);

    if ( selNew != wxNOT_FOUND )
    {
        wxNotebookPage *pPage = m_pages[selNew];
        pPage->Show(true);

        // In addition to showing the page, we also want to give focus to it to
        // make it possible to work with it from keyboard easily. However there
        // are two exceptions: first, don't touch the focus at all if the
        // notebook itself is not currently shown.
        if ( ::IsWindowVisible(GetHwnd()) )
        {
            // And second, don't give focus away if the tab control itself has
            // it, as this is how the native property sheets behave: if you
            // explicitly click on the tab label giving it focus, it will
            // remain after switching to another page. But if the focus was
            // inside the notebook page, it switches to the new page.
            if ( !HasFocus() )
                pPage->SetFocus();
        }
    }

    m_selection = selNew;
}

int wxNotebook::ChangeSelection(size_t nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), wxNOT_FOUND, wxT("notebook page out of range") );

    const int selOld = m_selection;

    if ( m_selection == wxNOT_FOUND || nPage != (size_t)m_selection )
    {
        (void)TabCtrl_SetCurSel(GetHwnd(), nPage);

        UpdateSelection(nPage);
    }

    return selOld;
}

bool wxNotebook::SetPageText(size_t nPage, const wxString& strText)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), false, wxT("notebook page out of range") );

    TC_ITEM tcItem;
    tcItem.mask = TCIF_TEXT;
    tcItem.pszText = wxMSW_CONV_LPTSTR(strText);

    if ( !HasFlag(wxNB_MULTILINE) )
        return TabCtrl_SetItem(GetHwnd(), nPage, &tcItem) != 0;

    // multiline - we need to set new page size if a line is added or removed
    int rows = GetRowCount();
    bool ret = TabCtrl_SetItem(GetHwnd(), nPage, &tcItem) != 0;

    if ( ret && rows != GetRowCount() )
    {
        const wxRect r = GetPageSize();
        const size_t count = m_pages.size();
        for ( size_t page = 0; page < count; page++ )
            m_pages[page]->SetSize(r);
    }

    return ret;
}

wxString wxNotebook::GetPageText(size_t nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), wxEmptyString, wxT("notebook page out of range") );

    wxChar buf[256];
    TC_ITEM tcItem;
    tcItem.mask = TCIF_TEXT;
    tcItem.pszText = buf;
    tcItem.cchTextMax = WXSIZEOF(buf);

    wxString str;
    if ( TabCtrl_GetItem(GetHwnd(), nPage, &tcItem) )
        str = tcItem.pszText;

    return str;
}

int wxNotebook::GetPageImage(size_t nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), wxNOT_FOUND, wxT("notebook page out of range") );

    TC_ITEM tcItem;
    tcItem.mask = TCIF_IMAGE;

    return TabCtrl_GetItem(GetHwnd(), nPage, &tcItem) ? tcItem.iImage
                                                      : wxNOT_FOUND;
}

bool wxNotebook::SetPageImage(size_t nPage, int nImage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), false, wxT("notebook page out of range") );

    TC_ITEM tcItem;
    tcItem.mask = TCIF_IMAGE;
    tcItem.iImage = nImage;

    return TabCtrl_SetItem(GetHwnd(), nPage, &tcItem) != 0;
}

void wxNotebook::OnImagesChanged()
{
    wxImageList* const iml = GetUpdatedImageListFor(this);

    (void) TabCtrl_SetImageList(GetHwnd(), iml ? GetHimagelistOf(iml) : nullptr);
}

// ----------------------------------------------------------------------------
// wxNotebook size settings
// ----------------------------------------------------------------------------

wxRect wxNotebook::GetPageSize() const
{
    wxRect r;

    RECT rc;
    ::GetClientRect(GetHwnd(), &rc);

    // This check is to work around a bug in TabCtrl_AdjustRect which will
    // cause a crash on win2k or on XP with themes disabled if either
    // wxNB_MULTILINE is used or tabs are placed on a side, if the rectangle
    // is too small.
    //
    // The value of 20 is chosen arbitrarily but seems to work
    if ( rc.right > 20 && rc.bottom > 20 )
    {
        (void)TabCtrl_AdjustRect(GetHwnd(), false, &rc);

        wxCopyRECTToRect(rc, r);
    }

    return r;
}

void wxNotebook::SetPageSize(const wxSize& size)
{
    // transform the page size into the notebook size
    RECT rc;
    rc.left =
    rc.top = 0;
    rc.right = size.x;
    rc.bottom = size.y;

    (void)TabCtrl_AdjustRect(GetHwnd(), true, &rc);

    // and now set it
    SetSize(rc.right - rc.left, rc.bottom - rc.top);
}

void wxNotebook::SetPadding(const wxSize& padding)
{
    TabCtrl_SetPadding(GetHwnd(), padding.x, padding.y);
}

// Windows-only at present. Also, you must use the wxNB_FIXEDWIDTH
// style.
void wxNotebook::SetTabSize(const wxSize& sz)
{
    ::SendMessage(GetHwnd(), TCM_SETITEMSIZE, 0, MAKELPARAM(sz.x, sz.y));
}

wxRect wxNotebook::GetTabRect(size_t page) const
{
    wxRect r;
    wxCHECK_MSG(IS_VALID_PAGE(page), r, wxT("invalid notebook page"));

    if (GetPageCount() > 0)
    {
        RECT rect;
        if (TabCtrl_GetItemRect(GetHwnd(), page, &rect))
            r = wxRectFromRECT(rect);
    }

    return r;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    // we can't use TabCtrl_AdjustRect here because it only works for wxNB_TOP
    wxSize sizeTotal = sizePage;

    wxSize tabSize;
    if ( GetPageCount() > 0 )
    {
        RECT rect;
        if ( TabCtrl_GetItemRect(GetHwnd(), 0, &rect) )
        {
            tabSize.x = rect.right - rect.left;
            tabSize.y = rect.bottom - rect.top;
        }
    }

    const int rows = GetRowCount();

    // add an extra margin in both directions
    const int MARGIN = 8;
    if ( IsVertical() )
    {
        sizeTotal.x += MARGIN;
        sizeTotal.y += tabSize.y * rows + MARGIN;
    }
    else // horizontal layout
    {
        sizeTotal.x += tabSize.x * rows + MARGIN;
        sizeTotal.y += MARGIN;
    }

    return sizeTotal;
}

void wxNotebook::AdjustPageSize(wxNotebookPage *page)
{
    wxCHECK_RET( page, wxT("null page in wxNotebook::AdjustPageSize") );

    const wxRect r = GetPageSize();
    if ( !r.IsEmpty() )
    {
        page->SetSize(r);
    }
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook, without deleting
wxNotebookPage *wxNotebook::DoRemovePage(size_t nPage)
{
    wxNotebookPage *pageRemoved = wxNotebookBase::DoRemovePage(nPage);
    if ( !pageRemoved )
        return nullptr;

    // hide the removed page to maintain the invariant that only the
    // selected page is visible and others are hidden:
    pageRemoved->Show(false);

    if ( !TabCtrl_DeleteItem(GetHwnd(), nPage) )
        wxLogLastError(wxS("TabCtrl_DeleteItem()"));

    if ( m_pages.empty() )
    {
        // no selection any more, the notebook becamse empty
        m_selection = wxNOT_FOUND;
    }
    else // notebook still not empty
    {
        int selNew = TabCtrl_GetCurSel(GetHwnd());
        if ( selNew != wxNOT_FOUND )
        {
            // No selection change, just refresh the current selection.
            // Because it could be that the slection index changed
            // we need to update it.
            // Note: this does not mean the selection it self changed.
            m_selection = selNew;
            m_pages[m_selection]->Refresh();
        }
        else if (int(nPage) == m_selection)
        {
            // The selection was deleted.

            // Determine new selection.
            if (m_selection == int(GetPageCount()))
                selNew = m_selection - 1;
            else
                selNew = m_selection;

            // m_selection must be always valid so reset it before calling
            // SetSelection()
            m_selection = wxNOT_FOUND;
            SetSelection(selNew);
        }
        else
        {
            wxFAIL; // Windows did not behave ok.
        }
    }

    return pageRemoved;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    wxBookCtrlBase::DeleteAllPages();

    if ( !TabCtrl_DeleteAllItems(GetHwnd()) )
        wxLogLastError(wxS("TabCtrl_DeleteAllItems()"));

    return true;
}

// same as AddPage() but does it at given position
bool wxNotebook::InsertPage(size_t nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    wxCHECK_MSG( pPage != nullptr, false, wxT("null page in wxNotebook::InsertPage") );
    wxCHECK_MSG( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), false,
                 wxT("invalid index in wxNotebook::InsertPage") );

    wxASSERT_MSG( pPage->GetParent() == this,
                    wxT("notebook pages must have notebook as parent") );

    // add a new tab to the control
    // ----------------------------

    // init all fields to 0
    TC_ITEM tcItem;
    wxZeroMemory(tcItem);

    // set the image, if any
    if ( imageId != -1 )
    {
        tcItem.mask |= TCIF_IMAGE;
        tcItem.iImage  = imageId;
    }

    // and the text
    if ( !strText.empty() )
    {
        tcItem.mask |= TCIF_TEXT;
        tcItem.pszText = wxMSW_CONV_LPTSTR(strText);
    }

    // hide the page: unless it is selected, it shouldn't be shown (and if it
    // is selected it will be shown later)
    pPage->Show(false);


    // fit the notebook page to the tab control's display area: this should be
    // done before adding it to the notebook or TabCtrl_InsertItem() will
    // change the notebooks size itself!
    AdjustPageSize(pPage);

    // finally do insert it
    if ( TabCtrl_InsertItem(GetHwnd(), nPage, &tcItem) == -1 )
    {
        wxLogError(wxT("Can't create the notebook page '%s'."), strText);

        return false;
    }

    // need to update the bg brush when the first page is added
    // so the first panel gets the correct themed background
    if ( m_pages.empty() )
    {
#if wxUSE_UXTHEME
        UpdateBgBrush();
#endif // wxUSE_UXTHEME
    }

    // succeeded: save the pointer to the page
    m_pages.insert(m_pages.begin() + nPage, pPage);

    // we may need to adjust the size again if the notebook size changed:
    // normally this only happens for the first page we add (the tabs which
    // hadn't been there before are now shown) but for a multiline notebook it
    // can happen for any page at all as a new row could have been started
    if ( m_pages.size() == 1 || HasFlag(wxNB_MULTILINE) )
    {
        AdjustPageSize(pPage);

        // Additionally, force the layout of the notebook itself by posting a
        // size event to it. If we don't do it, notebooks with pages on the
        // left or the right side may fail to account for the fact that they
        // are now big enough to fit all all of their pages on one row and
        // still reserve space for the second row of tabs, see #1792.
        const wxSize s = GetSize();
        ::PostMessage(GetHwnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM(s.x, s.y));
    }

    // now deal with the selection
    // ---------------------------

    // if the inserted page is before the selected one, we must update the
    // index of the selected page
    if ( int(nPage) <= m_selection )
    {
        // one extra page added
        m_selection++;
    }

    DoSetSelectionAfterInsertion(nPage, bSelect);

    InvalidateBestSize();

    return true;
}

// This can be defined to help with debugging this code by visually
// highlighting the rectangle passed to it.
#ifdef WXDEBUG_NOTEBOOK_HITTEST
static void ShowHitbox(wxWindow *w, wxRect r, bool text, bool refresh)
{
    wxBrush green(wxColor(0, 200, 0));
    wxBrush blue(wxColor(0, 0, 200));

    wxScreenDC dc;

    if (refresh)
    {
        w->Refresh();
        w->Update();
    }

    r.x += (r.width / 2)+2;
    w->ClientToScreen(&r.x, &r.y);
    if (text)
        dc.SetBrush(green);
    else
        dc.SetBrush(blue);

    dc.DrawRectangle(r);
}
#endif // WXDEBUG_NOTEBOOK_HITTEST

int wxNotebook::MSWHitTestLeftRight(const wxPoint& pt, long *flags) const
{
    long dummy;
    if (!flags)
        flags = &dummy;

    *flags = wxBK_HITTEST_NOWHERE;

    wxRect r = GetRect();
    if (!r.Contains(pt))
    {
        return -1;
    }

    const size_t pages = GetPageCount();
    if (!pages)
    {
        *flags |= wxBK_HITTEST_ONPAGE;
        return -1;
    }

    if (GetPageSize().Contains(pt))
    {
        *flags = wxBK_HITTEST_ONPAGE;
        return -1;
    }

    int item = wxNOT_FOUND;

    const wxDirection tabDir = GetTabOrientation();
    for (size_t i = 0; i < pages; i++)
    {
        r = GetTabRect(i);
        if (!r.Contains(pt))
            continue;

        item = i;

        // If the user doesn't care about the flags we are done here.
        if (flags == &dummy)
            break;

        // Default assumption.
        *flags = wxBK_HITTEST_ONITEM;

        // Just a magic number between the icon and the start of the text
        // I checked with different resolutions and it seems to be about right.
        // My assumption was that the icon and the label are both centered in
        // the tab and this assumption seems to be mostly right with this
        // adjustment offset being required.
        int gap = 8;

        TCHAR buffer[256];
        TCITEM ti;
        ti.mask = TCIF_IMAGE | TCIF_TEXT;
        ti.cchTextMax = WXSIZEOF(buffer);
        ti.pszText = buffer;
        TabCtrl_GetItem(GetHwnd(), i, &ti);

        wxRect tabRect;
        wxRect imageRect;

        // If the tab has an image, we need to check if the mouse is over it.
        if (ti.iImage != -1)
        {
            const wxIcon& icon = GetImage(ti.iImage);
            if (icon.IsOk())
                imageRect = icon.GetSize();
        }

        // The label text and the icon (if available) is centered in the tab,
        // so we need to find out the size of that.
        const wxString label = ti.pszText;
        if (!label.empty())
        {
            // This is intentional and not a bug. :) On left/right the font is
            // rotated 90 degree so we have to account for that by flipping
            // width with height.
            GetTextExtent(label, &tabRect.height, &tabRect.width);
        }

        // The tab has neither a text nor an icon
        if (label.empty() && imageRect.width == 0)
            break;

        if (!label.empty())
        {
            tabRect.x = r.x + (r.width - tabRect.width) / 2;
            if (tabDir == wxLEFT)
            {
                if (imageRect.height)
                    tabRect.y = r.y + (r.height - tabRect.height - gap - imageRect.height) / 2;
                else
                    tabRect.y = r.y + (r.height - tabRect.height - imageRect.height) / 2;
            }
            else
            {
                if (imageRect.height)
                    tabRect.y = r.y + (r.height - tabRect.height + gap + imageRect.height) / 2;
                else
                    tabRect.y = r.y + (r.height - tabRect.height) / 2;
            }

#ifdef WXDEBUG_NOTEBOOK_HITTEST
            ::ShowHitbox(const_cast<wxWindow *>(this), tabRect, true, true);
#endif
            if (tabRect.Contains(pt))
            {
                *flags = wxBK_HITTEST_ONLABEL;
                break;
            }
        }

        if (imageRect.height)
        {
            double scale = GetDPIScaleFactor();

            // When the tab has no text and only an icon, the position of the
            // icon is moving around a bit when scaling is applied. So I
            // checked with various resolutions and scaling settings to make
            // sure that the hitbox is where it is supposed to be. Otherwise
            // the hitbox is so far off the mark that the user will consider it
            // a bug as the flag is not reported where it is expected.
            // To get this numbers I switched the scaling factor (have to
            // restart the app in between) and look where the hitbox is
            // located, then adjust the values until they look right.
            int displacement;
            if (scale <= 1.0f)
                displacement = 3;
            else if (scale <= 1.25f)
                displacement = 8;
            else if (scale <= 1.50f)
                displacement = 14;
            else if (scale <= 1.75f)
                displacement = 3;
            else if (scale <= 2.00f)
                displacement = 6;
            else
                displacement = 12;

            imageRect.x = r.x + (r.width - imageRect.width) / 2;
            if (tabDir == wxLEFT)
            {
                if (!label.empty())
                    imageRect.y = tabRect.y + tabRect.height + gap;
                else
                    imageRect.y = r.y - displacement + (r.height - imageRect.height) / 2;
            }
            else
            {
                if (!label.empty())
                    imageRect.y = r.y + (r.height - imageRect.height - gap - tabRect.height) / 2;
                else
                    imageRect.y = r.y - displacement + (r.height - imageRect.height) / 2;
            }

#ifdef WXDEBUG_NOTEBOOK_HITTEST
            ::ShowHitbox(const_cast<wxWindow *>(this), imageRect, false, false);
#endif
            if (imageRect.Contains(pt))
            {
                *flags = wxBK_HITTEST_ONICON;
                break;
            }
        }

        break;
    }

    return item;
}

int wxNotebook::HitTest(const wxPoint& pt, long *flags) const
{
    // In Windows there is a bug so that the flags are not properly reported
    // for left/right orientation, so we have to do it manually.
    // It seems that the tabs are evaluated the same as if they were top/bottom
    // oriented, because if multiple tabs are stacked the flags are quite different
    // for each tab on the x axis.
    const wxDirection tabDir = GetTabOrientation();
    if (tabDir == wxLEFT || tabDir == wxRIGHT)
        return MSWHitTestLeftRight(pt, flags);

    TC_HITTESTINFO hitTestInfo;
    hitTestInfo.pt.x = pt.x;
    hitTestInfo.pt.y = pt.y;
    int item = TabCtrl_HitTest(GetHwnd(), &hitTestInfo);

    if ( flags )
    {
        *flags = 0;

        if ((hitTestInfo.flags & TCHT_NOWHERE) == TCHT_NOWHERE)
        {
            wxASSERT( item == wxNOT_FOUND );
            *flags |= wxBK_HITTEST_NOWHERE;
            if ( GetPageSize().Contains(pt) )
                *flags |= wxBK_HITTEST_ONPAGE;
        }
        else if ((hitTestInfo.flags & TCHT_ONITEM) == TCHT_ONITEM)
            *flags |= wxBK_HITTEST_ONITEM;
        else if ((hitTestInfo.flags & TCHT_ONITEMICON) == TCHT_ONITEMICON)
            *flags |= wxBK_HITTEST_ONICON;
        else if ((hitTestInfo.flags & TCHT_ONITEMLABEL) == TCHT_ONITEMLABEL)
            *flags |= wxBK_HITTEST_ONLABEL;
    }

    return item;
}

// ----------------------------------------------------------------------------
// flicker-less notebook redraw
// ----------------------------------------------------------------------------

#if USE_NOTEBOOK_ANTIFLICKER

// wnd proc for the spin button
LRESULT APIENTRY
wxNotebookSpinBtnWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch ( message )
    {
        case WM_ERASEBKGND:
            return 0;

        case WM_PAINT:
            if ( wxMSWDarkMode::PaintIfNecessary(hwnd, gs_wndprocNotebookSpinBtn) )
                return 0;
            break;
    }

    return ::CallWindowProc(CASTWNDPROC gs_wndprocNotebookSpinBtn,
                            hwnd, message, wParam, lParam);
}

LRESULT APIENTRY
wxNotebookWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return ::CallWindowProc(CASTWNDPROC gs_wndprocNotebook,
                            hwnd, message, wParam, lParam);
}

void wxNotebook::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
{
    // do nothing here
}

namespace
{

// Flags may include:
// - wxCONTROL_SELECTED for the currently selected tab
// - wxCONTROL_CURRENT for the "hot" tab, i.e. the one under mouse pointer
// - wxCONTROL_SPECIAL for the first tab.
void
DrawNotebookTab(wxWindow* win,
                wxDC& dc,
                const wxRect& rectOrig,
                const wxString& text,
                const wxBitmap& image,
                wxDirection tabOrient,
                int flags = wxCONTROL_NONE)
{
    // This colour is just an approximation which seems to look acceptable.
    dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_MENUBAR));

    // Note that FromDIP() should _not_ be used here, as 2px offset is used
    // even in high DPI.
    const int selectedOffset = 2;
    const int labelOffset = 3*selectedOffset;

    wxRect rectTab = rectOrig;
    wxColour colTab;
    if ( flags & wxCONTROL_SELECTED )
    {
        // Selected tab literally stands out, so make it bigger -- but clip
        // drawing to ensure we don't draw the inner border of the inflated
        // selected tab rectangle, it shouldn't overflow into the notebook
        // page area.
        rectTab.Inflate(selectedOffset);

        wxRect rectClip = rectTab;
        switch ( tabOrient )
        {
            case wxTOP:
                rectClip.height -= selectedOffset;
                break;

            case wxBOTTOM:
                rectClip.y += selectedOffset;
                rectClip.height -= selectedOffset;
                break;

            case wxLEFT:
                rectClip.width -= selectedOffset;
                break;

            case wxRIGHT:
                rectClip.x += selectedOffset;
                rectClip.width -= selectedOffset;
                break;

            default:
                wxFAIL_MSG("unreachable");
        }

        dc.SetClippingRegion(rectClip);

        colTab = win->GetBackgroundColour();
    }
    else // not the selected tab
    {
        // All tab rectangles overlap the previous one to avoid double pixel
        // borders between them in Windows 10 flat look, except for the first
        // one which has nothing to overlap.
        if ( !(flags & wxCONTROL_SPECIAL) )
        {
            switch ( tabOrient )
            {
                case wxTOP:
                case wxBOTTOM:
                    rectTab.x--;
                    rectTab.width++;
                    break;

                case wxLEFT:
                case wxRIGHT:
                    rectTab.y--;
                    rectTab.height++;
                    break;

                default:
                    wxFAIL_MSG("unreachable");
            }
        }

        if ( flags & wxCONTROL_CURRENT )
            colTab = wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT);
        else
            colTab = wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW);
    }

    dc.SetBrush(colTab);
    dc.DrawRectangle(rectTab);

    wxRect rectLabel = rectOrig;
    if ( flags & wxCONTROL_SELECTED )
    {
        dc.DestroyClippingRegion();

        // Also shift the label to mimic the native control which makes it "pop
        // up" for the selected tab (with "up" being "in the tab direction").
        switch ( tabOrient )
        {
            case wxTOP:
                rectLabel.y -= selectedOffset;
                break;

            case wxBOTTOM:
                rectLabel.y += selectedOffset;
                break;

            case wxLEFT:
                rectLabel.x -= selectedOffset;
                break;

            case wxRIGHT:
                rectLabel.x += selectedOffset;
                break;

            default:
                wxFAIL_MSG("unreachable");
        }
    }

    rectLabel.Deflate(labelOffset);

    // Draw the label and the image, if any.
    switch ( tabOrient )
    {
        case wxTOP:
        case wxBOTTOM:
            // We can use an existing helper that will do everything for us.
            dc.DrawLabel(text, image, rectLabel,
                         wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
            break;

        case wxLEFT:
        case wxRIGHT:
            {
                const wxSize textSize = dc.GetTextExtent(text);

                // Exchange width and height because we're drawing text
                // vertically.
                wxSize totalSize{textSize.y, textSize.x};

                int textOfs = 0;
                wxSize imageSize;
                if ( image.IsOk() )
                {
                    imageSize = image.GetSize();

                    // Use label offset for the gap between image and the label
                    // too because why not.
                    totalSize.y += imageSize.y + labelOffset;

                    if ( imageSize.x > totalSize.x )
                    {
                        textOfs = (imageSize.x - totalSize.x) / 2;
                        totalSize.x = imageSize.x;
                    }
                }

                // Native control actually draws text bottom/top aligned in the
                // first/only row but centers them if there is more than one
                // row of tabs. Don't bother with this, especially because it's
                // really not obvious that it looks any better and just center
                // them always.
                const wxRect rect = wxRect(totalSize).CentreIn(rectLabel);

                if ( tabOrient == wxLEFT )
                {
                    int y = rect.y + textSize.x;

                    dc.DrawRotatedText(text, rect.x + textOfs, y, 90.0);

                    if ( image.IsOk() )
                        dc.DrawBitmap(image, rect.x, y + labelOffset, true);
                }
                else // tabOrient == wxRIGHT
                {
                    int y = rect.y;

                    if ( image.IsOk() )
                    {
                        dc.DrawBitmap(image, rect.x, y, true);

                        y += imageSize.y + labelOffset;
                    }

                    dc.DrawRotatedText(text, rect.GetRight() - textOfs, y, -90.0);
                }
            }
            break;

        default:
            wxFAIL_MSG("unreachable");
    }
}

} // anonymous namespace

void wxNotebook::MSWNotebookPaint(wxDC& dc)
{
    dc.Clear();

    const wxDirection tabOrient = GetTabOrientation();

    const wxSize sizeWindow = GetClientSize();
    const int selected = GetSelection();
    const wxPoint posCursor = ScreenToClient(wxGetMousePosition());

    const auto drawTab = [this, &dc, tabOrient](wxRect rect, size_t n, int flags)
    {
        if ( n == 0 )
            flags |= wxCONTROL_SPECIAL;

        DrawNotebookTab(this, dc, rect,
                        GetPageText(n),
                        GetImageBitmapFor(this, GetPageImage(n)),
                        tabOrient,
                        flags);
    };

    const size_t pages = GetPageCount();
    for ( size_t n = 0; n < pages; ++n )
    {
        if ( static_cast<int>(n) == selected )
        {
            // We're going to draw this one after all the other ones as it
            // overlaps them.
            continue;
        }

        const wxRect rect = GetTabRect(n);

        // For horizontal tabs, some of them can be scrolled out of view, skip
        // drawing them just in case we have zillions of tabs to avoid drawing
        // the off-screen ones unnecessarily.
        if ( rect.x > sizeWindow.x )
        {
            // This tab, and all the remaining ones, can't be seen anyhow, so
            // don't bother drawing them.
            break;
        }

        // We can't track the "hot" tab when using non-top tabs as the native
        // control doesn't refresh them on mouse move (it seems to switch to
        // comctl32.dll v5-like implementation in this case), so don't paint
        // them specially.
        int flags = wxCONTROL_NONE;
        if ( tabOrient == wxTOP && rect.Contains(posCursor) )
            flags |= wxCONTROL_CURRENT;

        drawTab(rect, n, flags);
    }

    if ( selected != wxNOT_FOUND )
        drawTab(GetTabRect(selected), selected, wxCONTROL_SELECTED);
}

void wxNotebook::OnPaint(wxPaintEvent& event)
{
    // We can rely on the default implementation if we don't have a custom
    // background colour (note that it is always set when using dark mode).
    if ( !m_hasBgCol )
    {
        event.Skip();
        return;
    }

    wxPaintDC dc(this);

    if ( wxMSWDarkMode::IsActive() )
    {
        // We can't use default painting in dark mode, it just doesn't work
        // there, whichever theme we use, so draw everything ourselves.
        MSWNotebookPaint(dc);
        return;
    }

    RECT rc;
    ::GetClientRect(GetHwnd(), &rc);
    if ( !rc.right || !rc.bottom )
        return;

    wxBitmap bmp(rc.right, rc.bottom);
    wxMemoryDC memdc(bmp);

    const HDC hdc = GetHdcOf(memdc);

    // The drawing logic of the native tab control is absolutely impenetrable
    // but observation shows that in the current Windows versions (XP and 7),
    // the tab control always erases its entire background in its window proc
    // when the tabs are top-aligned but does not do it when the tabs are in
    // any other position.
    //
    // This means that we can't rely on our background colour being used for
    // the blank area in the tab row because this doesn't work in the default
    // top-aligned case, hence the hack with ExtFloodFill() below. But it also
    // means that we still do need to erase the DC to account for the other
    // cases.
    //
    // Moreover, just in case some very old or very new (or even future,
    // although it seems unlikely that this is ever going to change by now)
    // version of Windows didn't do it like this, do both things in all cases
    // instead of optimizing away the one of them which doesn't do anything for
    // the effectively used tab orientation -- better safe than fast.

    // Notice that we use our own background here, not the background used for
    // the pages, because the tab row background must blend with the parent and
    // so the background colour inherited from it (if any) must be used.
    AutoHBRUSH hbr(wxColourToRGB(GetBackgroundColour()));

    ::FillRect(hdc, &rc, hbr);

    MSWDefWindowProc(WM_PAINT, (WPARAM)hdc, 0);

    // At least for the top-aligned tabs, our background colour was overwritten
    // and so we now replace the default background with our colour. This is
    // horribly inefficient, of course, but seems to be the only way to do it.
    if ( UseBgCol() )
    {
        SelectInHDC selectBrush(hdc, hbr);

        // Find the point which must contain the default background colour:
        // this is a hack, of course, but using this point "close" to the
        // corner seems to work fine in practice.
        int x = 0,
            y = 0;

        switch ( GetWindowStyle() & wxBK_ALIGN_MASK )
        {
            case wxBK_TOP:
                x = rc.right - 2;
                y = 2;
                break;

            case wxBK_BOTTOM:
                x = rc.right - 2;
                y = rc.bottom - 2;
                break;

            case wxBK_LEFT:
                x = 2;
                y = rc.bottom - 2;
                break;

            case wxBK_RIGHT:
                x = 2;
                y = rc.bottom - 2;
                break;
        }

        ::ExtFloodFill(hdc, x, y, ::GetSysColor(COLOR_BTNFACE), FLOODFILLSURFACE);
    }

    dc.Blit(0, 0, rc.right, rc.bottom, &memdc, 0, 0);
}

#endif // USE_NOTEBOOK_ANTIFLICKER

// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

void wxNotebook::OnSize(wxSizeEvent& event)
{
    if ( GetPageCount() == 0 )
    {
        // Prevents droppings on resize, but does cause some flicker
        // when there are no pages.
        Refresh();
        event.Skip();
        return;
    }
    else
    {
        // Without this, we can sometimes get droppings at the edges
        // of a notebook, for example a notebook in a splitter window.
        // This needs to be reconciled with the RefreshRect calls
        // at the end of this function, which weren't enough to prevent
        // the droppings.

        wxSize sz = GetClientSize();

        // Refresh right side
        wxRect rect(sz.x-4, 0, 4, sz.y);
        RefreshRect(rect);

        // Refresh bottom side
        rect = wxRect(0, sz.y-4, sz.x, 4);
        RefreshRect(rect);

        // Refresh left side
        rect = wxRect(0, 0, 4, sz.y);
        RefreshRect(rect);
    }

    // fit all the notebook pages to the tab control's display area

    RECT rc;
    rc.left = rc.top = 0;
    GetSize((int *)&rc.right, (int *)&rc.bottom);

    // save the total size, we'll use it below
    int widthNbook = rc.right - rc.left,
        heightNbook = rc.bottom - rc.top;

    // there seems to be a bug in the implementation of TabCtrl_AdjustRect(): it
    // returns completely false values for multiline tab controls after the tabs
    // are added but before getting the first WM_SIZE (off by ~50 pixels, see
    //
    // http://sf.net/tracker/index.php?func=detail&aid=645323&group_id=9863&atid=109863
    //
    // and the only work around I could find was this ugly hack... without it
    // simply toggling the "multiline" checkbox in the notebook sample resulted
    // in a noticeable page displacement
    if ( HasFlag(wxNB_MULTILINE) )
    {
        // avoid an infinite recursion: we get another notification too!
        static bool s_isInOnSize = false;

        if ( !s_isInOnSize )
        {
            s_isInOnSize = true;
            SendMessage(GetHwnd(), WM_SIZE, SIZE_RESTORED,
                    MAKELPARAM(rc.right, rc.bottom));
            s_isInOnSize = false;
        }

        // The best size depends on the number of rows of tabs, which can
        // change when the notepad is resized.
        InvalidateBestSize();
    }

#if wxUSE_UXTHEME
    // background bitmap size has changed, update the brush using it too
    UpdateBgBrush();
#endif // wxUSE_UXTHEME

    (void)TabCtrl_AdjustRect(GetHwnd(), false, &rc);

    int width = rc.right - rc.left,
        height = rc.bottom - rc.top;
    size_t nCount = m_pages.size();
    for ( size_t nPage = 0; nPage < nCount; nPage++ ) {
        wxNotebookPage *pPage = m_pages[nPage];
        pPage->SetSize(rc.left, rc.top, width, height);
    }


    // unless we had already repainted everything, we now need to refresh
    if ( !HasFlag(wxFULL_REPAINT_ON_RESIZE) )
    {
        // invalidate areas not covered by pages
        RefreshRect(wxRect(0, 0, widthNbook, rc.top), false);
        RefreshRect(wxRect(0, rc.top, rc.left, height), false);
        RefreshRect(wxRect(0, rc.bottom, widthNbook, heightNbook - rc.bottom),
                    false);
        RefreshRect(wxRect(rc.right, rc.top, widthNbook - rc.right, height),
                    false);
    }

#if USE_NOTEBOOK_ANTIFLICKER
    // subclass the spin control used by the notebook to scroll pages to
    // prevent it from flickering on resize
    if ( !m_hasSubclassedUpdown )
    {
        // iterate over all child windows to find spin button
        for ( HWND child = ::GetWindow(GetHwnd(), GW_CHILD);
              child;
              child = ::GetWindow(child, GW_HWNDNEXT) )
        {
            wxWindow *childWindow = wxFindWinFromHandle((WXHWND)child);

            // see if it exists, if no wxWindow found then assume it's the spin
            // btn
            if ( !childWindow )
            {
                // subclass the spin button to override WM_ERASEBKGND
                if ( !gs_wndprocNotebookSpinBtn )
                    gs_wndprocNotebookSpinBtn = wxGetWindowProc(child);

                wxSetWindowProc(child, wxNotebookSpinBtnWndProc);
                m_hasSubclassedUpdown = true;
                break;
            }
        }
    }
#endif // USE_NOTEBOOK_ANTIFLICKER

    event.Skip();
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if ( event.IsWindowChange() ) {
        // change pages
        AdvanceSelection(event.GetDirection());
    }
    else {
        // we get this event in 3 cases
        //
        // a) one of our pages might have generated it because the user TABbed
        // out from it in which case we should propagate the event upwards and
        // our parent will take care of setting the focus to prev/next sibling
        //
        // or
        //
        // b) the parent panel wants to give the focus to us so that we
        // forward it to our selected page. We can't deal with this in
        // OnSetFocus() because we don't know which direction the focus came
        // from in this case and so can't choose between setting the focus to
        // first or last panel child
        //
        // or
        //
        // c) we ourselves (see MSWTranslateMessage) generated the event
        //
        wxWindow * const parent = GetParent();

        // the wxObject* casts are required to avoid MinGW GCC 2.95.3 ICE
        const bool isFromParent = event.GetEventObject() == (wxObject*) parent;
        const bool isFromSelf = event.GetEventObject() == (wxObject*) this;
        const bool isForward = event.GetDirection();

        if ( isFromSelf && !isForward )
        {
            // focus is currently on notebook tab and should leave
            // it backwards (Shift-TAB)
            event.SetCurrentFocus(this);
            parent->HandleWindowEvent(event);
        }
        else if ( isFromParent || isFromSelf )
        {
            // no, it doesn't come from child, case (b) or (c): forward to a
            // page but only if entering notebook page (i.e. direction is
            // backwards (Shift-TAB) comething from out-of-notebook, or
            // direction is forward (TAB) from ourselves),
            if ( m_selection != wxNOT_FOUND &&
                    (!event.GetDirection() || isFromSelf) )
            {
                // so that the page knows that the event comes from it's parent
                // and is being propagated downwards
                event.SetEventObject(this);

                wxWindow *page = m_pages[m_selection];
                if ( !page->HandleWindowEvent(event) )
                {
                    page->SetFocus();
                }
                //else: page manages focus inside it itself
            }
            else // otherwise set the focus to the notebook itself
            {
                SetFocus();
            }
        }
        else
        {
            // it comes from our child, case (a), pass to the parent, but only
            // if the direction is forwards. Otherwise set the focus to the
            // notebook itself. The notebook is always the 'first' control of a
            // page.
            if ( !isForward )
            {
                SetFocus();
            }
            else if ( parent )
            {
                event.SetCurrentFocus(this);
                parent->HandleWindowEvent(event);
            }
        }
    }
}

bool wxNotebook::SetBackgroundColour(const wxColour& colour)
{
    if ( !wxNotebookBase::SetBackgroundColour(colour) )
        return false;

#if wxUSE_UXTHEME
    UpdateBgBrush();
#endif // wxUSE_UXTHEME

#if USE_NOTEBOOK_ANTIFLICKER
    Unbind(wxEVT_ERASE_BACKGROUND, &wxNotebook::OnEraseBackground, this);
    if ( m_hasBgCol || !wxUxThemeIsActive() )
    {
        Bind(wxEVT_ERASE_BACKGROUND, &wxNotebook::OnEraseBackground, this);
    }
#endif // USE_NOTEBOOK_ANTIFLICKER

    return true;
}

#if wxUSE_UXTHEME

WXHBRUSH wxNotebook::QueryBgBitmap()
{
    RECT rc;
    ::GetClientRect(GetHwnd(), &rc);
    if ( ::IsRectEmpty(&rc) )
        return 0;

    wxUxThemeHandle theme(this, L"TAB");
    if ( !theme )
        return 0;

    ClientHDC hDC(GetHwnd());

    RECT rcBg;
    ::GetThemeBackgroundContentRect(theme,
                                    (HDC) hDC,
                                    9, /* TABP_PANE */
                                    0,
                                    &rc,
                                    &rcBg);

    m_bgBrushAdj = wxPoint(rcBg.left, rcBg.top);
    ::OffsetRect(&rcBg, -rcBg.left, -rcBg.top);

    ::GetThemeBackgroundExtent
                            (
                                theme,
                                (HDC) hDC,
                                9 /* TABP_PANE */,
                                0,
                                &rcBg,
                                &rc
                            );

    MemoryHDC hDCMem(hDC);
    CompatibleBitmap hBmp(hDC, rcBg.right, rcBg.bottom);

    {
        SelectInHDC selectBmp(hDCMem, hBmp);
        ::DrawThemeBackground
                                (
                                    theme,
                                    hDCMem,
                                    9 /* TABP_PANE */,
                                    0,
                                    &rc,
                                    nullptr
                                );
    } // deselect bitmap from the memory HDC before using it

    return (WXHBRUSH)::CreatePatternBrush(hBmp);
}

void wxNotebook::UpdateBgBrush()
{
    if ( m_hbrBackground )
        ::DeleteObject((HBRUSH)m_hbrBackground);

    if ( !m_hasBgCol && wxUxThemeIsActive() )
    {
        m_hbrBackground = QueryBgBitmap();
    }
    else // no themes or we've got user-defined solid colour
    {
        m_hbrBackground = nullptr;
    }
}

bool wxNotebook::MSWPrintChild(WXHDC hDC, wxWindow *child)
{
    const wxRect r = GetPageSize();
    if ( r.IsEmpty() )
        return false;

    RECT rc;
    wxCopyRectToRECT(r, rc);

    // map rect to the coords of the window we're drawing in
    if ( child )
        wxMapWindowPoints(GetHwnd(), GetHwndOf(child), &rc);

    // If we're using a solid colour (for example if we've switched off
    // theming for this notebook), paint it
    if (UseBgCol())
    {
        wxBrush brush(GetBackgroundColour());
        HBRUSH hbr = GetHbrushOf(brush);

        ::FillRect((HDC) hDC, &rc, hbr);

        return true;
    }
    else // No solid background colour, try to use themed background.
    {
        wxUxThemeHandle theme(child, L"TAB");
        if ( theme )
        {
            // we have the content area (page size), but we need to draw all of the
            // background for it to be aligned correctly
            ::GetThemeBackgroundExtent
                                    (
                                        theme,
                                        (HDC) hDC,
                                        9 /* TABP_PANE */,
                                        0,
                                        &rc,
                                        &rc
                                    );
            ::DrawThemeBackground
                                    (
                                        theme,
                                        (HDC) hDC,
                                        9 /* TABP_PANE */,
                                        0,
                                        &rc,
                                        nullptr
                                    );
            return true;
        }
    }

    return wxNotebookBase::MSWPrintChild(hDC, child);
}

#endif // wxUSE_UXTHEME

// Windows only: attempts to get colour for UX theme page background
wxColour wxNotebook::GetThemeBackgroundColour() const
{
#if wxUSE_UXTHEME
    if (wxUxThemeIsActive())
    {
        wxUxThemeHandle hTheme(this, L"TAB");
        if (hTheme)
        {
            // This is total guesswork.
            // See PlatformSDK\Include\Tmschema.h for values.
            // JACS: can also use 9 (TABP_PANE)
            wxColour colour = hTheme.GetColour(TABP_BODY, TMT_FILLCOLORHINT, TIS_NORMAL);
            if ( !colour.IsOk() )
                return GetBackgroundColour();

            /*
            [DS] Workaround for WindowBlinds:
            Some themes return a near black theme color using FILLCOLORHINT,
            this makes notebook pages have an ugly black background and makes
            text (usually black) unreadable. Retry again with FILLCOLOR.

            This workaround potentially breaks appearance of some themes,
            but in practice it already fixes some themes.
            */
            if ( colour.GetRGB() == 1 )
                colour = hTheme.GetColour(TABP_BODY, TMT_FILLCOLOR, TIS_NORMAL);

            // Under Vista, the tab background colour is reported incorrectly.
            // So for the default theme at least, hard-code the colour to something
            // that will blend in.

            static int s_AeroStatus = -1;
            if (s_AeroStatus == -1)
            {
                WCHAR szwThemeFile[1024];
                WCHAR szwThemeColor[256];
                if (S_OK == ::GetCurrentThemeName(szwThemeFile, 1024, szwThemeColor, 256, nullptr, 0))
                {
                    wxString themeFile(szwThemeFile);
                    if (themeFile.Find(wxT("Aero")) != -1 && wxString(szwThemeColor) == wxT("NormalColor"))
                        s_AeroStatus = 1;
                    else
                        s_AeroStatus = 0;
                }
                else
                    s_AeroStatus = 0;
            }

            if (s_AeroStatus == 1)
                colour = wxColour(255, 255, 255);

            return colour;
        }
    }
#endif // wxUSE_UXTHEME

    return GetBackgroundColour();
}

// ----------------------------------------------------------------------------
// wxNotebook base class virtuals
// ----------------------------------------------------------------------------

#if wxUSE_CONSTRAINTS

// override these 2 functions to do nothing: everything is done in OnSize

void wxNotebook::SetConstraintSizes(bool WXUNUSED(recurse))
{
  // don't set the sizes of the pages - their correct size is not yet known
  wxControl::SetConstraintSizes(false);
}

bool wxNotebook::DoPhase(int WXUNUSED(nPhase))
{
  return true;
}

#endif // wxUSE_CONSTRAINTS

// ----------------------------------------------------------------------------
// wxNotebook Windows message handlers
// ----------------------------------------------------------------------------

bool wxNotebook::MSWOnScroll(int orientation, WXWORD nSBCode,
                             WXWORD pos, WXHWND control)
{
    // don't generate EVT_SCROLLWIN events for the WM_SCROLLs coming from the
    // up-down control
    if ( control )
        return false;

    return wxNotebookBase::MSWOnScroll(orientation, nSBCode, pos, control);
}

bool wxNotebook::MSWOnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM* result)
{
  wxBookCtrlEvent event(wxEVT_NULL, m_windowId);

  NMHDR* hdr = (NMHDR *)lParam;
  switch ( hdr->code ) {
    case TCN_SELCHANGE:
      event.SetEventType(wxEVT_NOTEBOOK_PAGE_CHANGED);
      break;

    case TCN_SELCHANGING:
      event.SetEventType(wxEVT_NOTEBOOK_PAGE_CHANGING);
      break;

    default:
      return wxControl::MSWOnNotify(idCtrl, lParam, result);
  }

  event.SetSelection(TabCtrl_GetCurSel(GetHwnd()));
  event.SetOldSelection(m_selection);
  event.SetEventObject(this);
  event.SetInt(idCtrl);

  // Change the selection before generating the event as its handler should
  // already see the new page selected.
  if ( hdr->code == TCN_SELCHANGE )
  {
      UpdateSelection(event.GetSelection());

      // We need to update the tabs after the selection change when drawing
      // them ourselves, otherwise the previously selected tab is not redrawn.
      if ( wxMSWDarkMode::IsActive() )
          Refresh();
  }

  bool processed = HandleWindowEvent(event);
  *result = !event.IsAllowed();
  return processed;
}

#endif // wxUSE_NOTEBOOK
