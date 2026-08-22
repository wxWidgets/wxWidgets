/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/editors.cpp
// Purpose:     wxPropertyGrid editors
// Author:      Jaakko Salli
// Created:     2007-04-14
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#if wxUSE_PROPGRID

#ifndef WX_PRECOMP
    #include "wx/settings.h"
    #include "wx/textctrl.h"
#endif

#include "wx/dcbuffer.h"
#include "wx/odcombo.h"

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/props.h"
#include "wx/propgrid/private.h"
#include "wx/private/windowlifetime.h"
#include "wx/scopeguard.h"
#include "wx/weakref.h"

#if wxPG_USE_RENDERER_NATIVE
    #include "wx/renderer.h"
#endif

// How many pixels between textctrl and button
#ifdef __WXMAC__
    #define wxPG_TEXTCTRL_AND_BUTTON_SPACING        4
#else
    #define wxPG_TEXTCTRL_AND_BUTTON_SPACING        2
#endif

#define wxPG_BUTTON_SIZEDEC                         0

// -----------------------------------------------------------------------

#if defined(__WXMSW__)
    // tested
    #define wxPG_NAT_BUTTON_BORDER_Y            1

    #define wxPG_CHECKMARK_XADJ                 1
    #define wxPG_CHECKMARK_YADJ                 (-1)
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

    #define wxPG_TEXTCTRLYADJUST                (m_spacingy+0)

#elif defined(__WXGTK__)
    // tested
    #define wxPG_CHECKMARK_XADJ                 1
    #define wxPG_CHECKMARK_YADJ                 1
    #define wxPG_CHECKMARK_WADJ                 (-2)
    #define wxPG_CHECKMARK_HADJ                 (-2)
    #define wxPG_CHECKMARK_DEFLATE              3

    #define wxPG_NAT_BUTTON_BORDER_Y        1

    #define wxPG_TEXTCTRLYADJUST            0

#elif defined(__WXMAC__)
    // partially tested
    #define wxPG_CHECKMARK_XADJ                 4
    #define wxPG_CHECKMARK_YADJ                 4
    #define wxPG_CHECKMARK_WADJ                 -6
    #define wxPG_CHECKMARK_HADJ                 -6
    #define wxPG_CHECKMARK_DEFLATE              0

    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLYADJUST            2

#else
    // defaults
    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLYADJUST            0

#endif

#define wxPG_BUTTON_BORDER_WIDTH (-wxPG_BUTTON_SIZEDEC + wxPG_NAT_BUTTON_BORDER_Y)

// for odcombo
#ifdef __WXMAC__
#define wxPG_CHOICEXADJUST           -3 // required because wxComboCtrl reserves 3pixels for wxTextCtrl's focus ring
#define wxPG_CHOICEYADJUST           -3
#else
#define wxPG_CHOICEXADJUST           0
#define wxPG_CHOICEYADJUST           0
#endif

// Number added to image width for SetCustomPaintWidth
#define ODCB_CUST_PAINT_MARGIN               6

// Milliseconds to wait for two mouse-ups after focus in order
// to trigger a double-click.
#define DOUBLE_CLICK_CONVERSION_TRESHOLD        500

namespace
{

// Guard the result of application-overridable property callbacks before an
// editor implementation touches either the grid or its control again. The
// surrounding PropertyGrid operation keeps the property itself deferred; this
// object additionally proves that the grid, page, selection and control still
// have the identities captured at callback entry.
class wxPGEditorCallbackTransaction final
{
public:
    wxPGEditorCallbackTransaction(wxPropertyGrid* grid,
                                  wxPGProperty* property,
                                  wxWindow* control)
        : m_grid(grid),
          m_property(property),
          m_control(control),
          m_weakGrid(grid),
          m_weakControl(control),
          m_state(grid ? grid->GetState() : nullptr)
    {
    }

    bool IsValid() const
    {
        return m_grid &&
               wxWeakWindowIsAvailableForCallbacks(m_weakGrid, m_grid) &&
               m_state &&
               m_grid->GetState() == m_state &&
               m_grid->GetSelection() == m_property &&
               !m_grid->IsPropertyPendingRemoval(m_property) &&
               (!m_control ||
                wxWeakWindowIsAvailableForCallbacks(m_weakControl,
                                                    m_control));
    }

private:
    wxPropertyGrid* const m_grid;
    wxPGProperty* const m_property;
    wxWindow* const m_control;
    wxWeakRef<wxWindow> m_weakGrid;
    wxWeakRef<wxWindow> m_weakControl;
    wxPropertyGridPageState* const m_state;
};

// A successfully created control must be returned when a callback invalidates
// the selection so DoSelectProperty() can adopt it as an abandoned editor. A
// genuine native Create() failure in an otherwise intact transaction has no
// such owner, so dispose of that wrapper here instead of returning an invalid
// editor or leaving an untracked child behind.
wxWindow* wxPGResolveFailedEditorCreation(
    wxWindow* editor,
    const wxWeakRef<wxWindow>& weakEditor,
    bool transactionIsValid)
{
    wxWindow* const liveEditor = weakEditor.get();
    if ( !liveEditor )
        return nullptr;

    if ( transactionIsValid )
    {
        wxASSERT(liveEditor == editor);
        delete liveEditor;
        return nullptr;
    }

    return liveEditor;
}

} // anonymous namespace

// -----------------------------------------------------------------------
// wxPGEditor
// -----------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPGEditor, wxObject);

wxString wxPGEditor::GetName() const
{
    return GetClassInfo()->GetClassName();
}

void wxPGEditor::DrawValue( wxDC& dc, const wxRect& rect,
                            wxPGProperty* WXUNUSED(property),
                            const wxString& text ) const
{
    dc.DrawText( text, rect.x+wxPG_XBEFORETEXT, rect.y );
}

bool wxPGEditor::GetValueFromControl( wxVariant&, wxPGProperty*, wxWindow* ) const
{
    return false;
}

void wxPGEditor::SetControlStringValue( wxPGProperty* WXUNUSED(property), wxWindow*, const wxString& ) const
{
}


void wxPGEditor::SetControlIntValue( wxPGProperty* WXUNUSED(property), wxWindow*, int ) const
{
}


int wxPGEditor::InsertItem( wxWindow*, const wxString&, int ) const
{
    return -1;
}


void wxPGEditor::DeleteItem( wxWindow*, int ) const
{
}

void wxPGEditor::SetItems(wxWindow* WXUNUSED(ctrl), const wxArrayString& WXUNUSED(labels)) const
{
}

void wxPGEditor::OnFocus( wxPGProperty*, wxWindow* ) const
{
}

void wxPGEditor::SetControlAppearance( wxPropertyGrid* pg,
                                       wxPGProperty* property,
                                       wxWindow* ctrl,
                                       const wxPGCell& cell,
                                       const wxPGCell& oCell,
                                       bool unspecified ) const
{
    // Get old editor appearance
    wxTextCtrl* tc = wxDynamicCast(ctrl, wxTextCtrl);
    wxComboCtrl* cb = nullptr;
    if ( !tc )
    {
        cb = wxDynamicCast(ctrl, wxComboCtrl);
        if ( cb )
        {
            tc = cb->GetTextCtrl();
        }
    }

    const wxWeakRef<wxWindow> weakGrid(pg);
    const wxWeakRef<wxWindow> weakCtrl(ctrl);
    const wxWeakRef<wxWindow> weakTextCtrl(tc);
    const wxWeakRef<wxWindow> weakCombo(cb);
    wxPropertyGridPageState* const state = pg->GetState();
    const auto transactionIsValid =
        [weakGrid, weakCtrl, weakTextCtrl, weakCombo, pg, property,
         ctrl, tc, cb, state]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakGrid, pg) &&
                   pg->GetState() == state &&
                   pg->GetSelection() == property &&
                   property->GetParentState() == state &&
                   !property->HasFlag(wxPGFlags::BeingDeleted) &&
                   wxWeakWindowIsAvailableForCallbacks(weakCtrl, ctrl) &&
                   pg->GetEditorControl() == ctrl &&
                   (!tc || (wxWeakWindowIsAvailableForCallbacks(weakTextCtrl, tc))) &&
                   (!cb || (wxWeakWindowIsAvailableForCallbacks(weakCombo, cb)));
        };

    if ( !transactionIsValid() )
        return;

    if ( tc || cb )
    {
        wxString tcText;
        bool changeText = false;

        if ( cell.HasText() && !pg->IsEditorFocused() )
        {
            tcText = cell.GetText();
            changeText = true;
        }
        else if ( oCell.HasText() )
        {
#if WXWIN_COMPATIBILITY_3_2
            // Special implementation with check if user-overriden obsolete function is still in use
            tcText = property->GetValueAsStringWithCheck(
#else
            tcText = property->GetValueAsString(
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
                property->HasFlag(wxPGFlags::ReadOnly)?wxPGPropValFormatFlags::Null:wxPGPropValFormatFlags::EditableValue);
            if ( !transactionIsValid() )
                return;
            changeText = true;
        }

        if ( changeText )
        {
            // This prevents value from being modified
            if ( tc )
            {
                pg->SetupTextCtrlValue(tcText);
                if ( !transactionIsValid() )
                    return;
                tc->SetValue(tcText);
                if ( !transactionIsValid() )
                    return;
            }
            else
            {
                cb->SetText(tcText);
                if ( !transactionIsValid() )
                    return;
            }
        }
    }

    // Do not make the mistake of calling GetClassDefaultAttributes()
    // here. It is static, while GetDefaultAttributes() is virtual
    // and the correct one to use.
    wxVisualAttributes vattrs = ctrl->GetDefaultAttributes();
    if ( !transactionIsValid() )
        return;

    // Foreground colour
    const wxColour& fgCol = cell.GetFgCol();
    if ( fgCol.IsOk() )
    {
        ctrl->SetForegroundColour(fgCol);
        if ( !transactionIsValid() )
            return;
    }
    else if ( oCell.GetFgCol().IsOk() )
    {
        ctrl->SetForegroundColour(vattrs.colFg);
        if ( !transactionIsValid() )
            return;
    }

    // Background colour
    const wxColour& bgCol = cell.GetBgCol();
    if ( bgCol.IsOk() )
    {
        ctrl->SetBackgroundColour(bgCol);
        if ( !transactionIsValid() )
            return;
    }
    else if ( oCell.GetBgCol().IsOk() )
    {
        ctrl->SetBackgroundColour(vattrs.colBg);
        if ( !transactionIsValid() )
            return;
    }

    // Font
    const wxFont& font = cell.GetFont();
    if ( font.IsOk() )
    {
        ctrl->SetFont(font);
        if ( !transactionIsValid() )
            return;
    }
    else if ( oCell.GetFont().IsOk() )
    {
        ctrl->SetFont(vattrs.font);
        if ( !transactionIsValid() )
            return;
    }

    // Also call the old SetValueToUnspecified()
    if ( unspecified )
    {
        if ( !transactionIsValid() )
            return;
        SetValueToUnspecified(property, ctrl);
    }
}

void wxPGEditor::SetValueToUnspecified( wxPGProperty* WXUNUSED(property),
                                        wxWindow* WXUNUSED(ctrl) ) const
{
}

bool wxPGEditor::CanContainCustomImage() const
{
    return false;
}

// -----------------------------------------------------------------------
// wxPGTextCtrlEditor
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(TextCtrl,wxPGTextCtrlEditor,wxPGEditor)


