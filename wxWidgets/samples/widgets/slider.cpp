/////////////////////////////////////////////////////////////////////////////
// Program:     wxWidgets Widgets Sample
// Name:        slider.cpp
// Purpose:     Part of the widgets sample showing wxSlider
// Author:      Vadim Zeitlin
// Created:     16.04.01
// Id:          $Id$
// Copyright:   (c) 2001 Vadim Zeitlin
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_SLIDER

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/log.h"

    #include "wx/bitmap.h"
    #include "wx/button.h"
    #include "wx/checkbox.h"
    #include "wx/radiobox.h"
    #include "wx/slider.h"
    #include "wx/statbox.h"
    #include "wx/textctrl.h"
#endif

#if wxUSE_TOOLTIPS
    #include "wx/tooltip.h"
#endif

#include "wx/sizer.h"

#include "widgets.h"

#include "icons/slider.xpm"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control ids
enum
{
    SliderPage_Reset = wxID_HIGHEST,
    SliderPage_Clear,
    SliderPage_SetValue,
    SliderPage_SetMinAndMax,
    SliderPage_SetLineSize,
    SliderPage_SetPageSize,
    SliderPage_SetTickFreq,
    SliderPage_SetThumbLen,
    SliderPage_CurValueText,
    SliderPage_ValueText,
    SliderPage_MinText,
    SliderPage_MaxText,
    SliderPage_LineSizeText,
    SliderPage_PageSizeText,
    SliderPage_TickFreqText,
    SliderPage_ThumbLenText,
    SliderPage_RadioSides,
    SliderPage_BothSides,
    SliderPage_Slider
};

// sides radiobox values
enum
{
    SliderTicks_Top,
    SliderTicks_Bottom,
    SliderTicks_Left,
    SliderTicks_Right
};

// ----------------------------------------------------------------------------
// SliderWidgetsPage
// ----------------------------------------------------------------------------

class SliderWidgetsPage : public WidgetsPage
{
public:
    SliderWidgetsPage(WidgetsBookCtrl *book, wxImageList *imaglist);
    virtual ~SliderWidgetsPage(){};

    virtual wxControl *GetWidget() const { return m_slider; }
    virtual void RecreateWidget() { CreateSlider(); }

    // lazy creation of the content
    virtual void CreateContent();

protected:
    // event handlers
    void OnButtonReset(wxCommandEvent& event);
    void OnButtonClear(wxCommandEvent& event);
    void OnButtonSetValue(wxCommandEvent& event);
    void OnButtonSetMinAndMax(wxCommandEvent& event);
    void OnButtonSetLineSize(wxCommandEvent& event);
    void OnButtonSetPageSize(wxCommandEvent& event);
    void OnButtonSetTickFreq(wxCommandEvent& event);
    void OnButtonSetThumbLen(wxCommandEvent& event);

    void OnCheckOrRadioBox(wxCommandEvent& event);

    void OnSlider(wxScrollEvent& event);

    void OnUpdateUIValueButton(wxUpdateUIEvent& event);
    void OnUpdateUIMinMaxButton(wxUpdateUIEvent& event);
    void OnUpdateUILineSize(wxUpdateUIEvent& event);
    void OnUpdateUIPageSize(wxUpdateUIEvent& event);
    void OnUpdateUITickFreq(wxUpdateUIEvent& event);
    void OnUpdateUIThumbLen(wxUpdateUIEvent& event);
    void OnUpdateUIRadioSides(wxUpdateUIEvent& event);
    void OnUpdateUIBothSides(wxUpdateUIEvent& event);

    void OnUpdateUIResetButton(wxUpdateUIEvent& event);

    void OnUpdateUICurValueText(wxUpdateUIEvent& event);

    // reset the slider parameters
    void Reset();

    // (re)create the slider
    void CreateSlider();

    // set the line size from the text field value
    void DoSetLineSize();

    // set the page size from the text field value
    void DoSetPageSize();

