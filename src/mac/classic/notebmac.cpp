///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/notebmac.cpp
// Purpose:     implementation of wxNotebook
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/notebook.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/image.h"
#endif

#include "wx/imaglist.h"
#include "wx/mac/uma.h"
// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// check that the page index is valid
#define IS_VALID_PAGE(nPage) ((nPage) < GetPageCount())


// ----------------------------------------------------------------------------
// event table
// ----------------------------------------------------------------------------

DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
DEFINE_EVENT_TYPE(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)

BEGIN_EVENT_TABLE(wxNotebook, wxControl)
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, wxNotebook::OnSelChange)
    EVT_MOUSE_EVENTS(wxNotebook::OnMouse)

    EVT_SIZE(wxNotebook::OnSize)
    EVT_SET_FOCUS(wxNotebook::OnSetFocus)
    EVT_NAVIGATION_KEY(wxNotebook::OnNavigationKey)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxNotebook, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxNotebookEvent, wxCommandEvent)

// ============================================================================
// implementation
// ============================================================================

// The Appearance Manager docs show using tab controls in either edge to edge
// mode, or inset.  I think edge to edge conforms better to the other ports,
// and inset mode is better accomplished with space around the wxNotebook rather
// than within it.    --Robin

// CS : had to switch off tight spacing due to 10.3 problems
#define wxMAC_EDGE_TO_EDGE 0

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
//            otherMargin = 20;
            // JACS - this seems fine on 10.3; 20 is way too much
            otherMargin = 8;
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
    // but we have to special case wxBK_TOP since it is zero...
    if ( side == wxBK_TOP)
    {
        if ( nbStyle != 0 && nbStyle & (wxBK_LEFT|wxBK_RIGHT|wxBK_BOTTOM))
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
    return wxMacTabMargin(style, wxBK_LEFT);
}

static inline int wxMacTabTopMargin(long style)
{
    return wxMacTabMargin(style, wxBK_TOP);
}

static inline int wxMacTabRightMargin(long style)
{
    return wxMacTabMargin(style, wxBK_RIGHT);
}

static inline int wxMacTabBottomMargin(long style)
{
    return wxMacTabMargin(style, wxBK_BOTTOM);
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
    if ( !wxNotebookBase::Create(parent, id, pos, size, style, name) )
        return false;

    Rect bounds ;
    Str255 title ;

    if ( (style & wxBK_ALIGN_MASK) == wxBK_DEFAULT )
        style |= wxBK_TOP;

    MacPreControlCreate( parent , id ,  wxEmptyString , pos , size ,style, wxDefaultValidator , name , &bounds , title ) ;

    int tabstyle = kControlTabSmallNorthProc ;
    if ( HasFlag(wxBK_LEFT) )
        tabstyle = kControlTabSmallWestProc ;
    else if ( HasFlag( wxBK_RIGHT ) )
        tabstyle = kControlTabSmallEastProc ;
    else if ( HasFlag( wxBK_BOTTOM ) )
        tabstyle = kControlTabSmallSouthProc ;


    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 1,
        tabstyle , (long) this ) ;

    MacPostControlCreate() ;
    return true ;
}

// dtor
wxNotebook::~wxNotebook()
{
}

// ----------------------------------------------------------------------------
// wxNotebook accessors
// ----------------------------------------------------------------------------

void wxNotebook::SetPadding(const wxSize& padding)
{
    // unsupported by OS
}

void wxNotebook::SetTabSize(const wxSize& sz)
{
    // unsupported by OS
}

void wxNotebook::SetPageSize(const wxSize& size)
{
    SetSize( CalcSizeFromPage( size ) );
}

wxSize wxNotebook::CalcSizeFromPage(const wxSize& sizePage) const
{
    wxSize sizeTotal = sizePage;
    sizeTotal.x += 2 * m_macHorizontalBorder + wxMacTabLeftMargin(GetWindowStyle()) +
        wxMacTabRightMargin(GetWindowStyle()) ;
    sizeTotal.y += 2 * m_macVerticalBorder + wxMacTabTopMargin(GetWindowStyle()) +
        wxMacTabBottomMargin(GetWindowStyle()) ;

    return sizeTotal;
}

wxSize wxNotebook::DoGetBestSize() const
{
    // calculate the max page size
    wxSize size(0, 0);

    size_t count = GetPageCount();
    if ( count )
    {
        for ( size_t n = 0; n < count; n++ )
        {
            wxSize sizePage = m_pages[n]->GetSize();

            if ( size.x < sizePage.x )
                size.x = sizePage.x;
            if ( size.y < sizePage.y )
                size.y = sizePage.y;
        }
    }
    else // no pages
    {
        // use some arbitrary default size
        size.x =
        size.y = 100;
    }

    return CalcSizeFromPage(size);
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

            ChangePage(m_nSelection, nPage);
        }
    }

    return m_nSelection;
}

bool wxNotebook::SetPageText(size_t nPage, const wxString& strText)
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage *page = m_pages[nPage];
    page->SetLabel(strText);
    MacSetupTabs();

    return true;
}

wxString wxNotebook::GetPageText(size_t nPage) const
{
    wxASSERT( IS_VALID_PAGE(nPage) );

    wxNotebookPage *page = m_pages[nPage];
    return page->GetLabel();
}

int wxNotebook::GetPageImage(size_t nPage) const
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), wxNOT_FOUND, _T("invalid notebook page") );

    return m_images[nPage];
}

bool wxNotebook::SetPageImage(size_t nPage, int nImage)
{
    wxCHECK_MSG( IS_VALID_PAGE(nPage), false, _T("invalid notebook page") );

    wxCHECK_MSG( m_imageList && nImage < m_imageList->GetImageCount(), false,
        _T("invalid image index in SetPageImage()") );

    if ( nImage != m_images[nPage] )
    {
        // if the item didn't have an icon before or, on the contrary, did have
        // it but has lost it now, its size will change - but if the icon just
        // changes, it won't
        m_images[nPage] = nImage;

        MacSetupTabs() ;
    }

    return true;
}

// ----------------------------------------------------------------------------
// wxNotebook operations
// ----------------------------------------------------------------------------

// remove one page from the notebook, without deleting the window
wxNotebookPage* wxNotebook::DoRemovePage(size_t nPage)
{
    wxCHECK( IS_VALID_PAGE(nPage), NULL );
    wxNotebookPage* page = m_pages[nPage] ;
    m_pages.RemoveAt(nPage);

    MacSetupTabs();

    if(m_nSelection >= (int)GetPageCount()) {
        m_nSelection = GetPageCount() - 1;
    }
    if(m_nSelection >= 0) {
        m_pages[m_nSelection]->Show(true);
    }
    InvalidateBestSize();
    return page;
}

