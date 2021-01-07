/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/propgrid.cpp
// Purpose:     wxPropertyGrid
// Author:      Jaakko Salli
// Modified by:
// Created:     2004-09-25
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


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
    #include "wx/textctrl.h"
    #include "wx/settings.h"
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
#include "wx/scopeguard.h"

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


#define wxPG_GUTTER_DIV                 3 // gutter is max(iconwidth/gutter_div,gutter_min)
#define wxPG_GUTTER_MIN                 3 // gutter before and after image of [+] or [-]
#define wxPG_YSPACING_MIN               1
#define wxPG_DEFAULT_VSPACING           2 // This matches .NET propertygrid's value,
                                          // but causes normal combobox to spill out under MSW
#define wxPG_DEFAULT_CURSOR             wxNullCursor
#define wxPG_PIXELS_PER_UNIT            m_lineHeight

#ifdef wxPG_ICON_WIDTH
  #define m_iconHeight m_iconWidth
#endif

//#define wxPG_TOOLTIP_DELAY              1000

// This is the number of pixels the expander button inside
// property cells (i.e. not in the grey margin area are
// adjusted.
#define IN_CELL_EXPANDER_BUTTON_X_ADJUST    2

#if WXWIN_COMPATIBILITY_3_0
namespace
{
// Hash containing for every active wxPG the list of editors and their event handlers
// to be deleted in the idle event handler.
// It emulates member variable 'm_deletedEditorObjects' in 3.0 compatibility mode.
WX_DECLARE_HASH_MAP(wxPropertyGrid*, wxArrayPGObject*,
                    wxPointerHash, wxPointerEqual,
                    DeletedObjects);

DeletedObjects gs_deletedEditorObjects;

} // anonymous namespace
#endif

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

const char wxPropertyGridNameStr[] = "wxPropertyGrid";

// -----------------------------------------------------------------------
// Statics in one class for easy destruction.
// -----------------------------------------------------------------------

#include "wx/module.h"

class wxPGGlobalVarsClassManager : public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxPGGlobalVarsClassManager);
public:
    wxPGGlobalVarsClassManager() {}
    virtual bool OnInit() wxOVERRIDE { wxPGGlobalVars = new wxPGGlobalVarsClass(); return true; }
    virtual void OnExit() wxOVERRIDE { wxDELETE(wxPGGlobalVars); }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxPGGlobalVarsClassManager, wxModule);


// When wxPG is loaded dynamically after the application is already running
// then the built-in module system won't pick this one up.  Add it manually.
void wxPGInitResourceModule()
{
    wxModule* module = new wxPGGlobalVarsClassManager;
    wxModule::RegisterModule(module);
    wxModule::InitializeModules();
}

wxPGGlobalVarsClass* wxPGGlobalVars = NULL;


wxPGGlobalVarsClass::wxPGGlobalVarsClass()
    // Prepare some shared variants
    : m_vEmptyString(wxString())
    , m_vZero(0L)
    , m_vMinusOne(-1L)
    , m_vTrue(true)
    , m_vFalse(false)
    // Prepare cached string constants
    , m_strstring(wxS("string"))
    , m_strlong(wxS("long"))
    , m_strbool(wxS("bool"))
    , m_strlist(wxS("list"))
    , m_strDefaultValue(wxS("DefaultValue"))
    , m_strMin(wxS("Min"))
    , m_strMax(wxS("Max"))
    , m_strUnits(wxS("Units"))
    , m_strHint(wxS("Hint"))
#if wxPG_COMPATIBILITY_1_4
    , m_strInlineHelp(wxS("InlineHelp"))
#endif
{
    wxPGProperty::sm_wxPG_LABEL = new wxString(wxPG_LABEL_STRING);

    /* TRANSLATORS: Name of Boolean false value */
    m_boolChoices.Add(_("False"));
    /* TRANSLATORS: Name of Boolean true value */
    m_boolChoices.Add(_("True"));

    m_fontFamilyChoices = NULL;

    m_defaultRenderer = new wxPGDefaultRenderer();

    m_autoGetTranslation = false;

    m_offline = 0;

    m_extraStyle = 0;

    m_warnings = 0;
}


wxPGGlobalVarsClass::~wxPGGlobalVarsClass()
{
    delete m_defaultRenderer;

    // This will always have one ref
    delete m_fontFamilyChoices;

#if wxUSE_VALIDATORS
    for ( size_t i = 0; i < m_arrValidators.size(); i++ )
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

    // Make sure the global pointers have been reset
    wxASSERT(wxPG_EDITOR(TextCtrl) == NULL);
    wxASSERT(wxPG_EDITOR(ChoiceAndButton) == NULL);

    delete wxPGProperty::sm_wxPG_LABEL;
}

void wxPropertyGridInitGlobalsIfNeeded()
{
}

// -----------------------------------------------------------------------
// wxPropertyGrid
// -----------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxPropertyGrid, wxControl);

wxBEGIN_EVENT_TABLE(wxPropertyGrid, wxControl)
  EVT_IDLE(wxPropertyGrid::OnIdle)
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
  EVT_DPI_CHANGED(wxPropertyGrid::OnDPIChanged)
  EVT_MOTION(wxPropertyGrid::OnMouseMove)
  EVT_LEFT_DOWN(wxPropertyGrid::OnMouseClick)
  EVT_LEFT_UP(wxPropertyGrid::OnMouseUp)
  EVT_RIGHT_UP(wxPropertyGrid::OnMouseRightClick)
  EVT_LEFT_DCLICK(wxPropertyGrid::OnMouseDoubleClick)
  EVT_KEY_DOWN(wxPropertyGrid::OnKey)
wxEND_EVENT_TABLE()

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid()
    : wxScrolled<wxControl>()
{
    Init1();
}

// -----------------------------------------------------------------------