wxPGWindowList wxPGTextCtrlEditor::CreateControls( wxPropertyGrid* propGrid,
                                                   wxPGProperty* property,
                                                   const wxPoint& pos,
                                                   const wxSize& sz ) const
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakGrid(propGrid);
    wxPropertyGridPageState* const state = propGrid->GetState();
    const auto transactionIsValid =
        [weakGrid, propGrid, property, state]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) &&
                   propGrid->GetState() == state &&
                   propGrid->GetSelection() == property &&
                   !propGrid->IsPropertyPendingRemoval(property);
        };

    wxString text;

    //
    // If has children, and limited editing is specified, then don't create.
    if ( property->HasFlag(wxPGFlags::NoEditor) &&
         property->HasAnyChild() )
        return nullptr;

    wxPGPropValFormatFlags fmtFlags = wxPGPropValFormatFlags::Null;
    if ( !property->HasFlag(wxPGFlags::ReadOnly) &&
         !property->IsValueUnspecified() )
        fmtFlags |= wxPGPropValFormatFlags::EditableValue;
#if WXWIN_COMPATIBILITY_3_2
    // Special implementation with check if user-overriden obsolete function is still in use
    text = property->GetValueAsStringWithCheck(fmtFlags);
#else
    text = property->GetValueAsString(fmtFlags);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
    if ( !transactionIsValid() )
        return nullptr;

    int flags = 0;
    if ( property->HasFlag(wxPGPropertyFlags_Password) &&
         wxDynamicCast(property, wxStringProperty) )
        flags |= wxTE_PASSWORD;

    wxWindow* wnd = propGrid->GenerateEditorTextCtrl(pos,sz,text,nullptr,flags,
                                                     property->GetMaxLength());

    return wnd;
}

#if 0
void wxPGTextCtrlEditor::DrawValue( wxDC& dc, wxPGProperty* property, const wxRect& rect ) const
{
    if ( !property->IsValueUnspecified() )
    {
        wxString drawStr = property->GetDisplayedString();

        // Code below should no longer be needed, as the obfuscation
        // is now done in GetValueAsString.
        /*if ( property->HasFlag(wxPGPropertyFlags_Password) &&
             wxDynamicCast(property, wxStringProperty) )
        {
            size_t a = drawStr.length();
            drawStr.Empty();
            drawStr.Append(wxS('*'),a);
        }*/
        dc.DrawText( drawStr, rect.x+wxPG_XBEFORETEXT, rect.y );
    }
}
#endif

void wxPGTextCtrlEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxTextCtrl* tc = wxDynamicCast(ctrl, wxTextCtrl);
    if (!tc) return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxPropertyGrid* const pg = property->GetGrid();
    const wxPGEditorCallbackTransaction transaction(pg, property, tc);
    if ( !transaction.IsValid() )
        return;

    wxString s;

    if ( tc->HasFlag(wxTE_PASSWORD) )
#if WXWIN_COMPATIBILITY_3_2
        // Special implementation with check if user-overriden obsolete function is still in use
        s = property->GetValueAsStringWithCheck(wxPGPropValFormatFlags::FullValue);
#else
        s = property->GetValueAsString(wxPGPropValFormatFlags::FullValue);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
    else
        s = property->GetDisplayedString();

    if ( !transaction.IsValid() )
        return;

    pg->SetupTextCtrlValue(s);
    tc->SetValue(s);
    if ( !transaction.IsValid() )
        return;

    //
    // Fix indentation, just in case (change in font boldness is one good
    // reason).
    tc->SetMargins(0);
}

// Provided so that, for example, ComboBox editor can use the same code
// (multiple inheritance would get way too messy).
bool wxPGTextCtrlEditor::OnTextCtrlEvent( wxPropertyGrid* propGrid,
                                          wxPGProperty* WXUNUSED(property),
                                          wxWindow* ctrl,
                                          wxEvent& event )
{
    if ( !ctrl )
        return false;

    if ( event.GetEventType() == wxEVT_TEXT_ENTER )
    {
        if ( propGrid->IsEditorsValueModified() )
        {
            return true;
        }
    }
    else if ( event.GetEventType() == wxEVT_TEXT )
    {
        // Pass this event (with PG id) outside wxPropertyGrid
        // with so that, if necessary, program can tell when user
        // is editing a textctrl.
        wxEvent *evt = event.Clone();
        evt->SetId(propGrid->GetId());
        propGrid->GetEventHandler()->QueueEvent(evt);

        propGrid->EditorsValueWasModified();
    }
    return false;
}


bool wxPGTextCtrlEditor::OnEvent( wxPropertyGrid* propGrid,
                                  wxPGProperty* property,
                                  wxWindow* ctrl,
                                  wxEvent& event ) const
{
    return wxPGTextCtrlEditor::OnTextCtrlEvent(propGrid,property,ctrl,event);
}


bool wxPGTextCtrlEditor::GetTextCtrlValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl )
{
    wxTextCtrl* tc = wxStaticCast(ctrl, wxTextCtrl);
    wxString textVal = tc->GetValue();

    if ( property->UsesAutoUnspecified() && textVal.empty() )
    {
        variant.MakeNull();
        return true;
    }

#if WXWIN_COMPATIBILITY_3_2
    // Special implementation with check if user-overriden obsolete function is still in use
    bool res = property->StringToValueWithCheck(variant, textVal, wxPGPropValFormatFlags::EditableValue);
#else
    bool res = property->StringToValue(variant, textVal, wxPGPropValFormatFlags::EditableValue);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2

    // Changing unspecified always causes event (returning
    // true here should be enough to trigger it).
    // TODO: Move to propgrid.cpp
    if ( !res && variant.IsNull() )
        res = true;

    return res;
}


bool wxPGTextCtrlEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const
{
    return wxPGTextCtrlEditor::GetTextCtrlValueFromControl(variant, property, ctrl);
}


void wxPGTextCtrlEditor::SetControlStringValue( wxPGProperty* property, wxWindow* ctrl, const wxString& txt ) const
{
    wxTextCtrl* tc = wxStaticCast(ctrl, wxTextCtrl);

    wxPropertyGrid* pg = property->GetGrid();
    wxASSERT(pg);  // Really, property grid should exist if editor does
    if ( pg )
    {
        pg->SetupTextCtrlValue(txt);
        tc->SetValue(txt);
    }
}


static
void wxPGTextCtrlEditor_OnFocus( wxPGProperty* property,
                                 wxTextCtrl* tc )
{
    if ( !tc )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxPropertyGrid* const pg = property->GetGrid();
    const wxPGEditorCallbackTransaction transaction(pg, property, tc);
    if ( !transaction.IsValid() )
        return;

    // Make sure there is correct text (instead of unspecified value
    // indicator or hint text)
    wxPGPropValFormatFlags fmtFlags = property->HasFlag(wxPGFlags::ReadOnly) ?
        wxPGPropValFormatFlags::Null : wxPGPropValFormatFlags::EditableValue;
#if WXWIN_COMPATIBILITY_3_2
    // Special implementation with check if user-overriden obsolete function is still in use
    wxString correctText = property->GetValueAsStringWithCheck(fmtFlags);
#else
    wxString correctText = property->GetValueAsString(fmtFlags);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2

    if ( !transaction.IsValid() )
        return;

    if ( tc->GetValue() != correctText )
    {
        pg->SetupTextCtrlValue(correctText);
        tc->SetValue(correctText);
        if ( !transaction.IsValid() )
            return;
    }

    tc->SelectAll();
}

void wxPGTextCtrlEditor::OnFocus( wxPGProperty* property,
                                  wxWindow* wnd ) const
{
    wxTextCtrl* tc = wxStaticCast(wnd, wxTextCtrl);
    wxPGTextCtrlEditor_OnFocus(property, tc);
}

wxPGTextCtrlEditor::~wxPGTextCtrlEditor()
{
    // Reset the global pointer. Useful when wxPropertyGrid is accessed
    // from an external main loop.
    wxPG_EDITOR(TextCtrl) = nullptr;
}


// -----------------------------------------------------------------------
// wxPGChoiceEditor
// -----------------------------------------------------------------------


WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(Choice,wxPGChoiceEditor,wxPGEditor)


// This is a special enhanced double-click processor class.
// In essence, it allows for double-clicks for which the
// first click "created" the control.
class wxPGDoubleClickProcessor : public wxEvtHandler
{
public:

    wxPGDoubleClickProcessor( wxPropertyGrid* grid,
                              wxOwnerDrawnComboBox* combo,
                              wxBoolProperty* property )
        : wxEvtHandler()
        , m_timeLastMouseUp(0)
        , m_combo(combo)
        , m_grid(grid)
        , m_property(property)
        , m_downReceived(false)
    {
    }

protected:

    void OnMouseEvent( wxMouseEvent& event )
    {
        wxPropertyGrid* const grid =
            wxDynamicCast(m_grid.get(), wxPropertyGrid);
        wxOwnerDrawnComboBox* const combo =
            wxDynamicCast(m_combo.get(), wxOwnerDrawnComboBox);

        // On non-MSW ports editor destruction is intentionally delayed and a
        // queued mouse event can outlive the selected property. Validate the
        // weak owner and selection before dereferencing the raw property
        // identity retained for double-click cycling.
        if ( !grid || !combo || !m_property || wxWindowIsUnavailableForCallbacks(grid) ||
             grid->GetSelection() != m_property ||
             grid->GetEditorControl() != combo ||
             grid->IsPropertyPendingRemoval(m_property) )
        {
            event.Skip();
            return;
        }

        wxMilliClock_t t = ::wxGetLocalTimeMillis();
        wxEventType evtType = event.GetEventType();

        if ( m_property->HasFlag(wxPGPropertyFlags_UseDCC) &&
             !combo->IsPopupShown() )
        {
            // Just check that it is in the text area
            wxPoint pt = event.GetPosition();
            if ( combo->GetTextRect().Contains(pt) )
            {
                if ( evtType == wxEVT_LEFT_DOWN )
                {
                    // Set value to avoid up-events without corresponding downs
                    m_downReceived = true;
                }
                else if ( evtType == wxEVT_LEFT_DCLICK )
                {
                    // We'll make our own double-clicks
                    event.SetEventType(0);
                    return;
                }
                else if ( evtType == wxEVT_LEFT_UP )
                {
                    if ( m_downReceived || m_timeLastMouseUp == 1 )
                    {
                        wxMilliClock_t timeFromLastUp = (t-m_timeLastMouseUp);

                        if ( timeFromLastUp < DOUBLE_CLICK_CONVERSION_TRESHOLD )
                        {
                            event.SetEventType(wxEVT_LEFT_DCLICK);
                            m_timeLastMouseUp = 1;
                        }
                        else
                        {
                            m_timeLastMouseUp = t;
                        }
                    }
                }
            }
        }

        event.Skip();
    }

    void OnSetFocus( wxFocusEvent& event )
    {
        m_timeLastMouseUp = ::wxGetLocalTimeMillis();
        event.Skip();
    }

private:
    wxMilliClock_t              m_timeLastMouseUp;
    wxWindowRef                 m_combo;
    wxWindowRef                 m_grid;
    wxBoolProperty*             m_property;  // Selected property
    bool                        m_downReceived;

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(wxPGDoubleClickProcessor, wxEvtHandler)
    EVT_MOUSE_EVENTS(wxPGDoubleClickProcessor::OnMouseEvent)
    EVT_SET_FOCUS(wxPGDoubleClickProcessor::OnSetFocus)
wxEND_EVENT_TABLE()



class wxPGComboBox : public wxOwnerDrawnComboBox
{
public:

    wxPGComboBox()
        : wxOwnerDrawnComboBox()
        , m_dclickProcessor(nullptr)
        , m_selProp(nullptr)
        , m_grid(nullptr)
    {
    }

    ~wxPGComboBox()
    {
        if ( m_dclickProcessor )
        {
            // PropertyGrid teardown deliberately unlinks every pushed
            // handler before parking the editor HWND. In that case this
            // control-owned handler is already detached, but remains ours.
            if ( !m_dclickProcessor->IsUnlinked() )
                RemoveEventHandler(m_dclickProcessor);
            delete m_dclickProcessor;
        }
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxS("wxOwnerDrawnComboBox"))
    {
        m_grid = wxWindowRef(parent);
        const wxWeakRef<wxWindow> weakThis(this);
        const bool created =
            wxOwnerDrawnComboBox::Create(parent,
                                         id,
                                         value,
                                         pos,
                                         size,
                                         choices,
                                         style,
                                         validator,
                                         name);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             !created )
            return false;

