///////////////////////////////////////////////////////////////////////////////
// Name:        notebook.cpp
// Purpose:     implementation of wxNotebook
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "notebook.h"
#endif

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#include "wx/app.h"
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


// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

#if !USE_SHARED_LIBRARIES
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxNotebook, wxControl)
EVT_NOTEBOOK_PAGE_CHANGED(-1, wxNotebook::OnSelChange)
EVT_MOUSE_EVENTS(wxNotebook::OnMouse)

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

// The Appearance Manager docs show using tab controls in either edge to edge
// mode, or inset.  I think edge to edge conforms better to the other ports,
// and inset mode is better accomplished with space around the wxNotebook rather
// than within it.    --Robin
#define wxMAC_EDGE_TO_EDGE 1

static inline int wxMacTabMargin(long nbStyle, long side)
{
    static int tabMargin = -1;
    static int otherMargin = -1;

    if ( tabMargin == -1)
    {
        if ( UMAHasAquaLayout() )
        {
            tabMargin = 26;    // From Appearance Manager docs for small tab control dimensions
#if wxMAC_EDGE_TO_EDGE
            otherMargin = 0;
#else
            otherMargin = 20;
#endif
        }
        else
        {
            tabMargin = 30;
#if wxMAC_EDGE_TO_EDGE
            otherMargin = 0;
#else
            otherMargin = 16;
#endif
        }
    }

    // If the style matches the side asked for then return the tab margin,
    // but we have to special case wxNB_TOP since it is zero...
    if ( side == wxNB_TOP)
    {
        if ( nbStyle != 0 && nbStyle & wxNB_LEFT|wxNB_RIGHT|wxNB_BOTTOM)
        {
            return otherMargin;
        }
        else
        {
            return tabMargin;
        }
    }
    else if ( nbStyle & side)
        return tabMargin;
    else
        return otherMargin;
}

static inline int wxMacTabLeftMargin(long style)
{
    return wxMacTabMargin(style, wxNB_LEFT);
}

static inline int wxMacTabTopMargin(long style)
{
    return wxMacTabMargin(style, wxNB_TOP);
}

static inline int wxMacTabRightMargin(long style)
{
    return wxMacTabMargin(style, wxNB_RIGHT);
}

static inline int wxMacTabBottomMargin(long style)
{
    return wxMacTabMargin(style, wxNB_BOTTOM);
}

// ----------------------------------------------------------------------------
// wxNotebook construction
// ----------------------------------------------------------------------------

// common part of all ctors
void wxNotebook::Init()
{
    if ( UMAHasAquaLayout() )
    {
        // Should these depend on wxMAC_EDGE_TO_EDGE too?
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

    MacPreControlCreate( parent , id ,  wxEmptyString , pos , size ,style, wxDefaultValidator , name , &bounds , title ) ;

    int tabstyle = kControlTabSmallNorthProc ;
    if ( HasFlag(wxNB_LEFT) )
        tabstyle = kControlTabSmallWestProc ;
    else if ( HasFlag( wxNB_RIGHT ) )
        tabstyle = kControlTabSmallEastProc ;
    else if ( HasFlag( wxNB_BOTTOM ) )
        tabstyle = kControlTabSmallSouthProc ;


    m_macControl = ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 1,
        tabstyle , (long) this ) ;

    MacPostControlCreate() ;
    return TRUE ;
}

// dtor
wxNotebook::~wxNotebook()
{
    m_macControl = NULL ;
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage)
{
    wxSize sizeTotal = sizePage;

    int major,minor;
    wxGetOsVersion( &major, &minor );

    // Mac has large notebook borders. Aqua even more so.

    if ( HasFlag(wxNB_LEFT) || HasFlag(wxNB_RIGHT) )
    {
        sizeTotal.x += 90;

        if (major >= 10)
            sizeTotal.y += 28;
        else
            sizeTotal.y += 20;
    }
    else
    {
        if (major >= 10)
        {
            sizeTotal.x += 34;
            sizeTotal.y += 46;
        }
        else
        {
            sizeTotal.x += 22;
            sizeTotal.y += 44;
        }
    }

    return sizeTotal;
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
    SetControl32BitValue( (ControlHandle) m_macControl , m_nSelection + 1 ) ;

    Refresh();
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

    return m_images[nPage];
}

