///////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      David Webster
// Modified by:
// Created:     10/12/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

// wxWindows
#ifndef WX_PRECOMP
  #include  <wx/string.h>
#endif  // WX_PRECOMP

#include  <wx/log.h>
#include  <wx/imaglist.h>
#include  <wx/event.h>
#include  <wx/control.h>
#include  <wx/notebook.h>

#include  <wx/os2/private.h>


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
// TODO:  m_backgroundColour = wxColour(GetSysColor(COLOR_BTNFACE));
  m_foregroundColour = *wxBLACK ;

    // TODO:
/*
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
*/
  SubclassWin(m_hWnd);

    return FALSE;
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
    return m_aPages.Count();
}

int wxNotebook::GetRowCount() const
{
    // TODO
    return 0;
}

int wxNotebook::SetSelection(int nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

    ChangePage(m_nSelection, nPage);

    // TODO
    return 0;
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

    // TODO
    return FALSE;
}

wxString wxNotebook::GetPageText(int nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), wxT(""), wxT("notebook page out of range") );

    // TODO
    return wxString("");
}

int wxNotebook::GetPageImage(int nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, wxT("notebook page out of range") );

    // TODO
    return 0;
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

    // TODO
    return FALSE;
}

void wxNotebook::SetImageList(wxImageList* imageList)
{
    m_pImageList = imageList;
    // TODO
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
    // TODO
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook
bool wxNotebook::DeletePage(int nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

    // TODO: delete native widget page

    delete m_aPages[nPage];
    m_aPages.Remove(nPage);

    return TRUE;
}

// remove one page from the notebook, without deleting the window
bool wxNotebook::RemovePage(int nPage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, wxT("notebook page out of range") );

    m_aPages.Remove(nPage);

    return TRUE;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    // TODO: delete native widget pages

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

    // TODO: insert native widget page

    // save the pointer to the page
    m_aPages.Insert(pPage, nPage);

    // some page must be selected: either this one or the first one if there is
    // still no selection
    if ( bSelect )
        m_nSelection = nPage;
    else if ( m_nSelection == -1 )
        m_nSelection = 0;

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
        // TODO: any first-time-size processing.
        s_bFirstTime = FALSE;
    }

    // TODO: all this may or may not be necessary for your platform

    // emulate page change (it's esp. important to do it first time because
    // otherwise our page would stay invisible)
    int nSel = m_nSelection;
    m_nSelection = -1;
    SetSelection(nSel);

    // fit the notebook page to the tab control's display area
    int w, h;
    GetSize(&w, &h);

    unsigned int nCount = m_aPages.Count();
    for ( unsigned int nPage = 0; nPage < nCount; nPage++ ) {
        wxNotebookPage *pPage = m_aPages[nPage];
        pPage->SetSize(0, 0, w, h);
#if wxUSE_CONSTRAINTS
        if ( pPage->GetAutoLayout() )
            pPage->Layout();
#endif //wxUSE_CONSTRAINTS

    }

    // Processing continues to next OnSize
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
#if wxUSE_CONSTRAINTS
    // don't set the sizes of the pages - their correct size is not yet known
    wxControl::SetConstraintSizes(FALSE);
#endif
}

bool wxNotebook::DoPhase(int /* nPhase */)
{
    return TRUE;
}

bool wxNotebook::OS2OnNotify(int idCtrl, WXLPARAM lParam, WXLPARAM* result)
{
  wxNotebookEvent event(wxEVT_NULL, m_windowId);
//TODO:
/*
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
*/
// TODO:    event.SetSelection(TabCtrl_GetCurSel(m_hwnd));
    event.SetOldSelection(m_nSelection);
    event.SetEventObject(this);
    event.SetInt(idCtrl);

    bool processed = GetEventHandler()->ProcessEvent(event);
// TODO:   *result = !event.IsAllowed();
    return processed;
}

// ----------------------------------------------------------------------------
// wxNotebook helper functions
// ----------------------------------------------------------------------------

// hide the currently active panel and show the new one
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