// remove all pages
bool wxNotebook::DeleteAllPages()
{
    WX_CLEAR_ARRAY(m_pages) ;
    MacSetupTabs();
    m_nSelection = -1 ;
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
    if ( !wxNotebookBase::InsertPage(nPage, pPage, strText, bSelect, imageId) )
        return false;

    wxASSERT_MSG( pPage->GetParent() == this,
                    _T("notebook pages must have notebook as parent") );

    // don't show pages by default (we'll need to adjust their size first)
    pPage->Show( false ) ;

    pPage->SetLabel(strText);

    m_images.Insert(imageId, nPage);

    MacSetupTabs();

    wxRect rect = GetPageRect() ;
    pPage->SetSize(rect);
    if ( pPage->GetAutoLayout() ) {
        pPage->Layout();
    }


    // now deal with the selection
    // ---------------------------

    // if the inserted page is before the selected one, we must update the
    // index of the selected page

    if ( int(nPage) <= m_nSelection )
    {
        m_nSelection++;
        // while this still is the same page showing, we need to update the tabs
        SetControl32BitValue( (ControlHandle) m_macControl , m_nSelection + 1 ) ;
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

    const size_t countPages = GetPageCount();
    for(size_t ii = 0; ii < countPages; ii++)
    {
        page = m_pages[ii];
        info.version = 0;
        info.iconSuiteID = 0;
        wxMacStringToPascal( page->GetLabel() , info.name ) ;

        SetControlData( (ControlHandle) m_macControl, ii+1, kControlTabInfoTag,
            sizeof( ControlTabInfoRec) , (char*) &info ) ;
        SetTabEnabled( (ControlHandle) m_macControl , ii+1 , true ) ;
#if TARGET_CARBON
        if ( GetImageList() && GetPageImage(ii) >= 0 && UMAGetSystemVersion() >= 0x1020 )
        {
            // tab controls only support very specific types of images, therefore we are doing an odyssee
            // accross the icon worlds (even Apple DTS did not find a shorter path)
            // in order not to pollute the icon registry we put every icon into (OSType) 1 and immediately
            // afterwards Unregister it (IconRef is ref counted, so it will stay on the tab even if we
            // unregister it) in case this will ever lead to having the same icon everywhere add some kind
            // of static counter
            const wxBitmap* bmap = GetImageList()->GetBitmapPtr( GetPageImage(ii ) ) ;
            if ( bmap )
            {
                wxBitmap scaledBitmap ;
                if ( bmap->GetWidth() != 16 || bmap->GetHeight() != 16 )
                {
                    scaledBitmap = wxBitmap( bmap->ConvertToImage().Scale(16,16) ) ;
                    bmap = &scaledBitmap ;
                }
                ControlButtonContentInfo info ;
                wxMacCreateBitmapButton( &info , *bmap , kControlContentPictHandle) ;
                IconFamilyHandle iconFamily = (IconFamilyHandle) NewHandle(0) ;
                OSErr err = SetIconFamilyData( iconFamily, 'PICT' , (Handle) info.u.picture ) ;
                wxASSERT_MSG( err == noErr , wxT("Error when adding bitmap") ) ;
                IconRef iconRef ;
                err = RegisterIconRefFromIconFamily( 'WXNG' , (OSType) 1, iconFamily, &iconRef ) ;
                wxASSERT_MSG( err == noErr , wxT("Error when adding bitmap") ) ;
                info.contentType = kControlContentIconRef ;
                info.u.iconRef = iconRef ;
                SetControlData( (ControlHandle) m_macControl, ii+1,kControlTabImageContentTag,
                    sizeof( info ), (Ptr)&info );
                wxASSERT_MSG( err == noErr , wxT("Error when setting icon on tab") ) ;
                if ( UMAGetSystemVersion() < 0x1030 )
                {
                    UnregisterIconRef( 'WXNG' , (OSType) 1 ) ;
                }

                ReleaseIconRef( iconRef ) ;
                DisposeHandle( (Handle) iconFamily ) ;
            }
        }
#endif
    }
    Rect bounds;
    GetControlBounds((ControlHandle)m_macControl, &bounds);
    InvalWindowRect((WindowRef)MacGetRootWindow(), &bounds);
}

wxRect wxNotebook::GetPageRect() const
{
    // fit the notebook page to the tab control's display area
    int w, h;
    GetSize(&w, &h);

    return wxRect(
        wxMacTabLeftMargin(GetWindowStyle()) + m_macHorizontalBorder,
        wxMacTabTopMargin(GetWindowStyle()) + m_macVerticalBorder,
        w - wxMacTabLeftMargin(GetWindowStyle()) - wxMacTabRightMargin(GetWindowStyle()) - 2*m_macHorizontalBorder,
        h - wxMacTabTopMargin(GetWindowStyle()) - wxMacTabBottomMargin(GetWindowStyle()) - 2*m_macVerticalBorder);
}
// ----------------------------------------------------------------------------
// wxNotebook callbacks
// ----------------------------------------------------------------------------

// @@@ OnSize() is used for setting the font when it's called for the first
//     time because doing it in ::Create() doesn't work (for unknown reasons)
void wxNotebook::OnSize(wxSizeEvent& event)
{

    unsigned int nCount = m_pages.Count();
    wxRect rect = GetPageRect() ;
    for ( unsigned int nPage = 0; nPage < nCount; nPage++ ) {
        wxNotebookPage *pPage = m_pages[nPage];
        pPage->SetSize(rect);
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
    wxControl::SetConstraintSizes(false);
}

bool wxNotebook::DoPhase(int WXUNUSED(nPhase))
{
    return true;
}

#endif // wxUSE_CONSTRAINTS

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
    if ( nOldSel != -1 )
    {
        m_pages[nOldSel]->Show(false);
    }

    if ( nSel != -1 )
    {
        wxNotebookPage *pPage = m_pages[nSel];
        pPage->Show(true);
        pPage->SetFocus();
    }

    m_nSelection = nSel;
    SetControl32BitValue( (ControlHandle) m_macControl , m_nSelection + 1 ) ;
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


void wxNotebook::MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool WXUNUSED( mouseStillDown ) )
{
#if 0
    wxNotebookEvent event(wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, m_windowId , ::GetControl32BitValue((ControlHandle)m_macControl) - 1, m_nSelection);
    event.SetEventObject(this);

    ProcessEvent(event);
#endif
}