    // set the tick frequency from the text field value
    void DoSetTickFreq();

    // set the thumb len from the text field value
    void DoSetThumbLen();

    // is this slider value in range?
    bool IsValidValue(int val) const
        { return (val >= m_min) && (val <= m_max); }

    // the slider range
    int m_min, m_max;

    // the controls
    // ------------

    // the check/radio boxes for styles
    wxCheckBox *m_chkLabels,
               *m_chkInverse,
               *m_chkTicks,
               *m_chkBothSides;

    wxRadioBox *m_radioSides;

    // the slider itself and the sizer it is in
    wxSlider *m_slider;
    wxSizer *m_sizerSlider;

    // the text entries for set value/range
    wxTextCtrl *m_textValue,
               *m_textMin,
               *m_textMax,
               *m_textLineSize,
               *m_textPageSize,
               *m_textTickFreq,
               *m_textThumbLen;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_WIDGETS_PAGE(SliderWidgetsPage)
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(SliderWidgetsPage, WidgetsPage)
    EVT_BUTTON(SliderPage_Reset, SliderWidgetsPage::OnButtonReset)
    EVT_BUTTON(SliderPage_SetValue, SliderWidgetsPage::OnButtonSetValue)
    EVT_BUTTON(SliderPage_SetMinAndMax, SliderWidgetsPage::OnButtonSetMinAndMax)
    EVT_BUTTON(SliderPage_SetLineSize, SliderWidgetsPage::OnButtonSetLineSize)
    EVT_BUTTON(SliderPage_SetPageSize, SliderWidgetsPage::OnButtonSetPageSize)
    EVT_BUTTON(SliderPage_SetTickFreq, SliderWidgetsPage::OnButtonSetTickFreq)
    EVT_BUTTON(SliderPage_SetThumbLen, SliderWidgetsPage::OnButtonSetThumbLen)

    EVT_UPDATE_UI(SliderPage_SetValue, SliderWidgetsPage::OnUpdateUIValueButton)
    EVT_UPDATE_UI(SliderPage_SetMinAndMax, SliderWidgetsPage::OnUpdateUIMinMaxButton)
    EVT_UPDATE_UI(SliderPage_SetLineSize, SliderWidgetsPage::OnUpdateUILineSize)
    EVT_UPDATE_UI(SliderPage_SetPageSize, SliderWidgetsPage::OnUpdateUIPageSize)
    EVT_UPDATE_UI(SliderPage_SetTickFreq, SliderWidgetsPage::OnUpdateUITickFreq)
    EVT_UPDATE_UI(SliderPage_SetThumbLen, SliderWidgetsPage::OnUpdateUIThumbLen)
    EVT_UPDATE_UI(SliderPage_RadioSides, SliderWidgetsPage::OnUpdateUIRadioSides)
    EVT_UPDATE_UI(SliderPage_BothSides, SliderWidgetsPage::OnUpdateUIBothSides)

    EVT_UPDATE_UI(SliderPage_Reset, SliderWidgetsPage::OnUpdateUIResetButton)

    EVT_UPDATE_UI(SliderPage_CurValueText, SliderWidgetsPage::OnUpdateUICurValueText)

    EVT_COMMAND_SCROLL(SliderPage_Slider, SliderWidgetsPage::OnSlider)

