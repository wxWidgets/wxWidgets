/////////////////////////////////////////////////////////////////////////////
// Name:        src/propgrid/advprops.cpp
// Purpose:     wxPropertyGrid Advanced Properties (font, colour, etc.)
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
    #include "wx/dc.h"
    #include "wx/settings.h"
    #include "wx/textctrl.h"
    #include "wx/wxcrtvararg.h"
#endif

#include "wx/propgrid/propgrid.h"

#if wxPG_INCLUDE_ADVPROPS

#include "wx/propgrid/advprops.h"

#include "wx/odcombo.h"
#include "wx/uilocale.h"

#include <limits>

// Drawing ARGB on standard DC is supported by OSX and GTK3
#if defined(__WXOSX__) || defined(__WXGTK3__)
#define wxPG_DC_SUPPORTS_ALPHA 1
#else
#define wxPG_DC_SUPPORTS_ALPHA 0
#endif // __WXOSX__ || __WXGTK3__

#define wxPG_USE_GC_FOR_ALPHA  (wxUSE_GRAPHICS_CONTEXT && !wxPG_DC_SUPPORTS_ALPHA)

#if wxPG_USE_GC_FOR_ALPHA
#include "wx/dcgraph.h"
#ifndef WX_PRECOMP
#include "wx/dcclient.h" // for wxDynamicCast purposes
#include "wx/dcmemory.h" // for wxDynamicCast purposes
#endif // WX_PRECOMP
#if wxUSE_METAFILE
    #include "wx/metafile.h"  // for wxDynamicCast purposes
#endif // wxUSE_METAFILE
#endif // wxPG_USE_GC_FOR_ALPHA

// -----------------------------------------------------------------------

#if defined(__WXMSW__)
    #define wxPG_CAN_DRAW_CURSOR           1
#elif defined(__WXGTK__)
    #define wxPG_CAN_DRAW_CURSOR           1
#elif defined(__WXMAC__)
    #define wxPG_CAN_DRAW_CURSOR           0
#else
    #define wxPG_CAN_DRAW_CURSOR           0
#endif

constexpr int wxPG_INT_MIN = std::numeric_limits<int>::min();
constexpr int wxPG_INT_MAX = std::numeric_limits<int>::max();

// -----------------------------------------------------------------------
// Value type related
// -----------------------------------------------------------------------


// Implement dynamic class for type value.
wxIMPLEMENT_DYNAMIC_CLASS(wxColourPropertyValue, wxObject);

bool operator == (const wxColourPropertyValue& a, const wxColourPropertyValue& b)
{
    return ( ( a.m_colour == b.m_colour ) && (a.m_type == b.m_type) );
}

bool operator == (const wxArrayInt& array1, const wxArrayInt& array2)
{
    if ( array1.size() != array2.size() )
        return false;
    for ( size_t i = 0; i < array1.size(); i++ )
    {
        if ( array1[i] != array2[i] )
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------
// wxSpinCtrl-based property editor
// -----------------------------------------------------------------------

#if wxUSE_SPINBTN


#ifdef __WXMSW__
  #define IS_MOTION_SPIN_SUPPORTED  1
#else
  #define IS_MOTION_SPIN_SUPPORTED  0
#endif

#if IS_MOTION_SPIN_SUPPORTED

//
// This class implements ability to rapidly change "spin" value
// by moving mouse when one of the spin buttons is depressed.
class wxPGSpinButton : public wxSpinButton
{
public:
    wxPGSpinButton() : wxSpinButton()
        , m_spins(1)
        , m_hasCapture(false)
    {
        Bind(wxEVT_LEFT_DOWN, &wxPGSpinButton::OnMouseLeftDown, this);
        Bind(wxEVT_LEFT_UP, &wxPGSpinButton::OnMouseLeftUp, this);
        Bind(wxEVT_MOTION, &wxPGSpinButton::OnMouseMove, this);
        Bind(wxEVT_MOUSE_CAPTURE_LOST, &wxPGSpinButton::OnMouseCaptureLost, this);
    }

    int GetSpins() const
    {
        return m_spins;
    }

private:
    wxPoint m_ptPosition;

    // Having a separate spins variable allows us to handle validation etc. for
    // multiple spin events at once (with quick mouse movements there could be
    // hundreds of 'spins' being done at once). Technically things like this
    // should be stored in event (wxSpinEvent in this case), but there probably
    // isn't anything there that can be reliably reused.
    int     m_spins;

    // SpinButton seems to be a special for mouse capture, so we may need track
    // privately whether mouse is actually captured.
    bool    m_hasCapture;

    void Capture()
    {
        if ( !m_hasCapture )
        {
            CaptureMouse();
            m_hasCapture = true;
        }

        SetCursor(wxCURSOR_SIZENS);
    }
    void Release()
    {
        if ( m_hasCapture )
        {
            ReleaseMouse();
            m_hasCapture = false;
        }

        wxWindow *parent = GetParent();
        if ( parent )
            SetCursor(parent->GetCursor());
        else
            SetCursor(wxNullCursor);
    }

    void OnMouseLeftDown(wxMouseEvent& evt)
    {
        m_ptPosition = evt.GetPosition();
        evt.Skip();
    }

    void OnMouseLeftUp(wxMouseEvent& evt)
    {
        Release();
        evt.Skip();
    }

    void OnMouseMove(wxMouseEvent& evt)
    {
        if ( evt.LeftIsDown() )
        {
            int dy = m_ptPosition.y - evt.GetPosition().y;
            if ( dy )
            {
                Capture();
                m_ptPosition = evt.GetPosition();

                wxSpinEvent evtscroll( (dy >= 0) ? wxEVT_SCROLL_LINEUP :
                                                    wxEVT_SCROLL_LINEDOWN,
                                        GetId() );
                evtscroll.SetEventObject(this);

                wxASSERT( m_spins == 1 );

                m_spins = abs(dy);
                GetEventHandler()->ProcessEvent(evtscroll);
                m_spins = 1;
            }
        }

        evt.Skip();
    }

    void OnMouseCaptureLost(wxMouseCaptureLostEvent& WXUNUSED(event))
    {
        Release();
    }
};

#endif  // IS_MOTION_SPIN_SUPPORTED


WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(SpinCtrl,
                                      wxPGSpinCtrlEditor,
                                      wxPGEditor)


// Destructor. It is useful to reset the global pointer in it.
wxPGSpinCtrlEditor::~wxPGSpinCtrlEditor()
{
    wxPG_EDITOR(SpinCtrl) = nullptr;
}

// Create controls and initialize event handling.
wxPGWindowList wxPGSpinCtrlEditor::CreateControls( wxPropertyGrid* propgrid, wxPGProperty* property,
                                                   const wxPoint& pos, const wxSize& sz ) const
{
    wxSpinButton* wnd2;
    wxSize tcSz;

    wxNumericProperty* prop = wxDynamicCast(property, wxNumericProperty);
    if ( prop )
    {
        const int margin = 1;
#if IS_MOTION_SPIN_SUPPORTED
        if ( prop->UseSpinMotion() )
        {
            wnd2 = new wxPGSpinButton();
        }
        else
#endif
        {
            wnd2 = new wxSpinButton();
        }

#ifdef __WXMSW__
        wnd2->Hide();
#endif
        wnd2->Create(propgrid->GetPanel(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_VERTICAL);
        // Scale spin button to the required height (row height)
        wxSize butSz = wnd2->GetBestSize();
#ifdef __WXGTK3__
        // Under GTK+ 3 spin button is always horizontal and cannot be downscaled
        int butWidth = butSz.x;
#else
        double sc = (double)sz.y / butSz.y;
        int butWidth = wxMax(18, wxRound(sc*butSz.x));
#endif
        tcSz.Set(sz.x - butWidth - margin, sz.y);
        wnd2->SetSize(pos.x + tcSz.x + margin, pos.y, butWidth, sz.y);
        wnd2->SetRange(wxPG_INT_MIN, wxPG_INT_MAX);
        wnd2->SetValue(0);
    }
    else
    {
        wxFAIL_MSG( "SpinCtrl editor can be assigned only to numeric property" );
        tcSz.Set(sz.x, sz.y);
        wnd2 = nullptr;
    }

    wxWindow* wnd1 = wxPGTextCtrlEditor::CreateControls(propgrid, property, pos, tcSz).GetPrimary();
#if wxUSE_VALIDATORS
    // Let's add validator to make sure only numbers can be entered
    wxTextValidator validator(wxFILTER_NUMERIC, &m_tempString);
    wnd1->SetValidator(validator);
#endif

    return wxPGWindowList(wnd1, wnd2);
}

// Control's events are redirected here
bool wxPGSpinCtrlEditor::OnEvent(wxPropertyGrid* propgrid, wxPGProperty* property,
    wxWindow* wnd, wxEvent& event) const
{
    wxNumericProperty* prop = wxDynamicCast(property, wxNumericProperty);
    if ( prop )
    {
        wxEventType evtType = event.GetEventType();
        bool bigStep = false;

        if ( evtType == wxEVT_KEY_DOWN )
        {
            wxKeyEvent& keyEvent = (wxKeyEvent&)event;
            int keycode;
            keycode = keyEvent.GetKeyCode();

            if ( keycode == WXK_UP )
                evtType = wxEVT_SCROLL_LINEUP;
            else if ( keycode == WXK_DOWN )
                evtType = wxEVT_SCROLL_LINEDOWN;
            else if ( keycode == WXK_PAGEUP )
            {
                evtType = wxEVT_SCROLL_LINEUP;
                bigStep = true;
            }
            else if ( keycode == WXK_PAGEDOWN )
            {
                evtType = wxEVT_SCROLL_LINEDOWN;
                bigStep = true;
            }
        }

        if ( evtType == wxEVT_SCROLL_LINEUP || evtType == wxEVT_SCROLL_LINEDOWN )
        {
            int spins = 1;
#if IS_MOTION_SPIN_SUPPORTED
            if ( prop->UseSpinMotion() )
            {
                wxPGSpinButton* spinButton =
                    (wxPGSpinButton*)propgrid->GetEditorControlSecondary();

                if ( spinButton )
                    spins = spinButton->GetSpins();
            }
#endif

            long stepScale = (evtType == wxEVT_SCROLL_LINEUP) ? 1L : -1L;
            if ( bigStep )
                stepScale *= 10L;
            stepScale *= spins;

            wxVariant v = prop->AddSpinStepValue(stepScale);
            SetControlStringValue(prop, propgrid->GetEditorControl(), prop->ValueToString(v));
            return true;
        }
    }
    return wxPGTextCtrlEditor::OnEvent(propgrid, property, wnd, event);
}
#endif // wxUSE_SPINBTN


// -----------------------------------------------------------------------
// wxDatePickerCtrl-based property editor
// -----------------------------------------------------------------------

#if wxUSE_DATEPICKCTRL


#include "wx/datectrl.h"
#include "wx/dateevt.h"

class wxPGDatePickerCtrlEditor : public wxPGEditor
{
    wxDECLARE_DYNAMIC_CLASS(wxPGDatePickerCtrlEditor);
public:
    virtual ~wxPGDatePickerCtrlEditor();

    wxString GetName() const override;
    virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid,
                                          wxPGProperty* property,
                                          const wxPoint& pos,
                                          const wxSize& size) const override;
    virtual void UpdateControl( wxPGProperty* property, wxWindow* wnd ) const override;
    virtual bool OnEvent( wxPropertyGrid* propgrid, wxPGProperty* property,
        wxWindow* wnd, wxEvent& event ) const override;
    virtual bool GetValueFromControl( wxVariant& variant, wxPGProperty* property, wxWindow* wnd ) const override;
    virtual void SetValueToUnspecified( wxPGProperty* WXUNUSED(property), wxWindow* wnd ) const override;
};


WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(DatePickerCtrl,
                                      wxPGDatePickerCtrlEditor,
                                      wxPGEditor)


wxPGDatePickerCtrlEditor::~wxPGDatePickerCtrlEditor()
{
    wxPG_EDITOR(DatePickerCtrl) = nullptr;
}

wxPGWindowList wxPGDatePickerCtrlEditor::CreateControls( wxPropertyGrid* propgrid,
                                                         wxPGProperty* property,
                                                         const wxPoint& pos,
                                                         const wxSize& sz ) const
{
    wxDateProperty* prop = wxDynamicCast(property, wxDateProperty);
    wxCHECK_MSG(prop, nullptr, "wxDatePickerCtrl editor can only be used with wxDateProperty or derivative.");

    // Use two stage creation to allow cleaner display on wxMSW
    wxDatePickerCtrl* ctrl = new wxDatePickerCtrl();
#ifdef __WXMSW__
    ctrl->Hide();
    wxSize useSz = wxDefaultSize;
    useSz.x = sz.x;
#else
    wxSize useSz = sz;
#endif

    wxDateTime dateValue(wxInvalidDateTime);

    wxVariant value = prop->GetValue();
    if ( value.IsType(wxPG_VARIANT_TYPE_DATETIME) )
        dateValue = value.GetDateTime();

    ctrl->Create(propgrid->GetPanel(),
                 wxID_ANY,
                 dateValue,
                 pos,
                 useSz,
                 prop->GetDatePickerStyle() | wxNO_BORDER);

#ifdef __WXMSW__
    ctrl->Show();
#endif

    return ctrl;
}

// Copies value from property to control
void wxPGDatePickerCtrlEditor::UpdateControl( wxPGProperty* property,
                                              wxWindow* wnd ) const
{
    wxDatePickerCtrl* ctrl = wxDynamicCast(wnd, wxDatePickerCtrl);
    wxCHECK_RET(ctrl, "Only wxDatePickerCtrl editor can be updated");

    wxDateTime dateValue(wxInvalidDateTime);
    wxVariant v(property->GetValue());
    if ( v.IsType(wxPG_VARIANT_TYPE_DATETIME) )
        dateValue = v.GetDateTime();

    ctrl->SetValue( dateValue );
}

// Control's events are redirected here
bool wxPGDatePickerCtrlEditor::OnEvent( wxPropertyGrid* WXUNUSED(propgrid),
                                        wxPGProperty* WXUNUSED(property),
                                        wxWindow* WXUNUSED(wnd),
                                        wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_DATE_CHANGED )
        return true;

    return false;
}

bool wxPGDatePickerCtrlEditor::GetValueFromControl( wxVariant& variant, wxPGProperty* WXUNUSED(property), wxWindow* wnd ) const
{
    wxDatePickerCtrl* ctrl = wxDynamicCast(wnd, wxDatePickerCtrl);
    wxCHECK_MSG(ctrl, false, "Value can be retrieved only from wxDatePickerCtrl editor");

    variant = ctrl->GetValue();

    return true;
}

void wxPGDatePickerCtrlEditor::SetValueToUnspecified( wxPGProperty* property,
                                                      wxWindow* wnd ) const
{
    wxDatePickerCtrl* ctrl = wxDynamicCast(wnd, wxDatePickerCtrl);
    wxCHECK_RET(ctrl, "Only wxDatePickerCtrl editor can be updated");

    wxDateProperty* prop = wxDynamicCast(property, wxDateProperty);
    wxCHECK_RET(prop, "wxDatePickerCtrl editor can only be used with wxDateProperty or derivative.");

    long datePickerStyle = prop->GetDatePickerStyle();
    if ( datePickerStyle & wxDP_ALLOWNONE )
        ctrl->SetValue(wxInvalidDateTime);
}

#endif // wxUSE_DATEPICKCTRL


// -----------------------------------------------------------------------
// wxFontProperty
// -----------------------------------------------------------------------

#include "wx/fontdlg.h"
#include "wx/fontenum.h"

//
// NB: Do not use wxS here since unlike wxT it doesn't translate to wxChar*
//

static const wxChar* const gs_fp_es_family_labels[] = {
    wxT("Default"), wxT("Decorative"),
    wxT("Roman"), wxT("Script"),
    wxT("Swiss"), wxT("Modern"),
    wxT("Teletype"), wxT("Unknown"),
    (const wxChar*) nullptr
};

static const long gs_fp_es_family_values[] = {
    wxFONTFAMILY_DEFAULT, wxFONTFAMILY_DECORATIVE,
    wxFONTFAMILY_ROMAN, wxFONTFAMILY_SCRIPT,
    wxFONTFAMILY_SWISS, wxFONTFAMILY_MODERN,
    wxFONTFAMILY_TELETYPE, wxFONTFAMILY_UNKNOWN
};

static const wxChar* const gs_fp_es_style_labels[] = {
    wxT("Normal"),
    wxT("Slant"),
    wxT("Italic"),
    (const wxChar*) nullptr
};

static const long gs_fp_es_style_values[] = {
    wxFONTSTYLE_NORMAL,
    wxFONTSTYLE_SLANT,
    wxFONTSTYLE_ITALIC
};

static const wxChar* const gs_fp_es_weight_labels[] = {
    wxT("Thin"),
    wxT("ExtraLight"),
    wxT("Light"),
    wxT("Normal"),
    wxT("Medium"),
    wxT("SemiBold"),
    wxT("Bold"),
    wxT("ExtraBold"),
    wxT("Heavy"),
    wxT("ExtraHeavy"),
    (const wxChar*) nullptr
};

static const long gs_fp_es_weight_values[] = {
    wxFONTWEIGHT_THIN,
    wxFONTWEIGHT_EXTRALIGHT,
    wxFONTWEIGHT_LIGHT,
    wxFONTWEIGHT_NORMAL,
    wxFONTWEIGHT_MEDIUM,
    wxFONTWEIGHT_SEMIBOLD,
    wxFONTWEIGHT_BOLD,
    wxFONTWEIGHT_EXTRABOLD,
    wxFONTWEIGHT_HEAVY,
    wxFONTWEIGHT_EXTRAHEAVY
};

// Class body is in advprops.h


wxPG_IMPLEMENT_PROPERTY_CLASS(wxFontProperty,wxEditorDialogProperty,TextCtrlAndButton)


wxFontProperty::wxFontProperty( const wxString& label, const wxString& name,
                                const wxFont& value )
    : wxEditorDialogProperty(label,name)
{
    SetValue(WXVARIANT(value));

    // Initialize font family choices list
    if ( !wxPGGlobalVars->m_fontFamilyChoices )
    {
        wxArrayString faceNames = wxFontEnumerator::GetFacenames();

        faceNames.Sort();

        wxPGGlobalVars->m_fontFamilyChoices = new wxPGChoices(faceNames);
    }

    wxFont font;
    font << m_value;

    /* TRANSLATORS: Label of font point size */
    AddPrivateChild( new wxIntProperty( _("Point Size"),
                     wxS("Point Size"),(long)font.GetPointSize() ) );

    wxString faceName = font.GetFaceName();
    // If font was not in there, add it now
    if ( !faceName.empty() &&
         wxPGGlobalVars->m_fontFamilyChoices->Index(faceName) == wxNOT_FOUND )
        wxPGGlobalVars->m_fontFamilyChoices->AddAsSorted(faceName);

    /* TRANSLATORS: Label of font face name */
    wxPGProperty* p = new wxEnumProperty(_("Face Name"), wxS("Face Name"),
                                         *wxPGGlobalVars->m_fontFamilyChoices);

    p->SetValueFromString(faceName, wxPG_FULL_VALUE);

    AddPrivateChild( p );

    /* TRANSLATORS: Label of font style */
    AddPrivateChild( new wxEnumProperty(_("Style"), wxS("Style"),
                     gs_fp_es_style_labels,gs_fp_es_style_values,
                     font.GetStyle()) );

    /* TRANSLATORS: Label of font weight */
    AddPrivateChild( new wxEnumProperty(_("Weight"), wxS("Weight"),
                     gs_fp_es_weight_labels,gs_fp_es_weight_values,
                     font.GetWeight()) );

    /* TRANSLATORS: Label of underlined font */
    AddPrivateChild( new wxBoolProperty(_("Underlined"), wxS("Underlined"),
                     font.GetUnderlined()) );

    /* TRANSLATORS: Label of font family */
    AddPrivateChild( new wxEnumProperty(_("Family"), wxS("PointSize"),
                     gs_fp_es_family_labels,gs_fp_es_family_values,
                     font.GetFamily()) );
}

void wxFontProperty::OnSetValue()
{
    wxFont font;
    font << m_value;

    if ( !font.IsOk() )
    {
        m_value = WXVARIANT(*wxNORMAL_FONT);
    }
}

wxString wxFontProperty::ValueToString( wxVariant& value,
                                        int argFlags ) const
{
    return wxEditorDialogProperty::ValueToString(value, argFlags);
}

bool wxFontProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxFont font;

    wxASSERT_MSG(value.IsType(wxS("wxFont")), "Function called for incompatible property");
    if ( value.IsType(wxS("wxFont")) )
        font << value;

    wxFontData data;
    data.SetInitialFont(font);
    data.SetColour(*wxBLACK);

    wxFontDialog dlg(pg->GetPanel(), data);
    if ( !m_dlgTitle.empty() )
    {
        dlg.SetTitle(m_dlgTitle);
    }
    if ( dlg.ShowModal() == wxID_OK )
    {
        value = WXVARIANT(dlg.GetFontData().GetChosenFont());
        return true;
    }

    return false;
}

void wxFontProperty::RefreshChildren()
{
    if ( !GetChildCount() ) return;
    wxFont font;
    font << m_value;
    Item(0)->SetValue( (long)font.GetPointSize() );
    Item(1)->SetValueFromString( font.GetFaceName(), wxPG_FULL_VALUE );
    Item(2)->SetValue( (long)font.GetStyle() );
    Item(3)->SetValue( (long)font.GetWeight() );
    Item(4)->SetValue( font.GetUnderlined() );
    Item(5)->SetValue( (long)font.GetFamily() );
}

wxVariant wxFontProperty::ChildChanged( wxVariant& thisValue,
                                        int ind,
                                        wxVariant& childValue ) const
{
    wxFont font;
    font << thisValue;

    if ( ind == 0 )
    {
        font.SetPointSize( static_cast<int>(childValue.GetLong()) );
    }
    else if ( ind == 1 )
    {
        wxString faceName;
        int faceIndex = static_cast<int>(childValue.GetLong());

        if ( faceIndex >= 0 )
            faceName = wxPGGlobalVars->m_fontFamilyChoices->GetLabel(faceIndex);

        font.SetFaceName( faceName );
    }
    else if ( ind == 2 )
    {
        int st = static_cast<int>(childValue.GetLong());
        if ( st != wxFONTSTYLE_NORMAL &&
             st != wxFONTSTYLE_SLANT &&
             st != wxFONTSTYLE_ITALIC )
             st = wxFONTWEIGHT_NORMAL;
        font.SetStyle( static_cast<wxFontStyle>(st) );
    }
    else if ( ind == 3 )
    {
        int wt = static_cast<int>(childValue.GetLong());
        if ( wt < wxFONTWEIGHT_THIN || wt > wxFONTWEIGHT_MAX )
             wt = wxFONTWEIGHT_NORMAL;
        font.SetWeight( static_cast<wxFontWeight>(wt) );
    }
    else if ( ind == 4 )
    {
        font.SetUnderlined( childValue.GetBool() );
    }
    else if ( ind == 5 )
    {
        int fam = static_cast<int>(childValue.GetLong());
        if ( fam < wxFONTFAMILY_DEFAULT ||
             fam > wxFONTFAMILY_TELETYPE )
             fam = wxFONTFAMILY_DEFAULT;
        font.SetFamily( static_cast<wxFontFamily>(fam) );
    }

    return WXVARIANT(font);
}

/*
wxSize wxFontProperty::OnMeasureImage() const
{
    return wxPG_DEFAULT_IMAGE_SIZE;
}

void wxFontProperty::OnCustomPaint(wxDC& dc,
                                        const wxRect& rect,
                                        wxPGPaintData& paintData)
{
    wxString drawFace;
    if ( paintData.m_choiceItem >= 0 )
        drawFace = wxPGGlobalVars->m_fontFamilyChoices->GetLabel(paintData.m_choiceItem);
    else
        drawFace = m_value_wxFont.GetFaceName();

    if ( !drawFace.empty() )
    {
        // Draw the background
        dc.SetBrush( wxColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE)) );
        //dc.SetBrush( *wxWHITE_BRUSH );
        //dc.SetPen( *wxMEDIUM_GREY_PEN );
        dc.DrawRectangle( rect );

        wxFont oldFont = dc.GetFont();
        dc.SetFont(wxFontInfo(oldFont.GetPointSize().Bold().FaceName(drawFace));

        dc.SetTextForeground( wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT) );
        dc.DrawText( wxS("Aa"), rect.x+2, rect.y+1 );

        dc.SetFont(oldFont);
    }
    else
    {
        // No file - just draw a white box
        dc.SetBrush ( *wxWHITE_BRUSH );
        dc.DrawRectangle ( rect );
    }
}
*/


// -----------------------------------------------------------------------
// wxSystemColourProperty
// -----------------------------------------------------------------------

// wxEnumProperty based classes cannot use wxPG_PROP_CLASS_SPECIFIC_1
#define wxPG_PROP_HIDE_CUSTOM_COLOUR        wxPG_PROP_CLASS_SPECIFIC_2
#define wxPG_PROP_COLOUR_HAS_ALPHA          wxPG_PROP_CLASS_SPECIFIC_3

#include "wx/colordlg.h"

static const char* const gs_cp_es_syscolour_labels[] = {
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("AppWorkspace"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ActiveBorder"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ActiveCaption"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ButtonFace"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ButtonHighlight"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ButtonShadow"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ButtonText"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("CaptionText"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ControlDark"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("ControlLight"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("Desktop"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("GrayText"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("Highlight"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("HighlightText"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("InactiveBorder"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("InactiveCaption"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("InactiveCaptionText"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("Menu"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("Scrollbar"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("Tooltip"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("TooltipText"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("Window"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("WindowFrame"),
    /* TRANSLATORS: Keyword of system colour */ wxTRANSLATE("WindowText"),
    /* TRANSLATORS: Custom colour choice entry */ wxTRANSLATE("Custom"),
    nullptr
};

static const long gs_cp_es_syscolour_values[] = {
    wxSYS_COLOUR_APPWORKSPACE,
    wxSYS_COLOUR_ACTIVEBORDER,
    wxSYS_COLOUR_ACTIVECAPTION,
    wxSYS_COLOUR_BTNFACE,
    wxSYS_COLOUR_BTNHIGHLIGHT,
    wxSYS_COLOUR_BTNSHADOW,
    wxSYS_COLOUR_BTNTEXT ,
    wxSYS_COLOUR_CAPTIONTEXT,
    wxSYS_COLOUR_3DDKSHADOW,
    wxSYS_COLOUR_3DLIGHT,
    wxSYS_COLOUR_BACKGROUND,
    wxSYS_COLOUR_GRAYTEXT,
    wxSYS_COLOUR_HIGHLIGHT,
    wxSYS_COLOUR_HIGHLIGHTTEXT,
    wxSYS_COLOUR_INACTIVEBORDER,
    wxSYS_COLOUR_INACTIVECAPTION,
    wxSYS_COLOUR_INACTIVECAPTIONTEXT,
    wxSYS_COLOUR_MENU,
    wxSYS_COLOUR_SCROLLBAR,
    wxSYS_COLOUR_INFOBK,
    wxSYS_COLOUR_INFOTEXT,
    wxSYS_COLOUR_WINDOW,
    wxSYS_COLOUR_WINDOWFRAME,
    wxSYS_COLOUR_WINDOWTEXT,
    wxPG_COLOUR_CUSTOM
};


IMPLEMENT_VARIANT_OBJECT_EXPORTED_SHALLOWCMP(wxColourPropertyValue, WXDLLIMPEXP_PROPGRID)

template<> inline wxVariant WXVARIANT(const wxColourPropertyValue& value)
{
    wxVariant variant;
    variant << value;
    return variant;
}

// Class body is in advprops.h

wxPG_IMPLEMENT_PROPERTY_CLASS(wxSystemColourProperty,wxEnumProperty,Choice)


void wxSystemColourProperty::Init( int type, const wxColour& colour )
{
    wxColourPropertyValue cpv;

    cpv.Init(type, colour.IsOk() ? colour : *wxWHITE);

    m_flags |= wxPG_PROP_STATIC_CHOICES; // Colour selection cannot be changed.

    m_value = WXVARIANT(cpv);

    OnSetValue();
}


static wxPGChoices gs_wxSystemColourProperty_choicesCache;


wxSystemColourProperty::wxSystemColourProperty( const wxString& label, const wxString& name,
    const wxColourPropertyValue& value )
    : wxEnumProperty( label,
                      name,
                      gs_cp_es_syscolour_labels,
                      gs_cp_es_syscolour_values,
                      &gs_wxSystemColourProperty_choicesCache )
{
        Init( value.m_type, value.m_colour );
}


wxSystemColourProperty::wxSystemColourProperty( const wxString& label, const wxString& name,
    const char* const* labels, const long* values, wxPGChoices* choicesCache,
    const wxColourPropertyValue& value )
    : wxEnumProperty( label, name, labels, values, choicesCache )
{
        Init( value.m_type, value.m_colour );
}


wxSystemColourProperty::wxSystemColourProperty( const wxString& label, const wxString& name,
    const char* const* labels, const long* values, wxPGChoices* choicesCache,
    const wxColour& value )
    : wxEnumProperty( label, name, labels, values, choicesCache )
{
        Init( wxPG_COLOUR_CUSTOM, value );
}

wxColourPropertyValue wxSystemColourProperty::GetVal( const wxVariant* pVariant ) const
{
    if ( !pVariant )
        pVariant = &m_value;

    if ( pVariant->IsNull() )
        return wxColourPropertyValue(wxPG_COLOUR_UNSPECIFIED, wxColour());

    const wxString valType(pVariant->GetType());
    if ( valType == wxS("wxColourPropertyValue") )
    {
        wxColourPropertyValue v;
        v << *pVariant;
        return v;
    }

    wxColour col;
    bool variantProcessed = true;

    if ( valType == wxS("wxColour*") )
    {
        wxColour* pCol = wxStaticCast(pVariant->GetWxObjectPtr(), wxColour);
        col = *pCol;
    }
    else if ( valType == wxS("wxColour") )
    {
        col << *pVariant;
    }
    else if ( valType == wxArrayInt_VariantType )
    {
        // This code is mostly needed for wxPython bindings, which
        // may offer tuple of integers as colour value.
        wxArrayInt arr;
        arr << *pVariant;

        if ( arr.size() >= 3 )
        {
            int r, g, b;
            int a = 255;

            r = arr[0];
            g = arr[1];
            b = arr[2];
            if ( arr.size() >= 4 )
                a = arr[3];

            col = wxColour(r, g, b, a);
        }
        else
        {
            variantProcessed = false;
        }
    }
    else
    {
        variantProcessed = false;
    }

    if ( !variantProcessed )
        return wxColourPropertyValue(wxPG_COLOUR_UNSPECIFIED, wxColour());

    wxColourPropertyValue v2( wxPG_COLOUR_CUSTOM, col );

    int colInd = ColToInd(col);
    if ( colInd != wxNOT_FOUND )
        v2.m_type = colInd;

    return v2;
}

wxVariant wxSystemColourProperty::DoTranslateVal( wxColourPropertyValue& v ) const
{
    return WXVARIANT(v);
}

int wxSystemColourProperty::ColToInd( const wxColour& colour ) const
{
    const unsigned int i_max = m_choices.GetCount();
    for ( unsigned int i=0; i<i_max; i++ )
    {
        const int ind = m_choices[i].GetValue();
        // Skip custom colour
        if (ind == wxPG_COLOUR_CUSTOM)
            continue;

        if ( colour == GetColour(ind) )
        {
            /*wxLogDebug(wxS("%s(%s): Index %i for ( getcolour(%i,%i,%i), colour(%i,%i,%i))"),
                GetClassName(),GetLabel(),
                (int)i,(int)GetColour(ind).Red(),(int)GetColour(ind).Green(),(int)GetColour(ind).Blue(),
                (int)colour.Red(),(int)colour.Green(),(int)colour.Blue());*/
            return ind;
        }
    }
    return wxNOT_FOUND;
}

void wxSystemColourProperty::OnSetValue()
{
    // Convert from generic wxobject ptr to wxPGVariantDataColour
    if ( m_value.IsType(wxS("wxColour*")) )
    {
        wxColour* pCol = wxStaticCast(m_value.GetWxObjectPtr(), wxColour);
        m_value = WXVARIANT(*pCol);
    }

    wxColourPropertyValue val = GetVal(&m_value);

    if ( val.m_type == wxPG_COLOUR_UNSPECIFIED )
    {
        m_value.MakeNull();
        return;
    }
    else
    {

        if ( val.m_type < wxPG_COLOUR_WEB_BASE )
            val.m_colour = GetColour( val.m_type );

        m_value = TranslateVal(val);
    }

    int ind = wxNOT_FOUND;

    if ( m_value.IsType(wxS("wxColourPropertyValue")) )
    {
        wxColourPropertyValue cpv;
        cpv << m_value;
        wxColour col = cpv.m_colour;

        if ( !col.IsOk() )
        {
            SetValueToUnspecified();
            SetIndex(wxNOT_FOUND);
            return;
        }

        if ( cpv.m_type < wxPG_COLOUR_WEB_BASE ||
             (m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) )
        {
            ind = GetIndexForValue(cpv.m_type);
        }
        else
        {
            cpv.m_type = wxPG_COLOUR_CUSTOM;
            ind = GetCustomColourIndex();
        }
    }
    else
    {
        wxColour col;
        col << m_value;

        if ( !col.IsOk() )
        {
            SetValueToUnspecified();
            SetIndex(wxNOT_FOUND);
            return;
        }

        ind = ColToInd(col);

        if ( ind == wxNOT_FOUND &&
             !(m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) )
            ind = GetCustomColourIndex();
    }

    SetIndex(ind);
}


wxColour wxSystemColourProperty::GetColour( int index ) const
{
    return wxSystemSettings::GetColour( (wxSystemColour)index );
}

wxString wxSystemColourProperty::ColourToString( const wxColour& col,
                                                 int index,
                                                 int argFlags ) const
{

    if ( index == wxNOT_FOUND )
    {

        if ( (argFlags & wxPG_FULL_VALUE) || (m_flags & wxPG_PROP_COLOUR_HAS_ALPHA) )
        {
            return wxString::Format(wxS("(%i,%i,%i,%i)"),
                                    (int)col.Red(),
                                    (int)col.Green(),
                                    (int)col.Blue(),
                                    (int)col.Alpha());
        }
        else
        {
            return wxString::Format(wxS("(%i,%i,%i)"),
                                    (int)col.Red(),
                                    (int)col.Green(),
                                    (int)col.Blue());
        }
    }
    else
    {
        return m_choices.GetLabel(index);
    }
}

wxString wxSystemColourProperty::ValueToString( wxVariant& value,
                                                int argFlags ) const
{
    wxColourPropertyValue val = GetVal(&value);

    int index;

    if ( argFlags & wxPG_VALUE_IS_CURRENT )
    {
        // GetIndex() only works reliably if wxPG_VALUE_IS_CURRENT flag is set,
        // but we should use it whenever possible.
        index = GetIndex();

        // If custom colour was selected, use invalid index, so that
        // ColourToString() will return properly formatted colour text.
        if ( index == GetCustomColourIndex() &&
             !(m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) )
            index = wxNOT_FOUND;
    }
    else
    {
        index = m_choices.Index(val.m_type);
    }

    return ColourToString(val.m_colour, index, argFlags);
}


wxSize wxSystemColourProperty::OnMeasureImage( int ) const
{
    return wxPG_DEFAULT_IMAGE_SIZE;
}


int wxSystemColourProperty::GetCustomColourIndex() const
{
    return m_choices.Index(wxPG_COLOUR_CUSTOM);
}


bool wxSystemColourProperty::QueryColourFromUser( wxVariant& variant ) const
{
    wxASSERT( !m_value.IsType(wxPG_VARIANT_TYPE_STRING) );
    bool res = false;

    wxPropertyGrid* propgrid = GetGrid();
    wxASSERT( propgrid );

    // Must only occur when user triggers event
    if ( !propgrid->HasInternalFlag(wxPropertyGrid::wxPG_FL_IN_HANDLECUSTOMEDITOREVENT) )
        return res;

    wxColourPropertyValue val = GetVal();

    val.m_type = wxPG_COLOUR_CUSTOM;

    wxColourData data;
    data.SetChooseFull(true);
    data.SetChooseAlpha((m_flags & wxPG_PROP_COLOUR_HAS_ALPHA) != 0);
    data.SetColour(val.m_colour);
    for ( int i = 0; i < wxColourData::NUM_CUSTOM; i++ )
    {
        unsigned char n = i*(256/wxColourData::NUM_CUSTOM);
        data.SetCustomColour(i, wxColour(n, n, n));
    }

    wxColourDialog dialog(propgrid, &data);
    if ( dialog.ShowModal() == wxID_OK )
    {
        wxColourData retData = dialog.GetColourData();
        val.m_colour = retData.GetColour();

        variant = DoTranslateVal(val);

        SetValueInEvent(variant);

        res = true;
    }

    return res;
}


bool wxSystemColourProperty::IntToValue( wxVariant& variant, int number, int argFlags ) const
{
    int index = number;
    const int type = m_choices.GetValue(index);

    if ( type == wxPG_COLOUR_CUSTOM )
    {
         if ( !(argFlags & wxPG_PROPERTY_SPECIFIC) )
            return QueryColourFromUser(variant);

         // Call from event handler.
         // User will be asked for custom color later on in OnEvent().
         wxColourPropertyValue val = GetVal();
         variant = DoTranslateVal(val);
    }
    else
    {
        variant = TranslateVal( type, GetColour(type) );
    }

    return true;
}

// Need to do some extra event handling.
bool wxSystemColourProperty::OnEvent( wxPropertyGrid* propgrid,
                                      wxWindow* WXUNUSED(primary),
                                      wxEvent& event )
{
    bool askColour = false;

    if ( propgrid->IsMainButtonEvent(event) )
    {
        // We need to handle button click in case editor has been
        // switched to one that has wxButton as well.
        askColour = true;
    }
    else if ( event.GetEventType() == wxEVT_COMBOBOX )
    {
        // Must override index detection since at this point GetIndex()
        // will return old value.
        wxOwnerDrawnComboBox* cb =
            static_cast<wxOwnerDrawnComboBox*>(propgrid->GetEditorControl());

        if ( cb )
        {
            int index = cb->GetSelection();

            if ( index == GetCustomColourIndex() &&
                    !(m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) )
                askColour = true;
        }
    }

    if ( askColour && !propgrid->WasValueChangedInEvent() )
    {
        wxVariant variant;
        if ( QueryColourFromUser(variant) )
            return true;
    }
    return false;
}

/*class wxPGColourPropertyRenderer : public wxPGDefaultRenderer
{
public:
    virtual void Render( wxDC& dc, const wxRect& rect,
                         const wxPropertyGrid* propertyGrid, wxPGProperty* property,
                         int WXUNUSED(column), int item, int WXUNUSED(flags) ) const
    {
        wxASSERT( wxDynamicCast(property, wxSystemColourProperty) );
        wxSystemColourProperty* prop = wxStaticCast(property, wxSystemColourProperty);

        dc.SetPen(*wxBLACK_PEN);
        if ( item >= 0 &&
             ( item < (int)(GetCustomColourIndex) || (prop->HasFlag(wxPG_PROP_HIDE_CUSTOM_COLOUR)))
           )
        {
            int colInd;
            const wxArrayInt& values = prop->GetValues();
            if ( values.GetChildCount() )
                colInd = values[item];
            else
                colInd = item;
            dc.SetBrush( wxColour( prop->GetColour( colInd ) ) );
        }
        else if ( !prop->IsValueUnspecified() )
            dc.SetBrush( prop->GetVal().m_colour );
        else
            dc.SetBrush( *wxWHITE );

        wxRect imageRect = propertyGrid->GetImageRect(property, item);
        wxLogDebug(wxS("%i, %i"),imageRect.x,imageRect.y);
        dc.DrawRectangle( rect.x+imageRect.x, rect.y+imageRect.y,
                          imageRect.width, imageRect.height );

        wxString text;
        if ( item == -1 )
            text = property->GetValueAsString();
        else
            text = property->GetChoiceString(item);
        DrawText( dc, rect, imageRect.width, text );
    }
protected:
};

wxPGColourPropertyRenderer g_wxPGColourPropertyRenderer;

wxPGCellRenderer* wxSystemColourProperty::GetCellRenderer( int column ) const
{
    if ( column == 1 )
        return &g_wxPGColourPropertyRenderer;
    return wxEnumProperty::GetCellRenderer(column);
}*/

void wxSystemColourProperty::OnCustomPaint( wxDC& dc, const wxRect& rect,
                                            wxPGPaintData& paintdata )
{
    wxColour col;

    if ( paintdata.m_choiceItem >= 0 &&
         paintdata.m_choiceItem < (int)m_choices.GetCount() &&
         (paintdata.m_choiceItem != GetCustomColourIndex() ||
          m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) )
    {
        int colInd = m_choices[paintdata.m_choiceItem].GetValue();
        col = GetColour( colInd );
    }
    else if ( !IsValueUnspecified() )
    {
        col = GetVal().m_colour;
    }

    if ( col.IsOk() )
    {
#if wxPG_USE_GC_FOR_ALPHA
        wxGCDC *gdc = nullptr;
        if ( col.Alpha() != wxALPHA_OPAQUE )
        {
            if ( wxPaintDC *paintdc = wxDynamicCast(&dc, wxPaintDC) )
            {
                gdc = new wxGCDC(*paintdc);
            }
            else if ( wxMemoryDC *memdc = wxDynamicCast(&dc, wxMemoryDC) )
            {
                gdc = new wxGCDC(*memdc);
            }
#if wxUSE_METAFILE && defined(wxMETAFILE_IS_ENH)
            else if ( wxMetafileDC *metadc = wxDynamicCast(&dc, wxMetafileDC) )
            {
                gdc = new wxGCDC(*metadc);
            }
#endif
            else
            {
                wxFAIL_MSG( wxS("Unknown wxDC kind") );
            }
        }

        if ( gdc )
        {
            gdc->SetBrush(col);
            gdc->DrawRectangle(rect);
            delete gdc;
        }
        else
#endif // wxPG_USE_GC_FOR_ALPHA
        {
            dc.SetBrush(col);
            dc.DrawRectangle(rect);
        }
    }
}


bool wxSystemColourProperty::StringToValue( wxVariant& value, const wxString& text, int argFlags ) const
{
    const int custIndex = GetCustomColourIndex();
    wxString custColName;
    if (custIndex != wxNOT_FOUND)
        custColName = m_choices.GetLabel(custIndex);

    wxString colStr(text);
    colStr.Trim(true);
    colStr.Trim(false);

    const bool isCustomColour = (colStr == custColName);

    wxColour customColour;
    bool conversionSuccess = false;

    if ( !isCustomColour )
    {
        if ( colStr.Find(wxS("(")) == 0 )
        {
            // Eliminate whitespace
            colStr.Replace(wxS(" "), wxString());

            int commaCount = colStr.Freq(wxS(','));
            if ( commaCount == 2 )
            {
                // Convert (R,G,B) to rgb(R,G,B)
                colStr = wxS("rgb") + colStr;
            }
            else if ( commaCount == 3 )
            {
                // We have int alpha, CSS format that wxColour takes as
                // input processes float alpha. So, let's parse the colour
                // ourselves instead of trying to convert it to a format
                // that wxColour::FromString() understands.
                int r = -1, g = -1, b = -1, a = -1;
                wxSscanf(colStr, wxS("(%i,%i,%i,%i)"), &r, &g, &b, &a);
                customColour.Set(r, g, b, a);
                conversionSuccess = customColour.IsOk();
            }
        }

        if ( !conversionSuccess )
            conversionSuccess = customColour.Set(colStr);
    }

    if ( !conversionSuccess && m_choices.GetCount() &&
         !(m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) &&
         isCustomColour )
    {
        if ( !(argFlags & wxPG_EDITABLE_VALUE ))
        {
            // This really should not occur...
            // wxASSERT(false);
            return false;
        }

        if ( !QueryColourFromUser(value) )
        {
            if ( !(argFlags & wxPG_PROPERTY_SPECIFIC) )
                return false;
            // If query for value comes from the event handler
            // use current pending value to be processed later on in OnEvent().
            SetValueInEvent(value);
        }
    }
    else
    {
        wxColourPropertyValue val;

        bool done = false;

        if ( !conversionSuccess )
        {
            // Try predefined colour first
            int index;
            bool res = ValueFromString_(value, &index, colStr, argFlags);
            if ( res && index >= 0 )
            {
                val.m_type = index;
                if ( val.m_type < m_choices.GetCount() )
                    val.m_type = m_choices[val.m_type].GetValue();

                // Get proper colour for type.
                val.m_colour = GetColour(val.m_type);

                done = true;
            }
        }
        else
        {
            val.m_type = wxPG_COLOUR_CUSTOM;
            val.m_colour = customColour;
            done = true;
        }

        if ( !done )
        {
            return false;
        }

        value = DoTranslateVal(val);
    }

    return true;
}


bool wxSystemColourProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_COLOUR_ALLOW_CUSTOM )
    {
        bool allow = value.GetBool();

        if ( allow && (m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) )
        {
            // Show custom choice
            /* TRANSLATORS: Custom colour choice entry */
            m_choices.Add(_("Custom"), wxPG_COLOUR_CUSTOM);
            m_flags &= ~(wxPG_PROP_HIDE_CUSTOM_COLOUR);
        }
        else if ( !allow && !(m_flags & wxPG_PROP_HIDE_CUSTOM_COLOUR) )
        {
            // Hide custom choice
            m_choices.RemoveAt(GetCustomColourIndex());
            m_flags |= wxPG_PROP_HIDE_CUSTOM_COLOUR;
        }
        return true;
    }
    else if ( name == wxPG_COLOUR_HAS_ALPHA )
    {
        ChangeFlag(wxPG_PROP_COLOUR_HAS_ALPHA, value.GetBool());
        return true;
    }
    return wxEnumProperty::DoSetAttribute(name, value);
}


// -----------------------------------------------------------------------
// wxColourProperty
// -----------------------------------------------------------------------

static const char* const gs_cp_es_normcolour_labels[] = {
    wxTRANSLATE("Black"),
    wxTRANSLATE("Maroon"),
    wxTRANSLATE("Navy"),
    wxTRANSLATE("Purple"),
    wxTRANSLATE("Teal"),
    wxTRANSLATE("Gray"),
    wxTRANSLATE("Green"),
    wxTRANSLATE("Olive"),
    wxTRANSLATE("Brown"),
    wxTRANSLATE("Blue"),
    wxTRANSLATE("Fuchsia"),
    wxTRANSLATE("Red"),
    wxTRANSLATE("Orange"),
    wxTRANSLATE("Silver"),
    wxTRANSLATE("Lime"),
    wxTRANSLATE("Aqua"),
    wxTRANSLATE("Yellow"),
    wxTRANSLATE("White"),
    /* TRANSLATORS: Custom colour choice entry */ wxTRANSLATE("Custom"),
    nullptr
};

static const long gs_cp_es_normcolour_values[] = {
    0,
    1,
    2,
    3,
    4,
    5,
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    wxPG_COLOUR_CUSTOM
};

static const unsigned long gs_cp_es_normcolour_colours[] = {
    wxPG_COLOUR(0,0,0),
    wxPG_COLOUR(128,0,0),
    wxPG_COLOUR(0,0,128),
    wxPG_COLOUR(128,0,128),
    wxPG_COLOUR(0,128,128),
    wxPG_COLOUR(128,128,128),
    wxPG_COLOUR(0,128,0),
    wxPG_COLOUR(128,128,0),
    wxPG_COLOUR(166,124,81),
    wxPG_COLOUR(0,0,255),
    wxPG_COLOUR(255,0,255),
    wxPG_COLOUR(255,0,0),
    wxPG_COLOUR(247,148,28),
    wxPG_COLOUR(192,192,192),
    wxPG_COLOUR(0,255,0),
    wxPG_COLOUR(0,255,255),
    wxPG_COLOUR(255,255,0),
    wxPG_COLOUR(255,255,255),
    wxPG_COLOUR(0,0,0)
};

wxPG_IMPLEMENT_PROPERTY_CLASS(wxColourProperty, wxSystemColourProperty,
                              TextCtrlAndButton)

static wxPGChoices gs_wxColourProperty_choicesCache;

wxColourProperty::wxColourProperty( const wxString& label,
                      const wxString& name,
                      const wxColour& value )
    : wxSystemColourProperty(label, name, gs_cp_es_normcolour_labels,
                             gs_cp_es_normcolour_values,
                             &gs_wxColourProperty_choicesCache, value )
{
    wxASSERT_MSG( wxTheColourDatabase, wxS("No colour database") );
    if ( wxTheColourDatabase )
    {
        // Extend colour database with PG-specific colours.
        const char* const* colourLabels = gs_cp_es_normcolour_labels;
        for ( int i = 0; *colourLabels; colourLabels++, i++ )
        {
            // Don't take into account user-defined custom colour.
            if (gs_cp_es_normcolour_values[i] != wxPG_COLOUR_CUSTOM)
            {
                wxColour clr = wxTheColourDatabase->Find(*colourLabels);
                // Use standard wx colour value if its label was found,
                // otherwise register custom PG colour.
                if ( !clr.IsOk() )
                {
                    clr.Set(gs_cp_es_normcolour_colours[i]);
                    wxTheColourDatabase->AddColour(*colourLabels, clr);
                }
            }
        }
    }

    Init( value );

    m_flags |= wxPG_PROP_TRANSLATE_CUSTOM;
}

void wxColourProperty::Init( wxColour colour )
{
    if ( !colour.IsOk() )
        colour = *wxWHITE;
    m_value = WXVARIANT(colour);
    int ind = ColToInd(colour);
    if ( ind < 0 )
        ind = m_choices.GetCount() - 1;
    SetIndex( ind );
}

wxString wxColourProperty::ValueToString( wxVariant& value,
                                          int argFlags ) const
{
    const wxPGEditor* editor = GetEditorClass();
    if ( editor != wxPGEditor_Choice &&
         editor != wxPGEditor_ChoiceAndButton &&
         editor != wxPGEditor_ComboBox )
        argFlags |= wxPG_PROPERTY_SPECIFIC;

    return wxSystemColourProperty::ValueToString(value, argFlags);
}

wxColour wxColourProperty::GetColour( int index ) const
{
    return wxColour(gs_cp_es_normcolour_labels[m_choices.GetValue(index)]);
}

wxVariant wxColourProperty::DoTranslateVal( wxColourPropertyValue& v ) const
{
    return WXVARIANT(v.m_colour);
}

// -----------------------------------------------------------------------
// wxCursorProperty
// -----------------------------------------------------------------------

#if wxPG_CAN_DRAW_CURSOR
#define wxPG_CURSOR_IMAGE_WIDTH     32
#endif

#define NUM_CURSORS 28

static const char* const gs_cp_es_syscursors_labels[NUM_CURSORS+1] = {
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Default"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Arrow"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Right Arrow"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Blank"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Bullseye"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Character"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Cross"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Hand"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "I-Beam"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Left Button"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Magnifier"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Middle Button"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "No Entry"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Paint Brush"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Pencil"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Point Left"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Point Right"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Question Arrow"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Right Button"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Sizing NE-SW"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Sizing N-S"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Sizing NW-SE"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Sizing W-E"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Sizing"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Spraycan"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Wait"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Watch"),
    wxTRANSLATE_IN_CONTEXT("system cursor name", "Wait Arrow"),
    nullptr
};

static const long gs_cp_es_syscursors_values[NUM_CURSORS] = {
    wxCURSOR_NONE,
    wxCURSOR_ARROW,
    wxCURSOR_RIGHT_ARROW,
    wxCURSOR_BLANK,
    wxCURSOR_BULLSEYE,
    wxCURSOR_CHAR,
    wxCURSOR_CROSS,
    wxCURSOR_HAND,
    wxCURSOR_IBEAM,
    wxCURSOR_LEFT_BUTTON,
    wxCURSOR_MAGNIFIER,
    wxCURSOR_MIDDLE_BUTTON,
    wxCURSOR_NO_ENTRY,
    wxCURSOR_PAINT_BRUSH,
    wxCURSOR_PENCIL,
    wxCURSOR_POINT_LEFT,
    wxCURSOR_POINT_RIGHT,
    wxCURSOR_QUESTION_ARROW,
    wxCURSOR_RIGHT_BUTTON,
    wxCURSOR_SIZENESW,
    wxCURSOR_SIZENS,
    wxCURSOR_SIZENWSE,
    wxCURSOR_SIZEWE,
    wxCURSOR_SIZING,
    wxCURSOR_SPRAYCAN,
    wxCURSOR_WAIT,
    wxCURSOR_WATCH,
    wxCURSOR_ARROWWAIT
};

wxIMPLEMENT_DYNAMIC_CLASS(wxCursorProperty, wxEnumProperty);

static wxPGChoices gs_wxCursorProperty_choicesCache;

wxCursorProperty::wxCursorProperty( const wxString& label, const wxString& name,
    int value )
    : wxEnumProperty( label,
                      name,
                      gs_cp_es_syscursors_labels,
                      gs_cp_es_syscursors_values,
                      &gs_wxCursorProperty_choicesCache,
                      value )
{
    m_flags |= wxPG_PROP_STATIC_CHOICES; // Cursor selection cannot be changed.
}

wxString wxCursorProperty::ValueToString(wxVariant& value, int argFlags) const
{
    return wxGetTranslation(wxEnumProperty::ValueToString(value, argFlags),
                            wxString(), "system cursor name");
}

wxSize wxCursorProperty::OnMeasureImage( int item ) const
{
#if wxPG_CAN_DRAW_CURSOR
    if ( item != -1 && item < NUM_CURSORS )
        return wxSize(wxPG_CURSOR_IMAGE_WIDTH,wxPG_CURSOR_IMAGE_WIDTH);
#else
    wxUnusedVar(item);
#endif
    return wxSize(0,0);
}

#if wxPG_CAN_DRAW_CURSOR

void wxCursorProperty::OnCustomPaint( wxDC& dc,
                                      const wxRect& rect,
                                      wxPGPaintData& paintdata )
{
    // Background brush
    dc.SetBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

    if ( paintdata.m_choiceItem >= 0 )
    {
        dc.DrawRectangle( rect );

        if ( paintdata.m_choiceItem < NUM_CURSORS )
        {
            wxStockCursor cursorIndex =
                (wxStockCursor) gs_cp_es_syscursors_values[paintdata.m_choiceItem];

            if ( cursorIndex == wxCURSOR_NONE )
                cursorIndex = wxCURSOR_ARROW;

            wxCursor cursor( cursorIndex );

#if defined(__WXMSW__) || ( defined(__WXGTK__) && defined(__WXGTK__) )
            wxBitmap bmp(cursor);
            if ( bmp.IsOk() )
            {
                dc.DrawBitmap(bmp, rect.GetTopLeft(), true);
            }
#endif
        }
    }
}

#else
void wxCursorProperty::OnCustomPaint( wxDC&, const wxRect&, wxPGPaintData& ) { }
/*wxPGCellRenderer* wxCursorProperty::GetCellRenderer( int column ) const
{
    return wxEnumProperty::GetCellRenderer(column);
}*/
#endif

// -----------------------------------------------------------------------
// wxImageFileProperty
// -----------------------------------------------------------------------

#if wxUSE_IMAGE

const wxString& wxPGGetDefaultImageWildcard()
{
    // Form the wildcard, if not done yet
    if ( wxPGGlobalVars->m_pDefaultImageWildcard.empty() )
    {

        wxString str;

        // TODO: This section may require locking (using global).

        wxList& handlers = wxImage::GetHandlers();

        // Let's iterate over the image handler list.
        for ( auto node : handlers )
        {
            wxImageHandler *handler = static_cast<wxImageHandler*>(node);

            wxString ext_lo = handler->GetExtension();
            wxString ext_up = ext_lo.Upper();

            str.append( ext_up );
            str.append( wxS(" files (*.") );
            str.append( ext_up );
            str.append( wxS(")|*.") );
            str.append( ext_lo );
            str.append( wxS("|") );
        }

        str.append ( wxS("All files (*.*)|*.*") );

        wxPGGlobalVars->m_pDefaultImageWildcard = str;
    }

    return wxPGGlobalVars->m_pDefaultImageWildcard;
}

wxIMPLEMENT_DYNAMIC_CLASS(wxImageFileProperty, wxFileProperty);

wxImageFileProperty::wxImageFileProperty( const wxString& label, const wxString& name,
    const wxString& value )
    : wxFileProperty(label,name,value)
{
    m_wildcard = wxPGGetDefaultImageWildcard();

    LoadImageFromFile();
}

void wxImageFileProperty::OnSetValue()
{
    wxFileProperty::OnSetValue();

    // Delete old image
    SetImage(wxNullImage);

    LoadImageFromFile();
}

void wxImageFileProperty::SetImage(const wxImage& img)
{
    m_image = img;
    m_bitmap = wxNullBitmap;
}

void wxImageFileProperty::LoadImageFromFile()
{
    wxFileName filename = GetFileName();

    // Cache the image
    if ( filename.FileExists() )
    {
        m_image.LoadFile(filename.GetFullPath());
    }
}

wxSize wxImageFileProperty::OnMeasureImage( int ) const
{
    return wxPG_DEFAULT_IMAGE_SIZE;
}

void wxImageFileProperty::OnCustomPaint( wxDC& dc,
                                         const wxRect& rect,
                                         wxPGPaintData& )
{
    if ( m_image.IsOk() )
    {
        // Draw the thumbnail
        // Create the bitmap here because required size is not known in OnSetValue().

        // Delete the cache if required size changed
        if ( m_bitmap.IsOk() && (m_bitmap.GetSize() != rect.GetSize()) )
        {
            m_bitmap = wxNullBitmap;
        }

        if ( !m_bitmap.IsOk() )
        {
            wxImage imgScaled = m_image;
            imgScaled.Rescale(rect.width, rect.height);
            m_bitmap = wxBitmap(imgScaled, dc);
        }
    }

    if ( m_bitmap.IsOk() )
    {
        dc.DrawBitmap(m_bitmap, rect.x, rect.y, false);
    }
    else
    {
        // No file - just draw a white box
        dc.SetBrush( *wxWHITE_BRUSH );
        dc.DrawRectangle ( rect );
    }
}

#endif // wxUSE_IMAGE

// -----------------------------------------------------------------------
// wxMultiChoiceProperty
// -----------------------------------------------------------------------

#if wxUSE_CHOICEDLG

#include "wx/choicdlg.h"

wxPG_IMPLEMENT_PROPERTY_CLASS(wxMultiChoiceProperty,wxEditorDialogProperty,
                              TextCtrlAndButton)

wxMultiChoiceProperty::wxMultiChoiceProperty( const wxString& label,
                                              const wxString& name,
                                              const wxPGChoices& choices,
                                              const wxArrayString& value)
    : wxEditorDialogProperty(label,name)
{
    m_dlgStyle = wxCHOICEDLG_STYLE;
    m_userStringMode = 0;
    m_choices.Assign(choices);
    SetValue(value);
}

wxMultiChoiceProperty::wxMultiChoiceProperty( const wxString& label,
                                              const wxString& name,
                                              const wxArrayString& strings,
                                              const wxArrayString& value)
    : wxEditorDialogProperty(label,name)
{
    m_dlgStyle = wxCHOICEDLG_STYLE;
    m_userStringMode = 0;
    m_choices.Set(strings);
    SetValue(value);
}

