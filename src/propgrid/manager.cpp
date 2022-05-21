/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/manager.cpp
// Purpose:     wxPropertyGridManager
// Author:      Jaakko Salli
// Modified by:
// Created:     2005-01-14
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
    #include "wx/toolbar.h"
#endif

#include "wx/dcbuffer.h" // for wxALWAYS_NATIVE_DOUBLE_BUFFER
#include "wx/headerctrl.h" // for wxPGHeaderCtrl

// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include "wx/propgrid/manager.h"


#define wxPG_MAN_ALTERNATE_BASE_ID          11249 // Needed for wxID_ANY madness


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

#ifdef wxHAS_SVG
// Categoric Mode Icon
static const char gs_svg_catmode[] =
"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"32px\" height=\"32px\" viewBox=\"0 0 32 32\">"
"<circle cx=\"4\" cy=\"3\" r=\"2\" stroke-width=\"1.5\" stroke=\"#868686\" fill=\"#CACACA\"/>"
"<line x1=\"10\" y1=\"3\" x2=\"20\" y2=\"3\" stroke-width=\"2\" stroke=\"black\" stroke-linecap=\"square\"/>"
"<line x1=\"10\" y1=\"7\" x2=\"18\" y2=\"7\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"22\" y1=\"7\" x2=\"26\" y2=\"7\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"10\" y1=\"11\" x2=\"18\" y2=\"11\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"22\" y1=\"11\" x2=\"26\" y2=\"11\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"10\" y1=\"15\" x2=\"18\" y2=\"15\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"22\" y1=\"15\" x2=\"26\" y2=\"15\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"10\" y1=\"19\" x2=\"18\" y2=\"19\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"22\" y1=\"19\" x2=\"26\" y2=\"19\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<circle cx=\"4\" cy=\"25\" r=\"2\" stroke-width=\"1.5\" stroke=\"#868686\" fill=\"#CACACA\"/>"
"<line x1=\"10\" y1=\"25\" x2=\"20\" y2=\"25\" stroke-width=\"2\" stroke=\"black\" stroke-linecap=\"square\"/>"
"<line x1=\"10\" y1=\"29\" x2=\"18\" y2=\"29\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"22\" y1=\"29\" x2=\"26\" y2=\"29\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"</svg>";

// Alphabetic Mode Icon
static const char gs_svg_noncatmode[] =
"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"32px\" height=\"32px\" viewBox=\"0 0 32 32\">"
"<path d=\"M 1,9 L 4.5,9 M 2.5,9 L 6,1 L 9.5,9 M 7.5,9 L 11,9 M 4.2,6 L 7.8,6\" fill=\"none\" stroke=\"black\" stroke-width=\"1.5\"/>"
"<line x1=\"6\" y1=\"10\" x2=\"6\" y2=\"14\" stroke-width=\"1\" stroke=\"navy\" stroke-linecap=\"square\"/>"
"<polygon points=\"4,14 6,19 8,14\" stroke-width=\"0.1\" stroke=\"navy\"/>"
"<polyline points=\"2,23 2,21 9,21 2,29 9,29 9,27\" fill=\"none\" stroke=\"black\" stroke-width=\"1.5\"/>"
"<line x1=\"16\" y1=\"1\" x2=\"20\" y2=\"1\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"1\" x2=\"28\" y2=\"1\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"5\" x2=\"20\" y2=\"5\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"5\" x2=\"28\" y2=\"5\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"9\" x2=\"20\" y2=\"9\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"9\" x2=\"28\" y2=\"9\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"13\" x2=\"20\" y2=\"13\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"13\" x2=\"28\" y2=\"13\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"17\" x2=\"20\" y2=\"17\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"17\" x2=\"28\" y2=\"17\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"21\" x2=\"20\" y2=\"21\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"21\" x2=\"28\" y2=\"21\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"25\" x2=\"20\" y2=\"25\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"25\" x2=\"28\" y2=\"25\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"29\" x2=\"20\" y2=\"29\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>"
"<line x1=\"24\" y1=\"29\" x2=\"28\" y2=\"29\" stroke-width=\"2\" stroke=\"#868686\" stroke-linecap=\"square\"/>" 
"</svg>";

// Default Page Icon.
static const char gs_svg_defpage[] =
"<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"32px\" height=\"32px\" viewBox=\"0 0 32 32\">"
"<polygon points=\"5,4 27,4 27,28 5,28\" fill=\"none\" stroke-width=\"1.4\" stroke=\"black\"/>"
"<line x1=\"9\" y1=\"8.5\" x2=\"12\" y2=\"8.5\" stroke-width=\"2\" stroke=\"black\" stroke-linecap=\"square\"/>"
"<line x1=\"9\" y1=\"13.5\" x2=\"12\" y2=\"13.5\" stroke-width=\"2\" stroke=\"black\" stroke-linecap=\"square\"/>"
"<line x1=\"9\" y1=\"18.5\" x2=\"12\" y2=\"18.5\" stroke-width=\"2\" stroke=\"black\" stroke-linecap=\"square\"/>"
"<line x1=\"9\" y1=\"23.5\" x2=\"12\" y2=\"23.5\" stroke-width=\"2\" stroke=\"black\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"8.5\" x2=\"23\" y2=\"8.5\" stroke-width=\"2\" stroke=\"navy\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"13.5\" x2=\"23\" y2=\"13.5\" stroke-width=\"2\" stroke=\"navy\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"18.5\" x2=\"23\" y2=\"18.5\" stroke-width=\"2\" stroke=\"navy\" stroke-linecap=\"square\"/>"
"<line x1=\"16\" y1=\"23.5\" x2=\"23\" y2=\"23.5\" stroke-width=\"2\" stroke=\"navy\" stroke-linecap=\"square\"/>"
"</svg>";
#else
// Categoric Mode Icon
static const char* const gs_xpm_catmode[] = {
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

static const char* const gs_xpm_catmode_2x[] = {
"32 32 5 1",
". c none",
"B c black",
"D c #868686",
"L c #CACACA",
"W c #FFFFFF",
"................................",
"...DDDD.........................",
"..DDDDDD........................",
"..DDLLDD..BBBBBBBBBBBB..........",
"..DDLLDD..BBBBBBBBBBBB..........",
"..DDDDDD........................",
"...DDDD.........................",
"..........DDDDDDDDDD..DDDDDD....",
"..........DDDDDDDDDD..DDDDDD....",
"................................",
"................................",
"..........DDDDDDDDDD..DDDDDD....",
"..........DDDDDDDDDD..DDDDDD....",
"................................",
"................................",
"..........DDDDDDDDDD..DDDDDD....",
"..........DDDDDDDDDD..DDDDDD....",
"................................",
"................................",
"..........DDDDDDDDDD..DDDDDD....",
"..........DDDDDDDDDD..DDDDDD....",
"................................",
"................................",
"...DDDD.........................",
"..DDDDDD........................",
"..DDLLDD..BBBBBBBBBBBB..........",
"..DDLLDD..BBBBBBBBBBBB..........",
"..DDDDDD........................",
"...DDDD.........................",
"..........DDDDDDDDDD..DDDDDD....",
"..........DDDDDDDDDD..DDDDDD....",
"................................"
};

// Alphabetic Mode Icon
static const char* const gs_xpm_noncatmode[] = {
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

static const char* const gs_xpm_noncatmode_2x[] = {
"32 32 5 1",
". c none",
"B c black",
"D c #868686",
"L c #000080",
"W c #FFFFFF",
"................................",
"....DDBBDD......DDDDDD..DDDDDD..",
"....DDBBDD......DDDDDD..DDDDDD..",
"..DDBB..BBDD....................",
"..DDBB..BBDD....................",
"..BBBBBBBBBB....DDDDDD..DDDDDD..",
"..BBBBBBBBBB....DDDDDD..DDDDDD..",
"..BB......BB....................",
"..BB......BB....................",
"................DDDDDD..DDDDDD..",
"......LL........DDDDDD..DDDDDD..",
"......LL........................",
"......LL........................",
"......LL........DDDDDD..DDDDDD..",
"..LL..LL..LL....DDDDDD..DDDDDD..",
"...LL.LL.LL.....................",
"....LLLLLL......................",
".....LLLL.......DDDDDD..DDDDDD..",
"......LL........DDDDDD..DDDDDD..",
"......LL........................",
"................................",
"..BBBBBBBBBB....DDDDDD..DDDDDD..",
"..BBBBBBBBBB....DDDDDD..DDDDDD..",
"........BBDD....................",
".......BBDD.....................",
"......BBDD......DDDDDD..DDDDDD..",
".....BBDD.......DDDDDD..DDDDDD..",
"....BBDD........................",
"...BBDD.........................",
"..BBBBBBBBBB....DDDDDD..DDDDDD..",
"..BBBBBBBBBB....DDDDDD..DDDDDD..",
"................................"
};

// Default Page Icon.
static const char* const gs_xpm_defpage[] = {
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

static const char* const gs_xpm_defpage_2x[] = {
"32 32 5 1",
". c none",
"B c black",
"D c #868686",
"L c #000080",
"W c #FFFFFF",
"................................",
"................................",
"................................",
"................................",
"................................",
"....BBBBBBBBBBBBBBBBBBBBBBBB....",
"....BBBBBBBBBBBBBBBBBBBBBBBB....",
"....BB....................BB....",
"....BB....................BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB....................BB....",
"....BB....................BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB....................BB....",
"....BB....................BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB....................BB....",
"....BB....................BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB..BBBB..LLLLLLLLLL..BB....",
"....BB....................BB....",
"....BB....................BB....",
"....BBBBBBBBBBBBBBBBBBBBBBBB....",
"....BBBBBBBBBBBBBBBBBBBBBBBB....",
"................................",
"................................",
"................................",
"................................",
"................................"
};
#endif // wxHAS_SVG/!wxHAS_SVG

// -----------------------------------------------------------------------
// wxPropertyGridPage
// -----------------------------------------------------------------------


wxIMPLEMENT_CLASS(wxPropertyGridPage, wxEvtHandler);


wxBEGIN_EVENT_TABLE(wxPropertyGridPage, wxEvtHandler)
wxEND_EVENT_TABLE()


wxPropertyGridPage::wxPropertyGridPage()
    : wxEvtHandler(), wxPropertyGridInterface(), wxPropertyGridPageState()
    , m_manager(NULL)
    , m_isDefault(false)
{
    m_pState = this; // wxPropertyGridInterface to point to State
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

void wxPropertyGridPage::DoSetSplitterPosition( int pos,
                                                int splitterColumn,
                                                int flags )
{
    if ( (flags & wxPG_SPLITTER_ALL_PAGES) && m_manager->GetPageCount() )
        m_manager->SetSplitterPosition( pos, splitterColumn );
    else
        wxPropertyGridPageState::DoSetSplitterPosition( pos,
                                                        splitterColumn,
                                                        flags );
}

// -----------------------------------------------------------------------
// wxPGHeaderCtrl
// -----------------------------------------------------------------------

#if wxUSE_HEADERCTRL

class wxPGHeaderCtrl : public wxHeaderCtrl
{
public:
    wxPGHeaderCtrl(wxPropertyGridManager* manager, wxWindowID id, const wxPoint& pos,
                   const wxSize& size, long style)
        : wxHeaderCtrl(manager, id, pos, size, style)
        , m_manager(manager)
    {
        EnsureColumnCount(2);

        // Seed titles with defaults
        m_columns[0]->SetTitle(_("Property"));
        m_columns[1]->SetTitle(_("Value"));

        Bind(wxEVT_HEADER_RESIZING, &wxPGHeaderCtrl::OnResizing, this);
        Bind(wxEVT_HEADER_BEGIN_RESIZE, &wxPGHeaderCtrl::OnBeginResize, this);
        Bind(wxEVT_HEADER_END_RESIZE, &wxPGHeaderCtrl::OnEndResize, this);
    }

    virtual ~wxPGHeaderCtrl()
    {
        for (wxVector<wxHeaderColumnSimple*>::const_iterator it = m_columns.begin();
             it != m_columns.end(); ++it)
        {
            delete *it;
        }
    }

    virtual void OnColumnCountChanging(unsigned int count) wxOVERRIDE
    {
        EnsureColumnCount(count);
    }

    void OnPageChanged(const wxPropertyGridPage* page)
    {
        m_page = page;
        SetColumnCount(m_page->GetColumnCount());
        DetermineAllColumnWidths();
        UpdateAllColumns();
    }

    void OnColumWidthsChanged()
    {
        DetermineAllColumnWidths();
        UpdateAllColumns();
    }

    virtual const wxHeaderColumn& GetColumn(unsigned int idx) const wxOVERRIDE
    {
        return *m_columns[idx];
    }

    void SetColumnTitle(unsigned int idx, const wxString& title)
    {
        EnsureColumnCount(idx+1);
        m_columns[idx]->SetTitle(title);
    }

private:
    void UpdateAllColumns()
    {
        unsigned int colCount = GetColumnCount();
        for ( unsigned int i = 0; i < colCount; i++ )
        {
            UpdateColumn(i);
        }
    }

    void EnsureColumnCount(unsigned int count)
    {
        while ( m_columns.size() < count )
        {
            wxHeaderColumnSimple* colInfo = new wxHeaderColumnSimple(wxEmptyString);
            m_columns.push_back(colInfo);
        }
    }

    void DetermineAllColumnWidths() const
    {
        wxPropertyGrid* pg = m_manager->GetGrid();

        // Internal border width
        int borderWidth = pg->GetWindowBorderSize().x / 2;

        const unsigned int colCount = m_page->GetColumnCount();
        for ( unsigned int i = 0; i < colCount; i++ )
        {
            wxHeaderColumnSimple* colInfo = m_columns[i];

            int colWidth = m_page->GetColumnWidth(i);
            int colMinWidth = m_page->GetColumnMinWidth(i);
            if ( i == 0 )
            {
                // Compensate for the internal border
                int margin = pg->GetMarginWidth() + borderWidth;

                colWidth += margin;
                colMinWidth += margin;
            }
            else if ( i == colCount-1 )
            {
                // Compensate for the internal border and scrollbar
                int margin = borderWidth;

                colWidth += margin;
                colMinWidth += margin;
            }

            colInfo->SetWidth(colWidth);
            colInfo->SetMinWidth(colMinWidth);
        }
    }

    void OnSetColumnWidth(int col, int colWidth)
    {
        wxPropertyGrid* pg = m_manager->GetGrid();

        // Internal border width
        int borderWidth = pg->GetWindowBorderSize().x / 2;

        // Compensate for the internal border
        int x = -borderWidth;

        for ( int i=0; i<col; i++ )
            x += m_columns[i]->GetWidth();

        x += colWidth;

        pg->DoSetSplitterPosition(x, col,
                                  wxPG_SPLITTER_REFRESH |
                                  wxPG_SPLITTER_FROM_EVENT);
    }

    void OnResizing(wxHeaderCtrlEvent& evt)
    {
        int col = evt.GetColumn();
        int colWidth = evt.GetWidth();

        OnSetColumnWidth(col, colWidth);
        OnColumWidthsChanged();

        wxPropertyGrid* pg = m_manager->GetGrid();
        pg->SendEvent(wxEVT_PG_COL_DRAGGING, NULL, NULL, 0,
                      (unsigned int)col);
    }

    void OnBeginResize(wxHeaderCtrlEvent& evt)
    {
        int col = evt.GetColumn();
        wxPropertyGrid* pg = m_manager->GetGrid();

        // Don't allow resizing the rightmost column
        // (like it's not allowed for the rightmost wxPropertyGrid splitter)
        if ( col == (int)m_page->GetColumnCount() - 1 )
            evt.Veto();
        // Never allow column resize if layout is static
        else if ( m_manager->HasFlag(wxPG_STATIC_SPLITTER) )
            evt.Veto();
        // Allow application to veto dragging
        else if ( pg->SendEvent(wxEVT_PG_COL_BEGIN_DRAG,
                                NULL, NULL, 0,
                                (unsigned int)col) )
            evt.Veto();
    }

    void OnEndResize(wxHeaderCtrlEvent& evt)
    {
        int col = evt.GetColumn();
        wxPropertyGrid* pg = m_manager->GetGrid();
        pg->SendEvent(wxEVT_PG_COL_END_DRAG,
                      NULL, NULL, 0,
                      (unsigned int)col);
    }

    wxPropertyGridManager*          m_manager;
    const wxPropertyGridPage*       m_page;
    wxVector<wxHeaderColumnSimple*> m_columns;
};

#endif // wxUSE_HEADERCTRL

// -----------------------------------------------------------------------
// wxPropertyGridManager
// -----------------------------------------------------------------------

// Final default splitter y is client height minus this.
#define wxPGMAN_DEFAULT_NEGATIVE_SPLITTER_Y         100

// -----------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxPropertyGridManager, wxPanel);

// -----------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(wxPropertyGridManager, wxPanel)
  EVT_MOTION(wxPropertyGridManager::OnMouseMove)
  EVT_SIZE(wxPropertyGridManager::OnResize)
  EVT_PAINT(wxPropertyGridManager::OnPaint)
  EVT_LEFT_DOWN(wxPropertyGridManager::OnMouseClick)
  EVT_LEFT_UP(wxPropertyGridManager::OnMouseUp)
  EVT_LEAVE_WINDOW(wxPropertyGridManager::OnMouseEntry)
  //EVT_ENTER_WINDOW(wxPropertyGridManager::OnMouseEntry)
wxEND_EVENT_TABLE()

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
    if ( !m_pPropGrid )
        m_pPropGrid = CreatePropertyGrid();

    bool res = wxPanel::Create( parent, id, pos, size,
                                (style & wxWINDOW_STYLE_MASK)|wxWANTS_CHARS,
                                name );
    Init2(style);

    SetInitialSize(size);

    return res;
}

// -----------------------------------------------------------------------

//
// Initialize values to defaults
//
void wxPropertyGridManager::Init1()
{

    m_pPropGrid = NULL;

#if wxUSE_TOOLBAR
    m_pToolbar = NULL;
#endif
#if wxUSE_HEADERCTRL
    m_pHeaderCtrl = NULL;
    m_showHeader = false;
#endif
    m_pTxtHelpCaption = NULL;
    m_pTxtHelpContent = NULL;

    m_emptyPage = NULL;

    m_selPage = -1;

    m_width = m_height = 0;

    m_splitterHeight = 5;

    m_splitterY = -1; // -1 causes default to be set.

    m_nextDescBoxSize = -1;

    m_categorizedModeToolId = -1;
    m_alphabeticModeToolId = -1;

    m_extraHeight = 0;
    m_dragStatus = 0;
    m_onSplitter = false;
    m_iFlags = 0;
}

// -----------------------------------------------------------------------

// These flags are always used in wxPropertyGrid integrated in wxPropertyGridManager.
#define wxPG_MAN_PROPGRID_FORCED_FLAGS (  wxBORDER_THEME | \
                                          wxCLIP_CHILDREN)

// Which flags can be passed to underlying wxPropertyGrid.
#define wxPG_MAN_PASS_FLAGS_MASK       (wxPG_WINDOW_STYLE_MASK|wxTAB_TRAVERSAL)

//
// Initialize after parent etc. set
//
void wxPropertyGridManager::Init2( int style )
{

    if ( m_iFlags & wxPG_FL_INITIALIZED )
        return;

    m_windowStyle |= (style & wxPG_WINDOW_STYLE_MASK);

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

#ifdef __WXMAC__
   // Smaller controls on Mac
   SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif

   long propGridFlags = (m_windowStyle&wxPG_MAN_PASS_FLAGS_MASK)
                        |wxPG_MAN_PROPGRID_FORCED_FLAGS;

   propGridFlags &= ~wxBORDER_MASK;

   long pgManExStyle = 0;
   if ((style & wxPG_NO_INTERNAL_BORDER) == 0)
   {
       propGridFlags |= wxBORDER_THEME;
   }
   else
   {
       propGridFlags |= wxBORDER_NONE;
       pgManExStyle |= wxPG_EX_TOOLBAR_SEPARATOR;
   }

    // Create propertygrid.
    m_pPropGrid->Create(this,baseId,wxPoint(0,0),csz, propGridFlags);

    m_pPropGrid->m_eventObject = this;

    m_pPropGrid->SetId(useId);

    m_pPropGrid->SetInternalFlag(wxPG_FL_IN_MANAGER);

    m_pState = m_pPropGrid->m_pState;

    // Rely on native double-buffering by default.
    long pgExStyle = wxPG_EX_INIT_NOCAT;
#if wxALWAYS_NATIVE_DOUBLE_BUFFER
    pgExStyle |= wxPG_EX_NATIVE_DOUBLE_BUFFERING;
#endif // wxALWAYS_NATIVE_DOUBLE_BUFFER
    m_pPropGrid->SetExtraStyle(pgExStyle);
    wxWindow::SetExtraStyle(pgManExStyle | pgExStyle);

    // Connect to property grid onselect event.
    // NB: Even if wxID_ANY is used, this doesn't connect properly in wxPython
    //     (see wxPropertyGridManager::ProcessEvent).
    ReconnectEventHandlers(wxID_NONE, m_pPropGrid->GetId());

    // Optional initial controls.
    m_width = -12345;

    m_iFlags |= wxPG_FL_INITIALIZED;

}

// -----------------------------------------------------------------------

wxPropertyGridManager::~wxPropertyGridManager()
{
    END_MOUSE_CAPTURE

    //m_pPropGrid->ClearSelection();
    wxDELETE(m_pPropGrid);

    for ( size_t i = 0; i < m_arrPages.size(); i++ )
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

    // Reconnect propgrid event handlers.
    ReconnectEventHandlers(m_pPropGrid->GetId(), winid);

    m_pPropGrid->SetId(winid);
}

// -----------------------------------------------------------------------

wxSize wxPropertyGridManager::DoGetBestSize() const
{
    // Width: margin=15 + columns=2*40 + scroll bar
    return wxSize(15+2*40+wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, m_pPropGrid), 150);
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::SetFont( const wxFont& font )
{
    bool res = wxWindow::SetFont(font);
    m_pPropGrid->SetFont(font);

    // TODO: Need to do caption recalculations for other pages as well.
    for ( unsigned int i = 0; i < m_arrPages.size(); i++ )
    {
        wxPropertyGridPage* page = GetPage(i);

        if ( page != m_pPropGrid->GetState() )
            page->CalculateFontAndBitmapStuff(-1);
    }

    return res;
}

// -----------------------------------------------------------------------

// Which flags can affect the toolbar
#define wxPG_EX_WINDOW_TOOLBAR_STYLE_MASK  (wxPG_EX_NO_FLAT_TOOLBAR|wxPG_EX_MODE_BUTTONS|wxPG_EX_NO_TOOLBAR_DIVIDER)

void wxPropertyGridManager::SetExtraStyle( long exStyle )
{
    // Pass only relevant flags to wxPropertyGrid.
    m_pPropGrid->SetExtraStyle(exStyle & wxPG_EX_WINDOW_PG_STYLE_MASK);
    // Because it can happen that not all flags are actually changed
    // by call to SetExtraStyle() (e.g. wxPG_EX_NATIVE_DOUBLE_BUFFERING),
    // we have to get the actual style flags prior to storing them.
    exStyle &= ~wxPG_EX_WINDOW_PG_STYLE_MASK;
    exStyle |= m_pPropGrid->GetExtraStyle() & wxPG_EX_WINDOW_PG_STYLE_MASK;

#if wxUSE_TOOLBAR
    bool toolbarStyleChanged =
        (GetExtraStyle() & wxPG_EX_WINDOW_TOOLBAR_STYLE_MASK) != (exStyle & wxPG_EX_WINDOW_TOOLBAR_STYLE_MASK);
#endif // wxUSE_TOOLBAR

    wxWindow::SetExtraStyle( exStyle );
#if wxUSE_TOOLBAR
    if ( toolbarStyleChanged && m_pToolbar )
        RecreateControls();
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetWindowStyleFlag( long style )
{
    long oldWindowStyle = GetWindowStyleFlag();

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

bool wxPropertyGridManager::Reparent( wxWindowBase *newParent )
{
    if ( m_pPropGrid )
        m_pPropGrid->OnTLPChanging((wxWindow*)newParent);

    bool res = wxPanel::Reparent(newParent);

    return res;
}

// -----------------------------------------------------------------------

// Actually shows given page.
bool wxPropertyGridManager::DoSelectPage( int index )
{
    // -1 means no page was selected
    //wxASSERT( m_selPage >= 0 );

    wxCHECK_MSG( index >= -1 && index < (int)GetPageCount(),
                 false,
                 wxS("invalid page index") );

    if ( m_selPage == index )
        return true;

    if ( m_pPropGrid->GetSelection() )
    {
        if ( !m_pPropGrid->ClearSelection() )
            return false;
    }

#if wxUSE_TOOLBAR
    wxPropertyGridPage* prevPage;

    if ( m_selPage >= 0 )
        prevPage = GetPage(m_selPage);
    else
        prevPage = m_emptyPage;
#endif

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
            m_pToolbar->ToggleTool( nextPage->GetToolId(), true );
        else
            m_pToolbar->ToggleTool( prevPage->GetToolId(), false );
    }
#endif

#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() )
        m_pHeaderCtrl->OnPageChanged(nextPage);
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
    for ( size_t i = 0; i < GetPageCount(); i++ )
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

    for ( size_t i = 0; i < GetPageCount(); i++ )
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
    m_pPropGrid->ClearSelection(false);

    m_pPropGrid->Freeze();

    for ( int i=(int)GetPageCount()-1; i>=0; i-- )
        RemovePage(i);

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

    wxPropertyGridPageState* state = GetPageState(page);
#if wxUSE_HEADERCTRL
    // Update header only if column count is set for the currently visible page
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() && state == m_pState )
    {
        m_pHeaderCtrl->SetColumnCount(colCount);
    }
#endif // wxUSE_HEADERCTRL
    state->SetColumnCount( colCount );
    GetGrid()->Refresh();
#if wxUSE_HEADERCTRL
    // Update header only if column count is set for the currently visible page
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() && state == m_pState )
    {
        m_pHeaderCtrl->OnColumWidthsChanged();
    }
#endif // wxUSE_HEADERCTRL
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
                                                       const wxBitmapBundle& bmp,
                                                       wxPropertyGridPage* pageObj )
{
    if ( index < 0 )
        index = GetPageCount();

    wxCHECK_MSG( (size_t)index == GetPageCount(), NULL,
        wxS("wxPropertyGridManager currently only supports appending pages (due to wxToolBar limitation)."));

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
        if ( !isPageInserted )
        {
            state->EnableCategories(m_pPropGrid->HasFlag(wxPG_HIDE_CATEGORIES) ? false : true);
        }
    }

    if ( !label.empty() )
    {
        wxASSERT_MSG( pageObj->m_label.empty(),
                      wxS("If page label is given in constructor, empty label must be given in AddPage"));
        pageObj->m_label = label;
    }

    pageObj->m_toolId = -1;

    if ( !HasFlag(wxPG_SPLITTER_AUTO_CENTER) )
        pageObj->m_dontCenterSplitter = true;

    if ( isPageInserted )
        m_arrPages.push_back( pageObj );

#if wxUSE_TOOLBAR
    if ( m_windowStyle & wxPG_TOOLBAR )
    {
        if ( !m_pToolbar )
            RecreateControls();

        if ( !HasExtraStyle(wxPG_EX_HIDE_PAGE_BUTTONS) )
        {
            wxASSERT( m_pToolbar );

            // Add separator before first page.
            if ( GetPageCount() < 2 && HasExtraStyle(wxPG_EX_MODE_BUTTONS) &&
                 m_pToolbar->GetToolsCount() < 3 )
                m_pToolbar->AddSeparator();

            wxToolBarToolBase* tool;

            if ( bmp.IsOk() )
            {
                tool = m_pToolbar->AddTool(wxID_ANY, label, bmp, label, wxITEM_RADIO);
            }
            else
            {
#ifdef wxHAS_SVG
                wxBitmapBundle toolImages =
                    wxBitmapBundle::FromSVG(gs_svg_defpage, m_pToolbar->GetToolBitmapSize());
#else
                wxBitmap icon1x(gs_xpm_defpage);
                wxBitmap icon2x(gs_xpm_defpage_2x);
                icon2x.SetScaleFactor(2);
                wxBitmapBundle toolImages = wxBitmapBundle::FromBitmaps(icon1x, icon2x);
#endif // wxHAS_SVG/!wxHAS_SVG

                tool = m_pToolbar->AddTool(wxID_ANY, label, toolImages, label, wxITEM_RADIO);
            }

            pageObj->m_toolId = tool->GetId();

            // Connect to toolbar button events.
            Bind(wxEVT_TOOL, &wxPropertyGridManager::OnToolbarClick, this,
                 pageObj->GetToolId());

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
    for ( size_t i = 0; i < GetPageCount(); i++ )
    {
        if ( m_arrPages[i]->GetStatePtr()->m_anyModified )
            return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::IsPageModified( size_t index ) const
{
    wxCHECK_MSG( index < GetPageCount(), false, wxS("Invalid page index") );

#if WXWIN_COMPATIBILITY_3_0
    return m_arrPages[index]->GetStatePtr()->m_anyModified != (unsigned char)false;
#else
    return m_arrPages[index]->GetStatePtr()->m_anyModified;
#endif
}

// -----------------------------------------------------------------------

#if wxUSE_HEADERCTRL
void wxPropertyGridManager::ShowHeader(bool show)
{
    if ( show != m_showHeader)
    {
        m_showHeader = show;
        RecreateControls();
    }
}
#endif

// -----------------------------------------------------------------------

#if wxUSE_HEADERCTRL
void wxPropertyGridManager::SetColumnTitle( int idx, const wxString& title )
{
    if ( !m_pHeaderCtrl )
        ShowHeader();

    m_pHeaderCtrl->SetColumnTitle(idx, title);
}
#endif

// -----------------------------------------------------------------------

bool wxPropertyGridManager::IsPropertySelected( wxPGPropArg id ) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)
    for ( unsigned int i=0; i<GetPageCount(); i++ )
    {
        if ( GetPageState(i)->DoIsPropertySelected(p) )
            return true;
    }
    return false;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridManager::GetPageRoot( int index ) const
{
    wxCHECK_MSG( (index >= 0) && (index < (int)m_arrPages.size()),
                 NULL,
                 wxS("invalid page index") );

    return m_arrPages[index]->GetRoot();
}

// -----------------------------------------------------------------------

bool wxPropertyGridManager::RemovePage( int page )
{
    wxCHECK_MSG( (page >= 0) && (page < (int)GetPageCount()),
                 false,
                 wxS("invalid page index") );

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

        int toolPos = HasExtraStyle(wxPG_EX_MODE_BUTTONS) ? 3 : 0;
        toolPos += page;

        // Delete separator as well, for consistency
        if ( HasExtraStyle(wxPG_EX_MODE_BUTTONS) &&
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
    const wxEventType evtType = event.GetEventType();

    // NB: For some reason, under wxPython, Connect in Init doesn't work properly,
    //     so we'll need to call OnPropertyGridSelect manually. Multiple calls
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

void wxPropertyGridManager::RepaintDescBoxDecorations( wxDC& dc,
                                                       int newSplitterY,
                                                       int newWidth,
                                                       int newHeight )
{
    // Draw background
    wxColour bgcol = GetBackgroundColour();
    dc.SetBrush(bgcol);
    dc.SetPen(bgcol);
    int rectHeight = m_splitterHeight;
    dc.DrawRectangle(0, newSplitterY, newWidth, rectHeight);
    dc.SetPen( wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW) );
    int splitterBottom = newSplitterY + m_splitterHeight - 1;
    int boxHeight = newHeight - splitterBottom;
    if ( boxHeight > 1 )
        dc.DrawRectangle(0, splitterBottom, newWidth, boxHeight);
    else
        dc.DrawLine(0, splitterBottom, newWidth, splitterBottom);
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::UpdateDescriptionBox( int new_splittery, int new_width, int new_height )
{
    int use_hei = new_height-1;
    int use_width = new_width-6;

    // Fix help control positions.
    int cap_y = new_splittery+m_splitterHeight+5;
    m_pTxtHelpCaption->SetSize(3, cap_y, use_width, wxDefaultCoord, wxSIZE_AUTO_HEIGHT);
    int cap_hei = m_pTxtHelpCaption->GetSize().GetHeight();
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
        m_pTxtHelpCaption->Wrap(-1);
        m_pTxtHelpCaption->Show( true );
        if ( cnt_hei <= 2 )
        {
            m_pTxtHelpContent->Show( false );
        }
        else
        {
            m_pTxtHelpContent->SetSize(3,cnt_y,use_width,cnt_hei);
            m_pTxtHelpContent->Wrap(use_width);
            m_pTxtHelpContent->Show( true );
        }
    }

    wxRect r(0, new_splittery, new_width, new_height-new_splittery);
    RefreshRect(r);

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
        m_pToolbar->SetSize(0, 0, width, wxDefaultCoord);
        propgridY += m_pToolbar->GetSize().y;

        if ( HasExtraStyle(wxPG_EX_TOOLBAR_SEPARATOR) )
            propgridY += 1;
    }
#endif

    // Header comes after the tool bar
#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() )
    {
        m_pHeaderCtrl->SetSize(0, propgridY, width, wxDefaultCoord);
        // Sync horizontal scroll position with grid
        int x;
        m_pPropGrid->CalcScrolledPosition(0, 0, &x, NULL);
        m_pHeaderCtrl->ScrollWindow(x, 0);
        propgridY += m_pHeaderCtrl->GetSize().y;
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
        int nspy_min = propgridY + m_pPropGrid->GetRowHeight();
        if ( new_splittery < nspy_min )
            new_splittery = nspy_min;

        propgridBottomY = new_splittery;

        UpdateDescriptionBox( new_splittery, width, height );
    }

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        int pgh = propgridBottomY - propgridY;
        if ( pgh < 0 )
            pgh = 0;
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

    if ( HasExtraStyle(wxPG_EX_TOOLBAR_SEPARATOR) )
    {
#if wxUSE_TOOLBAR
        if (m_pToolbar && m_pPropGrid)
        {
            wxPen marginPen(m_pPropGrid->GetMarginColour());
            dc.SetPen(marginPen);

            int y = m_pPropGrid->GetPosition().y-1;
            dc.DrawLine(0, y, GetClientSize().x, y);
        }
#endif // wxUSE_TOOLBAR
    }

    // Repaint splitter and any other description box decorations
    if ( (r.y + r.height) >= m_splitterY && m_splitterY != -1)
        RepaintDescBoxDecorations( dc, m_splitterY, m_width, m_height );
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::RefreshProperty( wxPGProperty* p )
{
    wxASSERT( p->IsRoot() ||
              p->GetParentState() == p->GetParent()->GetParentState() );

    if ( GetPage(m_selPage)->GetStatePtr() == p->GetParentState() )
    {
        wxPropertyGrid* grid = p->GetGrid();
        grid->RefreshProperty(p);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::RecreateControls()
{
    bool was_shown = IsShown();
    if ( was_shown )
        Show ( false );

#if wxUSE_TOOLBAR
    if ( m_windowStyle & wxPG_TOOLBAR )
    {
        bool tbModified = false;

        long toolBarFlags = HasExtraStyle(wxPG_EX_NO_FLAT_TOOLBAR) ? 0 : wxTB_FLAT;
        if ( HasExtraStyle(wxPG_EX_NO_TOOLBAR_DIVIDER) )
            toolBarFlags |= wxTB_NODIVIDER;
        // Has toolbar.
        if ( !m_pToolbar )
        {
            m_pToolbar = new wxToolBar(this, wxID_ANY,
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       toolBarFlags);
            m_pToolbar->SetToolBitmapSize(wxSize(16, 15));

        #if defined(__WXMSW__)
            // Eliminate toolbar flicker on XP
            // NOTE: Not enabled since it corrupts drawing somewhat.

            /*
            #ifndef WS_EX_COMPOSITED
                #define WS_EX_COMPOSITED        0x02000000L
            #endif

            wxMSWWinExStyleUpdater(GetHwndOf(m_pToolbar))
                .TurnOn(WS_EX_COMPOSITED);
            */

        #endif

            m_pToolbar->SetCursor ( *wxSTANDARD_CURSOR );
            tbModified = true;
            m_categorizedModeToolId = -1;
            m_alphabeticModeToolId = -1;
        }
        else
        {
            m_pToolbar->SetWindowStyle(toolBarFlags);
            tbModified = true;
        }

        if ( HasExtraStyle(wxPG_EX_MODE_BUTTONS) )
        {
            // Add buttons if they don't already exist.
            if (m_categorizedModeToolId == -1)
            {
                wxString desc(_("Categorized Mode"));
#ifdef wxHAS_SVG
                wxBitmapBundle toolImages =
                    wxBitmapBundle::FromSVG(gs_svg_catmode, m_pToolbar->GetToolBitmapSize());
#else
                wxBitmap icon1x(gs_xpm_catmode);
                wxBitmap icon2x(gs_xpm_catmode_2x);
                icon2x.SetScaleFactor(2);
                wxBitmapBundle toolImages = wxBitmapBundle::FromBitmaps(icon1x, icon2x);
#endif // wxHAS_SVG/!wxHAS_SVG

                wxToolBarToolBase* tool = m_pToolbar->InsertTool(0,
                                            wxID_ANY,
                                            desc,
                                            toolImages, wxBitmapBundle(),
                                            wxITEM_RADIO,
                                            desc);
                m_categorizedModeToolId = tool->GetId();
                tbModified = true;

                Bind(wxEVT_TOOL, &wxPropertyGridManager::OnToolbarClick, this,
                     m_categorizedModeToolId);
            }

            if (m_alphabeticModeToolId == -1)
            {
                wxString desc(_("Alphabetic Mode"));
#ifdef wxHAS_SVG
                wxBitmapBundle toolImages =
                    wxBitmapBundle::FromSVG(gs_svg_noncatmode, m_pToolbar->GetToolBitmapSize());
#else
                wxBitmap icon1x(gs_xpm_noncatmode);
                wxBitmap icon2x(gs_xpm_noncatmode_2x);
                icon2x.SetScaleFactor(2);
                wxBitmapBundle toolImages = wxBitmapBundle::FromBitmaps(icon1x, icon2x);
#endif // wxHAS_SVG/!wxHAS_SVG

                wxToolBarToolBase* tool = m_pToolbar->InsertTool(1,
                                            wxID_ANY,
                                            desc,
                                            toolImages, wxBitmapBundle(),
                                            wxITEM_RADIO,
                                            desc);
                m_alphabeticModeToolId = tool->GetId();
                tbModified = true;

                Bind(wxEVT_TOOL, &wxPropertyGridManager::OnToolbarClick, this,
                     m_alphabeticModeToolId);
            }

            // Both buttons should exist here.
            wxASSERT( m_categorizedModeToolId != -1 && m_alphabeticModeToolId != -1);
        }
        else
        {
            // Remove buttons if they exist.
            if (m_categorizedModeToolId != -1)
            {
                Unbind(wxEVT_TOOL, &wxPropertyGridManager::OnToolbarClick, this,
                       m_categorizedModeToolId);

                m_pToolbar->DeleteTool(m_categorizedModeToolId);
                m_categorizedModeToolId = -1;
                tbModified = true;
            }

            if (m_alphabeticModeToolId != -1)
            {
                Unbind(wxEVT_TOOL, &wxPropertyGridManager::OnToolbarClick, this,
                       m_alphabeticModeToolId);

                m_pToolbar->DeleteTool(m_alphabeticModeToolId);
                m_alphabeticModeToolId = -1;
                tbModified = true;
            }

            // No button should exist here.
            wxASSERT( m_categorizedModeToolId == -1 && m_alphabeticModeToolId == -1);
        }

        // Rebuild toolbar if any changes were applied.
        if (tbModified)
        {
            m_pToolbar->Realize();
        }

        if ( HasExtraStyle(wxPG_EX_MODE_BUTTONS) )
        {
            // Toggle correct mode button.
            // TODO: This doesn't work in wxMSW (when changing,
            // both items will get toggled).
            int toggle_but_on_ind;
            int toggle_but_off_ind;
            if ( m_pPropGrid->m_pState->IsInNonCatMode() )
            {
                toggle_but_on_ind = m_alphabeticModeToolId;
                toggle_but_off_ind = m_categorizedModeToolId;
            }
            else
            {
                toggle_but_on_ind = m_categorizedModeToolId;
                toggle_but_off_ind = m_alphabeticModeToolId;
            }

            m_pToolbar->ToggleTool(toggle_but_on_ind, true);
            m_pToolbar->ToggleTool(toggle_but_off_ind, false);
        }
    }
    else
    {
        // No toolbar.
        if ( m_pToolbar )
            m_pToolbar->Destroy();
        m_pToolbar = NULL;
    }
#endif

#if wxUSE_HEADERCTRL
    if ( m_showHeader )
    {
        if ( !m_pHeaderCtrl )
        {
            m_pHeaderCtrl = new wxPGHeaderCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
        }
        else
        {
            m_pHeaderCtrl->Show();
        }

        m_pHeaderCtrl->OnPageChanged(GetCurrentPage());
    }
    else
    {
        if ( m_pHeaderCtrl )
            m_pHeaderCtrl->Hide();
    }
#endif

    if ( m_windowStyle & wxPG_DESCRIPTION )
    {
        // Has help box.
        m_pPropGrid->SetInternalFlag(wxPG_FL_NOSTATUSBARHELP);

        if ( !m_pTxtHelpCaption )
        {
            m_pTxtHelpCaption = new wxStaticText(this,
                                                 wxID_ANY,
                                                 wxEmptyString,
                                                 wxDefaultPosition,
                                                 wxDefaultSize,
                                                 wxALIGN_LEFT|wxST_NO_AUTORESIZE);
            m_pTxtHelpCaption->SetFont(m_pPropGrid->GetCaptionFont());
            m_pTxtHelpCaption->SetCursor( *wxSTANDARD_CURSOR );
        }
        if ( !m_pTxtHelpContent )
        {
            m_pTxtHelpContent = new wxStaticText(this,
                                                 wxID_ANY,
                                                 wxEmptyString,
                                                 wxDefaultPosition,
                                                 wxDefaultSize,
                                                 wxALIGN_LEFT|wxST_NO_AUTORESIZE);
            m_pTxtHelpContent->SetCursor( *wxSTANDARD_CURSOR );
        }

        SetDescribedProperty(GetSelection());
    }
    else
    {
        // No help box.
        m_pPropGrid->ClearInternalFlag(wxPG_FL_NOSTATUSBARHELP);

        if ( m_pTxtHelpCaption )
            m_pTxtHelpCaption->Destroy();

        m_pTxtHelpCaption = NULL;

        if ( m_pTxtHelpContent )
            m_pTxtHelpContent->Destroy();

        m_pTxtHelpContent = NULL;
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
    for ( size_t i = 0; i < GetPageCount(); i++ )
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

#if wxUSE_TOOLBAR
void wxPropertyGridManager::OnToolbarClick( wxCommandEvent &event )
{
    int id = event.GetId();

    if ( id == m_categorizedModeToolId )
    {
        // Categorized mode.
        if ( m_pPropGrid->HasFlag(wxPG_HIDE_CATEGORIES) )
        {
            if ( !m_pPropGrid->HasInternalFlag(wxPG_FL_CATMODE_AUTO_SORT) )
                m_pPropGrid->m_windowStyle &= ~wxPG_AUTO_SORT;
            m_pPropGrid->EnableCategories( true );
        }
    }
    else if ( id == m_alphabeticModeToolId )
    {
        // Alphabetic mode.
        if ( !m_pPropGrid->HasFlag(wxPG_HIDE_CATEGORIES) )
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

        // Find page with given id.
        for ( size_t i = 0; i < GetPageCount(); i++ )
        {
            wxPropertyGridPage* pdc = m_arrPages[i];
            if ( pdc->GetToolId() == id )
            {
                index = i;
                break;
            }
        }

        wxASSERT( index >= 0 );

        if ( DoSelectPage(index) )
        {
            // Event dispatching must be last.
            m_pPropGrid->SendEvent( wxEVT_PG_PAGE_CHANGED, (wxPGProperty*)NULL );
        }
        else
        {
            // Restore button state on toolbar.
            wxToolBar* tb = wxDynamicCast(event.GetEventObject(), wxToolBar);
            wxASSERT( tb );

            // Release the current button.
            tb->ToggleTool(id, false);
            // Depress the old button.
            if ( m_selPage >= 0 )
            {
                wxPropertyGridPage* prevPage = m_arrPages[m_selPage];
                tb->ToggleTool(prevPage->GetToolId(), true);
            }
        }
    }
}
#endif

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

        m_pTxtHelpCaption->SetSize(wxDefaultCoord, osz1.y);
        m_pTxtHelpContent->SetSize(wxDefaultCoord, osz2.y);

        UpdateDescriptionBox( m_splitterY, m_width, m_height );
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
            SetDescription( wxEmptyString, wxEmptyString );
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
        dc.SetFont(m_pPropGrid->GetFont());

        int highest = 0;

        for ( size_t i = 0; i < GetPageCount(); i++ )
        {
            int maxW = m_pState->GetColumnFitWidth(dc, m_arrPages[i]->DoGetRoot(), 0, subProps );
            maxW += m_pPropGrid->GetMarginWidth();
            if ( maxW > highest )
                highest = maxW;
            m_pState->m_dontCenterSplitter = true;
        }

        if ( highest > 0 )
            SetSplitterPosition( highest );
    }

#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() )
        m_pHeaderCtrl->OnColumWidthsChanged();
#endif
}

void wxPropertyGridManager::SetPageSplitterLeft(int page, bool subProps)
{
    wxASSERT_MSG( (page < (int) GetPageCount()),
                  wxS("SetPageSplitterLeft() has no effect until pages have been added") );

    if (page < (int) GetPageCount())
    {
        wxClientDC dc(this);
        dc.SetFont(m_pPropGrid->GetFont());

        int maxW = m_pState->GetColumnFitWidth(dc, m_arrPages[page]->DoGetRoot(), 0, subProps );
        maxW += m_pPropGrid->GetMarginWidth();
        SetPageSplitterPosition( page, maxW );

#if wxUSE_HEADERCTRL
        if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() )
            m_pHeaderCtrl->OnColumWidthsChanged();
#endif
    }
}

void wxPropertyGridManager::ReconnectEventHandlers(wxWindowID oldId, wxWindowID newId)
{
    wxCHECK_RET( oldId != newId,
                 wxS("Attempting to reconnect event handlers to the same window"));

    if (oldId != wxID_NONE)
    {
        Unbind(wxEVT_PG_SELECTED, &wxPropertyGridManager::OnPropertyGridSelect, this,
               oldId);
        Unbind(wxEVT_PG_HSCROLL, &wxPropertyGridManager::OnPGScrollH, this, oldId);
        Unbind(wxEVT_PG_COLS_RESIZED, &wxPropertyGridManager::OnColWidthsChanged, this, oldId);
    }

    if (newId != wxID_NONE)
    {
        Bind(wxEVT_PG_SELECTED, &wxPropertyGridManager::OnPropertyGridSelect, this,
             newId);
        Bind(wxEVT_PG_HSCROLL, &wxPropertyGridManager::OnPGScrollH, this, newId);
        Bind(wxEVT_PG_COLS_RESIZED, &wxPropertyGridManager::OnColWidthsChanged, this, newId);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnPropertyGridSelect( wxPropertyGridEvent& event )
{
    // Check id.
    wxASSERT_MSG( GetId() == m_pPropGrid->GetId(),
        wxS("wxPropertyGridManager id must be set with wxPropertyGridManager::SetId (not wxWindow::SetId).") );

    SetDescribedProperty(event.GetProperty());
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnPGScrollH(wxPropertyGridEvent& evt)
{
#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl )
    {
        m_pHeaderCtrl->ScrollWindow(evt.GetInt(), 0);
    }
#endif // wxUSE_HEADERCTRL
}

void wxPropertyGridManager::OnColWidthsChanged(wxPropertyGridEvent& WXUNUSED(evt))
{
#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl )
        m_pHeaderCtrl->OnColumWidthsChanged();
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnResize( wxSizeEvent& WXUNUSED(event) )
{
    int width, height;

    GetClientSize(&width, &height);

    if ( m_width == -12345 )
        RecreateControls();

    RecalculatePositions(width, height);

    if ( m_pPropGrid && m_pPropGrid->GetParent() )
    {
        int pgWidth, pgHeight;
        m_pPropGrid->GetClientSize(&pgWidth, &pgHeight);

        // Regenerate splitter positions for non-current pages
        for ( unsigned int i=0; i<GetPageCount(); i++ )
        {
            wxPropertyGridPage* page = GetPage(i);
            if ( page != m_pPropGrid->GetState() )
            {
                page->OnClientWidthChange(pgWidth,
                                          pgWidth - page->GetVirtualWidth(),
                                          true);
            }
        }
    }

#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() )
        m_pHeaderCtrl->OnColumWidthsChanged();
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::OnMouseEntry( wxMouseEvent& WXUNUSED(event) )
{
    // Correct cursor. This is required at least for wxGTK, for which
    // setting button's cursor to *wxSTANDARD_CURSOR does not work.
    SetCursor( wxNullCursor );
    m_onSplitter = false;
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
        int top_limit = m_pPropGrid->GetRowHeight();
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
                UpdateDescriptionBox( m_splitterY, m_width, m_height );

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
            m_onSplitter = true;
        }
        else
        {
            if ( m_onSplitter )
            {
                SetCursor ( wxNullCursor );
            }
            m_onSplitter = false;
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
            // Begin dragging the splitter
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
    // No event type check - basically calling this method should
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
                  wxS("SetSplitterPosition() has no effect until pages have been added") );

    for ( size_t i = 0; i < GetPageCount(); i++ )
    {
        wxPropertyGridPage* page = GetPage(i);
        page->DoSetSplitterPosition( pos, splitterColumn,
                                     wxPG_SPLITTER_REFRESH );
    }

#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() )
        m_pHeaderCtrl->OnColumWidthsChanged();
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGridManager::SetPageSplitterPosition( int page,
                                                     int pos,
                                                     int column )
{
    GetPage(page)->DoSetSplitterPosition( pos, column );

#if wxUSE_HEADERCTRL
    if ( m_pHeaderCtrl && m_pHeaderCtrl->IsShown() )
        m_pHeaderCtrl->OnColumWidthsChanged();
#endif
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
    virtual void Next() wxOVERRIDE
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
    return wxPGVIterator(new wxPGVIteratorBase_Manager(const_cast<wxPropertyGridManager*>(this), flags));
}

#endif  // wxUSE_PROPGRID