    EVT_CHECKBOX(wxID_ANY, SliderWidgetsPage::OnCheckOrRadioBox)
    EVT_RADIOBOX(wxID_ANY, SliderWidgetsPage::OnCheckOrRadioBox)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

#if defined(__WXUNIVERSAL__)
    #define FAMILY_CTRLS UNIVERSAL_CTRLS
#else
    #define FAMILY_CTRLS NATIVE_CTRLS
#endif

IMPLEMENT_WIDGETS_PAGE(SliderWidgetsPage, _T("Slider"), FAMILY_CTRLS );

SliderWidgetsPage::SliderWidgetsPage(WidgetsBookCtrl *book,
                                     wxImageList *imaglist)
                  : WidgetsPage(book, imaglist, slider_xpm)
{
    // init everything
    m_min = 0;
    m_max = 100;

    m_chkInverse =
    m_chkTicks =
    m_chkLabels =
    m_chkBothSides = (wxCheckBox *)NULL;

    m_radioSides = (wxRadioBox *)NULL;

    m_slider = (wxSlider *)NULL;
    m_sizerSlider = (wxSizer *)NULL;
}

void SliderWidgetsPage::CreateContent()
{
    wxSizer *sizerTop = new wxBoxSizer(wxHORIZONTAL);

    // left pane
    wxStaticBox *box = new wxStaticBox(this, wxID_ANY, _T("&Set style"));
    wxSizer *sizerLeft = new wxStaticBoxSizer(box, wxVERTICAL);

    m_chkInverse = CreateCheckBoxAndAddToSizer(sizerLeft, _T("&Inverse"));
    m_chkTicks = CreateCheckBoxAndAddToSizer(sizerLeft, _T("Show &ticks"));
    m_chkLabels = CreateCheckBoxAndAddToSizer(sizerLeft, _T("Show &labels"));
    static const wxString sides[] =
    {
        _T("top"),
        _T("bottom"),
        _T("left"),
        _T("right"),
    };
    m_radioSides = new wxRadioBox(this, SliderPage_RadioSides, _T("&Ticks/Labels"),
                                 wxDefaultPosition, wxDefaultSize,
                                 WXSIZEOF(sides), sides,
                                 1, wxRA_SPECIFY_COLS);
    sizerLeft->Add(m_radioSides, 0, wxGROW | wxALL, 5);
    m_chkBothSides = CreateCheckBoxAndAddToSizer
                     (sizerLeft, _T("&Both sides"), SliderPage_BothSides);
#if wxUSE_TOOLTIPS
    m_chkBothSides->SetToolTip( _T("\"Both sides\" is only supported \nin Win95 and Universal") );
#endif // wxUSE_TOOLTIPS

    sizerLeft->Add(5, 5, 0, wxGROW | wxALL, 5); // spacer

    wxButton *btn = new wxButton(this, SliderPage_Reset, _T("&Reset"));
    sizerLeft->Add(btn, 0, wxALIGN_CENTRE_HORIZONTAL | wxALL, 15);

    // middle pane
    wxStaticBox *box2 = new wxStaticBox(this, wxID_ANY, _T("&Change slider value"));
    wxSizer *sizerMiddle = new wxStaticBoxSizer(box2, wxVERTICAL);

    wxTextCtrl *text;
    wxSizer *sizerRow = CreateSizerWithTextAndLabel(_T("Current value"),
                                                    SliderPage_CurValueText,
                                                    &text);
    text->SetEditable(false);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SliderPage_SetValue,
                                            _T("Set &value"),
                                            SliderPage_ValueText,
                                            &m_textValue);
    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SliderPage_SetMinAndMax,
                                            _T("&Min and max"),
                                            SliderPage_MinText,
                                            &m_textMin);

    m_textMax = new wxTextCtrl(this, SliderPage_MaxText, wxEmptyString);
    sizerRow->Add(m_textMax, 1, wxLEFT | wxALIGN_CENTRE_VERTICAL, 5);

    m_textMin->SetValue( wxString::Format(_T("%d"), m_min) );
    m_textMax->SetValue( wxString::Format(_T("%d"), m_max) );

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SliderPage_SetLineSize,
                                            _T("Li&ne size"),
                                            SliderPage_LineSizeText,
                                            &m_textLineSize);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SliderPage_SetPageSize,
                                            _T("P&age size"),
                                            SliderPage_PageSizeText,
                                            &m_textPageSize);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SliderPage_SetTickFreq,
                                            _T("Tick &frequency"),
                                            SliderPage_TickFreqText,
                                            &m_textTickFreq);

    m_textTickFreq->SetValue(_T("10"));

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    sizerRow = CreateSizerWithTextAndButton(SliderPage_SetThumbLen,
                                            _T("Thumb &length"),
                                            SliderPage_ThumbLenText,
                                            &m_textThumbLen);

    sizerMiddle->Add(sizerRow, 0, wxALL | wxGROW, 5);

    // right pane
    wxSizer *sizerRight = new wxBoxSizer(wxHORIZONTAL);
    sizerRight->SetMinSize(150, 40);
    m_sizerSlider = sizerRight; // save it to modify it later

    Reset();
    CreateSlider();

    m_textLineSize->SetValue(wxString::Format(_T("%d"), m_slider->GetLineSize()));
    m_textPageSize->SetValue(wxString::Format(_T("%d"), m_slider->GetPageSize()));

    // the 3 panes panes compose the window
    sizerTop->Add(sizerLeft, 0, wxGROW | (wxALL & ~wxLEFT), 10);
    sizerTop->Add(sizerMiddle, 0, wxGROW | wxALL, 10);
    sizerTop->Add(sizerRight, 1, wxGROW | (wxALL & ~wxRIGHT), 10);

    // final initializations
    SetSizer(sizerTop);
}