wxMultiChoiceProperty::wxMultiChoiceProperty( const wxString& label,
                                              const wxString& name,
                                              const wxArrayString& value)
: wxEditorDialogProperty(label,name)
{
    m_dlgStyle = wxCHOICEDLG_STYLE;
    m_userStringMode = 0;
    wxArrayString strings;
    m_choices.Set(strings);
    SetValue(value);
}

void wxMultiChoiceProperty::OnSetValue()
{
    m_display = GenerateValueAsString(m_value);
}

wxString wxMultiChoiceProperty::ValueToString( wxVariant& value,
                                               int argFlags ) const
{
    // If possible, use cached string
    if ( argFlags & wxPG_VALUE_IS_CURRENT )
        return m_display;

    return GenerateValueAsString(value);
}

wxString wxMultiChoiceProperty::GenerateValueAsString(const wxVariant& value) const
{
    wxArrayString strings;

    if ( value.IsType(wxPG_VARIANT_TYPE_ARRSTRING) )
        strings = value.GetArrayString();

    const size_t itemCount = strings.size();

    wxString valStr;

    for ( size_t i = 0; i < itemCount; i++ )
    {
        valStr.append("\"");
        valStr.append(strings[i]);
        valStr.append("\"");
        if ( i < (itemCount - 1) )
            valStr.append(" ");
    }

    return valStr;
}

wxArrayInt wxMultiChoiceProperty::GetValueAsIndices() const
{
    wxVariant variant = GetValue();
    const wxArrayInt& valueArr = wxArrayIntRefFromVariant(variant);

    // Translate values to string indices.
    wxArrayInt selections;

    if ( !m_choices.IsOk() || !m_choices.GetCount() )
    {
        selections.Add(-1, valueArr.size());
    }
    else
    {
        for( int val : valueArr )
        {
            int sIndex = m_choices.Index(val);
            if ( sIndex >= 0 )
                selections.Add(sIndex);
        }
    }

    return selections;
}

bool wxMultiChoiceProperty::DisplayEditorDialog(wxPropertyGrid* pg, wxVariant& value)
{
    wxASSERT_MSG(value.IsType(wxPG_VARIANT_TYPE_ARRSTRING), "Function called for incompatible property");
    if ( !m_choices.IsOk() )
    {
        return false;
    }

    wxArrayString labels = m_choices.GetLabels();
    unsigned int choiceCount = m_choices.GetCount();

    // launch editor dialog
    wxMultiChoiceDialog dlg( pg->GetPanel(),
                             _("Make a selection:"),
                             m_dlgTitle.empty() ? GetLabel() : m_dlgTitle,
                             choiceCount,
                             choiceCount?&labels[0]:nullptr,
                             m_dlgStyle );

    dlg.Move( pg->GetGoodEditorDialogPosition(this,dlg.GetSize()) );

    wxArrayString strings = value.GetArrayString();
    wxArrayString extraStrings;

    dlg.SetSelections(m_choices.GetIndicesForStrings(strings, &extraStrings));

    if ( dlg.ShowModal() == wxID_OK && choiceCount )
    {
        wxArrayInt arrInt = dlg.GetSelections();

        // Strings that were not in list of choices
        wxArrayString newValue;

        // Translate string indices to strings

        if ( m_userStringMode == 1 )
        {
            for( wxString& str : extraStrings )
                newValue.push_back(str);
        }

        for( int ind : arrInt )
            newValue.push_back(m_choices.GetLabel(ind));

        if ( m_userStringMode == 2 )
        {
            for ( wxString& str : extraStrings )
                newValue.push_back(str);
        }

        value = wxVariant(newValue);
        return true;
    }
    return false;
}

bool wxMultiChoiceProperty::StringToValue( wxVariant& variant, const wxString& text, int ) const
{
    wxArrayString arr;

    WX_PG_TOKENIZER2_BEGIN(text,wxT('"'))
        if ( m_userStringMode > 0 || (m_choices.IsOk() && m_choices.Index( token ) != wxNOT_FOUND) )
            arr.Add(token);
    WX_PG_TOKENIZER2_END()

    variant = wxVariant(arr);

    return true;
}

bool wxMultiChoiceProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_ATTR_MULTICHOICE_USERSTRINGMODE )
    {
        m_userStringMode = (int)value.GetLong();
        return true;
    }
    return wxEditorDialogProperty::DoSetAttribute(name, value);
}

#endif // wxUSE_CHOICEDLG


// -----------------------------------------------------------------------
// wxDateProperty
// -----------------------------------------------------------------------

#if wxUSE_DATETIME


#if wxUSE_DATEPICKCTRL
    #define dtCtrl      DatePickerCtrl
#else
    #define dtCtrl      TextCtrl
#endif

wxPG_IMPLEMENT_PROPERTY_CLASS(wxDateProperty,wxPGProperty,
                              dtCtrl)


wxString wxDateProperty::ms_defaultDateFormat;


wxDateProperty::wxDateProperty( const wxString& label,
                                const wxString& name,
                                const wxDateTime& value )
    : wxPGProperty(label,name)
{
    //wxPGRegisterDefaultValueType(wxDateTime)

#if wxUSE_DATEPICKCTRL
    wxPGRegisterEditorClass(DatePickerCtrl);

    m_dpStyle = wxDP_DEFAULT | wxDP_SHOWCENTURY;
#else
    m_dpStyle = 0;
#endif

    SetValue( value );
}

void wxDateProperty::OnSetValue()
{
    //
    // Convert invalid dates to unspecified value
    if ( m_value.IsType(wxPG_VARIANT_TYPE_DATETIME) )
    {
        if ( !m_value.GetDateTime().IsValid() )
            m_value.MakeNull();
    }
}

bool wxDateProperty::StringToValue( wxVariant& variant, const wxString& text,
                                    int WXUNUSED(argFlags) ) const
{
    wxDateTime dt;

    // FIXME: do we really want to return true from here if only part of the
    //        string was parsed?
    const char* c = dt.ParseFormat(text);

    if ( c )
    {
        variant = dt;
        return true;
    }

    return false;
}

wxString wxDateProperty::ValueToString( wxVariant& value,
                                        int argFlags ) const
{
    wxDateTime dateTime = value.GetDateTime();

    if ( !dateTime.IsValid() )
        return wxS("Invalid");

    if ( ms_defaultDateFormat.empty() )
    {
#if wxUSE_DATEPICKCTRL
        bool showCentury = (m_dpStyle & wxDP_SHOWCENTURY) != 0;
#else
        bool showCentury = true;
#endif
        ms_defaultDateFormat = DetermineDefaultDateFormat( showCentury );
    }

    wxString format;
    if ( !m_format.empty() &&
         !(argFlags & wxPG_FULL_VALUE) )
            format = m_format;

    // Determine default from locale
    if ( format.empty() )
        format = ms_defaultDateFormat;

    return dateTime.Format(format);
}

wxString wxDateProperty::DetermineDefaultDateFormat( bool showCentury )
{
    // This code is based on datectlg.cpp's GetLocaleDateFormat()
#if wxUSE_INTL
    wxString format = wxUILocale::GetCurrent().GetInfo(wxLOCALE_SHORT_DATE_FMT);
    if ( showCentury )
        format.Replace(wxS("%y"), wxS("%Y"));
    else
        format.Replace(wxS("%Y"), wxS("%y"));

    return format;
#else
    return wxS("%x");
#endif // wxUSE_INTL/!wxUSE_INTL
}

bool wxDateProperty::DoSetAttribute( const wxString& name, wxVariant& value )
{
    if ( name == wxPG_DATE_FORMAT )
    {
        m_format = value.GetString();
        return true;
    }
    else if ( name == wxPG_DATE_PICKER_STYLE )
    {
        m_dpStyle = value.GetLong();
        ms_defaultDateFormat.clear();  // This may need recalculation
        return true;
    }
    return wxPGProperty::DoSetAttribute(name, value);
}

#endif  // wxUSE_DATETIME


// -----------------------------------------------------------------------
// wxPropertyGridInterface
// -----------------------------------------------------------------------

void wxPropertyGridInterface::InitAllTypeHandlers()
{
}

// -----------------------------------------------------------------------

void wxPropertyGridInterface::RegisterAdditionalEditors()
{
    // Register editor classes, if necessary.
    if ( wxPGGlobalVars->m_mapEditorClasses.empty() )
        wxPropertyGrid::RegisterDefaultEditors();

#if wxUSE_SPINBTN
    wxPGRegisterEditorClass(SpinCtrl);
#endif
#if wxUSE_DATEPICKCTRL
    wxPGRegisterEditorClass(DatePickerCtrl);
#endif
}

// -----------------------------------------------------------------------

#endif  // wxPG_INCLUDE_ADVPROPS

#endif  // wxUSE_PROPGRID

