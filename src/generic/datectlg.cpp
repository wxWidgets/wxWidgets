/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/datectlg.cpp
// Purpose:     generic wxDatePickerCtrlGeneric implementation
// Author:      Andreas Pflug
// Modified by:
// Created:     2005-01-19
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Andreas Pflug <pgadmin@pse-consulting.de>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DATEPICKCTRL

#include "wx/datectrl.h"

// use this version if we're explicitly requested to do it or if it's the only
// one we have
#if !defined(wxHAS_NATIVE_DATEPICKCTRL) || \
        (defined(wxUSE_DATEPICKCTRL_GENERIC) && wxUSE_DATEPICKCTRL_GENERIC)

#ifndef WX_PRECOMP
    #include "wx/bmpbuttn.h"
    #include "wx/dialog.h"
    #include "wx/dcmemory.h"
    #include "wx/panel.h"
    #include "wx/textctrl.h"
    #include "wx/valtext.h"
#endif

#ifdef wxHAS_NATIVE_DATEPICKCTRL
    // this header is not included from wx/datectrl.h if we have a native
    // version, but we do need it here
    #include "wx/generic/datectrl.h"
#else
    // we need to define _WX_DEFINE_DATE_EVENTS_ before including wx/dateevt.h to
    // define the event types we use if we're the only date picker control version
    // being compiled -- otherwise it's defined in the native version implementation
    #define _WX_DEFINE_DATE_EVENTS_
#endif

#include "wx/dateevt.h"

#include "wx/calctrl.h"
#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    CTRLID_TXT = 101,
    CTRLID_CAL,
    CTRLID_BTN,
    CTRLID_PAN
};

#ifndef DEFAULT_ITEM_WIDTH
    #define DEFAULT_ITEM_WIDTH 100
#endif

#if defined(__WXMSW__)
    #undef wxUSE_POPUPWIN
    #define wxUSE_POPUPWIN    0  // Popup not working
    #define TXTCTRL_FLAGS     wxNO_BORDER
    #define CALBORDER         0
    #define TXTPOSY           1
#elif defined(__WXGTK__)
    #define TXTCTRL_FLAGS     0
    #define CALBORDER         4
    #define TXTPOSY           0
#else
    #define TXTCTRL_FLAGS     0
    #define CALBORDER         4
    #define TXTPOSY           0
#endif

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// this should have been a flag in wxDatePickerCtrlGeneric itself but adding it
// there now would break backwards compatibility, so put it here as a global:
// this shouldn't be a big problem as only one (GUI) thread normally can call
// wxDatePickerCtrlGeneric::SetValue() and so it can be only ever used for one
// control at a time
//
// if the value is not NULL, it points to the control which is inside SetValue()
static wxDatePickerCtrlGeneric *gs_inSetValue = NULL;

// ----------------------------------------------------------------------------
// local classes
// ----------------------------------------------------------------------------

// This flag indicates that combo box style drop button is to be created
#define wxBU_COMBO          0x0400


class wxDropdownButton : public wxBitmapButton
{
public:
    wxDropdownButton() { Init(); }
    wxDropdownButton(wxWindow *parent,
                     wxWindowID id,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style=0,
                     const wxValidator& validator = wxDefaultValidator);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator);

protected:
    virtual void DoMoveWindow(int x, int y, int w, int h);

    void OnSize(wxSizeEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    void RecreateBitmaps(int w, int h);

    wxBitmap    m_bmpNormal;
    wxBitmap    m_bmpHot;

    int         m_borderX, m_borderY;

    // True if DrawDropArrow should be used instead of DrawComboBoxDropButton
    bool        m_useDropArrow;

private:

    void Init()
    {
        m_borderX = -1;
        m_borderY = -1;
    }

    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDropdownButton)
};


// Below, macro DROPBUT_USEDROPARROW should return false when
// DrawComboBoxDropButton is to be used to render the entire button.
// COMBOST is non-zero if wxBU_COMBO was set.

#if defined(__WXMSW__)

    #define DROPBUT_USEDROPARROW(COMBOST)   (COMBOST?false:true)
    #define DROPBUT_DEFAULT_WIDTH           17

#elif defined(__WXGTK__)

    #define DROPBUT_USEDROPARROW(COMBOST)   true
    #define DROPBUT_DEFAULT_WIDTH           19

#else

    #define DROPBUT_USEDROPARROW(COMBOST)   true
    #define DROPBUT_DEFAULT_WIDTH           17

#endif


IMPLEMENT_DYNAMIC_CLASS(wxDropdownButton, wxBitmapButton)


BEGIN_EVENT_TABLE(wxDropdownButton,wxBitmapButton)
    EVT_ENTER_WINDOW(wxDropdownButton::OnMouseEnter)
    EVT_LEAVE_WINDOW(wxDropdownButton::OnMouseLeave)
    EVT_SIZE(wxDropdownButton::OnSize)
END_EVENT_TABLE()


wxDropdownButton::wxDropdownButton(wxWindow *parent,
                                   wxWindowID id,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style,
                                   const wxValidator& validator)
{
    Init();
    Create(parent, id, pos, size, style, validator);
}


bool wxDropdownButton::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxPoint& pos,
                              const wxSize& size,
                              long style,
                              const wxValidator& validator)
{
    m_marginX = 0;
    m_marginY = 0;

    m_useDropArrow = DROPBUT_USEDROPARROW(style & wxBU_COMBO);

    wxBitmap chkBmp(15,15);  // arbitrary
    if ( !wxBitmapButton::Create(parent, id, chkBmp,
                                 pos, wxDefaultSize,
                                 style | (m_useDropArrow ? wxBU_AUTODRAW : wxNO_BORDER),
                                 validator) )
        return false;

    const wxSize sz = GetSize();
    int w = chkBmp.GetWidth(),
        h = chkBmp.GetHeight();
    m_borderX = sz.x - m_marginX - w;
    m_borderY = sz.y - m_marginY - h;

    DoMoveWindow(pos.x, pos.y, size.x, size.y);

    return true;
}


void wxDropdownButton::RecreateBitmaps(int w, int h)
{
    wxMemoryDC dc;

    int borderX = m_marginX + m_borderX;
    int borderY = m_marginY + m_borderY;
    int bw = w - borderX;
    int bh = h - borderY;

    wxBitmap bmp(bw, bh);
    wxBitmap bmpSel(bw, bh);
    wxRect r(0,0,w,h);

    wxRendererNative& renderer = wxRendererNative::Get();

    dc.SelectObject(bmp);

    if ( m_useDropArrow )
    {
        // Use DrawDropArrow on transparent background.

        wxColour magic(255,0,255);
        wxBrush magicBrush(magic);
        r.x = -(borderX/2);
        r.y = -(borderY/2);

        dc.SetBrush( magicBrush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(0,0,bw,bh);
        renderer.DrawDropArrow(this, dc, r);
        dc.SelectObject( wxNullBitmap );
        wxMask *mask = new wxMask( bmp, magic );
        bmp.SetMask( mask );

        dc.SelectObject(bmpSel);

        dc.SetBrush( magicBrush );
        dc.SetPen( *wxTRANSPARENT_PEN );
        dc.DrawRectangle(0,0,bw,bh);
        renderer.DrawDropArrow(this, dc, r, wxCONTROL_PRESSED);
        dc.SelectObject( wxNullBitmap );
        mask = new wxMask( bmpSel, magic );
        bmpSel.SetMask( mask );
    }
    else
    {
        // Use DrawComboBoxDropButton for the entire button
        // (also render extra "hot" button state).

        renderer.DrawComboBoxDropButton(this, dc, r);

        dc.SelectObject(bmpSel);

        renderer.DrawComboBoxDropButton(this, dc, r, wxCONTROL_PRESSED);

        wxBitmap bmpHot(bw,bh);
        dc.SelectObject(bmpHot);
        renderer.DrawComboBoxDropButton(this, dc, r, wxCONTROL_CURRENT);

        m_bmpNormal = bmp;
        m_bmpHot = bmpHot;
    }

    SetBitmapLabel(bmp);
    SetBitmapSelected(bmpSel);
}


void wxDropdownButton::DoMoveWindow(int x, int y, int w, int h)
{
    if (w < 0)
        w = DROPBUT_DEFAULT_WIDTH;

    wxBitmapButton::DoMoveWindow(x, y, w, h);
}


void wxDropdownButton::OnSize(wxSizeEvent& event)
{
    if ( m_borderX >= 0 && m_borderY >= 0 )
    {
        int w, h;
        GetClientSize(&w,&h);

        if ( w > 1 && h > 1 )
            RecreateBitmaps(w,h);
    }
    event.Skip();
}


void wxDropdownButton::OnMouseEnter(wxMouseEvent& event)
{
    if ( !m_useDropArrow )
        SetBitmapLabel(m_bmpHot);

    event.Skip();
}


void wxDropdownButton::OnMouseLeave(wxMouseEvent& event)
{
    if ( !m_useDropArrow )
        SetBitmapLabel(m_bmpNormal);

    event.Skip();
}


#if wxUSE_POPUPWIN

#include "wx/popupwin.h"

class wxDatePopupInternal : public wxPopupTransientWindow
{
public:
    wxDatePopupInternal(wxWindow *parent) : wxPopupTransientWindow(parent) { }

    void ShowAt(int x, int y)
    {
        Position(wxPoint(x, y), wxSize(0, 0));
        Popup();
    }

    void Hide()
    {
        Dismiss();
    }
};

#else // !wxUSE_POPUPWIN

class wxDatePopupInternal : public wxDialog
{
public:
    wxDatePopupInternal(wxWindow *parent)
        : wxDialog(parent,
                   wxID_ANY,
                   wxEmptyString,
                   wxDefaultPosition,
                   wxDefaultSize,
                   wxSIMPLE_BORDER)
    {
    }

    void ShowAt(int x, int y)
    {
        Show();
        Move(x, y);
    }

    void Hide()
    {
        wxDialog::Hide();
    }
};

#endif // wxUSE_POPUPWIN/!wxUSE_POPUPWIN

// ============================================================================
// wxDatePickerCtrlGeneric implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxDatePickerCtrlGeneric, wxDatePickerCtrlBase)
    EVT_BUTTON(CTRLID_BTN, wxDatePickerCtrlGeneric::OnClick)
    EVT_TEXT(CTRLID_TXT, wxDatePickerCtrlGeneric::OnText)
    EVT_CHILD_FOCUS(wxDatePickerCtrlGeneric::OnChildSetFocus)
    EVT_SIZE(wxDatePickerCtrlGeneric::OnSize)
END_EVENT_TABLE()

#ifndef wxHAS_NATIVE_DATEPICKCTRL
    IMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxControl)