        wxPropertyGrid* const grid = GetGrid();
        if ( !grid )
            return false;

        // Enabling double-click processor makes sense
        // only for wxBoolProperty.
        m_selProp = grid->GetSelection();
        if ( !m_selProp )
            return false;

        wxBoolProperty* boolProp = wxDynamicCast(m_selProp, wxBoolProperty);
        if ( boolProp )
        {
            m_dclickProcessor =
                new wxPGDoubleClickProcessor(grid, this, boolProp);
            PushEventHandler(m_dclickProcessor);
        }

        return true;
    }

    virtual void OnDrawItem( wxDC& dc,
                             const wxRect& rect,
                             int item,
                             int flags ) const override
    {
        wxPropertyGrid* pg = GetGrid();
        if ( !pg )
            return;

        // Handle hint text via super class
        if ( (flags & wxODCB_PAINTING_CONTROL) &&
             ShouldUseHintText(flags) )
        {
            wxOwnerDrawnComboBox::OnDrawItem(dc, rect, item, flags);
        }
        else
        {
            wxRect r(rect);
            pg->OnComboItemPaint(this, item, &dc, r, flags);
        }
    }

    virtual wxCoord OnMeasureItem( size_t item ) const override
    {
        wxPropertyGrid* pg = GetGrid();
        if ( !pg )
            return 0;
        wxRect rect;
        rect.x = -1;
        rect.width = 0;
        pg->OnComboItemPaint( this, item, nullptr, rect, 0 );
        return rect.height;
    }

    wxPropertyGrid* GetGrid() const
    {
        wxPropertyGrid* pg =
            wxDynamicCast(m_grid.get(), wxPropertyGrid);
        if ( pg && wxWindowIsUnavailableForCallbacks(pg) )
            return nullptr;
        return pg;
    }

    virtual wxCoord OnMeasureItemWidth( size_t item ) const override
    {
        wxPropertyGrid* pg = GetGrid();
        if ( !pg )
            return 0;
        wxRect rect;
        rect.x = -1;
        rect.width = -1;
        pg->OnComboItemPaint( this, item, nullptr, rect, 0 );
        return rect.width;
    }

#if defined(__WXMSW__)
#define wxPG_TEXTCTRLXADJUST3 0
#elif defined(__WXGTK__)
  #if defined(__WXGTK3__)
  #define wxPG_TEXTCTRLXADJUST3 2
  #else
  #define wxPG_TEXTCTRLXADJUST3 0
  #endif // wxGTK3/!wxGTK3
#elif defined(__WXOSX__)
#define wxPG_TEXTCTRLXADJUST3 6
#else
#define wxPG_TEXTCTRLXADJUST3 0
#endif

    virtual void PositionTextCtrl( int textCtrlXAdjust,
                                   int WXUNUSED(textCtrlYAdjust) ) override
    {
    #ifdef wxPG_TEXTCTRLXADJUST
        textCtrlXAdjust = wxPG_TEXTCTRLXADJUST -
                          (wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1) - 1;
    #endif
        wxOwnerDrawnComboBox::PositionTextCtrl(
            textCtrlXAdjust + wxPG_TEXTCTRLXADJUST3,
            0
        );
    }

    wxPGProperty* GetProperty() const { return m_selProp; }

private:
    wxPGDoubleClickProcessor*   m_dclickProcessor;
    wxPGProperty*               m_selProp;
    wxWindowRef                 m_grid;
};


void wxPropertyGrid::OnComboItemPaint( const wxPGComboBox* pCb,
                                       int item,
                                       wxDC* pDc,
                                       wxRect& rect,
                                       int flags )
{
    wxCHECK_RET( pCb, wxS("Invalid property-grid combo box") );

    const wxWeakRef<wxWindow> weakThis(this);
    const wxWeakRef<wxWindow> weakCombo(
        const_cast<wxPGComboBox*>(pCb));
    wxPropertyGridPageState* const state = m_pState;
    wxPGProperty* p = pCb->GetProperty();
    if ( !state || !p || GetSelection() != p ||
         IsPropertyPendingRemoval(p) )
    {
        return;
    }

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    const wxScopeGuard leaveComboPaint = wxMakeGuard(
        [weakThis, this]()
        {
            if ( weakThis.get() == this )
                --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        });
    wxUnusedVar(leaveComboPaint);

    // Application-defined properties and renderers can synchronously remove
    // the selected property or destroy/recreate its editor. Keep the property
    // itself deferred by m_propertyCallbackDepth and validate both windows and
    // all identity-bearing state before using any callback result.
    const auto transactionIsValid =
        [weakThis, weakCombo, this, pCb, state, p]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   wxWeakWindowIsAvailableForCallbacks(weakCombo, pCb) &&
                   m_pState == state &&
                   GetSelection() == p &&
                   !IsPropertyPendingRemoval(p) &&
                   pCb->GetProperty() == p;
        };

    wxString text;

    // Keep a COW snapshot: GetValueAsString()/GetString() below are virtual
    // callback boundaries and may mutate this property's live choices without
    // deleting either the property or the grid.
    const wxPGChoices choices = p->GetChoices();
    int comValIndex = -1;

    const int choiceCount = choices.IsOk()? choices.GetCount(): 0;
    if ( item >= choiceCount && p->GetDisplayedCommonValueCount() > 0 )
    {
        comValIndex = item - choiceCount;
        if ( !p->IsValueUnspecified() || !(flags & wxODCB_PAINTING_CONTROL) )
        {
            text = GetCommonValueLabel(comValIndex);
        }
    }
    else
    {
        if ( !(flags & wxODCB_PAINTING_CONTROL) )
        {
            text = pCb->GetString(item);
        }
        else
        {
            if ( !p->IsValueUnspecified() )
#if WXWIN_COMPATIBILITY_3_2
                // Special implementation with check if user-overriden obsolete function is still in use
                text = p->GetValueAsStringWithCheck(wxPGPropValFormatFlags::Null);
#else
                text = p->GetValueAsString(wxPGPropValFormatFlags::Null);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
        }
    }
    if ( !transactionIsValid() )
        return;

    if ( item < 0 )
        return;

    wxBitmap itemBitmap;
    wxPGChoiceEntry choiceEntry;
    bool hasChoiceEntry = false;

    if ( comValIndex == -1 && choices.IsOk() && item < choiceCount )
    {
        // Keep the entry, and especially its bitmap bundle, independent of the
        // property's choices before invoking application-provided bundle code.
        // GetBitmapFor() may synchronously mutate the choices or destroy the
        // grid/combo, so never retain a pointer into choices across it.
        choiceEntry = choices.Item(item);
        hasChoiceEntry = true;

        const wxBitmapBundle itemBundle = choiceEntry.GetBitmap();
        if ( itemBundle.IsOk() )
        {
            itemBitmap = itemBundle.GetBitmapFor(this);
            if ( !transactionIsValid() )
                return;
        }
    }

    //
    // Decide what custom image size to use
    // (Use item-specific bitmap only if not drawn in the control field.)
    wxSize cis;
    if ( itemBitmap.IsOk() && !(flags & wxODCB_PAINTING_CONTROL) )
    {
        cis = itemBitmap.GetSize();
    }
    else
    {
        if ( !TryGetImageSize(p, item, &cis) )
            return;
        if ( !transactionIsValid() )
            return;
    }

    if ( rect.x + rect.width < 0 )
    {
        // Default measure behaviour (no flexible, custom paint image only)
        if ( rect.width < 0 )
        {
            wxCoord x, y;
            pCb->GetTextExtent(text, &x, &y);
            rect.width = cis.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 + 9 + x;
        }

        rect.height = cis.y + 2;
        return;
    }

    wxPGPaintData paintdata;
    paintdata.m_parent = this;
    paintdata.m_choiceItem = item;

    // This is by the current (1.0.0b) spec - if painting control, item is -1
    if ( (flags & wxODCB_PAINTING_CONTROL) )
        paintdata.m_choiceItem = -1;

    wxCHECK_RET( pDc, wxS("Invalid DC") );

    wxDC& dc = *pDc;
    dc.SetBrush(*wxWHITE_BRUSH);

    //
    // DrawItem call
    wxPGCellRenderer* renderer = nullptr;
    const wxPGChoiceEntry* cell = nullptr;

    wxPoint pt(rect.x + wxPG_CONTROL_MARGIN - wxPG_CHOICEXADJUST - 1,
                rect.y + 1);

    int renderFlags = wxPGCellRenderer::DontUseCellColours;

    // If custom image had some size, we will start from the assumption
    // that custom paint procedure is required
    bool useCustomPaintProcedure =  cis.x > 0;

    if ( flags & wxODCB_PAINTING_SELECTED )
        renderFlags |= wxPGCellRenderer::Selected;

    if ( flags & wxODCB_PAINTING_CONTROL )
    {
        renderFlags |= wxPGCellRenderer::Control;

        // If wxPGFlags::CustomImage was set, then that means any custom
        // image will not appear on the control row (it may be too
        // large to fit, for instance). Also do not draw custom image
        // if no choice was selected.
        if ( !p->HasFlag(wxPGFlags::CustomImage) )
            useCustomPaintProcedure = false;
    }
    else
    {
        renderFlags |= wxPGCellRenderer::ChoicePopup;

        // For consistency, always use normal font when drawing drop down
        // items
        dc.SetFont(GetFont());
    }

    // If not drawing a selected popup item, then give property's
    // value image a chance.
    const bool hasValueImage = p->GetValueImage() != nullptr;
    if ( !transactionIsValid() )
        return;

    if ( hasValueImage && item != pCb->GetSelection() )
        useCustomPaintProcedure = false;
    // If current choice had a bitmap set by the application, then
    // use it instead of any custom paint procedure
    // (only if not drawn in the control field).
    else if ( itemBitmap.IsOk() && !(flags & wxODCB_PAINTING_CONTROL) )
        useCustomPaintProcedure = false;

    if ( useCustomPaintProcedure )
    {
        pt.x += wxCC_CUSTOM_IMAGE_MARGIN1;
        wxRect r(pt, cis);

        if ( flags & wxODCB_PAINTING_CONTROL )
        {
            //r.width = cis.x;
            r.height = wxPG_STD_CUST_IMAGE_HEIGHT(m_lineHeight);
        }

        paintdata.m_drawnWidth = r.width;

        dc.SetPen(m_colPropFore);
        if ( comValIndex >= 0 )
        {
            const wxPGCommonValue* cv = GetCommonValue(comValIndex);
            renderer = cv->GetRenderer();
            r.width = rect.width;
            renderer->Render( dc, r, this, p, m_selColumn, comValIndex, renderFlags );
            return;
        }
        else
        {
            p->OnCustomPaint( dc, r, paintdata );
            if ( !transactionIsValid() )
                return;
        }

        pt.x += paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN2 - 1;
    }
    else
    {
        // TODO: This aligns text so that it seems to be horizontally
        //       on the same line as property values. Not really
        //       sure if it is needed, but seems to not cause any harm.
        pt.x -= 1;

        if ( hasChoiceEntry && comValIndex < 0 )
        {
            // This aligns bitmap horizontally so that it is
            // on the same position as bitmap drawn for static content
            // (without editor).
            wxRect r(rect);
            r.x -= 1;

            cell = &choiceEntry;
            renderer = wxPGGlobalVars->m_defaultRenderer;
            int imageOffset = renderer->PreDrawCell(dc, r, this, *cell,
                                                    renderFlags );
            if ( !transactionIsValid() )
                return;
            if ( imageOffset )
                imageOffset += wxCC_CUSTOM_IMAGE_MARGIN1 +
                                wxCC_CUSTOM_IMAGE_MARGIN2;
            pt.x += imageOffset;
        }
    }

    //
    // Draw text
    //

    pt.y += (rect.height-m_fontHeight)/2 - 1;

    pt.x += 1;

    dc.DrawText( text, pt.x + wxPG_XBEFORETEXT, pt.y );

    if ( renderer )
        renderer->PostDrawCell(dc, this, *cell, renderFlags);
}

static
bool wxPGChoiceEditor_SetCustomPaintWidth( wxPropertyGrid* propGrid, wxPGComboBox* cb, int cmnVal )
{
    wxPGProperty* property = propGrid->GetSelectedProperty();
    wxASSERT( property );
    const wxWeakRef<wxWindow> weakGrid(propGrid);
    const wxWeakRef<wxWindow> weakCombo(cb);

    wxSize imageSize;
    bool res;

    // TODO: Do this always when cell has custom text.
    if ( property->IsValueUnspecified() )
    {
        cb->SetCustomPaintWidth( 0 );
        return true;
    }

    if ( cmnVal >= 0 )
    {
        // Yes, a common value is being selected
        property->SetCommonValue( cmnVal );
        imageSize = propGrid->GetCommonValue(cmnVal)->
                            GetRenderer()->GetImageSize(property, 1, cmnVal);
        res = false;
    }
    else
    {
        imageSize = propGrid->GetImageSize(property, -1);
        res = true;
    }

    if ( !wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) ||
         !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         propGrid->GetSelectedProperty() != property )
    {
        return false;
    }

    if ( imageSize.x )
        imageSize.x += ODCB_CUST_PAINT_MARGIN;
    cb->SetCustomPaintWidth( imageSize.x );

    return res;
}

// CreateControls calls this with CB_READONLY in extraStyle
wxWindow* wxPGChoiceEditor::CreateControlsBase( wxPropertyGrid* propGrid,
                                                wxPGProperty* property,
                                                const wxPoint& pos,
                                                const wxSize& sz,
                                                long extraStyle ) const
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakGrid(propGrid);
    wxPropertyGridPageState* const state = propGrid->GetState();
    const auto gridTransactionIsValid =
        [weakGrid, propGrid, property, state]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) &&
                   propGrid->GetState() == state &&
                   propGrid->GetSelection() == property &&
                   !propGrid->IsPropertyPendingRemoval(property);
        };

    // Since it is not possible (yet) to create a read-only combo box in
    // the same sense that wxTextCtrl is read-only, simply do not create
    // the control in this case.
    if ( property->HasFlag(wxPGFlags::ReadOnly) )
        return nullptr;

    // Keep the labels alive independently of a property callback removing
    // this property while its editor is being created.
    wxPGChoices choices = property->GetChoices();
    wxString defString;
    int index = property->GetChoiceSelection();
    if ( !gridTransactionIsValid() )
        return nullptr;

    wxPGPropValFormatFlags fmtFlags = wxPGPropValFormatFlags::Null;
    if ( !property->HasFlag(wxPGFlags::ReadOnly) &&
         !property->IsValueUnspecified() )
        fmtFlags |= wxPGPropValFormatFlags::EditableValue;
#if WXWIN_COMPATIBILITY_3_2
    // Special implementation with check if user-overriden obsolete function is still in use
    defString = property->GetValueAsStringWithCheck(fmtFlags);
#else
    defString = property->GetValueAsString(fmtFlags);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
    if ( !gridTransactionIsValid() )
        return nullptr;

    wxArrayString labels = choices.GetLabels();

    wxPoint po(pos);
    wxSize si(sz);
    po.y += wxPG_CHOICEYADJUST;
    si.y -= (wxPG_CHOICEYADJUST*2);

    po.x += wxPG_CHOICEXADJUST;
    si.x -= wxPG_CHOICEXADJUST;
    wxWindow* ctrlParent = propGrid->GetPanel();

    int odcbFlags = extraStyle | wxBORDER_NONE | wxTE_PROCESS_ENTER;

    if ( property->HasFlag(wxPGPropertyFlags_UseDCC) &&
         wxDynamicCast(property, wxBoolProperty) )
        odcbFlags |= wxODCB_DCLICK_CYCLES;

    //
    // If common value specified, use appropriate index
    unsigned int cmnVals = property->GetDisplayedCommonValueCount();
    if ( cmnVals )
    {
        if ( !property->IsValueUnspecified() )
        {
            int cmnVal = property->GetCommonValue();
            if ( cmnVal >= 0 )
            {
                index = labels.size() + cmnVal;
            }
        }

        for ( unsigned int i = 0; i < cmnVals; i++ )
            labels.Add(propGrid->GetCommonValueLabel(i));
    }
    if ( !gridTransactionIsValid() )
        return nullptr;

    wxPGComboBox* cb = new wxPGComboBox();
    const wxWeakRef<wxWindow> weakCombo(cb);
#ifdef __WXMSW__
    cb->Hide();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         !gridTransactionIsValid() )
        return weakCombo.get();
#endif
    const bool created = cb->Create(ctrlParent,
                                    wxID_ANY,
                                    wxString(),
                                    po,
                                    si,
                                    labels,
                                    odcbFlags);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) )
        return nullptr;
    if ( !created )
    {
        return wxPGResolveFailedEditorCreation(
            cb, weakCombo, gridTransactionIsValid());
    }
    if ( !gridTransactionIsValid() )
        return weakCombo.get();

    // Under OSX default button seems to look fine
    // so there is no need to change it.
#ifndef __WXOSX__
    cb->SetButtonPosition(si.y,0,wxRIGHT);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         !gridTransactionIsValid() )
        return weakCombo.get();
#endif // !__WXOSX__
    cb->SetMargins(wxPG_XBEFORETEXT-1);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         !gridTransactionIsValid() )
        return weakCombo.get();

    cb->SetBackgroundColour(propGrid->GetCellBackgroundColour());
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         !gridTransactionIsValid() )
        return weakCombo.get();

    // Set hint text
    const wxString hint = property->GetHintText();
    if ( !gridTransactionIsValid() )
        return weakCombo.get();
    cb->SetHint(hint);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         !gridTransactionIsValid() )
        return weakCombo.get();

    const int commonValue = property->GetCommonValue();
    if ( !gridTransactionIsValid() )
        return weakCombo.get();
    wxPGChoiceEditor_SetCustomPaintWidth(propGrid, cb, commonValue);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         !gridTransactionIsValid() )
        return weakCombo.get();

    if ( index >= 0 && index < (int)cb->GetCount() )
    {
        cb->SetSelection( index );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
             !gridTransactionIsValid() )
            return weakCombo.get();
        if ( !defString.empty() )
        {
            cb->SetText( defString );
            if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
                 !gridTransactionIsValid() )
                return weakCombo.get();
        }
    }
    else if ( !(extraStyle & wxCB_READONLY) && !defString.empty() )
    {
        propGrid->SetupTextCtrlValue(defString);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
             !gridTransactionIsValid() )
            return weakCombo.get();
        cb->SetValue( defString );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
             !gridTransactionIsValid() )
            return weakCombo.get();
    }
    else
    {
        cb->SetSelection( -1 );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
             !gridTransactionIsValid() )
            return weakCombo.get();
    }

#ifdef __WXMSW__
    cb->Show();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCombo, cb) ||
         !gridTransactionIsValid() )
        return weakCombo.get();
#endif

    return (wxWindow*) cb;
}


void wxPGChoiceEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_RET(cb, "Only wxOwnerDrawnComboBox editor can be updated");

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxPropertyGrid* const pg = property->GetGrid();
    const wxPGEditorCallbackTransaction transaction(pg, property, cb);
    if ( !transaction.IsValid() )
        return;

    const int ind = property->GetChoiceSelection();
    if ( !transaction.IsValid() )
        return;

    cb->SetSelection(ind);
}

wxPGWindowList wxPGChoiceEditor::CreateControls( wxPropertyGrid* propGrid, wxPGProperty* property,
        const wxPoint& pos, const wxSize& sz ) const
{
    return CreateControlsBase(propGrid,property,pos,sz,wxCB_READONLY);
}


int wxPGChoiceEditor::InsertItem( wxWindow* ctrl, const wxString& label, int index ) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_MSG(cb, wxNOT_FOUND, "Only wxOwnerDrawnComboBox editor can be updated");

    if (index < 0)
        index = cb->GetCount();

    return cb->Insert(label,index);
}


void wxPGChoiceEditor::DeleteItem( wxWindow* ctrl, int index ) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_RET(cb, "Only wxOwnerDrawnComboBox editor can be updated");

    cb->Delete(index);
}

void wxPGChoiceEditor::SetItems(wxWindow* ctrl, const wxArrayString& labels) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_RET(cb, "Only wxOwnerDrawnComboBox editor can be updated");

    cb->Set(labels);
}

bool wxPGChoiceEditor::OnEvent( wxPropertyGrid* propGrid, wxPGProperty* property,
    wxWindow* ctrl, wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMBOBOX )
    {
        wxPGComboBox* cb = (wxPGComboBox*)ctrl;
        int index = cb->GetSelection();
        int cmnValIndex = -1;
        int cmnVals = property->GetDisplayedCommonValueCount();
        int items = cb->GetCount();

        if ( index >= (items-cmnVals) )
        {
            // Yes, a common value is being selected
            cmnValIndex = index - (items-cmnVals);
            property->SetCommonValue( cmnValIndex );

            // Truly set value to unspecified?
            if ( propGrid->GetUnspecifiedCommonValue() == cmnValIndex )
            {
                if ( !property->IsValueUnspecified() )
                    propGrid->SetInternalFlag(wxPropertyGrid::wxPG_FL_VALUE_CHANGE_IN_EVENT);
                property->SetValueToUnspecified();
                if ( !cb->HasFlag(wxCB_READONLY) )
                {
                    wxString unspecValueText;
                    unspecValueText = propGrid->GetUnspecifiedValueText();
                    propGrid->SetupTextCtrlValue(unspecValueText);
                    cb->GetTextCtrl()->SetValue(unspecValueText);
                }
                return false;
            }
        }
        return wxPGChoiceEditor_SetCustomPaintWidth( propGrid, cb, cmnValIndex );
    }
    return false;
}


bool wxPGChoiceEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;

    int index = cb->GetSelection();

    if ( index != property->GetChoiceSelection() ||
        // Changing unspecified always causes event (returning
        // true here should be enough to trigger it).
         property->IsValueUnspecified()
       )
    {
#if WXWIN_COMPATIBILITY_3_2
        // Special implementation with check if user-overriden obsolete function is still in use
        return property->IntToValueWithCheck(variant, index, wxPGPropValFormatFlags::PropertySpecific);
#else
        return property->IntToValue(variant, index, wxPGPropValFormatFlags::PropertySpecific);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
    }
    return false;
}


void wxPGChoiceEditor::SetControlStringValue( wxPGProperty* property,
                                              wxWindow* ctrl,
                                              const wxString& txt ) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_RET(cb, "Only wxOwnerDrawnComboBox editor can be updated");

    property->GetGrid()->SetupTextCtrlValue(txt);
    cb->SetValue(txt);
}


void wxPGChoiceEditor::SetControlIntValue( wxPGProperty* WXUNUSED(property), wxWindow* ctrl, int value ) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_RET(cb, "Only wxOwnerDrawnComboBox editor can be updated");

    cb->SetSelection(value);
}


void wxPGChoiceEditor::SetValueToUnspecified( wxPGProperty* WXUNUSED(property),
                                              wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_RET(cb, "Only wxOwnerDrawnComboBox editor can be updated");

    if ( cb->HasFlag(wxCB_READONLY) )
        cb->SetSelection(-1);
}


bool wxPGChoiceEditor::CanContainCustomImage() const
{
    return true;
}


wxPGChoiceEditor::~wxPGChoiceEditor()
{
    wxPG_EDITOR(Choice) = nullptr;
}


// -----------------------------------------------------------------------
// wxPGComboBoxEditor
// -----------------------------------------------------------------------


WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(ComboBox,
                                      wxPGComboBoxEditor,
                                      wxPGChoiceEditor)


void wxPGComboBoxEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxPropertyGrid* const pg = property->GetGrid();
    const wxPGEditorCallbackTransaction transaction(pg, property, cb);
    if ( !transaction.IsValid() )
        return;

    const int index = property->GetChoiceSelection();
    if ( !transaction.IsValid() )
        return;
#if WXWIN_COMPATIBILITY_3_2
    // Special implementation with check if user-overriden obsolete function is still in use
    wxString s = property->GetValueAsStringWithCheck(wxPGPropValFormatFlags::EditableValue);
#else
    wxString s = property->GetValueAsString(wxPGPropValFormatFlags::EditableValue);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
    if ( !transaction.IsValid() )
        return;

    cb->SetSelection(index);
    if ( !transaction.IsValid() )
        return;

    pg->SetupTextCtrlValue(s);
    if ( !transaction.IsValid() )
        return;

    cb->SetValue(s);
}


wxPGWindowList wxPGComboBoxEditor::CreateControls( wxPropertyGrid* propGrid,
                                                   wxPGProperty* property,
                                                   const wxPoint& pos,
                                                   const wxSize& sz ) const
{
    return CreateControlsBase(propGrid,property,pos,sz,0);
}


bool wxPGComboBoxEditor::OnEvent( wxPropertyGrid* propGrid,
                                  wxPGProperty* property,
                                  wxWindow* ctrl,
                                  wxEvent& event ) const
{
    wxWindow* textCtrl = nullptr;

    if ( ctrl )
    {
        if ( auto cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox) )
            textCtrl = cb->GetTextCtrl();
    }

    if ( wxPGTextCtrlEditor::OnTextCtrlEvent(propGrid,property,textCtrl,event) )
        return true;

    return wxPGChoiceEditor::OnEvent(propGrid,property,ctrl,event);
}


bool wxPGComboBoxEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    wxString textVal = cb->GetValue();

    if ( property->UsesAutoUnspecified() && textVal.empty() )
    {
        variant.MakeNull();
        return true;
    }

#if WXWIN_COMPATIBILITY_3_2
    // Special implementation with check if user-overriden obsolete function is still in use
    bool res = property->StringToValueWithCheck(variant, textVal, wxPGPropValFormatFlags::EditableValue|wxPGPropValFormatFlags::PropertySpecific);
#else
    bool res = property->StringToValue(variant, textVal, wxPGPropValFormatFlags::EditableValue | wxPGPropValFormatFlags::PropertySpecific);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2

    // Changing unspecified always causes event (returning
    // true here should be enough to trigger it).
    if ( !res && variant.IsNull() )
        res = true;

    return res;
}


void wxPGComboBoxEditor::OnFocus( wxPGProperty* property,
                                  wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    wxPGTextCtrlEditor_OnFocus(property, cb->GetTextCtrl());
}


wxPGComboBoxEditor::~wxPGComboBoxEditor()
{
    wxPG_EDITOR(ComboBox) = nullptr;
}



// -----------------------------------------------------------------------
// wxPGChoiceAndButtonEditor
// -----------------------------------------------------------------------


WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(ChoiceAndButton,
                                      wxPGChoiceAndButtonEditor,
                                      wxPGChoiceEditor)


wxPGWindowList wxPGChoiceAndButtonEditor::CreateControls( wxPropertyGrid* propGrid,
                                                          wxPGProperty* property,
                                                          const wxPoint& pos,
                                                          const wxSize& sz ) const
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakGrid(propGrid);
    wxPropertyGridPageState* const state = propGrid->GetState();
    const auto gridTransactionIsValid =
        [weakGrid, propGrid, property, state]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) &&
                   propGrid->GetState() == state &&
                   propGrid->GetSelection() == property &&
                   !propGrid->IsPropertyPendingRemoval(property);
        };

    // Use one two units smaller to match size of the combo's dropbutton.
    // (normally a bigger button is used because it looks better)
    int bt_wid = sz.y;
    bt_wid -= 2;
    wxSize bt_sz(bt_wid,bt_wid);

    // Position of button.
    wxPoint bt_pos(pos.x+sz.x-bt_sz.x,pos.y);
#ifdef __WXMAC__
    bt_pos.y -= 1;
#else
    bt_pos.y += 1;
#endif

    wxWindow* bt = propGrid->GenerateEditorButton( bt_pos, bt_sz );
    const wxWeakRef<wxWindow> weakButton(bt);
    const auto completeTransactionIsValid =
        [&weakButton, bt, &gridTransactionIsValid]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakButton, bt) &&
                   gridTransactionIsValid();
        };
    if ( !completeTransactionIsValid() )
        return wxPGWindowList(nullptr, weakButton.get());

    // Size of choice.
    wxSize ch_sz(sz.x-bt->GetSize().x,sz.y);

#ifdef __WXMAC__
    ch_sz.x -= wxPG_TEXTCTRL_AND_BUTTON_SPACING;
#endif

    wxWindow* ch = wxPGChoiceEditor::CreateControls(propGrid,property,
        pos,ch_sz).GetPrimary();
    const wxWeakRef<wxWindow> weakChoice(ch);
    if ( !completeTransactionIsValid() )
        return wxPGWindowList(weakChoice.get(), weakButton.get());

#ifdef __WXMSW__
    wxWindow* const liveButton = weakButton.get();
    if ( liveButton != bt )
        return wxPGWindowList(weakChoice.get(), liveButton);

    liveButton->Show();
    if ( !completeTransactionIsValid() )
        return wxPGWindowList(weakChoice.get(), weakButton.get());
#endif

    return wxPGWindowList(weakChoice.get(), weakButton.get());
}


wxPGChoiceAndButtonEditor::~wxPGChoiceAndButtonEditor()
{
    wxPG_EDITOR(ChoiceAndButton) = nullptr;
}

// -----------------------------------------------------------------------
// wxPGTextCtrlAndButtonEditor
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(TextCtrlAndButton,
                                      wxPGTextCtrlAndButtonEditor,
                                      wxPGTextCtrlEditor)


wxPGWindowList wxPGTextCtrlAndButtonEditor::CreateControls( wxPropertyGrid* propGrid,
                                                            wxPGProperty* property,
                                                            const wxPoint& pos,
                                                            const wxSize& sz ) const
{
    wxWindow* wnd2;
    wxWindow* wnd = propGrid->GenerateEditorTextCtrlAndButton( pos, sz, &wnd2,
        property->HasFlag(wxPGFlags::NoEditor), property);

    return wxPGWindowList(wnd, wnd2);
}


wxPGTextCtrlAndButtonEditor::~wxPGTextCtrlAndButtonEditor()
{
    wxPG_EDITOR(TextCtrlAndButton) = nullptr;
}

// -----------------------------------------------------------------------
// wxPGCheckBoxEditor
// -----------------------------------------------------------------------

#if wxPG_INCLUDE_CHECKBOX

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(CheckBox,
                                      wxPGCheckBoxEditor,
                                      wxPGEditor)


// Check box state flags
enum class wxSimpleCheckBoxStates : int
{
    Unchecked   = 0,
    Checked     = 1,
    Bold        = 2,
    Unspecified = 4
};

constexpr wxSimpleCheckBoxStates operator&(wxSimpleCheckBoxStates a, wxSimpleCheckBoxStates b)
{
    return static_cast<wxSimpleCheckBoxStates>(static_cast<int>(a) & static_cast<int>(b));
}

constexpr wxSimpleCheckBoxStates operator|(wxSimpleCheckBoxStates a, wxSimpleCheckBoxStates b)
{
    return static_cast<wxSimpleCheckBoxStates>(static_cast<int>(a) | static_cast<int>(b));
}

inline wxSimpleCheckBoxStates operator|=(wxSimpleCheckBoxStates& a, wxSimpleCheckBoxStates b)
{
    return a = a | b;
}

constexpr wxSimpleCheckBoxStates operator^(wxSimpleCheckBoxStates a, wxSimpleCheckBoxStates b)
{
    return static_cast<wxSimpleCheckBoxStates>(static_cast<int>(a) ^ static_cast<int>(b));
}

inline wxSimpleCheckBoxStates operator^=(wxSimpleCheckBoxStates& a, wxSimpleCheckBoxStates b)
{
    return a = a ^ b;
}

constexpr bool operator!(wxSimpleCheckBoxStates a)
{
    return static_cast<int>(a) == 0;
}

const int wxSCB_SETVALUE_CYCLE = 2;

static void DrawSimpleCheckBox(wxWindow* win, wxDC& dc, const wxRect& rect, wxSimpleCheckBoxStates state)
{
#if wxPG_USE_RENDERER_NATIVE

    int cbFlags = 0;
    if ( !!(state & wxSimpleCheckBoxStates::Unspecified) )
    {
        cbFlags |= wxCONTROL_UNDETERMINED;
    }
    else if ( !!(state & wxSimpleCheckBoxStates::Checked) )
    {
        cbFlags |= wxCONTROL_CHECKED;
    }

    if ( !!(state & wxSimpleCheckBoxStates::Bold) )
    {
        // wxCONTROL_CHECKED and wxCONTROL_PRESSED flags
        // are equivalent for wxOSX so we have to use
        // other flag to indicate "selected state".
#ifdef __WXOSX__
        cbFlags |= wxCONTROL_FOCUSED;
#else
        cbFlags |= wxCONTROL_PRESSED;
#endif
    }

    // Ignore the specified height because the native renderer only draws
    // checkboxes correctly when using its own preferred size in high DPI.
    wxRendererNative::Get().DrawCheckBox
    (
        win,
        dc,
        wxRect(wxRendererNative::Get().GetCheckBoxSize(win)).CenterIn(rect),
        cbFlags
    );
#else
    wxUnusedVar(win);

    wxColour useCol = dc.GetTextForeground();

    if ( !!(state & wxSimpleCheckBoxStates::Unspecified) )
    {
        useCol = wxColour(220, 220, 220);
    }

    wxRect r(rect);
    // Draw check mark first because it is likely to overdraw the
    // surrounding rectangle.
    if ( !!(state & wxSimpleCheckBoxStates::Checked) )
    {
        wxRect r2(r.x+wxPG_CHECKMARK_XADJ,
                  r.y+wxPG_CHECKMARK_YADJ,
                  r.width+wxPG_CHECKMARK_WADJ,
                  r.height+wxPG_CHECKMARK_HADJ);
    #if wxPG_CHECKMARK_DEFLATE
        r2.Deflate(wxPG_CHECKMARK_DEFLATE);
    #endif
        dc.DrawCheckMark(r2);

        // This would draw a simple cross check mark.
        // dc.DrawLine(r.x,r.y,r.x+r.width-1,r.y+r.height-1);
        // dc.DrawLine(r.x,r.y+r.height-1,r.x+r.width-1,r.y);
    }

    if ( !(state & wxSimpleCheckBoxStates::Bold) )
    {
        // Pen for thin rectangle.
        dc.SetPen(useCol);
    }
    else
    {
        // Pen for bold rectangle.
        wxPen linepen(useCol,2,wxPENSTYLE_SOLID);
        linepen.SetJoin(wxJOIN_MITER); // This prevents round edges.
        dc.SetPen(linepen);
        r.x++;
        r.y++;
        r.width--;
        r.height--;
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.DrawRectangle(r);
    dc.SetPen(*wxTRANSPARENT_PEN);
#endif
}

//
// Real simple custom-drawn checkbox-without-label class.
//
class wxSimpleCheckBox : public wxControl
{
public:

    void SetValue( int value );

    wxSimpleCheckBox()
        : wxControl()
        , m_state(wxSimpleCheckBoxStates::Unchecked)
        , m_boxHeight(12)
        , m_grid(nullptr)
        , m_property(nullptr)
    {
    }

    bool Create( wxPropertyGrid* grid,
                 wxPGProperty* property,
                 wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize )
    {
        wxCHECK_MSG(grid && property, false,
                    "Invalid PropertyGrid checkbox owner");

        m_grid = grid;
        m_property = property;

        const wxWeakRef<wxWindow> weakGrid(grid);
        const wxWeakRef<wxWindow> weakThis(this);
        wxPropertyGridPageState* const state = grid->GetState();
        const auto transactionIsValid =
            [weakGrid, grid, property, state]()
            {
                return wxWeakWindowIsAvailableForCallbacks(weakGrid, grid) &&
                       state &&
                       grid->GetState() == state &&
                       grid->GetSelection() == property &&
                       !grid->IsPropertyPendingRemoval(property);
            };

        if ( !transactionIsValid() )
            return false;

        const bool created =
            wxControl::Create(grid->GetPanel(), id, pos, size,
                              wxBORDER_NONE | wxWANTS_CHARS);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             !created || !transactionIsValid() )
            return false;

        // Due to SetOwnFont stuff necessary for GTK+ 1.2, we need to have this
        wxControl::SetFont( grid->GetFont() );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
             !transactionIsValid() )
            return false;

        SetBoxHeight(12);
        wxControl::SetBackgroundStyle( wxBG_STYLE_PAINT );
        return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
               transactionIsValid();
    }

    virtual ~wxSimpleCheckBox() = default;

    void SetBoxHeight(int height)
    {
        m_boxHeight = height;
        m_boxRect = GetBoxRect(GetClientSize(), m_boxHeight);
    }

    static wxRect GetBoxRect(const wxRect& r, int box_h)
    {
        return wxRect(r.x + wxPG_XBEFORETEXT, r.y + ((r.height - box_h) / 2), box_h, box_h);
    }

    wxSimpleCheckBoxStates m_state;

private:
    void OnPaint( wxPaintEvent& event );
    void OnLeftClick( wxMouseEvent& event );
    void OnKeyDown( wxKeyEvent& event );

    void OnResize( wxSizeEvent& event )
    {
        SetBoxHeight(m_boxHeight); // Recalculate box rectangle
        Refresh();
        event.Skip();
    }
    void OnLeftClickActivate( wxCommandEvent& evt );

    int m_boxHeight;
    wxRect m_boxRect;
    wxWindowRef m_grid;
    wxPGProperty* m_property;

    wxDECLARE_EVENT_TABLE();
};

wxDEFINE_EVENT( wxEVT_CB_LEFT_CLICK_ACTIVATE, wxCommandEvent );

wxBEGIN_EVENT_TABLE(wxSimpleCheckBox, wxControl)
    EVT_PAINT(wxSimpleCheckBox::OnPaint)
    EVT_LEFT_DOWN(wxSimpleCheckBox::OnLeftClick)
    EVT_LEFT_DCLICK(wxSimpleCheckBox::OnLeftClick)
    EVT_KEY_DOWN(wxSimpleCheckBox::OnKeyDown)
    EVT_SIZE(wxSimpleCheckBox::OnResize)
    EVT_COMMAND(wxID_ANY, wxEVT_CB_LEFT_CLICK_ACTIVATE, wxSimpleCheckBox::OnLeftClickActivate)
wxEND_EVENT_TABLE()

void wxSimpleCheckBox::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxAutoBufferedPaintDC dc(this);

    wxColour bgcol = GetBackgroundColour();
    dc.SetBackground(wxBrush(bgcol));
    dc.Clear();
    dc.SetBrush( bgcol );
    dc.SetPen( bgcol );

    wxSimpleCheckBoxStates state = m_state;
    if ( !(state & wxSimpleCheckBoxStates::Unspecified) &&
         GetFont().GetWeight() == wxFONTWEIGHT_BOLD )
        state |= wxSimpleCheckBoxStates::Bold;

    DrawSimpleCheckBox(this, dc, m_boxRect, state);
}

void wxSimpleCheckBox::OnLeftClick( wxMouseEvent& event )
{
    if ( m_boxRect.Contains(event.GetPosition()) )
    {
        SetValue(wxSCB_SETVALUE_CYCLE);
    }
}

void wxSimpleCheckBox::OnKeyDown( wxKeyEvent& event )
{
    if ( event.GetKeyCode() == WXK_SPACE )
    {
        SetValue(wxSCB_SETVALUE_CYCLE);
    }
}

void wxSimpleCheckBox::SetValue( int value )
{
    wxPropertyGrid* const propGrid =
        wxDynamicCast(m_grid.get(), wxPropertyGrid);
    if ( !propGrid || !m_property || wxWindowIsUnavailableForCallbacks(propGrid) ||
         propGrid->GetSelection() != m_property ||
         propGrid->GetEditorControl() != this ||
         propGrid->IsPropertyPendingRemoval(m_property) )
    {
        return;
    }

    const wxWeakRef<wxWindow> weakGrid(propGrid);
    if ( value == wxSCB_SETVALUE_CYCLE )
    {
        m_state ^= wxSimpleCheckBoxStates::Checked;
    }
    else
    {
        m_state = value == 0 ? wxSimpleCheckBoxStates::Unchecked : wxSimpleCheckBoxStates::Checked;
    }
    Refresh();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) ||
         propGrid->GetSelection() != m_property ||
         propGrid->GetEditorControl() != this ||
         propGrid->IsPropertyPendingRemoval(m_property) )
    {
        return;
    }

    wxCommandEvent evt(wxEVT_CHECKBOX, propGrid->GetId());
    propGrid->HandleCustomEditorEvent(evt);
}

void wxSimpleCheckBox::OnLeftClickActivate( wxCommandEvent& evt )
{
    wxPoint pt(evt.GetInt(), evt.GetExtraLong());
    if ( m_boxRect.Contains(pt) )
    {
        SetValue(wxSCB_SETVALUE_CYCLE);
    }
}

wxPGWindowList wxPGCheckBoxEditor::CreateControls( wxPropertyGrid* propGrid,
                                                   wxPGProperty* property,
                                                   const wxPoint& pos,
                                                   const wxSize& size ) const
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakGrid(propGrid);
    wxPropertyGridPageState* const state = propGrid->GetState();
    const auto gridTransactionIsValid =
        [weakGrid, propGrid, property, state]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) &&
                   propGrid->GetState() == state &&
                   propGrid->GetSelection() == property &&
                   !propGrid->IsPropertyPendingRemoval(property);
        };

    if ( property->HasFlag(wxPGFlags::ReadOnly) )
        return nullptr;

    wxPoint pt = pos;
    pt.x -= wxPG_XBEFOREWIDGET;
    wxSize sz = size;
    sz.x = propGrid->GetFontHeight() + (wxPG_XBEFOREWIDGET*2) + 4;

    wxSimpleCheckBox* cb = new wxSimpleCheckBox();
    const wxWeakRef<wxWindow> weakCheckBox(cb);
    const bool created = cb->Create(propGrid, property, wxID_ANY, pt, sz);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCheckBox, cb) )
        return nullptr;
    if ( !created )
    {
        return wxPGResolveFailedEditorCreation(
            cb, weakCheckBox, gridTransactionIsValid());
    }
    if ( !gridTransactionIsValid() )
        return weakCheckBox.get();

    cb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCheckBox, cb) ||
         !gridTransactionIsValid() )
        return weakCheckBox.get();

    UpdateControl(property, cb);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakCheckBox, cb) ||
         !gridTransactionIsValid() )
        return weakCheckBox.get();

    if ( !property->IsValueUnspecified() )
    {
        if ( propGrid->HasInternalFlag(wxPropertyGrid::wxPG_FL_ACTIVATION_BY_CLICK) )
        {
            // Send the event to toggle the value (if mouse cursor is on the item)
            wxPoint point = cb->ScreenToClient(::wxGetMousePosition());
            wxCommandEvent *evt = new wxCommandEvent(wxEVT_CB_LEFT_CLICK_ACTIVATE, cb->GetId());
            // Store mouse pointer position
            evt->SetInt(point.x);
            evt->SetExtraLong(point.y);
            wxQueueEvent(cb, evt);
            if ( !wxWeakWindowIsAvailableForCallbacks(weakCheckBox, cb) ||
                 !gridTransactionIsValid() )
                return weakCheckBox.get();
        }
    }

    propGrid->SetInternalFlag(wxPropertyGrid::wxPG_FL_FIXED_WIDTH_EDITOR);

    return cb;
}

void wxPGCheckBoxEditor::DrawValue( wxDC& dc, const wxRect& rect,
                                    wxPGProperty* property,
                                    const wxString& WXUNUSED(text) ) const
{
    wxSimpleCheckBoxStates state = wxSimpleCheckBoxStates::Unchecked;

    if ( !property->IsValueUnspecified() )
    {
        state = property->GetChoiceSelection() == 0 ? wxSimpleCheckBoxStates::Unchecked : wxSimpleCheckBoxStates::Checked;
        if ( dc.GetFont().GetWeight() == wxFONTWEIGHT_BOLD )
            state |= wxSimpleCheckBoxStates::Bold;
    }
    else
    {
        state |= wxSimpleCheckBoxStates::Unspecified;
    }

    // Box rectangle
    wxRect r = wxSimpleCheckBox::GetBoxRect(rect, dc.GetCharHeight());
    DrawSimpleCheckBox(property->GetGrid(), dc, r, state);
}

void wxPGCheckBoxEditor::UpdateControl( wxPGProperty* property,
                                        wxWindow* ctrl ) const
{
    wxSimpleCheckBox* cb = wxDynamicCast(ctrl, wxSimpleCheckBox);
    wxCHECK_RET(cb, "Only wxSimpleCheckBox editor can be updated");

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    wxPropertyGrid* const propGrid = property->GetGrid();
    const wxPGEditorCallbackTransaction transaction(propGrid, property, cb);
    if ( !transaction.IsValid() )
        return;

    wxSimpleCheckBoxStates state;
    if ( !property->IsValueUnspecified() )
    {
        const int selection = property->GetChoiceSelection();
        if ( !transaction.IsValid() )
            return;

        state = selection == 0 ? wxSimpleCheckBoxStates::Unchecked
                               : wxSimpleCheckBoxStates::Checked;
    }
    else
    {
        state = wxSimpleCheckBoxStates::Unspecified;
    }

    cb->m_state = state;
    if ( !transaction.IsValid() )
        return;

    cb->SetBoxHeight(propGrid->GetFontHeight());
    if ( !transaction.IsValid() )
        return;

    cb->Refresh();
}

bool wxPGCheckBoxEditor::OnEvent( wxPropertyGrid* WXUNUSED(propGrid), wxPGProperty* WXUNUSED(property),
    wxWindow* WXUNUSED(ctrl), wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_CHECKBOX )
    {
        return true;
    }
    return false;
}


bool wxPGCheckBoxEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* ctrl ) const
{
    wxSimpleCheckBox* cb = (wxSimpleCheckBox*)ctrl;

    int index = !!(cb->m_state & wxSimpleCheckBoxStates::Checked) ? 1 : 0;

    if ( index != property->GetChoiceSelection() ||
         // Changing unspecified always causes event (returning
         // true here should be enough to trigger it).
         property->IsValueUnspecified()
       )
    {
#if WXWIN_COMPATIBILITY_3_2
        // Special implementation with check if user-overriden obsolete function is still in use
        return property->IntToValueWithCheck(variant, index, wxPGPropValFormatFlags::PropertySpecific);
#else
        return property->IntToValue(variant, index, wxPGPropValFormatFlags::PropertySpecific);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2
    }
    return false;
}


void wxPGCheckBoxEditor::SetControlIntValue( wxPGProperty* WXUNUSED(property), wxWindow* ctrl, int value ) const
{
    ((wxSimpleCheckBox*)ctrl)->m_state = value == 0 ? wxSimpleCheckBoxStates::Unchecked : wxSimpleCheckBoxStates::Checked;
    ctrl->Refresh();
}


void wxPGCheckBoxEditor::SetValueToUnspecified( wxPGProperty* WXUNUSED(property), wxWindow* ctrl ) const
{
    ((wxSimpleCheckBox*)ctrl)->m_state = wxSimpleCheckBoxStates::Unspecified;
    ctrl->Refresh();
}


wxPGCheckBoxEditor::~wxPGCheckBoxEditor()
{
    wxPG_EDITOR(CheckBox) = nullptr;
}

#endif // wxPG_INCLUDE_CHECKBOX

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GetEditorControl() const
{
    return  m_wndEditor;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CorrectEditorWidgetSizeX()
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    const wxScopeGuard leaveEditorResize = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveEditorResize);

    wxPropertyGridPageState* const state = m_pState;
    wxWindow* const primary = m_wndEditor;
    wxWindow* const secondary = m_wndEditor2;
    const wxWeakRef<wxWindow> weakPrimary(primary);
    const wxWeakRef<wxWindow> weakSecondary(secondary);
    const auto transactionIsValid =
        [this, state, primary, secondary,
         &weakThis, &weakPrimary, &weakSecondary]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state )
                return false;
            if ( primary &&
                 !wxWeakWindowIsAvailableForCallbacks(weakPrimary, primary) )
            {
                if ( m_wndEditor == primary )
                    m_wndEditor = nullptr;
                return false;
            }
            if ( secondary &&
                 !wxWeakWindowIsAvailableForCallbacks(weakSecondary,
                                                      secondary) )
            {
                if ( m_wndEditor2 == secondary )
                    m_wndEditor2 = nullptr;
                return false;
            }
            return m_wndEditor == primary && m_wndEditor2 == secondary;
        };

    if ( !state )
        return;

    int secWid = 0;

    // Use fixed selColumn 1 for main editor widgets
    int newSplitterx;
    CalcScrolledPosition(state->DoGetSplitterPosition(0), 0, &newSplitterx, nullptr);
    int newWidth = newSplitterx + state->GetColumnWidth(1);

    if ( secondary )
    {
        // if width change occurred, move secondary wnd by that amount
        wxRect r = secondary->GetRect();
        secWid = r.width;
        r.x = newWidth - secWid;

        secondary->SetSize( r );
        if ( !transactionIsValid() )
            return;

        // if primary is textctrl, then we have to add some extra space
#ifdef __WXMAC__
        if ( primary )
#else
        if ( wxDynamicCast(primary, wxTextCtrl) )
#endif
            secWid += wxPG_TEXTCTRL_AND_BUTTON_SPACING;
    }

    if ( primary )
    {
        wxRect r = primary->GetRect();

        r.x = newSplitterx+m_ctrlXAdjust;

        if ( !(m_iFlags & wxPG_FL_FIXED_WIDTH_EDITOR) )
            r.width = newWidth - r.x - secWid;

        primary->SetSize(r);
        if ( !transactionIsValid() )
            return;
    }

    if ( secondary )
    {
        secondary->Refresh();
        if ( !transactionIsValid() )
            return;
    }
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CorrectEditorWidgetPosY()
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    const wxScopeGuard leaveEditorMove = wxMakeGuard([weakThis, this]()
    {
        if ( weakThis.get() == this )
            --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    });
    wxUnusedVar(leaveEditorMove);

    wxPropertyGridPageState* const state = m_pState;
    wxPGProperty* const selected = GetSelection();
    wxWindow* const label = m_labelEditor;
    wxWindow* const primary = m_wndEditor;
    wxWindow* const secondary = m_wndEditor2;
    const wxWeakRef<wxWindow> weakLabel(label);
    const wxWeakRef<wxWindow> weakPrimary(primary);
    const wxWeakRef<wxWindow> weakSecondary(secondary);
    const auto transactionIsValid =
        [this, state, selected, label, primary, secondary,
         &weakThis, &weakLabel, &weakPrimary, &weakSecondary]()
        {
            if ( !wxWeakWindowIsAvailableForCallbacks(weakThis, this) ||
                 m_pState != state ||
                 GetSelection() != selected ||
                 (selected && IsPropertyPendingRemoval(selected)) )
            {
                return false;
            }
            if ( label &&
                 !wxWeakWindowIsAvailableForCallbacks(weakLabel, label) )
            {
                if ( m_labelEditor == label )
                {
                    m_labelEditor = nullptr;
                    m_labelEditorProperty = nullptr;
                }
                return false;
            }
            if ( primary &&
                 !wxWeakWindowIsAvailableForCallbacks(weakPrimary, primary) )
            {
                if ( m_wndEditor == primary )
                    m_wndEditor = nullptr;
                return false;
            }
            if ( secondary &&
                 !wxWeakWindowIsAvailableForCallbacks(weakSecondary,
                                                      secondary) )
            {
                if ( m_wndEditor2 == secondary )
                    m_wndEditor2 = nullptr;
                return false;
            }
            return m_labelEditor == label &&
                   m_wndEditor == primary &&
                   m_wndEditor2 == secondary;
        };

    if ( selected )
    {
        if ( label )
        {
            wxRect r;
            if ( !TryGetEditorWidgetRect(selected, m_selColumn, &r) ||
                 !transactionIsValid() )
                return;
            label->Move(r.GetPosition() + m_labelEditorPosRel);
            if ( !transactionIsValid() )
                return;
        }

        if ( primary || secondary )
        {
            wxRect r;
            if ( !TryGetEditorWidgetRect(selected, 1, &r) ||
                 !transactionIsValid() )
                return;

            if ( primary )
            {
                primary->Move(r.GetPosition() + m_wndEditorPosRel);
                if ( !transactionIsValid() )
                    return;
            }

            if ( secondary )
            {
                secondary->Move(r.GetPosition() + m_wndEditor2PosRel);
                if ( !transactionIsValid() )
                    return;
            }
        }
    }
}

// -----------------------------------------------------------------------

// Fixes position of wxTextCtrl-like control (wxSpinCtrl usually
// fits into that category as well).
#ifndef wxPG_TEXTCTRLXADJUST
#if defined(__WXMSW__)
#define wxPG_TEXTCTRLXADJUST2 0
#elif defined(__WXGTK__)
  #if defined(__WXGTK3__)
  #define wxPG_TEXTCTRLXADJUST2 (-2)
  #else
  #define wxPG_TEXTCTRLXADJUST2 0
  #endif // wxGTK3/!wxGTK3
#else
#error "wxPG_TEXTCTRLXADJUST should be defined for this platform"
#endif
#endif // !wxPG_TEXTCTRLXADJUST

void wxPropertyGrid::FixPosForTextCtrl( wxWindow* ctrl,
                                        unsigned int WXUNUSED(forColumn),
                                        const wxPoint& offset )
{
    // Center the control vertically
    wxRect finalPos = ctrl->GetRect();
    int y_adj = (m_lineHeight - finalPos.height)/2 + wxPG_TEXTCTRLYADJUST;

    // Prevent over-sized control
    int sz_dec = (y_adj + finalPos.height) - m_lineHeight;
    if ( sz_dec < 0 ) sz_dec = 0;

    finalPos.y += y_adj;
    finalPos.height -= (y_adj+sz_dec);

#ifndef wxPG_TEXTCTRLXADJUST
    int textCtrlXAdjust = wxPG_XBEFORETEXT - 1 + wxPG_TEXTCTRLXADJUST2;

    wxTextCtrl* tc = static_cast<wxTextCtrl*>(ctrl);
    tc->SetMargins(0);
#else
    int textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;
#endif

    finalPos.x += textCtrlXAdjust;
    finalPos.width -= textCtrlXAdjust;

    finalPos.Offset(offset);

    ctrl->SetSize(finalPos);
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorTextCtrl( const wxPoint& pos,
                                                  const wxSize& sz,
                                                  const wxString& value,
                                                  wxWindow* secondary,
                                                  int extraStyle,
                                                  int maxLen,
                                                  unsigned int forColumn )
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    wxPGProperty* prop = GetSelection();
    if ( !state || !prop )
        return nullptr;

    const wxWeakRef<wxWindow> weakSecondary(secondary);
    const auto gridTransactionIsValid =
        [weakThis, this, state, prop, secondary, &weakSecondary]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state &&
                   GetSelection() == prop &&
                   !IsPropertyPendingRemoval(prop) &&
                   (!secondary ||
                    wxWeakWindowIsAvailableForCallbacks(weakSecondary,
                                                        secondary));
        };

    int tcFlags = wxTE_PROCESS_ENTER | extraStyle;

    if ( prop->HasFlag(wxPGFlags::ReadOnly) && forColumn == 1 )
        tcFlags |= wxTE_READONLY;

    wxPoint p(pos);
    wxSize s(sz);

   // Need to reduce width of text control on Mac
#if defined(__WXMAC__)
    s.x -= 4;
#endif

    // For label editors, trim the size to allow better splitter grabbing
    if ( forColumn != 1 )
        s.x -= 2;

    // Take button into account
    if ( secondary )
    {
        s.x -= (secondary->GetSize().x + wxPG_TEXTCTRL_AND_BUTTON_SPACING);
        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
    }

    // If the height is significantly higher, then use border, and fill the rect exactly.
    const bool hasSpecialSize = (sz.y - m_lineHeight) > 5;

    wxWindow* ctrlParent = GetPanel();

    if ( !hasSpecialSize )
        tcFlags |= wxBORDER_NONE;

    wxTextCtrl* tc = new wxTextCtrl();
    const wxWeakRef<wxWindow> weakText(tc);

#if defined(__WXMSW__)
    tc->Hide();
#endif
    SetupTextCtrlValue(value);
    const bool created =
        tc->Create(ctrlParent,wxID_ANY,value, p, s,tcFlags);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) )
        return nullptr;
    if ( !created )
    {
        return wxPGResolveFailedEditorCreation(
            tc, weakText, gridTransactionIsValid());
    }
    if ( !gridTransactionIsValid() )
        return weakText.get();

#if defined(__WXMSW__)
    // On Windows, we need to override read-only text ctrl's background
    // colour to white. One problem with native 'grey' background is that
    // tc->GetBackgroundColour() doesn't seem to return correct value
    // for it.
    if ( tcFlags & wxTE_READONLY )
    {
        wxVisualAttributes vattrs = tc->GetDefaultAttributes();
        tc->SetBackgroundColour(vattrs.colBg);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
             !gridTransactionIsValid() )
            return weakText.get();
    }
#endif

    // This code is repeated from DoSelectProperty(). However, font boldness
    // must be set before margin is set up below in FixPosForTextCtrl().
    if ( forColumn == 1 &&
         prop->HasFlag(wxPGFlags::Modified) &&
         HasFlag(wxPG_BOLD_MODIFIED) )
    {
         tc->SetFont( m_captionFont );
         if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
              !gridTransactionIsValid() )
             return weakText.get();
    }

    // Center the control vertically
    if ( !hasSpecialSize )
    {
        FixPosForTextCtrl(tc, forColumn);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
             !gridTransactionIsValid() )
            return weakText.get();
    }

    if ( forColumn != 1 )
    {
        tc->SetBackgroundColour(m_colSelBack);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
             !gridTransactionIsValid() )
            return weakText.get();
        tc->SetForegroundColour(m_colSelFore);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
             !gridTransactionIsValid() )
            return weakText.get();
    }

#ifdef __WXMSW__
    tc->Show();
    if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
         !gridTransactionIsValid() )
        return weakText.get();
    if ( secondary )
    {
        secondary->Show();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
             !gridTransactionIsValid() )
            return weakText.get();
    }