bool wxNotebook::SetPageImage(int nPage, int nImage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), FALSE, _T("invalid notebook page") );

    wxCHECK_MSG( m_imageList && nImage < m_imageList->GetImageCount(), FALSE,
        _T("invalid image index in SetPageImage()") );

    if ( nImage != m_images[nPage] )
    {
        // if the item didn't have an icon before or, on the contrary, did have
        // it but has lost it now, its size will change - but if the icon just
        // changes, it won't
        m_images[nPage] = nImage;

        MacSetupTabs() ;
    }

    return TRUE;
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

    m_images.Insert(imageId, nPage);

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
    pPage->SetSize(wxMacTabLeftMargin(GetWindowStyle()) + m_macHorizontalBorder,
                   wxMacTabTopMargin(GetWindowStyle()) + m_macVerticalBorder,
                   w - wxMacTabLeftMargin(GetWindowStyle()) - wxMacTabRightMargin(GetWindowStyle()) - 2*m_macHorizontalBorder,
                   h - wxMacTabTopMargin(GetWindowStyle()) - wxMacTabBottomMargin(GetWindowStyle()) - 2*m_macVerticalBorder);
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
    SetControl32BitMaximum( (ControlHandle) m_macControl , GetPageCount() ) ;

    wxNotebookPage *page;
    ControlTabInfoRec info;

    for(int ii = 0; ii < GetPageCount(); ii++)
    {
        page = m_pages[ii];
        info.version = 0;
        info.iconSuiteID = 0;
        wxMacStringToPascal( page->GetLabel() , info.name ) ;

        SetControlData( (ControlHandle) m_macControl, ii+1, kControlTabInfoTag,
            sizeof( ControlTabInfoRec) , (char*) &info ) ;
        SetTabEnabled( (ControlHandle) m_macControl , ii+1 , true ) ;
#if 0 // TARGET_CARBON
        if ( GetImageList() && GetPageImage(ii) >= 0 && UMAGetSystemVersion() >= 0x1020 )
        {
            // tab controls only support very specific types of images, therefore we are doing an odyssee
            // accross the icon worlds (even Apple DTS did not find a shorter path)
            // in order not to pollute the icon registry we put every icon into (OSType) 1 and immediately
            // afterwards Unregister it (IconRef is ref counted, so it will stay on the tab even if we
            // unregister it) in case this will ever lead to having the same icon everywhere add some kind
            // of static counter
            ControlButtonContentInfo info ;
            wxMacCreateBitmapButton( &info , *GetImageList()->GetBitmap( GetPageImage(ii ) ) , kControlContentPictHandle) ;
            IconFamilyHandle iconFamily = (IconFamilyHandle) NewHandle(0) ;
            OSErr err = SetIconFamilyData( iconFamily, 'PICT' , (Handle) info.u.picture ) ;
            wxASSERT_MSG( err == noErr , "Error when adding bitmap" ) ;
            IconRef iconRef ;
            err = RegisterIconRefFromIconFamily( 'WXNG' , (OSType) 1 , iconFamily, &iconRef ) ;
            wxASSERT_MSG( err == noErr , "Error when adding bitmap" ) ;
            info.contentType = kControlContentIconRef ;
            info.u.iconRef = iconRef ;
            SetControlData( (ControlHandle) m_macControl, ii+1,kControlTabImageContentTag,
                sizeof( info ), (Ptr)&info );
            wxASSERT_MSG( err == noErr , "Error when setting icon on tab" ) ;
               UnregisterIconRef( 'WXNG' , (OSType) 1 ) ;
            ReleaseIconRef( iconRef ) ;
            DisposeHandle( (Handle) iconFamily ) ;
        }
#endif
    }
    Rect bounds;
    GetControlBounds((ControlHandle)m_macControl, &bounds);
    InvalWindowRect((WindowRef)MacGetRootWindow(), &bounds);
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
        pPage->SetSize(wxMacTabLeftMargin(GetWindowStyle()) + m_macHorizontalBorder,
                       wxMacTabTopMargin(GetWindowStyle()) + m_macVerticalBorder,
                       w - wxMacTabLeftMargin(GetWindowStyle()) - wxMacTabRightMargin(GetWindowStyle()) - 2*m_macHorizontalBorder,
                       h - wxMacTabTopMargin(GetWindowStyle()) - wxMacTabBottomMargin(GetWindowStyle()) - 2*m_macVerticalBorder);
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
    wxFAIL_MSG(wxT("wxNotebook::Command not implemented"));
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


void  wxNotebook::OnMouse( wxMouseEvent &event )
{
    if ( (ControlHandle) m_macControl == NULL )
    {
        event.Skip() ;
        return ;
    }

    if (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK )
    {
        int x = event.m_x ;
        int y = event.m_y ;

        MacClientToRootWindow( &x , &y ) ;

        ControlHandle   control ;
        Point       localwhere ;
        SInt16      controlpart ;

        localwhere.h = x ;
        localwhere.v = y ;

        short modifiers = 0;

        if ( !event.m_leftDown && !event.m_rightDown )
            modifiers  |= btnState ;

        if ( event.m_shiftDown )
            modifiers |= shiftKey ;

        if ( event.m_controlDown )
            modifiers |= controlKey ;

        if ( event.m_altDown )
            modifiers |= optionKey ;

        if ( event.m_metaDown )
            modifiers |= cmdKey ;

        control = (ControlHandle) m_macControl ;
        if ( control && ::IsControlActive( control ) )
        {
            {
                wxNotebookEvent changing(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING, m_windowId,
                    ::GetControl32BitValue(control) - 1, m_nSelection);
                changing.SetEventObject(this);
                GetEventHandler()->ProcessEvent(changing);

                if(changing.IsAllowed())
                {
                    controlpart = ::HandleControlClick(control, localwhere, modifiers,
                        (ControlActionUPP) -1);
                    wxTheApp->s_lastMouseDown = 0 ;

                    wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, m_windowId,
                        ::GetControl32BitValue(control) - 1, m_nSelection);
                    event.SetEventObject(this);

                    GetEventHandler()->ProcessEvent(event);
                }
            }
        }
    }
}


void wxNotebook::MacHandleControlClick( WXWidget control , wxInt16 controlpart )
{
#if 0
    wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, m_windowId , ::GetControl32BitValue((ControlHandle)m_macControl) - 1, m_nSelection);
    event.SetEventObject(this);

    ProcessEvent(event);
#endif
}

