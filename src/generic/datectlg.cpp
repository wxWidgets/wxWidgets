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

#if defined(__WXMSW__) && !defined(__WXUNIVERSAL__)
    #define HAS_NATIVE_VERSION
#endif

// use this version if we're explicitly requested to do it or if it's the only
// one we have
#if wxUSE_DATEPICKCTRL_GENERIC || !defined(HAS_NATIVE_VERSION)

#ifndef WX_PRECOMP
    #include "wx/bmpbuttn.h"
    #include "wx/dialog.h"
    #include "wx/dcmemory.h"
    #include "wx/panel.h"
    #include "wx/textctrl.h"
    #include "wx/valtext.h"
#endif

// otherwise it's defined in the native version implementation
#ifndef HAS_NATIVE_VERSION
    #define _WX_DEFINE_DATE_EVENTS_
#endif

#include "wx/dateevt.h"

#include "wx/datectrl.h"
#include "wx/calctrl.h"
#include "wx/popupwin.h"

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

// ============================================================================
// wxDatePickerCtrlGeneric implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxDatePickerCtrlGeneric, wxDatePickerCtrlBase)
    EVT_BUTTON(CTRLID_BTN, wxDatePickerCtrlGeneric::OnClick)
    EVT_TEXT(CTRLID_TXT, wxDatePickerCtrlGeneric::OnText)
    EVT_CHILD_FOCUS(wxDatePickerCtrlGeneric::OnChildSetFocus)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxDatePickerCtrlBase)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

bool wxDatePickerCtrlGeneric::Create(wxWindow *parent,
                                     wxWindowID id,
                                     const wxDateTime& date,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style,
                                     const wxString& name)
{
    wxASSERT_MSG( !(style & wxDP_SPIN),
                  _T("wxDP_SPIN style not supported, use wxDP_DEFAULT") );

    if ( !wxControl::Create(parent, id, pos, size,
                            style | wxCLIP_CHILDREN | wxWANTS_CHARS,
                            wxDefaultValidator, name) )

    {
        return false;
    }

    SetWindowStyle(style | wxWANTS_CHARS);
    InheritAttributes();

    wxBitmap bmp(8, 4);
    {
        wxMemoryDC dc;

        dc.SelectObject(bmp);
        dc.SetBrush(wxBrush(GetBackgroundColour()));
        dc.SetPen(wxPen(GetBackgroundColour()));
        dc.DrawRectangle(0,0, 8,4);

        dc.SetBrush(wxBrush(GetForegroundColour()));
        dc.SetPen(wxPen(GetForegroundColour()));
        wxPoint pt[3] = { wxPoint(0,0), wxPoint(6,0), wxPoint(3,3) };
        dc.DrawPolygon(3, pt);
        dc.SelectObject(wxNullBitmap);
    }

    m_txt=new wxTextCtrl(this, CTRLID_TXT);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnEditKey,
                   0, this);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnKillFocus,
                   0, this);

    m_btn = new wxBitmapButton(this, CTRLID_BTN, bmp);

    m_popup = new wxPopupWindow(this);
    m_popup->SetFont(GetFont());

    wxPanel *panel=new wxPanel(m_popup, CTRLID_PAN,
                               wxPoint(0, 0), wxDefaultSize,
                               wxSUNKEN_BORDER);
    m_cal = new wxCalendarCtrl(panel, CTRLID_CAL, wxDefaultDateTime,
                               wxPoint(0,0), wxDefaultSize,
                               wxCAL_SHOW_HOLIDAYS | wxSUNKEN_BORDER);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_SEL_CHANGED,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnSelChange,
                   0, this);
    m_cal->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnCalKey,
                   0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DOUBLECLICKED,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnSelChange,
                   0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DAY_CHANGED,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnSelChange,
                   0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_MONTH_CHANGED,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnSelChange,
                   0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_YEAR_CHANGED,
                   (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnSelChange,
                   0, this);

    wxWindow *yearControl = m_cal->GetYearControl();

    Connect(wxID_ANY, wxID_ANY, wxEVT_SET_FOCUS,
            (wxObjectEventFunction)&wxDatePickerCtrlGeneric::OnSetFocus);

    wxClientDC dc(yearControl);
    dc.SetFont(m_font);
    wxCoord width, dummy;
    dc.GetTextExtent(wxT("2000"), &width, &dummy);
    width += ConvertDialogToPixels(wxSize(20,0)).x;

    wxSize calSize = m_cal->GetBestSize();
    wxSize yearSize = yearControl->GetSize();
    yearSize.x = width;

    wxPoint yearPosition = yearControl->GetPosition();

    SetFormat(wxT("%x"));

    if (date.IsValid())
        m_txt->SetValue(date.Format(m_format));


