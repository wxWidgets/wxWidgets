///////////////////////////////////////////////////////////////////////////////
// Name:        msw/notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.06.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "notebook.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// wxWindows
#ifndef WX_PRECOMP
  #include  "wx/string.h"
#endif  // WX_PRECOMP

#include  "wx/log.h"
#include  "wx/imaglist.h"
#include  "wx/event.h"
#include  "wx/control.h"
#include  "wx/notebook.h"

#include  "wx/msw/private.h"

// Windows standard headers
#ifndef   __WIN95__
  #error  "wxNotebook is only supported Windows 95 and above"
#endif    //Win95

#include  <windowsx.h>  // for SetWindowFont

#ifndef __TWIN32__
#ifdef __GNUWIN32__
#ifndef wxUSE_NORLANDER_HEADERS
  #include "wx/msw/gnuwin32/extra.h"
#endif
#endif
#endif

#if !defined(__GNUWIN32__) || defined(__TWIN32__) || defined(wxUSE_NORLANDER_HEADERS)
  #include <commctrl.h>
#endif

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) (((nPage) >= 0) && ((nPage) < GetPageCount()))

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

#if !USE_SHARED_LIBRARIES
  BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, wxNotebook::OnSelChange)

    EVT_SIZE(wxNotebook::OnSize)

    EVT_SET_FOCUS(wxNotebook::OnSetFocus)

    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
  END_EVENT_TABLE()

  IMPLEMENT_DYNAMIC_CLASS(wxNotebook, wxControl)
  IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

// common part of all ctors
void wxNotebook::Init()
{
  m_pImageList = NULL;
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
  // base init
  if ( !CreateBase(parent, id, pos, size, style, wxDefaultValidator, name) )
      return FALSE;

  // colors and font
  m_backgroundColour = wxColour(GetSysColor(COLOR_BTNFACE));
  m_foregroundColour = *wxBLACK ;

  // style
  m_windowStyle = style | wxTAB_TRAVERSAL;

  long tabStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TCS_TABS;

  if (m_windowStyle & wxCLIP_CHILDREN)
    tabStyle |= WS_CLIPCHILDREN;
  if ( m_windowStyle & wxTC_MULTILINE )
    tabStyle |= TCS_MULTILINE;
  if ( m_windowStyle & wxBORDER )
    tabStyle &= WS_BORDER;
  if (m_windowStyle & wxNB_FIXEDWIDTH)
    tabStyle |= TCS_FIXEDWIDTH ;
  if (m_windowStyle & wxNB_BOTTOM)
    tabStyle |= TCS_RIGHT;
  if (m_windowStyle & wxNB_LEFT)
    tabStyle |= TCS_VERTICAL;
  if (m_windowStyle & wxNB_RIGHT)
    tabStyle |= TCS_VERTICAL|TCS_RIGHT;


  if ( !MSWCreate(GetId(), GetParent(), WC_TABCONTROL,
                  this, NULL, pos.x, pos.y, size.x, size.y,
                  tabStyle, NULL, 0) )
  {
    return FALSE;
  }

  // Not all compilers recognise SetWindowFont
  ::SendMessage(GetHwnd(), WM_SETFONT,
                (WPARAM)::GetStockObject(DEFAULT_GUI_FONT), TRUE);


  if ( parent != NULL )
    parent->AddChild(this);

  SubclassWin(m_hWnd);

  return TRUE;
}

// dtor
wxNotebook::~wxNotebook()
{
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------
int wxNotebook::GetPageCount() const
{
  // consistency check
  wxASSERT( (int)m_aPages.Count() == TabCtrl_GetItemCount(m_hwnd) );

  return m_aPages.Count();
}

int wxNotebook::GetRowCount() const
{
  return TabCtrl_GetRowCount(m_hwnd);
}

int wxNotebook::SetSelection(int nPage)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

  ChangePage(m_nSelection, nPage);

  return TabCtrl_SetCurSel(m_hwnd, nPage);
}

void wxNotebook::AdvanceSelection(bool bForward)
{
  int nSel = GetSelection();
  int nMax = GetPageCount() - 1;
  if ( bForward )
    SetSelection(nSel == nMax ? 0 : nSel + 1);
  else
    SetSelection(nSel == 0 ? nMax : nSel - 1);
}

bool wxNotebook::SetPageText(int nPage, const wxString& strText)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_TEXT;
  tcItem.pszText = (wxChar *)strText.c_str();

  return TabCtrl_SetItem(m_hwnd, nPage, &tcItem) != 0;
}

wxString wxNotebook::GetPageText(int nPage) const
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), wxT(""), wxT("notebook page out of range") );

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

int wxNotebook::GetPageImage(int nPage) const
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_IMAGE;

  return TabCtrl_GetItem(m_hwnd, nPage, &tcItem) ? tcItem.iImage : -1;
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_IMAGE;
  tcItem.iImage = nImage;

  return TabCtrl_SetItem(m_hwnd, nPage, &tcItem) != 0;
}

void wxNotebook::SetImageList(wxImageList* imageList)
{
  m_pImageList = imageList;
  TabCtrl_SetImageList(m_hwnd, (HIMAGELIST)imageList->GetHIMAGELIST());
}


// Windows-only at present. Also, you must use the wxNB_FIXEDWIDTH
// style.
void wxNotebook::SetTabSize(const wxSize& sz)
{
    ::SendMessage(GetHwnd(), TCM_SETITEMSIZE, 0, MAKELPARAM(sz.x, sz.y));
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook
bool wxNotebook::DeletePage(int nPage)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

  if ( m_nSelection == nPage ) {
      // advance selection backwards - the page being deleted shouldn't be left
      // selected
      AdvanceSelection(FALSE);
  }

  TabCtrl_DeleteItem(m_hwnd, nPage);

  delete m_aPages[nPage];
  m_aPages.Remove(nPage);

  if ( m_aPages.IsEmpty() ) {
      // no selection if the notebook became empty
      m_nSelection = -1;
  }

  return TRUE;
}

// remove one page from the notebook, without deleting
bool wxNotebook::RemovePage(int nPage)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

  TabCtrl_DeleteItem(m_hwnd, nPage);

  m_aPages.Remove(nPage);

  return TRUE;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
  int nPageCount = GetPageCount();
  int nPage;
  for ( nPage = 0; nPage < nPageCount; nPage++ )
    delete m_aPages[nPage];

  m_aPages.Clear();

  TabCtrl_DeleteAllItems(m_hwnd);

  return TRUE;
}

// add a page to the notebook
bool wxNotebook::AddPage(wxNotebookPage *pPage,
                         const wxString& strText,
                         bool bSelect,
                         int imageId)
{
  return InsertPage(GetPageCount(), pPage, strText, bSelect, imageId);
}

// same as AddPage() but does it at given position
bool wxNotebook::InsertPage(int nPage,
                            wxNotebookPage *pPage,
                            const wxString& strText,
                            bool bSelect,
                            int imageId)
{
  wxASSERT( pPage != NULL );
  wxCHECK( IS_VALID_PAGE(nPage) || nPage == GetPageCount(), FALSE );

  // do add the tab to the control

  // init all fields to 0
  TC_ITEM tcItem;
  memset(&tcItem, 0, sizeof(tcItem));

  if ( imageId != -1 )
  {
    tcItem.mask |= TCIF_IMAGE;
    tcItem.iImage  = imageId;
  }

  if ( !strText.IsEmpty() )
  {
    tcItem.mask |= TCIF_TEXT;
    tcItem.pszText = (wxChar *)strText.c_str(); // const_cast
  }

  if ( TabCtrl_InsertItem(m_hwnd, nPage, &tcItem) == -1 ) {
    wxLogError(wxT("Can't create the notebook page '%s'."), strText.c_str());

    return FALSE;
  }

  // if the inserted page is before the selected one, we must update the
  // index of the selected page
  if ( nPage <= m_nSelection )
  {
    // one extra page added
    m_nSelection++;
  }

  // save the pointer to the page
  m_aPages.Insert(pPage, nPage);

  // don't show pages by default (we'll need to adjust their size first)
  HWND hwnd = GetWinHwnd(pPage);
  SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_VISIBLE);

  // this updates internal flag too - otherwise it will get out of sync
  pPage->Show(FALSE);

  // fit the notebook page to the tab control's display area
  RECT rc;
  rc.left = rc.top = 0;
  GetSize((int *)&rc.right, (int *)&rc.bottom);
  TabCtrl_AdjustRect(m_hwnd, FALSE, &rc);
  pPage->SetSize(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);


  // some page should be selected: either this one or the first one if there is
  // still no selection
  int selNew = -1;
  if ( bSelect )
    selNew = nPage;
  else if ( m_nSelection == -1 )
    selNew = 0;

  if ( selNew != -1 )
    SetSelection(selNew);

  return TRUE;
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
  size_t nCount = m_aPages.Count();
  for ( size_t nPage = 0; nPage < nCount; nPage++ ) {
    wxNotebookPage *pPage = m_aPages[nPage];
    pPage->SetSize(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
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
        m_aPages[sel]->Show(FALSE);

      sel = event.GetSelection();
      if ( sel != -1 )
      {
        wxNotebookPage *pPage = m_aPages[sel];
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
  // set focus to the currently selected page if any
  if ( m_nSelection != -1 )
    m_aPages[m_nSelection]->SetFocus();

  event.Skip();
}

void wxNotebook::OnNavigationKey(wxNavigationKeyEvent& event)
{
  if ( event.IsWindowChange() ) {
    // change pages
    AdvanceSelection(event.GetDirection());
  }
  else {
    // pass to the parent
    if ( GetParent() ) {
      event.SetCurrentFocus(this);
      GetParent()->GetEventHandler()->ProcessEvent(event);
    }
  }
}

// ----------------------------------------------------------------------------
// wxNotebook base class virtuals
// ----------------------------------------------------------------------------

// override these 2 functions to do nothing: everything is done in OnSize

void wxNotebook::SetConstraintSizes(bool /* recurse */)
{
  // don't set the sizes of the pages - their correct size is not yet known
  wxControl::SetConstraintSizes(FALSE);
}

bool wxNotebook::DoPhase(int /* nPhase */)
{
  return TRUE;
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

// ----------------------------------------------------------------------------
// wxNotebook helper functions
// ----------------------------------------------------------------------------

// generate the page changing and changed events, hide the currently active
// panel and show the new one
void wxNotebook::ChangePage(int nOldSel, int nSel)
{
  // MT-FIXME should use a real semaphore
  static bool s_bInsideChangePage = FALSE;

  // when we call ProcessEvent(), our own OnSelChange() is called which calls
  // this function - break the infinite loop
  if ( s_bInsideChangePage )
    return;

  // it's not an error (the message may be generated by the tab control itself)
  // and it may happen - just do nothing
  if ( nSel == nOldSel )
    return;

  s_bInsideChangePage = TRUE;

  wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, m_windowId);
  event.SetSelection(nSel);
  event.SetOldSelection(nOldSel);
  event.SetEventObject(this);
  if ( ProcessEvent(event) && !event.IsAllowed() )
  {
    // program doesn't allow the page change
    s_bInsideChangePage = FALSE;
    return;
  }

  event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED);
  ProcessEvent(event);

  s_bInsideChangePage = FALSE;
}
