///////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
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

#include "wx/string.h"
#include "wx/log.h"
#include "wx/imaglist.h"
#include "wx/notebook.h"
#include "wx/mac/uma.h"
// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) (((nPage) >= 0) && ((nPage) < GetPageCount()))

static bool constantsSet = false ;

 short kwxMacTabLeftMargin = 0 ;
 short kwxMacTabTopMargin = 0 ;
 short kwxMacTabRightMargin = 0 ;
 short kwxMacTabBottomMargin = 0 ;

// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

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
    if ( !constantsSet )
    {
        if ( UMAHasAquaLayout() )
        {
      // I got these values for Mac OS X from the Appearance mgr docs. (Mark Newsam)
            kwxMacTabLeftMargin = 20 ;
            kwxMacTabTopMargin = 38 ;
            kwxMacTabRightMargin = 20 ;
            kwxMacTabBottomMargin = 12 ;
        }
        else
        {
            kwxMacTabLeftMargin = 16 ;
            kwxMacTabTopMargin = 30 ;
            kwxMacTabRightMargin = 16 ;
            kwxMacTabBottomMargin = 16 ;
        }
        constantsSet = true ;
    }
    if ( UMAHasAquaLayout() )
    {
        m_macHorizontalBorder = 7;
        m_macVerticalBorder = 8;
    }

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
	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  "" , pos , size ,style, wxDefaultValidator , name , &bounds , title ) ;

	m_macControl = ::NewControl( parent->MacGetRootWindow() , &bounds , title , false , 0 , 0 , 1, 
	  	kControlTabSmallProc , (long) this ) ;
	
	MacPostControlCreate() ;
	return TRUE ;
}

// dtor
wxNotebook::~wxNotebook()
{
	m_macControl = NULL ;
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------

void wxNotebook::SetPadding(const wxSize& padding)
{
   wxFAIL_MSG( wxT("wxNotebook::SetPadding not implemented") );
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
    wxFAIL_MSG( wxT("wxNotebook::SetTabSize not implemented") );
}

void wxNotebook::SetPageSize(const wxSize& size)
{
    wxFAIL_MSG( wxT("wxNotebook::SetPageSize not implemented") );
}

int wxNotebook::SetSelection(int nPage)
{
  if( !IS_VALID_PAGE(nPage) ) 
    return m_nSelection ;

    ChangePage(m_nSelection, nPage);
	SetControlValue( m_macControl , m_nSelection + 1 ) ;

    return m_nSelection;
}

bool wxNotebook::SetPageText(int nPage, const wxString& strText)
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage *page = m_pages[nPage];
    page->SetLabel(strText);
    MacSetupTabs();

    return true;
}

wxString wxNotebook::GetPageText(int nPage) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage *page = m_pages[nPage];
    return page->GetLabel();
}

int wxNotebook::GetPageImage(int nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), -1, _T("invalid notebook page") );

    return 0 ;
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, _T("invalid notebook page") );

    wxCHECK_MSG( m_imageList && nImage < m_imageList->GetImageCount(), FALSE,
                 _T("invalid image index in SetPageImage()") );

    return FALSE;
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook, without deleting the window
wxNotebookPage* wxNotebook::DoRemovePage(int nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), NULL );
    wxNotebookPage* page = m_pages[nPage] ;
    m_pages.RemoveAt(nPage);

    MacSetupTabs();

    if(m_nSelection >= GetPageCount()) {
        m_nSelection = GetPageCount() - 1;
    }
    if(m_nSelection >= 0) {
        m_pages[m_nSelection]->Show(true);
    }
    return page;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    // TODO: delete native widget pages

    WX_CLEAR_ARRAY(m_pages) ;
    MacSetupTabs();

    return TRUE;
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

    pPage->SetLabel(strText);

    // save the pointer to the page
    m_pages.Insert(pPage, nPage);

    MacSetupTabs();

    if ( bSelect ) {
        m_nSelection = nPage;
    }
    else if ( m_nSelection == -1 ) {
        m_nSelection = 0;
    }
    else if (m_nSelection >= nPage) {
        m_nSelection++;
    }
    // don't show pages by default (we'll need to adjust their size first)
    pPage->Show( false ) ;

    int h, w;
    GetSize(&w, &h);
    pPage->SetSize(kwxMacTabLeftMargin, kwxMacTabTopMargin,
                   w - kwxMacTabLeftMargin - kwxMacTabRightMargin,
                   h - kwxMacTabTopMargin - kwxMacTabBottomMargin );
    if ( pPage->GetAutoLayout() ) {
        pPage->Layout();
    }

    return true;
}