#endif

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

bool wxDatePickerCtrlGeneric::Create(wxWindow *parent,
                                     wxWindowID id,
                                     const wxDateTime& date,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxValidator& validator,
                                     const wxString& name)
{
    wxASSERT_MSG( !(style & wxDP_SPIN),
                  _T("wxDP_SPIN style not supported, use wxDP_DEFAULT") );

    if ( !wxControl::Create(parent, id, pos, size,
                            style | wxCLIP_CHILDREN | wxWANTS_CHARS,
                            validator, name) )

    {
        return false;
    }

    InheritAttributes();

    m_txt = new wxTextCtrl(this, CTRLID_TXT, wxEmptyString, wxDefaultPosition, wxDefaultSize, TXTCTRL_FLAGS);

    m_txt->Connect(wxEVT_KEY_DOWN,
                   wxKeyEventHandler(wxDatePickerCtrlGeneric::OnEditKey),
                   NULL, this);
    m_txt->Connect(wxEVT_KILL_FOCUS,
                   wxFocusEventHandler(wxDatePickerCtrlGeneric::OnKillFocus),
                   NULL, this);

    m_btn = new wxDropdownButton(this, CTRLID_BTN, wxDefaultPosition, wxDefaultSize, wxBU_COMBO);

    m_popup = new wxDatePopupInternal(this);
    m_popup->SetFont(GetFont());

    wxPanel *panel=new wxPanel(m_popup, CTRLID_PAN,
                               wxPoint(0, 0), wxDefaultSize,
                               wxSUNKEN_BORDER);
    m_cal = new wxCalendarCtrl(panel, CTRLID_CAL, wxDefaultDateTime,
                               wxPoint(0, 0), wxDefaultSize,
                               wxCAL_SHOW_HOLIDAYS | wxSUNKEN_BORDER);
    m_cal->Connect(wxEVT_CALENDAR_SEL_CHANGED,
                   wxCalendarEventHandler(wxDatePickerCtrlGeneric::OnSelChange),
                   NULL, this);
    m_cal->Connect(wxEVT_KEY_DOWN,
                   wxKeyEventHandler(wxDatePickerCtrlGeneric::OnCalKey),
                   NULL, this);
    m_cal->Connect(wxEVT_CALENDAR_DOUBLECLICKED,
                   wxCalendarEventHandler(wxDatePickerCtrlGeneric::OnSelChange),
                   NULL, this);
    m_cal->Connect(wxEVT_CALENDAR_DAY_CHANGED,
                   wxCalendarEventHandler(wxDatePickerCtrlGeneric::OnSelChange),
                   NULL, this);
    m_cal->Connect(wxEVT_CALENDAR_MONTH_CHANGED,
                   wxCalendarEventHandler(wxDatePickerCtrlGeneric::OnSelChange),
                   NULL, this);
    m_cal->Connect(wxEVT_CALENDAR_YEAR_CHANGED,
                   wxCalendarEventHandler(wxDatePickerCtrlGeneric::OnSelChange),
                   NULL, this);

    wxWindow *yearControl = m_cal->GetYearControl();

    Connect(wxEVT_SET_FOCUS,
            wxFocusEventHandler(wxDatePickerCtrlGeneric::OnSetFocus));

    wxClientDC dc(yearControl);
    dc.SetFont(yearControl->GetFont());
    wxCoord width, dummy;
    dc.GetTextExtent(wxT("2000"), &width, &dummy);
    width += ConvertDialogToPixels(wxSize(20, 0)).x;

    wxSize calSize = m_cal->GetBestSize();
    wxSize yearSize = yearControl->GetSize();
    yearSize.x = width;

    wxPoint yearPosition = yearControl->GetPosition();

    SetFormat(wxT("%x"));

    width = yearPosition.x + yearSize.x+2+CALBORDER/2;
    if (width < calSize.x-4)
        width = calSize.x-4;

    int calPos = (width-calSize.x)/2;
    if (calPos == -1)
    {
        calPos = 0;
        width += 2;
    }
    m_cal->SetSize(calPos, 0, calSize.x, calSize.y);
    yearControl->SetSize(width-yearSize.x-CALBORDER/2, yearPosition.y,
                         yearSize.x, yearSize.y);
    m_cal->GetMonthControl()->Move(0, 0);



    panel->SetClientSize(width+CALBORDER/2, calSize.y-2+CALBORDER);
    m_popup->SetClientSize(panel->GetSize());
    m_popup->Hide();

    SetValue(date.IsValid() ? date : wxDateTime::Today());

    SetBestFittingSize(size);

    SetBackgroundColour(m_txt->GetBackgroundColour());

    return true;
}


void wxDatePickerCtrlGeneric::Init()
{
    m_popup = NULL;
    m_txt = NULL;
    m_cal = NULL;
    m_btn = NULL;

    m_dropped = false;
    m_ignoreDrop = false;
}

wxDatePickerCtrlGeneric::~wxDatePickerCtrlGeneric()
{
    m_popup = NULL;
    m_txt = NULL;
    m_cal = NULL;
    m_btn = NULL;
}

bool wxDatePickerCtrlGeneric::Destroy()
{
    if (m_cal)
        m_cal->Destroy();
    if (m_popup)
        m_popup->Destroy();
    if (m_txt)
        m_txt->Destroy();
    if (m_btn)
        m_btn->Destroy();

    m_popup = NULL;
    m_txt = NULL;
    m_cal = NULL;
    m_btn = NULL;

    return wxControl::Destroy();
}

// ----------------------------------------------------------------------------
// overridden base class methods
// ----------------------------------------------------------------------------

void wxDatePickerCtrlGeneric::DoMoveWindow(int x, int y, int w, int h)
{
    wxControl::DoMoveWindow(x, y, w, h);

    if (m_dropped)
        DropDown(false);
}

wxSize wxDatePickerCtrlGeneric::DoGetBestSize() const
{
    if (m_btn && m_txt)
    {
        int bh=m_btn->GetBestSize().y;
        int eh=m_txt->GetBestSize().y;
        return wxSize(DEFAULT_ITEM_WIDTH, bh > eh ? bh : eh);
    }
    return wxControl::DoGetBestSize();
}


bool wxDatePickerCtrlGeneric::Show(bool show)
{
    if ( !wxControl::Show(show) )
    {
        return false;
    }

    if ( !show )
    {
        if ( m_popup )
        {
            m_popup->Hide();
            m_dropped = false;
        }
    }

    return true;
}


bool wxDatePickerCtrlGeneric::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
    {
        return false;
    }

    if ( !enable )
    {
        if ( m_popup )
            m_popup->Hide();
    }

    if ( m_btn )
        m_btn->Enable(enable);

    return true;
}

// ----------------------------------------------------------------------------
// wxDatePickerCtrlGeneric API
// ----------------------------------------------------------------------------