wxPropertyGrid::wxPropertyGrid( wxWindow *parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxString& name )
    : wxScrolled<wxControl>()
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
                             const wxString& name )
{

    if (!(style&wxBORDER_MASK))
    {
        style |= wxBORDER_THEME;
    }

    style |= wxVSCROLL;

    // Filter out wxTAB_TRAVERSAL - we will handle TABs manually
    style &= ~(wxTAB_TRAVERSAL);
    style |= wxWANTS_CHARS;

    wxControl::Create(parent, id, pos, size,
                      (style & wxWINDOW_STYLE_MASK) | wxScrolledWindowStyle,
                      wxDefaultValidator,
                      name);

    m_windowStyle |= (style & wxPG_WINDOW_STYLE_MASK);

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

    m_validatingEditor = 0;
    m_iFlags = 0;
    m_pState = NULL;
    m_wndEditor = m_wndEditor2 = NULL;
    m_selColumn = 1;
    m_colHover = 1;
    m_propHover = NULL;
    m_labelEditor = NULL;
    m_labelEditorProperty = NULL;
    m_eventObject = this;
    m_curFocused = NULL;
    m_processedEvent = NULL;
    m_tlp = NULL;
    m_sortFunction = NULL;
    m_inDoPropertyChanged = false;
    m_inCommitChangesFromEditor = false;
    m_inDoSelectProperty = false;
    m_inOnValidationFailure = false;
    m_permanentValidationFailureBehavior = wxPG_VFB_DEFAULT;
    m_dragStatus = 0;
    m_editorFocused = false;

    // Set up default unspecified value 'colour'
    m_unspecifiedAppearance.SetFgCol(*wxLIGHT_GREY);

    // Set default keys
    AddActionTrigger( wxPG_ACTION_NEXT_PROPERTY, WXK_RIGHT );
    AddActionTrigger( wxPG_ACTION_NEXT_PROPERTY, WXK_DOWN );
    AddActionTrigger( wxPG_ACTION_PREV_PROPERTY, WXK_LEFT );
    AddActionTrigger( wxPG_ACTION_PREV_PROPERTY, WXK_UP );
    AddActionTrigger( wxPG_ACTION_EXPAND_PROPERTY, WXK_RIGHT);
    AddActionTrigger( wxPG_ACTION_COLLAPSE_PROPERTY, WXK_LEFT);
    AddActionTrigger( wxPG_ACTION_CANCEL_EDIT, WXK_ESCAPE );
    AddActionTrigger( wxPG_ACTION_PRESS_BUTTON, WXK_DOWN, wxMOD_ALT );
    AddActionTrigger( wxPG_ACTION_PRESS_BUTTON, WXK_F4 );

    m_coloursCustomized = 0;

    m_doubleBuffer = NULL;

#ifndef wxPG_ICON_WIDTH
    m_expandbmp = NULL;
    m_collbmp = NULL;
    m_iconWidth = 11;
    m_iconHeight = 11;
#else
    m_iconWidth = wxPG_ICON_WIDTH;
#endif

    m_gutterWidth = wxPG_GUTTER_MIN;
    m_subgroup_extramargin = 10;

    m_lineHeight = 0;

    m_width = m_height = 0;

    /* TRANSLATORS: Text  displayed for unspecified value */
    m_commonValues.push_back(new wxPGCommonValue(_("Unspecified"), wxPGGlobalVars->m_defaultRenderer) );
    m_cvUnspecified = 0;

    m_chgInfo_changedProperty = NULL;
#if WXWIN_COMPATIBILITY_3_0
    // Object array for this wxPG shouldn't exist in the hash map.
    wxASSERT( gs_deletedEditorObjects.find(this) == gs_deletedEditorObjects.end() );
    gs_deletedEditorObjects[this] = new wxArrayPGObject;
#endif
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
        m_pState->m_dontCenterSplitter = true;

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
    m_vspacing = FromDIP(wxPG_DEFAULT_VSPACING);

    CalculateFontAndBitmapStuff( m_vspacing );

    // Allocate cell data
    m_propertyDefaultCell.SetEmptyData();
    m_categoryDefaultCell.SetEmptyData();

    RegainColours();

    // This helps with flicker
    SetBackgroundStyle( wxBG_STYLE_PAINT );

    // Rely on native double-buffering by default.
#if wxALWAYS_NATIVE_DOUBLE_BUFFER
    SetExtraStyle(GetExtraStyle() | wxPG_EX_NATIVE_DOUBLE_BUFFERING);
#endif // wxALWAYS_NATIVE_DOUBLE_BUFFER

    // Hook the top-level parent
    m_tlpClosed = NULL;
    m_tlpClosedTime = 0;

    // set virtual size to this window size
    wxSize clientSize = GetClientSize();
    SetVirtualSize(clientSize);

    m_timeCreated = ::wxGetLocalTimeMillis();

    m_iFlags |= wxPG_FL_INITIALIZED;

    wxSize wndsize = GetSize();
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

#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif

    //
    // Remove grid and property pointers from live wxPropertyGridEvents.
    for ( i=0; i<m_liveEvents.size(); i++ )
    {
        wxPropertyGridEvent* evt = m_liveEvents[i];
        evt->SetPropertyGrid(NULL);
        evt->SetProperty(NULL);
    }
    m_liveEvents.clear();

    if ( m_processedEvent )
    {
        // All right... we are being deleted while wxPropertyGrid event
        // is being sent. Make sure that event propagates as little
        // as possible (although usually this is not enough to prevent
        // a crash).
        m_processedEvent->Skip(false);
        m_processedEvent->StopPropagation();

        // Let's use wxMessageBox to make the message appear more
        // reliably (and *before* the crash can happen).
        ::wxMessageBox(wxS("wxPropertyGrid was being destroyed in an event ")
                       wxS("generated by it. This usually leads to a crash ")
                       wxS("so it is recommended to destroy the control ")
                       wxS("at idle time instead."));
    }

    DoSelectProperty(NULL, wxPG_SEL_NOVALIDATE|wxPG_SEL_DONT_SEND_EVENT);

    // This should do prevent things from going too badly wrong
    m_iFlags &= ~(wxPG_FL_INITIALIZED);

    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
        ReleaseMouse();

    // Call with NULL to disconnect event handling
    if ( HasExtraStyle(wxPG_EX_ENABLE_TLP_TRACKING) )
    {
        OnTLPChanging(NULL);

        wxASSERT_MSG( !IsEditorsValueModified(),
                      wxS("Most recent change in property editor was ")
                      wxS("lost!!! (if you don't want this to happen, ")
                      wxS("close your frames and dialogs using ")
                      wxS("Close(false).)") );
    }

    if ( m_processedEvent )
    {
        // We are inside event handler and we cannot delete
        // editor objects immediately. They have to be deleted
        // later on in the global idle handler.
#if WXWIN_COMPATIBILITY_3_0
        // Emulate member variable.
        wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
        while ( !m_deletedEditorObjects.empty() )
        {
            wxObject* obj = m_deletedEditorObjects.back();
            m_deletedEditorObjects.pop_back();

            wxPendingDelete.Append(obj);
        }
    }
    else
    {
        // Delete pending editor controls
        DeletePendingObjects();
    }

    if ( m_doubleBuffer )
        delete m_doubleBuffer;

    if ( m_iFlags & wxPG_FL_CREATEDSTATE )
        delete m_pState;

    delete m_cursorSizeWE;

#ifndef wxPG_ICON_WIDTH
    delete m_expandbmp;
    delete m_collbmp;
#endif

    // Delete common value records
    for ( i=0; i<m_commonValues.size(); i++ )
    {
        // Use temporary variable to work around possible strange VC6 (asserts because m_size is zero)
        wxPGCommonValue* value = m_commonValues[i];
        delete value;
    }
#if WXWIN_COMPATIBILITY_3_0
    wxASSERT( gs_deletedEditorObjects[this]->empty() );

    delete gs_deletedEditorObjects[this];
    gs_deletedEditorObjects.erase(this);
#endif
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Destroy()
{
    if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
        ReleaseMouse();

    return wxControl::Destroy();
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
            // Auto sort enabled
            //
            if ( !IsFrozen() )
                PrepareAfterItemsAdded();
            else
                m_pState->m_itemsAdded = true;
        }
#if wxUSE_TOOLTIPS
        if ( !(old_style & wxPG_TOOLTIPS) && (style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips enabled
            //
            //wxToolTip::SetDelay(wxPG_TOOLTIP_DELAY);
        }
        else if ( (old_style & wxPG_TOOLTIPS) && !(style & wxPG_TOOLTIPS) )
        {
            //
            // Tooltips disabled
            //
            UnsetToolTip();
        }
#endif // wxUSE_TOOLTIPS
    }

    wxControl::SetWindowStyleFlag ( style );

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

void wxPropertyGrid::DoThaw()
{
    if ( !IsFrozen() )
    {
        wxControl::DoThaw();
        RecalculateVirtualSize();
        Refresh();

        // Force property re-selection
        // NB: We must copy the selection.
        wxArrayPGProperty selection = m_pState->m_selection;
        DoSetSelection(selection, wxPG_SEL_FORCE | wxPG_SEL_NONVISIBLE);
    }
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoAddToSelection( wxPGProperty* prop, int selFlags )
{
    wxCHECK( prop, false );

    if ( !HasExtraStyle(wxPG_EX_MULTIPLE_SELECTION) )
        return DoSelectProperty(prop, selFlags);

    wxArrayPGProperty& selection = m_pState->m_selection;

    if ( selection.empty() )
    {
        return DoSelectProperty(prop, selFlags);
    }
    else
    {
        // For categories, only one can be selected at a time
        if ( prop->IsCategory() || selection[0]->IsCategory() )
            return true;

        selection.push_back(prop);

        if ( !(selFlags & wxPG_SEL_DONT_SEND_EVENT) )
        {
            SendEvent( wxEVT_PG_SELECTED, prop, NULL );
        }

        DrawItem(prop);
    }

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoRemoveFromSelection( wxPGProperty* prop, int selFlags )
{
    wxCHECK( prop, false );
    bool res;

    wxArrayPGProperty& selection = m_pState->m_selection;
    if ( selection.size() <= 1 )
    {
        res = DoSelectProperty(NULL, selFlags);
    }
    else
    {
        m_pState->DoRemoveFromSelection(prop);
        DrawItem(prop);
        res = true;
    }

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoSelectAndEdit( wxPGProperty* prop,
                                      unsigned int colIndex,
                                      unsigned int selFlags )
{
    //
    // NB: Enable following if label editor background colour is
    //     ever changed to any other than m_colSelBack.
    //
    // We use this workaround to prevent visible flicker when editing
    // a cell. At least on wxMSW, there is a difficult to find
    // (and perhaps prevent) redraw somewhere between making property
    // selected and enabling label editing.
    //
    //wxColour prevColSelBack = m_colSelBack;
    //m_colSelBack = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );

    bool res;

    if ( colIndex == 1 )
    {
        res = DoSelectProperty(prop, selFlags);
    }
    else
    {
        // send event
        DoClearSelection(false, wxPG_SEL_NO_REFRESH);

        if ( !wxPGItemExistsInVector<int>(m_pState->m_editableColumns, colIndex) )
        {
            res = DoAddToSelection(prop, selFlags);
        }
        else
        {
            res = DoAddToSelection(prop, selFlags|wxPG_SEL_NO_REFRESH);

            DoBeginLabelEdit(colIndex, selFlags);
        }
    }

    //m_colSelBack = prevColSelBack;
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::AddToSelectionFromInputEvent( wxPGProperty* prop,
                                                   unsigned int colIndex,
                                                   wxMouseEvent* mouseEvent,
                                                   int selFlags )
{
    const wxArrayPGProperty& selection = GetSelectedProperties();
    bool alreadySelected = m_pState->DoIsPropertySelected(prop);
    bool res = true;

    // Set to 2 if also add all items in between
    int addToExistingSelection = 0;

    if ( HasExtraStyle(wxPG_EX_MULTIPLE_SELECTION) )
    {
        if ( mouseEvent )
        {
            if ( mouseEvent->GetEventType() == wxEVT_RIGHT_DOWN ||
                 mouseEvent->GetEventType() == wxEVT_RIGHT_UP )
            {
                // Allow right-click for context menu without
                // disturbing the selection.
                if ( selection.size() <= 1 ||
                     !alreadySelected )
                    return DoSelectAndEdit(prop, colIndex, selFlags);
                return true;
            }
            else
            {
                if ( mouseEvent->ControlDown() )
                {
                    addToExistingSelection = 1;
                }
                else if ( mouseEvent->ShiftDown() )
                {
                    if ( !selection.empty() && !prop->IsCategory() )
                        addToExistingSelection = 2;
                    else
                        addToExistingSelection = 1;
                }
            }
        }
    }

    if ( addToExistingSelection == 1 )
    {
        // Add/remove one
        if ( !alreadySelected )
        {
            res = DoAddToSelection(prop, selFlags);
        }
        else if ( selection.size() > 1 )
        {
            res = DoRemoveFromSelection(prop, selFlags);
        }
    }
    else if ( addToExistingSelection == 2 )
    {
        // Add this, and all in between

        // Find top selected property
        wxPGProperty* topSelProp = selection[0];
        int topSelPropY = topSelProp->GetY();
        for ( unsigned int i=1; i<selection.size(); i++ )
        {
            wxPGProperty* p = selection[i];
            int y = p->GetY();
            if ( y < topSelPropY )
            {
                topSelProp = p;
                topSelPropY = y;
            }
        }

        wxPGProperty* startFrom;
        wxPGProperty* stopAt;

        if ( prop->GetY() <= topSelPropY )
        {
            // Property is above selection (or same)
            startFrom = prop;
            stopAt = topSelProp;
        }
        else
        {
            // Property is below selection
            startFrom = topSelProp;
            stopAt = prop;
        }

        // Iterate through properties in-between, and select them
        wxPropertyGridIterator it;

        for ( it = GetIterator(wxPG_ITERATE_VISIBLE, startFrom);
              !it.AtEnd();
              ++it )
        {
            wxPGProperty* p = *it;

            if ( !p->IsCategory() &&
                 !m_pState->DoIsPropertySelected(p) )
            {
                DoAddToSelection(p, selFlags);
            }

            if ( p == stopAt )
                break;
        }
    }
    else
    {
        res = DoSelectAndEdit(prop, colIndex, selFlags);
    }

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoSetSelection( const wxArrayPGProperty& newSelection,
                                     int selFlags )
{
    if ( !newSelection.empty() )
    {
        if ( !DoSelectProperty(newSelection[0], selFlags) )
            return;
    }
    else
    {
        DoClearSelection(false, selFlags);
    }

    for ( unsigned int i = 1; i < newSelection.size(); i++ )
    {
        DoAddToSelection(newSelection[i], selFlags);
    }

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::MakeColumnEditable( unsigned int column,
                                         bool editable )
{
    // The second column is always editable. To make it read-only is a property
    // by property decision by setting its wxPG_PROP_READONLY flag.
    wxASSERT_MSG
    (
         column != 1,
         wxS("Set wxPG_PROP_READONLY property flag instead")
    );

    wxVector<int>& cols = m_pState->m_editableColumns;

    if ( editable )
    {
        cols.push_back(column);
    }
    else
    {
        for ( int i = cols.size() - 1; i > 0; i-- )
        {
            if ( cols[i] == (int)column )
                cols.erase( cols.begin() + i );
        }
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoBeginLabelEdit( unsigned int colIndex,
                                       int selFlags )
{
    wxPGProperty* selected = GetSelection();
    wxCHECK_RET(selected, wxS("No property selected"));
    wxCHECK_RET(colIndex != 1, wxS("Do not use this for column 1"));

    if ( !(selFlags & wxPG_SEL_DONT_SEND_EVENT) )
    {
        if ( SendEvent( wxEVT_PG_LABEL_EDIT_BEGIN,
                        selected, NULL, 0,
                        colIndex ) )
            return;
    }

    wxString text;
    const wxPGCell* cell = NULL;
    if ( selected->HasCell(colIndex) )
    {
        cell = &selected->GetCell(colIndex);
        if ( !cell->HasText() && colIndex == 0 )
            text = selected->GetLabel();
    }

    if ( !cell  )
    {
        if ( colIndex == 0 )
            text = selected->GetLabel();
        else
            cell = &selected->GetOrCreateCell(colIndex);
    }

    if ( cell && cell->HasText() )
        text = cell->GetText();

    DoEndLabelEdit(true, wxPG_SEL_NOVALIDATE);  // send event

    m_selColumn = colIndex;

    wxRect r = GetEditorWidgetRect(selected, m_selColumn);

    wxWindow* tc = GenerateEditorTextCtrl(r.GetPosition(),
                                          r.GetSize(),
                                          text,
                                          NULL,
                                          wxTE_PROCESS_ENTER,
                                          0,
                                          colIndex);

    tc->Bind(wxEVT_TEXT_ENTER, &wxPropertyGrid::OnLabelEditorEnterPress, this);
    tc->Bind(wxEVT_KEY_DOWN, &wxPropertyGrid::OnLabelEditorKeyPress, this);

    tc->SetFocus();

    m_labelEditor = wxStaticCast(tc, wxTextCtrl);
    // Get actual position within required rectangle
    m_labelEditorPosRel = m_labelEditor->GetPosition() - r.GetPosition();
    m_labelEditorProperty = selected;
}

// -----------------------------------------------------------------------

void
wxPropertyGrid::OnLabelEditorEnterPress( wxCommandEvent& WXUNUSED(event) )
{
    DoEndLabelEdit(true);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnLabelEditorKeyPress( wxKeyEvent& event )
{
    int keycode = event.GetKeyCode();

    if ( keycode == WXK_ESCAPE )
    {
        DoEndLabelEdit(false);
    }
    else
    {
        HandleKeyEvent(event, true);
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoEndLabelEdit( bool commit, int selFlags )
{
    if ( !m_labelEditor )
        return;

    wxPGProperty* prop = m_labelEditorProperty;
    wxASSERT(prop);

    if ( commit )
    {
        const int labelColIdx = m_selColumn;

        if ( !(selFlags & wxPG_SEL_DONT_SEND_EVENT) )
        {
            // Don't send wxEVT_PG_LABEL_EDIT_ENDING event recursively
            // for the same property and the same label.
            if ( m_processedEvent &&
                 m_processedEvent->GetEventType() == wxEVT_PG_LABEL_EDIT_ENDING &&
                 m_processedEvent->GetProperty() == prop )
            {
                return;
            }

            // wxPG_SEL_NOVALIDATE is passed correctly in selFlags
            if ( SendEvent( wxEVT_PG_LABEL_EDIT_ENDING,
                            prop, NULL, selFlags,
                            m_selColumn ) )
                return;
        }

        wxString text = m_labelEditor->GetValue();

        // Cell handling for label in column 0
        // is done internally in SetLabel() function
        if ( labelColIdx == 0 )
        {
            prop->SetLabel(text);
        }
        else
        {
            wxPGCell* cell = NULL;
            if ( prop->HasCell(labelColIdx) )
            {
                cell = &prop->GetCell(labelColIdx);
            }
            else
            {
                cell = &prop->GetOrCreateCell(labelColIdx);
            }

            if ( cell && cell->HasText() )
            {
                cell->SetText(text);
            }
        }
    }

    m_selColumn = 1;
    int wasFocused = m_iFlags & wxPG_FL_FOCUSED;

    DestroyEditorWnd(m_labelEditor);

    m_labelEditor = NULL;
    m_labelEditorProperty = NULL;

    // Fix focus (needed at least on wxGTK)
    if ( wasFocused )
        SetFocusOnCanvas();

    DrawItem(prop);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetExtraStyle( long exStyle )
{
    if ( exStyle & wxPG_EX_ENABLE_TLP_TRACKING )
        OnTLPChanging(::wxGetTopLevelParent(this));
    else
        OnTLPChanging(NULL);

    if ( exStyle & wxPG_EX_NATIVE_DOUBLE_BUFFERING )
    {
        // Only apply wxPG_EX_NATIVE_DOUBLE_BUFFERING if the window
        // truly was double-buffered.
        if ( !IsDoubleBuffered() )
        {
            exStyle &= ~(wxPG_EX_NATIVE_DOUBLE_BUFFERING);
        }
        else
        {
            wxDELETE(m_doubleBuffer);
        }
    }

    wxControl::SetExtraStyle( exStyle );

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
    int lineHeight = wxMax(FromDIP(15), m_lineHeight);

    // don't make the grid too tall (limit height to 10 items) but don't
    // make it too small neither
    int numLines = wxMin
                   (
                    wxMax(m_pState->DoGetRoot()->GetChildCount(), 3),
                    10
                   );

    wxClientDC dc(const_cast<wxPropertyGrid *>(this));
    int width = m_marginWidth;
    for ( unsigned int i = 0; i < m_pState->GetColumnCount(); i++ )
    {
        width += m_pState->GetColumnFitWidth(dc, m_pState->DoGetRoot(), i, true);
    }

    return wxSize(width, lineHeight*numLines + 40);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnTLPChanging( wxWindow* newTLP )
{
    if ( newTLP == m_tlp )
        return;

    wxMilliClock_t currentTime = ::wxGetLocalTimeMillis();

    //
    // Parent changed so let's re-determine and re-hook the
    // correct top-level window.
    if ( m_tlp )
    {
        m_tlp->Unbind(wxEVT_CLOSE_WINDOW, &wxPropertyGrid::OnTLPClose, this);
        m_tlpClosed = m_tlp;
        m_tlpClosedTime = currentTime;
    }

    if ( newTLP )
    {
        // Only accept new tlp if same one was not just dismissed.
        if ( newTLP != m_tlpClosed ||
             m_tlpClosedTime+250 < currentTime )
        {
            newTLP->Bind(wxEVT_CLOSE_WINDOW, &wxPropertyGrid::OnTLPClose, this);
            m_tlpClosed = NULL;
        }
        else
        {
            newTLP = NULL;
        }
    }

    m_tlp = newTLP;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnTLPClose( wxCloseEvent& event )
{
    // ClearSelection forces value validation/commit.
    if ( event.CanVeto() && !DoClearSelection() )
    {
        event.Veto();
        return;
    }

    // Ok, it can close, set tlp pointer to NULL. Some other event
    // handler can of course veto the close, but our OnIdle() should
    // then be able to regain the tlp pointer.
    OnTLPChanging(NULL);

    event.Skip();
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::Reparent( wxWindowBase *newParent )
{
    OnTLPChanging((wxWindow*)newParent);

    bool res = wxControl::Reparent(newParent);

    return res;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ScrollWindow(int dx, int dy, const wxRect* rect)
{
    wxControl::ScrollWindow(dx, dy, rect);
    if ( dx != 0 )
    {
        // Notify wxPropertyGridManager about the grid being scrolled horizontally
        // to scroll the column header, if present.
        SendEvent(wxEVT_PG_HSCROLL, dx);
    }
}
// -----------------------------------------------------------------------

void wxPropertyGrid::SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                                   int noUnitsX, int noUnitsY,
                                   int xPos, int yPos, bool noRefresh)
{
    int oldX;
    CalcUnscrolledPosition(0, 0, &oldX, NULL);
    wxScrolled<wxControl>::SetScrollbars(pixelsPerUnitX, pixelsPerUnitY,
                                  noUnitsX, noUnitsY, xPos, yPos, noRefresh);
    int newX;
    CalcUnscrolledPosition(0, 0, &newX, NULL);
    if ( newX != oldX )
    {
        // Notify wxPropertyGridManager about the grid being scrolled horizontally
        // to scroll the column header, if present.
        SendEvent(wxEVT_PG_HSCROLL, oldX - newX);
    }
}

// -----------------------------------------------------------------------
// wxPropertyGrid Font and Colour Methods
// -----------------------------------------------------------------------

void wxPropertyGrid::CalculateFontAndBitmapStuff( int vspacing )
{
    int x = 0, y = 0;

    m_captionFont = wxControl::GetFont();

    GetTextExtent(wxS("jG"), &x, &y, 0, 0, &m_captionFont);
    m_subgroup_extramargin = x + (x/2);
    m_fontHeight = y;

#if wxPG_USE_RENDERER_NATIVE
    m_iconWidth = FromDIP(wxPG_ICON_WIDTH);
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

    m_captionFont.SetWeight(wxFONTWEIGHT_BOLD);
    GetTextExtent(wxS("jG"), &x, &y, 0, 0, &m_captionFont);

    m_lineHeight = m_fontHeight+(2*m_spacingy)+1;

    // button spacing
    m_buttonSpacingY = (m_lineHeight - m_iconHeight) / 2;
    if ( m_buttonSpacingY < 0 ) m_buttonSpacingY = 0;

    if ( m_pState )
        m_pState->CalculateFontAndBitmapStuff(vspacing);

    SetScrollRate(wxPG_PIXELS_PER_UNIT, wxPG_PIXELS_PER_UNIT);

    if ( m_iFlags & wxPG_FL_INITIALIZED )
        RecalculateVirtualSize();

    InvalidateBestSize();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnSysColourChanged( wxSysColourChangedEvent &WXUNUSED(event) )
{
    if ((m_iFlags & wxPG_FL_INITIALIZED)!=0) {
        RegainColours();
        Refresh();
    }
}

void wxPropertyGrid::OnDPIChanged(wxDPIChangedEvent &WXUNUSED(event))
{
    m_vspacing = FromDIP(wxPG_DEFAULT_VSPACING);
    CalculateFontAndBitmapStuff(m_vspacing);
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
                 wxS("wxPGAdjustColour should not be recursively called more than once") );

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
        m_categoryDefaultCell.GetData()->SetBgCol(m_colCapBack);
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
        if (wxPGGetColAvg(m_colCapBack) < 100)
            capForeCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );

        m_colCapFore = capForeCol;
        m_categoryDefaultCell.GetData()->SetFgCol(capForeCol);
    }

    if ( !(m_coloursCustomized & 0x0008) )
    {
        wxColour bgCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW );
        m_colPropBack = bgCol;
        m_propertyDefaultCell.GetData()->SetBgCol(bgCol);
        if ( !m_unspecifiedAppearance.GetBgCol().IsOk() )
            m_unspecifiedAppearance.SetBgCol(bgCol);
    }

    if ( !(m_coloursCustomized & 0x0010) )
    {
        wxColour fgCol = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
        m_colPropFore = fgCol;
        m_propertyDefaultCell.GetData()->SetFgCol(fgCol);
        if ( !m_unspecifiedAppearance.GetFgCol().IsOk() )
            m_unspecifiedAppearance.SetFgCol(fgCol);
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
    DoClearSelection();

    bool res = wxControl::SetFont( font );
    if ( res && GetParent()) // may not have been Create()ed yet if SetFont called from SetWindowVariant
    {
        CalculateFontAndBitmapStuff( m_vspacing );
        Refresh();
    }

    return res;
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

    m_propertyDefaultCell.GetData()->SetBgCol(col);
    m_unspecifiedAppearance.SetBgCol(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCellTextColour( const wxColour& col )
{
    m_colPropFore = col;
    m_coloursCustomized |= 0x10;

    m_propertyDefaultCell.GetData()->SetFgCol(col);
    m_unspecifiedAppearance.SetFgCol(col);

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

    m_categoryDefaultCell.GetData()->SetBgCol(col);

    Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetCaptionTextColour( const wxColour& col )
{
    m_colCapFore = col;
    m_coloursCustomized |= 0x04;

    m_categoryDefaultCell.GetData()->SetFgCol(col);

    Refresh();
}

// -----------------------------------------------------------------------
// wxPropertyGrid property adding and removal
// -----------------------------------------------------------------------

void wxPropertyGrid::PrepareAfterItemsAdded()
{
    if ( !m_pState || !m_pState->m_itemsAdded ) return;

    m_pState->m_itemsAdded = false;

    if ( m_windowStyle & wxPG_AUTO_SORT )
        Sort(wxPG_SORT_TOP_LEVEL_ONLY);

    RecalculateVirtualSize();

    // Fix editor position
    CorrectEditorWidgetPosY();
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

        if ( grandparent && grandparent != m_pState->DoGetRoot() )
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
    wxWindow* editor = GetEditorControl();
    editor->SetFont( m_captionFont );
}

// -----------------------------------------------------------------------

wxPoint wxPropertyGrid::GetGoodEditorDialogPosition( wxPGProperty* p,
                                                     const wxSize& sz )
{
    if ( IsSmallScreen() )
    {
        // On small-screen devices, always show dialogs with default position and size.
        return wxDefaultPosition;
    }

    int splitterX = GetSplitterPosition();
    int x = splitterX;
    int y = p->GetY();

    wxCHECK_MSG( y >= 0, wxDefaultPosition, wxS("invalid y?") );

    ImprovedClientToScreen( &x, &y );

    int sw = wxSystemSettings::GetMetric( ::wxSYS_SCREEN_X, this );
    int sh = wxSystemSettings::GetMetric( ::wxSYS_SCREEN_Y, this );

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
}

// -----------------------------------------------------------------------

wxString& wxPropertyGrid::ExpandEscapeSequences( wxString& dst_str, const wxString& src_str )
{
    dst_str.clear();

    if ( src_str.empty() )
    {
        return dst_str;
    }

    bool prev_is_slash = false;

    wxString::const_iterator i;
    for ( i = src_str.begin(); i != src_str.end(); ++i )
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
                    dst_str << wxS('\n');
                else if ( a == wxS('r') )
                    dst_str << wxS('\r');
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

wxString& wxPropertyGrid::CreateEscapeSequences( wxString& dst_str, const wxString& src_str )
{
    dst_str.clear();

    if ( src_str.empty() )
    {
        return dst_str;
    }

    wxString::const_iterator i;
    for ( i = src_str.begin(); i != src_str.end(); ++i )
    {
        wxUniChar a = *i;

        if ( a == wxS('\r') )
            // Carriage Return.
            dst_str << wxS("\\r");
        else if ( a == wxS('\n') )
            // Line Feed.
            dst_str << wxS("\\n");
        else if ( a == wxS('\t') )
            // Tab.
            dst_str << wxS("\\t");
        else if ( a == wxS('\\') )
            // Escape character (backslash).
            dst_str << wxS("\\\\");
        else
            dst_str << a;
    }
    return dst_str;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::IsSmallScreen()
{
    return wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA;
}

// -----------------------------------------------------------------------

// static
wxBitmap wxPropertyGrid::RescaleBitmap(const wxBitmap& srcBmp,
                                       double scaleX, double scaleY)
{
    int w = wxRound(srcBmp.GetWidth()*scaleX);
    int h = wxRound(srcBmp.GetHeight()*scaleY);

#if wxUSE_IMAGE
    // Here we use high-quality wxImage scaling functions available
    wxImage img = srcBmp.ConvertToImage();
    img.Rescale(w, h, wxIMAGE_QUALITY_HIGH);
    wxBitmap dstBmp(img);
#else // !wxUSE_IMAGE
    wxBitmap dstBmp(w, h, srcBmp.GetDepth());
#if defined(__WXMSW__) || defined(__WXOSX__)
    // wxBitmap::UseAlpha() is used only on wxMSW and wxOSX.
    dstBmp.UseAlpha(srcBmp.HasAlpha());
#endif // __WXMSW__ || __WXOSX__
    {
        wxMemoryDC dc(dstBmp);
        dc.SetUserScale(scaleX, scaleY);
        dc.DrawBitmap(srcBmp, 0, 0);
    }
#endif // wxUSE_IMAGE/!wxUSE_IMAGE

    return dstBmp;
}

// -----------------------------------------------------------------------

wxPGProperty* wxPropertyGrid::DoGetItemAtY( int y ) const
{
    // Outside?
    if ( y < 0 )
        return NULL;

    unsigned int a = 0;
    return m_pState->DoGetRoot()->GetItemAtY(y, m_lineHeight, &a);
}

// -----------------------------------------------------------------------
// wxPropertyGrid graphics related methods
// -----------------------------------------------------------------------

void wxPropertyGrid::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxDC* dcPtr = NULL;
    if ( !HasExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING) )
    {
        if ( m_doubleBuffer )
        {
            dcPtr = new wxBufferedPaintDC(this, *m_doubleBuffer);
        }
    }
    if ( !dcPtr )
    {
        dcPtr = new wxPaintDC(this);
    }
    wxASSERT( dcPtr );
    PrepareDC(*dcPtr);
    // Unused area will be cleared when drawing the items
/*
    dcPtr->SetBackground(m_colEmptySpace);
    dcPtr->Clear();
*/

    // Don't paint after destruction has begun
    if ( !HasInternalFlag(wxPG_FL_INITIALIZED) )
    {
        delete dcPtr;
        return;
    }

    // Find out where the window is scrolled to
    int vx, vy;
    GetViewStart(&vx, &vy);
    vx *= wxPG_PIXELS_PER_UNIT;
    vy *= wxPG_PIXELS_PER_UNIT;

    // Update everything inside the box
    wxRect r = GetUpdateRegion().GetBox();
    r.Offset(vx, vy);

    // Repaint this rectangle
    DrawItems(*dcPtr, r.y, r.y + r.height-1, &r);

    // This blits the buffer (if used) to the window DC.
    delete dcPtr;

    // We assume that the size set when grid is shown
    // is what is desired.
    SetInternalFlag(wxPG_FL_GOOD_SIZE_SET);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawExpanderButton( wxDC& dc, const wxRect& rect,
                                         wxPGProperty* property ) const
{
    // Prepare rectangle to be used
    wxRect r(rect);
    r.Offset(m_gutterWidth, m_buttonSpacingY);
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
                const_cast<wxPropertyGrid*>(this),
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
                const_cast<wxPropertyGrid*>(this),
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
// topy and bottomy are already unscrolled (i.e. physical)
//
void wxPropertyGrid::DrawItems( wxDC& dc,
                                unsigned int topItemY,
                                unsigned int bottomItemY,
                                const wxRect* itemsRect )
{
    if ( IsFrozen() ||
         m_height < 1 ||
         bottomItemY < topItemY ||
         !m_pState )
        return;

    m_pState->EnsureVirtualHeight();

    wxRect tempItemsRect;
    if ( !itemsRect )
    {
        tempItemsRect = wxRect(0, topItemY,
                               m_pState->GetVirtualWidth(),
                               bottomItemY - topItemY + 1);
        itemsRect = &tempItemsRect;
    }

    // items added check
    if ( m_pState->m_itemsAdded ) PrepareAfterItemsAdded();

    if ( m_pState->DoGetRoot()->GetChildCount() > 0 )
    {
        int paintFinishY = DoDrawItems(dc, itemsRect) + 1;
        int drawBottomY = itemsRect->GetBottom();

        // Clear area beyond last painted property
        if ( paintFinishY < drawBottomY )
        {
            dc.SetPen(m_colEmptySpace);
            dc.SetBrush(m_colEmptySpace);
            dc.DrawRectangle(0, paintFinishY,
                                    m_width,
                                    drawBottomY-paintFinishY+1);
        }
    }
    else
    {
        // Just clear the area
        dc.SetPen(m_colEmptySpace);
        dc.SetBrush(m_colEmptySpace);
        dc.DrawRectangle(*itemsRect);
    }
}

// -----------------------------------------------------------------------

#if WXWIN_COMPATIBILITY_3_0
int wxPropertyGrid::DoDrawItemsBase( wxDC& dc,
                                 const wxRect* itemsRect,
                                 bool WXUNUSED(isBuffered) ) const
#else
int wxPropertyGrid::DoDrawItems( wxDC& dc,
                                 const wxRect* itemsRect ) const
#endif
{
    const wxPGProperty* firstItem = DoGetItemAtY(itemsRect->y);
    if ( !firstItem ) // Signal a need to clear entire paint area if grid is empty
        return -1;

    if ( IsFrozen() || m_height < 1 )
        return itemsRect->GetBottom();

    wxCHECK_MSG( !m_pState->m_itemsAdded, itemsRect->GetBottom(),
                 wxS("no items added") );
    wxASSERT( m_pState->DoGetRoot()->GetChildCount() );

    int lh = m_lineHeight;
    int firstItemTopY = itemsRect->y;
    int lastItemBottomY = itemsRect->GetBottom();

    // Align y coordinates to item boundaries
    firstItemTopY -= firstItemTopY % lh;
    lastItemBottomY += lh - (lastItemBottomY % lh);
    lastItemBottomY -= 1;

    // Entire range outside scrolled, visible area?
    if ( firstItemTopY >= (int)m_pState->GetVirtualHeight() ||
         lastItemBottomY <= 0 )
        return itemsRect->GetBottom();

    wxCHECK_MSG( firstItemTopY < lastItemBottomY,
                 itemsRect->GetBottom(),
                 wxS("invalid y values") );

    /*
    wxLogDebug(wxS(" -> DoDrawItems(\"%s\" -> \"%s\"")
               wxS(" %i -> %i height=%i (ch=%i), itemsRect = 0x%lX %i,%i %ix%i)"),
        firstItem->GetLabel(),
        lastItem->GetLabel(),
        firstItemTopY, lastItemBottomY,
        (int)(lastItemBottomY - firstItemTopY),
        (int)m_height,
        (unsigned long)&itemsRect,
        itemsRect->x, itemsRect->y, itemsRect->width, itemsRect->height );
    */

    long windowStyle = m_windowStyle;

    int x = m_marginWidth;

    wxFont normalFont = GetFont();

    bool reallyFocused = (m_iFlags & wxPG_FL_FOCUSED) != 0;

    bool isPgEnabled = IsEnabled();

    //
    // Prepare some pens and brushes that are often changed to.
    //

    wxBrush marginBrush(m_colMargin);
    wxPen marginPen(m_colMargin);
    wxBrush capbgbrush(m_colCapBack,wxBRUSHSTYLE_SOLID);
    wxPen linepen(m_colLine,1,wxPENSTYLE_SOLID);

    wxColour selBackCol = isPgEnabled ? m_colSelBack : m_colMargin;

    // pen that has same colour as text
    wxPen outlinepen(m_colPropFore,1,wxPENSTYLE_SOLID);

    //
    // Clear margin with background colour
    //
    dc.SetBrush( marginBrush );
    if ( !(windowStyle & wxPG_HIDE_MARGIN) )
    {
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(-1,firstItemTopY-1,x+2,lastItemBottomY-firstItemTopY+2);
    }

    const wxPGProperty* firstSelected = GetSelection();
    const wxPropertyGridPageState* state = m_pState;
    const wxVector<int>& colWidths = state->m_colWidths;
    const unsigned int colCount = state->GetColumnCount();

    dc.SetFont(normalFont);

    wxPropertyGridConstIterator it( state, wxPG_ITERATE_VISIBLE, firstItem );
    int endScanBottomY = lastItemBottomY + lh;
    int y = firstItemTopY;

    //
    // Pre-generate list of visible properties.
    wxVector<wxPGProperty*> visPropArray;
    visPropArray.reserve((m_height/m_lineHeight)+6);

    for ( ; !it.AtEnd(); it.Next() )
    {
        const wxPGProperty* p = *it;

        if ( !p->HasFlag(wxPG_PROP_HIDDEN) )
        {
            visPropArray.push_back(const_cast<wxPGProperty*>(p));

            if ( y > endScanBottomY )
                break;

            y += lh;
        }
    }

    visPropArray.push_back(NULL);

    wxPGProperty* nextP = visPropArray[0];

    // Calculate splitters positions
    wxVector<int> splitterPos;
    splitterPos.reserve(colCount);
    int sx = x;
    for (wxVector<int>::const_iterator cit = colWidths.begin(); cit != colWidths.end(); ++cit)
    {
        sx += *cit;
        splitterPos.push_back(sx);
    }

    int viewLeftEdge = itemsRect->x;
    int viewRightEdge = viewLeftEdge + itemsRect->width - 1;
    // Determine columns range to be drawn
    unsigned int firstCol = 0;
    while ( firstCol < colCount-1 && splitterPos[firstCol] < viewLeftEdge )
        firstCol++;
    unsigned int lastCol = firstCol;
    while ( lastCol < colCount-1 && splitterPos[lastCol] < viewRightEdge )
        lastCol++;
    // Calculate position of the right edge of the last cell
    int cellX = splitterPos[lastCol]+ 1;

    y = firstItemTopY;
    for ( unsigned int arrInd=1;
          nextP && y <= lastItemBottomY;
          arrInd++ )
    {
        wxPGProperty* p = nextP;
        nextP = visPropArray[arrInd];

        int textMarginHere = x;
        int renderFlags = 0;

        int greyDepth = m_marginWidth;
        if ( !(windowStyle & wxPG_HIDE_CATEGORIES) )
            greyDepth += (((int)p->m_depthBgCol)-1) * m_subgroup_extramargin;

        // Use basic depth if in non-categoric mode and parent is base array.
        if ( !(windowStyle & wxPG_HIDE_CATEGORIES) || p->GetParent() != m_pState->DoGetRoot() )
        {
            textMarginHere += ((p->GetDepth()-1)*m_subgroup_extramargin);
        }

        // Paint margin area
        dc.SetBrush(marginBrush);
        dc.SetPen(marginPen);
        dc.DrawRectangle( 0, y, greyDepth, lh );

        dc.SetPen( linepen );

        int y2 = y + lh;

#ifdef __WXMSW__
        // Margin Edge
        // Modified by JACS to not draw a margin if wxPG_HIDE_MARGIN is specified, since it
        // looks better, at least under Windows when we have a themed border (the themed-window-specific
        // whitespace between the real border and the propgrid margin exacerbates the double-border look).

        // Is this or its parent themed?
        bool suppressMarginEdge = (GetWindowStyle() & wxPG_HIDE_MARGIN) &&
            (((GetWindowStyle() & wxBORDER_MASK) == wxBORDER_THEME) ||
            (((GetWindowStyle() & wxBORDER_MASK) == wxBORDER_NONE) && ((GetParent()->GetWindowStyle() & wxBORDER_MASK) == wxBORDER_THEME)));
        if (suppressMarginEdge)
        {
            // Blank out the margin edge
            dc.SetPen(wxPen(GetBackgroundColour()));
            dc.DrawLine( greyDepth, y, greyDepth, y2 );
            dc.SetPen( linepen );
        }
        else
#endif // __WXMSW__
        {
            dc.DrawLine(greyDepth, y, greyDepth, y2);
        }

        // Splitters
        for (unsigned int i = firstCol; i <= lastCol; i++)
        {
            dc.DrawLine(splitterPos[i], y, splitterPos[i], y2);
        }

        // Horizontal Line, below
        //   (not if both this and next is category caption)
        if ( p->IsCategory() &&
             nextP && nextP->IsCategory() )
            dc.SetPen(m_colCapBack);

        dc.DrawLine(greyDepth, y2 - 1, cellX, y2 - 1);

        //
        // Need to override row colours?
        wxColour rowFgCol;
        wxColour rowBgCol;

        bool isSelected = state->DoIsPropertySelected(p);

        if ( !isSelected )
        {
            // Disabled may get different colour.
            if ( !p->IsEnabled() )
            {
                renderFlags |= wxPGCellRenderer::Disabled |
                               wxPGCellRenderer::DontUseCellFgCol;
                rowFgCol = m_colDisPropFore;
            }
        }
        else
        {
            renderFlags |= wxPGCellRenderer::Selected;

            if ( !p->IsCategory() )
            {
                renderFlags |= wxPGCellRenderer::DontUseCellFgCol |
                               wxPGCellRenderer::DontUseCellBgCol;

                if ( reallyFocused && p == firstSelected )
                {
                    rowFgCol = m_colSelFore;
                    rowBgCol = selBackCol;
                }
                else if ( isPgEnabled )
                {
                    rowFgCol = m_colPropFore;
                    rowBgCol = p == firstSelected ? m_colMargin : selBackCol;
                }
                else
                {
                    rowFgCol = m_colDisPropFore;
                    rowBgCol = selBackCol;
                }
            }
        }

        wxBrush rowBgBrush;

        if ( rowBgCol.IsOk() )
            rowBgBrush = wxBrush(rowBgCol);

        if ( HasInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL) )
            renderFlags = renderFlags & ~wxPGCellRenderer::DontUseCellColours;

        //
        // Fill additional margin area with background colour of first cell
        if ( greyDepth < textMarginHere )
        {
            if ( !(renderFlags & wxPGCellRenderer::DontUseCellBgCol) )
            {
                wxPGCell& cell = p->GetCell(0);
                rowBgCol = cell.GetBgCol();
                rowBgBrush = wxBrush(rowBgCol);
            }
            dc.SetBrush(rowBgBrush);
            dc.SetPen(rowBgCol);
            dc.DrawRectangle(greyDepth+1, y,
                             textMarginHere-greyDepth, lh-1);
        }

        bool fontChanged = false;

        // Expander button rectangle
        wxRect butRect( ((p->GetDepth() - 1) * m_subgroup_extramargin),
                        y,
                        m_marginWidth,
                        lh );

        // Default cell rect fill the entire row
        wxRect cellRect(greyDepth, y, cellX - greyDepth, lh-1);

        bool isCategory = p->IsCategory();

        if ( isCategory )
        {
            dc.SetFont(m_captionFont);
            fontChanged = true;

            if ( renderFlags & wxPGCellRenderer::DontUseCellBgCol )
            {
                dc.SetBrush(rowBgBrush);
                dc.SetPen(rowBgCol);
            }

            if ( renderFlags & wxPGCellRenderer::DontUseCellFgCol )
            {
                dc.SetTextForeground(rowFgCol);
            }
        }
        else
        {
            // Fine tune button rectangle to actually fit the cell
            if ( butRect.x > 0 )
                butRect.x += IN_CELL_EXPANDER_BUTTON_X_ADJUST;

            if ( p->HasFlag(wxPG_PROP_MODIFIED) &&
                 (windowStyle & wxPG_BOLD_MODIFIED) )
            {
                dc.SetFont(m_captionFont);
                fontChanged = true;
            }

            // Magic fine-tuning for non-category rows
            cellRect.x += 1;
        }

        int firstCellWidth = colWidths[0] - (greyDepth - m_marginWidth);
        int firstCellX = cellRect.x;

        // Calculate cellRect.x for the last cell
        cellRect.x = cellX;

        // Draw cells from back to front so that we can easily tell if the
        // cell on the right was empty from text
        bool prevFilled = true;
        unsigned int ci = lastCol + 1;
        do
        {
            ci--;

            int textXAdd = 0;

            if ( ci == 0 )
            {
                textXAdd = textMarginHere - greyDepth;
                cellRect.width = firstCellWidth;
                cellRect.x = firstCellX;
            }
            else
            {
                int colWidth = colWidths[ci];
                cellRect.width = colWidth;
                cellRect.x -= colWidth;
            }

            // Merge with column to the right?
            if ( !prevFilled && isCategory )
            {
                cellRect.width += colWidths[ci+1];
            }

            if ( !isCategory )
                cellRect.width -= 1;

            wxWindow* cellEditor = NULL;
            int cellRenderFlags = renderFlags;

            // Tree Item Button (must be drawn before clipping is set up)
            if ( ci == 0 && !HasFlag(wxPG_HIDE_MARGIN) && p->HasVisibleChildren() )
                DrawExpanderButton( dc, butRect, p );

            // Background
            if ( isSelected && (ci == 1 || ci == m_selColumn) )
            {
                if ( p == firstSelected )
                {
                    if ( ci == 1 && m_wndEditor )
                        cellEditor = m_wndEditor;
                    else if ( ci == m_selColumn && m_labelEditor )
                        cellEditor = m_labelEditor;
                }

                if ( cellEditor )
                {
                    wxColour editorBgCol =
                        cellEditor->GetBackgroundColour();
                    dc.SetBrush(editorBgCol);
                    dc.SetPen(editorBgCol);
                    dc.SetTextForeground(m_colPropFore);
                    dc.DrawRectangle(cellRect);

                    if ( m_dragStatus != 0 ||
                         (m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE) )
                        cellEditor = NULL;
                }
                else
                {
                    dc.SetBrush(m_colPropBack);
                    dc.SetPen(m_colPropBack);
                    dc.SetTextForeground(p->IsEnabled() ? m_colPropFore : m_colDisPropFore);
                }
            }
            else
            {
                if ( renderFlags & wxPGCellRenderer::DontUseCellBgCol )
                {
                    dc.SetBrush(rowBgBrush);
                    dc.SetPen(rowBgCol);
                }

                if ( renderFlags & wxPGCellRenderer::DontUseCellFgCol )
                {
                    dc.SetTextForeground(rowFgCol);
                }
            }

            dc.SetClippingRegion(cellRect);

            cellRect.x += textXAdd;
            cellRect.width -= textXAdd;

            // Foreground
            if ( !cellEditor )
            {
                wxPGCellRenderer* renderer;
                int cmnVal = p->GetCommonValue();
                if ( cmnVal == -1 || ci != 1 )
                {
                    renderer = p->GetCellRenderer(ci);
                    prevFilled = renderer->Render(dc, cellRect, this,
                                                  p, ci, -1,
                                                  cellRenderFlags );
                }
                else
                {
                    renderer = GetCommonValue(cmnVal)->GetRenderer();
                    prevFilled = renderer->Render(dc, cellRect, this,
                                                  p, ci, -1,
                                                  cellRenderFlags );
                }
            }
            else
            {
                prevFilled = true;
            }

            dc.DestroyClippingRegion();
        }
        while ( ci > firstCol );

        if ( fontChanged )
            dc.SetFont(normalFont);

        y += lh;
    }

    // Clear empty space beyond the right edge of the grid
    dc.SetPen(wxPen(m_colEmptySpace));
    dc.SetBrush(wxBrush(m_colEmptySpace));
    dc.DrawRectangle(cellX, firstItemTopY, viewRightEdge - cellX + 1, lastItemBottomY - firstItemTopY);

    return y - 1;
}

// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetPropertyRect( const wxPGProperty* p1, const wxPGProperty* p2 ) const
{
    if ( m_width < 10 || m_height < 10 ||
         !m_pState->DoGetRoot()->GetChildCount() ||
         p1 == NULL )
        return wxRect(0,0,0,0);

    int vy = 0;

    //
    // Return rect which encloses the given property range
    // (in logical grid coordinates)
    //

    int visTop = p1->GetY();
    int visBottom;
    if ( p2 )
        visBottom = p2->GetY() + m_lineHeight;
    else
        visBottom = m_height + visTop;

    // If selected property is inside the range, we'll extend the range to include
    // control's size.
    wxPGProperty* selected = GetSelection();
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

    return wxRect(0,visTop-vy,m_pState->GetVirtualWidth(),visBottom-visTop);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItems( const wxPGProperty* p1, const wxPGProperty* p2 )
{
    if ( IsFrozen() )
        return;

    if ( m_pState->m_itemsAdded )
        PrepareAfterItemsAdded();

    wxRect r = GetPropertyRect(p1, p2);
    if ( !r.IsEmpty() )
    {
        // Convert rectangle from logical grid coordinates to physical ones
        int vx, vy;
        GetViewStart(&vx, &vy);
        vx *= wxPG_PIXELS_PER_UNIT;
        vy *= wxPG_PIXELS_PER_UNIT;
        r.Offset(-vx, -vy);
        RefreshRect(r);
        Update();
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::RefreshProperty( wxPGProperty* p )
{
    if ( m_pState->DoIsPropertySelected(p) || p->IsChildSelected(true) )
    {
        // NB: We must copy the selection.
        wxArrayPGProperty selection = m_pState->m_selection;
        int selFlags = wxPG_SEL_FORCE;
        // We want to keep property's editor focused.
        if ( IsEditorFocused() )
            selFlags |= wxPG_SEL_FOCUS;
        DoSetSelection(selection, selFlags);
    }

    DrawItemAndChildren(p);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DrawItemAndValueRelated( wxPGProperty* p )
{
    if ( IsFrozen() )
        return;

    // Draw item, children, and parent too, if it is not category
    wxPGProperty* parent = p->GetParent();

    while ( parent &&
            !parent->IsCategory() && !parent->IsRoot() )
    {
         DrawItem(parent);
         parent = parent->GetParent();
    }

    DrawItemAndChildren(p);
}

void wxPropertyGrid::DrawItemAndChildren( wxPGProperty* p )
{
    wxCHECK_RET( p, wxS("invalid property id") );

    // Do not draw if in non-visible page
    if ( p->GetParentState() != m_pState )
        return;

    // do not draw a single item if multiple pending
    if ( m_pState->m_itemsAdded || IsFrozen() )
        return;

    // Update child control.
    wxPGProperty* selected = GetSelection();
    if ( selected && selected->GetParent() == p )
        RefreshEditor();

    const wxPGProperty* lastDrawn = p->GetLastVisibleSubItem();

    DrawItems(p, lastDrawn);
}

// -----------------------------------------------------------------------

void wxPropertyGrid::Refresh( bool WXUNUSED(eraseBackground),
                              const wxRect *rect )
{
    PrepareAfterItemsAdded();

    wxWindow::Refresh(false, rect);

#if wxPG_REFRESH_CONTROLS
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
    m_pState->DoClear();

    m_propHover = NULL;

    RecalculateVirtualSize();

    // Need to clear some area at the end
    if ( !IsFrozen() )
        RefreshRect(wxRect(0, 0, m_width, m_height));
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::EnableCategories( bool enable )
{
    DoClearSelection();

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

    if ( !IsFrozen() )
    {
        if ( m_windowStyle & wxPG_AUTO_SORT )
        {
            m_pState->m_itemsAdded = true; // force
            PrepareAfterItemsAdded();
        }
    }
    else
        m_pState->m_itemsAdded = true;

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

    wxArrayPGProperty oldSelection = m_pState->m_selection;

    // Call ClearSelection() instead of DoClearSelection()
    // so that selection clear events are not sent.
    ClearSelection();

    m_pState->m_selection = oldSelection;

    bool orig_mode = m_pState->IsInNonCatMode();
    bool new_state_mode = pNewState->IsInNonCatMode();

    m_pState = pNewState;

    // Validate width
    int pgWidth = GetClientSize().x;
    if ( HasVirtualWidth() )
    {
        if ( pNewState->GetVirtualWidth() < pgWidth )
        {
            pNewState->m_width = pgWidth;
            pNewState->CheckColumnWidths();
        }
    }
    else
    {
        //
        // Just in case, fully re-center splitter
        //if ( HasFlag( wxPG_SPLITTER_AUTO_CENTER ) )
        //    pNewState->m_fSplitterX = -1.0;

        pNewState->OnClientWidthChange(pgWidth,
                                       pgWidth - pNewState->GetVirtualWidth());
    }

    m_propHover = NULL;

    // If necessary, convert state to correct mode.
    if ( orig_mode != new_state_mode )
    {
        // This should refresh as well.
        EnableCategories( orig_mode?false:true );
    }
    else if ( !IsFrozen() )
    {
        // Refresh, if not frozen.
        m_pState->PrepareAfterItemsAdded();

        // Reselect (Use SetSelection() instead of Do-variant so that
        // events won't be sent).
        SetSelection(m_pState->m_selection);

        RecalculateVirtualSize(0);
        Refresh();
    }
    else
        m_pState->m_itemsAdded = true;
}

// -----------------------------------------------------------------------

// Call to SetSplitterPosition will always disable splitter auto-centering
// if parent window is shown.
void wxPropertyGrid::DoSetSplitterPosition( int newxpos,
                                            int splitterIndex,
                                            int flags )
{
    if ( ( newxpos < wxPG_DRAG_MARGIN ) )
        return;

    if ( flags & wxPG_SPLITTER_FROM_EVENT )
        m_pState->m_dontCenterSplitter = true;

    m_pState->DoSetSplitterPosition(newxpos, splitterIndex, flags);

    if ( flags & wxPG_SPLITTER_REFRESH )
    {
        if ( GetSelection() )
        {
            CorrectEditorWidgetSizeX();
        }

        Refresh();
    }

    return;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::ResetColumnSizes( bool enableAutoResizing )
{
    if ( m_pState )
    {
        m_pState->ResetColumnSizes(0);
        if ( GetSelection() )
        {
            CorrectEditorWidgetSizeX();
        }
        Refresh();

        if ( enableAutoResizing && HasFlag(wxPG_SPLITTER_AUTO_CENTER) )
            m_pState->m_dontCenterSplitter = false;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CenterSplitter( bool enableAutoResizing )
{
    SetSplitterPosition( m_width/2 );
    if ( enableAutoResizing && HasFlag(wxPG_SPLITTER_AUTO_CENTER) )
        m_pState->m_dontCenterSplitter = false;
}

// -----------------------------------------------------------------------
// wxPropertyGrid item iteration (GetNextProperty etc.) methods
// -----------------------------------------------------------------------

// Returns nearest paint visible property (such that will be painted unless
// window is scrolled or resized). If given property is paint visible, then
// it itself will be returned
wxPGProperty* wxPropertyGrid::GetNearestPaintVisible( wxPGProperty* p ) const
{
    int vy1;// Top left corner of client
    GetViewStart(NULL,&vy1);
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
    if ( m_iFlags & wxPG_FL_IN_HANDLECUSTOMEDITOREVENT )
    {
        if ( m_inDoPropertyChanged )
            return true;

        return false;
    }

    wxPGProperty* selected = GetSelection();

    if ( m_wndEditor &&
         IsEditorsValueModified() &&
         (m_iFlags & wxPG_FL_INITIALIZED) &&
         selected )
    {
        m_inCommitChangesFromEditor = true;

        wxVariant variant(selected->GetValueRef());
        bool valueIsPending = false;

        // JACS - necessary to avoid new focus being found spuriously within OnIdle
        // due to another window getting focus
        wxWindow* oldFocus = m_curFocused;

        bool validationFailure = false;
        bool forceSuccess = (flags & (wxPG_SEL_NOVALIDATE|wxPG_SEL_FORCE)) ? true : false;

        m_chgInfo_changedProperty = NULL;

        // If truly modified, schedule value as pending.
        if ( selected->GetEditorClass()->
                GetValueFromControl( variant,
                                     selected,
                                     GetEditorControl() ) )
        {
            if ( DoEditorValidate() &&
                 PerformValidation(selected, variant) )
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

        m_inCommitChangesFromEditor = false;

        bool res = true;

        if ( validationFailure && !forceSuccess )
        {
            if (oldFocus)
            {
                oldFocus->SetFocus();
                m_curFocused = oldFocus;
            }

            res = OnValidationFailure(selected, variant);

            // Now prevent further validation failure messages
            if ( res )
            {
                EditorsValueWasNotModified();
                OnValidationFailureReset(selected);
            }
        }
        else if ( valueIsPending )
        {
            DoPropertyChanged( selected, flags );
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
    m_validationInfo.m_isFailing = true;

    //
    // Variant list a special value that cannot be validated
    // by normal means.
    if ( !pendingValue.IsType(wxPG_VARIANT_TYPE_LIST) )
    {
        if ( !p->ValidateValue(pendingValue, m_validationInfo) )
            return false;
    }

    //
    // Adapt list to child values, if necessary
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

    wxVariant listValue = pendingValue;
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

    if ( !pPendingValue->IsType(wxPG_VARIANT_TYPE_LIST) )
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
            if ( changedProperty == GetSelection() )
            {
                wxWindow* editor = GetEditorControl();
                wxASSERT( wxDynamicCast(editor, wxTextCtrl) );
                evtChangingValue = wxStaticCast(editor, wxTextCtrl)->GetValue();
            }
            else
            {
                wxLogDebug(wxS("WARNING: wxEVT_PG_CHANGING is about to happen with old value."));
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
    if ( p != changedProperty && !value.IsType(wxPG_VARIANT_TYPE_LIST) )
    {
        if ( !changedProperty->ValidateValue(value, m_validationInfo) )
            return false;
    }

    if ( flags & SendEvtChanging )
    {
        // SendEvent returns true if event was vetoed
        if ( SendEvent( wxEVT_PG_CHANGING, evtChangingProperty,
                        &evtChangingValue ) )
            return false;
    }

    if ( flags & IsStandaloneValidation )
    {
        // If called in 'generic' context, we need to reset
        // m_chgInfo_changedProperty and write back translated value.
        m_chgInfo_changedProperty = NULL;
        pendingValue = value;
    }

    m_validationInfo.m_isFailing = false;

    return true;
}

// -----------------------------------------------------------------------

#if wxUSE_STATUSBAR
wxStatusBar* wxPropertyGrid::GetStatusBar()
{
    wxWindow* topWnd = ::wxGetTopLevelParent(this);
    if ( wxDynamicCast(topWnd, wxFrame) )
    {
        wxFrame* pFrame = wxStaticCast(topWnd, wxFrame);
        if ( pFrame )
            return pFrame->GetStatusBar();
    }
    return NULL;
}
#endif

// -----------------------------------------------------------------------

void wxPropertyGrid::DoShowPropertyError( wxPGProperty* WXUNUSED(property), const wxString& msg )
{
    if ( msg.empty() )
        return;

#if wxUSE_STATUSBAR
    if ( !wxPGGlobalVars->m_offline )
    {
        wxStatusBar* pStatusBar = GetStatusBar();
        if ( pStatusBar )
        {
            pStatusBar->SetStatusText(msg);
            return;
        }
    }
#endif

    /* TRANSLATORS: Caption of message box displaying any property error */
    ::wxMessageBox(msg, _("Property Error"));
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoHidePropertyError( wxPGProperty* WXUNUSED(property) )
{
#if wxUSE_STATUSBAR
    if ( !wxPGGlobalVars->m_offline )
    {
        wxStatusBar* pStatusBar = GetStatusBar();
        if ( pStatusBar )
        {
            pStatusBar->SetStatusText(wxEmptyString);
            return;
        }
    }
#endif
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::OnValidationFailure( wxPGProperty* property,
                                          wxVariant& invalidValue )
{
    if ( m_inOnValidationFailure )
        return true;

    m_inOnValidationFailure = true;
    wxON_BLOCK_EXIT_SET(m_inOnValidationFailure, false);

    wxWindow* editor = GetEditorControl();
    int vfb = m_validationInfo.m_failureBehavior;

    if ( m_inDoSelectProperty )
    {
        // When property selection is being changed, do not display any
        // messages, if some were already shown for this property.
        if ( property->HasFlag(wxPG_PROP_INVALID_VALUE) )
        {
            m_validationInfo.m_failureBehavior =
                vfb & ~(wxPG_VFB_SHOW_MESSAGE |
                        wxPG_VFB_SHOW_MESSAGEBOX |
                        wxPG_VFB_SHOW_MESSAGE_ON_STATUSBAR);
        }
    }

    // First call property's handler
    property->OnValidationFailure(invalidValue);

    bool res = DoOnValidationFailure(property, invalidValue);

    //
    // For non-wxTextCtrl editors, we do need to revert the value
    if ( !wxDynamicCast(editor, wxTextCtrl) &&
         property == GetSelection() )
    {
        property->GetEditorClass()->UpdateControl(property, editor);
    }

    property->SetFlag(wxPG_PROP_INVALID_VALUE);

    return res;
}

bool wxPropertyGrid::DoOnValidationFailure( wxPGProperty* property, wxVariant& WXUNUSED(invalidValue) )
{
    int vfb = m_validationInfo.m_failureBehavior;

    if ( vfb & wxPG_VFB_BEEP )
        ::wxBell();

    if ( (vfb & wxPG_VFB_MARK_CELL) &&
         !property->HasFlag(wxPG_PROP_INVALID_VALUE) )
    {
        unsigned int colCount = m_pState->GetColumnCount();

        // We need backup marked property's cells
        m_propCellsBackup = property->m_cells;

        wxColour vfbFg = *wxWHITE;
        wxColour vfbBg = *wxRED;

        property->EnsureCells(colCount);

        for ( unsigned int i=0; i<colCount; i++ )
        {
            wxPGCell& cell = property->m_cells[i];
            cell.SetFgCol(vfbFg);
            cell.SetBgCol(vfbBg);
        }

        if ( property == GetSelection() )
        {
            SetInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL);

            wxWindow* editor = GetEditorControl();
            if ( editor )
            {
                editor->SetForegroundColour(vfbFg);
                editor->SetBackgroundColour(vfbBg);
            }
        }

        DrawItemAndChildren(property);
    }

    if ( vfb & (wxPG_VFB_SHOW_MESSAGE |
                wxPG_VFB_SHOW_MESSAGEBOX |
                wxPG_VFB_SHOW_MESSAGE_ON_STATUSBAR) )
    {
        wxString msg = m_validationInfo.m_failureMessage;

        if ( msg.empty() )
            msg = _("You have entered invalid value. Press ESC to cancel editing.");

    #if wxUSE_STATUSBAR
        if ( vfb & wxPG_VFB_SHOW_MESSAGE_ON_STATUSBAR )
        {
            if ( !wxPGGlobalVars->m_offline )
            {
                wxStatusBar* pStatusBar = GetStatusBar();
                if ( pStatusBar )
                    pStatusBar->SetStatusText(msg);
            }
        }
    #endif

        // Displaying error dialog box can cause (native) focus changes
        // so let's preserve the current focus in order to restore it afterwards.
        wxWindow* focusedWnd = wxWindow::FindFocus();

        if ( vfb & wxPG_VFB_SHOW_MESSAGE )
            DoShowPropertyError(property, msg);

        if ( vfb & wxPG_VFB_SHOW_MESSAGEBOX )
            /* TRANSLATORS: Caption of message box displaying any property error */
            ::wxMessageBox(msg, _("Property Error"));

        // Restore the focus
        if ( focusedWnd )
        {
            focusedWnd->SetFocus();
        }
    }

    return (vfb & wxPG_VFB_STAY_IN_PROPERTY) ? false : true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::DoOnValidationFailureReset( wxPGProperty* property )
{
    int vfb = m_validationInfo.m_failureBehavior;

    if ( vfb & wxPG_VFB_MARK_CELL )
    {
        // Revert cells
        property->m_cells = m_propCellsBackup;

        ClearInternalFlag(wxPG_FL_CELL_OVERRIDES_SEL);

        if ( property == GetSelection() && GetEditorControl() )
        {
            // Calling this will recreate the control, thus resetting its colour
            RefreshProperty(property);
        }
        else
        {
            DrawItemAndChildren(property);
        }
    }

#if wxUSE_STATUSBAR
    if ( vfb & wxPG_VFB_SHOW_MESSAGE_ON_STATUSBAR )
    {
        if ( !wxPGGlobalVars->m_offline )
        {
            wxStatusBar* pStatusBar = GetStatusBar();
            if ( pStatusBar )
                pStatusBar->SetStatusText(wxEmptyString);
        }
    }
#endif

    if ( vfb & wxPG_VFB_SHOW_MESSAGE )
    {
        DoHidePropertyError(property);
    }

    m_validationInfo.m_isFailing = false;
}

// -----------------------------------------------------------------------

// flags are same as with DoSelectProperty
bool wxPropertyGrid::DoPropertyChanged( wxPGProperty* p, unsigned int selFlags )
{
    if ( m_inDoPropertyChanged )
        return true;

    m_inDoPropertyChanged = true;
    wxON_BLOCK_EXIT_SET(m_inDoPropertyChanged, false);

    wxPGProperty* selected = GetSelection();

    m_pState->m_anyModified = true;

    // Maybe need to update control
    wxASSERT( m_chgInfo_changedProperty != NULL );

    // These values were calculated in PerformValidation()
    wxPGProperty* changedProperty = m_chgInfo_changedProperty;
    wxVariant value = m_chgInfo_pendingValue;

    // If property's value is being changed, assume it is valid
    OnValidationFailureReset(selected);

    changedProperty->SetValue(value, &m_chgInfo_valueList, wxPG_SETVAL_BY_USER);

    // NB: Call GetEditorControl() as late as possible, because OnSetValue()
    //     and perhaps other user-defined virtual functions may change it.
    wxWindow* editor = GetEditorControl();

    // Set as Modified (not if dragging just began)
    if ( !p->HasFlag(wxPG_PROP_MODIFIED) )
    {
        p->SetFlag(wxPG_PROP_MODIFIED);
        if ( p == selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
        {
            if ( editor )
                SetCurControlBoldFont();
        }
    }

    // Propagate updates to parent(s)
    wxPGProperty* topPaintedProperty = changedProperty->GetMainParent();
    wxPGProperty* pwc = p;
    wxPGProperty* prevPwc = NULL;

    while ( prevPwc != topPaintedProperty )
    {
        pwc->SetFlag(wxPG_PROP_MODIFIED);

        if ( pwc == selected && (m_windowStyle & wxPG_BOLD_MODIFIED) )
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
        RefreshEditor();
    }
    else
    {
#if wxPG_REFRESH_CONTROLS
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
            SendEvent( wxEVT_PG_CHANGED, pwc, NULL );
            pwc = pwc->GetParent();
        }
    }

    SendEvent( wxEVT_PG_CHANGED, changedProperty, NULL );

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
#if wxUSE_VALIDATORS
    wxRecursionGuard guard(m_validatingEditor);
    if ( guard.IsInside() )
        return false;

    m_validationInfo.m_failureBehavior = m_permanentValidationFailureBehavior;
    m_validationInfo.m_isFailing = true;

    wxPGProperty* selected = GetSelection();
    if ( selected )
    {
        wxWindow* wnd = GetEditorControl();

        wxValidator* validator = selected->GetValidator();
        if ( validator && wnd )
        {
            validator->SetWindow(wnd);
            if ( !validator->Validate(this) )
                return false;
        }
    }

    m_validationInfo.m_isFailing = false;
#endif
    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleCustomEditorEvent( wxEvent &event )
{
    //
    // NB: We should return true if the event was recognized as
    //     a dedicated wxPropertyGrid event, and as such was
    //     either properly handled or ignored.
    //

    // It is possible that this handler receives event even before
    // the control has been properly initialized. Let's skip the
    // event handling in that case.
    if ( !m_pState )
        return false;

    // Don't care about the event if it originated from the
    // 'label editor'. In this function we only care about the
    // property value editor.
    if ( m_labelEditor && event.GetId() == m_labelEditor->GetId() )
    {
        event.Skip();
        return true;
    }

    wxPGProperty* selected = GetSelection();

    // Somehow, event is handled after property has been deselected.
    // Possibly, but very rare.
    if ( !selected ||
          selected->HasFlag(wxPG_PROP_BEING_DELETED) ||
          m_inOnValidationFailure ||
          // Also don't handle editor event if wxEVT_PG_CHANGED or
          // similar is currently doing something (showing a
          // message box, for instance).
          m_processedEvent )
        return true;

    if ( m_iFlags & wxPG_FL_IN_HANDLECUSTOMEDITOREVENT )
        return true;

    wxVariant pendingValue(selected->GetValueRef());
    wxWindow* wnd = GetEditorControl();
    wxWindow* editorWnd = wxDynamicCast(event.GetEventObject(), wxWindow);
    int selFlags = 0;
    bool wasUnspecified = selected->IsValueUnspecified();
    int usesAutoUnspecified = selected->UsesAutoUnspecified();
    bool valueIsPending = false;

    m_chgInfo_changedProperty = NULL;

    m_iFlags &= ~wxPG_FL_VALUE_CHANGE_IN_EVENT;

    //
    // Ignore focus changes within the composite editor control
    if ( event.GetEventType() == wxEVT_SET_FOCUS || event.GetEventType() == wxEVT_KILL_FOCUS )
    {
        wxFocusEvent* fevt = wxDynamicCast(&event, wxFocusEvent);
        wxWindow* win = fevt->GetWindow();
        while ( win )
        {
            if ( win == wnd )
            {
                event.Skip();
                return true;
            }

            win = win->GetParent();
        }
    }
    // Filter out excess wxTextCtrl modified events
    else if ( event.GetEventType() == wxEVT_TEXT && wnd )
    {
        if ( wxDynamicCast(wnd, wxTextCtrl) )
        {
            wxTextCtrl* tc = (wxTextCtrl*) wnd;

            wxString newTcValue = tc->GetValue();
            if ( m_prevTcValue == newTcValue )
                return true;
            m_prevTcValue = newTcValue;
        }
        else if ( wxDynamicCast(wnd, wxComboCtrl) )
        {
            // In some cases we might stumble unintentionally on
            // wxComboCtrl's embedded wxTextCtrl's events. Let's
            // avoid them.
            if ( wxDynamicCast(editorWnd, wxTextCtrl) )
                return false;

            wxComboCtrl* cc = (wxComboCtrl*) wnd;

            wxString newTcValue = cc->GetTextCtrl()->GetValue();
            if ( m_prevTcValue == newTcValue )
                return true;
            m_prevTcValue = newTcValue;
        }
    }

    SetInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT);

    bool validationFailure = false;
    bool buttonWasHandled = false;
    bool result = false;

    //
    // Try common button handling
    if ( IsMainButtonEvent(event) )
    {
        wxPGEditorDialogAdapter* adapter = selected->GetEditorDialog();

        if ( adapter )
        {
            buttonWasHandled = true;
            // Store as res2, as previously (and still currently alternatively)
            // dialogs can be shown by handling wxEVT_BUTTON
            // in wxPGProperty::OnEvent().
            adapter->ShowDialog( this, selected );
            delete adapter;
        }
    }

    if ( !buttonWasHandled )
    {
        if ( wnd || m_wndEditor2 )
        {
            // First call editor class' event handler.
            const wxPGEditor* editor = selected->GetEditorClass();

            if ( editor->OnEvent( this, selected, editorWnd, event ) )
            {
                result = true;

                // If changes, validate them
                if ( DoEditorValidate() )
                {
                    if ( editor->GetValueFromControl( pendingValue,
                                                      selected,
                                                      wnd ) )
                        valueIsPending = true;

                    // Mark value always as pending if validation is currently
                    // failing and value was not unspecified
                    if ( !valueIsPending &&
                         !pendingValue.IsNull() &&
                         m_validationInfo.m_isFailing )
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
            buttonWasHandled = selected->OnEvent( this, editorWnd, event );
    }

    // SetValueInEvent(), as called in one of the functions referred above
    // overrides editor's value.
    if ( WasValueChangedInEvent() )
    {
        valueIsPending = true;
        pendingValue = m_changeInEventValue;
        selFlags |= wxPG_SEL_DIALOGVAL;
    }

    if ( !validationFailure && valueIsPending )
        if ( !PerformValidation(selected, pendingValue) )
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
        if ( event.GetEventType() == wxEVT_TEXT_ENTER )
        {
            SetFocusOnCanvas();
        }
    }
    else
    {
        // No value after all

        // Regardless of editor type, unfocus editor on
        // text-editing related enter press.
        if ( event.GetEventType() == wxEVT_TEXT_ENTER )
        {
            SetFocusOnCanvas();
        }

        // Let unhandled button click events go to the parent
        if ( !buttonWasHandled && event.GetEventType() == wxEVT_BUTTON )
        {
            result = true;
            wxCommandEvent evt(wxEVT_BUTTON,GetId());
            GetEventHandler()->AddPendingEvent(evt);
        }
    }

    ClearInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT);

    return result;
}

// -----------------------------------------------------------------------
// wxPropertyGrid editor control helper methods
// -----------------------------------------------------------------------

wxRect wxPropertyGrid::GetEditorWidgetRect( wxPGProperty* p, int column ) const
{
    int itemy = p->GetY2(m_lineHeight);
    int splitterX = m_pState->DoGetSplitterPosition(column-1);
    int colEnd = splitterX + m_pState->GetColumnWidth(column);
    int imageOffset = 0;

    int vx, vy;  // Top left corner of client
    GetViewStart(&vx, &vy);
    vx *= wxPG_PIXELS_PER_UNIT;
    vy *= wxPG_PIXELS_PER_UNIT;

    if ( column == 1 )
    {
        // TODO: If custom image detection changes from current, change this.
        if ( m_iFlags & wxPG_FL_CUR_USES_CUSTOM_IMAGE )
        {
            //m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;
            int iw = p->OnMeasureImage().x;
            wxASSERT( iw == wxDefaultCoord || iw >= 0 );
            if ( iw == wxDefaultCoord || iw == 0 )
                iw = wxPG_CUSTOM_IMAGE_WIDTH;
            imageOffset = p->GetImageOffset(iw);
        }
    }
    else if ( column == 0 )
    {
        splitterX += (p->GetDepth() - 1) * m_subgroup_extramargin;
    }

    return wxRect
      (
        splitterX+imageOffset+wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1-vx,
        itemy-vy,
        colEnd-splitterX-wxPG_XBEFOREWIDGET-wxPG_CONTROL_MARGIN-imageOffset-1,
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

    int choiceCount = p->GetChoices().GetCount();
    int comVals = p->GetDisplayedCommonValueCount();
    if ( item >= choiceCount && comVals > 0 )
    {
        unsigned int cvi = item-choiceCount;
        cis = GetCommonValue(cvi)->GetRenderer()->GetImageSize(NULL, 1, cvi);
    }
    else if ( item >= 0 && choiceCount == 0 )
        return wxSize(0, 0);

    wxASSERT( cis.x == wxDefaultCoord || cis.x >= 0 );
    if ( cis.x == wxDefaultCoord )
    {
        cis.x = wxPG_CUSTOM_IMAGE_WIDTH;
    }
    wxASSERT( cis.y == wxDefaultCoord || cis.y >= 0 );
    if ( cis.y == wxDefaultCoord || cis.y == 0 )
    {
        cis.y = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
    }
    return cis;
}

// -----------------------------------------------------------------------

// takes scrolling into account
void wxPropertyGrid::ImprovedClientToScreen( int* px, int* py )
{
    wxASSERT(px && py);
    CalcScrolledPosition(*px, *py, px, py);
    ClientToScreen( px, py );
}

// -----------------------------------------------------------------------

wxPropertyGridHitTestResult wxPropertyGrid::HitTest( const wxPoint& pt ) const
{
    return m_pState->HitTest(CalcUnscrolledPosition(pt));
}

// -----------------------------------------------------------------------

// custom set cursor
void wxPropertyGrid::CustomSetCursor( int type, bool override )
{
    if ( type == m_curcursor && !override ) return;

    wxCursor* cursor = &wxPG_DEFAULT_CURSOR;

    if ( type == wxCURSOR_SIZEWE )
        cursor = m_cursorSizeWE;

    SetCursor( *cursor );

    m_curcursor = type;
}

// -----------------------------------------------------------------------

wxString
wxPropertyGrid::GetUnspecifiedValueText( int argFlags ) const
{
    const wxPGCell& ua = GetUnspecifiedValueAppearance();

    if ( ua.HasText() &&
         !(argFlags & wxPG_FULL_VALUE) &&
         !(argFlags & wxPG_EDITABLE_VALUE) )
        return ua.GetText();

    return wxEmptyString;
}

// -----------------------------------------------------------------------
// wxPropertyGrid property selection, editor creation
// -----------------------------------------------------------------------

//
// This class forwards events from property editor controls to wxPropertyGrid.
class wxPropertyGridEditorEventForwarder : public wxEvtHandler
{
public:
    wxPropertyGridEditorEventForwarder( wxPropertyGrid* propGrid )
        : wxEvtHandler(), m_propGrid(propGrid)
    {
    }

    virtual ~wxPropertyGridEditorEventForwarder()
    {
    }

private:
    bool ProcessEvent( wxEvent& event ) wxOVERRIDE
    {
        // Always skip
        event.Skip();

        m_propGrid->HandleCustomEditorEvent(event);

        //
        // NB: We should return true if the event was recognized as
        //     a dedicated wxPropertyGrid event, and as such was
        //     either properly handled or ignored.
        //
        if ( m_propGrid->IsMainButtonEvent(event) )
            return true;

        //
        // NB: On wxMSW, a wxTextCtrl with wxTE_PROCESS_ENTER
        //     may beep annoyingly if that event is skipped
        //     and passed to parent event handler.
        if ( event.GetEventType() == wxEVT_TEXT_ENTER )
            return true;

        return wxEvtHandler::ProcessEvent(event);
    }

    wxPropertyGrid*         m_propGrid;
};

// Setups event handling for child control
void wxPropertyGrid::SetupChildEventHandling( wxWindow* argWnd )
{
    wxWindowID id = argWnd->GetId();

    if ( argWnd == m_wndEditor )
    {
        argWnd->Bind(wxEVT_MOTION, &wxPropertyGrid::OnMouseMoveChild, this, id);
        argWnd->Bind(wxEVT_LEFT_UP, &wxPropertyGrid::OnMouseUpChild, this, id);
        argWnd->Bind(wxEVT_LEFT_DOWN, &wxPropertyGrid::OnMouseClickChild, this, id);
        argWnd->Bind(wxEVT_RIGHT_UP, &wxPropertyGrid::OnMouseRightClickChild, this, id);
        argWnd->Bind(wxEVT_ENTER_WINDOW, &wxPropertyGrid::OnMouseEntry, this, id);
        argWnd->Bind(wxEVT_LEAVE_WINDOW, &wxPropertyGrid::OnMouseEntry, this, id);
    }

    wxPropertyGridEditorEventForwarder* forwarder;
    forwarder = new wxPropertyGridEditorEventForwarder(this);
    argWnd->PushEventHandler(forwarder);

    argWnd->Bind(wxEVT_KEY_DOWN, &wxPropertyGrid::OnChildKeyDown, this, id);
}

void wxPropertyGrid::DeletePendingObjects()
{
#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    // Delete pending property editors and their event handlers.
    while ( !m_deletedEditorObjects.empty() )
    {
        wxObject* obj = m_deletedEditorObjects.back();
        m_deletedEditorObjects.pop_back();

        delete obj;
    }
}

void wxPropertyGrid::DestroyEditorWnd( wxWindow* wnd )
{
    if ( !wnd )
        return;

    wnd->Hide();

    // Do not free editors immediately (for sake of processing events)
#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    m_deletedEditorObjects.push_back(wnd);
}

void wxPropertyGrid::FreeEditors()
{
    //
    // Return focus back to canvas from children (this is required at least for
    // GTK+, which, unlike Windows, clears focus when control is destroyed
    // instead of moving it to closest parent).
    SetFocusOnCanvas();

#if WXWIN_COMPATIBILITY_3_0
    // Emulate member variable.
    wxArrayPGObject& m_deletedEditorObjects = *gs_deletedEditorObjects[this];
#endif
    // Do not free editors immediately if processing events
    if ( m_wndEditor2 )
    {
        wxEvtHandler* handler = m_wndEditor2->PopEventHandler(false);
        m_wndEditor2->Hide();
        m_deletedEditorObjects.push_back(handler);
        DestroyEditorWnd(m_wndEditor2);
        m_wndEditor2 = NULL;
    }

    if ( m_wndEditor )
    {
        wxEvtHandler* handler = m_wndEditor->PopEventHandler(false);
        m_wndEditor->Hide();
        m_deletedEditorObjects.push_back(handler);
        DestroyEditorWnd(m_wndEditor);
        m_wndEditor = NULL;
    }
}

// Call with NULL to de-select property
bool wxPropertyGrid::DoSelectProperty( wxPGProperty* p, unsigned int flags )
{
    /*
    if (p)
    {
        wxLogDebug(wxS("SelectProperty( %s (%s[%i]) )"),p->GetLabel(),
            p->m_parent->GetLabel(),p->GetIndexInParent());
    }
    else
    {
        wxLogDebug(wxS("SelectProperty( NULL, -1 )"));
    }
    */

    if ( m_inDoSelectProperty )
        return true;

    m_inDoSelectProperty = true;
    wxON_BLOCK_EXIT_SET(m_inDoSelectProperty, false);

    if ( !m_pState )
        return false;

    wxArrayPGProperty prevSelection = m_pState->m_selection;
    wxPGProperty* prevFirstSel;

    prevFirstSel = prevSelection.empty()? NULL: prevSelection[0];

    if ( prevFirstSel && prevFirstSel->HasFlag(wxPG_PROP_BEING_DELETED) )
        prevFirstSel = NULL;

    // Always send event, as this is indirect call
    DoEndLabelEdit(true, wxPG_SEL_NOVALIDATE);

/*
    if ( prevFirstSel )
        wxPrintf( "Selected %s\n", prevFirstSel->GetClassInfo()->GetClassName() );
    else
        wxPrintf( "None selected\n" );

    if (p)
        wxPrintf( "P =  %s\n", p->GetClassInfo()->GetClassName() );
    else
        wxPrintf( "P = NULL\n" );
*/

    wxWindow* primaryCtrl = NULL;

    // If we are frozen, then just set the values.
    if ( IsFrozen() )
    {
        m_iFlags &= ~(wxPG_FL_ABNORMAL_EDITOR);
        m_editorFocused = false;
        m_pState->DoSetSelection(p);

        // If frozen, always free controls. But don't worry, as Thaw will
        // recall SelectProperty to recreate them.
        FreeEditors();

        // Prevent any further selection measures in this call
        p = NULL;
    }
    else
    {
        // Is it the same?
        if ( prevFirstSel == p &&
             prevSelection.size() <= 1 &&
             !(flags & wxPG_SEL_FORCE) )
        {
            // Only set focus if not deselecting
            if ( p )
            {
                if ( flags & wxPG_SEL_FOCUS )
                {
                    if ( m_wndEditor )
                    {
                        m_wndEditor->SetFocus();
                        m_editorFocused = true;
                    }
                }
                else
                {
                    SetFocusOnCanvas();
                }
            }

            return true;
        }

        //
        // First, deactivate previous
        if ( prevFirstSel )
        {
            // Must double-check if this is an selected in case of force switch
            if ( p != prevFirstSel )
            {
                if ( !CommitChangesFromEditor(flags) )
                {
                    // Validation has failed, so we can't exit the previous editor
                    //::wxMessageBox(_("Please correct the value or press ESC to cancel the edit."),
                    //               _("Invalid Value"),wxOK|wxICON_ERROR);
                    return false;
                }
            }

            // This should be called after CommitChangesFromEditor(), so that
            // OnValidationFailure() still has information on property's
            // validation state.
            OnValidationFailureReset(prevFirstSel);

            FreeEditors();

            m_iFlags &= ~(wxPG_FL_ABNORMAL_EDITOR);
            EditorsValueWasNotModified();
        }

        SetInternalFlag(wxPG_FL_IN_SELECT_PROPERTY);

        m_pState->DoSetSelection(p);

        // Redraw unselected
        for ( unsigned int i=0; i<prevSelection.size(); i++ )
        {
            DrawItem(prevSelection[i]);
        }

        //
        // Then, activate the one given.
        if ( p )
        {
            int propY = p->GetY2(m_lineHeight);

            int splitterX;
            CalcScrolledPosition(GetSplitterPosition(), 0, &splitterX, NULL);

            m_editorFocused = false;
            m_iFlags |= wxPG_FL_PRIMARY_FILLS_ENTIRE;

            wxASSERT( m_wndEditor == NULL );

            //
            // Only create editor for non-disabled non-caption
            if ( !p->IsCategory() && !p->HasFlag(wxPG_PROP_DISABLED) )
            {
            // do this for non-caption items

                m_selColumn = 1;

                // Do we need to paint the custom image, if any?
                m_iFlags &= ~(wxPG_FL_CUR_USES_CUSTOM_IMAGE);
                if ( p->HasFlag(wxPG_PROP_CUSTOMIMAGE) &&
                     !p->GetEditorClass()->CanContainCustomImage()
                   )
                    m_iFlags |= wxPG_FL_CUR_USES_CUSTOM_IMAGE;

                wxRect grect = GetEditorWidgetRect(p, m_selColumn);
                wxPoint goodPos = grect.GetPosition();

                // Editor appearance can now be considered clear
                m_editorAppearance.SetEmptyData();

                const wxPGEditor* editor = p->GetEditorClass();
                wxCHECK_MSG(editor, false,
                    wxS("NULL editor class not allowed"));

                m_iFlags &= ~wxPG_FL_FIXED_WIDTH_EDITOR;

                wxPGWindowList wndList =
                    editor->CreateControls(this,
                                           p,
                                           goodPos,
                                           grect.GetSize());

                m_wndEditor = wndList.GetPrimary();
                m_wndEditor2 = wndList.GetSecondary();
                // Remember actual positions within required cell.
                // These values can be used when there will be required
                // to reposition the cell.
                if ( m_wndEditor )
                {
                    m_wndEditorPosRel = m_wndEditor->GetPosition() - goodPos;
                }
                if ( m_wndEditor2 )
                {
                    m_wndEditor2PosRel = m_wndEditor2->GetPosition() - goodPos;
                }
                primaryCtrl = GetEditorControl();

                //
                // Essentially, primaryCtrl == m_wndEditor
                //

                // NOTE: It is allowed for m_wndEditor to be NULL - in this
                //       case value is drawn as normal, and m_wndEditor2 is
                //       assumed to be a right-aligned button that triggers
                //       a separate editorCtrl window.

                if ( m_wndEditor )
                {
                    wxASSERT_MSG( m_wndEditor->GetParent() == GetPanel(),
                                  wxS("CreateControls must use result of ")
                                  wxS("wxPropertyGrid::GetPanel() as parent ")
                                  wxS("of controls.") );

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
                    if ( p->HasFlag(wxPG_PROP_MODIFIED) &&
                         (m_windowStyle & wxPG_BOLD_MODIFIED) )
                        SetCurControlBoldFont();
                    // Store x relative to splitter (we'll need it).
                    m_ctrlXAdjust = m_wndEditor->GetPosition().x - splitterX;

                    // Check if background clear is not necessary
                    wxPoint pos = m_wndEditor->GetPosition();
                    if ( pos.x > (splitterX+1) || pos.y > propY )
                    {
                        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
                    }

                    m_wndEditor->SetSizeHints(3, 3);

                    SetupChildEventHandling(primaryCtrl);

                    // Focus and select all (wxTextCtrl, wxComboBox etc.)
                    if ( flags & wxPG_SEL_FOCUS )
                    {
                        primaryCtrl->SetFocus();

                        p->GetEditorClass()->OnFocus(p, primaryCtrl);
                    }
                    else
                    {
                        if ( p->IsValueUnspecified() )
                            SetEditorAppearance(m_unspecifiedAppearance,
                                                true);
                    }
                }

                if ( m_wndEditor2 )
                {
                    wxASSERT_MSG( m_wndEditor2->GetParent() == GetPanel(),
                                  wxS("CreateControls must use result of ")
                                  wxS("wxPropertyGrid::GetPanel() as parent ")
                                  wxS("of controls.") );

                    // Get proper id for wndSecondary
                    m_wndSecId = m_wndEditor2->GetId();
                    wxWindowList children = m_wndEditor2->GetChildren();
                    wxWindowList::iterator node = children.begin();
                    if ( node != children.end() )
                        m_wndSecId = ((wxWindow*)*node)->GetId();

                    m_wndEditor2->SetSizeHints(3,3);

                    m_wndEditor2->Show();

                    SetupChildEventHandling(m_wndEditor2);

                    // If no primary editor, focus to button to allow
                    // it to interpret ENTER etc.
                    // NOTE: Due to problems focusing away from it, this
                    //       has been disabled.
                    /*
                    if ( (flags & wxPG_SEL_FOCUS) && !m_wndEditor )
                        m_wndEditor2->SetFocus();
                    */
                }

                if ( flags & wxPG_SEL_FOCUS )
                    m_editorFocused = true;

            }
            else
            {
                // Make sure focus is in grid canvas (important for wxGTK,
                // at least)
                SetFocusOnCanvas();
            }

            EditorsValueWasNotModified();

            // If it's inside collapsed section, expand parent, scroll, etc.
            // Also, if it was partially visible, scroll it into view.
            if ( !(flags & wxPG_SEL_NONVISIBLE) )
                EnsureVisible( p );

            if ( m_wndEditor )
            {
                m_wndEditor->Show(true);
            }

            if ( !(flags & wxPG_SEL_NO_REFRESH) )
                DrawItem(p);
        }
        else
        {
            // Make sure focus is in grid canvas
            SetFocusOnCanvas();
        }

        ClearInternalFlag(wxPG_FL_IN_SELECT_PROPERTY);
    }

    const wxString* pHelpString = NULL;

    if ( p )
        pHelpString = &p->GetHelpString();

    if ( !HasExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS) )
    {
#if wxUSE_STATUSBAR

        //
        // Show help text in status bar.
        //   (if found and grid not embedded in manager with help box and
        //    style wxPG_EX_HELP_AS_TOOLTIPS is not used).
        //
        wxStatusBar* statusbar = GetStatusBar();
        if ( statusbar )
        {
            if ( pHelpString && !pHelpString->empty() )
            {
                // Set help box text.
                statusbar->SetStatusText( *pHelpString );
                m_iFlags |= wxPG_FL_STRING_IN_STATUSBAR;
            }
            else if ( m_iFlags & wxPG_FL_STRING_IN_STATUSBAR )
            {
                // Clear help box - but only if it was written
                // by us at previous time.
                statusbar->SetStatusText(wxEmptyString);
                m_iFlags &= ~(wxPG_FL_STRING_IN_STATUSBAR);
            }
        }
#endif
    }
    else
    {
#if wxUSE_TOOLTIPS
        //
        // Show help as a tool tip on the editor control.
        //
        if ( pHelpString && !pHelpString->empty() &&
             primaryCtrl )
        {
            primaryCtrl->SetToolTip(*pHelpString);
        }
#endif
    }

    // call wx event handler (here so that it also occurs on deselection)
    // In case of deselection previously selected property
    // is passed to the event object.
    if (!p)
    {
        p = prevFirstSel;
    }
    if ( !(flags & wxPG_SEL_DONT_SEND_EVENT) && p)
        SendEvent( wxEVT_PG_SELECTED, p, NULL );

    return true;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::UnfocusEditor()
{
    wxPGProperty* selected = GetSelection();

    if ( !selected || !m_wndEditor || IsFrozen() )
        return true;

    if ( !CommitChangesFromEditor(0) )
        return false;

    SetFocusOnCanvas();
    DrawItem(selected);

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::RefreshEditor()
{
    wxPGProperty* p = GetSelection();
    if ( !p )
        return;

    wxWindow* wnd = GetEditorControl();
    if ( !wnd )
        return;

    // Set editor font boldness - must do this before
    // calling UpdateControl().
    if ( HasFlag(wxPG_BOLD_MODIFIED) )
    {
        if ( p->HasFlag(wxPG_PROP_MODIFIED) )
            wnd->SetFont(GetCaptionFont());
        else
            wnd->SetFont(GetFont());
    }

    const wxPGEditor* editorClass = p->GetEditorClass();

    editorClass->UpdateControl(p, wnd);

    if ( p->IsValueUnspecified() )
        SetEditorAppearance(m_unspecifiedAppearance, true);
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::SelectProperty( wxPGPropArg id, bool focus )
{
    wxPG_PROP_ARG_CALL_PROLOG_RETVAL(false)

    unsigned int flags = wxPG_SEL_DONT_SEND_EVENT;
    if ( focus )
        flags |= wxPG_SEL_FOCUS;

    return DoSelectProperty(p, flags);
}

// -----------------------------------------------------------------------
// wxPropertyGrid expand/collapse state
// -----------------------------------------------------------------------

bool wxPropertyGrid::DoCollapse( wxPGProperty* p, bool sendEvents )
{
    wxPGProperty* pwc = wxStaticCast(p, wxPGProperty);
    wxPGProperty* selected = GetSelection();

    // If active editor was inside collapsed section, then disable it
    if ( selected && selected->IsSomeParent(p) )
    {
        DoClearSelection();
    }

    // Store dont-center-splitter flag because we need to temporarily set it
    bool prevDontCenterSplitter = m_pState->m_dontCenterSplitter;
    m_pState->m_dontCenterSplitter = true;

    bool res = m_pState->DoCollapse(pwc);

    if ( res )
    {
        if ( sendEvents )
            SendEvent( wxEVT_PG_ITEM_COLLAPSED, p );

        RecalculateVirtualSize();
        Refresh();
    }

    m_pState->m_dontCenterSplitter = prevDontCenterSplitter;

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoExpand( wxPGProperty* p, bool sendEvents )
{
    wxCHECK_MSG( p, false, wxS("invalid property id") );

    // Store dont-center-splitter flag 'cause we need to temporarily set it
    bool prevDontCenterSplitter = m_pState->m_dontCenterSplitter;
    m_pState->m_dontCenterSplitter = true;

    bool res = m_pState->DoExpand(p);

    if ( res )
    {
        if ( sendEvents )
            SendEvent( wxEVT_PG_ITEM_EXPANDED, p );

        RecalculateVirtualSize();
        Refresh();
    }

    m_pState->m_dontCenterSplitter = prevDontCenterSplitter;

    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::DoHideProperty( wxPGProperty* p, bool hide, int flags )
{
    if ( IsFrozen() )
        return m_pState->DoHideProperty(p, hide, flags);

    wxArrayPGProperty selection = m_pState->m_selection;  // Must use a copy
    for ( unsigned int i=0; i<selection.size(); i++ )
    {
        wxPGProperty* selected = selection[i];
        if ( selected == p || selected->IsSomeParent(p) )
        {
            if ( !DoRemoveFromSelection(p, flags) )
                return false;
        }
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
    // Don't check for !HasInternalFlag(wxPG_FL_INITIALIZED) here. Otherwise
    // virtual size calculation may go wrong.
    if ( HasInternalFlag(wxPG_FL_RECALCULATING_VIRTUAL_SIZE) ||
         IsFrozen() ||
         !m_pState )
        return;

    //
    // If virtual height was changed, then recalculate editor control position(s)
    if ( m_pState->m_vhCalcPending )
        CorrectEditorWidgetPosY();

    m_pState->EnsureVirtualHeight();

    wxASSERT_LEVEL_2_MSG(
        m_pState->GetVirtualHeight() == m_pState->GetActualVirtualHeight(),
        wxS("VirtualHeight and ActualVirtualHeight should match")
    );

    m_iFlags |= wxPG_FL_RECALCULATING_VIRTUAL_SIZE;

    int w = m_pState->GetVirtualWidth();
    int h = m_pState->m_virtualHeight;
    // Now adjust virtual size.
    SetVirtualSize(w, h);
    if ( forceXPos != -1 )
    {
        Scroll(forceXPos, wxDefaultCoord);
    }

    // Must re-get size now
    GetClientSize(&m_width, &m_height);

    if ( !HasVirtualWidth() )
    {
        m_pState->SetVirtualWidth(m_width);
    }
    m_pState->CheckColumnWidths();

    if ( GetSelection() )
    {
        CorrectEditorWidgetSizeX();
    }

    m_iFlags &= ~wxPG_FL_RECALCULATING_VIRTUAL_SIZE;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnResize( wxSizeEvent& event )
{
    if ( !(m_iFlags & wxPG_FL_INITIALIZED) )
        return;

    int width, height;
    GetClientSize(&width, &height);

    m_width = width;
    m_height = height;

    if ( !HasExtraStyle(wxPG_EX_NATIVE_DOUBLE_BUFFERING) )
    {
        double scaleFactor = GetDPIScaleFactor();
        int dblh = (m_lineHeight*2);
        if ( !m_doubleBuffer )
        {
            // Create double buffer bitmap to draw on, if none
            int w = wxMax(width, 250);
            int h = wxMax(height + dblh, 400);
            m_doubleBuffer = new wxBitmap;
            m_doubleBuffer->CreateScaled( w, h, wxBITMAP_SCREEN_DEPTH, scaleFactor );
        }
        else
        {
            int w = m_doubleBuffer->GetScaledWidth();
            int h = m_doubleBuffer->GetScaledHeight();

            // Double buffer must be large enough
            if ( w < width || h < (height+dblh) )
            {
                if ( w < width ) w = width;
                if ( h < (height+dblh) ) h = height + dblh;
                delete m_doubleBuffer;
                m_doubleBuffer = new wxBitmap;
                m_doubleBuffer->CreateScaled( w, h, wxBITMAP_SCREEN_DEPTH, scaleFactor );
            }
        }
    }

    m_pState->OnClientWidthChange( width, event.GetSize().x - m_ncWidth, true );
    m_ncWidth = event.GetSize().x;

    if ( !IsFrozen() )
    {
        if ( m_pState->m_itemsAdded )
            PrepareAfterItemsAdded();
        else
            // Without this, virtual size (at least under wxGTK) will be skewed
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
    RecalculateVirtualSize();
    Refresh();
}

void wxPropertyGrid::SetFocusOnCanvas()
{
    // By default, to prevent wxPropertyGrid from stealing focus from
    // other controls, only move focus to the grid if it was already
    // in one of its child controls.
    // If wxPG_EX_ALWAYS_ALLOW_FOCUS flag is set then wxPropertyGrid
    // can take focus on the entire grid area (canvas) even if focus
    // is moved from another control.
    if ( HasExtraStyle(wxPG_EX_ALWAYS_ALLOW_FOCUS) )
    {
        SetFocus();
    }
    else
    {
        wxWindow* focus = wxWindow::FindFocus();
        if ( focus )
        {
            wxWindow* parent = focus->GetParent();
            while ( parent )
            {
                if ( parent == this )
                {
                    SetFocus();
                    break;
                }
                parent = parent->GetParent();
            }
        }
    }

    m_editorFocused = false;
}

// -----------------------------------------------------------------------
// wxPropertyGrid mouse event handling
// -----------------------------------------------------------------------

// selFlags uses same values DoSelectProperty's flags
// Returns true if event was vetoed.
bool wxPropertyGrid::SendEvent( wxEventType eventType, wxPGProperty* p,
                                wxVariant* pValue,
                                unsigned int selFlags,
                                unsigned int column )
{
    // selFlags should have wxPG_SEL_NOVALIDATE if event is not
    // vetoable.

    // Send property grid event of specific type and with specific property
    wxPropertyGridEvent evt( eventType, m_eventObject->GetId() );
    evt.SetPropertyGrid(this);
    evt.SetEventObject(m_eventObject);
    evt.SetProperty(p);
    evt.SetColumn(column);
    if ( eventType == wxEVT_PG_CHANGING )
    {
        wxASSERT( pValue );
        evt.SetCanVeto(true);
        m_validationInfo.m_pValue = pValue;
        evt.SetupValidationInfo();
    }
    else
    {
        if ( p )
            evt.SetPropertyValue(p->GetValue());

        if ( !(selFlags & wxPG_SEL_NOVALIDATE) )
            evt.SetCanVeto(true);
    }

    wxPropertyGridEvent* prevProcessedEvent = m_processedEvent;
    m_processedEvent = &evt;
    m_eventObject->HandleWindowEvent(evt);
    m_processedEvent = prevProcessedEvent;

    return evt.WasVetoed();
}

void wxPropertyGrid::SendEvent(wxEventType eventType, int intVal)
{
    wxPropertyGridEvent evt(eventType, m_eventObject->GetId());
    evt.SetPropertyGrid(this);
    evt.SetEventObject(m_eventObject);
    evt.SetProperty(NULL);
    evt.SetColumn(0);
    evt.SetInt(intVal);

    wxPropertyGridEvent* prevProcessedEvent = m_processedEvent;
    m_processedEvent = &evt;
    m_eventObject->HandleWindowEvent(evt);
    m_processedEvent = prevProcessedEvent;
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

                int textX = m_marginWidth + ((pwc->GetDepth()-1)*m_subgroup_extramargin);

                // Expand, collapse, activate etc. if click on text or left of splitter.
                if ( x >= textX
                     &&
                     ( x < (textX+pwc->GetTextExtent(this, m_captionFont)+(wxPG_CAPRECTXMARGIN*2)) ||
                       columnHit == 0
                     )
                    )
                {
                    if ( !AddToSelectionFromInputEvent( p,
                                                        columnHit,
                                                        &event ) )
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
                if ( !AddToSelectionFromInputEvent( p,
                                                    columnHit,
                                                    &event,
                                                    selFlag ) )
                    return res;

                m_iFlags &= ~(wxPG_FL_ACTIVATION_BY_CLICK);

                if ( p->GetChildCount() && !p->IsCategory() )
                    // On double-click, expand/collapse.
                    if ( event.ButtonDClick() && !(m_windowStyle & wxPG_HIDE_MARGIN) )
                    {
                        if ( p->IsExpanded() ) DoCollapse( p, true );
                        else DoExpand( p, true );
                    }

                // Do not Skip() the event after selection has been made.
                // Otherwise default event handling behaviour kicks in
                // and may revert focus back to the main canvas.
                res = true;
            }
            else
            {
            // click on splitter
                if ( !(m_windowStyle & wxPG_STATIC_SPLITTER) )
                {
                    if ( event.GetEventType() == wxEVT_LEFT_DCLICK )
                    {
                        // Double-clicking the splitter causes auto-centering
                        if ( m_pState->GetColumnCount() <= 2 )
                        {
                            ResetColumnSizes( true );

                            SendEvent(wxEVT_PG_COLS_RESIZED, (wxPGProperty*)NULL);
                            SendEvent(wxEVT_PG_COL_DRAGGING,
                                      m_propHover,
                                      NULL,
                                      wxPG_SEL_NOVALIDATE,
                                      0); // dragged splitter is always 0 here
                        }
                    }
                    else if ( m_dragStatus == 0 )
                    {
                        //
                        // Begin dragging the splitter
                        //

                        // send event
                        DoEndLabelEdit(true, wxPG_SEL_NOVALIDATE);

                        // Allow application to veto dragging
                        if ( !SendEvent(wxEVT_PG_COL_BEGIN_DRAG,
                                        p, NULL, 0,
                                        (unsigned int)splitterHit) )
                        {
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
                                CaptureMouse();
                                m_iFlags |= wxPG_FL_MOUSE_CAPTURED;
                            }

                            m_dragStatus = 1;
                            m_draggedSplitter = splitterHit;
                            m_dragOffset = splitterHitOffset;

                            if ( m_wndEditor2 )
                            {
                                m_wndEditor2->Hide();
                            }

                            m_startingSplitterX = x - splitterHitOffset;
                        }
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

                // Fine tune cell button x
                if ( !p->IsCategory() )
                    nx -= IN_CELL_EXPANDER_BUTTON_X_ADJUST;

                if ( (nx >= m_gutterWidth && nx < (m_gutterWidth+m_iconWidth)) )
                {
                    int y2 = y % m_lineHeight;
                    if ( (y2 >= m_buttonSpacingY && y2 < (m_buttonSpacingY+m_iconHeight)) )
                    {
                        // On click on expander button, expand/collapse
                        if ( p->IsExpanded() )
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

bool wxPropertyGrid::HandleMouseRightClick( int WXUNUSED(x),
                                            unsigned int WXUNUSED(y),
                                            wxMouseEvent& event )
{
    if ( m_propHover )
    {
        // Select property here as well
        wxPGProperty* p = m_propHover;
        AddToSelectionFromInputEvent(p, m_colHover, &event);

        // Send right click event.
        SendEvent( wxEVT_PG_RIGHT_CLICK, p );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::HandleMouseDoubleClick( int WXUNUSED(x),
                                             unsigned int WXUNUSED(y),
                                             wxMouseEvent& event )
{
    if ( m_propHover )
    {
        // Select property here as well
        AddToSelectionFromInputEvent(m_propHover, m_colHover, &event);

        // Send double-click event.
        SendEvent( wxEVT_PG_DOUBLE_CLICK, m_propHover );

        return true;
    }
    return false;
}

// -----------------------------------------------------------------------

// Return false if should be skipped
bool wxPropertyGrid::HandleMouseMove( int x, unsigned int y,
                                      wxMouseEvent &event )
{
    // Safety check (needed because mouse capturing may
    // otherwise freeze the control)
    if ( m_dragStatus > 0 && !event.Dragging() )
    {
        HandleMouseUp(x, y, event);
    }

    wxPropertyGridPageState* state = m_pState;
    int splitterHit;
    int splitterHitOffset;
    int columnHit = state->HitTestH( x, &splitterHit, &splitterHitOffset );

    #if wxUSE_TOOLTIPS
        wxPGProperty* prevHover = m_propHover;
        int prevCol = m_colHover;
    #endif
    m_colHover = columnHit;

    if ( m_dragStatus > 0 )
    {
        if ( x > (m_marginWidth + wxPG_DRAG_MARGIN) &&
             x < (m_pState->GetVirtualWidth() - wxPG_DRAG_MARGIN) )
        {

            int splitterX = x - splitterHitOffset;
            int newSplitterX = x - m_dragOffset;

            // Splitter redraw required?
            if ( newSplitterX != splitterX )
            {
                // Move everything
                DoSetSplitterPosition(newSplitterX,
                                      m_draggedSplitter,
                                      wxPG_SPLITTER_REFRESH |
                                      wxPG_SPLITTER_FROM_EVENT);

                SendEvent(wxEVT_PG_COLS_RESIZED, (wxPGProperty*)NULL);
                SendEvent(wxEVT_PG_COL_DRAGGING,
                          m_propHover,
                          NULL,
                          wxPG_SEL_NOVALIDATE,
                          (unsigned int)m_draggedSplitter);
            }

            m_dragStatus = 2;
        }

        return false;
    }
    else
    {

        int ih = m_lineHeight;
        int sy = y;

        int curPropHoverY = y - (y % ih);

        // On which item it hovers
        if ( !m_propHover
             ||
             ( sy < m_propHoverY || sy >= (m_propHoverY+ih) )
           )
        {
            // Mouse moves on another property

            m_propHover = DoGetItemAtY(y);
            m_propHoverY = curPropHoverY;

            // Send hover event
            SendEvent( wxEVT_PG_HIGHLIGHTED, m_propHover );
        }

#if wxUSE_TOOLTIPS
        //
        // If tooltips are enabled, show label or value as a tip
        // in case it doesn't otherwise show in full length.
        //
        if ( m_windowStyle & wxPG_TOOLTIPS )
        {
            if ( m_propHover != prevHover || prevCol != m_colHover )
            {
                if ( m_propHover && !m_propHover->IsCategory() )
                {

                    if ( HasExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS) )
                    {
                        // Show help string as a tooltip
                        wxString tipString = m_propHover->GetHelpString();

                        SetToolTip(tipString);
                    }
                    else if ( m_colHover >= 0 && m_colHover < (int)m_pState->GetColumnCount())
                    {
                        // Show cropped value string as a tooltip
                        wxString tipString;
                        wxPGCell cell;
                        int item = ( m_colHover == 1 ? m_propHover->GetChoiceSelection() : -1 );
                        m_propHover->GetDisplayInfo(m_colHover, item, 0, &tipString, &cell);
                        int space = m_pState->GetColumnWidth(m_colHover);

                        int imageWidth = 0;
                        const wxBitmap& bmp = cell.GetBitmap();
                        if ( bmp.IsOk() )
                        {
                            imageWidth = bmp.GetWidth();
                            int hMax = m_lineHeight - wxPG_CUSTOM_IMAGE_SPACINGY - 1;
                            if ( bmp.GetHeight() > hMax )
                                imageWidth *= (double)hMax / bmp.GetHeight();
                        }

                        if ( m_colHover == 0 )
                        {
                            if ( !(m_windowStyle & wxPG_HIDE_CATEGORIES) || m_propHover->GetParent() != m_pState->DoGetRoot() )
                                space -= (m_propHover->GetDepth()-1)*m_subgroup_extramargin;
                        }
                        else if ( m_colHover == 1 && !m_propHover->IsValueUnspecified())
                        {
                            wxSize imageSize = GetImageSize(m_propHover, -1);
                            if ( imageSize.x > 0 )
                                imageWidth = imageSize.x;
                            tipString = m_propHover->GetValueAsString();
                            if ( GetColumnCount() <= 2 )
                            {
                                wxString unitsString = m_propHover->GetAttribute(wxPG_ATTR_UNITS, wxEmptyString);
                                if ( !unitsString.empty() )
                                    tipString = wxString::Format(wxS("%s %s"), tipString, unitsString );
                            }
                        }

                        space -= m_propHover->GetImageOffset(imageWidth);
                        space -= (wxPG_XBEFORETEXT + 1);
                        int tw, th;
                        const wxFont* font = NULL;
                        if ( (m_windowStyle & wxPG_BOLD_MODIFIED) && m_propHover->HasFlag(wxPG_PROP_MODIFIED) )
                            font = &m_captionFont;
                        if ( cell.GetFont().IsOk() )
                            font = &cell.GetFont();
                        GetTextExtent( tipString, &tw, &th, 0, 0, font );
                        if ( tw > space )
                            SetToolTip( tipString );
                    }
                    else
                    {
                        SetToolTip(wxEmptyString);
                    }
                }
                else
                {
                    SetToolTip(wxEmptyString);
                }
            }
        }
#endif // wxUSE_TOOLTIPS

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
            // (also not if we were dragging and it's started
            // outside the splitter region)

            if ( !m_propHover->IsCategory() &&
                 !event.Dragging() )
            {

                // hovering on splitter

                // NB: Condition disabled since MouseLeave event (from the
                //     editor control) cannot be reliably detected.
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

        //
        // Multi select by dragging
        //
        if ( HasExtraStyle(wxPG_EX_MULTIPLE_SELECTION) &&
             event.LeftIsDown() &&
             m_propHover &&
             GetSelection() &&
             columnHit != 1 &&
             !state->DoIsPropertySelected(m_propHover) )
        {
            // Additional requirement is that the hovered property
            // is adjacent to edges of selection.
            const wxArrayPGProperty& selection = GetSelectedProperties();

            // Since categories cannot be selected along with 'other'
            // properties, exclude them from iterator flags.
            int iterFlags = wxPG_ITERATE_VISIBLE & (~wxPG_PROP_CATEGORY);

            for ( int i=(selection.size()-1); i>=0; i-- )
            {
                // TODO: This could be optimized by keeping track of
                //       which properties are at the edges of selection.
                wxPGProperty* selProp = selection[i];
                if ( state->ArePropertiesAdjacent(m_propHover, selProp,
                                                  iterFlags) )
                {
                    DoAddToSelection(m_propHover);
                    break;
                }
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

    // No event type check - basically calling this method should
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

        SendEvent(wxEVT_PG_COL_END_DRAG,
                  m_propHover,
                  NULL,
                  wxPG_SEL_NOVALIDATE,
                  (unsigned int)m_draggedSplitter);

        // Disable splitter auto-centering (but only if moved any -
        // otherwise we end up disabling auto-center even after a
        // recentering double-click).
        int posDiff = abs(m_startingSplitterX -
                          GetSplitterPosition(m_draggedSplitter));

        if ( posDiff > 1 )
            state->m_dontCenterSplitter = true;

        // This is necessary to return cursor
        if ( m_iFlags & wxPG_FL_MOUSE_CAPTURED )
        {
            ReleaseMouse();
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
        wxPGProperty* selected = GetSelection();
        if ( !(m_iFlags & wxPG_FL_PRIMARY_FILLS_ENTIRE) && selected )
            DrawItem( selected );

        if ( m_wndEditor )
        {
            m_wndEditor->Show ( true );
        }

        if ( m_wndEditor2 )
        {
            m_wndEditor2->Show(true);
        }

        // This clears the focus.
        m_editorFocused = false;

    }
    return res;
}

// -----------------------------------------------------------------------

bool wxPropertyGrid::OnMouseCommon( wxMouseEvent& event, int* px, int* py )
{
    int splitterX = GetSplitterPosition();

    int ux, uy;
    CalcUnscrolledPosition( event.m_x, event.m_y, &ux, &uy );

    wxWindow* wnd = GetEditorControl();

    // Hide popup on clicks
    if ( event.GetEventType() != wxEVT_MOTION )
        if ( wxDynamicCast(wnd, wxOwnerDrawnComboBox) )
        {
            ((wxOwnerDrawnComboBox*)wnd)->HidePopup();
        }

    wxRect r;
    if ( wnd )
        r = wnd->GetRect();
    if ( wnd == NULL || m_dragStatus ||
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
        if ( !HandleMouseClick(x, y, event) )
            event.Skip();
    }
    else
    {
        event.Skip();
    }
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

    // Do not Skip() event here - OnMouseClick() call above
    // should have already taken care of it.
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

void wxPropertyGrid::OnMouseUp( wxMouseEvent &event )
{
    int x, y;
    if ( OnMouseCommon( event, &x, &y ) )
    {
        if ( !HandleMouseUp(x, y, event) )
            event.Skip();
    }
    else
    {
        event.Skip();
    }
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
        SetCursor( wxNullCursor );

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
    int ux, uy;
    CalcUnscrolledPosition(r.x + x, r.y + y, &ux, &uy);

    if ( !m_dragStatus &&
         ux > (splitterX + wxPG_SPLITTERX_DETECTMARGIN2) &&
         y >= 0 && y < r.height
       )
    {
        if ( m_curcursor != wxCURSOR_ARROW ) CustomSetCursor ( wxCURSOR_ARROW );
        event.Skip();
    }
    else
    {
        if ( px )
        {
            *px = ux;
        }
        if ( py )
        {
            *py = uy;
        }
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

    // FIXME: Used to set m_propHover to selection here. Was it really
    //        necessary?

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
                      wxS("You can only add up to two separate actions per key combination.") );

        action = it->second | (action<<16);
    }

    m_actionTriggers[hashMapKey] = action;
}

void wxPropertyGrid::ClearActionTriggers( int action )
{
    wxPGHashMapI2I::iterator it;
    bool didSomething;

    do
    {
        didSomething = false;

        for ( it = m_actionTriggers.begin();
              it != m_actionTriggers.end();
              ++it )
        {
            if ( it->second == action )
            {
                m_actionTriggers.erase(it);
                didSomething = true;
                break;
            }
        }
    }
    while ( didSomething );
}

void wxPropertyGrid::HandleKeyEvent( wxKeyEvent &event, bool fromChild )
{
    //
    // Handles key event when editor control is not focused.
    //

    wxCHECK2(!IsFrozen(), return);

    // Traversal between items, collapsing/expanding, etc.
    wxPGProperty* selected = GetSelection();
    int keycode = event.GetKeyCode();
    bool editorFocused = IsEditorFocused();

    if ( keycode == WXK_TAB )
    {
#if defined(__WXGTK__)
        wxWindow* mainControl;

        if ( HasInternalFlag(wxPG_FL_IN_MANAGER) )
            mainControl = GetParent();
        else
            mainControl = this;
#endif

        if ( !event.ShiftDown() )
        {
            if ( !editorFocused && m_wndEditor )
            {
                DoSelectProperty( selected, wxPG_SEL_FOCUS );
            }
            else
            {
                // Tab traversal workaround for platforms on which
                // wxWindow::Navigate() may navigate into first child
                // instead of next sibling. Does not work perfectly
                // in every scenario (for instance, when property grid
                // is either first or last control).
            #if defined(__WXGTK__)
                wxWindow* sibling = mainControl->GetNextSibling();
                if ( sibling )
                    sibling->SetFocusFromKbd();
            #else
                Navigate(wxNavigationKeyEvent::IsForward);
            #endif
            }
        }
        else
        {
            if ( editorFocused )
            {
                UnfocusEditor();
            }
            else
            {
            #if defined(__WXGTK__)
                wxWindow* sibling = mainControl->GetPrevSibling();
                if ( sibling )
                    sibling->SetFocusFromKbd();
            #else
                Navigate(wxNavigationKeyEvent::IsBackward);
            #endif
            }
        }

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

    if ( editorFocused && action == wxPG_ACTION_CANCEL_EDIT )
    {
        //
        // Esc cancels any changes
        if ( IsEditorsValueModified() )
        {
            EditorsValueWasNotModified();

            // Update the control as well
            selected->GetEditorClass()->
                SetControlStringValue( selected,
                                       GetEditorControl(),
                                       selected->GetDisplayedString() );
        }

        OnValidationFailureReset(selected);

        UnfocusEditor();
        return;
    }

    // Except for TAB, ESC, and any keys specifically dedicated to
    // wxPropertyGrid itself, handle child control events in child control.
    if ( fromChild &&
#if WXWIN_COMPATIBILITY_3_0
         // Deprecated: use a hash set instead.
         !wxPGItemExistsInVector<int>(m_dedicatedKeys, keycode) )
#else
         m_dedicatedKeys.find(keycode) == m_dedicatedKeys.end() )
#endif
    {
        // Only propagate event if it had modifiers
        if ( !event.HasModifiers() )
        {
            event.StopPropagation();
        }
        event.Skip();
        return;
    }

    bool wasHandled = false;

    if ( selected )
    {
        // Show dialog?
        if ( ButtonTriggerKeyTest(action, event) )
            return;

        wxPGProperty* p = selected;

        if ( action == wxPG_ACTION_EDIT && !editorFocused )
        {
            DoSelectProperty( p, wxPG_SEL_FOCUS );
            wasHandled = true;
        }

        // Travel and expand/collapse
        int selectDir = -2;

        if ( p->GetChildCount() )
        {
            if ( action == wxPG_ACTION_COLLAPSE_PROPERTY || secondAction == wxPG_ACTION_COLLAPSE_PROPERTY )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || DoCollapse(p, true) )
                    wasHandled = true;
            }
            else if ( action == wxPG_ACTION_EXPAND_PROPERTY || secondAction == wxPG_ACTION_EXPAND_PROPERTY )
            {
                if ( (m_windowStyle & wxPG_HIDE_MARGIN) || DoExpand(p, true) )
                    wasHandled = true;
            }
        }

        if ( !wasHandled )
        {
            if ( action == wxPG_ACTION_PREV_PROPERTY || secondAction == wxPG_ACTION_PREV_PROPERTY )
            {
                selectDir = -1;
            }
            else if ( action == wxPG_ACTION_NEXT_PROPERTY || secondAction == wxPG_ACTION_NEXT_PROPERTY )
            {
                selectDir = 1;
            }
        }

        if ( selectDir >= -1 )
        {
            p = wxPropertyGridIterator::OneStep( m_pState, wxPG_ITERATE_VISIBLE, p, selectDir );
            if ( p )
            {
                int selFlags = 0;
                int reopenLabelEditorCol = -1;

                if ( editorFocused )
                {
                    // If editor was focused, then make the next editor
                    // focused as well
                    selFlags |= wxPG_SEL_FOCUS;
                }
                else
                {
                    // Also maintain the same label editor focus state
                    if ( m_labelEditor )
                        reopenLabelEditorCol = m_selColumn;
                }

                DoSelectProperty(p, selFlags);

                if ( reopenLabelEditorCol >= 0 )
                    DoBeginLabelEdit(reopenLabelEditorCol);
            }
            wasHandled = true;
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
            wasHandled = true;
        }
    }

    if ( !wasHandled )
        event.Skip();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnKey( wxKeyEvent &event )
{
    // If there was editor open and focused, then this event should not
    // really be processed here.
    if ( IsEditorFocused() )
    {
        // However, if event had modifiers, it is probably still best
        // to skip it.
        if ( event.HasModifiers() )
            event.Skip();
        else
            event.StopPropagation();
        return;
    }

    HandleKeyEvent(event, false);
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
        wxCommandEvent evt(wxEVT_BUTTON, m_wndEditor2->GetId());
        GetEventHandler()->AddPendingEvent(evt);
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::OnChildKeyDown( wxKeyEvent &event )
{
    HandleKeyEvent(event, true);
}

// -----------------------------------------------------------------------
// wxPropertyGrid miscellaneous event handling
// -----------------------------------------------------------------------

void wxPropertyGrid::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    // Skip fake idle events generated e.g. by calling
    // wxYield from within event handler.
    if ( m_processedEvent )
        return;

    //
    // Check if the focus is in this control or one of its children
    wxWindow* newFocused = wxWindow::FindFocus();

    if ( newFocused != m_curFocused )
        HandleFocusChange( newFocused );

    //
    // Check if top-level parent has changed
    if ( HasExtraStyle(wxPG_EX_ENABLE_TLP_TRACKING) )
    {
        wxWindow* tlp = ::wxGetTopLevelParent(this);
        if ( tlp != m_tlp )
            OnTLPChanging(tlp);
    }

    // Delete pending property editors and their event handlers.
    DeletePendingObjects();

    //
    // Resolve pending property removals
    // In order to determine whether deletion/removal
    // was done we need to track the size of the list
    // before and after the operation.
    // (Note that lists are changed at every operation.)
    size_t cntAfter = m_deletedProperties.size();
    while ( cntAfter > 0 )
    {
        size_t cntBefore = cntAfter;

        DeleteProperty(m_deletedProperties[0]);

        cntAfter = m_deletedProperties.size();
        wxASSERT_MSG( cntAfter <= cntBefore,
            wxS("Increased number of pending items after deletion") );
        // Break if deletion was not done
        if ( cntAfter >= cntBefore )
            break;
    }
    cntAfter = m_removedProperties.size();
    while ( cntAfter > 0 )
    {
        size_t cntBefore = cntAfter;

        RemoveProperty(m_removedProperties[0]);

        cntAfter = m_removedProperties.size();
        wxASSERT_MSG( cntAfter <= cntBefore,
            wxS("Increased number of pending items after removal") );
        // Break if removal was not done
        if ( cntAfter >= cntBefore )
            break;
    }
}

bool wxPropertyGrid::IsEditorFocused() const
{
    wxWindow* focus = wxWindow::FindFocus();

    if ( focus == m_wndEditor || focus == m_wndEditor2 ||
         focus == GetEditorControl() ||
         // In case a combobox text control is focused
         (focus && focus->GetParent() && (focus->GetParent() == m_wndEditor)) )
         return true;

    return false;
}

// Called by focus event handlers. newFocused is the window that becomes focused.
void wxPropertyGrid::HandleFocusChange( wxWindow* newFocused )
{
    //
    // Never allow focus to be changed when handling editor event.
    // Especially because they may be displaying a dialog which
    // could cause all kinds of weird (native) focus changes.
    if ( HasInternalFlag(wxPG_FL_IN_HANDLECUSTOMEDITOREVENT) )
        return;

    unsigned int oldFlags = m_iFlags;
    bool wasEditorFocused = false;
    wxWindow* wndEditor = m_wndEditor;

    m_iFlags &= ~(wxPG_FL_FOCUSED);

    wxWindow* parent = newFocused;

    // This must be one of nextFocus' parents.
    while ( parent )
    {
        if ( parent == wndEditor )
        {
            // If editor is active consider focus set on its components
            // as a focus set on the editor itself (to prevent doing actions
            // when focus is switched between subcontrols of a compound
            // editor like e.g. wxComboCtrl).
            newFocused = wndEditor;
            wasEditorFocused = true;
        }
        // Use m_eventObject, which is either wxPropertyGrid or
        // wxPropertyGridManager, as appropriate.
        else if ( parent == m_eventObject )
        {
            m_iFlags |= wxPG_FL_FOCUSED;
            break;
        }
        parent = parent->GetParent();
    }

    // Notify editor control when it receives a focus
    if ( wasEditorFocused && m_curFocused != newFocused )
    {
        wxPGProperty* p = GetSelection();
        if ( p )
        {
            const wxPGEditor* editor = p->GetEditorClass();
            ResetEditorAppearance();
            editor->OnFocus(p, GetEditorControl());
        }
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
        wxPGProperty* selected = GetSelection();
        if ( selected && (m_iFlags & wxPG_FL_INITIALIZED) )
            DrawItem( selected );
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
    event.Skip();
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
wxPGEditor* wxPropertyGrid::DoRegisterEditorClass( wxPGEditor* editorClass,
                                                   const wxString& editorName,
                                                   bool noDefCheck )
{
    wxASSERT( editorClass );

    if ( !noDefCheck && wxPGGlobalVars->m_mapEditorClasses.empty() )
        RegisterDefaultEditors();

    wxString name = editorName;
    if ( name.empty() )
        name = editorClass->GetName();

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
                 wxS("Editor with given name was already registered") );

    wxPGGlobalVars->m_mapEditorClasses[name] = (void*)editorClass;

    return editorClass;
}

// Use this in RegisterDefaultEditors.
#define wxPGRegisterDefaultEditorClass(EDITOR) \
    if ( wxPGEditor_##EDITOR == NULL ) \
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

wxPGStringTokenizer::wxPGStringTokenizer( const wxString& str, wxChar delimiter )
    : m_str(&str), m_curPos(str.begin()), m_delimiter(delimiter)
{
}

wxPGStringTokenizer::~wxPGStringTokenizer()
{
}

bool wxPGStringTokenizer::HasMoreTokens()
{
    const wxString& str = *m_str;

    wxString::const_iterator i = m_curPos;

    wxUniChar delim = m_delimiter;
    wxUniChar prev_a = wxS('\0');

    bool inToken = false;

    while ( i != str.end() )
    {
        wxUniChar a = *i;

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
            if ( prev_a != wxS('\\') )
            {
                if ( a != delim )
                {
                    if ( a != wxS('\\') )
                        m_readyToken << a;
                }
                else
                {
                    ++i;
                    m_curPos = i;
                    return true;
                }
                prev_a = a;
            }
            else
            {
                m_readyToken << a;
                prev_a = wxS('\0');
            }
        }
        ++i;
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

// -----------------------------------------------------------------------
// wxPGChoicesData
// -----------------------------------------------------------------------

wxPGChoicesData::wxPGChoicesData()
{
}

wxPGChoicesData::~wxPGChoicesData()
{
    Clear();
}

void wxPGChoicesData::Clear()
{
    m_items.clear();
}

void wxPGChoicesData::CopyDataFrom( wxPGChoicesData* data )
{
    wxASSERT( m_items.empty() );

    m_items = data->m_items;
}

wxPGChoiceEntry& wxPGChoicesData::Insert( int index,
                                          const wxPGChoiceEntry& item )
{
    wxVector<wxPGChoiceEntry>::iterator it;
    if ( index == -1 )
    {
        it = m_items.end();
        index = (int) m_items.size();
    }
    else
    {
        it = m_items.begin() + index;
    }

    m_items.insert(it, item);

    wxPGChoiceEntry& ownEntry = m_items[index];

    // Need to fix value?
    if ( ownEntry.GetValue() == wxPG_INVALID_VALUE )
        ownEntry.SetValue(index);

    return ownEntry;
}

// -----------------------------------------------------------------------
// wxPropertyGridEvent
// -----------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxPropertyGridEvent, wxCommandEvent);


wxDEFINE_EVENT( wxEVT_PG_SELECTED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_CHANGING, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_CHANGED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_HIGHLIGHTED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_RIGHT_CLICK, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_PAGE_CHANGED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_ITEM_EXPANDED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_ITEM_COLLAPSED, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_DOUBLE_CLICK, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_LABEL_EDIT_BEGIN, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_LABEL_EDIT_ENDING, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_COL_BEGIN_DRAG, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_COL_DRAGGING, wxPropertyGridEvent );
wxDEFINE_EVENT( wxEVT_PG_COL_END_DRAG, wxPropertyGridEvent );
// Events used only internally
wxDEFINE_EVENT( wxEVT_PG_HSCROLL, wxPropertyGridEvent);
wxDEFINE_EVENT( wxEVT_PG_COLS_RESIZED, wxPropertyGridEvent);

// -----------------------------------------------------------------------

void wxPropertyGridEvent::Init()
{
    m_validationInfo = NULL;
    m_column = 1;
    m_canVeto = false;
    m_wasVetoed = false;
    m_pg = NULL;
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
    OnPropertyGridSet();
    m_property = event.m_property;
    m_validationInfo = event.m_validationInfo;
    m_canVeto = event.m_canVeto;
    m_wasVetoed = event.m_wasVetoed;
}

// -----------------------------------------------------------------------

void wxPropertyGridEvent::OnPropertyGridSet()
{
    if ( !m_pg )
        return;

#if wxUSE_THREADS
    wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
#endif
    m_pg->m_liveEvents.push_back(this);
}

// -----------------------------------------------------------------------

wxPropertyGridEvent::~wxPropertyGridEvent()
{
    if ( m_pg )
    {
    #if wxUSE_THREADS
        wxCriticalSectionLocker lock(wxPGGlobalVars->m_critSect);
    #endif

        // Use iterate from the back since it is more likely that the event
        // being destroyed is at the end of the array.
        wxVector<wxPropertyGridEvent*>& liveEvents = m_pg->m_liveEvents;

        for ( int i = liveEvents.size()-1; i >= 0; i-- )
        {
            if ( liveEvents[i] == this )
            {
                liveEvents.erase(liveEvents.begin() + i);
                break;
            }
        }
    }
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
        ProcessError(wxString::Format(wxS("new children cannot be added to '%s'"),parent->GetName()));
        return NULL;
    }

    if ( !classInfo || !classInfo->IsKindOf(wxCLASSINFO(wxPGProperty)) )
    {
        ProcessError(wxString::Format(wxS("'%s' is not valid property class"),propClass));
        return NULL;
    }

    wxPGProperty* property = (wxPGProperty*) classInfo->CreateObject();

    property->SetLabel(propLabel);
    property->DoSetName(propName);

    if ( pChoices && pChoices->IsOk() )
        property->SetChoices(*pChoices);

    m_state->DoInsert(parent, -1, property);

    if ( propValue )
        property->SetValueFromString( *propValue, wxPG_FULL_VALUE|
                                                  wxPG_PROGRAMMATIC_VALUE );

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
            ProcessError(wxString::Format(wxS("No choices defined for id '%s'"),ids));
        else
            choices.AssignData((wxPGChoicesData*)it->second);
    }
    else
    {
        bool found = false;
        if ( !idString.empty() )
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
            wxString::const_iterator it;
            wxString label;
            wxString value;
            int state = 0;
            bool labelValid = false;

            for ( it = choicesString.begin(); it != choicesString.end(); ++it )
            {
                wxUniChar c = *it;

                if ( state != 1 )
                {
                    if ( c == wxS('"') )
                    {
                        if ( labelValid )
                        {
                            long l;
                            if ( !value.ToLong(&l, 0) ) l = wxPG_INVALID_VALUE;
                            choices.Add(label, l);
                        }
                        labelValid = false;
                        //wxLogDebug(wxS("%s, %s"),label,value);
                        value.clear();
                        label.clear();
                        state = 1;
                    }
                    else if ( c == wxS('=') )
                    {
                        if ( labelValid )
                        {
                            state = 2;
                        }
                    }
                    else if ( state == 2 && (wxIsalnum(c) || c == wxS('x')) )
                    {
                        value << c;
                    }
                }
                else
                {
                    if ( c == wxS('"') )
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
            if ( !idString.empty() )
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
    if ( m_propHierarchy.empty() )
        return false;

    wxPGProperty* p = m_propHierarchy.back();
    wxString valuel = value.Lower();
    wxVariant variant;

    if ( type.empty() )
    {
        long v;

        // Auto-detect type
        if ( valuel == wxS("true") || valuel == wxS("yes") || valuel == wxS("1") )
            variant = true;
        else if ( valuel == wxS("false") || valuel == wxS("no") || valuel == wxS("0") )
            variant = false;
        else if ( value.ToLong(&v, 0) )
            variant = v;
        else
            variant = value;
    }
    else
    {
        if ( type == wxS("string") )
        {
            variant = value;
        }
        else if ( type == wxS("int") )
        {
            long v = 0;
            value.ToLong(&v, 0);
            variant = v;
        }
        else if ( type == wxS("bool") )
        {
            if ( valuel == wxS("true") || valuel == wxS("yes") || valuel == wxS("1") )
                variant = true;
            else
                variant = false;
        }
        else
        {
            ProcessError(wxString::Format(wxS("Invalid attribute type '%s'"),type));
            return false;
        }
    }

    p->SetAttribute( name, variant );

    return true;
}

// -----------------------------------------------------------------------

void wxPropertyGridPopulator::ProcessError( const wxString& msg )
{
    wxLogError(_("Error in resource: %s"),msg);
}

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID
