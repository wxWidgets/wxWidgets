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

// TODO: 
// 1) keyboard interface for changing pages ([Shift]+Ctrl-Tab)
// 2) using OnSize() for showing pages for the first time works, but it surely
//    us ugly
// 3) I'm not sure that setting fonts works

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
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
  #include  <wx/string.h>
#endif  // WX_PRECOMP

#include  <wx/log.h>
#include  <wx/imaglist.h>
#include  <wx/notebook.h>

#include  <wx/msw/private.h>

// Windows standard headers
#ifndef   __WIN95__
  #error  "wxNotebook is not supported under Windows 3.1"
#endif    //Win95

#ifdef __GNUWIN32__
  #include "wx/msw/gnuwin32/extra.h"
#else   //!GnuWin32
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
  IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxCommandEvent)
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
  SetName(name);
  SetParent(parent);

  m_windowId = id == -1 ? NewControlId() : id;

  // colors and font
  m_backgroundColour = wxColour(GetSysColor(COLOR_BTNFACE));
  m_foregroundColour = *wxBLACK ;

  m_defaultForegroundColour = *wxBLACK ;
  m_defaultBackgroundColour = wxColour(GetSysColor(COLOR_BTNFACE));

  // style
  m_windowStyle = style;

  long tabStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | TCS_TABS;
  if ( m_windowStyle & wxTC_MULTILINE )
    tabStyle |= TCS_MULTILINE;
  if ( m_windowStyle & wxBORDER )
    tabStyle &= WS_BORDER;

  // create the tab control.
  m_hWnd = (WXHWND)CreateWindowEx
    (
      0,                                      // extended style
      WC_TABCONTROL,                          // class name for the tab control
      "",                                     // no caption
      tabStyle,                               // style
      pos.x, pos.y, size.x, size.y,           // size and position
      (HWND)parent->GetHWND(),                // parent window
      (HMENU)m_windowId,                      // child id
      wxGetInstance(),                        // current instance
      NULL                                    // no class data
    );

  if ( m_hWnd == 0 ) {
    wxLogSysError("Can't create the notebook control");
    return FALSE;
  }

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
  wxASSERT( IS_VALID_PAGE(nPage) );

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
  wxASSERT( IS_VALID_PAGE(nPage) );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_TEXT;
  tcItem.pszText = (char *)strText.c_str();

  return TabCtrl_SetItem(m_hwnd, nPage, &tcItem) != 0;
}

wxString wxNotebook::GetPageText(int nPage) const
{
  wxASSERT( IS_VALID_PAGE(nPage) );

  char buf[256];
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
  wxASSERT( IS_VALID_PAGE(nPage) );

  TC_ITEM tcItem;
  tcItem.mask = TCIF_IMAGE;

  return TabCtrl_GetItem(m_hwnd, nPage, &tcItem) ? tcItem.iImage : -1;
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
  wxASSERT( IS_VALID_PAGE(nPage) );

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

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook
bool wxNotebook::DeletePage(int nPage)
{
  wxCHECK( IS_VALID_PAGE(nPage), FALSE );

  TabCtrl_DeleteItem(m_hwnd, nPage);

  delete m_aPages[nPage];
  m_aPages.Remove(nPage);

  return TRUE;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
  TabCtrl_DeleteAllItems(m_hwnd);

  int nPageCount = GetPageCount();
  int nPage;
  for ( nPage = 0; nPage < nPageCount; nPage++ )
    delete m_aPages[nPage];

  m_aPages.Clear();

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

  // add the tab to the control
  TC_ITEM tcItem;
  tcItem.mask    = TCIF_TEXT | TCIF_IMAGE;
  tcItem.pszText = (char *)strText.c_str();
  tcItem.iImage  = imageId;

  if ( TabCtrl_InsertItem(m_hwnd, nPage, &tcItem) == -1 ) {
    wxLogError("Can't create the notebook page '%s'.", strText.c_str());
    return FALSE;
  }

  // save the pointer to the page
  m_aPages.Insert(pPage, nPage);

  // some page must be selected: either this one or the first one if there is 
  // still no selection
  if ( bSelect )
    m_nSelection = nPage;
  else if ( m_nSelection == -1 )
    m_nSelection = 0;

  // don't show pages by default (we'll need to adjust their size first)
  HWND hwnd = (HWND)pPage->GetHWND();
  SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_VISIBLE);

  return TRUE;
}

// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

// @@@ OnSize() is used for setting the font when it's called for the first
//     time because doing it in ::Create() doesn't work (for unknown reasons)
void wxNotebook::OnSize(wxSizeEvent& event)
{
  static bool s_bFirstTime = TRUE;
  if ( s_bFirstTime ) {
    SendMessage((HWND)m_hwnd, WM_SETFONT,
                (WPARAM)GetStockObject(DEFAULT_GUI_FONT),
                MAKELPARAM(TRUE, 0));
    s_bFirstTime = FALSE;
  }

  // emulate page change (it's esp. important to do it first time because
  // otherwise our page would stay invisible)
  int nSel = m_nSelection;
  m_nSelection = -1;
  SetSelection(nSel);

  // fit the notebook page to the tab control's display area
  RECT rc;
  rc.left = rc.top = 0;
  GetSize((int *)&rc.right, (int *)&rc.bottom);

  TabCtrl_AdjustRect(m_hwnd, FALSE, &rc);
  uint nCount = m_aPages.Count();
  for ( uint nPage = 0; nPage < nCount; nPage++ ) {
    wxNotebookPage *pPage = m_aPages[nPage];
    pPage->SetSize(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
    if ( pPage->GetAutoLayout() )
      pPage->Layout();
  }

  event.Skip();
}

void wxNotebook::OnSelChange(wxNotebookEvent& event)
{
  // is it our tab control?
  if ( event.GetEventObject() == this )
    ChangePage(event.GetOldSelection(), event.GetSelection());

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
      GetParent()->ProcessEvent(event);
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

void wxNotebook::Command(wxCommandEvent& event)
{
  wxFAIL_MSG("wxNotebook::Command not implemented");
}

bool wxNotebook::MSWNotify(WXWPARAM wParam, WXLPARAM lParam)
{
  wxNotebookEvent event(wxEVT_NULL, m_windowId, 
                        TabCtrl_GetCurSel(m_hwnd), m_nSelection);

  NMHDR* hdr = (NMHDR *)lParam;
  switch ( hdr->code ) {
    case TCN_SELCHANGE:
      event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED);
      break;

    case TCN_SELCHANGING:
      event.SetEventType(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING);
      break;

    default :
      return wxControl::MSWNotify(wParam, lParam);
  }

  event.SetEventObject(this);
  event.SetInt(LOWORD(wParam));

  return ProcessEvent(event);
}

// ----------------------------------------------------------------------------
// wxNotebook helper functions
// ----------------------------------------------------------------------------

// hide the currently active panel and show the new one
void wxNotebook::ChangePage(int nOldSel, int nSel)
{
  wxASSERT( nOldSel != nSel ); // impossible

  if ( nOldSel != -1 ) {
    m_aPages[nOldSel]->Show(FALSE);
  }

  wxNotebookPage *pPage = m_aPages[nSel];
  pPage->Show(TRUE);
  pPage->SetFocus();

  m_nSelection = nSel;
}
