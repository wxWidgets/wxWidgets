/////////////////////////////////////////////////////////////////////////////
// Name:        generic/datectlg.cpp
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
#if wxUSE_DATEPICKCTRL_GENERIC || !defined(wxHAS_NATIVE_DATEPICKCTRL)

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

#ifdef __WXMSW__
    #undef wxUSE_POPUPWIN
    #define wxUSE_POPUPWIN    0  // Popup not working
    #define TXTCTRL_FLAGS     wxNO_BORDER
    #define BTN_FLAGS         wxNO_BORDER
    #define CALBORDER         0
    #define RIGHTBUTTONBORDER 3
    #define TOPBUTTONBORDER   0
    #define BUTTONBORDER      3
    #define TXTPOSY           1
#else
    #define TXTCTRL_FLAGS     0
    #define BTN_FLAGS         wxBU_AUTODRAW
    #define CALBORDER         4
    #define RIGHTBUTTONBORDER 0
    #define TOPBUTTONBORDER   0
    #define BUTTONBORDER      0
    #define TXTPOSY           0
#endif


// ----------------------------------------------------------------------------
// local classes
// ----------------------------------------------------------------------------


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

    void Init()
    {
        m_borderX = -1;
        m_borderY = -1;
    }
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator);

    void DoMoveWindow(int x, int y, int w, int h);

protected:
    int m_borderX, m_borderY;
};


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
                              long WXUNUSED(style),
                              const wxValidator& validator)
{
    m_marginX = 0;
    m_marginY = 0;

    wxBitmap chkBmp(15,15);  // arbitrary
    if ( !wxBitmapButton::Create(parent, id, chkBmp,
                                 pos, wxDefaultSize, BTN_FLAGS, validator) )
        return false;

#if (BTNFLAGS & wxBU_AUTODRAW ) == 0
    m_windowStyle |= wxBU_AUTODRAW;
#endif

    const wxSize sz = GetSize();
    int w = chkBmp.GetWidth(),
        h = chkBmp.GetHeight();
    m_borderX = sz.x - m_marginX - w;
    m_borderY = sz.y - m_marginY - h;

    w = size.x > 0 ? size.x : sz.x;
    h = size.y > 0 ? size.y : sz.y;

    DoMoveWindow(pos.x, pos.y, w, h);

    return true;
}


void wxDropdownButton::DoMoveWindow(int x, int y, int w, int h)
{
    if (m_borderX >= 0 && m_borderY >= 0 && (w >= 0 || h >= 0))
    {
        wxMemoryDC dc;
        if (w < 0)
              w = GetSize().x;
#ifdef __WXGTK__
        else
            w = m_marginX + m_borderX + 15; // GTK magic size
#endif
        if (h < 0)
            h = GetSize().y;

        int bw = w - m_marginX - m_borderX;
        int bh = h - m_marginY - m_borderY;
        if (bh < 11) bh=11;
        if (bw < 9)  bw=9;

        wxBitmap bmp(bw, bh);
        dc.SelectObject(bmp);

        wxRendererNative::Get().DrawComboBoxDropButton(this, dc, wxRect(0,0,bw, bh));
        SetBitmapLabel(bmp);
    }

    wxBitmapButton::DoMoveWindow(x, y, w, h);
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

    const int height = m_txt->GetBestSize().y - BUTTONBORDER;

    m_btn = new wxDropdownButton(this, CTRLID_BTN, wxDefaultPosition, wxSize(height, height));

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
    dc.SetFont(m_font);
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
    wxSize bs=m_btn->GetBestSize();
    int eh=m_txt->GetBestSize().y;

    m_txt->SetSize(0, TXTPOSY, w-bs.x-RIGHTBUTTONBORDER, h > eh ? eh-TXTPOSY : h-TXTPOSY);
    m_btn->SetSize(w - bs.x-RIGHTBUTTONBORDER, TOPBUTTONBORDER, bs.x, h > bs.y ? bs.y : h);

    if (m_dropped)
        DropDown(true);
}

wxSize wxDatePickerCtrlGeneric::DoGetBestSize() const
{
    int bh=m_btn->GetBestSize().y;
    int eh=m_txt->GetBestSize().y;
    return wxSize(DEFAULT_ITEM_WIDTH, bh > eh ? bh : eh);
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
        if ( m_cal )
            m_cal->Hide();
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
    wxDateTime dt;
    dt.ParseFormat(wxT("2003-10-13"), wxT("%Y-%m-%d"));
    wxString str=dt.Format(fmt);
    wxChar *p=(wxChar*)str.c_str();

    m_format=wxEmptyString;

    while (*p)
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

    if (m_txt)
    {
        wxArrayString allowedChars;
        for ( wxChar c = _T('0'); c <= _T('9'); c++ )
            allowedChars.Add(wxString(c, 1));

        const wxChar *p = m_format.c_str();
        while (*p)
        {
            if (*p == '%')
                p += 2;
            else
                allowedChars.Add(wxString(*p++, 1));
        }

        wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
        tv.SetIncludes(allowedChars);

        m_txt->SetValidator(tv);

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
    if (m_cal)
    {
        if (date.IsValid())
            m_txt->SetValue(date.Format(m_format));
        else
        {
            wxASSERT_MSG( HasFlag(wxDP_ALLOWNONE),
                            _T("this control must have a valid date") );

            m_txt->SetValue(wxEmptyString);
        }

        m_currentDate = date;
    }
}


bool wxDatePickerCtrlGeneric::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    if (dt1)
        *dt1 = m_cal->GetLowerDateLimit();
    if (dt1)
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
        if (ev.GetEventObject() == m_btn)
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
    if ( (dt.IsValid() && m_currentDate != dt) ||
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