bool
wxDatePickerCtrlGeneric::SetDateRange(const wxDateTime& lowerdate,
                                      const wxDateTime& upperdate)
{
    return m_cal->SetDateRange(lowerdate, upperdate);
}

bool wxDatePickerCtrlGeneric::SetFormat(const wxChar *fmt)
{
    m_format.clear();

    wxDateTime dt;
    dt.ParseFormat(wxT("2003-10-13"), wxT("%Y-%m-%d"));
    wxString str(dt.Format(fmt));

    const wxChar *p = str.c_str();
    while ( *p )
    {
        int n=wxAtoi(p);
        if (n == dt.GetDay())
        {
            m_format.Append(wxT("%d"));
            p += 2;
        }
        else if (n == (int)dt.GetMonth()+1)
        {
            m_format.Append(wxT("%m"));
            p += 2;
        }
        else if (n == dt.GetYear())
        {
            m_format.Append(wxT("%Y"));
            p += 4;
        }
        else if (n == (dt.GetYear() % 100))
        {
            if (GetWindowStyle() & wxDP_SHOWCENTURY)
                m_format.Append(wxT("%Y"));
            else
                m_format.Append(wxT("%y"));
            p += 2;
        }
        else
            m_format.Append(*p++);
    }

    if ( m_txt )
    {
        wxArrayString allowedChars;
        for ( wxChar c = _T('0'); c <= _T('9'); c++ )
            allowedChars.Add(wxString(c, 1));

        const wxChar *p2 = m_format.c_str();
        while ( *p2 )
        {
            if ( *p2 == '%')
                p2 += 2;
            else
                allowedChars.Add(wxString(*p2++, 1));
        }

#if wxUSE_VALIDATORS
        wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
        tv.SetIncludes(allowedChars);
        m_txt->SetValidator(tv);
#endif

        if (m_currentDate.IsValid())
            m_txt->SetValue(m_currentDate.Format(m_format));
    }

    return true;
}


wxDateTime wxDatePickerCtrlGeneric::GetValue() const
{
    return m_currentDate;
}


void wxDatePickerCtrlGeneric::SetValue(const wxDateTime& date)
{
    if ( !m_cal ) 
        return;

    // we need to suppress the event sent from wxTextCtrl as calling our
    // SetValue() should not result in an event being sent (wxTextCtrl is
    // an exception to this rule)
    gs_inSetValue = this;

    if ( date.IsValid() )
    {
        m_txt->SetValue(date.Format(m_format));
    }
    else // invalid date
    {
        wxASSERT_MSG( HasFlag(wxDP_ALLOWNONE),
                        _T("this control must have a valid date") );

        m_txt->SetValue(wxEmptyString);
    }

    gs_inSetValue = NULL;

    m_currentDate = date;
}


bool wxDatePickerCtrlGeneric::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    if (dt1)
        *dt1 = m_cal->GetLowerDateLimit();
    if (dt2)
        *dt2 = m_cal->GetUpperDateLimit();
    return true;
}


