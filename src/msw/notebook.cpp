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

// wxWindows
#ifndef WX_PRECOMP
  #include  "wx/string.h"
#endif  // WX_PRECOMP

#include  "wx/log.h"
#include  "wx/imaglist.h"
#include  "wx/event.h"
#include  "wx/control.h"
#include  "wx/notebook.h"
#include  "wx/app.h"

#include  "wx/msw/private.h"

// Windows standard headers
#ifndef   __WIN95__
  #error  "wxNotebook is only supported Windows 95 and above"
#endif    //Win95

#include  <windowsx.h>  // for SetWindowFont

#ifdef __GNUWIN32_OLD__
    #include "wx/msw/gnuwin32/extra.h"
#endif

#if defined(__WIN95__) && !(defined(__GNUWIN32_OLD__) && !defined(__CYGWIN10__))
    #include <commctrl.h>
#endif

#include "wx/msw/winundef.h"

#if wxUSE_UXTHEME
#include "wx/msw/uxtheme.h"

#include "wx/radiobut.h"
#include "wx/radiobox.h"
#include "wx/checkbox.h"
#include "wx/bmpbuttn.h"
#include "wx/statline.h"
#include "wx/statbox.h"
#include "wx/stattext.h"
#include "wx/slider.h"
#include "wx/scrolwin.h"
#include "wx/panel.h"
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

    EVT_SET_FOCUS(wxNotebook::OnSetFocus)

    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
END_EVENT_TABLE()

#if wxUSE_EXTENDED_RTTI
WX_DEFINE_FLAGS( wxNotebookStyle )

WX_BEGIN_FLAGS( wxNotebookStyle )
    // new style border flags, we put them first to
    // use them for streaming out
    WX_FLAGS_MEMBER(wxBORDER_SIMPLE)
    WX_FLAGS_MEMBER(wxBORDER_SUNKEN)
    WX_FLAGS_MEMBER(wxBORDER_DOUBLE)
    WX_FLAGS_MEMBER(wxBORDER_RAISED)
    WX_FLAGS_MEMBER(wxBORDER_STATIC)
    WX_FLAGS_MEMBER(wxBORDER_NONE)
    
    // old style border flags
    WX_FLAGS_MEMBER(wxSIMPLE_BORDER)
    WX_FLAGS_MEMBER(wxSUNKEN_BORDER)
    WX_FLAGS_MEMBER(wxDOUBLE_BORDER)
    WX_FLAGS_MEMBER(wxRAISED_BORDER)
    WX_FLAGS_MEMBER(wxSTATIC_BORDER)
    WX_FLAGS_MEMBER(wxNO_BORDER)

    // standard window styles
    WX_FLAGS_MEMBER(wxTAB_TRAVERSAL)
    WX_FLAGS_MEMBER(wxCLIP_CHILDREN)
    WX_FLAGS_MEMBER(wxTRANSPARENT_WINDOW)
    WX_FLAGS_MEMBER(wxWANTS_CHARS)
    WX_FLAGS_MEMBER(wxNO_FULL_REPAINT_ON_RESIZE)
    WX_FLAGS_MEMBER(wxALWAYS_SHOW_SB )
    WX_FLAGS_MEMBER(wxVSCROLL)
    WX_FLAGS_MEMBER(wxHSCROLL)

    WX_FLAGS_MEMBER(wxNB_FIXEDWIDTH)
    WX_FLAGS_MEMBER(wxNB_LEFT)
    WX_FLAGS_MEMBER(wxNB_RIGHT)
    WX_FLAGS_MEMBER(wxNB_BOTTOM)

WX_END_FLAGS( wxNotebookStyle )

IMPLEMENT_DYNAMIC_CLASS_XTI(wxNotebook, wxControl,"wx/notebook.h")
IMPLEMENT_DYNAMIC_CLASS_XTI(wxNotebookPageInfo, wxObject , "wx/notebook.h" )

WX_COLLECTION_TYPE_INFO( wxNotebookPageInfo * , wxNotebookPageInfoList ) ;

template<> void wxCollectionToVariantArray( wxNotebookPageInfoList const &theList, wxxVariantArray &value)
{
    wxListCollectionToVariantArray<wxNotebookPageInfoList::compatibility_iterator>( theList , value ) ;
}

