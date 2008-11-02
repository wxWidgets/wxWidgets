/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/manager.cpp
// Purpose:     wxPropertyGridManager
// Author:      Jaakko Salli
// Modified by:
// Created:     2005-01-14
// RCS-ID:      $Id$
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/defs.h"
    #include "wx/object.h"
    #include "wx/hash.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/event.h"
    #include "wx/window.h"
    #include "wx/panel.h"
    #include "wx/dc.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"
    #include "wx/sizer.h"
    #include "wx/statusbr.h"
    #include "wx/intl.h"
#endif

// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include "wx/propgrid/propgrid.h"

#include "wx/propgrid/manager.h"


#define wxPG_MAN_ALTERNATE_BASE_ID          11249 // Needed for wxID_ANY madnesss


// -----------------------------------------------------------------------

// For wxMSW cursor consistency, we must do mouse capturing even
// when using custom controls

#define BEGIN_MOUSE_CAPTURE \
    if ( !(m_iFlags & wxPG_FL_MOUSE_CAPTURED) ) \
    { \
        CaptureMouse(); \
        m_iFlags |= wxPG_FL_MOUSE_CAPTURED; \
    }

#define END_MOUSE_CAPTURE \
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED ) \
    { \
        ReleaseMouse(); \
        m_iFlags &= ~(wxPG_FL_MOUSE_CAPTURED); \
    }

// -----------------------------------------------------------------------
// wxPropertyGridManager
// -----------------------------------------------------------------------

const char wxPropertyGridManagerNameStr[] = "wxPropertyGridManager";


// Categoric Mode Icon
static const char* gs_xpm_catmode[] = {
"16 16 5 1",
". c none",
"B c black",
"D c #868686",
"L c #CACACA",
"W c #FFFFFF",
".DDD............",
".DLD.BBBBBB.....",
".DDD............",
".....DDDDD.DDD..",
"................",
".....DDDDD.DDD..",
"................",
".....DDDDD.DDD..",
"................",
".....DDDDD.DDD..",
"................",
".DDD............",
".DLD.BBBBBB.....",
".DDD............",
".....DDDDD.DDD..",
"................"
};

// Alphabetic Mode Icon
static const char* gs_xpm_noncatmode[] = {
"16 16 5 1",
". c none",
"B c black",
"D c #868686",
"L c #000080",
"W c #FFFFFF",
"..DBD...DDD.DDD.",
".DB.BD..........",
".BBBBB..DDD.DDD.",
".B...B..........",
"...L....DDD.DDD.",
"...L............",
".L.L.L..DDD.DDD.",
"..LLL...........",
"...L....DDD.DDD.",
"................",
".BBBBB..DDD.DDD.",
"....BD..........",
"...BD...DDD.DDD.",
"..BD............",
".BBBBB..DDD.DDD.",
"................"
};

// Default Page Icon.
static const char* gs_xpm_defpage[] = {
"16 16 5 1",
". c none",
"B c black",
"D c #868686",
"L c #000080",
"W c #FFFFFF",
"................",
"................",
"..BBBBBBBBBBBB..",
"..B..........B..",
"..B.BB.LLLLL.B..",
"..B..........B..",
"..B.BB.LLLLL.B..",
"..B..........B..",
"..B.BB.LLLLL.B..",
"..B..........B..",
"..B.BB.LLLLL.B..",
"..B..........B..",
"..BBBBBBBBBBBB..",
"................",
"................",
"................"
};

// -----------------------------------------------------------------------
// wxPropertyGridPage
// -----------------------------------------------------------------------


IMPLEMENT_CLASS(wxPropertyGridPage, wxEvtHandler)


BEGIN_EVENT_TABLE(wxPropertyGridPage, wxEvtHandler)
END_EVENT_TABLE()


wxPropertyGridPage::wxPropertyGridPage()
    : wxEvtHandler(), wxPropertyGridInterface(), wxPropertyGridPageState()
{
    m_pState = this; // wxPropertyGridInterface to point to State
    m_manager = NULL;
    m_isDefault = false;
}

wxPropertyGridPage::~wxPropertyGridPage()
{
}

void wxPropertyGridPage::Clear()
{
    GetStatePtr()->DoClear();
}

wxSize wxPropertyGridPage::FitColumns()
{
    wxSize sz = DoFitColumns();
    return sz;
}

void wxPropertyGridPage::RefreshProperty( wxPGProperty* p )
{
    if ( m_manager )
        m_manager->RefreshProperty(p);
}

void wxPropertyGridPage::OnShow()
{
}

void wxPropertyGridPage::SetSplitterPosition( int splitterPos, int col )
{
    wxPropertyGrid* pg = GetGrid();
    if ( pg->GetState() == this )
        pg->SetSplitterPosition(splitterPos);
    else
        DoSetSplitterPosition(splitterPos, col, false);
}

void wxPropertyGridPage::DoSetSplitterPosition( int pos, int splitterColumn, bool allPages )
{
    if ( allPages && m_manager->GetPageCount() )
        m_manager->SetSplitterPosition( pos, splitterColumn );
    else
        DoSetSplitterPositionThisPage( pos, splitterColumn );
}

// -----------------------------------------------------------------------
// wxPropertyGridManager
// -----------------------------------------------------------------------

// Final default splitter y is client height minus this.
#define wxPGMAN_DEFAULT_NEGATIVE_SPLITTER_Y         100

// -----------------------------------------------------------------------

IMPLEMENT_CLASS(wxPropertyGridManager, wxPanel)

#define ID_ADVTOOLBAR_OFFSET        1
#define ID_ADVHELPCAPTION_OFFSET    2
#define ID_ADVHELPCONTENT_OFFSET    3
#define ID_ADVBUTTON_OFFSET         4
#define ID_ADVTBITEMSBASE_OFFSET    5   // Must be last.