#endif

    // Set maximum length
    if ( maxLen > 0 )
    {
        tc->SetMaxLength( maxLen );
        if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
             !gridTransactionIsValid() )
            return weakText.get();
    }

    wxVariant attrVal = prop->GetAttribute(wxPG_ATTR_AUTOCOMPLETE);
    if ( !gridTransactionIsValid() )
        return weakText.get();
    if ( !attrVal.IsNull() )
    {
        wxASSERT(attrVal.IsType(wxPG_VARIANT_TYPE_ARRSTRING));
        tc->AutoComplete(attrVal.GetArrayString());
        if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
             !gridTransactionIsValid() )
            return weakText.get();
    }

    // Set hint text
    const wxString hint = prop->GetHintText();
    if ( !gridTransactionIsValid() )
        return weakText.get();
    tc->SetHint(hint);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakText, tc) ||
         !gridTransactionIsValid() )
        return weakText.get();

    return tc;
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorButton( const wxPoint& pos, const wxSize& sz )
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    wxPGProperty* selected = GetSelection();
    if ( !state || !selected )
        return nullptr;

    const auto gridTransactionIsValid =
        [weakThis, this, state, selected]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state &&
                   GetSelection() == selected &&
                   !IsPropertyPendingRemoval(selected);
        };

    const wxString label(L"\u2026"); // "Horizontal ellipsis" character

    int dim = sz.y + 2*wxPG_BUTTON_BORDER_WIDTH;

    wxPoint p(pos.x + sz.x, pos.y - wxPG_BUTTON_BORDER_WIDTH);
    wxSize s(wxDefaultCoord, dim);

    wxButton* but = new wxButton();
    const wxWeakRef<wxWindow> weakButton(but);
  #ifdef __WXMSW__
    but->Hide();
  #endif
    const bool created =
        but->Create(GetPanel(), wxID_ANY, label, p, s,
                    wxWANTS_CHARS | wxBU_EXACTFIT);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakButton, but) )
        return nullptr;
    if ( !created )
    {
        return wxPGResolveFailedEditorCreation(
            but, weakButton, gridTransactionIsValid());
    }
    if ( !gridTransactionIsValid() )
        return weakButton.get();

    but->SetFont(GetFont().GetBaseFont().Smaller());
    if ( !wxWeakWindowIsAvailableForCallbacks(weakButton, but) ||
         !gridTransactionIsValid() )
        return weakButton.get();

    // If button is narrow make it a square and move it to the correct position
    s = but->GetSize();
    if ( s.x < s.y )
    {
        s.x = s.y;
        but->SetSize(s);
        if ( !wxWeakWindowIsAvailableForCallbacks(weakButton, but) ||
             !gridTransactionIsValid() )
            return weakButton.get();
    }
    p.x = pos.x + sz.x - s.x;
    but->Move(p);
    if ( !wxWeakWindowIsAvailableForCallbacks(weakButton, but) ||
         !gridTransactionIsValid() )
        return weakButton.get();

    if ( selected->HasFlag(wxPGFlags::ReadOnly) && !selected->HasFlag(wxPGPropertyFlags_ActiveButton) )
    {
        but->Disable();
        if ( !wxWeakWindowIsAvailableForCallbacks(weakButton, but) ||
             !gridTransactionIsValid() )
            return weakButton.get();
    }

    return but;
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorTextCtrlAndButton( const wxPoint& pos,
                                                           const wxSize& sz,
                                                           wxWindow** psecondary,
                                                           int limitedEditing,
                                                           wxPGProperty* property )
{
    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakThis(this);
    wxPropertyGridPageState* const state = m_pState;
    *psecondary = nullptr;
    if ( !state || !property || GetSelection() != property )
        return nullptr;

    const auto gridTransactionIsValid =
        [weakThis, this, state, property]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakThis, this) &&
                   m_pState == state &&
                   GetSelection() == property &&
                   !IsPropertyPendingRemoval(property);
        };

    wxButton* but = static_cast<wxButton*>(GenerateEditorButton(pos, sz));
    const wxWeakRef<wxWindow> weakButton(but);
    *psecondary = weakButton.get();
    const auto completeTransactionIsValid =
        [&weakButton, but, &gridTransactionIsValid]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakButton, but) &&
                   gridTransactionIsValid();
        };
    if ( !completeTransactionIsValid() )
        return nullptr;

    if ( limitedEditing )
    {
    #ifdef __WXMSW__
        // There is button Show in GenerateEditorTextCtrl as well
        wxWindow* const liveButton = weakButton.get();
        if ( liveButton != but )
        {
            *psecondary = liveButton;
            return nullptr;
        }

        liveButton->Show();
        *psecondary = weakButton.get();
        if ( !completeTransactionIsValid() )
            return nullptr;
    #endif
        return nullptr;
    }

    wxString text;

    if ( !property->IsValueUnspecified() )
#if WXWIN_COMPATIBILITY_3_2
        // Special implementation with check if user-overriden obsolete function is still in use
        text = property->GetValueAsStringWithCheck(property->HasFlag(wxPGFlags::ReadOnly)?wxPGPropValFormatFlags::Null : wxPGPropValFormatFlags::EditableValue);
#else
        text = property->GetValueAsString(property->HasFlag(wxPGFlags::ReadOnly) ? wxPGPropValFormatFlags::Null : wxPGPropValFormatFlags::EditableValue);
#endif // WXWIN_COMPATIBILITY_3_2 | !WXWIN_COMPATIBILITY_3_2

    *psecondary = weakButton.get();
    if ( !completeTransactionIsValid() )
        return nullptr;

    const int maxLength = property->GetMaxLength();
    if ( !completeTransactionIsValid() )
        return nullptr;

    wxWindow* const primary =
        GenerateEditorTextCtrl(pos, sz, text, weakButton.get(), 0, maxLength);
    *psecondary = weakButton.get();
    return primary;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::SetEditorAppearance( const wxPGCell& cell,
                                          bool unspecified )
{
    wxPGProperty* property = GetSelection();
    if ( !property )
        return;
    wxWindow* ctrl = GetEditorControl();
    if ( !ctrl )
        return;

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakGrid(this);
    const wxWeakRef<wxWindow> weakCtrl(ctrl);
    wxPropertyGridPageState* const state = m_pState;
    ++wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
    const wxScopeGuard leaveAppearanceCallback = wxMakeGuard(
        [weakGrid, this]()
        {
            if ( weakGrid.get() == this )
                --wxPGGetPropertyGridTransientState(this).propertyCallbackDepth;
        });
    wxUnusedVar(leaveAppearanceCallback);

    const auto transactionIsValid =
        [weakGrid, weakCtrl, this, ctrl, state, property]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakGrid, this) &&
                   wxWeakWindowIsAvailableForCallbacks(weakCtrl, ctrl) &&
                   m_pState == state &&
                   GetSelection() == property &&
                   !IsPropertyPendingRemoval(property) &&
                   GetEditorControl() == ctrl;
        };

    // Both cells can alias grid-owned storage. Keep their values stable across
    // the application-provided editor callbacks below.
    const wxPGCell appearance = cell;
    const wxPGCell oldAppearance = m_editorAppearance;
    const wxPGEditor* const editor = property->GetEditorClass();
    if ( !transactionIsValid() || !editor )
        return;

    editor->SetControlAppearance(this,
                                 property,
                                 ctrl,
                                 appearance,
                                 oldAppearance,
                                 unspecified);
    if ( !transactionIsValid() )
        return;

    m_editorAppearance = appearance;
}

// -----------------------------------------------------------------------

wxTextCtrl* wxPropertyGrid::GetEditorTextCtrl() const
{
    wxWindow* wnd = GetEditorControl();

    if ( !wnd )
        return nullptr;

    wxTextCtrl* tc = wxDynamicCast(wnd, wxTextCtrl);
    if ( tc )
        return tc;

    wxOwnerDrawnComboBox* cb = wxDynamicCast(wnd, wxOwnerDrawnComboBox);
    if ( cb )
    {
        return cb->GetTextCtrl();
    }

    return nullptr;
}

// -----------------------------------------------------------------------

wxPGEditor* wxPropertyGridInterface::GetEditorByName( const wxString& editorName )
{
    auto it = wxPGGlobalVars->m_mapEditorClasses.find(editorName);
    return it == wxPGGlobalVars->m_mapEditorClasses.end() ? nullptr : it->second;
}

// -----------------------------------------------------------------------
// wxPGEditorDialogAdapter
// -----------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPGEditorDialogAdapter, wxObject);

bool wxPGEditorDialogAdapter::ShowDialog( wxPropertyGrid* propGrid, wxPGProperty* property )
{
    wxCHECK_MSG( propGrid && property, false,
                 "invalid property grid dialog transaction" );

    wxPGDeferredEditorCallbackEpoch deferredEditorCallbackEpoch;
    const wxWeakRef<wxWindow> weakGrid(propGrid);
    ++wxPGGetPropertyGridTransientState(propGrid).propertyCallbackDepth;
    const wxScopeGuard leaveDialogCallback = wxMakeGuard(
        [propGrid, weakGrid]()
        {
            if ( weakGrid.get() == propGrid )
                --wxPGGetPropertyGridTransientState(propGrid).propertyCallbackDepth;
        });
    wxUnusedVar(leaveDialogCallback);

    wxPropertyGridPageState* const state = propGrid->GetState();
    const auto transactionIsValid =
        [propGrid, property, state, &weakGrid]()
        {
            return wxWeakWindowIsAvailableForCallbacks(weakGrid, propGrid) &&
                   propGrid->GetState() == state &&
                   propGrid->GetSelection() == property &&
                   !propGrid->IsPropertyPendingRemoval(property);
        };

    if ( !propGrid->EditorValidate() )
        return false;
    if ( !transactionIsValid() )
        return false;

    bool res = DoShowDialog( propGrid, property );
    if ( !transactionIsValid() )
        return false;

    if ( res )
    {
        propGrid->ValueChangeInEvent( m_value );
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------
// wxPGMultiButton
// -----------------------------------------------------------------------

wxPGMultiButton::wxPGMultiButton( wxPropertyGrid* pg, const wxSize& sz )
    : wxWindow( pg->GetPanel(), wxID_ANY, wxPoint(-100,-100), wxSize(0, sz.y + 2*wxPG_BUTTON_BORDER_WIDTH) ),
      m_fullEditorSize(sz), m_buttonsWidth(0)
{
    SetBackgroundColour(pg->GetCellBackgroundColour());
    SetFont(pg->GetFont().GetBaseFont().Smaller());
}

void wxPGMultiButton::Finalize( wxPropertyGrid* WXUNUSED(propGrid),
                                const wxPoint& pos )
{
    Move( pos.x + m_fullEditorSize.x - m_buttonsWidth, pos.y - wxPG_BUTTON_BORDER_WIDTH, wxSIZE_ALLOW_MINUS_ONE);
}

#if wxUSE_BMPBUTTON

void wxPGMultiButton::Add( const wxBitmapBundle& bitmap, int itemid )
{
    wxSize sz = GetSize();
    wxBitmapButton* button = new wxBitmapButton(this, itemid, bitmap,
                           wxPoint(sz.x, 0), wxSize(wxDefaultCoord, sz.y),
                           wxBU_EXACTFIT);
    // If button is narrow make it a square
    wxSize szBtn = button->GetSize();
    if ( szBtn.x < szBtn.y )
        button->SetSize(wxSize(szBtn.y, szBtn.y));

    DoAddButton( button, sz );
}
#endif

void wxPGMultiButton::Add( const wxString& label, int itemid )
{
    wxSize sz = GetSize();
    wxButton* button = new wxButton(this, itemid, label,
                    wxPoint(sz.x, 0), wxSize(wxDefaultCoord, sz.y), wxBU_EXACTFIT);
    // If button is narrow make it a square
    wxSize szBtn = button->GetSize();
    if ( szBtn.x < szBtn.y )
        button->SetSize(wxSize(szBtn.y, szBtn.y));

    DoAddButton( button, sz );
}

void wxPGMultiButton::DoAddButton( wxWindow* button,
                                   const wxSize& sz )
{
    m_buttons.push_back(button);
    int bw = button->GetSize().x;
    SetSize(wxSize(sz.x+bw,sz.y));
    m_buttonsWidth += bw;
}

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID
