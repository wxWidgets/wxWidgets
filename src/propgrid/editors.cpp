/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/editors.cpp
// Purpose:     wxPropertyGrid editors
// Author:      Jaakko Salli
// Modified by:
// Created:     2007-04-14
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
    #include "wx/dcclient.h"
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


#include "wx/timer.h"
#include "wx/dcbuffer.h"
#include "wx/bmpbuttn.h"


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
    #define wxPG_TEXTCTRL_AND_BUTTON_SPACING        8
#else
    #define wxPG_TEXTCTRL_AND_BUTTON_SPACING        2
#endif 

#define wxPG_BUTTON_SIZEDEC                         0

#include "wx/odcombo.h"

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

// -----------------------------------------------------------------------

#if defined(__WXMSW__)
    // tested
    #define wxPG_NAT_BUTTON_BORDER_ANY          1
    #define wxPG_NAT_BUTTON_BORDER_X            1
    #define wxPG_NAT_BUTTON_BORDER_Y            1

    #define wxPG_CHECKMARK_XADJ                 1
    #define wxPG_CHECKMARK_YADJ                 (-1)
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

    #define wxPG_TEXTCTRLYADJUST                (m_spacingy+0)

#elif defined(__WXGTK__)
    // tested
    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 (-1)
    #define wxPG_CHECKMARK_HADJ                 (-1)
    #define wxPG_CHECKMARK_DEFLATE              3

    #define wxPG_NAT_BUTTON_BORDER_ANY      1
    #define wxPG_NAT_BUTTON_BORDER_X        1
    #define wxPG_NAT_BUTTON_BORDER_Y        1

    #define wxPG_TEXTCTRLYADJUST            0

#elif defined(__WXMAC__)
    // *not* tested
    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLYADJUST            0

#else
    // defaults
    #define wxPG_CHECKMARK_XADJ                 0
    #define wxPG_CHECKMARK_YADJ                 0
    #define wxPG_CHECKMARK_WADJ                 0
    #define wxPG_CHECKMARK_HADJ                 0
    #define wxPG_CHECKMARK_DEFLATE              0

    #define wxPG_NAT_BUTTON_BORDER_ANY      0
    #define wxPG_NAT_BUTTON_BORDER_X        0
    #define wxPG_NAT_BUTTON_BORDER_Y        0

    #define wxPG_TEXTCTRLYADJUST            0

#endif

// for odcombo
#ifdef __WXMAC__
#define wxPG_CHOICEXADJUST           0
#define wxPG_CHOICEYADJUST           -3
#else
#define wxPG_CHOICEXADJUST           0
#define wxPG_CHOICEYADJUST           0
#endif

#define ODCB_CUST_PAINT_MARGIN               6  // Number added to image width for SetCustomPaintWidth

// Milliseconds to wait for two mouse-ups after focus inorder
// to trigger a double-click.
#define DOUBLE_CLICK_CONVERSION_TRESHOLD        500

// -----------------------------------------------------------------------
// wxPGEditor
// -----------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxPGEditor, wxObject)


wxPGEditor::~wxPGEditor()
{
}

wxString wxPGEditor::GetName() const
{
    return GetClassInfo()->GetClassName();
}

void wxPGEditor::DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& text ) const
{
    if ( !property->IsValueUnspecified() )
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
    return;
}


void wxPGEditor::OnFocus( wxPGProperty*, wxWindow* ) const
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
    if ( (property->GetFlags() & wxPG_PROP_NOEDITOR) &&
         property->GetChildCount() )
        return (wxWindow*) NULL;

    if ( !property->IsValueUnspecified() )
        text = property->GetValueString(property->HasFlag(wxPG_PROP_READONLY)?0:wxPG_EDITABLE_VALUE);

    int flags = 0;
    if ( (property->GetFlags() & wxPG_PROP_PASSWORD) &&
         property->IsKindOf(CLASSINFO(wxStringProperty)) )
        flags |= wxTE_PASSWORD;

    wxWindow* wnd = propGrid->GenerateEditorTextCtrl(pos,sz,text,(wxWindow*)NULL,flags,
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
        /*if ( (property->GetFlags() & wxPG_PROP_PASSWORD) &&
             property->IsKindOf(WX_PG_CLASSINFO(wxStringProperty)) )
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

    tc->SetValue(s);    
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

    if ( event.GetEventType() == wxEVT_COMMAND_TEXT_ENTER )
    {
        if ( propGrid->IsEditorsValueModified() )
        {
            return true;
        }
    }
    else if ( event.GetEventType() == wxEVT_COMMAND_TEXT_UPDATED )
    {
        //
        // Pass this event outside wxPropertyGrid so that,
        // if necessary, program can tell when user is editing
        // a textctrl.
        // FIXME: Is it safe to change event id in the middle of event
        //        processing (seems to work, but...)?
        event.Skip();
        event.SetId(propGrid->GetId());

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

    if ( property->UsesAutoUnspecified() && !textVal.length() )
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


void wxPGTextCtrlEditor::SetValueToUnspecified( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxTextCtrl* tc = wxStaticCast(ctrl, wxTextCtrl);

    wxPropertyGrid* pg = property->GetGrid();
    wxASSERT(pg);  // Really, property grid should exist if editor does
    if ( pg )
        tc->SetValue(wxEmptyString);
}


void wxPGTextCtrlEditor::SetControlStringValue( wxPGProperty* property, wxWindow* ctrl, const wxString& txt ) const
{
    wxTextCtrl* tc = wxStaticCast(ctrl, wxTextCtrl);

    wxPropertyGrid* pg = property->GetGrid();
    wxASSERT(pg);  // Really, property grid should exist if editor does
    if ( pg )
        tc->SetValue(txt);
}


void wxPGTextCtrlEditor::OnFocus( wxPGProperty*, wxWindow* wnd ) const
{
    wxTextCtrl* tc = wxStaticCast(wnd, wxTextCtrl);

    tc->SetSelection(-1,-1);
}


wxPGTextCtrlEditor::~wxPGTextCtrlEditor() { }


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

    wxPGDoubleClickProcessor( wxOwnerDrawnComboBox* combo, wxPGProperty* property )
        : wxEvtHandler()
    {
        m_timeLastMouseUp = 0;
        m_combo = combo;
        m_property = property;
        m_downReceived = false;
    }

protected:

    void OnMouseEvent( wxMouseEvent& event )
    {
        wxLongLong t = ::wxGetLocalTimeMillis();
        int evtType = event.GetEventType();

        if ( m_property->HasFlag(wxPG_PROP_USE_DCC) &&
             m_property->IsKindOf(CLASSINFO(wxBoolProperty)) &&
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
                        wxLongLong timeFromLastUp = (t-m_timeLastMouseUp);

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
    wxLongLong                  m_timeLastMouseUp;
    wxOwnerDrawnComboBox*       m_combo;
    wxPGProperty*               m_property;  // Selected property
    bool                        m_downReceived;

    DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxPGDoubleClickProcessor, wxEvtHandler)
    EVT_MOUSE_EVENTS(wxPGDoubleClickProcessor::OnMouseEvent)
    EVT_SET_FOCUS(wxPGDoubleClickProcessor::OnSetFocus)
END_EVENT_TABLE()



class wxPGComboBox : public wxOwnerDrawnComboBox
{
public:

    wxPGComboBox()
        : wxOwnerDrawnComboBox()
    {
        m_dclickProcessor = (wxPGDoubleClickProcessor*) NULL;
        m_sizeEventCalled = false;
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

        m_dclickProcessor = new wxPGDoubleClickProcessor(this, GetGrid()->GetSelection() );

        PushEventHandler(m_dclickProcessor);

        return true;
    }

    virtual void OnDrawItem( wxDC& dc, const wxRect& rect, int item, int flags ) const
    {
        wxPropertyGrid* pg = GetGrid();
        pg->OnComboItemPaint((wxPGCustomComboControl*)this,item,dc,(wxRect&)rect,flags);
    }
    virtual wxCoord OnMeasureItem( size_t item ) const
    {
        wxPropertyGrid* pg = GetGrid();
        wxRect rect;
        rect.x = -1;
        rect.width = 0;
        pg->OnComboItemPaint((wxPGCustomComboControl*)this,item,*((wxDC*)NULL),rect,0);
        return rect.height;
    }

    wxPropertyGrid* GetGrid() const
    {
        wxPropertyGrid* pg = wxDynamicCast(GetParent()->GetParent(),wxPropertyGrid);
        wxASSERT(pg);
        return pg;
    }

    virtual wxCoord OnMeasureItemWidth( size_t item ) const
    {
        wxPropertyGrid* pg = GetGrid();
        wxRect rect;
        rect.x = -1;
        rect.width = -1;
        pg->OnComboItemPaint((wxPGCustomComboControl*)this,item,*((wxDC*)NULL),rect,0);
        return rect.width;
    }

    virtual void PositionTextCtrl( int WXUNUSED(textCtrlXAdjust), int WXUNUSED(textCtrlYAdjust) )
    {
        wxPropertyGrid* pg = GetGrid();
        wxOwnerDrawnComboBox::PositionTextCtrl(
            wxPG_TEXTCTRLXADJUST - (wxPG_XBEFOREWIDGET+wxPG_CONTROL_MARGIN+1) - 1,
            pg->GetSpacingY() + 2
        );
    }

private:
    wxPGDoubleClickProcessor*   m_dclickProcessor;
    bool                        m_sizeEventCalled;
};


void wxPropertyGrid::OnComboItemPaint( wxPGCustomComboControl* pCc,
                                       int item,
                                       wxDC& dc,
                                       wxRect& rect,
                                       int flags )
{
    wxPGComboBox* pCb = (wxPGComboBox*)pCc;

    // Sanity check
    wxASSERT( IsKindOf(CLASSINFO(wxPropertyGrid)) );

    wxPGProperty* p = m_selected;
    wxString text;

    const wxPGChoices& choices = p->GetChoices();
    const wxPGCommonValue* comVal = NULL;
    int comVals = p->GetDisplayedCommonValueCount();
    int comValIndex = -1;

    int choiceCount = 0;
    if ( choices.IsOk() )
        choiceCount = choices.GetCount();

    if ( item >= choiceCount && comVals > 0 )
    {
        comValIndex = item - choiceCount;
        comVal = GetCommonValue(comValIndex);
        if ( !p->IsValueUnspecified() )
            text = comVal->GetLabel();
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
                text = p->GetValueString(0);
        }
    }

    if ( item < 0 )
        return;

    wxSize cis;

    const wxBitmap* itemBitmap = NULL;

    if ( item >= 0 && choices.IsOk() && choices.Item(item).GetBitmap().Ok() && comValIndex == -1 )
        itemBitmap = &choices.Item(item).GetBitmap();

    //
    // Decide what custom image size to use
    if ( itemBitmap )
    {
        cis.x = itemBitmap->GetWidth();
        cis.y = itemBitmap->GetHeight();
    }
    else
    {
        cis = GetImageSize(p, item);
    }

    if ( rect.x < 0 )
    {
        // Default measure behaviour (no flexible, custom paint image only)
        if ( rect.width < 0 )
        {
            wxCoord x, y;
            GetTextExtent(text, &x, &y, 0, 0, &m_font);
            rect.width = cis.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 + 9 + x;
        }

        rect.height = cis.y + 2;
        return;
    }

    wxPGPaintData paintdata;
    paintdata.m_parent = NULL;
    paintdata.m_choiceItem = item;

    // This is by the current (1.0.0b) spec - if painting control, item is -1
    if ( (flags & wxODCB_PAINTING_CONTROL) )
        paintdata.m_choiceItem = -1;

    if ( &dc )
        dc.SetBrush(*wxWHITE_BRUSH);

    if ( rect.x >= 0 )
    {
        //
        // DrawItem call

        wxPoint pt(rect.x + wxPG_CONTROL_MARGIN - wxPG_CHOICEXADJUST - 1,
                   rect.y + 1);

        int renderFlags = 0;

        if ( flags & wxODCB_PAINTING_CONTROL )
            renderFlags |= wxPGCellRenderer::Control;

        if ( flags & wxODCB_PAINTING_SELECTED )
            renderFlags |= wxPGCellRenderer::Selected;

        if ( cis.x > 0 && (p->HasFlag(wxPG_PROP_CUSTOMIMAGE) || !(flags & wxODCB_PAINTING_CONTROL)) &&
             ( !p->m_valueBitmap || item == pCb->GetSelection() ) &&
             ( item >= 0 || (flags & wxODCB_PAINTING_CONTROL) ) &&
             !itemBitmap
           )
        {
            pt.x += wxCC_CUSTOM_IMAGE_MARGIN1;
            wxRect r(pt.x,pt.y,cis.x,cis.y);

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
                wxPGCellRenderer* renderer = cv->GetRenderer();
                r.width = rect.width;
                renderer->Render( dc, r, this, p, m_selColumn, comValIndex, renderFlags );
                return;
            }
            else if ( item >= 0 )
            {
                p->OnCustomPaint( dc, r, paintdata );
            }
            else
            {
                dc.DrawRectangle( r );
            }

            pt.x += paintdata.m_drawnWidth + wxCC_CUSTOM_IMAGE_MARGIN2 - 1;
        }
        else
        {
            // TODO: This aligns text so that it seems to be horizontally
            //       on the same line as property values. Not really
            //       sure if its needed, but seems to not cause any harm.
            pt.x -= 1;

            if ( item < 0 && (flags & wxODCB_PAINTING_CONTROL) )
                item = pCb->GetSelection();

            if ( choices.IsOk() && item >= 0 && comValIndex < 0 )
            {
                const wxPGChoiceEntry& cell = choices.Item(item);
                wxPGCellRenderer* renderer = wxPGGlobalVars->m_defaultRenderer;
                int imageOffset = renderer->PreDrawCell( dc, rect, cell, renderFlags );
                if ( imageOffset )
                    imageOffset += wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2;
                pt.x += imageOffset;
            }
        }

        //
        // Draw text
        //

        pt.y += (rect.height-m_fontHeight)/2 - 1;

        pt.x += 1;

        dc.DrawText( text, pt.x + wxPG_XBEFORETEXT, pt.y );
    }
    else
    {
        //
        // MeasureItem call

        p->OnCustomPaint( dc, rect, paintdata );
        rect.height = paintdata.m_drawnHeight + 2;
        rect.width = cis.x + wxCC_CUSTOM_IMAGE_MARGIN1 + wxCC_CUSTOM_IMAGE_MARGIN2 + 9;
    }
}

bool wxPGChoiceEditor_SetCustomPaintWidth( wxPropertyGrid* propGrid, wxPGComboBox* cb, int cmnVal )
{
    wxPGProperty* property = propGrid->GetSelectedProperty();
    wxASSERT( property );

    if ( cmnVal >= 0 )
    {
        // Yes, a common value is being selected
        property->SetCommonValue( cmnVal );
        wxSize imageSize = propGrid->GetCommonValue(cmnVal)->
                            GetRenderer()->GetImageSize(property, 1, cmnVal);
        if ( imageSize.x ) imageSize.x += ODCB_CUST_PAINT_MARGIN;
        cb->SetCustomPaintWidth( imageSize.x );
        return false;
    }
    else
    {
        wxSize imageSize = propGrid->GetImageSize(property, -1);
        if ( imageSize.x ) imageSize.x += ODCB_CUST_PAINT_MARGIN;
        cb->SetCustomPaintWidth( imageSize.x );
        return true;
    }
}

// CreateControls calls this with CB_READONLY in extraStyle
wxWindow* wxPGChoiceEditor::CreateControlsBase( wxPropertyGrid* propGrid,
                                                wxPGProperty* property,
                                                const wxPoint& pos,
                                                const wxSize& sz,
                                                long extraStyle ) const
{
    const wxPGChoices& choices = property->GetChoices();
    wxString defString;
    int index = property->GetChoiceSelection();

    bool isUnspecified = property->IsValueUnspecified();

    if ( !isUnspecified )
        defString = property->GetDisplayedString();

    wxArrayString labels = choices.GetLabels();

    wxPGComboBox* cb;

    wxPoint po(pos);
    wxSize si(sz);
    po.y += wxPG_CHOICEYADJUST;
    si.y -= (wxPG_CHOICEYADJUST*2);

    po.x += wxPG_CHOICEXADJUST;
    si.x -= wxPG_CHOICEXADJUST;
    wxWindow* ctrlParent = propGrid->GetPanel();

    int odcbFlags = extraStyle | wxBORDER_NONE | wxTE_PROCESS_ENTER;

    //
    // If common value specified, use appropriate index
    unsigned int cmnVals = property->GetDisplayedCommonValueCount();
    if ( cmnVals )
    {
        if ( !isUnspecified )
        {
            int cmnVal = property->GetCommonValue();
            if ( cmnVal >= 0 )
            {
                index = labels.size() + cmnVal;
            }
        }

        unsigned int i;
        for ( i=0; i<cmnVals; i++ )
            labels.Add(propGrid->GetCommonValueLabel(i));
    }

    cb = new wxPGComboBox();
#ifdef __WXMSW__
    cb->Hide();
#endif
    cb->Create(ctrlParent,
               wxPG_SUBID1,
               wxString(),
               po,
               si,
               labels,
               odcbFlags);

    cb->SetButtonPosition(si.y,0,wxRIGHT);
    cb->SetTextIndent(wxPG_XBEFORETEXT-1);

    wxPGChoiceEditor_SetCustomPaintWidth( propGrid, cb, property->GetCommonValue() );

    if ( index >= 0 && index < (int)cb->GetCount() )
    {
        cb->SetSelection( index );
        if ( defString.length() )
            cb->SetText( defString );
    }
    else if ( !(extraStyle & wxCB_READONLY) && defString.length() )
        cb->SetValue( defString );
    else
        cb->SetSelection( -1 );

    // Connect event handling
    wxWindowID id = cb->GetId();
    propGrid->Connect(id, wxEVT_COMMAND_COMBOBOX_SELECTED,
        wxCommandEventHandler(wxPropertyGrid::OnCustomEditorEvent));
    propGrid->Connect(id, wxEVT_COMMAND_TEXT_UPDATED,
        wxCommandEventHandler(wxPropertyGrid::OnCustomEditorEvent));
    propGrid->Connect(id, wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(wxPropertyGrid::OnCustomEditorEvent));

#ifdef __WXMSW__
    cb->Show();
#endif

    return (wxWindow*) cb;
}


void wxPGChoiceEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxASSERT( ctrl );
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    wxASSERT( cb->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)));
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
    wxASSERT( ctrl );
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    wxASSERT( cb->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)));

    if (index < 0)
        index = cb->GetCount();

    return cb->Insert(label,index);
}


void wxPGChoiceEditor::DeleteItem( wxWindow* ctrl, int index ) const
{
    wxASSERT( ctrl );
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    wxASSERT( cb->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)));

    cb->Delete(index);
}

bool wxPGChoiceEditor::OnEvent( wxPropertyGrid* propGrid, wxPGProperty* property,
    wxWindow* ctrl, wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_COMBOBOX_SELECTED )
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
                    cb->GetTextCtrl()->SetValue(wxEmptyString);
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
        return property->IntToValue( variant, index, 0 );
    }
    return false;
}


void wxPGChoiceEditor::SetControlStringValue( wxPGProperty* WXUNUSED(property), wxWindow* ctrl, const wxString& txt ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    wxASSERT( cb );
    cb->SetValue(txt);
}


void wxPGChoiceEditor::SetControlIntValue( wxPGProperty* WXUNUSED(property), wxWindow* ctrl, int value ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    wxASSERT( cb );
    cb->SetSelection(value);
}


void wxPGChoiceEditor::SetValueToUnspecified( wxPGProperty* WXUNUSED(property), wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    cb->SetSelection(-1);
}


bool wxPGChoiceEditor::CanContainCustomImage() const
{
    return true;
}


wxPGChoiceEditor::~wxPGChoiceEditor() { }


// -----------------------------------------------------------------------
// wxPGComboBoxEditor
// -----------------------------------------------------------------------


WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(ComboBox,
                                      wxPGComboBoxEditor,
                                      wxPGChoiceEditor)


void wxPGComboBoxEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    cb->SetValue(property->GetValueString(wxPG_EDITABLE_VALUE));

    // TODO: If string matches any selection, then select that.
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
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*) NULL;
    wxWindow* textCtrl = (wxWindow*) NULL;

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

    if ( property->UsesAutoUnspecified() && !textVal.length() )
    {
        variant.MakeNull();
        return true;
    }

    bool res = property->StringToValue(variant, textVal, wxPG_EDITABLE_VALUE);

    // Changing unspecified always causes event (returning
    // true here should be enough to trigger it).
    if ( !res && variant.IsNull() )
        res = true;

    return res;
}


void wxPGComboBoxEditor::OnFocus( wxPGProperty*, wxWindow* ctrl ) const
{
    wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
    cb->GetTextCtrl()->SetSelection(-1,-1);
}


wxPGComboBoxEditor::~wxPGComboBoxEditor() { }


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

    wxWindow* ch = wxPGEditor_Choice->CreateControls(propGrid,property,
        pos,ch_sz).m_primary;

#ifdef __WXMSW__
    bt->Show();
#endif

    return wxPGWindowList(ch, bt);
}


wxPGChoiceAndButtonEditor::~wxPGChoiceAndButtonEditor() { }


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
        property->GetFlags() & wxPG_PROP_NOEDITOR, property);

    return wxPGWindowList(wnd, wnd2);
}


wxPGTextCtrlAndButtonEditor::~wxPGTextCtrlAndButtonEditor() { }


// -----------------------------------------------------------------------
// wxPGCheckBoxEditor
// -----------------------------------------------------------------------

#if wxPG_INCLUDE_CHECKBOX

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(CheckBox,
                                      wxPGCheckBoxEditor,
                                      wxPGEditor)


// state argument: 0x01 = set if checked
//                 0x02 = set if rectangle should be bold
static void DrawSimpleCheckBox( wxDC& dc, const wxRect& rect, int box_hei, int state, const wxColour& linecol )
{

    // Box rectangle.
    wxRect r(rect.x+wxPG_XBEFORETEXT,rect.y+((rect.height-box_hei)/2),box_hei,box_hei);

    // Draw check mark first because it is likely to overdraw the
    // surrounding rectangle.
    if ( state & 1 )
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

    if ( !(state & 2) )
    {
        // Pen for thin rectangle.
        dc.SetPen(linecol);
    }
    else
    {
        // Pen for bold rectangle.
        wxPen linepen(linecol,2,wxSOLID);
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
    {
        // Due to SetOwnFont stuff necessary for GTK+ 1.2, we need to have this
        SetFont( parent->GetFont() );

        m_state = 0;
        wxPropertyGrid* pg = (wxPropertyGrid*) parent->GetParent();
        wxASSERT( pg->IsKindOf(CLASSINFO(wxPropertyGrid)) );
        m_boxHeight = pg->GetFontHeight();
        SetBackgroundStyle( wxBG_STYLE_COLOUR );
    }

    virtual ~wxSimpleCheckBox();

    virtual bool ProcessEvent(wxEvent& event);

    int m_state;
    int m_boxHeight;

    static wxBitmap* ms_doubleBuffer;

};

wxSimpleCheckBox::~wxSimpleCheckBox()
{
    delete ms_doubleBuffer;
    ms_doubleBuffer = NULL;
}


wxBitmap* wxSimpleCheckBox::ms_doubleBuffer = (wxBitmap*) NULL;

// value = 2 means toggle (sorry, too lazy to do constants)
void wxSimpleCheckBox::SetValue( int value )
{
    if ( value > 1 )
    {
        m_state++;
        if ( m_state > 1 ) m_state = 0;
    }
    else
    {
        m_state = value;
    }
    Refresh();

    wxCommandEvent evt(wxEVT_COMMAND_CHECKBOX_CLICKED,GetParent()->GetId());

    wxPropertyGrid* propGrid = (wxPropertyGrid*) GetParent()->GetParent();
    wxASSERT( propGrid->IsKindOf(CLASSINFO(wxPropertyGrid)) );
    propGrid->OnCustomEditorEvent(evt);
}


bool wxSimpleCheckBox::ProcessEvent(wxEvent& event)
{
    wxPropertyGrid* propGrid = (wxPropertyGrid*) GetParent()->GetParent();
    wxASSERT( propGrid->IsKindOf(CLASSINFO(wxPropertyGrid)) );

    if ( ( (event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK)
          && ((wxMouseEvent&)event).m_x > (wxPG_XBEFORETEXT-2)
          && ((wxMouseEvent&)event).m_x <= (wxPG_XBEFORETEXT-2+m_boxHeight) )
       )
    {
        SetValue(2);
        return true;
    }
    else if ( event.GetEventType() == wxEVT_PAINT )
    {
        wxSize clientSize = GetClientSize();
        wxPaintDC dc(this);

        /*
        // Buffered paint DC doesn't seem to do much good
        if ( !ms_doubleBuffer ||
             clientSize.x > ms_doubleBuffer->GetWidth() ||
             clientSize.y > ms_doubleBuffer->GetHeight() )
        {
            delete ms_doubleBuffer;
            ms_doubleBuffer = new wxBitmap(clientSize.x+25,clientSize.y+25);
        }

        wxBufferedPaintDC dc(this,*ms_doubleBuffer);
        */

        wxRect rect(0,0,clientSize.x,clientSize.y);
        //rect.x -= 1;
        rect.y += 1;
        rect.width += 1;

        m_boxHeight = propGrid->GetFontHeight();

        wxColour bgcol = GetBackgroundColour();
        dc.SetBrush( bgcol );
        dc.SetPen( bgcol );
        dc.DrawRectangle( rect );

        wxColour txcol = GetForegroundColour();

        int state = m_state;
        if ( m_font.GetWeight() == wxBOLD )
            state |= 2;

        DrawSimpleCheckBox(dc,rect,m_boxHeight,state,txcol);

        // If focused, indicate it somehow.
        /*
        if ( wxWindow::FindFocus() == this )
        {
            rect.x += 1;
            rect.width -= 1;

            wxPGDrawFocusRect(dc,rect);
        }
        */

        return true;
    }
    else if ( event.GetEventType() == wxEVT_SIZE ||
              event.GetEventType() == wxEVT_SET_FOCUS ||
              event.GetEventType() == wxEVT_KILL_FOCUS
            )
    {
        Refresh();
    }
    else if ( event.GetEventType() == wxEVT_KEY_DOWN )
    {
        wxKeyEvent& keyEv = (wxKeyEvent&) event;

        if ( keyEv.GetKeyCode() == WXK_SPACE )
        {
            SetValue(2);
            return true;
        }
    }
    return wxControl::ProcessEvent(event);
}


wxPGWindowList wxPGCheckBoxEditor::CreateControls( wxPropertyGrid* propGrid,
                                                   wxPGProperty* property,
                                                   const wxPoint& pos,
                                                   const wxSize& size ) const
{
    wxPoint pt = pos;
    pt.x -= wxPG_XBEFOREWIDGET;
    wxSize sz = size;
    sz.x = propGrid->GetFontHeight() + (wxPG_XBEFOREWIDGET*2) + 4;

    wxSimpleCheckBox* cb = new wxSimpleCheckBox(propGrid->GetPanel(),wxPG_SUBID1,pt,sz);

    cb->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    cb->Connect( wxPG_SUBID1, wxEVT_LEFT_DOWN,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &wxPropertyGrid::OnCustomEditorEvent, NULL, propGrid );

    cb->Connect( wxPG_SUBID1, wxEVT_LEFT_DCLICK,
            (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)
            &wxPropertyGrid::OnCustomEditorEvent, NULL, propGrid );

    if ( property->GetChoiceSelection() > 0 &&
         !property->IsValueUnspecified() )
        cb->m_state = 1;

    // If mouse cursor was on the item, toggle the value now.
    if ( propGrid->GetInternalFlags() & wxPG_FL_ACTIVATION_BY_CLICK )
    {
        wxPoint pt = cb->ScreenToClient(::wxGetMousePosition());
        if ( pt.x <= (wxPG_XBEFORETEXT-2+cb->m_boxHeight) )
        {
            cb->m_state++;

            if ( cb->m_state > 1 )
                cb->m_state = 0;

            // Makes sure wxPG_EVT_CHANGING etc. is sent for this initial click 
            propGrid->ChangePropertyValue(property, wxPGVariant_Bool(cb->m_state));
        }
    }

    propGrid->SetInternalFlag( wxPG_FL_FIXED_WIDTH_EDITOR );

    return cb;
}

/*
class wxPGCheckBoxRenderer : public wxPGDefaultRenderer
{
public:

    virtual void Render( wxDC& dc, const wxRect& rect,
                         const wxPropertyGrid* WXUNUSED(propertyGrid), wxPGProperty* property,
                         int WXUNUSED(column), int WXUNUSED(item), int WXUNUSED(flags) ) const
    {
        int state = 0;
        if ( !(property->GetFlags() & wxPG_PROP_UNSPECIFIED) )
        {
            state = ((wxPGProperty*)property)->GetChoiceInfo((wxPGChoiceInfo*)NULL);
            if ( dc.GetFont().GetWeight() == wxBOLD ) state |= 2;
        }
        DrawSimpleCheckBox(dc,rect,dc.GetCharHeight(),state,dc.GetTextForeground());
    }

protected:
};

wxPGCheckBoxRenderer g_wxPGCheckBoxRenderer;

wxPGCellRenderer* wxPGCheckBoxEditor::GetCellRenderer() const
{
    return &g_wxPGCheckBoxRenderer;
}
*/

void wxPGCheckBoxEditor::DrawValue( wxDC& dc, const wxRect& rect, wxPGProperty* property, const wxString& WXUNUSED(text) ) const
{
    int state = 0;
    if ( !property->IsValueUnspecified() )
    {
        state = property->GetChoiceSelection();
        if ( dc.GetFont().GetWeight() == wxBOLD ) state |= 2;
    }
    DrawSimpleCheckBox(dc,rect,dc.GetCharHeight(),state,dc.GetTextForeground());
}

void wxPGCheckBoxEditor::UpdateControl( wxPGProperty* property, wxWindow* ctrl ) const
{
    wxASSERT( ctrl );
    ((wxSimpleCheckBox*)ctrl)->m_state = property->GetChoiceSelection();
    ctrl->Refresh();
}


bool wxPGCheckBoxEditor::OnEvent( wxPropertyGrid* WXUNUSED(propGrid), wxPGProperty* WXUNUSED(property),
    wxWindow* WXUNUSED(ctrl), wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_CHECKBOX_CLICKED )
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
        return property->IntToValue(variant, index, 0);
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
    ((wxSimpleCheckBox*)ctrl)->m_state = 0;
    ctrl->Refresh();
}


wxPGCheckBoxEditor::~wxPGCheckBoxEditor() { }


#endif // wxPG_INCLUDE_CHECKBOX

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GetEditorControl() const
{
    wxWindow* ctrl = m_wndEditor;

    if ( !ctrl )
        return ctrl;

    return ctrl;
}

// -----------------------------------------------------------------------

void wxPropertyGrid::CorrectEditorWidgetSizeX()
{
    if ( m_selColumn == -1 )
        return;

    int secWid = 0;
    int newSplitterx = m_pState->DoGetSplitterPosition(m_selColumn-1);
    int newWidth = newSplitterx + m_pState->m_colWidths[m_selColumn];

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
        if ( m_wndEditor && m_wndEditor->IsKindOf(CLASSINFO(wxTextCtrl)) )
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
    if ( m_selected && (m_wndEditor || m_wndEditor2) ) 
    {
        wxRect r = GetEditorWidgetRect(m_selected, m_selColumn);

        if ( m_wndEditor )
        {
            wxPoint pos = m_wndEditor->GetPosition();

            // Calculate y offset
            int offset = pos.y % m_lineHeight;

            m_wndEditor->Move(pos.x, r.y + offset);
        }

        if ( m_wndEditor2 )
        {
            wxPoint pos = m_wndEditor2->GetPosition();

            m_wndEditor2->Move(pos.x, r.y);
        }
    }
}

// -----------------------------------------------------------------------

// Fixes position of wxTextCtrl-like control (wxSpinCtrl usually
// fits into that category as well).
void wxPropertyGrid::FixPosForTextCtrl( wxWindow* ctrl, const wxPoint& offset )
{
    // Center the control vertically
    wxRect finalPos = ctrl->GetRect();
    int y_adj = (m_lineHeight - finalPos.height)/2 + wxPG_TEXTCTRLYADJUST;

    // Prevent over-sized control
    int sz_dec = (y_adj + finalPos.height) - m_lineHeight;
    if ( sz_dec < 0 ) sz_dec = 0;

    finalPos.y += y_adj;
    finalPos.height -= (y_adj+sz_dec);

    const int textCtrlXAdjust = wxPG_TEXTCTRLXADJUST;

    finalPos.x += textCtrlXAdjust;
    finalPos.width -= textCtrlXAdjust;

    finalPos.x += offset.x;
    finalPos.y += offset.y;

    ctrl->SetSize(finalPos);
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorTextCtrl( const wxPoint& pos,
                                                  const wxSize& sz,
                                                  const wxString& value,
                                                  wxWindow* secondary,
                                                  int extraStyle,
                                                  int maxLen )
{
    wxWindowID id = wxPG_SUBID1;
    wxPGProperty* selected = m_selected;
    wxASSERT(selected);

    int tcFlags = wxTE_PROCESS_ENTER | extraStyle;

    if ( selected->HasFlag(wxPG_PROP_READONLY) )
        tcFlags |= wxTE_READONLY;

    wxPoint p(pos.x,pos.y);
    wxSize s(sz.x,sz.y);

   // Need to reduce width of text control on Mac
#if defined(__WXMAC__)
    s.x -= 8;
#endif

     // Take button into acccount
    if ( secondary )
    {
        s.x -= (secondary->GetSize().x + wxPG_TEXTCTRL_AND_BUTTON_SPACING);
        m_iFlags &= ~(wxPG_FL_PRIMARY_FILLS_ENTIRE);
    }

    // If the height is significantly higher, then use border, and fill the rect exactly.
    bool hasSpecialSize = false;

    if ( (sz.y - m_lineHeight) > 5 )
        hasSpecialSize = true;

    wxWindow* ctrlParent = GetPanel();

    if ( !hasSpecialSize )
        tcFlags |= wxBORDER_NONE;

    wxTextCtrl* tc = new wxTextCtrl();

#if defined(__WXMSW__)
    tc->Hide();
#endif
    SetupTextCtrlValue(value);
    tc->Create(ctrlParent,id,value, p, s,tcFlags);
    
    wxWindow* ed = tc;

    // Center the control vertically
    if ( !hasSpecialSize )
        FixPosForTextCtrl(ed);

#ifdef __WXMSW__
    ed->Show();
    if ( secondary )
        secondary->Show();
#endif

    // Set maximum length
    if ( maxLen > 0 )
        tc->SetMaxLength( maxLen );

    // Connect event handling
    id = ed->GetId();
    this->Connect(id, wxEVT_COMMAND_TEXT_UPDATED,
        wxCommandEventHandler(wxPropertyGrid::OnCustomEditorEvent));
    this->Connect(id, wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(wxPropertyGrid::OnCustomEditorEvent));

    return (wxWindow*) ed;
}

// -----------------------------------------------------------------------

wxWindow* wxPropertyGrid::GenerateEditorButton( const wxPoint& pos, const wxSize& sz )
{
    wxWindowID id = wxPG_SUBID2;
    wxPGProperty* selected = m_selected;
    wxASSERT(selected);

#ifdef __WXMAC__
   // Decorations are chunky on Mac, and we can't make the button square, so
   // do things a bit differently on this platform.

   wxPoint p(pos.x+sz.x,
             pos.y+wxPG_BUTTON_SIZEDEC-wxPG_NAT_BUTTON_BORDER_Y);
   wxSize s(25, -1);

   wxButton* but = new wxButton();
   but->Create(GetPanel(),id,wxS("..."),p,s,wxWANTS_CHARS);

   // Now that we know the size, move to the correct position
   p.x = pos.x + sz.x - but->GetSize().x - 2;
   but->Move(p);

#else 
    wxSize s(sz.y-(wxPG_BUTTON_SIZEDEC*2)+(wxPG_NAT_BUTTON_BORDER_Y*2),
        sz.y-(wxPG_BUTTON_SIZEDEC*2)+(wxPG_NAT_BUTTON_BORDER_Y*2));

    // Reduce button width to lineheight
    if ( s.x > m_lineHeight )
        s.x = m_lineHeight;

#ifdef __WXGTK__
    // On wxGTK, take fixed button margins into account
    if ( s.x < 25 )
        s.x = 25;
#endif

    wxPoint p(pos.x+sz.x-s.x,
        pos.y+wxPG_BUTTON_SIZEDEC-wxPG_NAT_BUTTON_BORDER_Y);

    wxButton* but = new wxButton();
  #ifdef __WXMSW__
    but->Hide();
  #endif
    but->Create(GetPanel(),id,wxS("..."),p,s,wxWANTS_CHARS);

  #ifdef __WXGTK__
    wxFont font = GetFont();
    font.SetPointSize(font.GetPointSize()-2);
    but->SetFont(font);
  #else
    but->SetFont(GetFont());
  #endif
#endif

    if ( selected->HasFlag(wxPG_PROP_READONLY) )
        but->Disable();

    // Connect event handling
    id = but->GetId();
    this->Connect(id, wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(wxPropertyGrid::OnCustomEditorEvent));

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
        return (wxWindow*) NULL;
    }

    wxString text;

    if ( !property->IsValueUnspecified() )
        text = property->GetValueString(property->HasFlag(wxPG_PROP_READONLY)?0:wxPG_EDITABLE_VALUE);

    return GenerateEditorTextCtrl(pos,sz,text,but,property->m_maxLen);
}

// -----------------------------------------------------------------------

wxTextCtrl* wxPropertyGrid::GetEditorTextCtrl() const
{
    wxWindow* wnd = GetEditorControl();

    if ( !wnd )
        return NULL;

    if ( wnd->IsKindOf(CLASSINFO(wxTextCtrl)) )
        return wxStaticCast(wnd, wxTextCtrl);

    if ( wnd->IsKindOf(CLASSINFO(wxOwnerDrawnComboBox)) )
    {
        wxOwnerDrawnComboBox* cb = wxStaticCast(wnd, wxOwnerDrawnComboBox);
        return cb->GetTextCtrl();
    }

    return NULL;
}

// -----------------------------------------------------------------------

wxPGEditor* wxPropertyGridInterface::GetEditorByName( const wxString& editorName )
{
    wxPGHashMapS2P::const_iterator it;

    it = wxPGGlobalVars->m_mapEditorClasses.find(editorName);
    if ( it == wxPGGlobalVars->m_mapEditorClasses.end() )
        return NULL;
    return (wxPGEditor*) it->second;
}

// -----------------------------------------------------------------------
// wxPGEditorDialogAdapter
// -----------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxPGEditorDialogAdapter, wxObject)

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
    : wxWindow( pg->GetPanel(), wxPG_SUBID2, wxPoint(-100,-100), wxSize(0, sz.y) ),
      m_fullEditorSize(sz), m_buttonsWidth(0)
{
    SetBackgroundColour(pg->GetCellBackgroundColour());
}

void wxPGMultiButton::Finalize( wxPropertyGrid* propGrid, const wxPoint& pos )
{
    Move( pos.x + m_fullEditorSize.x - m_buttonsWidth, pos.y );

    // Connect event handling
    for ( unsigned int i=0; i<GetCount(); i++ )
    {
        wxWindowID id = GetButtonId(i);
        propGrid->Connect(id, wxEVT_COMMAND_BUTTON_CLICKED,
            wxCommandEventHandler(wxPropertyGrid::OnCustomEditorEvent));
    }
}

int wxPGMultiButton::GenId( int id ) const
{
    if ( id < -1 )
    {
        if ( m_buttons.size() )
            id = GetButton(m_buttons.size()-1)->GetId() + 1;
        else
            id = wxPG_SUBID2;
    }
    return id;
}

#if wxUSE_BMPBUTTON
void wxPGMultiButton::Add( const wxBitmap& bitmap, int id )
{
    id = GenId(id);
    wxSize sz = GetSize();
    wxButton* button = new wxBitmapButton( this, id, bitmap, wxPoint(sz.x, 0), wxSize(sz.y, sz.y) );
    m_buttons.push_back(button);
    int bw = button->GetSize().x;
    SetSize(wxSize(sz.x+bw,sz.y));
    m_buttonsWidth += bw;
}
#endif

void wxPGMultiButton::Add( const wxString& label, int id )
{
    id = GenId(id);
    wxSize sz = GetSize();
    wxButton* button = new wxButton( this, id, label, wxPoint(sz.x, 0), wxSize(sz.y, sz.y) );
    m_buttons.push_back(button);
    int bw = button->GetSize().x;
    SetSize(wxSize(sz.x+bw,sz.y));
    m_buttonsWidth += bw;
}

// -----------------------------------------------------------------------

#endif  // wxUSE_PROPGRID
