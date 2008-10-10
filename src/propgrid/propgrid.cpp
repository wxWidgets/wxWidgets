/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgrid.cpp
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     2004-09-25
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
    #include "wx/dcmemory.h"
    #include "wx/button.h"
    #include "wx/pen.h"
    #include "wx/brush.h"
    #include "wx/cursor.h"
    #include "wx/dialog.h"
    #include "wx/settings.h"
    #include "wx/msgdlg.h"
    #include "wx/choice.h"
    #include "wx/stattext.h"
    #include "wx/scrolwin.h"
    #include "wx/dirdlg.h"
    #include "wx/sizer.h"
    #include "wx/textdlg.h"
    #include "wx/filedlg.h"
    #include "wx/statusbr.h"
    #include "wx/intl.h"
    #include "wx/frame.h"
#endif


// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/editors.h"

#if wxPG_USE_RENDERER_NATIVE
    #include "wx/renderer.h"
#endif

#include "wx/odcombo.h"

#include "wx/timer.h"
#include "wx/dcbuffer.h"
#include "wx/clipbrd.h"
#include "wx/dataobj.h"

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

// Two pics for the expand / collapse buttons.
// Files are not supplied with this project (since it is
// recommended to use either custom or native rendering).
// If you want them, get wxTreeMultiCtrl by Jorgen Bodde,
// and copy xpm files from archive to wxPropertyGrid src directory
// (and also comment/undef wxPG_ICON_WIDTH in propGrid.h
// and set wxPG_USE_RENDERER_NATIVE to 0).
#ifndef wxPG_ICON_WIDTH
  #if defined(__WXMAC__)
    #include "mac_collapse.xpm"
    #include "mac_expand.xpm"
  #elif defined(__WXGTK__)
    #include "linux_collapse.xpm"
    #include "linux_expand.xpm"
  #else
    #include "default_collapse.xpm"
    #include "default_expand.xpm"
  #endif
#endif


//#define wxPG_TEXT_INDENT                4 // For the wxComboControl
#define wxPG_ALLOW_CLIPPING             1 // If 1, GetUpdateRegion() in OnPaint event handler is not ignored
#define wxPG_GUTTER_DIV                 3 // gutter is max(iconwidth/gutter_div,gutter_min)
#define wxPG_GUTTER_MIN                 3 // gutter before and after image of [+] or [-]
#define wxPG_YSPACING_MIN               1
#define wxPG_DEFAULT_VSPACING           2 // This matches .NET propertygrid's value,
                                          // but causes normal combobox to spill out under MSW

#define wxPG_OPTIMAL_WIDTH              200 // Arbitrary

#define wxPG_MIN_SCROLLBAR_WIDTH        10 // Smallest scrollbar width on any platform
                                           // Must be larger than largest control border
                                           // width * 2.


#define wxPG_DEFAULT_CURSOR             wxNullCursor


//#define wxPG_NAT_CHOICE_BORDER_ANY   0

#define wxPG_HIDER_BUTTON_HEIGHT        25

#define wxPG_PIXELS_PER_UNIT            m_lineHeight

#ifdef wxPG_ICON_WIDTH
  #define m_iconHeight m_iconWidth
#endif

#define wxPG_TOOLTIP_DELAY              1000

// -----------------------------------------------------------------------

#if wxUSE_INTL
void wxPropertyGrid::AutoGetTranslation ( bool enable )
{
    wxPGGlobalVars->m_autoGetTranslation = enable;
}
#else
void wxPropertyGrid::AutoGetTranslation ( bool ) { }
#endif

// -----------------------------------------------------------------------

const wxChar *wxPropertyGridNameStr = wxT("wxPropertyGrid");

// -----------------------------------------------------------------------
// Statics in one class for easy destruction.
// -----------------------------------------------------------------------

#include "wx/module.h"

class wxPGGlobalVarsClassManager : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxPGGlobalVarsClassManager)
public:
    wxPGGlobalVarsClassManager() {}
    virtual bool OnInit() { wxPGGlobalVars = new wxPGGlobalVarsClass(); return true; }
    virtual void OnExit() { delete wxPGGlobalVars; wxPGGlobalVars = NULL; }
};

IMPLEMENT_DYNAMIC_CLASS(wxPGGlobalVarsClassManager, wxModule)


wxPGGlobalVarsClass* wxPGGlobalVars = (wxPGGlobalVarsClass*) NULL;


wxPGGlobalVarsClass::wxPGGlobalVarsClass()
{
    wxPGProperty::sm_wxPG_LABEL = new wxString(wxPG_LABEL_STRING);

    m_boolChoices.Add(_("False"));
    m_boolChoices.Add(_("True"));

    m_fontFamilyChoices = (wxPGChoices*) NULL;

    m_defaultRenderer = new wxPGDefaultRenderer();

    m_autoGetTranslation = false;

    m_offline = 0;

    m_extraStyle = 0;

    wxVariant v;

	// Prepare some shared variants
    m_vEmptyString = wxString();
    m_vZero = (long) 0;
    m_vMinusOne = (long) -1;
    m_vTrue = true;
    m_vFalse = false;

    // Prepare cached string constants
    m_strstring = wxS("string");
    m_strlong = wxS("long");
    m_strbool = wxS("bool");
    m_strlist = wxS("list");
    m_strMin = wxS("Min");
    m_strMax = wxS("Max");
    m_strUnits = wxS("Units");
    m_strInlineHelp = wxS("InlineHelp");

#ifdef __WXDEBUG__
    m_warnings = 0;
#endif
}


wxPGGlobalVarsClass::~wxPGGlobalVarsClass()
{
    size_t i;

    delete m_defaultRenderer;

    // This will always have one ref
    delete m_fontFamilyChoices;

#if wxUSE_VALIDATORS
    for ( i=0; i<m_arrValidators.size(); i++ )
        delete ((wxValidator*)m_arrValidators[i]);
#endif

    //
    // Destroy value type class instances.
    wxPGHashMapS2P::iterator vt_it;

    // Destroy editor class instances.
    // iterate over all the elements in the class
    for( vt_it = m_mapEditorClasses.begin(); vt_it != m_mapEditorClasses.end(); ++vt_it )
    {
        delete ((wxPGEditor*)vt_it->second);
    }

    delete wxPGProperty::sm_wxPG_LABEL;
}

void wxPropertyGridInitGlobalsIfNeeded()
{
}

// -----------------------------------------------------------------------
// wxPGBrush
// -----------------------------------------------------------------------

//
// This class is a wxBrush derivative used in the background colour
// brush cache. It adds wxPG-type colour-in-long to the class.
// JMS: Yes I know wxBrush doesn't actually hold the value (refcounted
//   object does), but this is simpler implementation and equally
//   effective.
//

class wxPGBrush : public wxBrush
{
public:
    wxPGBrush( const wxColour& colour );
    wxPGBrush();
    virtual ~wxPGBrush() { }
    void SetColour2( const wxColour& colour );
    inline long GetColourAsLong() const { return m_colAsLong; }
private:
    long    m_colAsLong;
};


void wxPGBrush::SetColour2( const wxColour& colour )
{
    wxBrush::SetColour(colour);
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}


wxPGBrush::wxPGBrush() : wxBrush()
{
    m_colAsLong = 0;
}


wxPGBrush::wxPGBrush( const wxColour& colour ) : wxBrush(colour)
{
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}


// -----------------------------------------------------------------------
// wxPGColour
// -----------------------------------------------------------------------

//
// Same as wxPGBrush, but for wxColour instead.
//

class wxPGColour : public wxColour
{
public:
    wxPGColour( const wxColour& colour );
    wxPGColour();
    virtual ~wxPGColour() { }
    void SetColour2( const wxColour& colour );
    inline long GetColourAsLong() const { return m_colAsLong; }
private:
    long    m_colAsLong;
};


void wxPGColour::SetColour2( const wxColour& colour )
{
    *this = colour;
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}


wxPGColour::wxPGColour() : wxColour()
{
    m_colAsLong = 0;
}


wxPGColour::wxPGColour( const wxColour& colour ) : wxColour(colour)
{
    m_colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());
}


// -----------------------------------------------------------------------
// wxPGTLWHandler
//   Intercepts Close-events sent to wxPropertyGrid's top-level parent,
//   and tries to commit property value.
// -----------------------------------------------------------------------

class wxPGTLWHandler : public wxEvtHandler
{
public:

    wxPGTLWHandler( wxPropertyGrid* pg )
        : wxEvtHandler()
    {
        m_pg = pg;
    }

protected:

    void OnClose( wxCloseEvent& event )
    {
        // ClearSelection forces value validation/commit.
        if ( event.CanVeto() && !m_pg->ClearSelection() )
        {
            event.Veto();
            return;
        }

        event.Skip();
    }

private:
    wxPropertyGrid*     m_pg;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxPGTLWHandler, wxEvtHandler)
    EVT_CLOSE(wxPGTLWHandler::OnClose)
END_EVENT_TABLE()

// -----------------------------------------------------------------------
// wxPGCanvas
// -----------------------------------------------------------------------

//
// wxPGCanvas acts as a graphics sub-window of the
// wxScrolledWindow that wxPropertyGrid is.
//
class wxPGCanvas : public wxPanel
{
public:
    wxPGCanvas() : wxPanel()
    {
    }
    virtual ~wxPGCanvas() { }

protected:
    void OnMouseMove( wxMouseEvent &event )
    {
        wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
        pg->OnMouseMove( event );
    }

    void OnMouseClick( wxMouseEvent &event )
    {
        wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
        pg->OnMouseClick( event );
    }

    void OnMouseUp( wxMouseEvent &event )
    {
        wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
        pg->OnMouseUp( event );
    }

    void OnMouseRightClick( wxMouseEvent &event )
    {
        wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
        pg->OnMouseRightClick( event );
    }

    void OnMouseDoubleClick( wxMouseEvent &event )
    {
        wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
        pg->OnMouseDoubleClick( event );
    }

    void OnKey( wxKeyEvent& event )
    {
        wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
        pg->OnKey( event );
    }

    void OnKeyUp( wxKeyEvent& event )
    {
        wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
        pg->OnKeyUp( event );
    }

    void OnPaint( wxPaintEvent& event );
    
    // Always be focussable, even with child windows
    virtual void SetCanFocus(bool WXUNUSED(canFocus))
    {  wxPanel::SetCanFocus(true); }
    

private:
    DECLARE_EVENT_TABLE()
    DECLARE_ABSTRACT_CLASS(wxPGCanvas)
};


IMPLEMENT_ABSTRACT_CLASS(wxPGCanvas,wxPanel)

BEGIN_EVENT_TABLE(wxPGCanvas, wxPanel)
    EVT_MOTION(wxPGCanvas::OnMouseMove)
    EVT_PAINT(wxPGCanvas::OnPaint)
    EVT_LEFT_DOWN(wxPGCanvas::OnMouseClick)
    EVT_LEFT_UP(wxPGCanvas::OnMouseUp)
    EVT_RIGHT_UP(wxPGCanvas::OnMouseRightClick)
    EVT_LEFT_DCLICK(wxPGCanvas::OnMouseDoubleClick)
    EVT_KEY_DOWN(wxPGCanvas::OnKey)
    EVT_KEY_UP(wxPGCanvas::OnKeyUp)
    EVT_CHAR(wxPGCanvas::OnKey)
END_EVENT_TABLE()


void wxPGCanvas::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPropertyGrid* pg = wxStaticCast(GetParent(), wxPropertyGrid);
    wxASSERT( pg->IsKindOf(CLASSINFO(wxPropertyGrid)) );

    wxPaintDC dc(this);

    // Don't paint after destruction has begun
    if ( !(pg->GetInternalFlags() & wxPG_FL_INITIALIZED) )
        return;

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();

    // Repaint this rectangle
    pg->DrawItems( dc, r.y, r.y + r.height, &r );

    // We assume that the size set when grid is shown
    // is what is desired.
    pg->SetInternalFlag(wxPG_FL_GOOD_SIZE_SET);
}

// -----------------------------------------------------------------------
// wxPropertyGrid
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyGrid, wxScrolledWindow)

BEGIN_EVENT_TABLE(wxPropertyGrid, wxScrolledWindow)
  EVT_IDLE(wxPropertyGrid::OnIdle)
  EVT_MOTION(wxPropertyGrid::OnMouseMoveBottom)
  EVT_PAINT(wxPropertyGrid::OnPaint)
  EVT_SIZE(wxPropertyGrid::OnResize)
  EVT_ENTER_WINDOW(wxPropertyGrid::OnMouseEntry)
  EVT_LEAVE_WINDOW(wxPropertyGrid::OnMouseEntry)
  EVT_MOUSE_CAPTURE_CHANGED(wxPropertyGrid::OnCaptureChange)
  EVT_SCROLLWIN(wxPropertyGrid::OnScrollEvent)
  EVT_CHILD_FOCUS(wxPropertyGrid::OnChildFocusEvent)
  EVT_SET_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_KILL_FOCUS(wxPropertyGrid::OnFocusEvent)
  EVT_SYS_COLOUR_CHANGED(wxPropertyGrid::OnSysColourChanged)
END_EVENT_TABLE()


// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid()
    : wxScrolledWindow()
{
    Init1();
}

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid( wxWindow *parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxChar* name )
    : wxScrolledWindow()
{
    Init1();
    Create(parent,id,pos,size,style,name);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Create( wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             long style,
                             const wxChar* name )
{

    if ( !(style&wxBORDER_MASK) )
        style |= wxSIMPLE_BORDER;

    style |= wxVSCROLL;

#ifdef __WXMSW__
    // This prevents crash under Win2K, but still
    // enables keyboard navigation
    if ( style & wxTAB_TRAVERSAL )
    {
        style &= ~(wxTAB_TRAVERSAL);
        style |= wxWANTS_CHARS;
    }
#else
    if ( style & wxTAB_TRAVERSAL )
        style |= wxWANTS_CHARS;
#endif

    wxScrolledWindow::Create(parent,id,pos,size,style,name);

    Init2();

    return true;
}

// -----------------------------------------------------------------------

//
// Initialize values to defaults
//
void wxPropertyGrid::Init1()
{
    // Register editor classes, if necessary.
    if ( wxPGGlobalVars->m_mapEditorClasses.empty() )
        wxPropertyGrid::RegisterDefaultEditors();

    m_iFlags = 0;
    m_pState = (wxPropertyGridPageState*) NULL;
    m_wndEditor = m_wndEditor2 = (wxWindow*) NULL;
    m_selected = (wxPGProperty*) NULL;
    m_selColumn = -1;
    m_propHover = (wxPGProperty*) NULL;
    m_eventObject = this;
    m_curFocused = (wxWindow*) NULL;
    m_tlwHandler = NULL;
    m_inDoPropertyChanged = 0;
    m_inCommitChangesFromEditor = 0;
    m_inDoSelectProperty = 0;
    m_permanentValidationFailureBehavior = wxPG_VFB_DEFAULT;
    m_dragStatus = 0;
    m_mouseSide = 16;
    m_editorFocused = 0;

    // Set default keys
    AddActionTrigger( wxPG_ACTION_NEXT_PROPERTY, WXK_RIGHT );
    AddActionTrigger( wxPG_ACTION_NEXT_PROPERTY, WXK_DOWN );
    AddActionTrigger( wxPG_ACTION_PREV_PROPERTY, WXK_LEFT );
    AddActionTrigger( wxPG_ACTION_PREV_PROPERTY, WXK_UP );
    AddActionTrigger( wxPG_ACTION_EXPAND_PROPERTY, WXK_RIGHT);
    AddActionTrigger( wxPG_ACTION_COLLAPSE_PROPERTY, WXK_LEFT);
    AddActionTrigger( wxPG_ACTION_CANCEL_EDIT, WXK_ESCAPE );
    AddActionTrigger( wxPG_ACTION_CUT, 'X', wxMOD_CONTROL );
    AddActionTrigger( wxPG_ACTION_CUT, WXK_DELETE, wxMOD_SHIFT );
    AddActionTrigger( wxPG_ACTION_COPY, 'C', wxMOD_CONTROL);
    AddActionTrigger( wxPG_ACTION_COPY, WXK_INSERT, wxMOD_CONTROL );
    AddActionTrigger( wxPG_ACTION_PASTE, 'V', wxMOD_CONTROL );
    AddActionTrigger( wxPG_ACTION_PASTE, WXK_INSERT, wxMOD_SHIFT );
    AddActionTrigger( wxPG_ACTION_PRESS_BUTTON, WXK_DOWN, wxMOD_ALT );
    AddActionTrigger( wxPG_ACTION_PRESS_BUTTON, WXK_F4 );

    m_coloursCustomized = 0;
    m_frozen = 0;

    m_canvas = NULL;

#if wxPG_DOUBLE_BUFFER
    m_doubleBuffer = (wxBitmap*) NULL;
#endif

#ifndef wxPG_ICON_WIDTH
	m_expandbmp = NULL;
	m_collbmp = NULL;
	m_iconWidth = 11;
	m_iconHeight = 11;
#else
    m_iconWidth = wxPG_ICON_WIDTH;
#endif

    m_prevVY = -1;

    m_gutterWidth = wxPG_GUTTER_MIN;
    m_subgroup_extramargin = 10;

    m_lineHeight = 0;

    m_width = m_height = 0;

    m_commonValues.push_back(new wxPGCommonValue(_("Unspecified"), wxPGGlobalVars->m_defaultRenderer) );
    m_cvUnspecified = 0;

    m_chgInfo_changedProperty = NULL;
}

// -----------------------------------------------------------------------

//
// Initialize after parent etc. set
//
void wxPropertyGrid::Init2()
{
    wxASSERT( !(m_iFlags & wxPG_FL_INITIALIZED ) );

#ifdef __WXMAC__
   // Smaller controls on Mac
   SetWindowVariant(wxWINDOW_VARIANT_SMALL);
#endif 

    // Now create state, if one didn't exist already
    // (wxPropertyGridManager might have created it for us).
    if ( !m_pState )
    {
        m_pState = CreateState();
        m_pState->m_pPropGrid = this;
        m_iFlags |= wxPG_FL_CREATEDSTATE;
    }

    if ( !(m_windowStyle & wxPG_SPLITTER_AUTO_CENTER) )
        m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    if ( m_windowStyle & wxPG_HIDE_CATEGORIES )
    {
        m_pState->InitNonCatMode();

        m_pState->m_properties = m_pState->m_abcArray;
    }

    GetClientSize(&m_width,&m_height);

#ifndef wxPG_ICON_WIDTH
    // create two bitmap nodes for drawing
	m_expandbmp = new wxBitmap(expand_xpm);
	m_collbmp = new wxBitmap(collapse_xpm);

	// calculate average font height for bitmap centering

	m_iconWidth = m_expandbmp->GetWidth();
	m_iconHeight = m_expandbmp->GetHeight();
#endif

    m_curcursor = wxCURSOR_ARROW;
    m_cursorSizeWE = new wxCursor( wxCURSOR_SIZEWE );

	// adjust bitmap icon y position so they are centered
    m_vspacing = wxPG_DEFAULT_VSPACING;

    if ( !m_font.Ok() )
    {
        wxFont useFont = wxScrolledWindow::GetFont();
        wxScrolledWindow::SetOwnFont( useFont );
    }
    else
        // This should be otherwise called by SetOwnFont
	    CalculateFontAndBitmapStuff( wxPG_DEFAULT_VSPACING );

    // Add base brush item
    m_arrBgBrushes.Add((void*)new wxPGBrush());

    // Add base colour items
    m_arrFgCols.Add((void*)new wxPGColour());
    m_arrFgCols.Add((void*)new wxPGColour());

    RegainColours();

    // This helps with flicker
    SetBackgroundStyle( wxBG_STYLE_CUSTOM );

    // Hook the TLW
    wxPGTLWHandler* handler = new wxPGTLWHandler(this);
    m_tlp = ::wxGetTopLevelParent(this);
    m_tlwHandler = handler;
    m_tlp->PushEventHandler(handler);

	// set virtual size to this window size
    wxSize wndsize = GetSize();
	SetVirtualSize(wndsize.GetWidth(), wndsize.GetWidth());

    m_timeCreated = ::wxGetLocalTimeMillis();

    m_canvas = new wxPGCanvas();
    m_canvas->Create(this, 1, wxPoint(0, 0), GetClientSize(),
                     (GetWindowStyle() & wxTAB_TRAVERSAL) | wxWANTS_CHARS | wxCLIP_CHILDREN);
    m_canvas->SetBackgroundStyle( wxBG_STYLE_CUSTOM );

    m_iFlags |= wxPG_FL_INITIALIZED;

    m_ncWidth = wndsize.GetWidth();

    // Need to call OnResize handler or size given in constructor/Create
    // will never work.
    wxSizeEvent sizeEvent(wndsize,0);
    OnResize(sizeEvent);
}

// -----------------------------------------------------------------------

wxPropertyGrid::~wxPropertyGrid()
{
    size_t i;

    DoSelectProperty(NULL);

    // This should do prevent things from going too badly wrong
    m_iFlags &= ~(wxPG_FL_INITIALIZED);

    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
        m_canvas->ReleaseMouse();

    wxPGTLWHandler* handler = (wxPGTLWHandler*) m_tlwHandler;
    m_tlp->RemoveEventHandler(handler);
    delete handler;

#ifdef __WXDEBUG__
    if ( IsEditorsValueModified() )
        ::wxMessageBox(wxS("Most recent change in property editor was lost!!!\n\n(if you don't want this to happen, close your frames and dialogs using Close(false).)"),
                       wxS("wxPropertyGrid Debug Warning") );
#endif

#if wxPG_DOUBLE_BUFFER
    if ( m_doubleBuffer )
        delete m_doubleBuffer;
#endif

    //m_selected = (wxPGProperty*) NULL;

    if ( m_iFlags & wxPG_FL_CREATEDSTATE )
        delete m_pState;

    delete m_cursorSizeWE;

#ifndef wxPG_ICON_WIDTH
	delete m_expandbmp;
	delete m_collbmp;
#endif

    // Delete cached text colours.
    for ( i=0; i<m_arrFgCols.size(); i++ )
    {
        delete (wxPGColour*)m_arrFgCols.Item(i);
    }

    // Delete cached brushes.
    for ( i=0; i<m_arrBgBrushes.size(); i++ )
    {
        delete (wxPGBrush*)m_arrBgBrushes.Item(i);
    }

    // Delete common value records
    for ( i=0; i<m_commonValues.size(); i++ )
    {
        delete GetCommonValue(i);
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Destroy()
{
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
        m_canvas->ReleaseMouse();

    return wxScrolledWindow::Destroy();
}

// -----------------------------------------------------------------------

wxPropertyGridPageState* wxPropertyGrid::CreateState() const
{
    return new wxPropertyGridPageState();
}

// -----------------------------------------------------------------------
// wxPropertyGrid overridden wxWindow methods
// -----------------------------------------------------------------------

void wxPropertyGrid::SetWindowStyleFlag( long style )
{
    long old_style = m_windowStyle;

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxASSERT( m_pState );

        if ( !(style & wxPG_HIDE_CATEGORIES) && (old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Enable categories
            EnableCategories( true );
        }
        else if ( (style & wxPG_HIDE_CATEGORIES) && !(old_style & wxPG_HIDE_CATEGORIES) )
        {
        // Disable categories
            EnableCategories( false );
        }
        if ( !(old_style & wxPG_AUTO_SORT) && (style & wxPG_AUTO_SORT) )
        {
            //
            // Autosort enabled
            //
            if ( !m_frozen )
                PrepareAfterItemsAdded();
            else
                m_pState->m_itemsAdded = 1;
        }
    #if wxPG_SUPPORT_TOOLTIPS
        if ( !(old_style & wxPG_TOOLTIPS) && (style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips enabled
            //
            /*
            wxToolTip* tooltip = new wxToolTip ( wxEmptyString );
            SetToolTip ( tooltip );
            tooltip->SetDelay ( wxPG_TOOLTIP_DELAY );
            */
        }
        else if ( (old_style & wxPG_TOOLTIPS) && !(style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips disabled
            //
            m_canvas->SetToolTip( (wxToolTip*) NULL );
        }
    #endif
    }

    wxScrolledWindow::SetWindowStyleFlag ( style );

    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        if ( (old_style & wxPG_HIDE_MARGIN) != (style & wxPG_HIDE_MARGIN) )
        {
            CalculateFontAndBitmapStuff( m_vspacing );
            Refresh();
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Freeze()
{
    if ( !m_frozen )
    {
        wxScrolledWindow::Freeze();
    }
    m_frozen++;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Thaw()
{
    m_frozen--;

    if ( !m_frozen )
    {
        wxScrolledWindow::Thaw();
        RecalculateVirtualSize();
    #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
        m_canvas->Refresh();
    #endif

        // Force property re-selection
        if ( m_selected )
            DoSelectProperty(m_selected, wxPG_SEL_FORCE);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetExtraStyle( long exStyle )
{
    if ( exStyle & wxPG_EX_NATIVE_DOUBLE_BUFFERING )
    {
#if defined(__WXMSW__)

        /*
        // Don't use WS_EX_COMPOSITED just now.
        HWND hWnd;

        if ( m_iFlags & wxPG_FL_IN_MANAGER )
            hWnd = (HWND)GetParent()->GetHWND();
        else
            hWnd = (HWND)GetHWND();

        ::SetWindowLong( hWnd, GWL_EXSTYLE,
                         ::GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_COMPOSITED );
        */

//#elif defined(__WXGTK20__)
#endif
        // Only apply wxPG_EX_NATIVE_DOUBLE_BUFFERING if the window
        // truly was double-buffered.
        if ( !this->IsDoubleBuffered() )
        {
            exStyle &= ~(wxPG_EX_NATIVE_DOUBLE_BUFFERING);
        }
        else
        {
        #if wxPG_DOUBLE_BUFFER
            delete m_doubleBuffer;
            m_doubleBuffer = NULL;
        #endif
        }
    }

    wxScrolledWindow::SetExtraStyle( exStyle );

    if ( exStyle & wxPG_EX_INIT_NOCAT )
        m_pState->InitNonCatMode();

    if ( exStyle & wxPG_EX_HELP_AS_TOOLTIPS )
        m_windowStyle |= wxPG_TOOLTIPS;

    // Set global style
    wxPGGlobalVars->m_extraStyle = exStyle;
}

// -----------------------------------------------------------------------

// returns the best acceptable minimal size
wxSize wxPropertyGrid::DoGetBestSize() const
{
    int hei = 15;
    if ( m_lineHeight > hei )
        hei = m_lineHeight;
    wxSize sz = wxSize( 60, hei+40 );

    CacheBestSize(sz);
    return sz;
}

// -----------------------------------------------------------------------
// wxPropertyGrid Font and Colour Methods
// -----------------------------------------------------------------------

void wxPropertyGrid::CalculateFontAndBitmapStuff( int vspacing )
{
	int x = 0, y = 0;

    m_captionFont = wxScrolledWindow::GetFont();

	GetTextExtent(wxS("jG"), &x, &y, 0, 0, &m_captionFont);
    m_subgroup_extramargin = x + (x/2);
	m_fontHeight = y;

#if wxPG_USE_RENDERER_NATIVE
    m_iconWidth = wxPG_ICON_WIDTH;
#elif wxPG_ICON_WIDTH
    // scale icon
    m_iconWidth = (m_fontHeight * wxPG_ICON_WIDTH) / 13;
    if ( m_iconWidth < 5 ) m_iconWidth = 5;
    else if ( !(m_iconWidth & 0x01) ) m_iconWidth++; // must be odd

#endif

    m_gutterWidth = m_iconWidth / wxPG_GUTTER_DIV;
    if ( m_gutterWidth < wxPG_GUTTER_MIN )
        m_gutterWidth = wxPG_GUTTER_MIN;

    int vdiv = 6;
    if ( vspacing <= 1 ) vdiv = 12;
    else if ( vspacing >= 3 ) vdiv = 3;

    m_spacingy = m_fontHeight / vdiv;
    if ( m_spacingy < wxPG_YSPACING_MIN )
        m_spacingy = wxPG_YSPACING_MIN;

    m_marginWidth = 0;
    if ( !(m_windowStyle & wxPG_HIDE_MARGIN) )
        m_marginWidth = m_gutterWidth*2 + m_iconWidth;

    m_captionFont.SetWeight(wxBOLD);
	GetTextExtent(wxS("jG"), &x, &y, 0, 0, &m_captionFont);

    m_lineHeight = m_fontHeight+(2*m_spacingy)+1;

    // button spacing
    m_buttonSpacingY = (m_lineHeight - m_iconHeight) / 2;
    if ( m_buttonSpacingY < 0 ) m_buttonSpacingY = 0;

    if ( m_pState )
        m_pState->CalculateFontAndBitmapStuff(vspacing);

    if ( m_iFlags & wxPG_FL_INITIALIZED )
        RecalculateVirtualSize();

    InvalidateBestSize();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnSysColourChanged( wxSysColourChangedEvent &WXUNUSED(event) )
{
    RegainColours();
    Refresh();
}

// -----------------------------------------------------------------------

static wxColour wxPGAdjustColour(const wxColour& src, int ra,
                                 int ga = 1000, int ba = 1000,
                                 bool forceDifferent = false)
{
    if ( ga >= 1000 )
        ga = ra;
    if ( ba >= 1000 )
        ba = ra;

    // Recursion guard (allow 2 max)
    static int isinside = 0;
    isinside++;
    wxCHECK_MSG( isinside < 3,
                 *wxBLACK,
                 wxT("wxPGAdjustColour should not be recursively called more than once") );

    wxColour dst;

    int r = src.Red();
    int g = src.Green();
    int b = src.Blue();
    int r2 = r + ra;
    if ( r2>255 ) r2 = 255;
    else if ( r2<0) r2 = 0;
    int g2 = g + ga;
    if ( g2>255 ) g2 = 255;
    else if ( g2<0) g2 = 0;
    int b2 = b + ba;
    if ( b2>255 ) b2 = 255;
    else if ( b2<0) b2 = 0;

    // Make sure they are somewhat different
    if ( forceDifferent && (abs((r+g+b)-(r2+g2+b2)) < abs(ra/2)) )
        dst = wxPGAdjustColour(src,-(ra*2));
    else
        dst = wxColour(r2,g2,b2);

    // Recursion guard (allow 2 max)
    isinside--;

    return dst;
}


static int wxPGGetColAvg( const wxColour& col )
{
    return (col.Red() + col.Green() + col.Blue()) / 3;
}


void wxPropertyGrid::RegainColours()
{
    wxColour def_bgcol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );

    if ( !(m_coloursCustomized & 0x0002) )
    {
        wxColour col = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );

        // Make sure colour is dark enough
    #ifdef __WXGTK__
        int colDec = wxPGGetColAvg(col) - 230;
    #else
        int colDec = wxPGGetColAvg(col) - 200;
    #endif
        if ( colDec > 0 )
            m_colCapBack = wxPGAdjustColour(col,-colDec);
        else
            m_colCapBack = col;
    }

    if ( !(m_coloursCustomized & 0x0001) )
        m_colMargin = m_colCapBack;

    if ( !(m_coloursCustomized & 0x0004) )
    {
    #ifdef __WXGTK__
        int colDec = -90;
    #else
        int colDec = -72;
    #endif
        wxColour capForeCol = wxPGAdjustColour(m_colCapBack,colDec,5000,5000,true);
        m_colCapFore = capForeCol;

        // Set the cached colour as well.
        ((wxPGColour*)m_arrFgCols.Item(1))->SetColour2(capForeCol);
    }

    if ( !(m_coloursCustomized & 0x0008) )
    {
        wxColour bgCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );
        m_colPropBack = bgCol;

        // Set the cached brush as well.
        ((wxPGBrush*)m_arrBgBrushes.Item(0))->SetColour2(bgCol);
    }

    if ( !(m_coloursCustomized & 0x0010) )
    {
        wxColour fgCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
        m_colPropFore = fgCol;

        // Set the cached colour as well.
        ((wxPGColour*)m_arrFgCols.Item(0))->SetColour2(fgCol);
    }

    if ( !(m_coloursCustomized & 0x0020) )
        m_colSelBack = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );

    if ( !(m_coloursCustomized & 0x0040) )
        m_colSelFore = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT );

    if ( !(m_coloursCustomized & 0x0080) )
        m_colLine = m_colCapBack;

    if ( !(m_coloursCustomized & 0x0100) )
        m_colDisPropFore = m_colCapFore;

    m_colEmptySpace = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ResetColours()
{
    m_coloursCustomized = 0;

    RegainColours();

    Refresh();
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::SetFont( const wxFont& font )
{
    // Must disable active editor.
    if ( m_selected )
    {
        bool selRes = ClearSelection();
        wxPG_CHECK_MSG_DBG( selRes,
                            false,
                            wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    // TODO: Following code is disabled with wxMac because
    //   it is reported to fail. I (JMS) cannot debug it
    //   personally right now.
    // CS: should be fixed now, leaving old code in just in case, TODO: REMOVE
#if 1 // !defined(__WXMAC__)
    bool res = wxScrolledWindow::SetFont( font );
    if ( res )
    {
        CalculateFontAndBitmapStuff( m_vspacing );

        if ( m_pState )
            m_pState->CalculateFontAndBitmapStuff(m_vspacing);

        Refresh();
    }

    return res;
#else
    // ** wxMAC Only **
    // TODO: Remove after SetFont crash fixed.
    if ( m_iFlags & wxPG_FL_INITIALIZED )
    {
        wxLogDebug(wxT("WARNING: propGrid.cpp: wxPropertyGrid::SetFont has been disabled on wxMac since there has been crash reported in it. If you are willing to debug the cause, replace line '#if !defined(__WXMAC__)' with line '#if 1' in wxPropertyGrid::SetFont."));
    }
    return false;
#endif
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetLineColour( const wxColour& col )
{
    m_colLine = col;
    m_coloursCustomized |= 0x80;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetMarginColour( const wxColour& col )
{
    m_colMargin = col;
    m_coloursCustomized |= 0x01;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellBackgroundColour( const wxColour& col )
{
    m_colPropBack = col;
    m_coloursCustomized |= 0x08;

    // Set the cached brush as well.
    ((wxPGBrush*)m_arrBgBrushes.Item(0))->SetColour2(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellTextColour( const wxColour& col )
{
    m_colPropFore = col;
    m_coloursCustomized |= 0x10;

    // Set the cached colour as well.
    ((wxPGColour*)m_arrFgCols.Item(0))->SetColour2(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetEmptySpaceColour( const wxColour& col )
{
    m_colEmptySpace = col;

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellDisabledTextColour( const wxColour& col )
{
    m_colDisPropFore = col;
    m_coloursCustomized |= 0x100;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionBackgroundColour( const wxColour& col )
{
    m_colSelBack = col;
    m_coloursCustomized |= 0x20;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetSelectionTextColour( const wxColour& col )
{
    m_colSelFore = col;
    m_coloursCustomized |= 0x40;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionBackgroundColour( const wxColour& col )
{
    m_colCapBack = col;
    m_coloursCustomized |= 0x02;
    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionTextColour( const wxColour& col )
{
    m_colCapFore = col;
    m_coloursCustomized |= 0x04;

    // Set the cached colour as well.
    ((wxPGColour*)m_arrFgCols.Item(1))->SetColour2(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetBackgroundColourIndex( wxPGProperty* p, int index )
{
    unsigned char ind = index;

    p->m_bgColIndex = ind;

    unsigned int i;
    for ( i=0; i<p->GetChildCount(); i++ )
        SetBackgroundColourIndex(p->Item(i),index);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyBackgroundColour( wxPGPropArg id, const wxColour& colour )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    size_t i;
    int colInd = -1;

    long colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());

    // As it is most likely that the previous colour is used, start comparison
    // from the end.
    for ( i=(m_arrBgBrushes.size()-1); i>0; i-- )
    {
        if ( ((wxPGBrush*)m_arrBgBrushes.Item(i))->GetColourAsLong() == colAsLong )
        {
            colInd = i;
            break;
        }
    }

    if ( colInd < 0 )
    {
        colInd = m_arrBgBrushes.size();
        wxCHECK_RET( colInd < 256, wxT("wxPropertyGrid: Warning - Only 255 different property background colours allowed.") );
        m_arrBgBrushes.Add( (void*)new wxPGBrush(colour) );
    }

    // Set indexes
    SetBackgroundColourIndex(p,colInd);

    // If this was on a visible grid, then draw it.
    DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

wxColour wxPropertyGrid::GetPropertyBackgroundColour( wxPGPropArg id ) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxColour())

    return ((wxPGBrush*)m_arrBgBrushes.Item(p->m_bgColIndex))->GetColour();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetTextColourIndex( wxPGProperty* p, int index, int flags )
{
    unsigned char ind = index;

    p->m_fgColIndex = ind;

    if ( p->GetChildCount() && (flags & wxPG_RECURSE) )
    {
        unsigned int i;
        for ( i=0; i<p->GetChildCount(); i++ )
            SetTextColourIndex( p->Item(i), index, flags );
    }
}

// -----------------------------------------------------------------------

int wxPropertyGrid::CacheColour( const wxColour& colour )
{
    unsigned int i;
    int colInd = -1;

    long colAsLong = wxPG_COLOUR(colour.Red(),colour.Green(),colour.Blue());

    // As it is most likely that the previous colour is used, start comparison
    // from the end.
    for ( i=(m_arrFgCols.size()-1); i>0; i-- )
    {
        if ( ((wxPGColour*)m_arrFgCols.Item(i))->GetColourAsLong() == colAsLong )
        {
            colInd = i;
            break;
        }
    }

    if ( colInd < 0 )
    {
        colInd = m_arrFgCols.size();
        wxCHECK_MSG( colInd < 256, 0, wxT("wxPropertyGrid: Warning - Only 255 different property foreground colours allowed.") );
        m_arrFgCols.Add( (void*)new wxPGColour(colour) );
    }

    return colInd;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetPropertyTextColour( wxPGPropArg id, const wxColour& colour,
                                            bool recursively )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    if ( p->IsCategory() )
    {
        wxPropertyCategory* cat = (wxPropertyCategory*) p;
        cat->SetTextColIndex(CacheColour(colour));
    }

    // Set indexes
    int flags = 0;
    if ( recursively )
        flags |= wxPG_RECURSE;
    SetTextColourIndex(p, CacheColour(colour), flags);

    DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

wxColour wxPropertyGrid::GetPropertyTextColour( wxPGPropArg id ) const
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(wxColour())

    return wxColour(*((wxPGColour*)m_arrFgCols.Item(p->m_fgColIndex)));
}

void wxPropertyGrid::SetPropertyColoursToDefault( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    SetBackgroundColourIndex( p, 0 );
    SetTextColourIndex( p, 0, wxPG_RECURSE );

    if ( p->IsCategory() )
    {
        wxPropertyCategory* cat = (wxPropertyCategory*) p;
        cat->SetTextColIndex(1);
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid property adding and removal
// -----------------------------------------------------------------------

void wxPropertyGrid::PrepareAfterItemsAdded()
{
    if ( !m_pState || !m_pState->m_itemsAdded ) return;

    m_pState->m_itemsAdded = 0;

    if ( m_windowStyle & wxPG_AUTO_SORT )
        Sort();

    RecalculateVirtualSize();
}

// -----------------------------------------------------------------------
// wxPropertyGrid property value setting and getting
// -----------------------------------------------------------------------

void wxPropertyGrid::DoSetPropertyValueUnspecified( wxPGProperty* p )
{
    m_pState->DoSetPropertyValueUnspecified(p);
    DrawItemAndChildren(p);

    wxPGProperty* parent = p->GetParent();
    while ( (parent->GetFlags() & wxPG_PROP_PARENTAL_FLAGS) == wxPG_PROP_MISC_PARENT )
    {
        DrawItem(parent);
        parent = parent->GetParent();
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid property operations
// -----------------------------------------------------------------------

bool wxPropertyGrid::EnsureVisible( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    Update();

    bool changed = false;

    // Is it inside collapsed section?
    if ( !p->IsVisible() )
    {
        // expand parents
        wxPGProperty* parent = p->GetParent();
        wxPGProperty* grandparent = parent->GetParent();

        if ( grandparent && grandparent != m_pState->m_properties )
            Expand( grandparent );

        Expand( parent );
        changed = true;
    }

    // Need to scroll?
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;

    int y = p->GetY();

    if ( y < vy )
    {
        Scroll(vx, y/wxPG_PIXELS_PER_UNIT );
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = true;
    }
    else if ( (y+m_lineHeight) > (vy+m_height) )
    {
        Scroll(vx, (y-m_height+(m_lineHeight*2))/wxPG_PIXELS_PER_UNIT );
        m_iFlags |= wxPG_FL_SCROLLED;
        changed = true;
    }

    if ( changed )
        DrawItems( p, p );

    return changed;
}

// -----------------------------------------------------------------------
// wxPropertyGrid helper methods called by properties
// -----------------------------------------------------------------------

// Control font changer helper.
void wxPropertyGrid::SetCurControlBoldFont()
{
    wxASSERT( m_wndEditor );
    m_wndEditor->SetFont( m_captionFont );
}

// -----------------------------------------------------------------------

wxPoint wxPropertyGrid::GetGoodEditorDialogPosition( wxPGProperty* p,
                                                     const wxSize& sz )
{
#if wxPG_SMALL_SCREEN
    // On small-screen devices, always show dialogs with default position and size.
    return wxDefaultPosition;
#else
    int splitterX = GetSplitterPosition();
    int x = splitterX;
    int y = p->GetY();

    wxCHECK_MSG( y >= 0, wxPoint(-1,-1), wxT("invalid y?") );

    ImprovedClientToScreen( &x, &y );

    int sw = wxSystemSettings::GetMetric( ::wxSYS_SCREEN_X );
    int sh = wxSystemSettings::GetMetric( ::wxSYS_SCREEN_Y );

    int new_x;
    int new_y;

    if ( x > (sw/2) )
        // left
        new_x = x + (m_width-splitterX) - sz.x;
    else
        // right
        new_x = x;

    if ( y > (sh/2) )
        // above
        new_y = y - sz.y;
    else
        // below
        new_y = y + m_lineHeight;

    return wxPoint(new_x,new_y);
#endif
}

// -----------------------------------------------------------------------

wxString& wxPropertyGrid::ExpandEscapeSequences( wxString& dst_str, wxString& src_str )
{
    if ( src_str.length() == 0 )
    {
        dst_str = src_str;
        return src_str;
    }

    bool prev_is_slash = false;

    wxString::const_iterator i = src_str.begin();

    dst_str.clear();

    for ( ; i != src_str.end(); i++ )
    {
        wxUniChar a = *i;

        if ( a != wxS('\\') )
        {
            if ( !prev_is_slash )
            {
                dst_str << a;
            }
            else
            {
                if ( a == wxS('n') )
                {
            #ifdef __WXMSW__
                    dst_str << wxS('\n');
            #else
                    dst_str << wxS('\n');
            #endif
                }
                else if ( a == wxS('t') )
                    dst_str << wxS('\t');
                else
                    dst_str << a;
            }
            prev_is_slash = false;
        }
        else
        {
            if ( prev_is_slash )
            {
                dst_str << wxS('\\');
                prev_is_slash = false;
            }
            else
            {
                prev_is_slash = true;
            }
        }
    }
    return dst_str;
}

// -----------------------------------------------------------------------

wxString& wxPropertyGrid::CreateEscapeSequences( wxString& dst_str, wxString& src_str )
{
    if ( src_str.length() == 0 )
    {
        dst_str = src_str;
        return src_str;
    }

    wxString::const_iterator i = src_str.begin();
    wxUniChar prev_a = wxS('\0');

    dst_str.clear();

    for ( ; i != src_str.end(); i++ )
    {
        wxChar a = *i;

        if ( a >= wxS(' ') )
        {
            // This surely is not something that requires an escape sequence.
            dst_str << a;
        }
        else
        {
            // This might need...
            if ( a == wxS('\r')  )
            {
                // DOS style line end.
                // Already taken care below
            }
            else if ( a == wxS('\n') )
                // UNIX style line end.
                dst_str << wxS("\\n");
            else if ( a == wxS('\t') )
                // Tab.
                dst_str << wxS('\t');
            else
            {
                //wxLogDebug(wxT("WARNING: Could not create escape sequence for character #%i"),(int)a);
                dst_str << a;
            }
        }

        prev_a = a;
    }
    return dst_str;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGrid::DoGetItemAtY( int y ) const
{
    // Outside?
    if ( y < 0 )
        return (wxPGProperty*) NULL;

    unsigned int a = 0;
    return m_pState->m_properties->GetItemAtY(y, m_lineHeight, &a);
}

// -----------------------------------------------------------------------
// wxPropertyGrid graphics related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();

    dc.SetPen(m_colEmptySpace);
    dc.SetBrush(m_colEmptySpace);
    dc.DrawRectangle(r);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawExpanderButton( wxDC& dc, const wxRect& rect,
                                         wxPGProperty* property ) const
{
    // Prepare rectangle to be used
    wxRect r(rect);
    r.x += m_gutterWidth; r.y += m_buttonSpacingY;
    r.width = m_iconWidth; r.height = m_iconHeight;

#if (wxPG_USE_RENDERER_NATIVE)
    //
#elif wxPG_ICON_WIDTH
    // Drawing expand/collapse button manually
    dc.SetPen(m_colPropFore);
    if ( property->IsCategory() )
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
    else
        dc.SetBrush(m_colPropBack);

    dc.DrawRectangle( r );
    int _y = r.y+(m_iconWidth/2);
    dc.DrawLine(r.x+2,_y,r.x+m_iconWidth-2,_y);
#else
    wxBitmap* bmp;
#endif

    if ( property->IsExpanded() )
    {
    // wxRenderer functions are non-mutating in nature, so it
    // should be safe to cast "const wxPropertyGrid*" to "wxWindow*".
    // Hopefully this does not cause problems.
    #if (wxPG_USE_RENDERER_NATIVE)
        wxRendererNative::Get().DrawTreeItemButton(
                (wxWindow*)this,
                dc,
                r,
                wxCONTROL_EXPANDED
            );
    #elif wxPG_ICON_WIDTH
        //
    #else
        bmp = m_collbmp;
    #endif

    }
    else
    {
    #if (wxPG_USE_RENDERER_NATIVE)
        wxRendererNative::Get().DrawTreeItemButton(
                (wxWindow*)this,
                dc,
                r,
                0
            );
    #elif wxPG_ICON_WIDTH
        int _x = r.x+(m_iconWidth/2);
        dc.DrawLine(_x,r.y+2,_x,r.y+m_iconWidth-2);
    #else
        bmp = m_expandbmp;
    #endif
    }

#if (wxPG_USE_RENDERER_NATIVE)
    //
#elif wxPG_ICON_WIDTH
    //
#else
    dc.DrawBitmap( *bmp, r.x, r.y, true );
#endif
}

// -----------------------------------------------------------------------

//
// This is the one called by OnPaint event handler and others.
// topy and bottomy are already unscrolled (ie. physical)
//
void wxPropertyGrid::DrawItems( wxDC& dc,
                                unsigned int topy,
                                unsigned int bottomy,
                                const wxRect* clipRect )
{
    if ( m_frozen || m_height < 1 || bottomy < topy || !m_pState ) return;

    m_pState->EnsureVirtualHeight();

    wxRect tempClipRect;
    if ( !clipRect )
    {
        tempClipRect = wxRect(0,topy,m_pState->m_width,bottomy);
        clipRect = &tempClipRect;
    }

    // items added check
    if ( m_pState->m_itemsAdded ) PrepareAfterItemsAdded();

    int paintFinishY = 0;

    if ( m_pState->m_properties->GetChildCount() > 0 )
    {
        wxDC* dcPtr = &dc;
        bool isBuffered = false;

    #if wxPG_DOUBLE_BUFFER
        wxMemoryDC* bufferDC = NULL;

        if ( !(GetExtraStyle() & wxPG_EX_NATIVE_DOUBLE_BUFFERING) )
        {
            if ( !m_doubleBuffer )
            {
                paintFinishY = clipRect->y;
                dcPtr = NULL;
            }
            else
            {
                bufferDC = new wxMemoryDC();

                // If nothing was changed, then just copy from double-buffer
                bufferDC->SelectObject( *m_doubleBuffer );
                dcPtr = bufferDC;

                isBuffered = true;
            }
        }
    #endif

        if ( dcPtr )
        {
            dc.SetClippingRegion( *clipRect );
            paintFinishY = DoDrawItems( *dcPtr, NULL, NULL, clipRect, isBuffered );
        }

    #if wxPG_DOUBLE_BUFFER
        if ( bufferDC )
        {
            dc.Blit( clipRect->x, clipRect->y, clipRect->width, clipRect->height,
                bufferDC, 0, 0, wxCOPY );
            dc.DestroyClippingRegion(); // Is this really necessary?
            delete bufferDC;
        }
    #endif
    }

    // Clear area beyond bottomY?
    if ( paintFinishY < (clipRect->y+clipRect->height) )
    {
        dc.SetPen(m_colEmptySpace);
        dc.SetBrush(m_colEmptySpace);
        dc.DrawRectangle( 0, paintFinishY, m_width, (clipRect->y+clipRect->height) );
    }
}

// -----------------------------------------------------------------------

int wxPropertyGrid::DoDrawItems( wxDC& dc,
                                 const wxPGProperty* firstItem,
                                 const wxPGProperty* lastItem,
                                 const wxRect* clipRect,
                                 bool isBuffered ) const
{
    // TODO: This should somehow be eliminated.
    wxRect tempClipRect;
    if ( !clipRect )
    {
        wxASSERT(firstItem);
        wxASSERT(lastItem);
        tempClipRect = GetPropertyRect(firstItem, lastItem);
        clipRect = &tempClipRect;
    }

    if ( !firstItem )
        firstItem = DoGetItemAtY(clipRect->y);

    if ( !lastItem )
    {
        lastItem = DoGetItemAtY(clipRect->y+clipRect->height-1);
        if ( !lastItem )
            lastItem = GetLastItem( wxPG_ITERATE_VISIBLE );
    }

    if ( m_frozen || m_height < 1 || firstItem == NULL )
        return clipRect->y;

    wxCHECK_MSG( !m_pState->m_itemsAdded, clipRect->y, wxT("no items added") );
    wxASSERT( m_pState->m_properties->GetChildCount() );

    int lh = m_lineHeight;

    int firstItemTopY;
    int lastItemBottomY;

    firstItemTopY = clipRect->y;
    lastItemBottomY = clipRect->y + clipRect->height;

    // Align y coordinates to item boundaries
    firstItemTopY -= firstItemTopY % lh;
    lastItemBottomY += lh - (lastItemBottomY % lh);
    lastItemBottomY -= 1;

    // Entire range outside scrolled, visible area?
    if ( firstItemTopY >= (int)m_pState->GetVirtualHeight() || lastItemBottomY <= 0 )
        return clipRect->y;

    wxCHECK_MSG( firstItemTopY < lastItemBottomY, clipRect->y, wxT("invalid y values") );


    /*
    wxLogDebug(wxT("  -> DoDrawItems ( \"%s\" -> \"%s\", height=%i (ch=%i), clipRect = 0x%lX )"),
        firstItem->GetLabel().c_str(),
        lastItem->GetLabel().c_str(),
        (int)(lastItemBottomY - firstItemTopY),
        (int)m_height,
        (unsigned long)clipRect );
    */

    wxRect r;

    long windowStyle = m_windowStyle;

    int xRelMod = 0;
    int yRelMod = 0;

    //
    // With wxPG_DOUBLE_BUFFER, do double buffering
    // - buffer's y = 0, so align cliprect and coordinates to that
    //
#if wxPG_DOUBLE_BUFFER

    wxRect cr2;

    if ( isBuffered )
    {
        xRelMod = clipRect->x;
        yRelMod = clipRect->y;

        //
        // clipRect conversion
        if ( clipRect )
        {
            cr2 = *clipRect;
            cr2.x -= xRelMod;
            cr2.y -= yRelMod;
            clipRect = &cr2;
        }
        firstItemTopY -= yRelMod;
        lastItemBottomY -= yRelMod;
    }
#else
    wxUnusedVar(isBuffered);
#endif

    int x = m_marginWidth - xRelMod;

    const wxFont& normalfont = m_font;

    bool reallyFocused = (m_iFlags & wxPG_FL_FOCUSED) ? true : false;

    bool isEnabled = IsEnabled();

    //
    // Prepare some pens and brushes that are often changed to.
    //

    wxBrush marginBrush(m_colMargin);
    wxPen marginPen(m_colMargin);
    wxBrush capbgbrush(m_colCapBack,wxSOLID);
    wxPen linepen(m_colLine,1,wxSOLID);

    // pen that has same colour as text
    wxPen outlinepen(m_colPropFore,1,wxSOLID);

    //
    // Clear margin with background colour
    //
    dc.SetBrush( marginBrush );
    if ( !(windowStyle & wxPG_HIDE_MARGIN) )
    {
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(-1-xRelMod,firstItemTopY-1,x+2,lastItemBottomY-firstItemTopY+2);
    }

    const wxPGProperty* selected = m_selected;
    const wxPropertyGridPageState* state = m_pState;

#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    bool wasSelectedPainted = false;
#endif

    // TODO: Only render columns that are within clipping region.

    dc.SetFont(normalfont);

    wxPropertyGridConstIterator it( state, wxPG_ITERATE_VISIBLE, firstItem );
    int endScanBottomY = lastItemBottomY + lh;
    int y = firstItemTopY;

    //
    // Pregenerate list of visible properties.
    wxArrayPGProperty visPropArray;
    visPropArray.reserve((m_height/m_lineHeight)+6);

    for ( ; !it.AtEnd(); it.Next() )
    {
        const wxPGProperty* p = *it;

        if ( !p->HasFlag(wxPG_PROP_HIDDEN) )
        {
            visPropArray.push_back((wxPGProperty*)p);

            if ( y > endScanBottomY )
                break;

            y += lh;
        }
    }

    visPropArray.push_back(NULL);

    wxPGProperty* nextP = visPropArray[0];

    int gridWidth = state->m_width;

    y = firstItemTopY;
    for ( unsigned int arrInd=1;
          nextP && y <= lastItemBottomY;
          arrInd++ )
    {
        wxPGProperty* p = nextP;
        nextP = visPropArray[arrInd];

        int rowHeight = m_fontHeight+(m_spacingy*2)+1;
        int textMarginHere = x;
        int renderFlags = wxPGCellRenderer::Control;

        int greyDepth = m_marginWidth;
        if ( !(windowStyle & wxPG_HIDE_CATEGORIES) )
            greyDepth = (((int)p->m_depthBgCol)-1) * m_subgroup_extramargin + m_marginWidth;

        int greyDepthX = greyDepth - xRelMod;

        // Use basic depth if in non-categoric mode and parent is base array.
        if ( !(windowStyle & wxPG_HIDE_CATEGORIES) || p->GetParent() != m_pState->m_properties )
        {
            textMarginHere += ((unsigned int)((p->m_depth-1)*m_subgroup_extramargin));
        }

        // Paint margin area
        dc.SetBrush(marginBrush);
        dc.SetPen(marginPen);
        dc.DrawRectangle( -xRelMod, y, greyDepth, lh );

        dc.SetPen( linepen );

        int y2 = y + lh;

        // Margin Edge
        dc.DrawLine( greyDepthX, y, greyDepthX, y2 );

        // Splitters
        unsigned int si;
        int sx = x;

        for ( si=0; si<state->m_colWidths.size(); si++ )
        {
            sx += state->m_colWidths[si];
            dc.DrawLine( sx, y, sx, y2 );
        }

        // Horizontal Line, below
        //   (not if both this and next is category caption)
        if ( p->IsCategory() &&
             nextP && nextP->IsCategory() )
            dc.SetPen(m_colCapBack);

        dc.DrawLine( greyDepthX, y2-1, gridWidth-xRelMod, y2-1 );

        if ( p == selected )
        {
            renderFlags |= wxPGCellRenderer::Selected;
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
            wasSelectedPainted = true;
#endif
        }

        wxColour rowBgCol;
        wxColour rowFgCol;
        wxBrush rowBgBrush;

        if ( p->IsCategory() )
        {
            if ( p->m_fgColIndex == 0 )
                rowFgCol = m_colCapFore;
            else
                rowFgCol = *(wxPGColour*)m_arrFgCols[p->m_fgColIndex];
            rowBgBrush = wxBrush(m_colCapBack);
        }
        else if ( p != selected )
        {
            // Disabled may get different colour.
            if ( !p->IsEnabled() )
                rowFgCol = m_colDisPropFore;
            else
                rowFgCol = *(wxPGColour*)m_arrFgCols[p->m_fgColIndex];

            rowBgBrush = *(wxPGBrush*)m_arrBgBrushes[p->m_bgColIndex];
        }
        else
        {
            // Selected gets different colour.
            if ( reallyFocused )
            {
                rowFgCol = m_colSelFore;
                rowBgBrush = wxBrush(m_colSelBack);
            }
            else if ( isEnabled )
            {
                rowFgCol = *(wxPGColour*)m_arrFgCols[p->m_fgColIndex];
                rowBgBrush = marginBrush;
            }
            else
            {
                rowFgCol = m_colDisPropFore;
                rowBgBrush = wxBrush(m_colSelBack);
            }
        }

        bool fontChanged = false;

        wxRect butRect( ((p->m_depth - 1) * m_subgroup_extramargin) - xRelMod,
                        y,
                        m_marginWidth,
                        lh );

        if ( p->IsCategory() )
        {
            // Captions are all cells merged as one
            dc.SetFont(m_captionFont);
            fontChanged = true;
            wxRect cellRect(greyDepthX, y, gridWidth - greyDepth + 2, rowHeight-1 );

            dc.SetBrush(rowBgBrush);
            dc.SetPen(rowBgBrush.GetColour());
            dc.SetTextForeground(rowFgCol);

            dc.DrawRectangle(cellRect);

            // Foreground
            wxPGCellRenderer* renderer = p->GetCellRenderer(0);
            renderer->Render( dc, cellRect, this, p, 0, -1, renderFlags );

            // Tree Item Button
            if ( !HasFlag(wxPG_HIDE_MARGIN) && p->HasVisibleChildren() )
                DrawExpanderButton( dc, butRect, p );
        }
        else
        {
            if ( p->m_flags & wxPG_PROP_MODIFIED && (windowStyle & wxPG_BOLD_MODIFIED) )
            {
                dc.SetFont(m_captionFont);
                fontChanged = true;
            }

            unsigned int ci;
            int cellX = x + 1;
            int nextCellWidth = state->m_colWidths[0];
            wxRect cellRect(greyDepthX+1, y, 0, rowHeight-1);
            int textXAdd = textMarginHere - greyDepthX;

            for ( ci=0; ci<state->m_colWidths.size(); ci++ )
            {
                cellRect.width = nextCellWidth - 1;

                bool ctrlCell = false;

                // Background
                if ( p == selected && m_wndEditor && ci == 1 )
                {
                    wxColour editorBgCol = GetEditorControl()->GetBackgroundColour();
                    dc.SetBrush(editorBgCol);
                    dc.SetPen(editorBgCol);
                    dc.SetTextForeground(m_colPropFore);

                    if ( m_dragStatus == 0 && !(m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE) )
                        ctrlCell = true;
                }
                else
                {
                    dc.SetBrush(rowBgBrush);
                    dc.SetPen(rowBgBrush.GetColour());
                    dc.SetTextForeground(rowFgCol);
                }

                dc.DrawRectangle(cellRect);

                // Tree Item Button
                if ( ci == 0 && !HasFlag(wxPG_HIDE_MARGIN) && p->HasVisibleChildren() )
                    DrawExpanderButton( dc, butRect, p );

                dc.SetClippingRegion(cellRect);

                cellRect.x += textXAdd;
                cellRect.width -= textXAdd;

                // Foreground
                if ( !ctrlCell )
                {
                    wxPGCellRenderer* renderer;
                    int cmnVal = p->GetCommonValue();
                    if ( cmnVal == -1 || ci != 1 )
                    {
                        renderer = p->GetCellRenderer(ci);
                        renderer->Render( dc, cellRect, this, p, ci, -1, renderFlags );
                    }
                    else
                    {
                        renderer = GetCommonValue(cmnVal)->GetRenderer();
                        renderer->Render( dc, cellRect, this, p, ci, -1, renderFlags );
                    }
                }

                cellX += state->m_colWidths[ci];
                if ( ci < (state->m_colWidths.size()-1) )
                    nextCellWidth = state->m_colWidths[ci+1];
                cellRect.x = cellX; 
                dc.DestroyClippingRegion(); // Is this really necessary?
                textXAdd = 0;
            }
        }

        if ( fontChanged )
            dc.SetFont(normalfont);

        y += rowHeight;
    }

    // Refresh editor controls (seems not needed on msw)
    // NOTE: This code is mandatory for GTK!
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    if ( wasSelectedPainted )
    {
        if ( m_wndEditor )
            m_wndEditor->Refresh();
        if ( m_wndEditor2 )
            m_wndEditor2->Refresh();
    }
#endif

    return y + yRelMod;
}

// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetPropertyRect( const wxPGProperty* p1, const wxPGProperty* p2 ) const
{
    wxRect r;

    if ( m_width < 10 || m_height < 10 ||
         !m_pState->m_properties->GetChildCount() ||
         p1 == (wxPGProperty*) NULL )
        return wxRect(0,0,0,0);

    int vy = 0;

    //
    // Return rect which encloses the given property range

    int visTop = p1->GetY();
    int visBottom;
    if ( p2 )
        visBottom = p2->GetY() + m_lineHeight;
    else
        visBottom = m_height + visTop;

    // If seleced property is inside the range, we'll extend the range to include
    // control's size.
    wxPGProperty* selected = m_selected;
    if ( selected )
    {
        int selectedY = selected->GetY();
        if ( selectedY >= visTop && selectedY < visBottom )
        {
            wxWindow* editor = GetEditorControl();
            if ( editor )
            {
                int visBottom2 = selectedY + editor->GetSize().y;
                if ( visBottom2 > visBottom )
                    visBottom = visBottom2;
            }
        }
    }

    return wxRect(0,visTop-vy,m_pState->m_width,visBottom-visTop);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItems( const wxPGProperty* p1, const wxPGProperty* p2 )
{
    if ( m_frozen )
        return;

    if ( m_pState->m_itemsAdded )
        PrepareAfterItemsAdded();

    wxRect r = GetPropertyRect(p1, p2);
    if ( r.width > 0 )
    {
        m_canvas->RefreshRect(r);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::RefreshProperty( wxPGProperty* p )
{
    if ( p == m_selected )
        DoSelectProperty(p, wxPG_SEL_FORCE);

    DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItemAndValueRelated( wxPGProperty* p )
{
    if ( m_frozen )
        return;

    // Draw item, children, and parent too, if it is not category
    wxPGProperty* parent = p->GetParent();

    while ( parent &&
            !parent->IsCategory() &&
            parent->GetParent() )
    {
         DrawItem(parent);
         parent = parent->GetParent();
    }

    DrawItemAndChildren(p);
}

void wxPropertyGrid::DrawItemAndChildren( wxPGProperty* p )
{
    wxCHECK_RET( p, wxT("invalid property id") );

    // Do not draw if in non-visible page
    if ( p->GetParentState() != m_pState )
        return;

    // do not draw a single item if multiple pending
    if ( m_pState->m_itemsAdded || m_frozen )
        return;

    wxWindow* wndPrimary = GetEditorControl();

    // Update child control.
    if ( m_selected && m_selected->GetParent() == p )
        m_selected->UpdateControl(wndPrimary);

    const wxPGProperty* lastDrawn = p->GetLastVisibleSubItem();

    DrawItems(p, lastDrawn);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Refresh( bool WXUNUSED(eraseBackground),
                              const wxRect *rect )
{
    PrepareAfterItemsAdded();

    wxWindow::Refresh(false);
    if ( m_canvas )
        // TODO: Coordinate translation
        m_canvas->Refresh(false, rect);

#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
    // I think this really helps only GTK+1.2
    if ( m_wndEditor ) m_wndEditor->Refresh();
    if ( m_wndEditor2 ) m_wndEditor2->Refresh();
#endif
}

// -----------------------------------------------------------------------
// wxPropertyGrid global operations
// -----------------------------------------------------------------------

void wxPropertyGrid::Clear()
{
    if ( m_selected )
    {
        bool selRes = DoSelectProperty(NULL, wxPG_SEL_DELETING);  // This must be before state clear
        wxPG_CHECK_RET_DBG( selRes,
                            wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    m_pState->DoClear();

    m_propHover = NULL;

    m_prevVY = 0;

    RecalculateVirtualSize();

    // Need to clear some area at the end
    if ( !m_frozen )
        RefreshRect(wxRect(0, 0, m_width, m_height));
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::EnableCategories( bool enable )
{
    if ( !ClearSelection() )
        return false;

    if ( enable )
    {
        //
        // Enable categories
        //

        m_windowStyle &= ~(wxPG_HIDE_CATEGORIES);
    }
    else
    {
        //
        // Disable categories
        //
        m_windowStyle |= wxPG_HIDE_CATEGORIES;
    }

    if ( !m_pState->EnableCategories(enable) )
        return false;

    if ( !m_frozen )
    {
        if ( m_windowStyle & wxPG_AUTO_SORT )
        {
            m_pState->m_itemsAdded = 1; // force
            PrepareAfterItemsAdded();
        }
    }
    else
        m_pState->m_itemsAdded = 1;

    // No need for RecalculateVirtualSize() here - it is already called in
    // wxPropertyGridPageState method above.

    Refresh();

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SwitchState( wxPropertyGridPageState* pNewState )
{
    wxASSERT( pNewState );
    wxASSERT( pNewState->GetGrid() );

    if ( pNewState == m_pState )
        return;

    wxPGProperty* oldSelection = m_selected;

    // Deselect
    if ( m_selected )
    {
        bool selRes = ClearSelection();
        wxPG_CHECK_RET_DBG( selRes,
                            wxT("failed to deselect a property (editor probably had invalid value)") );
    }

    m_pState->m_selected = oldSelection;

    bool orig_mode = m_pState->IsInNonCatMode();
    bool new_state_mode = pNewState->IsInNonCatMode();

    m_pState = pNewState;

    // Validate width
    int pgWidth = GetClientSize().x;
    if ( HasVirtualWidth() )
    {
        int minWidth = pgWidth;
        if ( pNewState->m_width < minWidth )
        {
            pNewState->m_width = minWidth;
            pNewState->CheckColumnWidths();
        }
    }
    else
    {
        //
        // Just in case, fully re-center splitter
        if ( HasFlag( wxPG_SPLITTER_AUTO_CENTER ) )
            pNewState->m_fSplitterX = -1.0;

        pNewState->OnClientWidthChange( pgWidth, pgWidth - pNewState->m_width );
    }

    m_propHover = (wxPGProperty*) NULL;

    // If necessary, convert state to correct mode.
    if ( orig_mode != new_state_mode )
    {
        // This should refresh as well.
        EnableCategories( orig_mode?false:true );
    }
    else if ( !m_frozen )
    {
        // Refresh, if not frozen.
        if ( m_pState->m_itemsAdded )
            PrepareAfterItemsAdded();

        // Reselect
        if ( m_pState->m_selected )
            DoSelectProperty( m_pState->m_selected );

        RecalculateVirtualSize(0);
        Refresh();
    }
    else
        m_pState->m_itemsAdded = 1;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SortChildren( wxPGPropArg id )
{
    wxPG_PROP_ARG_CALL_PROLOG()

    m_pState->SortChildren( p );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Sort()
{
    bool selRes = ClearSelection();  // This must be before state clear
    wxPG_CHECK_RET_DBG( selRes,
                        wxT("failed to deselect a property (editor probably had invalid value)") );

    m_pState->Sort();
}

// -----------------------------------------------------------------------

// Call to SetSplitterPosition will always disable splitter auto-centering
// if parent window is shown.
void wxPropertyGrid::DoSetSplitterPosition_( int newxpos, bool refresh, int splitterIndex, bool allPages )
{
    if ( ( newxpos < wxPG_DRAG_MARGIN ) )
        return;

    wxPropertyGridPageState* state = m_pState;

    state->DoSetSplitterPosition( newxpos, splitterIndex, allPages );

    if ( refresh )
    {
        if ( m_selected )
            CorrectEditorWidgetSizeX();

        Refresh();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CenterSplitter( bool enableAutoCentering )
{
    SetSplitterPosition( m_width/2, true );
    if ( enableAutoCentering && ( m_windowStyle & wxPG_SPLITTER_AUTO_CENTER ) )
        m_iFlags &= ~(wxPG_FL_DONT_CENTER_SPLITTER);
}

// -----------------------------------------------------------------------
// wxPropertyGrid item iteration (GetNextProperty etc.) methods
// -----------------------------------------------------------------------

// Returns nearest paint visible property (such that will be painted unless
// window is scrolled or resized). If given property is paint visible, then
// it itself will be returned
wxPGProperty* wxPropertyGrid::GetNearestPaintVisible( wxPGProperty* p ) const
{
    int vx,vy1;// Top left corner of client
    GetViewStart(&vx,&vy1);
    vy1 *= wxPG_PIXELS_PER_UNIT;

    int vy2 = vy1 + m_height;
    int propY = p->GetY2(m_lineHeight);

    if ( (propY + m_lineHeight) < vy1 )
    {
    // Too high
        return DoGetItemAtY( vy1 );
    }
    else if ( propY > vy2 )
    {
    // Too low
        return DoGetItemAtY( vy2 );
    }

    // Itself paint visible
    return p;

}

// -----------------------------------------------------------------------
// Methods related to change in value, value modification and sending events
// -----------------------------------------------------------------------

// commits any changes in editor of selected property
// return true if validation did not fail
// flags are same as with DoSelectProperty
bool wxPropertyGrid::CommitChangesFromEditor( wxUint32 flags )
{
    // Committing already?
    if ( m_inCommitChangesFromEditor )
        return true;

    // Don't do this if already processing editor event. It might
    // induce recursive dialogs and crap like that.
    if ( m_iFlags & wxPG_FL_IN_ONCUSTOMEDITOREVENT )
    {
        if ( m_inDoPropertyChanged )
            return true;

        return false;
    }

    if ( m_wndEditor &&
         IsEditorsValueModified() &&
         (m_iFlags & wxPG_FL_INITIALIZED) &&
         m_selected )
    {
        m_inCommitChangesFromEditor = 1;

        wxVariant variant(m_selected->GetValueRef());
        bool valueIsPending = false;

        // JACS - necessary to avoid new focus being found spuriously within OnIdle
        // due to another window getting focus
        wxWindow* oldFocus = m_curFocused;

        bool validationFailure = false;
        bool forceSuccess = (flags & (wxPG_SEL_NOVALIDATE|wxPG_SEL_FORCE)) ? true : false;

        m_chgInfo_changedProperty = NULL;

        // If truly modified, schedule value as pending.
        if ( m_selected->GetEditorClass()->GetValueFromControl( variant, m_selected, GetEditorControl() ) )
        {
            if ( DoEditorValidate() &&
                 PerformValidation(m_selected, variant) )
            {
                valueIsPending = true;
            }
            else
            {
                validationFailure = true;
            }
        }
        else
        {
            EditorsValueWasNotModified();
        }

        bool res = true;

        m_inCommitChangesFromEditor = 0;

        if ( validationFailure && !forceSuccess )
        {
            if (oldFocus)
            {
                oldFocus->SetFocus();
                m_curFocused = oldFocus;
            }

            res = OnValidationFailure(m_selected, variant);

            // Now prevent further validation failure messages
            if ( res )
            {
                EditorsValueWasNotModified();
                OnValidationFailureReset(m_selected);
            }
        }
        else if ( valueIsPending )
        {
            DoPropertyChanged( m_selected, flags );
            EditorsValueWasNotModified();
        }

        return res;
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::PerformValidation( wxPGProperty* p, wxVariant& pendingValue,
                                        int flags )
{
    //
    // Runs all validation functionality.
    // Returns true if value passes all tests.
    //

    m_validationInfo.m_failureBehavior = m_permanentValidationFailureBehavior;

    if ( pendingValue.GetType() == wxPG_VARIANT_TYPE_LIST )
    {
        if ( !p->ValidateValue(pendingValue, m_validationInfo) )
            return false;
    }

    //
    // Adapt list to child values, if necessary
    wxVariant listValue = pendingValue;
    wxVariant* pPendingValue = &pendingValue;
    wxVariant* pList = NULL;

    // If parent has wxPG_PROP_AGGREGATE flag, or uses composite
    // string value, then we need treat as it was changed instead
    // (or, in addition, as is the case with composite string parent).
    // This includes creating list variant for child values.

    wxPGProperty* pwc = p->GetParent();
    wxPGProperty* changedProperty = p;
    wxPGProperty* baseChangedProperty = changedProperty;
    wxVariant bcpPendingList;

    listValue = pendingValue;
    listValue.SetName(p->GetBaseName());

    while ( pwc &&
            (pwc->HasFlag(wxPG_PROP_AGGREGATE) || pwc->HasFlag(wxPG_PROP_COMPOSED_VALUE)) )
    {
        wxVariantList tempList;
        wxVariant lv(tempList, pwc->GetBaseName());
        lv.Append(listValue);
        listValue = lv;
        pPendingValue = &listValue;

        if ( pwc->HasFlag(wxPG_PROP_AGGREGATE) )
        {
            baseChangedProperty = pwc;
            bcpPendingList = lv;
        }

        changedProperty = pwc;
        pwc = pwc->GetParent();
    }

    wxVariant value;
    wxPGProperty* evtChangingProperty = changedProperty;

    if ( pPendingValue->GetType() != wxPG_VARIANT_TYPE_LIST )
    {
        value = *pPendingValue;
    }
    else
    {
        // Convert list to child values
        pList = pPendingValue;
        changedProperty->AdaptListToValue( *pPendingValue, &value );
    }

    wxVariant evtChangingValue = value;

    if ( flags & SendEvtChanging )
    {
        // FIXME: After proper ValueToString()s added, remove
        // this. It is just a temporary fix, as evt_changing
        // will simply not work for wxPG_PROP_COMPOSED_VALUE
        // (unless it is selected, and textctrl editor is open).
        if ( changedProperty->HasFlag(wxPG_PROP_COMPOSED_VALUE) )
        {
            evtChangingProperty = baseChangedProperty;
            if ( evtChangingProperty != p )
            {
                evtChangingProperty->AdaptListToValue( bcpPendingList, &evtChangingValue );
            }
            else
            {
                evtChangingValue = pendingValue;
            }
        }

        if ( evtChangingProperty->HasFlag(wxPG_PROP_COMPOSED_VALUE) )
        {
            if ( changedProperty == m_selected )
            {
                wxWindow* editor = GetEditorControl();
                wxASSERT( editor->IsKindOf(CLASSINFO(wxTextCtrl)) );
                evtChangingValue = wxStaticCast(editor, wxTextCtrl)->GetValue();
            }
            else
            {
                wxLogDebug(wxT("WARNING: wxEVT_PG_CHANGING is about to happen with old value."));
            }
        }
    }

    wxASSERT( m_chgInfo_changedProperty == NULL );
    m_chgInfo_changedProperty = changedProperty;
    m_chgInfo_baseChangedProperty = baseChangedProperty;
    m_chgInfo_pendingValue = value;

    if ( pList )
        m_chgInfo_valueList = *pList;
    else
        m_chgInfo_valueList.MakeNull();

    // If changedProperty is not property which value was edited,
    // then call wxPGProperty::ValidateValue() for that as well.
    if ( p != changedProperty && value.GetType() != wxPG_VARIANT_TYPE_LIST )
    {
        if ( !changedProperty->ValidateValue(value, m_validationInfo) )
            return false;
    }

    if ( flags & SendEvtChanging )
    {
        // SendEvent returns true if event was vetoed
        if ( SendEvent( wxEVT_PG_CHANGING, evtChangingProperty, &evtChangingValue, 0 ) )
            return false;
    }

    if ( flags & IsStandaloneValidation )
    {
        // If called in 'generic' context, we need to reset
        // m_chgInfo_changedProperty and write back translated value.
        m_chgInfo_changedProperty = NULL;
        pendingValue = value;
    }

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoShowPropertyError( wxPGProperty* WXUNUSED(property), const wxString& msg )
{
    if ( !msg.length() )
        return;

#if wxUSE_STATUSBAR
    if ( !wxPGGlobalVars->m_offline )
    {
        wxWindow* topWnd = ::wxGetTopLevelParent(this);
        if ( topWnd )
        {
            wxFrame* pFrame = wxDynamicCast(topWnd, wxFrame);
            if ( pFrame )
            {
                wxStatusBar* pStatusBar = pFrame->GetStatusBar();
                if ( pStatusBar )
                {
                    pStatusBar->SetStatusText(msg);
                    return;
                }
            }
        }
    }
#endif

    ::wxMessageBox(msg, _T("Property Error"));
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoOnValidationFailure( wxPGProperty* property, wxVariant& WXUNUSED(invalidValue) )
{
    int vfb = m_validationInfo.m_failureBehavior;

    if ( vfb & wxPG_VFB_BEEP )
        ::wxBell();

    if ( (vfb & wxPG_VFB_MARK_CELL) &&
         !property->HasFlag(wxPG_PROP_INVALID_VALUE) )
    {
        wxASSERT_MSG( !property->GetCell(0) && !property->GetCell(1),
                      wxT("Currently wxPG_VFB_MARK_CELL only works with properties with standard first two cells") );

        if ( !property->GetCell(0) && !property->GetCell(1) )
        {
            wxColour vfbFg = *wxWHITE;
            wxColour vfbBg = *wxRED;
            property->SetCell(0, new wxPGCell(property->GetLabel(), wxNullBitmap, vfbFg, vfbBg));
            property->SetCell(1, new wxPGCell(property->GetDisplayedString(), wxNullBitmap, vfbFg, vfbBg));

            DrawItemAndChildren(property);

            if ( property == m_selected )
            {
                SetInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL);

                wxWindow* editor = GetEditorControl();
                if ( editor )
                {
                    editor->SetForegroundColour(vfbFg);
                    editor->SetBackgroundColour(vfbBg);
                }
            }
        }
    }

    if ( vfb & wxPG_VFB_SHOW_MESSAGE )
    {
        wxString msg = m_validationInfo.m_failureMessage;

        if ( !msg.length() )
            msg = _T("You have entered invalid value. Press ESC to cancel editing.");

        DoShowPropertyError(property, msg);
    }

    return (vfb & wxPG_VFB_STAY_IN_PROPERTY) ? false : true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoOnValidationFailureReset( wxPGProperty* property )
{
    int vfb = m_validationInfo.m_failureBehavior;

    if ( vfb & wxPG_VFB_MARK_CELL )
    {
        property->SetCell(0, NULL);
        property->SetCell(1, NULL);

        ClearInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL);

        if ( property == m_selected && GetEditorControl() )
        {
            // Calling this will recreate the control, thus resetting its colour
            RefreshProperty(property);
        }
        else
        {
            DrawItemAndChildren(property);
        }
    }
}

// -----------------------------------------------------------------------

// flags are same as with DoSelectProperty
bool wxPropertyGrid::DoPropertyChanged( wxPGProperty* p, unsigned int selFlags )
{
    if ( m_inDoPropertyChanged )
        return true;

    wxWindow* editor = GetEditorControl();

    m_pState->m_anyModified = 1;

    m_inDoPropertyChanged = 1;

    // Maybe need to update control
    wxASSERT( m_chgInfo_changedProperty != NULL );

    // These values were calculated in PerformValidation()
    wxPGProperty* changedProperty = m_chgInfo_changedProperty;
    wxVariant value = m_chgInfo_pendingValue;

    wxPGProperty* topPaintedProperty = changedProperty;

    while ( !topPaintedProperty->IsCategory() &&
            !topPaintedProperty->IsRoot() )
    {
        topPaintedProperty = topPaintedProperty->GetParent();
    }

    changedProperty->SetValue(value, &m_chgInfo_valueList, wxPG_SETVAL_BY_USER);

    // Set as Modified (not if dragging just began)
    if ( !(p->m_flags & wxPG_PROP_MODIFIED) )
    {
        p->m_flags |= wxPG_PROP_MODIFIED;
        if ( p == m_selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
        {
            if ( editor )
                SetCurControlBoldFont();
        }
    }

    wxPGProperty* pwc;

    // Propagate updates to parent(s)
    pwc = p;
    wxPGProperty* prevPwc = NULL;

    while ( prevPwc != topPaintedProperty )
    {
        pwc->m_flags |= wxPG_PROP_MODIFIED;

        if ( pwc == m_selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
        {
            if ( editor )
                SetCurControlBoldFont();
        }

        prevPwc = pwc;
        pwc = pwc->GetParent();
    }

    // Draw the actual property
    DrawItemAndChildren( topPaintedProperty );

    //
    // If value was set by wxPGProperty::OnEvent, then update the editor
    // control.
    if ( selFlags & wxPG_SEL_DIALOGVAL )
    {
        if ( editor )
            p->GetEditorClass()->UpdateControl(p, editor);
    }
    else
    {
#if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
        if ( m_wndEditor ) m_wndEditor->Refresh();
        if ( m_wndEditor2 ) m_wndEditor2->Refresh();
#endif
    }

    // Sanity check
    wxASSERT( !changedProperty->GetParent()->HasFlag(wxPG_PROP_AGGREGATE) );

    // If top parent has composite string value, then send to child parents,
    // starting from baseChangedProperty.
    if ( changedProperty->HasFlag(wxPG_PROP_COMPOSED_VALUE) )
    {
        pwc = m_chgInfo_baseChangedProperty;

        while ( pwc != changedProperty )
        {
            SendEvent( wxEVT_PG_CHANGED, pwc, NULL, selFlags );
            pwc = pwc->GetParent();
        }
    }

    SendEvent( wxEVT_PG_CHANGED, changedProperty, NULL, selFlags );

    m_inDoPropertyChanged = 0;

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::ChangePropertyValue( wxPGPropArg id, wxVariant newValue )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    m_chgInfo_changedProperty = NULL;

    if ( PerformValidation(p, newValue) )
    {
        DoPropertyChanged(p);
        return true;
    }
    else
    {
        OnValidationFailure(p, newValue);
    }

    return false;
}

// -----------------------------------------------------------------------

wxVariant wxPropertyGrid::GetUncommittedPropertyValue()
{
    wxPGProperty* prop = GetSelectedProperty();

    if ( !prop )
        return wxNullVariant;

    wxTextCtrl* tc = GetEditorTextCtrl();
    wxVariant value = prop->GetValue();

    if ( !tc || !IsEditorsValueModified() )
        return value;

    if ( !prop->StringToValue(value, tc->GetValue()) )
        return value;

    if ( !PerformValidation(prop, value, IsStandaloneValidation) )
        return prop->GetValue();

    return value;
}

// -----------------------------------------------------------------------

// Runs wxValidator for the selected property
bool wxPropertyGrid::DoEditorValidate()
{
    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::ProcessEvent(wxEvent& event)
{
    wxWindow* wnd = (wxWindow*) event.GetEventObject();
    if ( wnd && wnd->IsKindOf(CLASSINFO(wxWindow)) )
    {
        wxWindow* parent = wnd->GetParent();

        if ( parent &&
             (parent == m_canvas ||
              parent->GetParent() == m_canvas) )
        {
            OnCustomEditorEvent((wxCommandEvent&)event);
            return true;
        }
    }
    return wxPanel::ProcessEvent(event);
}

// -----------------------------------------------------------------------

// NB: It may really not be wxCommandEvent - must check if necessary
//     (usually not).
void wxPropertyGrid::OnCustomEditorEvent( wxCommandEvent &event )
{
    wxPGProperty* selected = m_selected;

    // Somehow, event is handled after property has been deselected.
    // Possibly, but very rare.
    if ( !selected )
        return;

    if ( m_iFlags & wxPG_FL_IN_ONCUSTOMEDITOREVENT )
        return;

    wxVariant pendingValue(selected->GetValueRef());
    wxWindow* wnd = GetEditorControl();
    int selFlags = 0;
    bool wasUnspecified = selected->IsValueUnspecified();
    int usesAutoUnspecified = selected->UsesAutoUnspecified();

    bool valueIsPending = false;

    m_chgInfo_changedProperty = NULL;

    m_iFlags &= ~(wxPG_FL_VALIDATION_FAILED|wxPG_FL_VALUE_CHANGE_IN_EVENT);

    //
    // Filter out excess wxTextCtrl modified events
    if ( event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED &&
         wnd &&
         wnd->IsKindOf(CLASSINFO(wxTextCtrl)) )
    {
        wxTextCtrl* tc = (wxTextCtrl*) wnd;

        wxString newTcValue = tc->GetValue();
        if ( m_prevTcValue == newTcValue )
            return;

        m_prevTcValue = newTcValue;
    }

    SetInternalFlag(wxPG_FL_IN_ONCUSTOMEDITOREVENT);

    bool validationFailure = false;
    bool buttonWasHandled = false;

    //
    // Try common button handling
    if ( m_wndEditor2 && event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        wxPGEditorDialogAdapter* adapter = selected->GetEditorDialog();

        if ( adapter )
        {
            buttonWasHandled = true;
            // Store as res2, as previously (and still currently alternatively)
            // dialogs can be shown by handling wxEVT_COMMAND_BUTTON_CLICKED
            // in wxPGProperty::OnEvent().
            adapter->ShowDialog( this, selected );
            delete adapter;
        }
    }

    if ( !buttonWasHandled )
    {
        if ( wnd )
        {
            // First call editor class' event handler.
            const wxPGEditor* editor = selected->GetEditorClass();

            if ( editor->OnEvent( this, selected, wnd, event ) )
            {
                // If changes, validate them
                if ( DoEditorValidate() )
                {
                    if ( editor->GetValueFromControl( pendingValue, m_selected, wnd ) )
                        valueIsPending = true;
                }
                else
                {
                    validationFailure = true;
                }
            }
        }

        // Then the property's custom handler (must be always called, unless
        // validation failed).
        if ( !validationFailure )
            buttonWasHandled = selected->OnEvent( this, wnd, event );
    }

    // SetValueInEvent(), as called in one of the functions referred above
    // overrides editor's value.
    if ( m_iFlags & wxPG_FL_VALUE_CHANGE_IN_EVENT )
    {
        valueIsPending = true;
        pendingValue = m_changeInEventValue;
        selFlags |= wxPG_SEL_DIALOGVAL;
    }

    if ( !validationFailure && valueIsPending )
        if ( !PerformValidation(m_selected, pendingValue) )
            validationFailure = true;

    if ( validationFailure)
    {
        OnValidationFailure(selected, pendingValue);
    }
    else if ( valueIsPending )
    {
        selFlags |= ( !wasUnspecified && selected->IsValueUnspecified() && usesAutoUnspecified ) ? wxPG_SEL_SETUNSPEC : 0;

        DoPropertyChanged(selected, selFlags);
        EditorsValueWasNotModified();

        // Regardless of editor type, unfocus editor on
        // text-editing related enter press.
        if ( event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER )
        {
            SetFocusOnCanvas();
        }
    }
    else
    {
        // No value after all
        
        // Regardless of editor type, unfocus editor on
        // text-editing related enter press.
        if ( event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER )
        {
            SetFocusOnCanvas();
        }

        // Let unhandled button click events go to the parent
        if ( !buttonWasHandled && event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
        {
            wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED,GetId());
            GetEventHandler()->AddPendingEvent(evt);
        }
    }

    ClearInternalFlag(wxPG_FL_IN_ONCUSTOMEDITOREVENT);
}

// -----------------------------------------------------------------------
// wxPropertyGrid editor control helper methods
// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetEditorWidgetRect( wxPGProperty* p, int column ) const
{
    int itemy = p->GetY2(m_lineHeight);
    int vy = 0;
    int cust_img_space = 0;
    int splitterX = m_pState->DoGetSplitterPosition(column-1);
    int colEnd = splitterX + m_pState->m_colWidths[column];

    // TODO: If custom image detection changes from current, change this.
    if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE /*p->m_flags & wxPG_PROP_CUSTOMIMAGE*/ )
    {
        //m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;
        int imwid = p->OnMeasureImage().x;
        if ( imwid < 1 ) imwid = wxPG_CUSTOM_IMAGE_WIDTH;
        cust_img_space = imwid + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
    }

    return wxRect
      (
        splitterX+cust_img_space+wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1,
        itemy-vy,
        colEnd-splitterX-wxPG_XBEFOREWIDGET-wxPG_CONTROL_MARGIN-cust_img_space-1,
        m_lineHeight-1
      );
}

// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetImageRect( wxPGProperty* p, int item ) const
{
    wxSize sz = GetImageSize(p, item);
    return wxRect(wxPG_CONTROL_MARGIN + wxCC_CUSTOM_IMAGE_MARGIN1,
                  wxPG_CUSTOM_IMAGE_SPACINGY,
                  sz.x,
                  sz.y);
}

// return size of custom paint image
wxSize wxPropertyGrid::GetImageSize( wxPGProperty* p, int item ) const
{
    // If called with NULL property, then return default image
    // size for properties that use image.
    if ( !p )
        return wxSize(wxPG_CUSTOM_IMAGE_WIDTH,wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight));

    wxSize cis = p->OnMeasureImage(item);

    int choiceCount = p->m_choices.GetCount();
    int comVals = p->GetDisplayedCommonValueCount();
    if ( item >= choiceCount && comVals > 0 )
    {
        unsigned int cvi = item-choiceCount;
        cis = GetCommonValue(cvi)->GetRenderer()->GetImageSize(NULL, 1, cvi);
    }
    else if ( item >= 0 && choiceCount == 0 )
        return wxSize(0, 0);

    if ( cis.x < 0 )
    {
        if ( cis.x <= -1 )
            cis.x = wxPG_CUSTOM_IMAGE_WIDTH;
    }
    if ( cis.y <= 0 )
    {
        if ( cis.y >= -1 )
            cis.y = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
        else
            cis.y = -cis.y;
    }
    return cis;
}

// -----------------------------------------------------------------------

// takes scrolling into account
void wxPropertyGrid::ImprovedClientToScreen( int* px, int* py )
{
    int vx, vy;
    GetViewStart(&vx,&vy);
    vy*=wxPG_PIXELS_PER_UNIT;
    vx*=wxPG_PIXELS_PER_UNIT;
    *px -= vx;
    *py -= vy;
    ClientToScreen( px, py );
}

// -----------------------------------------------------------------------

wxPropertyGridHitTestResult wxPropertyGrid::HitTest( const wxPoint& pt ) const
{
    wxPoint pt2;
    GetViewStart(&pt2.x,&pt2.y);
    pt2.x *= wxPG_PIXELS_PER_UNIT;
    pt2.y *= wxPG_PIXELS_PER_UNIT;
    pt2.x += pt.x;
    pt2.y += pt.y;

    return m_pState->HitTest(pt2);
}

// -----------------------------------------------------------------------

// custom set cursor
void wxPropertyGrid::CustomSetCursor( int type, bool override )
{
    if ( type == m_curcursor && !override ) return;

    wxCursor* cursor = &wxPG_DEFAULT_CURSOR;

    if ( type == wxCURSOR_SIZEWE )
        cursor = m_cursorSizeWE;

    m_canvas->SetCursor( *cursor );

    m_curcursor = type;
}

// -----------------------------------------------------------------------
// wxPropertyGrid property selection
// -----------------------------------------------------------------------

// Setups event handling for child control
void wxPropertyGrid::SetupChildEventHandling( wxWindow* argWnd )
{
    wxWindowID id = argWnd->GetId();

    if ( argWnd == m_wndEditor )
    {
        argWnd->Connect(id, wxEVT_MOTION,
            wxMouseEventHandler(wxPropertyGrid::OnMouseMoveChild),
            NULL, this);
        argWnd->Connect(id, wxEVT_LEFT_UP,
            wxMouseEventHandler(wxPropertyGrid::OnMouseUpChild),
            NULL, this);
        argWnd->Connect(id, wxEVT_LEFT_DOWN,
            wxMouseEventHandler(wxPropertyGrid::OnMouseClickChild),
            NULL, this);
        argWnd->Connect(id, wxEVT_RIGHT_UP,
            wxMouseEventHandler(wxPropertyGrid::OnMouseRightClickChild),
            NULL, this);
        argWnd->Connect(id, wxEVT_ENTER_WINDOW,
            wxMouseEventHandler(wxPropertyGrid::OnMouseEntry),
            NULL, this);
        argWnd->Connect(id, wxEVT_LEAVE_WINDOW,
            wxMouseEventHandler(wxPropertyGrid::OnMouseEntry),
            NULL, this);
        argWnd->Connect(id, wxEVT_KEY_DOWN,
            wxCharEventHandler(wxPropertyGrid::OnChildKeyDown),
            NULL, this);
    }
}

void wxPropertyGrid::FreeEditors()
{
    // Do not free editors immediately if processing events
    if ( m_wndEditor2 )
    {
        m_wndEditor2->Hide();
        wxPendingDelete.Append( m_wndEditor2 );
        m_wndEditor2 = (wxWindow*) NULL;
    }

    if ( m_wndEditor )
    {
        m_wndEditor->Hide();
        wxPendingDelete.Append( m_wndEditor );
        m_wndEditor = (wxWindow*) NULL;
    }
}

// Call with NULL to de-select property
bool wxPropertyGrid::DoSelectProperty( wxPGProperty* p, unsigned int flags )
{
    wxPanel* canvas = GetPanel();

    /*
    if (p)
        wxLogDebug(wxT("SelectProperty( %s (%s[%i]) )"),p->m_label.c_str(),
            p->m_parent->m_label.c_str(),p->GetIndexInParent());
    else
        wxLogDebug(wxT("SelectProperty( NULL, -1 )"));
    */

    if ( m_inDoSelectProperty )
        return true;

    m_inDoSelectProperty = 1;

    wxPGProperty* prev = m_selected;

    if ( !m_pState )
    {
        m_inDoSelectProperty = 0;
        return false;
    }

/*
    if (m_selected)
        wxPrintf( "Selected %s\n", m_selected->GetClassInfo()->GetClassName() );
    else
        wxPrintf( "None selected\n" );
    
    if (p)
        wxPrintf( "P =  %s\n", p->GetClassInfo()->GetClassName() );
    else
        wxPrintf( "P = NULL\n" );
*/
  
    // If we are frozen, then just set the values.
    if ( m_frozen )
    {
        m_iFlags &= ~(wxPG_FL_ABNORMAL_EDITOR);
        m_editorFocused = 0;
        m_selected = p;
        m_selColumn = 1;
        m_pState->m_selected = p;

        // If frozen, always free controls. But don't worry, as Thaw will
        // recall SelectProperty to recreate them.
        FreeEditors();

        // Prevent any further selection measures in this call
        p = (wxPGProperty*) NULL;
    }
    else
    {
        // Is it the same?
        if ( m_selected == p && !(flags & wxPG_SEL_FORCE) )
        {
            // Only set focus if not deselecting
            if ( p )
            {
                if ( flags & wxPG_SEL_FOCUS )
                {
                    if ( m_wndEditor )
                    {
                        m_wndEditor->SetFocus();
                        m_editorFocused = 1;
                    }
                }
                else
                {
                    SetFocusOnCanvas();
                }
            }

            m_inDoSelectProperty = 0;
            return true;
        }

        //
        // First, deactivate previous
        if ( m_selected )
        {

            OnValidationFailureReset(m_selected);

            // Must double-check if this is an selected in case of forceswitch
            if ( p != prev )
            {
                if ( !CommitChangesFromEditor(flags) )
                {
                    // Validation has failed, so we can't exit the previous editor
                    //::wxMessageBox(_("Please correct the value or press ESC to cancel the edit."),
                    //               _("Invalid Value"),wxOK|wxICON_ERROR);
                    m_inDoSelectProperty = 0;
                    return false;
                }
            }

            FreeEditors();
            m_selColumn = -1;

            m_selected = (wxPGProperty*) NULL;
            m_pState->m_selected = (wxPGProperty*) NULL;

            // We need to always fully refresh the grid here
            Refresh(false);

            m_iFlags &= ~(wxPG_FL_ABNORMAL_EDITOR);
            EditorsValueWasNotModified();
        }

        SetInternalFlag(wxPG_FL_IN_SELECT_PROPERTY);

        //
        // Then, activate the one given.
        if ( p )
        {
            int propY = p->GetY2(m_lineHeight);

            int splitterX = GetSplitterPosition();
            m_editorFocused = 0;
            m_selected = p;
            m_pState->m_selected = p;
            m_iFlags |= wxPG_FL_PRIMARY_FILLS_ENTIRE;
            if ( p != prev )
                m_iFlags &= ~(wxPG_FL_VALIDATION_FAILED);

            wxASSERT( m_wndEditor == (wxWindow*) NULL );

            // Do we need OnMeasureCalls?
            wxSize imsz = p->OnMeasureImage();

            //
            // Only create editor for non-disabled non-caption
            if ( !p->IsCategory() && !(p->m_flags & wxPG_PROP_DISABLED) )
            {
            // do this for non-caption items

                m_selColumn = 1;

                // Do we need to paint the custom image, if any?
                m_iFlags &= ~(wxPG_FL_CUR_USES_CUSTOM_IMAGE);
                if ( (p->m_flags & wxPG_PROP_CUSTOMIMAGE) &&
                     !p->GetEditorClass()->CanContainCustomImage()
                   )
                    m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;

                wxRect grect = GetEditorWidgetRect(p, m_selColumn);
                wxPoint goodPos = grect.GetPosition();
            #if wxPG_CREATE_CONTROLS_HIDDEN
                int coord_adjust = m_height - goodPos.y;
                goodPos.y += coord_adjust;
            #endif

                const wxPGEditor* editor = p->GetEditorClass();
                wxCHECK_MSG(editor, false,
                    wxT("NULL editor class not allowed"));

                m_iFlags &= ~wxPG_FL_FIXED_WIDTH_EDITOR;

                wxPGWindowList wndList = editor->CreateControls(this,
                                                                p,
                                                                goodPos,
                                                                grect.GetSize());

                m_wndEditor = wndList.m_primary;
                m_wndEditor2 = wndList.m_secondary;
                wxWindow* primaryCtrl = GetEditorControl();

                //
                // Essentially, primaryCtrl == m_wndEditor
                //

                // NOTE: It is allowed for m_wndEditor to be NULL - in this case
                //       value is drawn as normal, and m_wndEditor2 is assumed
                //       to be a right-aligned button that triggers a separate editorCtrl
                //       window.

                if ( m_wndEditor )
                {
                    wxASSERT_MSG( m_wndEditor->GetParent() == canvas,
                                  wxT("CreateControls must use result of wxPropertyGrid::GetPanel() as parent of controls.") );

                    // Set validator, if any
                #if wxUSE_VALIDATORS
                    wxValidator* validator = p->GetValidator();
                    if ( validator )
                        primaryCtrl->SetValidator(*validator);
                #endif

                    if ( m_wndEditor->GetSize().y > (m_lineHeight+6) )
                        m_iFlags |= wxPG_FL_ABNORMAL_EDITOR;

                    // If it has modified status, use bold font
                    // (must be done before capturing m_ctrlXAdjust)
                    if ( (p->m_flags & wxPG_PROP_MODIFIED) && (m_windowStyle & wxPG_BOLD_MODIFIED) )
                        SetCurControlBoldFont();

                    //
                    // Fix TextCtrl indentation
                #if defined(__WXMSW__) && !defined(__WXWINCE__)
                    wxTextCtrl* tc = NULL;
                    if ( primaryCtrl->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)) )
                        tc = ((wxOwnerDrawnComboBox*)primaryCtrl)->GetTextCtrl();
                    else
                        tc = wxDynamicCast(primaryCtrl, wxTextCtrl);
                    if ( tc )
                        ::SendMessage(GetHwndOf(tc), EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(0, 0));
                #endif

                    // Store x relative to splitter (we'll need it).
                    m_ctrlXAdjust = m_wndEditor->GetPosition().x - splitterX;

                    // Check if background clear is not necessary
                    wxPoint pos = m_wndEditor->GetPosition();
                    if ( pos.x > (splitterX+1) || pos.y > propY )
                    {
                        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
                    }

                    m_wndEditor->SetSizeHints(3, 3);

                #if wxPG_CREATE_CONTROLS_HIDDEN
                    m_wndEditor->Show(false);
                    m_wndEditor->Freeze();

                    goodPos = m_wndEditor->GetPosition();
                    goodPos.y -= coord_adjust;
                    m_wndEditor->Move( goodPos );
                #endif

                    SetupChildEventHandling(primaryCtrl);

                    // Focus and select all (wxTextCtrl, wxComboBox etc)
                    if ( flags & wxPG_SEL_FOCUS )
                    {
                        primaryCtrl->SetFocus();

                        p->GetEditorClass()->OnFocus(p, primaryCtrl);
                    }
                }

                if ( m_wndEditor2 )
                {
                    wxASSERT_MSG( m_wndEditor2->GetParent() == canvas,
                                  wxT("CreateControls must use result of wxPropertyGrid::GetPanel() as parent of controls.") );

                    // Get proper id for wndSecondary
                    m_wndSecId = m_wndEditor2->GetId();
                    wxWindowList children = m_wndEditor2->GetChildren();
                    wxWindowList::iterator node = children.begin();
                    if ( node != children.end() )
                        m_wndSecId = ((wxWindow*)*node)->GetId();

                    m_wndEditor2->SetSizeHints(3,3);

                #if wxPG_CREATE_CONTROLS_HIDDEN
                    wxRect sec_rect = m_wndEditor2->GetRect();
                    sec_rect.y -= coord_adjust;

                    // Fine tuning required to fix "oversized"
                    // button disappearance bug.
                    if ( sec_rect.y < 0 )
                    {
                        sec_rect.height += sec_rect.y;
                        sec_rect.y = 0;
                    }
                    m_wndEditor2->SetSize( sec_rect );
                #endif
                    m_wndEditor2->Show();

                    SetupChildEventHandling(m_wndEditor2);

                    // If no primary editor, focus to button to allow
                    // it to interprete ENTER etc.
                    // NOTE: Due to problems focusing away from it, this
                    //       has been disabled.
                    /*
                    if ( (flags & wxPG_SEL_FOCUS) && !m_wndEditor )
                        m_wndEditor2->SetFocus();
                    */
                }

                if ( flags & wxPG_SEL_FOCUS )
                    m_editorFocused = 1;

            }
            else
            {
                // Make sure focus is in grid canvas (important for wxGTK, at least)
                SetFocusOnCanvas();
            }

            EditorsValueWasNotModified();

            // If it's inside collapsed section, expand parent, scroll, etc.
            // Also, if it was partially visible, scroll it into view.
            if ( !(flags & wxPG_SEL_NONVISIBLE) )
                EnsureVisible( p );

            if ( m_wndEditor )
            {
            #if wxPG_CREATE_CONTROLS_HIDDEN
                m_wndEditor->Thaw();
            #endif
                m_wndEditor->Show(true);
            }

            DrawItems(p, p);
        }
        else
        {
            // Make sure focus is in grid canvas
            SetFocusOnCanvas();
        }

        ClearInternalFlag(wxPG_FL_IN_SELECT_PROPERTY);
    }

#if wxUSE_STATUSBAR

    //
    // Show help text in status bar.
    //   (if found and grid not embedded in manager with help box and
    //    style wxPG_EX_HELP_AS_TOOLTIPS is not used).
    //

    if ( !(GetExtraStyle() & wxPG_EX_HELP_AS_TOOLTIPS) )
    {
        wxStatusBar* statusbar = (wxStatusBar*) NULL;
        if ( !(m_iFlags & wxPG_FL_NOSTATUSBARHELP) )
        {
            wxFrame* frame = wxDynamicCast(::wxGetTopLevelParent(this),wxFrame);
            if ( frame )
                statusbar = frame->GetStatusBar();
        }

        if ( statusbar )
        {
            const wxString* pHelpString = (const wxString*) NULL;

            if ( p )
            {
                pHelpString = &p->GetHelpString();
                if ( pHelpString->length() )
                {
                    // Set help box text.
                    statusbar->SetStatusText( *pHelpString );
                    m_iFlags |= wxPG_FL_STRING_IN_STATUSBAR;
                }
            }

            if ( (!pHelpString || !pHelpString->length()) &&
                 (m_iFlags & wxPG_FL_STRING_IN_STATUSBAR) )
            {
                // Clear help box - but only if it was written
                // by us at previous time.
                statusbar->SetStatusText( m_emptyString );
                m_iFlags &= ~(wxPG_FL_STRING_IN_STATUSBAR);
            }
        }
    }
#endif

    m_inDoSelectProperty = 0;

    // call wx event handler (here so that it also occurs on deselection)
    SendEvent( wxEVT_PG_SELECTED, m_selected, NULL, flags );

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::UnfocusEditor()
{
    if ( !m_selected || !m_wndEditor || m_frozen )
        return true;

    if ( !CommitChangesFromEditor(0) )
        return false;

    SetFocusOnCanvas();
    DrawItem(m_selected);

    return true;
}

// -----------------------------------------------------------------------

// This method is not inline because it called dozens of times
// (i.e. two-arg function calls create smaller code size).
bool wxPropertyGrid::DoClearSelection()
{
    return DoSelectProperty((wxPGProperty*)NULL);
}

// -----------------------------------------------------------------------
// wxPropertyGrid expand/collapse state
// -----------------------------------------------------------------------

bool wxPropertyGrid::DoCollapse( wxPGProperty* p, bool sendEvents )
{
    wxPGProperty* pwc = wxStaticCast(p, wxPGProperty);

    // If active editor was inside collapsed section, then disable it
    if ( m_selected && m_selected->IsSomeParent (p) )
    {
        if ( !ClearSelection() )
            return false;
    }

    // Store dont-center-splitter flag 'cause we need to temporarily set it
    wxUint32 old_flag = m_iFlags & wxPG_FL_DONT_CENTER_SPLITTER;
    m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    bool res = m_pState->DoCollapse(pwc);

    if ( res )
    {
        if ( sendEvents )
            SendEvent( wxEVT_PG_ITEM_COLLAPSED, p );

        RecalculateVirtualSize();

        // Redraw etc. only if collapsed was visible.
        if (pwc->IsVisible() &&
            !m_frozen &&
            ( !pwc->IsCategory() || !(m_windowStyle & wxPG_HIDE_CATEGORIES) ) )
        {
            // When item is collapsed so that scrollbar would move,
            // graphics mess is about (unless we redraw everything).
            Refresh();
        }
    }

    // Clear dont-center-splitter flag if it wasn't set
    m_iFlags = (m_iFlags & ~wxPG_FL_DONT_CENTER_SPLITTER) | old_flag;

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoExpand( wxPGProperty* p, bool sendEvents )
{
    wxCHECK_MSG( p, false, wxT("invalid property id") );

    wxPGProperty* pwc = (wxPGProperty*)p;

    // Store dont-center-splitter flag 'cause we need to temporarily set it
    wxUint32 old_flag = m_iFlags & wxPG_FL_DONT_CENTER_SPLITTER;
    m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

    bool res = m_pState->DoExpand(pwc);

    if ( res )
    {
        if ( sendEvents )
            SendEvent( wxEVT_PG_ITEM_EXPANDED, p );

        RecalculateVirtualSize();

        // Redraw etc. only if expanded was visible.
        if ( pwc->IsVisible() && !m_frozen &&
             ( !pwc->IsCategory() || !(m_windowStyle & wxPG_HIDE_CATEGORIES) )
           )
        {
            // Redraw
        #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
            Refresh();
        #else
            DrawItems(pwc, NULL);
        #endif
        }
    }

    // Clear dont-center-splitter flag if it wasn't set
    m_iFlags = m_iFlags & ~(wxPG_FL_DONT_CENTER_SPLITTER) | old_flag;

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoHideProperty( wxPGProperty* p, bool hide, int flags )
{
    if ( m_frozen )
        return m_pState->DoHideProperty(p, hide, flags);

    if ( m_selected &&
         ( m_selected == p || m_selected->IsSomeParent(p) )
       )
        {
            if ( !ClearSelection() )
                return false;
        }

    m_pState->DoHideProperty(p, hide, flags);

    RecalculateVirtualSize();
    Refresh();

    return true;
}


// -----------------------------------------------------------------------
// wxPropertyGrid size related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::RecalculateVirtualSize( int forceXPos )
{
    if ( (m_iFlags & wxPG_FL_RECALCULATING_VIRTUAL_SIZE) || m_frozen )
        return;

    //
    // If virtual height was changed, then recalculate editor control position(s)
    if ( m_pState->m_vhCalcPending )
        CorrectEditorWidgetPosY();

    m_pState->EnsureVirtualHeight();

#ifdef __WXDEBUG__
    int by1 = m_pState->GetVirtualHeight();
    int by2 = m_pState->GetActualVirtualHeight();
    if ( by1 != by2 )
    {
        wxString s = wxString::Format(wxT("VirtualHeight=%i, ActualVirtualHeight=%i, should match!"), by1, by2);
        wxASSERT_MSG( false,
                      s.c_str() );
        wxLogDebug(s);
    }
#endif

    m_iFlags |= wxPG_FL_RECALCULATING_VIRTUAL_SIZE;

    int x = m_pState->m_width;
    int y = m_pState->m_virtualHeight;

    int width, height;
    GetClientSize(&width,&height);

    // Now adjust virtual size.
	SetVirtualSize(x, y);

    int xAmount = 0;
    int xPos = 0;

    //
    // Adjust scrollbars
    if ( HasVirtualWidth() )
    {
        xAmount = x/wxPG_PIXELS_PER_UNIT;
        xPos = GetScrollPos( wxHORIZONTAL );
    }

    if ( forceXPos != -1 )
        xPos = forceXPos;
    // xPos too high?
    else if ( xPos > (xAmount-(width/wxPG_PIXELS_PER_UNIT)) )
        xPos = 0;

    int yAmount = (y+wxPG_PIXELS_PER_UNIT+2)/wxPG_PIXELS_PER_UNIT;
    int yPos = GetScrollPos( wxVERTICAL );

    SetScrollbars( wxPG_PIXELS_PER_UNIT, wxPG_PIXELS_PER_UNIT,
                   xAmount, yAmount, xPos, yPos, true );

    // Must re-get size now
    GetClientSize(&width,&height);

    if ( !HasVirtualWidth() )
    {
        m_pState->SetVirtualWidth(width);
        x = width;
    }

    m_width = width;
    m_height = height;

    m_canvas->SetSize( x, y );

    m_pState->CheckColumnWidths();

    if ( m_selected )
        CorrectEditorWidgetSizeX();

    m_iFlags &= ~wxPG_FL_RECALCULATING_VIRTUAL_SIZE;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnResize( wxSizeEvent& event )
{
    if ( !(m_iFlags & wxPG_FL_INITIALIZED) )
        return;

    int width, height;
    GetClientSize(&width,&height);

    m_width = width;
    m_height = height;

#if wxPG_DOUBLE_BUFFER
    if ( !(GetExtraStyle() & wxPG_EX_NATIVE_DOUBLE_BUFFERING) )
    {
        int dblh = (m_lineHeight*2);
        if ( !m_doubleBuffer )
        {
            // Create double buffer bitmap to draw on, if none
            int w = (width>250)?width:250;
            int h = height + dblh;
            h = (h>400)?h:400;
            m_doubleBuffer = new wxBitmap( w, h );
        }
        else
        {
            int w = m_doubleBuffer->GetWidth();
            int h = m_doubleBuffer->GetHeight();

            // Double buffer must be large enough
            if ( w < width || h < (height+dblh) )
            {
                if ( w < width ) w = width;
                if ( h < (height+dblh) ) h = height + dblh;
                delete m_doubleBuffer;
                m_doubleBuffer = new wxBitmap( w, h );
            }
        }
    }

#endif

    m_pState->OnClientWidthChange( width, event.GetSize().x - m_ncWidth, true );
    m_ncWidth = event.GetSize().x;

    if ( !m_frozen )
    {
        if ( m_pState->m_itemsAdded )
            PrepareAfterItemsAdded();
        else
            // Without this, virtual size (atleast under wxGTK) will be skewed
            RecalculateVirtualSize();

        Refresh();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetVirtualWidth( int width )
{
    if ( width == -1 )
    {
        // Disable virtual width
        width = GetClientSize().x;
        ClearInternalFlag(wxPG_FL_HAS_VIRTUAL_WIDTH);
    }
    else
    {
        // Enable virtual width
        SetInternalFlag(wxPG_FL_HAS_VIRTUAL_WIDTH);
    }
    m_pState->SetVirtualWidth( width );
}

void wxPropertyGrid::SetFocusOnCanvas()
{
    m_canvas->SetFocusIgnoringChildren();
    m_editorFocused = 0;
}

// -----------------------------------------------------------------------
// wxPropertyGrid mouse event handling
// -----------------------------------------------------------------------

// selFlags uses same values DoSelectProperty's flags
// Returns true if event was vetoed.
bool wxPropertyGrid::SendEvent( int eventType, wxPGProperty* p, wxVariant* pValue, unsigned int WXUNUSED(selFlags) )
{
    // Send property grid event of specific type and with specific property
    wxPropertyGridEvent evt( eventType, m_eventObject->GetId() );
    evt.SetPropertyGrid(this);
    evt.SetEventObject(m_eventObject);
    evt.SetProperty(p);
    if ( pValue )
    {
        evt.SetCanVeto(true);
        evt.SetupValidationInfo();
        m_validationInfo.m_pValue = pValue;
    }
    wxEvtHandler* evtHandler = m_eventObject->GetEventHandler();

    evtHandler->ProcessEvent(evt);

    return evt.WasVetoed();
}

// -----------------------------------------------------------------------

// Return false if should be skipped
bool wxPropertyGrid::HandleMouseClick( int x, unsigned int y, wxMouseEvent &event )
{
    bool res = true;

    // Need to set focus?
    if ( !(m_iFlags & wxPG_FL_FOCUSED) )
    {
        SetFocusOnCanvas();
    }

    wxPropertyGridPageState* state = m_pState;
    int splitterHit;
    int splitterHitOffset;
    int columnHit = state->HitTestH( x, &splitterHit, &splitterHitOffset );

    wxPGProperty* p = DoGetItemAtY(y);

    if ( p )
    {
        int depth = (int)p->GetDepth() - 1;

        int marginEnds = m_marginWidth + ( depth * m_subgroup_extramargin );

        if ( x >= marginEnds )
        {
            // Outside margin.

            if ( p->IsCategory() )
            {
                // This is category.
                wxPropertyCategory* pwc = (wxPropertyCategory*)p;

                int textX = m_marginWidth + ((unsigned int)((pwc->m_depth-1)*m_subgroup_extramargin));

                // Expand, collapse, activate etc. if click on text or left of splitter.
                if ( x >= textX
                     &&
                     ( x < (textX+pwc->GetTextExtent(this, m_captionFont)+(wxPG_CAPRECTXMARGIN*2)) ||
                       columnHit == 0
                     )
                    )
                {
                    if ( !DoSelectProperty( p ) )
                        return res;

                    // On double-click, expand/collapse.
                    if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                    {
                        if ( pwc->IsExpanded() ) DoCollapse( p, true );
                        else DoExpand( p, true );
                    }
                }
            }
            else if ( splitterHit == -1 )
            {
            // Click on value.
                unsigned int selFlag = 0;
                if ( columnHit == 1 )
                {
                    m_iFlags |= wxPG_FL_ACTIVATION_BY_CLICK;
                    selFlag = wxPG_SEL_FOCUS;
                }
                if ( !DoSelectProperty( p, selFlag ) )
                    return res;

                m_iFlags &= ~(wxPG_FL_ACTIVATION_BY_CLICK);

                if ( p->GetChildCount() && !p->IsCategory() )
                    // On double-click, expand/collapse.
                    if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                    {
                        wxPGProperty* pwc = (wxPGProperty*)p;
                        if ( pwc->IsExpanded() ) DoCollapse( p, true );
                        else DoExpand( p, true );
                    }

                res = false;
            }
            else
            {
            // click on splitter
                if ( !(m_windowStyle & wxPG_STATIC_SPLITTER) )
                {
                    if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
                    {
                        // Double-clicking the splitter causes auto-centering
                        CenterSplitter( true );
                    }
                    else if ( m_dragStatus == 0 )
                    {
                    //
                    // Begin draggin the splitter
                    //
                        if ( m_wndEditor )
                        {
                            // Changes must be committed here or the
                            // value won't be drawn correctly
                            if ( !CommitChangesFromEditor() )
                                return res;

                            m_wndEditor->Show ( false );
                        }

                        if ( !(m_iFlags & wxPG_FL_MOUSE_CAPTURED) )
                        {
                            m_canvas->CaptureMouse();
                            m_iFlags |= wxPG_FL_MOUSE_CAPTURED;
                        }

                        m_dragStatus = 1;
                        m_draggedSplitter = splitterHit;
                        m_dragOffset = splitterHitOffset;

                        wxClientDC dc(m_canvas);

                    #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
                        // Fixes button disappearance bug
                        if ( m_wndEditor2 )
                            m_wndEditor2->Show ( false );
                    #endif

                        m_startingSplitterX = x - splitterHitOffset;
                    }
                }
            }
        }
        else
        {
        // Click on margin.
            if ( p->GetChildCount() )
            {
                int nx = x + m_marginWidth - marginEnds; // Normalize x.

                if ( (nx >= m_gutterWidth && nx < (m_gutterWidth+m_iconWidth)) )
                {
                    int y2 = y % m_lineHeight;
                    if ( (y2 >= m_buttonSpacingY && y2 < (m_buttonSpacingY+m_iconHeight)) )
                    {
                        // On click on expander button, expand/collapse
                        if ( ((wxPGProperty*)p)->IsExpanded() )
                            DoCollapse( p, true );
                        else
                            DoExpand( p, true );
                    }
                }
            }
        }
    }
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleMouseRightClick( int WXUNUSED(x), unsigned int WXUNUSED(y),
                                            wxMouseEvent& WXUNUSED(event) )
{
    if ( m_propHover )
    {
        // Select property here as well
        wxPGProperty* p = m_propHover;
        if ( p != m_selected )
            DoSelectProperty( p );

        // Send right click event.
        SendEvent( wxEVT_PG_RIGHT_CLICK, p );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleMouseDoubleClick( int WXUNUSED(x), unsigned int WXUNUSED(y),
                                             wxMouseEvent& WXUNUSED(event) )
{
    if ( m_propHover )
    {
        // Select property here as well
        wxPGProperty* p = m_propHover;

        if ( p != m_selected )
            DoSelectProperty( p );

        // Send double-click event.
        SendEvent( wxEVT_PG_DOUBLE_CLICK, m_propHover );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

#if wxPG_SUPPORT_TOOLTIPS

void wxPropertyGrid::SetToolTip( const wxString& tipString )
{
    if ( tipString.length() )
    {
        m_canvas->SetToolTip(tipString);
    }
    else
    {
    #if wxPG_ALLOW_EMPTY_TOOLTIPS
        m_canvas->SetToolTip( m_emptyString );
    #else
        m_canvas->SetToolTip( NULL );
    #endif
    }
}

#endif // #if wxPG_SUPPORT_TOOLTIPS

// -----------------------------------------------------------------------

// Return false if should be skipped
bool wxPropertyGrid::HandleMouseMove( int x, unsigned int y, wxMouseEvent &event )
{
    // Safety check (needed because mouse capturing may
    // otherwise freeze the control)
    if ( m_dragStatus > 0 && !event.Dragging() )
    {
        HandleMouseUp(x,y,event);
    }

    wxPropertyGridPageState* state = m_pState;
    int splitterHit;
    int splitterHitOffset;
    int columnHit = state->HitTestH( x, &splitterHit, &splitterHitOffset );
    int splitterX = x - splitterHitOffset;

    if ( m_dragStatus > 0 )
    {
        if ( x > (m_marginWidth + wxPG_DRAG_MARGIN) &&
             x < (m_pState->m_width - wxPG_DRAG_MARGIN) )
        {

            int newSplitterX = x - m_dragOffset;
            int splitterX = x - splitterHitOffset;

            // Splitter redraw required?
            if ( newSplitterX != splitterX )
            {
                // Move everything
                SetInternalFlag(wxPG_FL_DONT_CENTER_SPLITTER);
                state->DoSetSplitterPosition( newSplitterX, m_draggedSplitter, false );
                state->m_fSplitterX = (float) newSplitterX;

                if ( m_selected )
                    CorrectEditorWidgetSizeX();

                Update();
                Refresh();
            }

            m_dragStatus = 2;
        }

        return false;
    }
    else
    {

        int ih = m_lineHeight;
        int sy = y;

    #if wxPG_SUPPORT_TOOLTIPS
        wxPGProperty* prevHover = m_propHover;
        unsigned char prevSide = m_mouseSide;
    #endif
        int curPropHoverY = y - (y % ih);

        // On which item it hovers
        if ( ( !m_propHover )
             ||
             ( m_propHover && ( sy < m_propHoverY || sy >= (m_propHoverY+ih) ) )
           )
        {
            // Mouse moves on another property

            m_propHover = DoGetItemAtY(y);
            m_propHoverY = curPropHoverY;

            // Send hover event
            SendEvent( wxEVT_PG_HIGHLIGHTED, m_propHover );
        }

    #if wxPG_SUPPORT_TOOLTIPS
        // Store which side we are on
        m_mouseSide = 0;
        if ( columnHit == 1 )
            m_mouseSide = 2;
        else if ( columnHit == 0 )
            m_mouseSide = 1;

        //
        // If tooltips are enabled, show label or value as a tip
        // in case it doesn't otherwise show in full length.
        //
        if ( m_windowStyle & wxPG_TOOLTIPS )
        {
            wxToolTip* tooltip = m_canvas->GetToolTip();

            if ( m_propHover != prevHover || prevSide != m_mouseSide )
            {
                if ( m_propHover && !m_propHover->IsCategory() )
                {

                    if ( GetExtraStyle() & wxPG_EX_HELP_AS_TOOLTIPS )
                    {
                        // Show help string as a tooltip
                        wxString tipString = m_propHover->GetHelpString();

                        SetToolTip(tipString);
                    }
                    else
                    {
                        // Show cropped value string as a tooltip
                        wxString tipString;
                        int space = 0;

                        if ( m_mouseSide == 1 )
                        {
                            tipString = m_propHover->m_label;
                            space = splitterX-m_marginWidth-3;
                        }
                        else if ( m_mouseSide == 2 )
                        {
                            tipString = m_propHover->GetDisplayedString();

                            space = m_width - splitterX;
                            if ( m_propHover->m_flags & wxPG_PROP_CUSTOMIMAGE )
                                space -= wxPG_CUSTOM_IMAGE_WIDTH + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
                        }

                        if ( space )
                        {
                            int tw, th;
    	                    GetTextExtent( tipString, &tw, &th, 0, 0, &m_font );
                            if ( tw > space )
                            {
                                SetToolTip( tipString );
                            }
                        }
                        else
                        {
                            if ( tooltip )
                            {
                            #if wxPG_ALLOW_EMPTY_TOOLTIPS
                                m_canvas->SetToolTip( m_emptyString );
                            #else
                                m_canvas->SetToolTip( NULL );
                            #endif
                            }
                        }

                    }
                }
                else
                {
                    if ( tooltip )
                    {
                    #if wxPG_ALLOW_EMPTY_TOOLTIPS
                        m_canvas->SetToolTip( m_emptyString );
                    #else
                        m_canvas->SetToolTip( NULL );
                    #endif
                    }
                }
            }
        }
    #endif

        if ( splitterHit == -1 ||
             !m_propHover ||
             HasFlag(wxPG_STATIC_SPLITTER) )
        {
            // hovering on something else
            if ( m_curcursor != wxCURSOR_ARROW )
                CustomSetCursor( wxCURSOR_ARROW );
        }
        else
        {
            // Do not allow splitter cursor on caption items.
            // (also not if we were dragging and its started
            // outside the splitter region)

            if ( m_propHover &&
                 !m_propHover->IsCategory() &&
                 !event.Dragging() )
            {

                // hovering on splitter

                // NB: Condition disabled since MouseLeave event (from the editor control) cannot be
                //     reliably detected.
                //if ( m_curcursor != wxCURSOR_SIZEWE )
                CustomSetCursor( wxCURSOR_SIZEWE, true );

                return false;
            }
            else
            {
                // hovering on something else
                if ( m_curcursor != wxCURSOR_ARROW )
                    CustomSetCursor( wxCURSOR_ARROW );
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------

// Also handles Leaving event
bool wxPropertyGrid::HandleMouseUp( int x, unsigned int WXUNUSED(y),
                                    wxMouseEvent &WXUNUSED(event) )
{
    wxPropertyGridPageState* state = m_pState;
    bool res = false;

    int splitterHit;
    int splitterHitOffset;
    state->HitTestH( x, &splitterHit, &splitterHitOffset );

    // No event type check - basicly calling this method should
    // just stop dragging.
    // Left up after dragged?
    if ( m_dragStatus >= 1 )
    {
    //
    // End Splitter Dragging
    //
        // DO NOT ENABLE FOLLOWING LINE!
        // (it is only here as a reminder to not to do it)
        //splitterX = x;

        // Disable splitter auto-centering
        m_iFlags |= wxPG_FL_DONT_CENTER_SPLITTER;

        // This is necessary to return cursor
        if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
        {
            m_canvas->ReleaseMouse();
            m_iFlags &= ~(wxPG_FL_MOUSE_CAPTURED);
        }

        // Set back the default cursor, if necessary
        if ( splitterHit == -1 ||
             !m_propHover )
        {
            CustomSetCursor( wxCURSOR_ARROW );
        }

        m_dragStatus = 0;

        // Control background needs to be cleared
        if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) && m_selected )
            DrawItem( m_selected );

        if ( m_wndEditor )
        {
            m_wndEditor->Show ( true );
        }

    #if wxPG_REFRESH_CONTROLS_AFTER_REPAINT
        // Fixes button disappearance bug
        if ( m_wndEditor2 )
            m_wndEditor2->Show ( true );
    #endif

        // This clears the focus.
        m_editorFocused = 0;

    }
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::OnMouseCommon( wxMouseEvent& event, int* px, int* py )
{
    int splitterX = GetSplitterPosition();

    //int ux, uy;
    //CalcUnscrolledPosition( event.m_x, event.m_y, &ux, &uy );
    int ux = event.m_x;
    int uy = event.m_y;

    wxWindow* wnd = GetEditorControl();

    // Hide popup on clicks
    if ( event.GetEventType() != wxEVT_MOTION )
        if ( wnd && wnd->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)) )
        {
            ((wxOwnerDrawnComboBox*)wnd)->HidePopup();
        }

    wxRect r;
    if ( wnd )
        r = wnd->GetRect();
    if ( wnd == (wxWindow*) NULL || m_dragStatus ||
         (
           ux <= (splitterX + wxPG_SPLITTERX_DETECTMARGIN2) ||
           ux >= (r.x+r.width) ||
           event.m_y < r.y ||
           event.m_y >= (r.y+r.height)
         )
       )
    {
        *px = ux;
        *py = uy;
        return true;
    }
    else
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
    }
    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseClick( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon( event, &x, &y ) )
    {
        HandleMouseClick(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseRightClick( wxMouseEvent &event )
{
    int x, y;
    CalcUnscrolledPosition( event.m_x, event.m_y, &x, &y );
    HandleMouseRightClick(x,y,event);
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseDoubleClick( wxMouseEvent &event )
{
    // Always run standard mouse-down handler as well
    OnMouseClick(event);

    int x, y;
    CalcUnscrolledPosition( event.m_x, event.m_y, &x, &y );
    HandleMouseDoubleClick(x,y,event);
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseMove( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon( event, &x, &y ) )
    {
        HandleMouseMove(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseMoveBottom( wxMouseEvent& WXUNUSED(event) )
{
    // Called when mouse moves in the empty space below the properties.
    CustomSetCursor( wxCURSOR_ARROW );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseUp( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon( event, &x, &y ) )
    {
        HandleMouseUp(x,y,event);
    }
    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnMouseEntry( wxMouseEvent &event )
{
    // This may get called from child control as well, so event's
    // mouse position cannot be relied on.

    if ( event.Entering() )
    {
        if ( !(m_iFlags & wxPG_FL_MOUSE_INSIDE) )
        {
            // TODO: Fix this (detect parent and only do
            //   cursor trick if it is a manager).
            wxASSERT( GetParent() );
            GetParent()->SetCursor(wxNullCursor);

            m_iFlags |= wxPG_FL_MOUSE_INSIDE;
        }
        else
            GetParent()->SetCursor(wxNullCursor);
    }
    else if ( event.Leaving() )
    {
        // Without this, wxSpinCtrl editor will sometimes have wrong cursor
        m_canvas->SetCursor( wxNullCursor );

        // Get real cursor position
        wxPoint pt = ScreenToClient(::wxGetMousePosition());

        if ( ( pt.x <= 0 || pt.y <= 0 || pt.x >= m_width || pt.y >= m_height ) )
        {
            {
                if ( (m_iFlags & wxPG_FL_MOUSE_INSIDE) )
                {
                    m_iFlags &= ~(wxPG_FL_MOUSE_INSIDE);
                }

                if ( m_dragStatus )
                    wxPropertyGrid::HandleMouseUp ( -1, 10000, event );
            }
        }
    }

    event.Skip();
}

// -----------------------------------------------------------------------

// Common code used by various OnMouseXXXChild methods.
bool wxPropertyGrid::OnMouseChildCommon( wxMouseEvent &event, int* px, int *py )
{
    wxWindow* topCtrlWnd = (wxWindow*)event.GetEventObject();
    wxASSERT( topCtrlWnd );
    int x, y;
    event.GetPosition(&x,&y);

    int splitterX = GetSplitterPosition();

    wxRect r = topCtrlWnd->GetRect();
    if ( !m_dragStatus &&
         x > (splitterX-r.x+wxPG_SPLITTERX_DETECTMARGIN2) &&
         y >= 0 && y < r.height \
       )
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
        event.Skip();
    }
    else
    {
        CalcUnscrolledPosition( event.m_x + r.x, event.m_y + r.y, \
            px, py );
        return true;
    }
    return false;
}

void wxPropertyGrid::OnMouseClickChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseClick(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseRightClickChild( wxMouseEvent &event )
{
    int x,y;
    wxASSERT( m_wndEditor );
    // These coords may not be exact (about +-2),
    // but that should not matter (right click is about item, not position).
    wxPoint pt = m_wndEditor->GetPosition();
    CalcUnscrolledPosition( event.m_x + pt.x, event.m_y + pt.y, &x, &y );
    wxASSERT( m_selected );
    m_propHover = m_selected;
    bool res = HandleMouseRightClick(x,y,event);
    if ( !res ) event.Skip();
}

void wxPropertyGrid::OnMouseMoveChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseMove(x,y,event);
        if ( !res ) event.Skip();
    }
}

void wxPropertyGrid::OnMouseUpChild( wxMouseEvent &event )
{
    int x,y;
    if ( OnMouseChildCommon(event,&x,&y) )
    {
        bool res = HandleMouseUp(x,y,event);
        if ( !res ) event.Skip();
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid keyboard event handling
// -----------------------------------------------------------------------

int wxPropertyGrid::KeyEventToActions(wxKeyEvent &event, int* pSecond) const
{
    // Translates wxKeyEvent to wxPG_ACTION_XXX

    int keycode = event.GetKeyCode();
    int modifiers = event.GetModifiers();

    wxASSERT( !(modifiers&~(0xFFFF)) );

    int hashMapKey = (keycode & 0xFFFF) | ((modifiers & 0xFFFF) << 16);

    wxPGHashMapI2I::const_iterator it = m_actionTriggers.find(hashMapKey);

    if ( it == m_actionTriggers.end() )
        return 0;

    if ( pSecond )
    {
        int second = (it->second>>16) & 0xFFFF;
        *pSecond = second;
    }

    return (it->second & 0xFFFF);
}

void wxPropertyGrid::AddActionTrigger( int action, int keycode, int modifiers )
{
    wxASSERT( !(modifiers&~(0xFFFF)) );

    int hashMapKey = (keycode & 0xFFFF) | ((modifiers & 0xFFFF) << 16);

    wxPGHashMapI2I::iterator it = m_actionTriggers.find(hashMapKey);

    if ( it != m_actionTriggers.end() )
    {
        // This key combination is already used

        // Can add secondary?
        wxASSERT_MSG( !(it->second&~(0xFFFF)),
                      wxT("You can only add up to two separate actions per key combination.") );

        action = it->second | (action<<16);
    }

    m_actionTriggers[hashMapKey] = action;
}

void wxPropertyGrid::ClearActionTriggers( int action )
{
    wxPGHashMapI2I::iterator it;

    for ( it = m_actionTriggers.begin(); it != m_actionTriggers.end(); it++ )
    {
        if ( it->second == action )
        {
            m_actionTriggers.erase(it);
        }
    }
}

static void CopyTextToClipboard( const wxString& text )
{
    if ( wxTheClipboard->Open() )
    {
        // This data objects are held by the clipboard, 
        // so do not delete them in the app.
        wxTheClipboard->SetData( new wxTextDataObject(text) );
        wxTheClipboard->Close();
    }
}

void wxPropertyGrid::HandleKeyEvent(wxKeyEvent &event)
{
    //
    // Handles key event when editor control is not focused.
    //

    wxASSERT( !m_frozen );
    if ( m_frozen )
        return;

    // Travelsal between items, collapsing/expanding, etc.
    int keycode = event.GetKeyCode();

    if ( keycode == WXK_TAB )
    {
        if (m_selected)
            DoSelectProperty( m_selected, wxPG_SEL_FOCUS );
        return;
    }

    // Ignore Alt and Control when they are down alone
    if ( keycode == WXK_ALT ||
         keycode == WXK_CONTROL )
    {
        event.Skip();
        return;
    }

    int secondAction;
    int action = KeyEventToActions(event, &secondAction);

    if ( m_selected )
    {

        // Show dialog?
        if ( ButtonTriggerKeyTest(action, event) )
            return;

        wxPGProperty* p = m_selected;

        if ( action == wxPG_ACTION_COPY )
        {
            CopyTextToClipboard(p->GetDisplayedString());
        }
        else
        {
            // Travel and expand/collapse
            int selectDir = -2;

            if ( p->GetChildCount() &&
                 !(p->m_flags & wxPG_PROP_DISABLED)
               )
            {
                if ( action == wxPG_ACTION_COLLAPSE_PROPERTY || secondAction == wxPG_ACTION_COLLAPSE_PROPERTY )
                {
                    if ( (m_windowStyle & wxPG_HIDE_MARGIN) || Collapse(p) )
                        keycode = 0;
                }
                else if ( action == wxPG_ACTION_EXPAND_PROPERTY || secondAction == wxPG_ACTION_EXPAND_PROPERTY )
                {
                    if ( (m_windowStyle & wxPG_HIDE_MARGIN) || Expand(p) )
                        keycode = 0;
                }
            }

            if ( keycode )
            {
                if ( action == wxPG_ACTION_PREV_PROPERTY || secondAction == wxPG_ACTION_PREV_PROPERTY )
                {
                    selectDir = -1;
                }
                else if ( action == wxPG_ACTION_NEXT_PROPERTY || secondAction == wxPG_ACTION_NEXT_PROPERTY )
                {
                    selectDir = 1;
                }
                else
                {
                    event.Skip();
                }

            }

            if ( selectDir >= -1 )
            {
                p = wxPropertyGridIterator::OneStep( m_pState, wxPG_ITERATE_VISIBLE, p, selectDir );
                if ( p )
                    DoSelectProperty(p);
            }
        }
    }
    else
    {
        // If nothing was selected, select the first item now
        // (or navigate out of tab).
        if ( action != wxPG_ACTION_CANCEL_EDIT && secondAction != wxPG_ACTION_CANCEL_EDIT )
        {
            wxPGProperty* p = wxPropertyGridInterface::GetFirst();
            if ( p ) DoSelectProperty(p);
        }
    }
}

// -----------------------------------------------------------------------

// Potentially handles a keyboard event for editor controls.
// Returns false if event should *not* be skipped (on true it can
// be optionally skipped).
// Basicly, false means that SelectProperty was called (or was about
// to be called, if canDestroy was false).
bool wxPropertyGrid::HandleChildKey( wxKeyEvent& event )
{
    bool res = true;

    if ( !m_selected || !m_wndEditor )
    {
        return true;
    }

    int action = KeyEventToAction(event);

    // Unfocus?
    if ( action == wxPG_ACTION_CANCEL_EDIT )
    {
        //
        // Esc cancels any changes
        if ( IsEditorsValueModified() )
        {
            EditorsValueWasNotModified();

            // Update the control as well
            m_selected->GetEditorClass()->SetControlStringValue( m_selected,
                                                                 GetEditorControl(),
                                                                 m_selected->GetDisplayedString() );
        }

        OnValidationFailureReset(m_selected);

        res = false;
        UnfocusEditor();
    }
    else if ( action == wxPG_ACTION_COPY )
    {
        // NB: There is some problem with getting native cut-copy-paste keys to go through
        //     for embedded editor wxTextCtrl. This is why we emulate.
        //
        wxTextCtrl* tc = GetEditorTextCtrl();
        if ( tc )
        {
            wxString sel = tc->GetStringSelection();
            if ( sel.length() )
                CopyTextToClipboard(sel);
        }
        else
        {
            CopyTextToClipboard(m_selected->GetDisplayedString());
        }
    }
    else if ( action == wxPG_ACTION_CUT )
    {
        wxTextCtrl* tc = GetEditorTextCtrl();
        if ( tc )
        {
            long from, to;
            tc->GetSelection(&from, &to);
            if ( from < to )
            {
                CopyTextToClipboard(tc->GetStringSelection());
                tc->Remove(from, to);
            }
        }
    }
    else if ( action == wxPG_ACTION_PASTE )
    {
        wxTextCtrl* tc = GetEditorTextCtrl();
        if ( tc )
        {
            if (wxTheClipboard->Open())
            {
                if (wxTheClipboard->IsSupported( wxDF_TEXT ))
                {
                    wxTextDataObject data;
                    wxTheClipboard->GetData( data );
                    long from, to;
                    tc->GetSelection(&from, &to);
                    if ( from < to )
                    {
                        tc->Remove(from, to);
                        tc->WriteText(data.GetText());
                    }
                    else
                    {
                        tc->WriteText(data.GetText());
                    }
                }  
                wxTheClipboard->Close();
            }
        }
    }

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnKey( wxKeyEvent &event )
{
    HandleKeyEvent( event );
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnKeyUp(wxKeyEvent &event)
{
    event.Skip();
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::ButtonTriggerKeyTest( int action, wxKeyEvent& event )
{
    if ( action == -1 )
    {
        int secondAction;
        action = KeyEventToActions(event, &secondAction);
    }

    // Does the keycode trigger button?
    if ( action == wxPG_ACTION_PRESS_BUTTON &&
         m_wndEditor2 )
    {
        wxCommandEvent evt(wxEVT_COMMAND_BUTTON_CLICKED, m_wndEditor2->GetId());
        GetEventHandler()->AddPendingEvent(evt);
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnChildKeyDown( wxKeyEvent &event )
{
    int keycode = event.GetKeyCode();

    // Ignore Alt and Control when they are down alone
    if ( keycode == WXK_ALT ||
         keycode == WXK_CONTROL )
    {
        event.Skip();
        return;
    }

    if ( ButtonTriggerKeyTest(-1, event) )
        return;

    if ( HandleChildKey(event) == true )
        event.Skip();

    GetEventHandler()->AddPendingEvent(event);
}

void wxPropertyGrid::OnChildKeyUp( wxKeyEvent &event )
{
    GetEventHandler()->AddPendingEvent(event);

    event.Skip();
}

// -----------------------------------------------------------------------
// wxPropertyGrid miscellaneous event handling
// -----------------------------------------------------------------------

void wxPropertyGrid::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    //
    // Check if the focus is in this control or one of its children
    wxWindow* newFocused = wxWindow::FindFocus();

    if ( newFocused != m_curFocused )
        HandleFocusChange( newFocused );
}

// Called by focus event handlers. newFocused is the window that becomes focused.
void wxPropertyGrid::HandleFocusChange( wxWindow* newFocused )
{
    unsigned int oldFlags = m_iFlags;

    m_iFlags &= ~(wxPG_FL_FOCUSED);

    wxWindow* parent = newFocused;

    // This must be one of nextFocus' parents.
    while ( parent )
    {
        // Use m_eventObject, which is either wxPropertyGrid or
        // wxPropertyGridManager, as appropriate.
        if ( parent == m_eventObject )
        {
            m_iFlags |= wxPG_FL_FOCUSED;
            break;
        }
        parent = parent->GetParent();
    }

    m_curFocused = newFocused;

    if ( (m_iFlags & wxPG_FL_FOCUSED) !=
         (oldFlags & wxPG_FL_FOCUSED) )
    {
        if ( !(m_iFlags & wxPG_FL_FOCUSED) )
        {
            // Need to store changed value
            CommitChangesFromEditor();
        }
        else
        {
            /*
            //
            // Preliminary code for tab-order respecting
            // tab-traversal (but should be moved to
            // OnNav handler)
            //
            wxWindow* prevFocus = event.GetWindow();
            wxWindow* useThis = this;
            if ( m_iFlags & wxPG_FL_IN_MANAGER )
                useThis = GetParent();

            if ( prevFocus &&
                 prevFocus->GetParent() == useThis->GetParent() )
            {
                wxList& children = useThis->GetParent()->GetChildren();

                wxNode* node = children.Find(prevFocus);

                if ( node->GetNext() &&
                     useThis == node->GetNext()->GetData() )
                    DoSelectProperty(GetFirst());
                else if ( node->GetPrevious () &&
                          useThis == node->GetPrevious()->GetData() )
                    DoSelectProperty(GetLastProperty());

            }
            */
        }

        // Redraw selected
        if ( m_selected && (m_iFlags & wxPG_FL_INITIALIZED) )
            DrawItem( m_selected );
    }
}

void wxPropertyGrid::OnFocusEvent( wxFocusEvent& event )
{
    if ( event.GetEventType() == wxEVT_SET_FOCUS )
        HandleFocusChange((wxWindow*)event.GetEventObject());
    // Line changed to "else" when applying wxPropertyGrid patch #1675902
    //else if ( event.GetWindow() )
    else
        HandleFocusChange(event.GetWindow());

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnChildFocusEvent( wxChildFocusEvent& event )
{
    HandleFocusChange((wxWindow*)event.GetEventObject());

    //
    // event.Skip() being commented out is aworkaround for bug reported
    // in ticket #4840 (wxScrolledWindow problem with automatic scrolling).
    //event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnScrollEvent( wxScrollWinEvent &event )
{
    m_iFlags |= wxPG_FL_SCROLLED;

    event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnCaptureChange( wxMouseCaptureChangedEvent& WXUNUSED(event) )
{
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
    {
        m_iFlags &= ~(wxPG_FL_MOUSE_CAPTURED);
    }
}

// -----------------------------------------------------------------------
// Property editor related functions
// -----------------------------------------------------------------------

// noDefCheck = true prevents infinite recursion.
wxPGEditor* wxPropertyGrid::RegisterEditorClass( wxPGEditor* editorClass,
                                                 bool noDefCheck )
{
    wxASSERT( editorClass );

    if ( !noDefCheck && wxPGGlobalVars->m_mapEditorClasses.empty() )
        RegisterDefaultEditors();

    wxString name = editorClass->GetName();

    // Existing editor under this name?
    wxPGHashMapS2P::iterator vt_it = wxPGGlobalVars->m_mapEditorClasses.find(name);

    if ( vt_it != wxPGGlobalVars->m_mapEditorClasses.end() )
    {
        // If this name was already used, try class name.
        name = editorClass->GetClassInfo()->GetClassName();
        vt_it = wxPGGlobalVars->m_mapEditorClasses.find(name);
    }

    wxCHECK_MSG( vt_it == wxPGGlobalVars->m_mapEditorClasses.end(),
                 (wxPGEditor*) vt_it->second,
                 "Editor with given name was already registered" );

    wxPGGlobalVars->m_mapEditorClasses[name] = (void*)editorClass;

    return editorClass;
}

// Use this in RegisterDefaultEditors.
#define wxPGRegisterDefaultEditorClass(EDITOR) \
    if ( wxPGEditor_##EDITOR == (wxPGEditor*) NULL ) \
    { \
        wxPGEditor_##EDITOR = wxPropertyGrid::RegisterEditorClass( \
            new wxPG##EDITOR##Editor, true ); \
    }

// Registers all default editor classes
void wxPropertyGrid::RegisterDefaultEditors()
{
    wxPGRegisterDefaultEditorClass( TextCtrl );
    wxPGRegisterDefaultEditorClass( Choice );
    wxPGRegisterDefaultEditorClass( ComboBox );
    wxPGRegisterDefaultEditorClass( TextCtrlAndButton );
#if wxPG_INCLUDE_CHECKBOX
    wxPGRegisterDefaultEditorClass( CheckBox );
#endif
    wxPGRegisterDefaultEditorClass( ChoiceAndButton );

    // Register SpinCtrl etc. editors before use
    RegisterAdditionalEditors();
}

// -----------------------------------------------------------------------
// wxPGStringTokenizer
//   Needed to handle C-style string lists (e.g. "str1" "str2")
// -----------------------------------------------------------------------

wxPGStringTokenizer::wxPGStringTokenizer( const wxString& str, wxChar delimeter )
    : m_str(&str), m_curPos(str.begin()), m_delimeter(delimeter)
{
}

wxPGStringTokenizer::~wxPGStringTokenizer()
{
}

bool wxPGStringTokenizer::HasMoreTokens()
{
    const wxString& str = *m_str;

    wxString::const_iterator i = m_curPos;

    wxUniChar delim = m_delimeter;
    wxUniChar a;
    wxUniChar prev_a = wxT('\0');

    bool inToken = false;

    while ( i != str.end() )
    {
        a = *i;

        if ( !inToken )
        {
            if ( a == delim )
            {
                inToken = true;
                m_readyToken.clear();
            }
        }
        else
        {
            if ( prev_a != wxT('\\') )
            {
                if ( a != delim )
                {
                    if ( a != wxT('\\') )
                        m_readyToken << a;
                }
                else
                {
                    i++;
                    m_curPos = i;
                    return true;
                }
                prev_a = a;
            }
            else
            {
                m_readyToken << a;
                prev_a = wxT('\0');
            }
        }
        i++;
    }

    m_curPos = str.end();

    if ( inToken )
        return true;

    return false;
}

wxString wxPGStringTokenizer::GetNextToken()
{
    return m_readyToken;
}

// -----------------------------------------------------------------------
// wxPGChoiceEntry
// -----------------------------------------------------------------------

wxPGChoiceEntry::wxPGChoiceEntry()
    : wxPGCell(), m_value(wxPG_INVALID_VALUE)
{
}

wxPGChoiceEntry::wxPGChoiceEntry( const wxPGChoiceEntry& entry )
    : wxPGCell( entry.GetText(), entry.GetBitmap(),
        entry.GetFgCol(), entry.GetBgCol() ), m_value(entry.GetValue())
{
}

// -----------------------------------------------------------------------
// wxPGChoicesData
// -----------------------------------------------------------------------

wxPGChoicesData::wxPGChoicesData()
{
    m_refCount = 1;
}

wxPGChoicesData::~wxPGChoicesData()
{
    Clear();
}

void wxPGChoicesData::Clear()
{
    unsigned int i;

    for ( i=0; i<m_items.size(); i++ )
    {
        delete Item(i);
    }

    m_items.clear();
}

void wxPGChoicesData::CopyDataFrom( wxPGChoicesData* data )
{
    wxASSERT( m_items.size() == 0 );

    unsigned int i;

    for ( i=0; i<data->GetCount(); i++ )
        m_items.push_back( new wxPGChoiceEntry(*data->Item(i)) );
}

// -----------------------------------------------------------------------
// wxPGChoices
// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Add( const wxString& label, int value )
{
    EnsureData();

    wxPGChoiceEntry* p = new wxPGChoiceEntry(label, value);
    m_data->Insert( -1, p );
    return *p;
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Add( const wxString& label, const wxBitmap& bitmap, int value )
{
    EnsureData();

    wxPGChoiceEntry* p = new wxPGChoiceEntry(label, value);
    p->SetBitmap(bitmap);
    m_data->Insert( -1, p );
    return *p;
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Insert( const wxPGChoiceEntry& entry, int index )
{
    EnsureData();

    wxPGChoiceEntry* p = new wxPGChoiceEntry(entry);
    m_data->Insert(index, p);
    return *p;
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::Insert( const wxString& label, int index, int value )
{
    EnsureData();

    wxPGChoiceEntry* p = new wxPGChoiceEntry(label, value);
    m_data->Insert( index, p );
    return *p;
}

// -----------------------------------------------------------------------

wxPGChoiceEntry& wxPGChoices::AddAsSorted( const wxString& label, int value )
{
    EnsureData();

    size_t index = 0;

    while ( index < GetCount() )
    {
        int cmpRes = GetLabel(index).Cmp(label);
        if ( cmpRes > 0 )
            break;
        index++;
    }

    wxPGChoiceEntry* p = new wxPGChoiceEntry(label, value);
    m_data->Insert( index, p );
    return *p;
}

// -----------------------------------------------------------------------

void wxPGChoices::Add( const wxChar** labels, const ValArrItem* values )
{
    EnsureData();

    unsigned int itemcount = 0;
    const wxChar** p = &labels[0];
    while ( *p ) { p++; itemcount++; }

    unsigned int i;
    for ( i = 0; i < itemcount; i++ )
    {
        int value = wxPG_INVALID_VALUE;
        if ( values )
            value = values[i];
        m_data->Insert( -1, new wxPGChoiceEntry(labels[i], value) );
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Add( const wxArrayString& arr, const ValArrItem* values )
{
    EnsureData();

    unsigned int i;
    unsigned int itemcount = arr.size();

    for ( i = 0; i < itemcount; i++ )
    {
        int value = wxPG_INVALID_VALUE;
        if ( values )
            value = values[i];
        m_data->Insert( -1, new wxPGChoiceEntry(arr[i], value) );
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Add( const wxArrayString& arr, const wxArrayInt& arrint )
{
    EnsureData();

    unsigned int i;
    unsigned int itemcount = arr.size();

    for ( i = 0; i < itemcount; i++ )
    {
        int value = wxPG_INVALID_VALUE;
        if ( &arrint && arrint.size() )
            value = arrint[i];
        m_data->Insert( -1, new wxPGChoiceEntry(arr[i], value) );
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::RemoveAt(size_t nIndex, size_t count)
{
    wxASSERT( m_data->m_refCount != 0xFFFFFFF );
    unsigned int i;
    for ( i=nIndex; i<(nIndex+count); i++)
        delete m_data->Item(i);
    m_data->m_items.erase(m_data->m_items.begin()+nIndex,
                          m_data->m_items.begin()+nIndex+count);
}

// -----------------------------------------------------------------------

int wxPGChoices::Index( const wxString& str ) const
{
    if ( IsOk() )
    {
        unsigned int i;
        for ( i=0; i< m_data->GetCount(); i++ )
        {
            if ( m_data->Item(i)->GetText() == str )
                return i;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------

int wxPGChoices::Index( int val ) const
{
    if ( IsOk() )
    {
        unsigned int i;
        for ( i=0; i< m_data->GetCount(); i++ )
        {
            if ( m_data->Item(i)->GetValue() == val )
                return i;
        }
    }
    return -1;
}

// -----------------------------------------------------------------------

wxArrayString wxPGChoices::GetLabels() const
{
    wxArrayString arr;
    unsigned int i;

    if ( this && IsOk() )
        for ( i=0; i<GetCount(); i++ )
            arr.push_back(GetLabel(i));

    return arr;
}

// -----------------------------------------------------------------------

bool wxPGChoices::HasValues() const
{
    return true;
}

// -----------------------------------------------------------------------

wxArrayInt wxPGChoices::GetValuesForStrings( const wxArrayString& strings ) const
{
    wxArrayInt arr;

    if ( IsOk() )
    {
        unsigned int i;
        for ( i=0; i< strings.size(); i++ )
        {
            int index = Index(strings[i]);
            if ( index >= 0 )
                arr.Add(GetValue(index));
            else
                arr.Add(wxPG_INVALID_VALUE);
        }
    }

    return arr;
}

// -----------------------------------------------------------------------

wxArrayInt wxPGChoices::GetIndicesForStrings( const wxArrayString& strings, 
                                              wxArrayString* unmatched ) const
{
    wxArrayInt arr;

    if ( IsOk() )
    {
        unsigned int i;
        for ( i=0; i< strings.size(); i++ )
        {
            const wxString& str = strings[i];
            int index = Index(str);
            if ( index >= 0 )
                arr.Add(index);
            else if ( unmatched )
                unmatched->Add(str);
        }
    }

    return arr;
}

// -----------------------------------------------------------------------

void wxPGChoices::AssignData( wxPGChoicesData* data )
{
    Free();

    if ( data != wxPGChoicesEmptyData )
    {
        m_data = data;
        data->m_refCount++;
    }
}

// -----------------------------------------------------------------------

void wxPGChoices::Init()
{
    m_data = wxPGChoicesEmptyData;
}

// -----------------------------------------------------------------------

void wxPGChoices::Free()
{
    if ( m_data != wxPGChoicesEmptyData )
    {
        m_data->DecRef();
        m_data = wxPGChoicesEmptyData;
    }
}

// -----------------------------------------------------------------------
// wxPropertyGridEvent
// -----------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPropertyGridEvent, wxCommandEvent)


DEFINE_EVENT_TYPE( wxEVT_PG_SELECTED )
DEFINE_EVENT_TYPE( wxEVT_PG_CHANGING )
DEFINE_EVENT_TYPE( wxEVT_PG_CHANGED )
DEFINE_EVENT_TYPE( wxEVT_PG_HIGHLIGHTED )
DEFINE_EVENT_TYPE( wxEVT_PG_RIGHT_CLICK )
DEFINE_EVENT_TYPE( wxEVT_PG_PAGE_CHANGED )
DEFINE_EVENT_TYPE( wxEVT_PG_ITEM_EXPANDED )
DEFINE_EVENT_TYPE( wxEVT_PG_ITEM_COLLAPSED )
DEFINE_EVENT_TYPE( wxEVT_PG_DOUBLE_CLICK )


// -----------------------------------------------------------------------

void wxPropertyGridEvent::Init()
{
    m_validationInfo = NULL;
    m_canVeto = false;
    m_wasVetoed = false;
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::wxPropertyGridEvent(wxEventType commandType, int id)
    : wxCommandEvent(commandType,id)
{
    m_property = NULL;
    Init();
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::wxPropertyGridEvent(const wxPropertyGridEvent& event)
    : wxCommandEvent(event)
{
    m_eventType = event.GetEventType();
    m_eventObject = event.m_eventObject;
    m_pg = event.m_pg;
    m_property = event.m_property;
    m_validationInfo = event.m_validationInfo;
    m_canVeto = event.m_canVeto;
    m_wasVetoed = event.m_wasVetoed;
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::~wxPropertyGridEvent()
{
}

// -----------------------------------------------------------------------

wxEvent* wxPropertyGridEvent::Clone() const
{
    return new wxPropertyGridEvent( *this );
}

// -----------------------------------------------------------------------
// wxPropertyGridPopulator
// -----------------------------------------------------------------------

wxPropertyGridPopulator::wxPropertyGridPopulator()
{
    m_state = NULL;
    m_pg = NULL;
    wxPGGlobalVars->m_offline++;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::SetState( wxPropertyGridPageState* state )
{
    m_state = state;
    m_propHierarchy.clear();
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::SetGrid( wxPropertyGrid* pg )
{
    m_pg = pg;
    pg->Freeze();
}

// -----------------------------------------------------------------------

wxPropertyGridPopulator::~wxPropertyGridPopulator()
{
    //
    // Free unused sets of choices
    wxPGHashMapS2P::iterator it;

    for( it = m_dictIdChoices.begin(); it != m_dictIdChoices.end(); ++it )
    {
        wxPGChoicesData* data = (wxPGChoicesData*) it->second;
        data->DecRef();
    }

    if ( m_pg )
    {
        m_pg->Thaw();
        m_pg->GetPanel()->Refresh();
    }
    wxPGGlobalVars->m_offline--;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGridPopulator::Add( const wxString& propClass,
                                            const wxString& propLabel,
                                            const wxString& propName,
                                            const wxString* propValue,
                                            wxPGChoices* pChoices )
{
    wxClassInfo* classInfo = wxClassInfo::FindClass(propClass);
    wxPGProperty* parent = GetCurParent();

    if ( parent->HasFlag(wxPG_PROP_AGGREGATE) )
    {
        ProcessError(wxString::Format(wxT("new children cannot be added to '%s'"),parent->GetName().c_str()));
        return NULL;
    }

    if ( !classInfo || !classInfo->IsKindOf(CLASSINFO(wxPGProperty)) )
    {
        ProcessError(wxString::Format(wxT("'%s' is not valid property class"),propClass.c_str()));
        return NULL;
    }

    wxPGProperty* property = (wxPGProperty*) classInfo->CreateObject();

    property->SetLabel(propLabel);
    property->DoSetName(propName);

    if ( pChoices && pChoices->IsOk() )
        property->SetChoices(*pChoices);

    m_state->DoInsert(parent, -1, property);

    if ( propValue )
        property->SetValueFromString( *propValue, wxPG_FULL_VALUE );

    return property;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::AddChildren( wxPGProperty* property )
{
    m_propHierarchy.push_back(property);
    DoScanForChildren();
    m_propHierarchy.pop_back();
}

// -----------------------------------------------------------------------

wxPGChoices wxPropertyGridPopulator::ParseChoices( const wxString& choicesString,
                                                   const wxString& idString )
{
    wxPGChoices choices;

    // Using id?
    if ( choicesString[0] == wxT('@') )
    {
        wxString ids = choicesString.substr(1);
        wxPGHashMapS2P::iterator it = m_dictIdChoices.find(ids);
        if ( it == m_dictIdChoices.end() )
            ProcessError(wxString::Format(wxT("No choices defined for id '%s'"),ids.c_str()));
        else
            choices.AssignData((wxPGChoicesData*)it->second);
    }
    else
    {
        bool found = false;
        if ( idString.length() )
        {
            wxPGHashMapS2P::iterator it = m_dictIdChoices.find(idString);
            if ( it != m_dictIdChoices.end() )
            {
                choices.AssignData((wxPGChoicesData*)it->second);
                found = true;
            }
        }

        if ( !found )
        {
            // Parse choices string
            wxString::const_iterator it = choicesString.begin();
            wxString label;
            wxString value;
            int state = 0;
            bool labelValid = false;

            for ( ; it != choicesString.end(); it++ )
            {
                wxChar c = *it;

                if ( state != 1 )
                {
                    if ( c == wxT('"') )
                    {
                        if ( labelValid )
                        {
                            long l;
                            if ( !value.ToLong(&l, 0) ) l = wxPG_INVALID_VALUE;
                            choices.Add(label, l);
                        }
                        labelValid = false;
                        //wxLogDebug(wxT("%s, %s"),label.c_str(),value.c_str());
                        value.clear();
                        label.clear();
                        state = 1;
                    }
                    else if ( c == wxT('=') )
                    {
                        if ( labelValid )
                        {
                            state = 2;
                        }
                    }
                    else if ( state == 2 && (wxIsalnum(c) || c == wxT('x')) )
                    {
                        value << c;
                    }
                }
                else
                {
                    if ( c == wxT('"') )
                    {
                        state = 0;
                        labelValid = true;
                    }
                    else
                        label << c;
                }
            }

            if ( labelValid )
            {
                long l;
                if ( !value.ToLong(&l, 0) ) l = wxPG_INVALID_VALUE;
                choices.Add(label, l);
            }

            if ( !choices.IsOk() )
            {
                choices.EnsureData();
            }

            // Assign to id
            if ( idString.length() )
                m_dictIdChoices[idString] = choices.GetData();
        }
    }

    return choices;
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::ToLongPCT( const wxString& s, long* pval, long max )
{
    if ( s.Last() == wxT('%') )
    {
        wxString s2 = s.substr(0,s.length()-1);
        long val;
        if ( s2.ToLong(&val, 10) )
        {
            *pval = (val*max)/100;
            return true;
        }
        return false;
    }

    return s.ToLong(pval, 10);
}

// -----------------------------------------------------------------------

bool wxPropertyGridPopulator::AddAttribute( const wxString& name,
                                            const wxString& type,
                                            const wxString& value )
{
    int l = m_propHierarchy.size();
    if ( !l )
        return false;

    wxPGProperty* p = m_propHierarchy[l-1];
    wxString valuel = value.Lower();
    wxVariant variant;

    if ( type.length() == 0 )
    {
        long v;

        // Auto-detect type
        if ( valuel == wxT("true") || valuel == wxT("yes") || valuel == wxT("1") )
            variant = true;
        else if ( valuel == wxT("false") || valuel == wxT("no") || valuel == wxT("0") )
            variant = false;
        else if ( value.ToLong(&v, 0) )
            variant = v;
        else
            variant = value;
    }
    else
    {
        if ( type == wxT("string") )
        {
            variant = value;
        }
        else if ( type == wxT("int") )
        {
            long v = 0;
            value.ToLong(&v, 0);
            variant = v;
        }
        else if ( type == wxT("bool") )
        {
            if ( valuel == wxT("true") || valuel == wxT("yes") || valuel == wxT("1") )
                variant = true;
            else
                variant = false;
        }
        else
        {
            ProcessError(wxString::Format(wxT("Invalid attribute type '%s'"),type.c_str()));
            return false;
        }
    }

    p->SetAttribute( name, variant );

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::ProcessError( const wxString& msg )
{
    wxLogError(_("Error in resource: %s"),msg.c_str());
}

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID
