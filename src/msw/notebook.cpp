///////////////////////////////////////////////////////////////////////////////
// Name:        msw/notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.06.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "notebook.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_NOTEBOOK

// wxWidgets
#ifndef WX_PRECOMP
  #include  "wx/string.h"
  #include  "wx/dc.h"
#endif  // WX_PRECOMP

#include  "wx/log.h"
#include  "wx/imaglist.h"
#include  "wx/event.h"
#include  "wx/control.h"
#include  "wx/notebook.h"
#include  "wx/app.h"
#include  "wx/sysopt.h"

#include  "wx/msw/private.h"

#include  <windowsx.h>

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#endif

#if !(defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

#include "wx/msw/winundef.h"

#if wxUSE_UXTHEME
    #include "wx/msw/uxtheme.h"
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())

// hide the ugly cast
#define m_hwnd    (HWND)GetHWND()

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
// event table
// ----------------------------------------------------------------------------

#include <wx/listimpl.cpp>

WX_DEFINE_LIST( wxNotebookPageInfoList ) ;

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, wxNotebook::OnSelChange)

    EVT_SIZE(wxNotebook::OnSize)

    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
END_EVENT_TABLE()

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxNotebookStyle )

wxBEGIN_FLAGS( wxNotebookStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    wxFLAGS_MEMBER(wxBORDER_SIMPLE)
    wxFLAGS_MEMBER(wxBORDER_SUNKEN)
    wxFLAGS_MEMBER(wxBORDER_DOUBLE)
    wxFLAGS_MEMBER(wxBORDER_RAISED)
    wxFLAGS_MEMBER(wxBORDER_STATIC)
    wxFLAGS_MEMBER(wxBORDER_NONE)

    // old style border flags
    wxFLAGS_MEMBER(wxSIMPLE_BORDER)
    wxFLAGS_MEMBER(wxSUNKEN_BORDER)
    wxFLAGS_MEMBER(wxDOUBLE_BORDER)
    wxFLAGS_MEMBER(wxRAISED_BORDER)
    wxFLAGS_MEMBER(wxSTATIC_BORDER)
    wxFLAGS_MEMBER(wxBORDER)

    // standard window styles
    wxFLAGS_MEMBER(wxTAB_TRAVERSAL)
    wxFLAGS_MEMBER(wxCLIP_CHILDREN)
    wxFLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    wxFLAGS_MEMBER(wxWANTS_CHARS)
    wxFLAGS_MEMBER(wxFULL_REPAINT_ON_RESIZE)
    wxFLAGS_MEMBER(wxALWAYS_SHOW_SB )
    wxFLAGS_MEMBER(wxVSCROLL)
    wxFLAGS_MEMBER(wxHSCROLL)

    wxFLAGS_MEMBER(wxNB_FIXEDWIDTH)
    wxFLAGS_MEMBER(wxNB_LEFT)
    wxFLAGS_MEMBER(wxNB_RIGHT)
    wxFLAGS_MEMBER(wxNB_BOTTOM)

wxEND_FLAGS( wxNotebookStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxNotebook, wxControl,"wx/notebook.h")
IMPLEMENT_DYNAMIC_CLASS_XTI(wxNotebookPageInfo, wxObject , "wx/notebook.h" )

wxCOLLECTION_TYPE_INFO( wxNotebookPageInfo * , wxNotebookPageInfoList ) ;

template<> void wxCollectionToVariantArray( wxNotebookPageInfoList const &theList, wxxVariantArray &value)
{
    wxListCollectionToVariantArray<wxNotebookPageInfoList::compatibility_iterator>( theList , value ) ;
}

wxBEGIN_PROPERTIES_TABLE(wxNotebook)
    wxEVENT_PROPERTY( PageChanging , wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING , wxNotebookEvent )
    wxEVENT_PROPERTY( PageChanged , wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED , wxNotebookEvent )

    wxPROPERTY_COLLECTION( PageInfos , wxNotebookPageInfoList , wxNotebookPageInfo* , AddPageInfo , GetPageInfos , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY_FLAGS( WindowStyle , wxNotebookStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxNotebook)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_5( wxNotebook , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size , long , WindowStyle)


wxBEGIN_PROPERTIES_TABLE(wxNotebookPageInfo)
    wxREADONLY_PROPERTY( Page , wxNotebookPage* , GetPage , EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxREADONLY_PROPERTY( Text , wxString , GetText , wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxREADONLY_PROPERTY( Selected , bool , GetSelected , false, 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    wxREADONLY_PROPERTY( ImageId , int , GetImageId , -1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
wxEND_PROPERTIES_TABLE()

wxBEGIN_HANDLERS_TABLE(wxNotebookPageInfo)
wxEND_HANDLERS_TABLE()

wxCONSTRUCTOR_4( wxNotebookPageInfo , wxNotebookPage* , Page , wxString , Text , bool , Selected , int , ImageId )

#else
IMPLEMENT_DYNAMIC_CLASS(wxNotebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxNotebookPageInfo, wxObject )
#endif
IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

const wxNotebookPageInfoList& wxNotebook::GetPageInfos() const
{
    wxNotebookPageInfoList* list = const_cast< wxNotebookPageInfoList* >( &m_pageInfos ) ;
    WX_CLEAR_LIST( wxNotebookPageInfoList , *list ) ;
    for( size_t i = 0 ; i < GetPageCount() ; ++i )
    {
        wxNotebookPageInfo *info = new wxNotebookPageInfo() ;
        info->Create( const_cast<wxNotebook*>(this)->GetPage(i) , GetPageText(i) , GetSelection() == int(i) , GetPageImage(i) ) ;
        list->Append( info ) ;
    }
    return m_pageInfos ;
}

// common part of all ctors
void wxNotebook::Init()
{
  m_imageList = NULL;
  m_nSelection = -1;

#if wxUSE_UXTHEME
  m_hbrBackground = NULL;
#endif // wxUSE_UXTHEME
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
    // comctl32.dll 6.0 doesn't support non-top tabs with visual styles (the
    // control is simply not rendered correctly), so disable them in this case
    const int verComCtl32 = wxApp::GetComCtl32Version();
    if ( verComCtl32 == 600 )
    {
        // check if we use themes at all -- if we don't, we're still ok
#if wxUSE_UXTHEME
        if ( wxUxThemeEngine::GetIfActive() )
#endif
        {
            style &= ~(wxNB_BOTTOM | wxNB_LEFT | wxNB_RIGHT);
        }
    }

    LPCTSTR className = WC_TABCONTROL;

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

            if ( ::GetClassInfo(::GetModuleHandle(NULL), WC_TABCONTROL, &wc) )
            {
                wc.lpszClassName = wxT("_wx_SysTabCtl32");
                wc.style &= ~(CS_HREDRAW | CS_VREDRAW);

                s_clsNotebook.Register(wc);
            }
            else
            {
                wxLogLastError(_T("GetClassInfoEx(SysTabCtl32)"));
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

    if ( !CreateControl(parent, id, pos, size, style | wxTAB_TRAVERSAL,
                        wxDefaultValidator, name) )
        return false;

    if ( !MSWCreateControl(className, wxEmptyString, pos, size) )
        return false;

    if (HasFlag(wxNB_NOPAGETHEME) || (wxSystemOptions::HasOption(wxT("msw.notebook.themed-background")) &&
                                      wxSystemOptions::GetOptionInt(wxT("msw.notebook.themed-background")) == 0))
    {
        wxColour col = GetThemeBackgroundColour();
        if (col.Ok())
        {
            SetBackgroundColour(col);
        }
    }
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

    if ( style & wxNB_BOTTOM )
        tabStyle |= TCS_RIGHT;
    else if ( style & wxNB_LEFT )
        tabStyle |= TCS_VERTICAL;
    else if ( style & wxNB_RIGHT )
        tabStyle |= TCS_VERTICAL | TCS_RIGHT;

    // ex style
    if ( exstyle )
    {
        // note that we never want to have the default WS_EX_CLIENTEDGE style
        // as it looks too ugly for the notebooks
        *exstyle = 0;
    }

    return tabStyle;
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
  wxASSERT( (int)m_pages.Count() == TabCtrl_GetItemCount(m_hwnd) );

  return m_pages.Count();
}

int wxNotebook::GetRowCount() const
{
  return TabCtrl_GetRowCount(m_hwnd);
}

int wxNotebook::SetSelection(size_t nPage)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), wxNOT_FOUND, wxT("notebook page out of range") );

  if ( int(nPage) != m_nSelection )
  {
    wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, m_windowId);
    event.SetSelection(nPage);
    event.SetOldSelection(m_nSelection);
    event.SetEventObject(this);
    if ( !GetEventHandler()->ProcessEvent(event) || event.IsAllowed() )
    {
      // program allows the page change
      event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED);
      (void)GetEventHandler()->ProcessEvent(event);

      TabCtrl_SetCurSel(m_hwnd, nPage);
    }
  }

  return m_nSelection;
}

bool wxNotebook::SetPageText(size_t nPage, const wxString& strText)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), false, wxT("notebook page out of range") );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_TEXT;
  tcItem.pszText = (wxChar *)strText.c_str();

  return TabCtrl_SetItem(m_hwnd, nPage, &tcItem) != 0;
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
  if ( TabCtrl_GetItem(m_hwnd, nPage, &tcItem) )
    str = tcItem.pszText;

  return str;
}

