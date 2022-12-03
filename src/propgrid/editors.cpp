/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/editors.cpp
// Purpose:     wxPropertyGrid editors
// Author:      Jaakko Salli
// Modified by:
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

// This define is necessary to prevent macro clearing
#define __wxPG_SOURCE_FILE__

#include "wx/propgrid/propgrid.h"
#include "wx/propgrid/editors.h"
#include "wx/propgrid/props.h"

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

// -----------------------------------------------------------------------
// wxPGEditor
// -----------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPGEditor, wxObject);


wxPGEditor::~wxPGEditor()
{
}

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
    wxTextCtrl* tc = nullptr;
    wxComboCtrl* cb = nullptr;
    if ( wxDynamicCast(ctrl, wxTextCtrl) )
    {
        tc = (wxTextCtrl*) ctrl;
    }
    else
    {
        if ( wxDynamicCast(ctrl, wxComboCtrl) )
        {
            cb = (wxComboCtrl*) ctrl;
            tc = cb->GetTextCtrl();
        }
    }

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
            tcText = property->GetValueAsString(
                property->HasFlag(wxPG_PROP_READONLY)?0:wxPG_EDITABLE_VALUE);
            changeText = true;
        }

        if ( changeText )
        {
            // This prevents value from being modified
            if ( tc )
            {
                pg->SetupTextCtrlValue(tcText);
                tc->SetValue(tcText);
            }
            else
            {
                cb->SetText(tcText);
            }
        }
    }

    // Do not make the mistake of calling GetClassDefaultAttributes()
    // here. It is static, while GetDefaultAttributes() is virtual
    // and the correct one to use.
    wxVisualAttributes vattrs = ctrl->GetDefaultAttributes();

    // Foreground colour
    const wxColour& fgCol = cell.GetFgCol();
    if ( fgCol.IsOk() )
    {
        ctrl->SetForegroundColour(fgCol);
    }
    else if ( oCell.GetFgCol().IsOk() )
    {
        ctrl->SetForegroundColour(vattrs.colFg);
    }

    // Background colour
    const wxColour& bgCol = cell.GetBgCol();
    if ( bgCol.IsOk() )
    {
        ctrl->SetBackgroundColour(bgCol);
    }
    else if ( oCell.GetBgCol().IsOk() )
    {
        ctrl->SetBackgroundColour(vattrs.colBg);
    }

    // Font
    const wxFont& font = cell.GetFont();
    if ( font.IsOk() )
    {
        ctrl->SetFont(font);
    }
    else if ( oCell.GetFont().IsOk() )
    {
        ctrl->SetFont(vattrs.font);
    }

    // Also call the old SetValueToUnspecified()
    if ( unspecified )
        SetValueToUnspecified(property, ctrl);
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
    wxString text;

    //
    // If has children, and limited editing is specified, then don't create.
    if ( property->HasFlag(wxPG_PROP_NOEDITOR) &&
         property->HasAnyChild() )
        return nullptr;

    int argFlags = 0;
    if ( !property->HasFlag(wxPG_PROP_READONLY) &&
         !property->IsValueUnspecified() )
        argFlags |= wxPG_EDITABLE_VALUE;
    text = property->GetValueAsString(argFlags);

    int flags = 0;
    if ( property->HasFlag(wxPG_PROP_PASSWORD) &&
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
        /*if ( property->HasFlag(wxPG_PROP_PASSWORD) &&
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

    wxString s;

    if ( tc->HasFlag(wxTE_PASSWORD) )
        s = property->GetValueAsString(wxPG_FULL_VALUE);
    else
        s = property->GetDisplayedString();

    wxPropertyGrid* pg = property->GetGrid();

    pg->SetupTextCtrlValue(s);
    tc->SetValue(s);

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

    bool res = property->StringToValue(variant, textVal, wxPG_EDITABLE_VALUE);

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
    // Make sure there is correct text (instead of unspecified value
    // indicator or hint text)
    int flags = property->HasFlag(wxPG_PROP_READONLY) ?
        0 : wxPG_EDITABLE_VALUE;
    wxString correctText = property->GetValueAsString(flags);

    if ( tc->GetValue() != correctText )
    {
        property->GetGrid()->SetupTextCtrlValue(correctText);
        tc->SetValue(correctText);
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

    wxPGDoubleClickProcessor( wxOwnerDrawnComboBox* combo, wxBoolProperty* property )
        : wxEvtHandler()
        , m_timeLastMouseUp(0)
        , m_combo(combo)
        , m_property(property)
        , m_downReceived(false)
    {
    }

protected:

    void OnMouseEvent( wxMouseEvent& event )
    {
        wxMilliClock_t t = ::wxGetLocalTimeMillis();
        wxEventType evtType = event.GetEventType();

        if ( m_property->HasFlag(wxPG_PROP_USE_DCC) &&
             !m_combo->IsPopupShown() )
        {
            // Just check that it is in the text area
            wxPoint pt = event.GetPosition();
            if ( m_combo->GetTextRect().Contains(pt) )
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
    wxOwnerDrawnComboBox*       m_combo;
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
    {
    }

    ~wxPGComboBox()
    {
        if ( m_dclickProcessor )
        {
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
        if ( !wxOwnerDrawnComboBox::Create( parent,
                                            id,
                                            value,
                                            pos,
                                            size,
                                            choices,
                                            style,
                                            validator,
                                            name ) )
            return false;

        // Enabling double-click processor makes sense
        // only for wxBoolProperty.
        m_selProp = GetGrid()->GetSelection();
        wxASSERT(m_selProp);
        wxBoolProperty* boolProp = wxDynamicCast(m_selProp, wxBoolProperty);
        if ( boolProp )
        {
            m_dclickProcessor = new wxPGDoubleClickProcessor(this, boolProp);
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
        wxRect rect;
        rect.x = -1;
        rect.width = 0;
        pg->OnComboItemPaint( this, item, nullptr, rect, 0 );
        return rect.height;
    }

    wxPropertyGrid* GetGrid() const
    {
        wxPropertyGrid* pg = wxDynamicCast(GetParent(),
                                           wxPropertyGrid);
        wxASSERT(pg);
        return pg;
    }

    virtual wxCoord OnMeasureItemWidth( size_t item ) const override
    {
        wxPropertyGrid* pg = GetGrid();
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
                          (wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1) - 1,
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
};


void wxPropertyGrid::OnComboItemPaint( const wxPGComboBox* pCb,
                                       int item,
                                       wxDC* pDc,
                                       wxRect& rect,
                                       int flags )
{
    wxPGProperty* p = pCb->GetProperty();

    wxString text;

    const wxPGChoices& choices = p->GetChoices();
    int comValIndex = -1;

    const int choiceCount = choices.IsOk()? choices.GetCount(): 0;
    if ( item >= choiceCount && p->GetDisplayedCommonValueCount() > 0 )
    {
        comValIndex = item - choiceCount;
        if ( !p->IsValueUnspecified() || !(flags & wxODCB_PAINTING_CONTROL) )
        {
            const wxPGCommonValue* cv = GetCommonValue(comValIndex);
            text = cv->GetLabel();
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
                text = p->GetValueAsString(0);
        }
    }

    if ( item < 0 )
        return;

    wxBitmap itemBitmap;

    if ( comValIndex == -1 && choices.IsOk() && choices.Item(item).GetBitmap().IsOk() )
        itemBitmap = choices.Item(item).GetBitmap().GetBitmapFor(this);

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
        cis = GetImageSize(p, item);
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

        // If wxPG_PROP_CUSTOMIMAGE was set, then that means any custom
        // image will not appear on the control row (it may be too
        // large to fit, for instance). Also do not draw custom image
        // if no choice was selected.
        if ( !p->HasFlag(wxPG_PROP_CUSTOMIMAGE) )
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
    if ( p->GetValueImage() && item != pCb->GetSelection() )
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
        }

        pt.x += paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN2 - 1;
    }
    else
    {
        // TODO: This aligns text so that it seems to be horizontally
        //       on the same line as property values. Not really
        //       sure if it is needed, but seems to not cause any harm.
        pt.x -= 1;

        if ( choices.IsOk() && comValIndex < 0 )
        {
            // This aligns bitmap horizontally so that it is
            // on the same position as bitmap drawn for static content
            // (without editor).
            wxRect r(rect);
            r.x -= 1;

            cell = &choices.Item(item);
            renderer = wxPGGlobalVars->m_defaultRenderer;
            int imageOffset = renderer->PreDrawCell(dc, r, this, *cell,
                                                    renderFlags );
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
    // Since it is not possible (yet) to create a read-only combo box in
    // the same sense that wxTextCtrl is read-only, simply do not create
    // the control in this case.
    if ( property->HasFlag(wxPG_PROP_READONLY) )
        return nullptr;

    const wxPGChoices& choices = property->GetChoices();
    wxString defString;
    int index = property->GetChoiceSelection();

    int argFlags = 0;
    if ( !property->HasFlag(wxPG_PROP_READONLY) &&
         !property->IsValueUnspecified() )
        argFlags |= wxPG_EDITABLE_VALUE;
    defString = property->GetValueAsString(argFlags);

    wxArrayString labels = choices.GetLabels();

    wxPoint po(pos);
    wxSize si(sz);
    po.y += wxPG_CHOICEYADJUST;
    si.y -= (wxPG_CHOICEYADJUST*2);

    po.x += wxPG_CHOICEXADJUST;
    si.x -= wxPG_CHOICEXADJUST;
    wxWindow* ctrlParent = propGrid->GetPanel();

    int odcbFlags = extraStyle | wxBORDER_NONE | wxTE_PROCESS_ENTER;

    if ( property->HasFlag(wxPG_PROP_USE_DCC) &&
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

    wxPGComboBox* cb = new wxPGComboBox();
#ifdef __WXMSW__
    cb->Hide();
#endif
    cb->Create(ctrlParent,
               wxID_ANY,
               wxString(),
               po,
               si,
               labels,
               odcbFlags);

    // Under OSX default button seems to look fine
    // so there is no need to change it.
#ifndef __WXOSX__
    cb->SetButtonPosition(si.y,0,wxRIGHT);
#endif // !__WXOSX__
    cb->SetMargins(wxPG_XBEFORETEXT-1);

    cb->SetBackgroundColour(propGrid->GetCellBackgroundColour());

    // Set hint text
    cb->SetHint(property->GetHintText());

    wxPGChoiceEditor_SetCustomPaintWidth( propGrid, cb,
                                          property->GetCommonValue() );

    if ( index >= 0 && index < (int)cb->GetCount() )
    {
        cb->SetSelection( index );
        if ( !defString.empty() )
            cb->SetText( defString );
    }
    else if ( !(extraStyle & wxCB_READONLY) && !defString.empty() )
    {
        propGrid->SetupTextCtrlValue(defString);
        cb->SetValue( defString );
    }
    else
    {
        cb->SetSelection( -1 );
    }

#ifdef __WXMSW__
    cb->Show();
#endif

    return (wxWindow*) cb;
}


void wxPGChoiceEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = wxDynamicCast(ctrl, wxOwnerDrawnComboBox);
    wxCHECK_RET(cb, "Only wxOwnerDrawnComboBox editor can be updated");

    int ind = property->GetChoiceSelection();
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
                    propGrid->SetInternalFlag(wxPG_FL_VALUE_CHANGE_IN_EVENT);
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
        return property->IntToValue(variant, index, wxPG_PROPERTY_SPECIFIC);
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
    const int index = property->GetChoiceSelection();
    wxString s = property->GetValueAsString(wxPG_EDITABLE_VALUE);
    cb->SetSelection(index);
    property->GetGrid()->SetupTextCtrlValue(s);
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
    wxOwnerDrawnComboBox* cb = nullptr;
    wxWindow* textCtrl = nullptr;

    if ( ctrl )
    {
        cb = (wxOwnerDrawnComboBox*)ctrl;
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

    bool res = property->StringToValue(variant, textVal, wxPG_EDITABLE_VALUE|wxPG_PROPERTY_SPECIFIC);

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

    // Size of choice.
    wxSize ch_sz(sz.x-bt->GetSize().x,sz.y);

#ifdef __WXMAC__
    ch_sz.x -= wxPG_TEXTCTRL_AND_BUTTON_SPACING;
#endif

    wxWindow* ch = wxPGChoiceEditor::CreateControls(propGrid,property,
        pos,ch_sz).GetPrimary();

#ifdef __WXMSW__
    bt->Show();
#endif

    return wxPGWindowList(ch, bt);
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
        property->HasFlag(wxPG_PROP_NOEDITOR), property);

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
enum
{
    wxSCB_STATE_UNCHECKED   = 0,
    wxSCB_STATE_CHECKED     = 1,
    wxSCB_STATE_BOLD        = 2,
    wxSCB_STATE_UNSPECIFIED = 4
};

const int wxSCB_SETVALUE_CYCLE = 2;

static void DrawSimpleCheckBox(wxWindow* win, wxDC& dc, const wxRect& rect, int state)
{
#if wxPG_USE_RENDERER_NATIVE

    int cbFlags = 0;
    if ( state & wxSCB_STATE_UNSPECIFIED )
    {
        cbFlags |= wxCONTROL_UNDETERMINED;
    }
    else if ( state & wxSCB_STATE_CHECKED )
    {
        cbFlags |= wxCONTROL_CHECKED;
    }

    if ( state & wxSCB_STATE_BOLD )
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

    wxRendererNative::Get().DrawCheckBox(win, dc, rect, cbFlags);
#else
    wxUnusedVar(win);

    wxColour useCol = dc.GetTextForeground();

    if ( state & wxSCB_STATE_UNSPECIFIED )
    {
        useCol = wxColour(220, 220, 220);
    }

    wxRect r(rect);
    // Draw check mark first because it is likely to overdraw the
    // surrounding rectangle.
    if ( state & wxSCB_STATE_CHECKED )
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

    if ( !(state & wxSCB_STATE_BOLD) )
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

    wxSimpleCheckBox( wxWindow* parent,
                      wxWindowID id,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize )
        : wxControl(parent,id,pos,size,wxBORDER_NONE|wxWANTS_CHARS)
        , m_state(0)
    {
        // Due to SetOwnFont stuff necessary for GTK+ 1.2, we need to have this
        wxControl::SetFont( parent->GetFont() );

        SetBoxHeight(12);
        wxControl::SetBackgroundStyle( wxBG_STYLE_PAINT );
    }

    virtual ~wxSimpleCheckBox();


    void SetBoxHeight(int height)
    {
        m_boxHeight = height;
        // Box rectangle
        wxRect rect(GetClientSize());
        rect.y += 1;
        rect.width += 1;
        m_boxRect = GetBoxRect(rect, m_boxHeight);
    }

    static wxRect GetBoxRect(const wxRect& r, int box_h)
    {
        return wxRect(r.x + wxPG_XBEFORETEXT, r.y + ((r.height - box_h) / 2), box_h, box_h);
    }

    int m_state;

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

wxSimpleCheckBox::~wxSimpleCheckBox()
{
}

void wxSimpleCheckBox::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxAutoBufferedPaintDC dc(this);

    wxColour bgcol = GetBackgroundColour();
    dc.SetBackground(wxBrush(bgcol));
    dc.Clear();
    dc.SetBrush( bgcol );
    dc.SetPen( bgcol );

    dc.SetTextForeground(GetForegroundColour());

    int state = m_state;
    if ( !(state & wxSCB_STATE_UNSPECIFIED) &&
         GetFont().GetWeight() == wxFONTWEIGHT_BOLD )
        state |= wxSCB_STATE_BOLD;

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
    if ( value == wxSCB_SETVALUE_CYCLE )
    {
        m_state ^= wxSCB_STATE_CHECKED;
    }
    else
    {
        m_state = value;
    }
    Refresh();

    wxCommandEvent evt(wxEVT_CHECKBOX,GetParent()->GetId());

    wxPropertyGrid* propGrid = (wxPropertyGrid*) GetParent();
    wxASSERT( wxDynamicCast(propGrid, wxPropertyGrid) );
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
    if ( property->HasFlag(wxPG_PROP_READONLY) )
        return nullptr;

    wxPoint pt = pos;
    pt.x -= wxPG_XBEFOREWIDGET;
    wxSize sz = size;
    sz.x = propGrid->GetFontHeight() + (wxPG_XBEFOREWIDGET*2) + 4;

    wxSimpleCheckBox* cb = new wxSimpleCheckBox(propGrid->GetPanel(),
                                                wxID_ANY, pt, sz);

    cb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    UpdateControl(property, cb);

    if ( !property->IsValueUnspecified() )
    {
        if ( propGrid->HasInternalFlag(wxPG_FL_ACTIVATION_BY_CLICK) )
        {
            // Send the event to toggle the value (if mouse cursor is on the item)
            wxPoint point = cb->ScreenToClient(::wxGetMousePosition());
            wxCommandEvent *evt = new wxCommandEvent(wxEVT_CB_LEFT_CLICK_ACTIVATE, cb->GetId());
            // Store mouse pointer position
            evt->SetInt(point.x);
            evt->SetExtraLong(point.y);
            wxQueueEvent(cb, evt);
        }
    }

    propGrid->SetInternalFlag( wxPG_FL_FIXED_WIDTH_EDITOR );

    return cb;
}

void wxPGCheckBoxEditor::DrawValue( wxDC& dc, const wxRect& rect,
                                    wxPGProperty* property,
                                    const wxString& WXUNUSED(text) ) const
{
    int state = wxSCB_STATE_UNCHECKED;

    if ( !property->IsValueUnspecified() )
    {
        state = property->GetChoiceSelection();
        if ( dc.GetFont().GetWeight() == wxFONTWEIGHT_BOLD )
            state |= wxSCB_STATE_BOLD;
    }
    else
    {
        state |= wxSCB_STATE_UNSPECIFIED;
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

    if ( !property->IsValueUnspecified() )
        cb->m_state = property->GetChoiceSelection();
    else
        cb->m_state = wxSCB_STATE_UNSPECIFIED;

    wxPropertyGrid* propGrid = property->GetGrid();
    cb->SetBoxHeight(propGrid->GetFontHeight());

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

    int index = cb->m_state;

    if ( index != property->GetChoiceSelection() ||
         // Changing unspecified always causes event (returning
         // true here should be enough to trigger it).
         property->IsValueUnspecified()
       )
    {
        return property->IntToValue(variant, index, wxPG_PROPERTY_SPECIFIC);
    }
    return false;
}


void wxPGCheckBoxEditor::SetControlIntValue( wxPGProperty* WXUNUSED(property), wxWindow* ctrl, int value ) const
{
    if ( value != 0 ) value = 1;
    ((wxSimpleCheckBox*)ctrl)->m_state = value;
    ctrl->Refresh();
}


void wxPGCheckBoxEditor::SetValueToUnspecified( wxPGProperty* WXUNUSED(property), wxWindow* ctrl ) const
{
    ((wxSimpleCheckBox*)ctrl)->m_state = wxSCB_STATE_UNSPECIFIED;
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
    int secWid = 0;

    // Use fixed selColumn 1 for main editor widgets
    int newSplitterx;
    CalcScrolledPosition(m_pState->DoGetSplitterPosition(0), 0, &newSplitterx, nullptr);
    int newWidth = newSplitterx + m_pState->GetColumnWidth(1);

    if ( m_wndEditor2 )
    {
        // if width change occurred, move secondary wnd by that amount
        wxRect r = m_wndEditor2->GetRect();
        secWid = r.width;
        r.x = newWidth - secWid;

        m_wndEditor2->SetSize( r );

        // if primary is textctrl, then we have to add some extra space
#ifdef __WXMAC__
        if ( m_wndEditor )
#else
        if ( wxDynamicCast(m_wndEditor, wxTextCtrl) )
#endif
            secWid += wxPG_TEXTCTRL_AND_BUTTON_SPACING;
    }

    if ( m_wndEditor )
    {
        wxRect r = m_wndEditor->GetRect();

        r.x = newSplitterx+m_ctrlXAdjust;

        if ( !(m_iFlags & wxPG_FL_FIXED_WIDTH_EDITOR) )
            r.width = newWidth - r.x - secWid;

        m_wndEditor->SetSize(r);
    }

    if ( m_wndEditor2 )
        m_wndEditor2->Refresh();
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CorrectEditorWidgetPosY()
{
    wxPGProperty* selected = GetSelection();

    if ( selected )
    {
        if ( m_labelEditor )
        {
            wxRect r = GetEditorWidgetRect(selected, m_selColumn);
            m_labelEditor->Move(r.GetPosition() + m_labelEditorPosRel);
        }

        if ( m_wndEditor || m_wndEditor2 )
        {
            wxRect r = GetEditorWidgetRect(selected, 1);

            if ( m_wndEditor )
            {
                m_wndEditor->Move(r.GetPosition() + m_wndEditorPosRel);
            }

            if ( m_wndEditor2 )
            {
                m_wndEditor2->Move(r.GetPosition() + m_wndEditor2PosRel);
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
    wxPGProperty* prop = GetSelection();
    wxASSERT(prop);

    int tcFlags = wxTE_PROCESS_ENTER | extraStyle;

    if ( prop->HasFlag(wxPG_PROP_READONLY) && forColumn == 1 )
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

#if defined(__WXMSW__)
    tc->Hide();
#endif
    SetupTextCtrlValue(value);
    tc->Create(ctrlParent,wxID_ANY,value, p, s,tcFlags);

#if defined(__WXMSW__)
    // On Windows, we need to override read-only text ctrl's background
    // colour to white. One problem with native 'grey' background is that
    // tc->GetBackgroundColour() doesn't seem to return correct value
    // for it.
    if ( tcFlags & wxTE_READONLY )
    {
        wxVisualAttributes vattrs = tc->GetDefaultAttributes();
        tc->SetBackgroundColour(vattrs.colBg);
    }
#endif

    // This code is repeated from DoSelectProperty(). However, font boldness
    // must be set before margin is set up below in FixPosForTextCtrl().
    if ( forColumn == 1 &&
         prop->HasFlag(wxPG_PROP_MODIFIED) &&
         HasFlag(wxPG_BOLD_MODIFIED) )
         tc->SetFont( m_captionFont );

    // Center the control vertically
    if ( !hasSpecialSize )
        FixPosForTextCtrl(tc, forColumn);

    if ( forColumn != 1 )
    {
        tc->SetBackgroundColour(m_colSelBack);
        tc->SetForegroundColour(m_colSelFore);
    }

#ifdef __WXMSW__
    tc->Show();
    if ( secondary )
        secondary->Show();
#endif

    // Set maximum length
    if ( maxLen > 0 )
        tc->SetMaxLength( maxLen );

    wxVariant attrVal = prop->GetAttribute(wxPG_ATTR_AUTOCOMPLETE);
    if ( !attrVal.IsNull() )
    {
        wxASSERT(attrVal.IsType(wxPG_VARIANT_TYPE_ARRSTRING));
        tc->AutoComplete(attrVal.GetArrayString());
    }

    // Set hint text
    tc->SetHint(prop->GetHintText());

    return tc;
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorButton( const wxPoint& pos, const wxSize& sz )
{
    wxPGProperty* selected = GetSelection();
    wxASSERT(selected);

    const wxString label = wxString::FromUTF8("\xe2\x80\xa6"); // "Horizontal ellipsis" character

    int dim = sz.y + 2*wxPG_BUTTON_BORDER_WIDTH;

    wxPoint p(pos.x + sz.x, pos.y - wxPG_BUTTON_BORDER_WIDTH);
    wxSize s(wxDefaultCoord, dim);

    wxButton* but = new wxButton();
  #ifdef __WXMSW__
    but->Hide();
  #endif
    but->Create(GetPanel(),wxID_ANY,label,p,s,wxWANTS_CHARS|wxBU_EXACTFIT);
    but->SetFont(GetFont().GetBaseFont().Smaller());
    // If button is narrow make it a square and move it to the correct position
    s = but->GetSize();
    if ( s.x < s.y )
    {
        s.x = s.y;
        but->SetSize(s);
    }
    p.x = pos.x + sz.x - s.x;
    but->Move(p);

    if ( selected->HasFlag(wxPG_PROP_READONLY) && !selected->HasFlag(wxPG_PROP_ACTIVE_BTN) )
        but->Disable();

    return but;
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorTextCtrlAndButton( const wxPoint& pos,
                                                           const wxSize& sz,
                                                           wxWindow** psecondary,
                                                           int limitedEditing,
                                                           wxPGProperty* property )
{
    wxButton* but = (wxButton*)GenerateEditorButton(pos,sz);
    *psecondary = (wxWindow*)but;

    if ( limitedEditing )
    {
    #ifdef __WXMSW__
        // There is button Show in GenerateEditorTextCtrl as well
        but->Show();
    #endif
        return nullptr;
    }

    wxString text;

    if ( !property->IsValueUnspecified() )
        text = property->GetValueAsString(property->HasFlag(wxPG_PROP_READONLY)?0:wxPG_EDITABLE_VALUE);

    return GenerateEditorTextCtrl(pos,sz,text,but,property->GetMaxLength());
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

    property->GetEditorClass()->SetControlAppearance( this,
                                                      property,
                                                      ctrl,
                                                      cell,
                                                      m_editorAppearance,
                                                      unspecified );

    m_editorAppearance = cell;
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
    wxPGHashMapS2P::const_iterator it;

    it = wxPGGlobalVars->m_mapEditorClasses.find(editorName);
    if ( it == wxPGGlobalVars->m_mapEditorClasses.end() )
        return nullptr;
    return (wxPGEditor*) it->second;
}

// -----------------------------------------------------------------------
// wxPGEditorDialogAdapter
// -----------------------------------------------------------------------

wxIMPLEMENT_ABSTRACT_CLASS(wxPGEditorDialogAdapter, wxObject);

bool wxPGEditorDialogAdapter::ShowDialog( wxPropertyGrid* propGrid, wxPGProperty* property )
{
    if ( !propGrid->EditorValidate() )
        return false;

    bool res = DoShowDialog( propGrid, property );

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

int wxPGMultiButton::GenId( int itemid ) const
{
    return itemid < -1 ? wxID_ANY : itemid;
}

#if wxUSE_BMPBUTTON

#if defined(__WXGTK__)
// Dedicated wxBitmapButton with reduced internal borders
#include "wx/gtk/private.h"

class wxPGEditorBitmapButton : public wxBitmapButton
{
public:
    wxPGEditorBitmapButton(wxWindow *parent, wxWindowID id,
                     const wxBitmap& bitmap, const wxPoint& pos,
                     const wxSize& size, long style = 0)
        : wxBitmapButton(parent, id, bitmap, pos, size, style)
    {
#if defined(__WXGTK3__)
        GTKApplyCssStyle("*{ padding:0 }");
#else
        // Define a special button style without inner border
        // if it's not yet done.
        if ( !m_exactFitStyleDefined )
        {
            gtk_rc_parse_string(
              "style \"wxPGEditorBitmapButton_style\"\n"
              "{ GtkButton::inner-border = { 0, 0, 0, 0 } }\n"
              "widget \"*wxPGEditorBitmapButton*\" style \"wxPGEditorBitmapButton_style\"\n"
            );
            m_exactFitStyleDefined = true;
        }

        // Assign the button to the GTK style without inner border.
        gtk_widget_set_name(m_widget, "wxPGEditorBitmapButton");
#endif
    }

    virtual ~wxPGEditorBitmapButton() = default;

private:
#ifndef __WXGTK3__
    // To mark if special GTK style was already defined.
    static bool m_exactFitStyleDefined;
#endif // !__WXGTK3__
};

#ifndef __WXGTK3__
bool wxPGEditorBitmapButton::m_exactFitStyleDefined = false;
#endif // !__WXGTK3__

#else // !__WXGTK__

typedef wxBitmapButton wxPGEditorBitmapButton;

#endif // __WXGTK__ / !__WXGTK__

void wxPGMultiButton::Add( const wxBitmapBundle& bitmap, int itemid )
{
    itemid = GenId(itemid);
    wxSize sz = GetSize();

    // Internal margins around the bitmap inside the button
    const int margins =
#if defined(__WXMSW__)
            2*4;
#elif defined(__WXGTK3__)
            2*2;
#elif defined(__WXGTK__)
            2*6;
#elif defined(__WXOSX__)
            2*3;
#else
            0;
#endif
    // Maximal heigth of the bitmap
    const int hMax = wxMax(4, sz.y - margins);

    wxBitmap bmp = bitmap.GetBitmapFor(this);
    wxBitmap scaledBmp;
    // Scale bitmap down if necessary
    if ( bmp.GetHeight() > hMax )
    {
        double scale = (double)hMax / bmp.GetHeight();
        scaledBmp = wxPropertyGrid::RescaleBitmap(bmp, scale, scale);
    }
    else
    {
        scaledBmp = bmp;
    }

    wxBitmapButton* button = new wxPGEditorBitmapButton(this, itemid, scaledBmp,
                           wxPoint(sz.x, 0), wxSize(wxDefaultCoord, sz.y));
    // If button is narrow make it a square
    wxSize szBtn = button->GetSize();
    if ( szBtn.x < szBtn.y )
        button->SetSize(wxSize(szBtn.y, szBtn.y));

    DoAddButton( button, sz );
}
#endif

void wxPGMultiButton::Add( const wxString& label, int itemid )
{
    itemid = GenId(itemid);
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