#ifdef __WXMSW__
#define CALBORDER   0
#else
#define CALBORDER   4
#endif

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

    return TRUE;
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

    m_txt->SetSize(0, 0, w-bs.x-1, h > eh ? eh : h);
    m_btn->SetSize(w - bs.x, 0, bs.x, h > bs.y ? bs.y : h);

    if (m_dropped)
        DropDown();
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
        return FALSE;
    }

    if (!show)
    {
        if (m_popup)
        {
            m_popup->Hide();
            m_dropped = false;
        }
    }

    return TRUE;
}


bool wxDatePickerCtrlGeneric::Enable(bool enable)
{
    if ( !wxControl::Enable(enable) )
    {
        return FALSE;
    }

    if (!enable)
    {
        if (m_cal)
            m_cal->Hide();
    }
    if (m_btn)
        m_btn->Enable(enable);
    return TRUE;
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
    wxString currentText;
    wxDateTime currentDate;
    if (m_txt)
    {
        currentText = m_txt->GetValue();
        if (!currentText.IsEmpty())
            currentDate.ParseFormat(currentText, m_format);
    }
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
            m_format.Append(wxT("%y"));
            p += 2;
        }
        else
            m_format.Append(*p++);
    }

    if (m_txt)
    {
        wxStringList valList;
        wxChar c;
        for (c='0'; c <= '9'; c++)
            valList.Add(wxString(c, 1));
        wxChar *p=(wxChar*)m_format.c_str();
        while (*p)
        {
            if (*p == '%')
                p += 2;
            else
                valList.Add(wxString(*p++, 1));
        }
        wxTextValidator tv(wxFILTER_INCLUDE_CHAR_LIST);
        tv.SetIncludeList(valList);

        m_txt->SetValidator(tv);

        if (!currentText.IsEmpty())
            m_txt->SetValue(currentDate.Format(m_format));
    }
    return true;
}


wxDateTime wxDatePickerCtrlGeneric::GetValue() const
{
    wxDateTime dt;
    wxString txt=m_txt->GetValue();

    if (!txt.IsEmpty())
        dt.ParseFormat(txt, m_format);

    return dt;
}


void wxDatePickerCtrlGeneric::SetValue(const wxDateTime& date)
{
    if (m_cal)
    {
        if (date.IsValid())
            m_txt->SetValue(date.Format(m_format));
        else
            m_txt->SetValue(wxEmptyString);
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
            if (!m_txt->GetValue().IsEmpty())
                dt.ParseFormat(m_txt->GetValue(), m_format);

            if (dt.IsValid())
                m_cal->SetDate(dt);
            else
                m_cal->SetDate(wxDateTime::Today());

            wxPoint pos=GetParent()->ClientToScreen(GetPosition());
            m_popup->Move(pos.x, pos.y + GetSize().y);
            m_popup->Show();
            m_dropped = true;
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


void wxDatePickerCtrlGeneric::OnClick(wxCommandEvent& event)
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


void wxDatePickerCtrlGeneric::OnSetFocus(wxFocusEvent &ev)
{
    if (m_txt)
    {
        m_txt->SetFocus();
        m_txt->SetSelection(0, 100);
    }
}


void wxDatePickerCtrlGeneric::OnKillFocus(wxFocusEvent &ev)
{
    ev.Skip();

    wxDateTime dt;
    dt.ParseFormat(m_txt->GetValue(), m_format);
    if (!dt.IsValid())
        m_txt->SetValue(wxEmptyString);
    else
        m_txt->SetValue(dt.Format(m_format));
}


void wxDatePickerCtrlGeneric::OnSelChange(wxCalendarEvent &ev)
{
    if (m_cal)
    {
        m_txt->SetValue(m_cal->GetDate().Format(m_format));
        if (ev.GetEventType() == wxEVT_CALENDAR_DOUBLECLICKED)
        {
            DropDown(false);
            m_txt->SetFocus();
        }
    }
    ev.SetEventObject(this);
    ev.SetId(GetId());
    GetParent()->ProcessEvent(ev);
}


void wxDatePickerCtrlGeneric::OnText(wxCommandEvent &ev)
{
    ev.SetEventObject(this);
    ev.SetId(GetId());
    GetParent()->ProcessEvent(ev);

    // We'll create an additional event if the date is valid.
    // If the date isn't valid, the user's probable in the middle of typing
    wxString txt=m_txt->GetValue();
    wxDateTime dt;
    if (!txt.IsEmpty())
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
}


void wxDatePickerCtrlGeneric::OnEditKey(wxKeyEvent & ev)
{
    if (ev.GetKeyCode() == WXK_DOWN && !ev.HasModifiers())
        DropDown();
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