int wxNotebook::GetPageImage(size_t nPage) const
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_IMAGE;

  return TabCtrl_GetItem(m_hwnd, nPage, &tcItem) ? tcItem.iImage : -1;
}

bool wxNotebook::SetPageImage(size_t nPage, int nImage)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), false, wxT("notebook page out of range") );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_IMAGE;
  tcItem.iImage = nImage;

  return TabCtrl_SetItem(m_hwnd, nPage, &tcItem) != 0;
}

void wxNotebook::SetImageList(wxImageList* imageList)
{
  wxNotebookBase::SetImageList(imageList);

  if ( imageList )
  {
    TabCtrl_SetImageList(m_hwnd, (HIMAGELIST)imageList->GetHIMAGELIST());
  }
}

// ----------------------------------------------------------------------------
// wxNotebook size settings
// ----------------------------------------------------------------------------

void wxNotebook::SetPageSize(const wxSize& size)
{
    // transform the page size into the notebook size
    RECT rc;
    rc.left =
    rc.top = 0;
    rc.right = size.x;
    rc.bottom = size.y;

    TabCtrl_AdjustRect(GetHwnd(), true, &rc);

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

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    wxSize sizeTotal = sizePage;

    // We need to make getting tab size part of the wxWidgets API.
    wxSize tabSize;
    if (GetPageCount() > 0)
    {
        RECT rect;
        TabCtrl_GetItemRect((HWND) GetHWND(), 0, & rect);
        tabSize.x = rect.right - rect.left;
        tabSize.y = rect.bottom - rect.top;
    }
    if ( HasFlag(wxNB_LEFT) || HasFlag(wxNB_RIGHT) )
    {
        sizeTotal.x += tabSize.x + 7;
        sizeTotal.y += 7;
    }
    else
    {
        sizeTotal.x += 7;
        sizeTotal.y += tabSize.y + 7;
    }

    return sizeTotal;
}

