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

#ifndef __TWIN32__
    #ifdef __GNUWIN32_OLD__
        #include "wx/msw/gnuwin32/extra.h"
    #endif
#endif

#if defined(__WIN95__) && !((defined(__GNUWIN32_OLD__) || defined(__TWIN32__)) && !defined(__CYGWIN10__))
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

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, wxNotebook::OnSelChange)

    EVT_SIZE(wxNotebook::OnSize)

    EVT_SET_FOCUS(wxNotebook::OnSetFocus)

    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNotebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxNotifyEvent)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

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
    
    // base init
    if ( !CreateControl(parent, id, pos, size, style, wxDefaultValidator, name) )
        return FALSE;

    // notebook, so explicitly specify 0 as last parameter
    if ( !MSWCreateControl(WC_TABCONTROL, _T(""), pos, size,
                style | wxTAB_TRAVERSAL) )
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

int wxNotebook::GetPageCount() const
{
  // consistency check
  wxASSERT( (int)m_pages.Count() == TabCtrl_GetItemCount(m_hwnd) );

  return m_pages.Count();
}

int wxNotebook::GetRowCount() const
{
  return TabCtrl_GetRowCount(m_hwnd);
}

int wxNotebook::SetSelection(int nPage)
{
  wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

  if ( nPage != m_nSelection )
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

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook, without deleting
wxNotebookPage *wxNotebook::DoRemovePage(int nPage)
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
        if ( m_nSelection == GetPageCount() )
        {
            // last page deleted, make the new last page the new selection
            selNew = m_nSelection - 1;
        }
        else if ( nPage <= m_nSelection )
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
  int nPageCount = GetPageCount();
  int nPage;
  for ( nPage = 0; nPage < nPageCount; nPage++ )
    delete m_pages[nPage];

  m_pages.Clear();

  TabCtrl_DeleteAllItems(m_hwnd);

  m_nSelection = -1;

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

#if wxUSE_UXTHEME && wxUSE_UXTHEME_AUTO
    // Automatically apply the theme background,
    // changing the colour of the panel to match the
    // tab page colour. This won't work well with all
    // themes but it's a start.
    if (wxUxThemeEngine::Get() && pPage->IsKindOf(CLASSINFO(wxPanel)))
    {
        wxNotebookApplyThemeBackground(this, pPage, wxNotebookGetThemeBackgroundColour(this));
    }
#endif

    // add a new tab to the control
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
  m_pages.Insert(pPage, nPage);

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
        if ( event.GetEventObject() == parent )
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
wxColour wxNotebookGetThemeBackgroundColour(wxNotebook* notebook)
{
#if wxUSE_UXTHEME
    if (wxUxThemeEngine::Get())
    {
        WXHTHEME hTheme = wxUxThemeEngine::Get()->m_pfnOpenThemeData(notebook->GetHWND(), L"TAB");
        if (hTheme)
        {
            // This is total guesswork.
            // See PlatformSDK\Include\Tmschema.h for values
            COLORREF themeColor;
            wxUxThemeEngine::Get()->
                m_pfnGetThemeColor(hTheme,
                10 /* TABP_BODY */,
                1 /* NORMAL */,
                3821, /* FILLCOLORHINT */
                & themeColor);
            
            wxUxThemeEngine::Get()->m_pfnCloseThemeData(hTheme);
            
            wxColour colour(GetRValue(themeColor), GetGValue(themeColor), GetBValue(themeColor));
            return colour;
        }
    }
#endif
    return notebook->GetBackgroundColour();
}

// Windows only: attempts to apply the UX theme page background to this page
void wxNotebookApplyThemeBackground(wxNotebook* notebook, wxWindow* window, const wxColour& colour)
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
             (window->IsKindOf(CLASSINFO(wxNotebook)) && (window != notebook)) ||
             window->IsKindOf(CLASSINFO(wxScrolledWindow))
        )
    {
        window->SetBackgroundColour(colour);
    }

    for ( wxWindowList::Node *node = window->GetChildren().GetFirst(); node; node = node->GetNext() )
    {
        wxWindow *child = node->GetData();
        wxNotebookApplyThemeBackground(notebook, child, colour);
    }
#endif
}

#endif // wxUSE_NOTEBOOK
