///////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
#pragma implementation "notebook.h"
#endif

#include  <wx/string.h>
#include  <wx/log.h>
#include  <wx/imaglist.h>
#include  <wx/notebook.h>
#include  <wx/dcclient.h>

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) (((nPage) >= 0) && ((nPage) < GetPageCount()))

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(-1, wxNotebook::OnSelChange)
    EVT_SIZE(wxNotebook::OnSize)
    EVT_PAINT(wxNotebook::OnPaint)
    EVT_MOUSE_EVENTS(wxNotebook::OnMouseEvent)
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
    m_tabView = (wxNotebookTabView*) NULL;
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

    // style
    m_windowStyle = style;

    if ( parent != NULL )
        parent->AddChild(this);

    // It's like a normal window...
    if (!wxWindow::Create(parent, id, pos, size, style, name))
        return FALSE;

    SetTabView(new wxNotebookTabView(this));

    return TRUE;
}

// dtor
wxNotebook::~wxNotebook()
{
    delete m_tabView;
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
    wxASSERT( IS_VALID_PAGE(nPage) );

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
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return FALSE;
}

wxString wxNotebook::GetPageText(int nPage) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return wxString("");
}

int wxNotebook::GetPageImage(int nPage) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return 0;
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    // TODO
    return FALSE;
}

void wxNotebook::SetImageList(wxImageList* imageList)
{ 
    m_pImageList = imageList;
    // TODO
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook
bool wxNotebook::DeletePage(int nPage)
{
    wxFAIL_MSG("Sorry, DeletePage not implemented for Motif wxNotebook because wxTabView doesn't support it.");
    return FALSE;

/*
    wxCHECK( IS_VALID_PAGE(nPage), FALSE );

    // TODO: delete native widget page

    delete m_aPages[nPage];
    m_aPages.Remove(nPage);

    return TRUE;
*/
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    m_tabView->ClearTabs(TRUE);

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

    m_tabView->AddTab(nPage, strText);

/*
    if (bSelect)
        m_tabView->SetTabSelection(nPage, TRUE);
*/

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

    if (m_tabView)
    {
        int cw, ch;
        GetClientSize(& cw, & ch);

        int tabHeight = m_tabView->GetTotalTabHeight();
        wxRect rect;
        rect.x = 4;
        rect.y = tabHeight + 4;
        rect.width = cw - 8;
        rect.height = ch - 4 - rect.y ;
  
        m_tabView->SetViewRect(rect);

        m_tabView->Layout();

        // Need to do it a 2nd time to get the tab height with
        // the new view width, since changing the view width changes the
        // tab layout.
        tabHeight = m_tabView->GetTotalTabHeight();
        rect.x = 4;
        rect.y = tabHeight + 4;
        rect.width = cw - 8;
        rect.height = ch - 4 - rect.y ;
  
        m_tabView->SetViewRect(rect);

        m_tabView->Layout();

        // emulate page change (it's esp. important to do it first time because
        // otherwise our page would stay invisible)
        int nSel = m_nSelection;
        m_nSelection = -1;
        SetSelection(nSel);

        // fit the notebook page to the tab control's display area

        unsigned int nCount = m_aPages.Count();
        for ( unsigned int nPage = 0; nPage < nCount; nPage++ ) {
            wxNotebookPage *pPage = m_aPages[nPage];
            pPage->SetSize(rect.x + 2, rect.y + 2, rect.width - 2, rect.height - 2);
            if ( pPage->GetAutoLayout() )
                pPage->Layout();
        }
        Refresh();
    }

    // Processing continues to next OnSize
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

void wxNotebook::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxNotebook::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();
}

void wxNotebook::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();
}

void wxNotebook::OnMouseEvent(wxMouseEvent& event)
{
  if (m_tabView)
    m_tabView->OnEvent(event);
}

void wxNotebook::OnPaint(wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    if (m_tabView)
        m_tabView->Draw(dc);
}

/*
 * wxNotebookTabView
 */
 
IMPLEMENT_CLASS(wxNotebookTabView, wxTabView)

wxNotebookTabView::wxNotebookTabView(wxNotebook *notebook, long style): wxTabView(style)
{
  m_notebook = notebook;

//  m_currentWindow = (wxWindow *) NULL;

  m_notebook->SetTabView(this);

  SetWindow(m_notebook);
}

wxNotebookTabView::~wxNotebookTabView(void)
{
//  ClearWindows(TRUE);
}

// Called when a tab is activated
void wxNotebookTabView::OnTabActivate(int activateId, int deactivateId)
{
  if (!m_notebook)
    return;
    
  wxWindow *oldWindow = ((deactivateId == -1) ? 0 : m_notebook->GetPage(deactivateId));
  wxWindow *newWindow = m_notebook->GetPage(activateId);

  if (oldWindow)
    oldWindow->Show(FALSE);
  if (newWindow)
    newWindow->Show(TRUE);
    
  m_notebook->Refresh();
}

#if 0
void wxNotebookTabView::AddTabWindow(int id, wxWindow *window)
{
  m_tabWindows.Append((long)id, window);
  window->Show(FALSE);
}

wxWindow *wxNotebookTabView::GetTabWindow(int id) const
{
  wxNode *node = m_tabWindows.Find((long)id);
  if (!node)
    return (wxWindow *) NULL;
  return (wxWindow *)node->Data();    
}

void wxNotebookTabView::ClearWindows(bool deleteWindows)
{
  if (deleteWindows)
    m_tabWindows.DeleteContents(TRUE);
  m_tabWindows.Clear();
  m_tabWindows.DeleteContents(FALSE);
}

void wxNotebookTabView::ShowWindowForTab(int id)
{
  wxWindow *newWindow = GetTabWindow(id);
  if (newWindow == m_currentWindow)
    return;
  if (m_currentWindow)
    m_currentWindow->Show(FALSE);
  newWindow->Show(TRUE);
  newWindow->Refresh();
}
#endif