void wxNotebook::AdjustPageSize(wxNotebookPage *page)
{
    wxCHECK_RET( page, _T("NULL page in wxNotebook::AdjustPageSize") );

    RECT rc;
    rc.left =
    rc.top = 0;

    // get the page size from the notebook size
    GetSize((int *)&rc.right, (int *)&rc.bottom);

    // This check is to work around a bug in TabCtrl_AdjustRect which will
    // cause a crash on win2k, or on XP with themes disabled, if the
    // wxNB_MULTILINE style is used and the rectangle is very small, (such as
    // when the notebook is first created.)  The value of 20 is just
    // arbitrarily chosen, if there is a better way to determine this value
    // then please do so.  --RD
    if (rc.right > 20 && rc.bottom > 20)
    {
        TabCtrl_AdjustRect(m_hwnd, false, &rc);
        page->SetSize(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
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
        return NULL;

    TabCtrl_DeleteItem(m_hwnd, nPage);

    if ( m_pages.IsEmpty() )
    {
        // no selection any more, the notebook becamse empty
        m_nSelection = -1;
    }
    else // notebook still not empty
    {
        int selNew = TabCtrl_GetCurSel(m_hwnd);
        if (selNew != -1)
        {
            // No selection change, just refresh the current selection.
            // Because it could be that the slection index changed
            // we need to update it.
            // Note: this does not mean the selection it self changed.
            m_nSelection = selNew;
            m_pages[m_nSelection]->Refresh();
        }
        else if (int(nPage) == m_nSelection)
        {
            // The selection was deleted.

            // Determine new selection.
            if (m_nSelection == int(GetPageCount()))
                selNew = m_nSelection - 1;
            else
                selNew = m_nSelection;

            // m_nSelection must be always valid so reset it before calling
            // SetSelection()
            m_nSelection = -1;
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
  size_t nPageCount = GetPageCount();
  size_t nPage;
  for ( nPage = 0; nPage < nPageCount; nPage++ )
    delete m_pages[nPage];

  m_pages.Clear();

  TabCtrl_DeleteAllItems(m_hwnd);

  m_nSelection = -1;

  InvalidateBestSize();
  return true;
}

// same as AddPage() but does it at given position
bool wxNotebook::InsertPage(size_t nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    wxCHECK_MSG( pPage != NULL, false, _T("NULL page in wxNotebook::InsertPage") );
    wxCHECK_MSG( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), false,
                 _T("invalid index in wxNotebook::InsertPage") );

    wxASSERT_MSG( pPage->GetParent() == this,
                    _T("notebook pages must have notebook as parent") );

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
        tcItem.pszText = (wxChar *)strText.c_str(); // const_cast
    }

    // hide the page: unless it is selected, it shouldn't be shown (and if it
    // is selected it will be shown later)
    HWND hwnd = GetWinHwnd(pPage);
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_VISIBLE);

    // this updates internal flag too -- otherwise it would get out of sync
    // with the real state
    pPage->Show(false);


    // fit the notebook page to the tab control's display area: this should be
    // done before adding it to the notebook or TabCtrl_InsertItem() will
    // change the notebooks size itself!
    AdjustPageSize(pPage);

    // finally do insert it
    if ( TabCtrl_InsertItem(m_hwnd, nPage, &tcItem) == -1 )
    {
        wxLogError(wxT("Can't create the notebook page '%s'."), strText.c_str());

        return false;
    }

    // succeeded: save the pointer to the page
    m_pages.Insert(pPage, nPage);

    // we may need to adjust the size again if the notebook size changed:
    // normally this only happens for the first page we add (the tabs which
    // hadn't been there before are now shown) but for a multiline notebook it
    // can happen for any page at all as a new row could have been started
    if ( m_pages.GetCount() == 1 || HasFlag(wxNB_MULTILINE) )
    {
        AdjustPageSize(pPage);
    }

    // now deal with the selection
    // ---------------------------

    // if the inserted page is before the selected one, we must update the
    // index of the selected page
    if ( int(nPage) <= m_nSelection )
    {
        // one extra page added
        m_nSelection++;
    }

    // some page should be selected: either this one or the first one if there
    // is still no selection
    int selNew = -1;
    if ( bSelect )
        selNew = nPage;
    else if ( m_nSelection == -1 )
        selNew = 0;

    if ( selNew != -1 )
        SetSelection(selNew);

    InvalidateBestSize();

    return true;
}

int wxNotebook::HitTest(const wxPoint& pt, long *flags) const
{
    TC_HITTESTINFO hitTestInfo;
    hitTestInfo.pt.x = pt.x;
    hitTestInfo.pt.y = pt.y;
    int item = TabCtrl_HitTest(GetHwnd(), &hitTestInfo);

    if ( flags )
    {
        *flags = 0;

        if ((hitTestInfo.flags & TCHT_NOWHERE) == TCHT_NOWHERE)
            *flags |= wxNB_HITTEST_NOWHERE;
        if ((hitTestInfo.flags & TCHT_ONITEM) == TCHT_ONITEM)
            *flags |= wxNB_HITTEST_ONITEM;
        if ((hitTestInfo.flags & TCHT_ONITEMICON) == TCHT_ONITEMICON)
            *flags |= wxNB_HITTEST_ONICON;
        if ((hitTestInfo.flags & TCHT_ONITEMLABEL) == TCHT_ONITEMLABEL)
            *flags |= wxNB_HITTEST_ONLABEL;
    }

    return item;
}


// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

void wxNotebook::OnSize(wxSizeEvent& event)
{
    // update the background brush
#if wxUSE_UXTHEME
    UpdateBgBrush();
#endif // wxUSE_UXTHEME

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
    }

    TabCtrl_AdjustRect(m_hwnd, false, &rc);

    int width = rc.right - rc.left,
        height = rc.bottom - rc.top;
    size_t nCount = m_pages.Count();
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
        RefreshRect(wxRect(rc.right, rc.top, widthNbook - rc.bottom, height),
                    false);
    }

    event.Skip();
}