WX_BEGIN_PROPERTIES_TABLE(wxNotebook)
    WX_DELEGATE( OnPageChanging , wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING , wxNotebookEvent )
    WX_DELEGATE( OnPageChanged , wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED , wxNotebookEvent )

    WX_PROPERTY_COLLECTION( PageInfos , wxNotebookPageInfoList , wxNotebookPageInfo* , AddPageInfo , GetPageInfos , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    WX_PROPERTY_FLAGS( WindowStyle , wxNotebookStyle , long , SetWindowStyleFlag , GetWindowStyleFlag , , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // style
WX_END_PROPERTIES_TABLE()

WX_BEGIN_HANDLERS_TABLE(wxNotebook)
WX_END_HANDLERS_TABLE()

WX_CONSTRUCTOR_5( wxNotebook , wxWindow* , Parent , wxWindowID , Id , wxPoint , Position , wxSize , Size , long , WindowStyle) 


WX_BEGIN_PROPERTIES_TABLE(wxNotebookPageInfo)
    WX_READONLY_PROPERTY( Page , wxNotebookPage* , GetPage , , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    WX_READONLY_PROPERTY( Text , wxString , GetText , wxString() , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    WX_READONLY_PROPERTY( Selected , bool , GetSelected , false, 0 /*flags*/ , wxT("Helpstring") , wxT("group") )
    WX_READONLY_PROPERTY( ImageId , int , GetImageId , -1 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
WX_END_PROPERTIES_TABLE()

WX_BEGIN_HANDLERS_TABLE(wxNotebookPageInfo)
WX_END_HANDLERS_TABLE()

WX_CONSTRUCTOR_4( wxNotebookPageInfo , wxNotebookPage* , Page , wxString , Text , bool , Selected , int , ImageId ) 

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
    // Does ComCtl32 support non-top tabs?
    int verComCtl32 = wxApp::GetComCtl32Version();
    if ( verComCtl32 < 470 || verComCtl32 >= 600 )
    {
        if (style & wxNB_BOTTOM)
            style &= ~wxNB_BOTTOM;
        
        if (style & wxNB_LEFT)
            style &= ~wxNB_LEFT;
        
        if (style & wxNB_RIGHT)
            style &= ~wxNB_RIGHT;
    }
    
    if ( !CreateControl(parent, id, pos, size, style | wxTAB_TRAVERSAL,
                        wxDefaultValidator, name) )
        return FALSE;

    if ( !MSWCreateControl(WC_TABCONTROL, wxEmptyString, pos, size) )
        return FALSE;

    SetBackgroundColour(wxColour(::GetSysColor(COLOR_BTNFACE)));

    return TRUE;
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
  wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

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
  wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

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
  wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

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

    TabCtrl_AdjustRect(GetHwnd(), TRUE, &rc);

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
    
    // We need to make getting tab size part of the wxWindows API.
    wxSize tabSize(0, 0);
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
    TabCtrl_AdjustRect(m_hwnd, FALSE, &rc);

    page->SetSize(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
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
        // change the selected page if it was deleted or became invalid
        int selNew;
        if ( m_nSelection == int(GetPageCount()) )
        {
            // last page deleted, make the new last page the new selection
            selNew = m_nSelection - 1;
        }
        else if ( int(nPage) <= m_nSelection )
        {
            // we must show another page, even if it has the same index
            selNew = m_nSelection;
        }
        else // nothing changes for the currently selected page
        {
            selNew = -1;

            // we still must refresh the current page: this needs to be done
            // for some unknown reason if the tab control shows the up-down
            // control (i.e. when there are too many pages) -- otherwise after
            // deleting a page nothing at all is shown
            if (m_nSelection >= 0)
                m_pages[m_nSelection]->Refresh();
        }

        if ( selNew != -1 )
        {
            // m_nSelection must be always valid so reset it before calling
            // SetSelection()
            m_nSelection = -1;
            SetSelection(selNew);
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

  return TRUE;
}

// same as AddPage() but does it at given position
bool wxNotebook::InsertPage(size_t nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
    wxCHECK_MSG( pPage != NULL, FALSE, _T("NULL page in wxNotebook::InsertPage") );
    wxCHECK_MSG( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), FALSE,
                 _T("invalid index in wxNotebook::InsertPage") );

    wxASSERT_MSG( pPage->GetParent() == this,
                    _T("notebook pages must have notebook as parent") );

#if wxUSE_UXTHEME && wxUSE_UXTHEME_AUTO
    static bool g_TestedForTheme = FALSE;
    static bool g_UseTheme = FALSE;
    if (!g_TestedForTheme)
    {
        int commCtrlVersion = wxTheApp->GetComCtl32Version() ;
        
        g_UseTheme = (commCtrlVersion >= 600);
        g_TestedForTheme = TRUE;
    }
    
    // Automatically apply the theme background,
    // changing the colour of the panel to match the
    // tab page colour. This won't work well with all
    // themes but it's a start.
    if (g_UseTheme && wxUxThemeEngine::Get() && pPage->IsKindOf(CLASSINFO(wxPanel)))
    {
        ApplyThemeBackground(pPage, GetThemeBackgroundColour());
    }
#endif

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
    if ( !strText.IsEmpty() )
    {
        tcItem.mask |= TCIF_TEXT;
        tcItem.pszText = (wxChar *)strText.c_str(); // const_cast
    }

    // fit the notebook page to the tab control's display area: this should be
    // done before adding it to the notebook or TabCtrl_InsertItem() will
    // change the notebooks size itself!
    AdjustPageSize(pPage);

    // finally do insert it
    if ( TabCtrl_InsertItem(m_hwnd, nPage, &tcItem) == -1 )
    {
        wxLogError(wxT("Can't create the notebook page '%s'."), strText.c_str());

        return FALSE;
    }

    // succeeded: save the pointer to the page
    m_pages.Insert(pPage, nPage);

    // for the first page (only) we need to adjust the size again because the
    // notebook size changed: the tabs which hadn't been there before are now
    // shown
    if ( m_pages.GetCount() == 1 )
    {
        AdjustPageSize(pPage);
    }

    // hide the page: unless it is selected, it shouldn't be shown (and if it
    // is selected it will be shown later)
    HWND hwnd = GetWinHwnd(pPage);
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_VISIBLE);

    // this updates internal flag too -- otherwise it would get out of sync
    // with the real state
    pPage->Show(FALSE);


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

    return TRUE;
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
  // fit the notebook page to the tab control's display area
  RECT rc;
  rc.left = rc.top = 0;
  GetSize((int *)&rc.right, (int *)&rc.bottom);

  TabCtrl_AdjustRect(m_hwnd, FALSE, &rc);

  int width = rc.right - rc.left,
      height = rc.bottom - rc.top;
  size_t nCount = m_pages.Count();
  for ( size_t nPage = 0; nPage < nCount; nPage++ ) {
    wxNotebookPage *pPage = m_pages[nPage];
    pPage->SetSize(rc.left, rc.top, width, height);
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
        m_pages[sel]->Show(FALSE);

      sel = event.GetSelection();
      if ( sel != -1 )
      {
        wxNotebookPage *pPage = m_pages[sel];
        pPage->Show(TRUE);
        pPage->SetFocus();
      }

      m_nSelection = sel;
  }

  // we want to give others a chance to process this message as well
  event.Skip();
}

void wxNotebook::OnSetFocus(wxFocusEvent& event)
{
    // this function is only called when the focus is explicitly set (i.e. from
    // the program) to the notebook - in this case we don't need the
    // complicated OnNavigationKey() logic because the programmer knows better
    // what [s]he wants

    // set focus to the currently selected page if any
    if ( m_nSelection != -1 )
        m_pages[m_nSelection]->SetFocus();

    event.Skip();
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
    if ( event.IsWindowChange() ) {
        // change pages
        AdvanceSelection(event.GetDirection());
    }
    else {
        // we get this event in 2 cases
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
        wxWindow *parent = GetParent();
        // the cast is here to fic a GCC ICE
        if ( ((wxWindow*)event.GetEventObject()) == parent )
        {
            // no, it doesn't come from child, case (b): forward to a page
            if ( m_nSelection != -1 )
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
            else
            {
                // we have no pages - still have to give focus to _something_
                SetFocus();
            }
        }
        else
        {
            // it comes from our child, case (a), pass to the parent
            if ( parent ) {
                event.SetCurrentFocus(this);
                parent->GetEventHandler()->ProcessEvent(event);
            }
        }
    }
}

// ----------------------------------------------------------------------------
// wxNotebook base class virtuals
// ----------------------------------------------------------------------------

#if wxUSE_CONSTRAINTS

// override these 2 functions to do nothing: everything is done in OnSize

void wxNotebook::SetConstraintSizes(bool WXUNUSED(recurse))
{
  // don't set the sizes of the pages - their correct size is not yet known
  wxControl::SetConstraintSizes(FALSE);
}

bool wxNotebook::DoPhase(int WXUNUSED(nPhase))
{
  return TRUE;
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
        return FALSE;

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

// Windows only: attempts to get colour for UX theme page background
wxColour wxNotebook::GetThemeBackgroundColour()
{
#if wxUSE_UXTHEME
    if (wxUxThemeEngine::Get())
    {
        wxUxThemeHandle hTheme(this, L"TAB");
        if (hTheme)
        {
            // This is total guesswork.
            // See PlatformSDK\Include\Tmschema.h for values
            COLORREF themeColor;
            wxUxThemeEngine::Get()->GetThemeColor
                                    (
                                        hTheme,
                                        10 /* TABP_BODY */,
                                        1 /* NORMAL */,
                                        3821, /* FILLCOLORHINT */
                                        & themeColor
                                    );
            
            wxColour colour(GetRValue(themeColor), GetGValue(themeColor), GetBValue(themeColor));
            return colour;
        }
    }
#endif // wxUSE_UXTHEME

    return GetBackgroundColour();
}

// Windows only: attempts to apply the UX theme page background to this page
#if wxUSE_UXTHEME
void wxNotebook::ApplyThemeBackground(wxWindow* window, const wxColour& colour)
#else
void wxNotebook::ApplyThemeBackground(wxWindow*, const wxColour&)
#endif
{
#if wxUSE_UXTHEME
    // Don't set the background for buttons since this will
    // switch it into ownerdraw mode
    if (window->IsKindOf(CLASSINFO(wxButton)) && !window->IsKindOf(CLASSINFO(wxBitmapButton)))
        // This is essential, otherwise you'll see dark grey
        // corners in the buttons.
        ((wxButton*)window)->wxControl::SetBackgroundColour(colour);
    else if (window->IsKindOf(CLASSINFO(wxStaticText)) ||
             window->IsKindOf(CLASSINFO(wxStaticBox)) ||
             window->IsKindOf(CLASSINFO(wxStaticLine)) ||
             window->IsKindOf(CLASSINFO(wxRadioButton)) ||
             window->IsKindOf(CLASSINFO(wxRadioBox)) ||
             window->IsKindOf(CLASSINFO(wxCheckBox)) ||
             window->IsKindOf(CLASSINFO(wxBitmapButton)) ||
             window->IsKindOf(CLASSINFO(wxSlider)) ||
             window->IsKindOf(CLASSINFO(wxPanel)) ||
             (window->IsKindOf(CLASSINFO(wxNotebook)) && (window != this)) ||
             window->IsKindOf(CLASSINFO(wxScrolledWindow))
        )
    {
        window->SetBackgroundColour(colour);
    }

    for ( wxWindowList::compatibility_iterator node = window->GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        ApplyThemeBackground(child, colour);
    }
#endif
}

long wxNotebook::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    static bool g_TestedForTheme = FALSE;
    static bool g_UseTheme = FALSE;
    switch ( nMsg )
    {
        case WM_ERASEBKGND:
        {
            if (!g_TestedForTheme)
            {
                int commCtrlVersion = wxTheApp->GetComCtl32Version() ;

                g_UseTheme = (commCtrlVersion >= 600);
                g_TestedForTheme = TRUE;
            }

            // If using XP themes, it seems we can get away
            // with not drawing a background, which reduces flicker.
            if (g_UseTheme)            
                return TRUE;
        }
    }

    return wxControl::MSWWindowProc(nMsg, wParam, lParam);
}


#endif // wxUSE_NOTEBOOK