// ----------------------------------------------------------------------------
// operations
// ----------------------------------------------------------------------------

void SliderWidgetsPage::Reset()
{
    m_chkInverse->SetValue(false);
    m_chkTicks->SetValue(true);
    m_chkLabels->SetValue(true);
    m_chkBothSides->SetValue(false);

    m_radioSides->SetSelection(SliderTicks_Top);
}

void SliderWidgetsPage::CreateSlider()
{
    int flags = ms_defaultFlags;

    if ( m_chkInverse->GetValue() )
    {
        flags |= wxSL_INVERSE;
    }

    if ( m_chkLabels->GetValue() )
    {
        flags |= wxSL_LABELS;
    }

    if ( m_chkTicks->GetValue() )
    {
        flags |= wxSL_AUTOTICKS;
    }

    switch ( m_radioSides->GetSelection() )
    {
        case SliderTicks_Top:
            flags |= wxSL_TOP;
            break;

        case SliderTicks_Left:
            flags |= wxSL_LEFT;
            break;

        case SliderTicks_Bottom:
            flags |= wxSL_BOTTOM;
            break;

        case SliderTicks_Right:
            flags |= wxSL_RIGHT;
            break;

        default:
            wxFAIL_MSG(_T("unexpected radiobox selection"));
            // fall through
    }

    if ( m_chkBothSides->GetValue() )
    {
        flags |= wxSL_BOTH;
    }

    int val = m_min;
    if ( m_slider )
    {
        int valOld = m_slider->GetValue();
        if ( !IsValidValue(valOld) )
        {
            val = valOld;
        }

        m_sizerSlider->Detach( m_slider );

        if ( m_sizerSlider->GetChildren().GetCount() )
        {
            // we have 2 spacers, remove them too
            m_sizerSlider->Remove( 0 );
            m_sizerSlider->Remove( 0 );
        }

        delete m_slider;
    }

    m_slider = new wxSlider(this, SliderPage_Slider,
                            val, m_min, m_max,
                            wxDefaultPosition, wxDefaultSize,
                            flags);

    if ( m_slider->HasFlag(wxSL_VERTICAL) )
    {
        m_sizerSlider->Add(0, 0, 1);
        m_sizerSlider->Add(m_slider, 0, wxGROW | wxALL, 5);
        m_sizerSlider->Add(0, 0, 1);
    }
    else
    {
        m_sizerSlider->Add(m_slider, 1, wxCENTRE | wxALL, 5);
    }

    if ( m_chkTicks->GetValue() )
    {
        DoSetTickFreq();
    }

    m_sizerSlider->Layout();
}