void wxNotebook::OnSelChange(wxNotebookEvent& event)
{
  // is it our tab control?
  if ( event.GetEventObject() == this )
  {
      int sel = event.GetOldSelection();
      if ( sel != -1 )
        m_pages[sel]->Show(false);

      sel = event.GetSelection();
      if ( sel != -1 )
      {
        wxNotebookPage *pPage = m_pages[sel];
        pPage->Show(true);
        pPage->SetFocus();

        // If the newly focused window is not a child of the new page,
        // SetFocus was not successful and the notebook itself should be
        // focused
        wxWindow *currentFocus = FindFocus();
        wxWindow *startFocus = currentFocus;
        while ( currentFocus && currentFocus != pPage && currentFocus != this )
            currentFocus = currentFocus->GetParent();

        if ( startFocus == pPage || currentFocus != pPage )
            SetFocus();

      }
      else // no pages in the notebook, give the focus to itself
      {
          SetFocus();
      }

      m_nSelection = sel;
  }

  // we want to give others a chance to process this message as well
  event.Skip();
}

bool wxNotebook::MSWTranslateMessage(WXMSG *wxmsg)
{
    const MSG * const msg = (MSG *)wxmsg;

    // intercept TAB, CTRL+TAB and CTRL+SHIFT+TAB for processing by wxNotebook.
    // TAB will be passed to the currently selected page, CTRL+TAB and
    // CTRL+SHIFT+TAB will be processed by the notebook itself. do not
    // intercept SHIFT+TAB. This goes to the parent of the notebook which will
    // process it.
    if ( msg->message == WM_KEYDOWN && msg->wParam == VK_TAB &&
            msg->hwnd == m_hwnd &&
                (wxIsCtrlDown() || !wxIsShiftDown()) )
    {
        return MSWProcessMessage(wxmsg);
    }

    return false;
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

        const bool isFromParent = event.GetEventObject() == parent;
        const bool isFromSelf = event.GetEventObject() == this;

        if ( isFromParent || isFromSelf )
        {
            // no, it doesn't come from child, case (b) or (c): forward to a
            // page but only if direction is backwards (TAB) or from ourselves,
            if ( m_nSelection != -1 &&
                    (!event.GetDirection() || isFromSelf) )
            {
                // so that the page knows that the event comes from it's parent
                // and is being propagated downwards
                event.SetEventObject(this);

                wxWindow *page = m_pages[m_nSelection];
                if ( !page->GetEventHandler()->ProcessEvent(event) )
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
            if ( !event.GetDirection() )
            {
                SetFocus();
            }
            else if ( parent )
            {
                event.SetCurrentFocus(this);
                parent->GetEventHandler()->ProcessEvent(event);
            }
        }
    }
}