void
wxDatePickerCtrlGeneric::SetRange(const wxDateTime &dt1, const wxDateTime &dt2)
{
    m_cal->SetDateRange(dt1, dt2);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxDatePickerCtrlGeneric::DropDown(bool down)
{
    if (m_popup)
    {
        if (down)
        {
            wxDateTime dt;
            if (!m_txt->GetValue().empty())
                dt.ParseFormat(m_txt->GetValue(), m_format);

            if (dt.IsValid())
                m_cal->SetDate(dt);
            else
                m_cal->SetDate(wxDateTime::Today());

            wxPoint pos=GetParent()->ClientToScreen(GetPosition());
            m_popup->ShowAt(pos.x, pos.y + GetSize().y);
            m_dropped = true;
            m_cal->SetFocus();
        }
        else
        {
            if (m_dropped)
                m_popup->Hide();
            m_dropped = false;
        }
    }
}


void wxDatePickerCtrlGeneric::OnSize(wxSizeEvent& event)
{
    if ( m_btn )
    {
        wxSize sz = GetClientSize();

        wxSize bs=m_btn->GetSize();
        int eh=m_txt->GetBestSize().y;

        m_txt->SetSize(0, TXTPOSY, sz.x-bs.x, sz.y > eh ? eh-TXTPOSY : sz.y-TXTPOSY);
        m_btn->SetSize(sz.x - bs.x, 0, bs.x, sz.y);
    }

    event.Skip();
}


void wxDatePickerCtrlGeneric::OnChildSetFocus(wxChildFocusEvent &ev)
{
    ev.Skip();
    m_ignoreDrop = false;

    wxWindow  *w=(wxWindow*)ev.GetEventObject();
    while (w)
    {
        if (w == m_popup)
            return;
        w = w->GetParent();
    }

    if (m_dropped)
    {
        DropDown(false);
        if (::wxFindWindowAtPoint(::wxGetMousePosition()) == m_btn)
            m_ignoreDrop = true;
    }
}


void wxDatePickerCtrlGeneric::OnClick(wxCommandEvent& WXUNUSED(event))
{
    if (m_ignoreDrop)
    {
        m_ignoreDrop = false;
        m_txt->SetFocus();
    }
    else
    {
        DropDown();
        m_cal->SetFocus();
    }
}


void wxDatePickerCtrlGeneric::OnSetFocus(wxFocusEvent& WXUNUSED(ev))
{
    if (m_txt)
    {
        m_txt->SetFocus();
        m_txt->SetSelection(-1, -1); // select everything
    }
}


void wxDatePickerCtrlGeneric::OnKillFocus(wxFocusEvent &ev)
{
    if (!m_txt)
        return;
    
    ev.Skip();

    wxDateTime dt;
    dt.ParseFormat(m_txt->GetValue(), m_format);
    if ( !dt.IsValid() )
    {
        if ( !HasFlag(wxDP_ALLOWNONE) )
            dt = m_currentDate;
    }

    if(dt.IsValid())
        m_txt->SetValue(dt.Format(m_format));
    else
        m_txt->SetValue(wxEmptyString);

    // notify that we had to change the date after validation
    if ( (dt.IsValid() && (!m_currentDate.IsValid() || m_currentDate != dt)) ||
            (!dt.IsValid() && m_currentDate.IsValid()) )
    {
        m_currentDate = dt;
        wxDateEvent event(this, dt, wxEVT_DATE_CHANGED);
        GetEventHandler()->ProcessEvent(event);
    }
}


void wxDatePickerCtrlGeneric::OnSelChange(wxCalendarEvent &ev)
{
    if (m_cal)
    {
        m_currentDate = m_cal->GetDate();
        m_txt->SetValue(m_currentDate.Format(m_format));
        if (ev.GetEventType() == wxEVT_CALENDAR_DOUBLECLICKED)
        {
            DropDown(false);
            m_txt->SetFocus();
        }
    }
    ev.SetEventObject(this);
    ev.SetId(GetId());
    GetParent()->ProcessEvent(ev);

    wxDateEvent dev(this, ev.GetDate(), wxEVT_DATE_CHANGED);
    GetParent()->ProcessEvent(dev);
}


void wxDatePickerCtrlGeneric::OnText(wxCommandEvent &ev)
{
    if ( gs_inSetValue )
    {
        // artificial event resulting from our own SetValue() call, ignore it
        return;
    }

    ev.SetEventObject(this);
    ev.SetId(GetId());
    GetParent()->ProcessEvent(ev);

    // We'll create an additional event if the date is valid.
    // If the date isn't valid, the user's probably in the middle of typing
    wxString txt = m_txt->GetValue();
    wxDateTime dt;
    if (!txt.empty())
    {
        dt.ParseFormat(txt, m_format);
        if (!dt.IsValid())
            return;
    }

    wxCalendarEvent cev(m_cal, wxEVT_CALENDAR_SEL_CHANGED);
    cev.SetEventObject(this);
    cev.SetId(GetId());
    cev.SetDate(dt);

    GetParent()->ProcessEvent(cev);

    wxDateEvent dev(this, dt, wxEVT_DATE_CHANGED);
    GetParent()->ProcessEvent(dev);
}


void wxDatePickerCtrlGeneric::OnEditKey(wxKeyEvent & ev)
{
    if (ev.GetKeyCode() == WXK_DOWN && !ev.HasModifiers())
        DropDown(true);
    else
        ev.Skip();
}


void wxDatePickerCtrlGeneric::OnCalKey(wxKeyEvent & ev)
{
    if (ev.GetKeyCode() == WXK_ESCAPE && !ev.HasModifiers())
        DropDown(false);
    else
        ev.Skip();
}

#endif // wxUSE_DATEPICKCTRL_GENERIC

#endif // wxUSE_DATEPICKCTRL