void SliderWidgetsPage::DoSetLineSize()
{
    long lineSize;
    if ( !m_textLineSize->GetValue().ToLong(&lineSize) )
    {
        wxLogWarning(_T("Invalid slider line size"));

        return;
    }

    m_slider->SetLineSize(lineSize);

    if ( m_slider->GetLineSize() != lineSize )
    {
        wxLogWarning(_T("Invalid line size in slider."));
    }
}

void SliderWidgetsPage::DoSetPageSize()
{
    long pageSize;
    if ( !m_textPageSize->GetValue().ToLong(&pageSize) )
    {
        wxLogWarning(_T("Invalid slider page size"));

        return;
    }

    m_slider->SetPageSize(pageSize);

    if ( m_slider->GetPageSize() != pageSize )
    {
        wxLogWarning(_T("Invalid page size in slider."));
    }
}

void SliderWidgetsPage::DoSetTickFreq()
{
    long freq;
    if ( !m_textTickFreq->GetValue().ToLong(&freq) )
    {
        wxLogWarning(_T("Invalid slider tick frequency"));

        return;
    }

    m_slider->SetTickFreq(freq, 0 /* unused */);
}

void SliderWidgetsPage::DoSetThumbLen()
{
    long len;
    if ( !m_textThumbLen->GetValue().ToLong(&len) )
    {
        wxLogWarning(_T("Invalid slider thumb length"));

        return;
    }

    m_slider->SetThumbLength(len);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void SliderWidgetsPage::OnButtonReset(wxCommandEvent& WXUNUSED(event))
{
    Reset();

    CreateSlider();
}

void SliderWidgetsPage::OnButtonSetLineSize(wxCommandEvent& WXUNUSED(event))
{
    DoSetLineSize();
}

void SliderWidgetsPage::OnButtonSetPageSize(wxCommandEvent& WXUNUSED(event))
{
    DoSetPageSize();
}

void SliderWidgetsPage::OnButtonSetTickFreq(wxCommandEvent& WXUNUSED(event))
{
    DoSetTickFreq();
}

void SliderWidgetsPage::OnButtonSetThumbLen(wxCommandEvent& WXUNUSED(event))
{
    DoSetThumbLen();
}

void SliderWidgetsPage::OnButtonSetMinAndMax(wxCommandEvent& WXUNUSED(event))
{
    long minNew,
         maxNew = 0; // init to suppress compiler warning
    if ( !m_textMin->GetValue().ToLong(&minNew) ||
         !m_textMax->GetValue().ToLong(&maxNew) ||
         minNew >= maxNew )
    {
        wxLogWarning(_T("Invalid min/max values for the slider."));

        return;
    }

    m_min = minNew;
    m_max = maxNew;

    m_slider->SetRange(minNew, maxNew);

    if ( m_slider->GetMin() != m_min ||
         m_slider->GetMax() != m_max )
    {
        wxLogWarning(_T("Invalid range in slider."));
    }
}

void SliderWidgetsPage::OnButtonSetValue(wxCommandEvent& WXUNUSED(event))
{
    long val;
    if ( !m_textValue->GetValue().ToLong(&val) || !IsValidValue(val) )
    {
        wxLogWarning(_T("Invalid slider value."));

        return;
    }

    m_slider->SetValue(val);
}

void SliderWidgetsPage::OnUpdateUIValueButton(wxUpdateUIEvent& event)
{
    long val;
    event.Enable( m_textValue->GetValue().ToLong(&val) && IsValidValue(val) );
}

void SliderWidgetsPage::OnUpdateUILineSize(wxUpdateUIEvent& event)
{
    long lineSize;
    event.Enable( m_textLineSize->GetValue().ToLong(&lineSize) &&
            (lineSize > 0) && (lineSize <= m_max - m_min) );
}

void SliderWidgetsPage::OnUpdateUIPageSize(wxUpdateUIEvent& event)
{
    long pageSize;
    event.Enable( m_textPageSize->GetValue().ToLong(&pageSize) &&
            (pageSize > 0) && (pageSize <= m_max - m_min) );
}

void SliderWidgetsPage::OnUpdateUITickFreq(wxUpdateUIEvent& event)
{
    long freq;
    event.Enable( m_chkTicks->GetValue() &&
                  m_textTickFreq->GetValue().ToLong(&freq) &&
                  (freq > 0) && (freq <= m_max - m_min) );
}

void SliderWidgetsPage::OnUpdateUIThumbLen(wxUpdateUIEvent& event)
{
    long val;
    event.Enable( m_textThumbLen->GetValue().ToLong(&val));
}

void SliderWidgetsPage::OnUpdateUIMinMaxButton(wxUpdateUIEvent& event)
{
    long mn, mx;
    event.Enable( m_textMin->GetValue().ToLong(&mn) &&
                  m_textMax->GetValue().ToLong(&mx) &&
                  mn < mx);
}

void SliderWidgetsPage::OnUpdateUIResetButton(wxUpdateUIEvent& event)
{
    event.Enable( m_chkInverse->GetValue() ||
                  !m_chkTicks->GetValue() ||
                  !m_chkLabels->GetValue() ||
                  m_chkBothSides->GetValue() ||
                  m_radioSides->GetSelection() != SliderTicks_Top );
}

void SliderWidgetsPage::OnCheckOrRadioBox(wxCommandEvent& WXUNUSED(event))
{
    CreateSlider();
}

void SliderWidgetsPage::OnUpdateUICurValueText(wxUpdateUIEvent& event)
{
    event.SetText( wxString::Format(_T("%d"), m_slider->GetValue()) );
}

void SliderWidgetsPage::OnUpdateUIRadioSides(wxUpdateUIEvent& event)
{
    event.Enable( m_chkLabels->GetValue() || m_chkTicks->GetValue() );
}

void SliderWidgetsPage::OnUpdateUIBothSides(wxUpdateUIEvent& event)
{
#if defined(__WXMSW__) || defined(__WXUNIVERSAL__)
    event.Enable( m_chkTicks->GetValue() );
#else
    event.Enable( false );
#endif // defined(__WXMSW__) || defined(__WXUNIVERSAL__)
}

void SliderWidgetsPage::OnSlider(wxScrollEvent& event)
{
    wxASSERT_MSG( event.GetInt() == m_slider->GetValue(),
                  wxT("slider value should be the same") );

    wxEventType eventType = event.GetEventType();

    /*
        This array takes the EXACT order of the declarations in
        include/wx/event.h
        (section "wxScrollBar and wxSlider event identifiers")
    */
    static const wxChar *eventNames[] =
    {
        wxT("wxEVT_SCROLL_TOP"),
        wxT("wxEVT_SCROLL_BOTTOM"),
        wxT("wxEVT_SCROLL_LINEUP"),
        wxT("wxEVT_SCROLL_LINEDOWN"),
        wxT("wxEVT_SCROLL_PAGEUP"),
        wxT("wxEVT_SCROLL_PAGEDOWN"),
        wxT("wxEVT_SCROLL_THUMBTRACK"),
        wxT("wxEVT_SCROLL_THUMBRELEASE"),
        wxT("wxEVT_SCROLL_CHANGED")
    };

    int index = eventType - wxEVT_SCROLL_TOP;

    /*
        If this assert is triggered, there is an unknown slider event which
        should be added to the above eventNames array.
    */
    wxASSERT_MSG(index >= 0 && (size_t)index < WXSIZEOF(eventNames),
                 wxT("Unknown slider event") );


    static int s_numSliderEvents = 0;

    wxLogMessage(wxT("Slider event #%d: %s (pos = %d, int value = %d)"),
                 s_numSliderEvents++,
                 eventNames[index],
                 event.GetPosition(),
                 event.GetInt());
}

#endif // wxUSE_SLIDER