#if wxUSE_UXTHEME

WXHANDLE wxNotebook::QueryBgBitmap(wxWindow *win)
{
    RECT rc;
    GetWindowRect(GetHwnd(), &rc);

    WindowHDC hDC(GetHwnd());
    MemoryHDC hDCMem(hDC);
    CompatibleBitmap hBmp(hDC, rc.right - rc.left, rc.bottom - rc.top);

    SelectInHDC selectBmp(hDCMem, hBmp);

    ::SendMessage(GetHwnd(), WM_PRINTCLIENT,
                  (WPARAM)(HDC)hDCMem,
                  PRF_ERASEBKGND | PRF_CLIENT | PRF_NONCLIENT);

    if ( win )
    {
        RECT rc2;
        ::GetWindowRect(GetHwndOf(win), &rc2);

        COLORREF c = ::GetPixel(hDCMem, rc2.left - rc.left, rc2.top - rc.top);

        return (WXHANDLE)c;
    }
    //else: we are asked to create the brush

    return (WXHANDLE)::CreatePatternBrush(hBmp);
}

void wxNotebook::UpdateBgBrush()
{
    if ( m_hbrBackground )
        ::DeleteObject((HBRUSH)m_hbrBackground);

    if ( !m_hasBgCol && wxUxThemeEngine::GetIfActive() )
    {
        m_hbrBackground = (WXHBRUSH)QueryBgBitmap();
    }
    else // no themes
    {
        m_hbrBackground = NULL;
    }
}

WXHBRUSH wxNotebook::MSWGetBgBrushForChild(WXHDC hDC, wxWindow *win)
{
    if ( m_hbrBackground )
    {
        // before drawing with the background brush, we need to position it
        // correctly
        RECT rc;
        ::GetWindowRect(GetHwndOf(win), &rc);

        ::MapWindowPoints(NULL, GetHwnd(), (POINT *)&rc, 1);

        if ( !::SetBrushOrgEx((HDC)hDC, -rc.left, -rc.top, NULL) )
        {
            wxLogLastError(_T("SetBrushOrgEx(notebook bg brush)"));
        }

        return m_hbrBackground;
    }

    return wxNotebookBase::MSWGetBgBrushForChild(hDC, win);
}

wxColour wxNotebook::MSWGetBgColourForChild(wxWindow *win)
{
    if ( m_hasBgCol )
        return GetBackgroundColour();

    if ( !wxUxThemeEngine::GetIfActive() )
        return wxNullColour;

    COLORREF c = (COLORREF)QueryBgBitmap(win);

    return c == CLR_INVALID ? wxNullColour : wxRGBToColour(c);
}

#endif // wxUSE_UXTHEME

// Windows only: attempts to get colour for UX theme page background
wxColour wxNotebook::GetThemeBackgroundColour() const
{
#if wxUSE_UXTHEME
    if (wxUxThemeEngine::Get())
    {
        wxUxThemeHandle hTheme((wxNotebook*) this, L"TAB");
        if (hTheme)
        {
            // This is total guesswork.
            // See PlatformSDK\Include\Tmschema.h for values
            COLORREF themeColor;
            wxUxThemeEngine::Get()->GetThemeColor(
                                        hTheme,
                                        10 /* TABP_BODY */,
                                        1 /* NORMAL */,
                                        3821 /* FILLCOLORHINT */,
                                        &themeColor);

            /*
            [DS] Workaround for WindowBlinds:
            Some themes return a near black theme color using FILLCOLORHINT,
            this makes notebook pages have an ugly black background and makes
            text (usually black) unreadable. Retry again with FILLCOLOR.

            This workaround potentially breaks appearance of some themes,
            but in practice it already fixes some themes.
            */
            if (themeColor == 1)
            {
                wxUxThemeEngine::Get()->GetThemeColor(
                                            hTheme,
                                            10 /* TABP_BODY */,
                                            1 /* NORMAL */,
                                            3802 /* FILLCOLOR */,
                                            &themeColor);
            }

            wxColour colour(GetRValue(themeColor), GetGValue(themeColor), GetBValue(themeColor));
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
  wxNotebookEvent event(wxEVT_NULL, m_windowId);

  NMHDR* hdr = (NMHDR *)lParam;
  switch ( hdr->code ) {
    case TCN_SELCHANGE:
      event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED);
      break;

    case TCN_SELCHANGING:
      event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING);
      break;

    default:
      return wxControl::MSWOnNotify(idCtrl, lParam, result);
  }

  event.SetSelection(TabCtrl_GetCurSel(m_hwnd));
  event.SetOldSelection(m_nSelection);
  event.SetEventObject(this);
  event.SetInt(idCtrl);

  bool processed = GetEventHandler()->ProcessEvent(event);
  *result = !event.IsAllowed();
  return processed;
}

#endif // wxUSE_NOTEBOOK