// -----------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxPropertyGridManager, wxPanel)
  EVT_MOTION(wxPropertyGridManager::OnMouseMove)
  EVT_SIZE(wxPropertyGridManager::OnResize)
  EVT_PAINT(wxPropertyGridManager::OnPaint)
  EVT_LEFT_DOWN(wxPropertyGridManager::OnMouseClick)
  EVT_LEFT_UP(wxPropertyGridManager::OnMouseUp)
  EVT_LEAVE_WINDOW(wxPropertyGridManager::OnMouseEntry)
  //EVT_ENTER_WINDOW(wxPropertyGridManager::OnMouseEntry)
END_EVENT_TABLE()

// -----------------------------------------------------------------------

wxPropertyGridManager::wxPropertyGridManager()
    : wxPanel()
{
    Init1();
}

// -----------------------------------------------------------------------

wxPropertyGridManager::wxPropertyGridManager( wxWindow *parent,
                                              wxWindowID id,
                                              const wxPoint& pos,
                                              const wxSize& size,
                                              long style,
                                              const wxString& name )
    : wxPanel()
{
    Init1();
    Create(parent,id,pos,size,style,name);
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::Create( wxWindow *parent,
                                    wxWindowID id,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    const wxString& name )
{

    bool res = wxPanel::Create( parent, id, pos, size,
                                (style&0xFFFF0000)|wxWANTS_CHARS,
                                name );
    Init2(style);

    return res;
}

// -----------------------------------------------------------------------

//
// Initialize values to defaults
//
void wxPropertyGridManager::Init1()
{

    //m_pPropGrid = (wxPropertyGrid*) NULL;
    m_pPropGrid = CreatePropertyGrid();

#if wxUSE_TOOLBAR
    m_pToolbar = (wxToolBar*) NULL;
#endif
    m_pTxtHelpCaption = (wxStaticText*) NULL;
    m_pTxtHelpContent = (wxStaticText*) NULL;

    m_emptyPage = (wxPropertyGridPage*) NULL;

    m_selPage = -1;

    m_width = m_height = 0;

    m_splitterHeight = 5;

    m_splitterY = -1; // -1 causes default to be set.

    m_nextDescBoxSize = -1;

    m_extraHeight = 0;
    m_dragStatus = 0;
    m_onSplitter = 0;
    m_iFlags = 0;
}

// -----------------------------------------------------------------------

// These flags are always used in wxPropertyGrid integrated in wxPropertyGridManager.
#ifndef __WXMAC__
  #define wxPG_MAN_PROPGRID_FORCED_FLAGS (wxSIMPLE_BORDER| \
                                          wxNO_FULL_REPAINT_ON_RESIZE| \
                                          wxCLIP_CHILDREN)
#else
  #define wxPG_MAN_PROPGRID_FORCED_FLAGS (wxNO_BORDER| \
                                          wxNO_FULL_REPAINT_ON_RESIZE| \
                                          wxCLIP_CHILDREN)
#endif

// Which flags can be passed to underlying wxPropertyGrid.
#define wxPG_MAN_PASS_FLAGS_MASK       (0xFFF0|wxTAB_TRAVERSAL)

//
// Initialize after parent etc. set
//
void wxPropertyGridManager::Init2( int style )
{

    if ( m_iFlags & wxPG_FL_INITIALIZED )
        return;

    m_windowStyle |= (style&0x0000FFFF);

    wxSize csz = GetClientSize();

    m_cursorSizeNS = wxCursor(wxCURSOR_SIZENS);

    // Prepare the first page
    // NB: But just prepare - you still need to call Add/InsertPage
    //     to actually add properties on it.
    wxPropertyGridPage* pd = new wxPropertyGridPage();
    pd->m_isDefault = true;
    pd->m_manager = this;
    wxPropertyGridPageState* state = pd->GetStatePtr();
    state->m_pPropGrid = m_pPropGrid;
    m_arrPages.push_back( pd );
    m_pPropGrid->m_pState = state;

    wxWindowID baseId = GetId();
    wxWindowID useId = baseId;
    if ( baseId < 0 )
        baseId = wxPG_MAN_ALTERNATE_BASE_ID;

    m_baseId = baseId;

#ifdef __WXMAC__
   // Smaller controls on Mac
   SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

    // Create propertygrid.
    m_pPropGrid->Create(this,baseId,wxPoint(0,0),csz,
                        (m_windowStyle&wxPG_MAN_PASS_FLAGS_MASK)
                            |wxPG_MAN_PROPGRID_FORCED_FLAGS);

    m_pPropGrid->m_eventObject = this;

    m_pPropGrid->SetId(useId);

    m_pPropGrid->m_iFlags |= wxPG_FL_IN_MANAGER;

    m_pState = m_pPropGrid->m_pState;

    m_pPropGrid->SetExtraStyle(wxPG_EX_INIT_NOCAT);

    m_nextTbInd = baseId+ID_ADVTBITEMSBASE_OFFSET + 2;


    // Connect to property grid onselect event.
    // NB: Even if wxID_ANY is used, this doesn't connect properly in wxPython
    //     (see wxPropertyGridManager::ProcessEvent).
    Connect(m_pPropGrid->GetId()/*wxID_ANY*/,
            wxEVT_PG_SELECTED,
            wxPropertyGridEventHandler(wxPropertyGridManager::OnPropertyGridSelect) );

    // Connect to toolbar button events.
    Connect(baseId+ID_ADVTBITEMSBASE_OFFSET,baseId+ID_ADVTBITEMSBASE_OFFSET+50,
            wxEVT_COMMAND_TOOL_CLICKED,
            wxCommandEventHandler(wxPropertyGridManager::OnToolbarClick) );

    // Optional initial controls.
    m_width = -12345;

    m_iFlags |= wxPG_FL_INITIALIZED;

}

// -----------------------------------------------------------------------

wxPropertyGridManager::~wxPropertyGridManager()
{
    END_MOUSE_CAPTURE

    m_pPropGrid->DoSelectProperty(NULL);
    m_pPropGrid->m_pState = NULL;

    size_t i;
    for ( i=0; i<m_arrPages.size(); i++ )
    {
        delete m_arrPages[i];
    }

    delete m_emptyPage;
}

// -----------------------------------------------------------------------

wxPropertyGrid* wxPropertyGridManager::CreatePropertyGrid() const
{
    return new wxPropertyGrid();
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetId( wxWindowID winid )
{
    wxWindow::SetId(winid);

    // TODO: Reconnect propgrid event handler(s).

    m_pPropGrid->SetId(winid);
}

// -----------------------------------------------------------------------

wxSize wxPropertyGridManager::DoGetBestSize() const
{
    return wxSize(60,150);
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::SetFont( const wxFont& font )
{
    bool res = wxWindow::SetFont(font);
    m_pPropGrid->SetFont(font);

    // TODO: Need to do caption recacalculations for other pages as well.
    unsigned int i;
    for ( i=0; i<m_arrPages.size(); i++ )
    {
        wxPropertyGridPage* page = GetPage(i);

        if ( page != m_pPropGrid->GetState() )
            page->CalculateFontAndBitmapStuff(-1);
    }

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetExtraStyle( long exStyle )
{
    wxWindow::SetExtraStyle( exStyle );
    m_pPropGrid->SetExtraStyle( exStyle & 0xFFFFF000 );
#if wxUSE_TOOLBAR
    if ( (exStyle & wxPG_EX_NO_FLAT_TOOLBAR) && m_pToolbar )
        RecreateControls();
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::Freeze()
{
    m_pPropGrid->Freeze();
    wxWindow::Freeze();
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::Thaw()
{
    wxWindow::Thaw();
    m_pPropGrid->Thaw();
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetWindowStyleFlag( long style )
{
    int oldWindowStyle = GetWindowStyleFlag();

    wxWindow::SetWindowStyleFlag( style );
    m_pPropGrid->SetWindowStyleFlag( (m_pPropGrid->GetWindowStyleFlag()&~(wxPG_MAN_PASS_FLAGS_MASK)) |
                                   (style&wxPG_MAN_PASS_FLAGS_MASK) );

    // Need to re-position windows?
    if ( (oldWindowStyle & (wxPG_TOOLBAR|wxPG_DESCRIPTION)) !=
         (style & (wxPG_TOOLBAR|wxPG_DESCRIPTION)) )
    {
        RecreateControls();
    }
}

// -----------------------------------------------------------------------

// Actually shows given page.
bool wxPropertyGridManager::DoSelectPage( int index )
{
    // -1 means no page was selected
    //wxASSERT( m_selPage >= 0 );

    wxCHECK_MSG( index >= -1 && index < (int)GetPageCount(),
                 false,
                 wxT("invalid page index") );

    if ( m_selPage == index )
        return true;

    if ( m_pPropGrid->m_selected )
    {
        if ( !m_pPropGrid->ClearSelection() )
            return false;
    }

    wxPropertyGridPage* prevPage;

    if ( m_selPage >= 0 )
        prevPage = GetPage(m_selPage);
    else
        prevPage = m_emptyPage;

    wxPropertyGridPage* nextPage;

    if ( index >= 0 )
    {
        nextPage = m_arrPages[index];

        nextPage->OnShow();
    }
    else
    {
        if ( !m_emptyPage )
        {
            m_emptyPage = new wxPropertyGridPage();
            m_emptyPage->m_pPropGrid = m_pPropGrid;
        }

        nextPage = m_emptyPage;
    }

    m_iFlags |= wxPG_FL_DESC_REFRESH_REQUIRED;

    m_pPropGrid->SwitchState( nextPage->GetStatePtr() );

    m_pState = m_pPropGrid->m_pState;

    m_selPage = index;

#if wxUSE_TOOLBAR
    if ( m_pToolbar )
    {
        if ( index >= 0 )
            m_pToolbar->ToggleTool( nextPage->m_id, true );
        else
            m_pToolbar->ToggleTool( prevPage->m_id, false );
    }
#endif

    return true;
}

// -----------------------------------------------------------------------

// Changes page *and* set the target page for insertion operations.
void wxPropertyGridManager::SelectPage( int index )
{
    DoSelectPage(index);
}

// -----------------------------------------------------------------------

int wxPropertyGridManager::GetPageByName( const wxString& name ) const
{
    size_t i;
    for ( i=0; i<GetPageCount(); i++ )
    {
        if ( m_arrPages[i]->m_label == name )
            return i;
    }
    return wxNOT_FOUND;
}

// -----------------------------------------------------------------------

int wxPropertyGridManager::GetPageByState( const wxPropertyGridPageState* pState ) const
{
    wxASSERT( pState );

    size_t i;
    for ( i=0; i<GetPageCount(); i++ )
    {
        if ( pState == m_arrPages[i]->GetStatePtr() )
            return i;
    }

    return wxNOT_FOUND;
}

// -----------------------------------------------------------------------

const wxString& wxPropertyGridManager::GetPageName( int index ) const
{
    wxASSERT( index >= 0 && index < (int)GetPageCount() );
    return m_arrPages[index]->m_label;
}

// -----------------------------------------------------------------------

wxPropertyGridPageState* wxPropertyGridManager::GetPageState( int page ) const
{
    // Do not change this into wxCHECK because returning NULL is important
    // for wxPropertyGridInterface page enumeration mechanics.
    if ( page >= (int)GetPageCount() )
        return NULL;

    if ( page == -1 )
        return m_pState;
    return m_arrPages[page];
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::Clear()
{
    m_pPropGrid->Freeze();

    int i;
    for ( i=(int)GetPageCount()-1; i>=0; i-- )
        RemovePage(i);

    // Reset toolbar ids
    m_nextTbInd = m_baseId+ID_ADVTBITEMSBASE_OFFSET + 2;

    m_pPropGrid->Thaw();
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::ClearPage( int page )
{
    wxASSERT( page >= 0 );
    wxASSERT( page < (int)GetPageCount() );

    if ( page >= 0 && page < (int)GetPageCount() )
    {
        wxPropertyGridPageState* state = m_arrPages[page];

        if ( state == m_pPropGrid->GetState() )
            m_pPropGrid->Clear();
        else
            state->DoClear();
    }
}

// -----------------------------------------------------------------------

int wxPropertyGridManager::GetColumnCount( int page ) const
{
    wxASSERT( page >= -1 );
    wxASSERT( page < (int)GetPageCount() );

    return GetPageState(page)->GetColumnCount();
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetColumnCount( int colCount, int page )
{
    wxASSERT( page >= -1 );
    wxASSERT( page < (int)GetPageCount() );

    GetPageState(page)->SetColumnCount( colCount );
    GetGrid()->Refresh();
}
// -----------------------------------------------------------------------

size_t wxPropertyGridManager::GetPageCount() const
{
	if ( !(m_iFlags & wxPG_MAN_FL_PAGE_INSERTED) )
		return 0;

	return m_arrPages.size();
}

// -----------------------------------------------------------------------

wxPropertyGridPage* wxPropertyGridManager::InsertPage( int index,
                                                       const wxString& label,
                                                       const wxBitmap& bmp,
                                                       wxPropertyGridPage* pageObj )
{
    if ( index < 0 )
        index = GetPageCount();

    wxCHECK_MSG( (size_t)index == GetPageCount(), NULL,
        wxT("wxPropertyGridManager currently only supports appending pages (due to wxToolBar limitation)."));

    bool needInit = true;
    bool isPageInserted = m_iFlags & wxPG_MAN_FL_PAGE_INSERTED ? true : false;

    wxASSERT( index == 0 || isPageInserted );

    if ( !pageObj )
    {
        // No custom page object was given, so we will either re-use the default base
        // page (if index==0), or create a new default page object.
        if ( !isPageInserted )
        {
            pageObj = GetPage(0);
            // Of course, if the base page was custom, we need to delete and
            // re-create it.
            if ( !pageObj->m_isDefault )
            {
                delete pageObj;
                pageObj = new wxPropertyGridPage();
                m_arrPages[0] = pageObj;
            }
            needInit = false;
        }
        else
        {
            pageObj = new wxPropertyGridPage();
        }
        pageObj->m_isDefault = true;
    }
    else
    {
        if ( !isPageInserted )
        {
            // Initial page needs to be deleted and replaced
            delete GetPage(0);
            m_arrPages[0] = pageObj;
            m_pPropGrid->m_pState = pageObj->GetStatePtr();
        }
    }

    wxPropertyGridPageState* state = pageObj->GetStatePtr();

    pageObj->m_manager = this;

    if ( needInit )
    {
        state->m_pPropGrid = m_pPropGrid;
        state->InitNonCatMode();
    }

    if ( label.length() )
    {
        wxASSERT_MSG( !pageObj->m_label.length(),
                      wxT("If page label is given in constructor, empty label must be given in AddPage"));
        pageObj->m_label = label;
    }

    pageObj->m_id = m_nextTbInd;

    if ( isPageInserted )
        m_arrPages.push_back( pageObj );

#if wxUSE_TOOLBAR
    if ( m_windowStyle & wxPG_TOOLBAR )
    {
        if ( !m_pToolbar )
            RecreateControls();

        if ( !(GetExtraStyle()&wxPG_EX_HIDE_PAGE_BUTTONS) )
        {
            wxASSERT( m_pToolbar );

            // Add separator before first page.
            if ( GetPageCount() < 2 && (GetExtraStyle()&wxPG_EX_MODE_BUTTONS) &&
                 m_pToolbar->GetToolsCount() < 3 )
                m_pToolbar->AddSeparator();

            if ( &bmp != &wxNullBitmap )
                m_pToolbar->AddTool(m_nextTbInd,label,bmp,label,wxITEM_RADIO);
                //m_pToolbar->InsertTool(index+3,m_nextTbInd,bmp);
            else
                m_pToolbar->AddTool(m_nextTbInd,label,wxBitmap( (const char**)gs_xpm_defpage ),
                    label,wxITEM_RADIO);

            m_nextTbInd++;

            m_pToolbar->Realize();
        }
    }
#else
    wxUnusedVar(bmp);
#endif

    // If selected page was above the point of insertion, fix the current page index
    if ( isPageInserted )
    {
        if ( m_selPage >= index )
        {
            m_selPage += 1;
        }
    }
    else
    {
        // Set this value only when adding the first page
        m_selPage = 0;
    }

    pageObj->Init();

    m_iFlags |= wxPG_MAN_FL_PAGE_INSERTED;

    wxASSERT( pageObj->GetGrid() );

    return pageObj;
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::IsAnyModified() const
{
    size_t i;
    for ( i=0; i<GetPageCount(); i++ )
    {
        if ( m_arrPages[i]->GetStatePtr()->m_anyModified )
            return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::IsPageModified( size_t index ) const
{
    if ( m_arrPages[index]->GetStatePtr()->m_anyModified )
        return true;
    return false;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridManager::GetPageRoot( int index ) const
{
    wxASSERT( index >= 0 );
    wxASSERT( index < (int)m_arrPages.size() );

    return m_arrPages[index]->GetStatePtr()->m_properties;
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::RemovePage( int page )
{
    wxCHECK_MSG( (page >= 0) && (page < (int)GetPageCount()),
                 false,
                 wxT("invalid page index") );

    wxPropertyGridPage* pd = m_arrPages[page];

    if ( m_arrPages.size() == 1 )
    {
        // Last page: do not remove page entry
        m_pPropGrid->Clear();
        m_selPage = -1;
        m_iFlags &= ~wxPG_MAN_FL_PAGE_INSERTED;
        pd->m_label.clear();
    }

    // Change selection if current is page
    else if ( page == m_selPage )
    {
        if ( !m_pPropGrid->ClearSelection() )
                return false;

        // Substitute page to select
        int substitute = page - 1;
        if ( substitute < 0 )
            substitute = page + 1;

        SelectPage(substitute);
    }

    // Remove toolbar icon
#if wxUSE_TOOLBAR
    if ( HasFlag(wxPG_TOOLBAR) )
    {
        wxASSERT( m_pToolbar );

        int toolPos = GetExtraStyle() & wxPG_EX_MODE_BUTTONS ? 3 : 0;
        toolPos += page;

        // Delete separator as well, for consistency
        if ( (GetExtraStyle() & wxPG_EX_MODE_BUTTONS) &&
             GetPageCount() == 1 )
            m_pToolbar->DeleteToolByPos(2);

        m_pToolbar->DeleteToolByPos(toolPos);
    }
#endif

    if ( m_arrPages.size() > 1 )
    {
        m_arrPages.erase(m_arrPages.begin() + page);
        delete pd;
    }

    // Adjust indexes that were above removed
    if ( m_selPage > page )
        m_selPage--;

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::ProcessEvent( wxEvent& event )
{
    int evtType = event.GetEventType();

    // NB: For some reason, under wxPython, Connect in Init doesn't work properly,
    //     so we'll need to call OnPropertyGridSelect manually. Multiple call's
    //     don't really matter.
    if ( evtType == wxEVT_PG_SELECTED )
        OnPropertyGridSelect((wxPropertyGridEvent&)event);

    // Property grid events get special attention
    if ( evtType >= wxPG_BASE_EVT_TYPE &&
         evtType < (wxPG_MAX_EVT_TYPE) &&
         m_selPage >= 0 )
    {
        wxPropertyGridPage* page = GetPage(m_selPage);
        wxPropertyGridEvent* pgEvent = wxDynamicCast(&event, wxPropertyGridEvent);

        // Add property grid events to appropriate custom pages
        // but stop propagating to parent if page says it is
        // handling everything.
        if ( pgEvent && !page->m_isDefault )
        {
            /*if ( pgEvent->IsPending() )
                page->AddPendingEvent(event);
            else*/
                page->ProcessEvent(event);

            if ( page->IsHandlingAllEvents() )
                event.StopPropagation();
        }
    }

    return wxPanel::ProcessEvent(event);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::RepaintSplitter( wxDC& dc, int new_splittery, int new_width,
                                             int new_height, bool desc_too )
{
    int use_hei = new_height;

    // Draw background
    wxColour bgcol = GetBackgroundColour();
    dc.SetBrush( bgcol );
    dc.SetPen( bgcol );
    int rect_hei = use_hei-new_splittery;
    if ( !desc_too )
        rect_hei = m_splitterHeight;
    dc.DrawRectangle(0,new_splittery,new_width,rect_hei);
    dc.SetPen ( wxSystemSettings::GetColour ( wxSYS_COLOUR_3DDKSHADOW ) );
    int splitter_bottom = new_splittery+m_splitterHeight - 1;
    int box_height = use_hei-splitter_bottom;
    if ( box_height > 1 )
        dc.DrawRectangle(0,splitter_bottom,new_width,box_height);
    else
        dc.DrawLine(0,splitter_bottom,new_width,splitter_bottom);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::RefreshHelpBox( int new_splittery, int new_width, int new_height )
{
    //if ( new_splittery == m_splitterY && new_width == m_width )
    //    return;

    int use_hei = new_height;
    use_hei--;

    //wxRendererNative::Get().DrawSplitterSash(this,dc,
        //wxSize(width,m_splitterHeight),new_splittery,wxHORIZONTAL);

    //wxRendererNative::Get().DrawSplitterBorder(this,dc,
    //    wxRect(0,new_splittery,new_width,m_splitterHeight));

    // Fix help control positions.
    int cap_hei = m_pPropGrid->m_fontHeight;
    int cap_y = new_splittery+m_splitterHeight+5;
    int cnt_y = cap_y+cap_hei+3;
    int sub_cap_hei = cap_y+cap_hei-use_hei;
    int cnt_hei = use_hei-cnt_y;
    if ( sub_cap_hei > 0 )
    {
        cap_hei -= sub_cap_hei;
        cnt_hei = 0;
    }
    if ( cap_hei <= 2 )
    {
        m_pTxtHelpCaption->Show( false );
        m_pTxtHelpContent->Show( false );
    }
    else
    {
        m_pTxtHelpCaption->SetSize(3,cap_y,new_width-6,cap_hei);
        m_pTxtHelpCaption->Wrap(-1);
        m_pTxtHelpCaption->Show( true );
        if ( cnt_hei <= 2 )
        {
            m_pTxtHelpContent->Show( false );
        }
        else
        {
            m_pTxtHelpContent->SetSize(3,cnt_y,new_width-6,cnt_hei);
            m_pTxtHelpContent->Show( true );
        }
    }

    wxClientDC dc(this);
    RepaintSplitter( dc, new_splittery, new_width, new_height, true );

    m_splitterY = new_splittery;

    m_iFlags &= ~(wxPG_FL_DESC_REFRESH_REQUIRED);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::RecalculatePositions( int width, int height )
{
    int propgridY = 0;
    int propgridBottomY = height;

    // Toolbar at the top.
#if wxUSE_TOOLBAR
    if ( m_pToolbar )
    {
        int tbHeight;

    #if ( wxMINOR_VERSION < 6 || (wxMINOR_VERSION == 6 && wxRELEASE_NUMBER < 2) )
        tbHeight = -1;
    #else
        // In wxWidgets 2.6.2+, Toolbar default height may be broken
        #if defined(__WXMSW__)
            tbHeight = 24;
        #elif defined(__WXGTK__)
            tbHeight = -1; // 22;
        #elif defined(__WXMAC__)
            tbHeight = 22;
        #else
            tbHeight = 22;
        #endif
    #endif

        m_pToolbar->SetSize(0,0,width,tbHeight);
        propgridY += m_pToolbar->GetSize().y;
    }
#endif

    // Help box.
    if ( m_pTxtHelpCaption )
    {
        int new_splittery = m_splitterY;

        // Move m_splitterY
        if ( ( m_splitterY >= 0 || m_nextDescBoxSize ) && m_height > 32 )
        {
            if ( m_nextDescBoxSize >= 0 )
            {
                new_splittery = m_height - m_nextDescBoxSize - m_splitterHeight;
                m_nextDescBoxSize = -1;
            }
            new_splittery += (height-m_height);
        }
        else
        {
            new_splittery = height - wxPGMAN_DEFAULT_NEGATIVE_SPLITTER_Y;
            if ( new_splittery < 32 )
                new_splittery = 32;
        }

        // Check if beyond minimum.
        int nspy_min = propgridY + m_pPropGrid->m_lineHeight;
        if ( new_splittery < nspy_min )
            new_splittery = nspy_min;

        propgridBottomY = new_splittery;

        RefreshHelpBox( new_splittery, width, height );
    }

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        int pgh = propgridBottomY - propgridY;
        m_pPropGrid->SetSize( 0, propgridY, width, pgh );

        m_extraHeight = height - pgh;

        m_width = width;
        m_height = height;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetDescBoxHeight( int ht, bool refresh )
{
    if ( m_windowStyle & wxPG_DESCRIPTION )
    {
        if ( ht != GetDescBoxHeight() )
        {
            m_nextDescBoxSize = ht;
            if ( refresh )
                RecalculatePositions(m_width, m_height);
        }
    }
}

// -----------------------------------------------------------------------

int wxPropertyGridManager::GetDescBoxHeight() const
{
    return GetClientSize().y - m_splitterY - m_splitterHeight;
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();

    // Repaint splitter?
    int r_bottom = r.y + r.height;
    int splitter_bottom = m_splitterY + m_splitterHeight;
    if ( r.y < splitter_bottom && r_bottom >= m_splitterY )
        RepaintSplitter( dc, m_splitterY, m_width, m_height, false );
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::Refresh(bool eraseBackground, const wxRect* rect )
{
    m_pPropGrid->Refresh(eraseBackground);
    wxWindow::Refresh(eraseBackground,rect);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::RefreshProperty( wxPGProperty* p )
{
    wxPropertyGrid* grid = p->GetGrid();

    if ( GetPage(m_selPage)->GetStatePtr() == p->GetParent()->GetParentState() )
        grid->RefreshProperty(p);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::RecreateControls()
{

    bool was_shown = IsShown();
    if ( was_shown )
        Show ( false );

    wxWindowID baseId = m_pPropGrid->GetId();
    if ( baseId < 0 )
        baseId = wxPG_MAN_ALTERNATE_BASE_ID;

#if wxUSE_TOOLBAR
    if ( m_windowStyle & wxPG_TOOLBAR )
    {
        // Has toolbar.
        if ( !m_pToolbar )
        {
            m_pToolbar = new wxToolBar(this,baseId+ID_ADVTOOLBAR_OFFSET,
                                       wxDefaultPosition,wxDefaultSize,
                                       ((GetExtraStyle()&wxPG_EX_NO_FLAT_TOOLBAR)?0:wxTB_FLAT)
                                        /*| wxTB_HORIZONTAL | wxNO_BORDER*/ );

        #if defined(__WXMSW__)
            // Eliminate toolbar flicker on XP
            // NOTE: Not enabled since it corrupts drawing somewhat.

            /*
            #ifndef WS_EX_COMPOSITED
                #define WS_EX_COMPOSITED        0x02000000L
            #endif

            HWND hWnd = (HWND)m_pToolbar->GetHWND();

            ::SetWindowLong( hWnd, GWL_EXSTYLE,
                             ::GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_COMPOSITED );
            */

        #endif

            m_pToolbar->SetCursor ( *wxSTANDARD_CURSOR );

            if ( (GetExtraStyle()&wxPG_EX_MODE_BUTTONS) )
            {
                wxString desc1(_("Categorized Mode"));
                wxString desc2(_("Alphabetic Mode"));
                m_pToolbar->AddTool(baseId+ID_ADVTBITEMSBASE_OFFSET+0,
                    desc1,wxBitmap ( (const char**)gs_xpm_catmode ),
                    desc1,wxITEM_RADIO);
                m_pToolbar->AddTool(baseId+ID_ADVTBITEMSBASE_OFFSET+1,
                    desc2,wxBitmap ( (const char**)gs_xpm_noncatmode ),
                    desc2,wxITEM_RADIO);
                m_pToolbar->Realize();
            }

        }

        if ( (GetExtraStyle()&wxPG_EX_MODE_BUTTONS) )
        {
            // Toggle correct mode button.
            // TODO: This doesn't work in wxMSW (when changing,
            // both items will get toggled).
            int toggle_but_on_ind = ID_ADVTBITEMSBASE_OFFSET+0;
            int toggle_but_off_ind = ID_ADVTBITEMSBASE_OFFSET+1;
            if ( m_pPropGrid->m_pState->IsInNonCatMode() )
            {
                toggle_but_on_ind++;
                toggle_but_off_ind--;
            }

            m_pToolbar->ToggleTool(baseId+toggle_but_on_ind,true);
            m_pToolbar->ToggleTool(baseId+toggle_but_off_ind,false);
        }

    }
    else
    {
        // No toolbar.
        if ( m_pToolbar )
            m_pToolbar->Destroy();
        m_pToolbar = (wxToolBar*) NULL;
    }
#endif

    if ( m_windowStyle & wxPG_DESCRIPTION )
    {
        // Has help box.
        m_pPropGrid->m_iFlags |= (wxPG_FL_NOSTATUSBARHELP);

        if ( !m_pTxtHelpCaption )
        {
            m_pTxtHelpCaption = new wxStaticText (this,baseId+ID_ADVHELPCAPTION_OFFSET,wxEmptyString);
            m_pTxtHelpCaption->SetFont( m_pPropGrid->m_captionFont );
            m_pTxtHelpCaption->SetCursor ( *wxSTANDARD_CURSOR );
        }
        if ( !m_pTxtHelpContent )
        {
            m_pTxtHelpContent = new wxStaticText (this,baseId+ID_ADVHELPCONTENT_OFFSET,
                wxEmptyString,wxDefaultPosition,wxDefaultSize,wxALIGN_LEFT|wxST_NO_AUTORESIZE);
            m_pTxtHelpContent->SetCursor ( *wxSTANDARD_CURSOR );
        }

        SetDescribedProperty(GetSelection());
    }
    else
    {
        // No help box.
        m_pPropGrid->m_iFlags &= ~(wxPG_FL_NOSTATUSBARHELP);

        if ( m_pTxtHelpCaption )
            m_pTxtHelpCaption->Destroy();

        m_pTxtHelpCaption = (wxStaticText*) NULL;

        if ( m_pTxtHelpContent )
            m_pTxtHelpContent->Destroy();

        m_pTxtHelpContent = (wxStaticText*) NULL;
    }

    int width, height;

    GetClientSize(&width,&height);

    RecalculatePositions(width,height);

    if ( was_shown )
        Show ( true );
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridManager::DoGetPropertyByName( const wxString& name ) const
{
    size_t i;
    for ( i=0; i<GetPageCount(); i++ )
    {
        wxPropertyGridPageState* pState = m_arrPages[i]->GetStatePtr();
        wxPGProperty* p = pState->BaseGetPropertyByName(name);
        if ( p )
        {
            return p;
        }
    }
    return NULL;
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::EnsureVisible( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    wxPropertyGridPageState* parentState = p->GetParentState();

    // Select correct page.
    if ( m_pPropGrid->m_pState != parentState )
        DoSelectPage( GetPageByState(parentState) );

    return m_pPropGrid->EnsureVisible(id);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnToolbarClick( wxCommandEvent &event )
{
    int id = event.GetId();
    if ( id >= 0 )
    {
        int baseId = m_pPropGrid->GetId();
        if ( baseId < 0 )
            baseId = wxPG_MAN_ALTERNATE_BASE_ID;

        if ( id == ( baseId + ID_ADVTBITEMSBASE_OFFSET + 0 ) )
        {
            // Categorized mode.
            if ( m_pPropGrid->m_windowStyle & wxPG_HIDE_CATEGORIES )
            {
                if ( !m_pPropGrid->HasInternalFlag(wxPG_FL_CATMODE_AUTO_SORT) )
                    m_pPropGrid->m_windowStyle &= ~wxPG_AUTO_SORT;
                m_pPropGrid->EnableCategories( true );
            }
        }
        else if ( id == ( baseId + ID_ADVTBITEMSBASE_OFFSET + 1 ) )
        {
            // Alphabetic mode.
            if ( !(m_pPropGrid->m_windowStyle & wxPG_HIDE_CATEGORIES) )
            {
                if ( m_pPropGrid->HasFlag(wxPG_AUTO_SORT) )
                    m_pPropGrid->SetInternalFlag(wxPG_FL_CATMODE_AUTO_SORT);
                else
                    m_pPropGrid->ClearInternalFlag(wxPG_FL_CATMODE_AUTO_SORT);

                m_pPropGrid->m_windowStyle |= wxPG_AUTO_SORT;
                m_pPropGrid->EnableCategories( false );
            }
        }
        else
        {
            // Page Switching.

            int index = -1;
            size_t i;
            wxPropertyGridPage* pdc;

            // Find page with given id.
            for ( i=0; i<GetPageCount(); i++ )
            {
                pdc = m_arrPages[i];
                if ( pdc->m_id == id )
                {
                    index = i;
                    break;
                }
            }

            wxASSERT( index >= 0 );

            if ( DoSelectPage( index ) )
            {

                // Event dispatching must be last.
                m_pPropGrid->SendEvent(  wxEVT_PG_PAGE_CHANGED, (wxPGProperty*) NULL );

            }
            else
            {
                // TODO: Depress the old button on toolbar.
            }

        }
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::SetEditableStateItem( const wxString& name, wxVariant value )
{
    if ( name == wxS("descboxheight") )
    {
        SetDescBoxHeight(value.GetLong(), true);
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

wxVariant wxPropertyGridManager::GetEditableStateItem( const wxString& name ) const
{
    if ( name == wxS("descboxheight") )
    {
        return (long) GetDescBoxHeight();
    }
    return wxNullVariant;
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetDescription( const wxString& label, const wxString& content )
{
    if ( m_pTxtHelpCaption )
    {
        wxSize osz1 = m_pTxtHelpCaption->GetSize();
        wxSize osz2 = m_pTxtHelpContent->GetSize();

        m_pTxtHelpCaption->SetLabel(label);
        m_pTxtHelpContent->SetLabel(content);

        m_pTxtHelpCaption->SetSize(-1,osz1.y);
        m_pTxtHelpContent->SetSize(-1,osz2.y);

        if ( (m_iFlags & wxPG_FL_DESC_REFRESH_REQUIRED) || (osz2.x<(m_width-10)) )
            RefreshHelpBox( m_splitterY, m_width, m_height );
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetDescribedProperty( wxPGProperty* p )
{
    if ( m_pTxtHelpCaption )
    {
        if ( p )
        {
            SetDescription( p->GetLabel(), p->GetHelpString() );
        }
        else
        {
            m_pTxtHelpCaption->SetLabel(wxEmptyString);
            m_pTxtHelpContent->SetLabel(wxEmptyString);
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetSplitterLeft( bool subProps, bool allPages )
{
    if ( !allPages )
    {
        m_pPropGrid->SetSplitterLeft(subProps);
    }
    else
    {
        wxClientDC dc(this);
        dc.SetFont(m_pPropGrid->m_font);

        int highest = 0;
        unsigned int i;

        for ( i=0; i<GetPageCount(); i++ )
        {
            int maxW = m_pState->GetColumnFitWidth(dc, m_arrPages[i]->m_properties, 0, subProps );
            maxW += m_pPropGrid->m_marginWidth;
            if ( maxW > highest )
                highest = maxW;
        }

        if ( highest > 0 )
            m_pPropGrid->SetSplitterPosition( highest );

        m_pPropGrid->m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnPropertyGridSelect( wxPropertyGridEvent& event )
{
    // Check id.
    wxASSERT_MSG( GetId() == m_pPropGrid->GetId(),
        wxT("wxPropertyGridManager id must be set with wxPropertyGridManager::SetId (not wxWindow::SetId).") );

    SetDescribedProperty(event.GetProperty());
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnResize( wxSizeEvent& WXUNUSED(event) )
{
    int width, height;

    GetClientSize(&width,&height);

    if ( m_width == -12345 )
        RecreateControls();

    RecalculatePositions(width,height);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnMouseEntry( wxMouseEvent& WXUNUSED(event) )
{
    // Correct cursor. This is required atleast for wxGTK, for which
    // setting button's cursor to *wxSTANDARD_CURSOR does not work.
    SetCursor( wxNullCursor );
    m_onSplitter = 0;
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnMouseMove( wxMouseEvent &event )
{
    if ( !m_pTxtHelpCaption )
        return;

    int y = event.m_y;

    if ( m_dragStatus > 0 )
    {
        int sy = y - m_dragOffset;

        // Calculate drag limits
        int bottom_limit = m_height - m_splitterHeight + 1;
        int top_limit = m_pPropGrid->m_lineHeight;
#if wxUSE_TOOLBAR
        if ( m_pToolbar ) top_limit += m_pToolbar->GetSize().y;
#endif

        if ( sy >= top_limit && sy < bottom_limit )
        {

            int change = sy - m_splitterY;
            if ( change )
            {
                m_splitterY = sy;

                m_pPropGrid->SetSize( m_width, m_splitterY - m_pPropGrid->GetPosition().y );
                RefreshHelpBox( m_splitterY, m_width, m_height );

                m_extraHeight -= change;
                InvalidateBestSize();
            }

        }

    }
    else
    {
        if ( y >= m_splitterY && y < (m_splitterY+m_splitterHeight+2) )
        {
            SetCursor ( m_cursorSizeNS );
            m_onSplitter = 1;
        }
        else
        {
            if ( m_onSplitter )
            {
                SetCursor ( wxNullCursor );
            }
            m_onSplitter = 0;
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnMouseClick( wxMouseEvent &event )
{
    int y = event.m_y;

    // Click on splitter.
    if ( y >= m_splitterY && y < (m_splitterY+m_splitterHeight+2) )
    {
        if ( m_dragStatus == 0 )
        {
            //
            // Begin draggin the splitter
            //

            BEGIN_MOUSE_CAPTURE

            m_dragStatus = 1;

            m_dragOffset = y - m_splitterY;

        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnMouseUp( wxMouseEvent &event )
{
    // No event type check - basicly calling this method should
    // just stop dragging.

    if ( m_dragStatus >= 1 )
    {
        //
        // End Splitter Dragging
        //

        int y = event.m_y;

        // DO NOT ENABLE FOLLOWING LINE!
        // (it is only here as a reminder to not to do it)
        //m_splitterY = y;

        // This is necessary to return cursor
        END_MOUSE_CAPTURE

        // Set back the default cursor, if necessary
        if ( y < m_splitterY || y >= (m_splitterY+m_splitterHeight+2) )
        {
            SetCursor ( wxNullCursor );
        }

        m_dragStatus = 0;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetSplitterPosition( int pos, int splitterColumn )
{
    wxASSERT_MSG( GetPageCount(),
                  wxT("SetSplitterPosition() has no effect until pages have been added") );

    size_t i;
    for ( i=0; i<GetPageCount(); i++ )
    {
        wxPropertyGridPage* page = GetPage(i);
        page->DoSetSplitterPositionThisPage( pos, splitterColumn );
    }

    m_pPropGrid->SetInternalFlag(wxPG_FL_SPLITTER_PRE_SET);
}

// -----------------------------------------------------------------------
// wxPGVIterator_Manager
// -----------------------------------------------------------------------

// Default returned by wxPropertyGridInterface::CreateVIterator().
class wxPGVIteratorBase_Manager : public wxPGVIteratorBase
{
public:
    wxPGVIteratorBase_Manager( wxPropertyGridManager* manager, int flags )
        : m_manager(manager), m_flags(flags), m_curPage(0)
    {
        m_it.Init(manager->GetPage(0), flags);
    }
    virtual ~wxPGVIteratorBase_Manager() { }
    virtual void Next()
    {
        m_it.Next();

        // Next page?
        if ( m_it.AtEnd() )
        {
            m_curPage++;
            if ( m_curPage < m_manager->GetPageCount() )
                m_it.Init( m_manager->GetPage(m_curPage), m_flags );
        }
    }
private:
    wxPropertyGridManager*  m_manager;
    int                     m_flags;
    unsigned int            m_curPage;
};

wxPGVIterator wxPropertyGridManager::GetVIterator( int flags ) const
{
    return wxPGVIterator( new wxPGVIteratorBase_Manager( (wxPropertyGridManager*)this, flags ) );
}

#endif  // wxUSE_PROPGRID