/* Added by Mark Newsam
 * When a page is added or deleted to the notebook this function updates
 * information held in the m_macControl so that it matches the order
 * the user would expect.
 */
void wxNotebook::MacSetupTabs()
{
    SetControlMaximum( m_macControl , GetPageCount() ) ;

    wxNotebookPage *page;
    ControlTabInfoRec info;
    Boolean enabled = true;
    for(int ii = 0; ii < GetPageCount(); ii++)
    {
        page = m_pages[ii];
        info.version = 0;
        info.iconSuiteID = 0;
#if TARGET_CARBON
		c2pstrcpy( (StringPtr) info.name , page->GetLabel() ) ;
#else
		strcpy( (char *) info.name , page->GetLabel() ) ;
		c2pstr( (char *) info.name ) ;
#endif
        SetControlData( m_macControl, ii+1, kControlTabInfoTag,
                        sizeof( ControlTabInfoRec) , (char*) &info ) ;
        SetControlData( m_macControl, ii+1, kControlTabEnabledFlagTag,
                        sizeof( Boolean ), (Ptr)&enabled );
    }
    Rect bounds;
    GetControlBounds(m_macControl, &bounds);
    InvalWindowRect(MacGetRootWindow(), &bounds);
}

// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

// @@@ OnSize() is used for setting the font when it's called for the first
//     time because doing it in ::Create() doesn't work (for unknown reasons)
void wxNotebook::OnSize(wxSizeEvent& event)
{
    // emulate page change (it's esp. important to do it first time because
    // otherwise our page would stay invisible)
    int nSel = m_nSelection;
    m_nSelection = -1;
    SetSelection(nSel);

    // fit the notebook page to the tab control's display area
    int w, h;
    GetSize(&w, &h);

    unsigned int nCount = m_pages.Count();
    for ( unsigned int nPage = 0; nPage < nCount; nPage++ ) {
        wxNotebookPage *pPage = m_pages[nPage];
        pPage->SetSize(kwxMacTabLeftMargin, kwxMacTabTopMargin,
                       w - kwxMacTabLeftMargin - kwxMacTabRightMargin,
                       h - kwxMacTabTopMargin - kwxMacTabBottomMargin );
        if ( pPage->GetAutoLayout() ) {
            pPage->Layout();
        }
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
    // it's not an error (the message may be generated by the tab control itself)
    // and it may happen - just do nothing
    if ( nSel == nOldSel )
    {
        wxNotebookPage *pPage = m_pages[nSel];
        pPage->Show(FALSE);
        pPage->Show(TRUE);
        pPage->SetFocus();
        return;
    }

    // Hide previous page
    if ( nOldSel != -1 ) {
        m_pages[nOldSel]->Show(FALSE);
    }

    wxNotebookPage *pPage = m_pages[nSel];
    pPage->Show(TRUE);
    pPage->SetFocus();

    m_nSelection = nSel;
}

void wxNotebook::MacHandleControlClick( ControlHandle control , SInt16 controlpart ) 
{
  wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, m_windowId , ::GetControlValue(m_macControl) - 1, m_nSelection);
  event.SetEventObject(this);

  ProcessEvent(event);
}

