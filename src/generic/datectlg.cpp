/////////////////////////////////////////////////////////////////////////////
// Name:        generic/datectrl.cpp
// Purpose:     generic wxDatePickerCtrl implementation
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

#include "wx/datectrl.h"

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

enum
{
    CTRLID_TXT = 101,
    CTRLID_CAL
    CTRLID_BTN
    CTRLID_PAN
};

#ifndef DEFAULT_ITEM_WIDTH
    #define DEFAULT_ITEM_WIDTH 100
#endif

// ============================================================================
// wxDatePickerCtrl implementation
// ============================================================================

BEGIN_EVENT_TABLE(wxDatePickerCtrl, wxDatePickerCtrlBase)
    EVT_BUTTON(CTRLID_BTN, wxDatePickerCtrl::OnClick)
    EVT_TEXT(CTRLID_TXT, wxDatePickerCtrl::OnText)
    EVT_CHILD_FOCUS(wxDatePickerCtrl::OnChildSetFocus)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxDatePickerCtrl, wxDatePickerCtrlBase)

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

wxDatePickerCtrl::wxDatePickerCtrl(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    Init();
    Create(parent, id, date, pos, size, style, name);
}


bool wxDatePickerCtrl::Create(wxWindow *parent,
                            wxWindowID id,
                            const wxDateTime& date,
                            const wxPoint& pos,
                            const wxSize& size,
                            long style,
                            const wxString& name)
{
    wxString txt;
    if (date.IsValid())
        txt = date.FormatDate();

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

    m_txt=new wxTextCtrl(this, CTRLID_TXT, txt);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, (wxObjectEventFunction)&wxDatePickerCtrl::OnEditKey, 0, this);
    m_txt->Connect(wxID_ANY, wxID_ANY, wxEVT_KILL_FOCUS, (wxObjectEventFunction)&wxDatePickerCtrl::OnKillFocus, 0, this);
    SetFormat(wxT("%x"));

    m_btn = new wxBitmapButton(this, CTRLID_BTN, bmp);

    m_dlg = new wxDialog(this, CTRLID_CAL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER);
    m_dlg->SetFont(GetFont());

    wxPanel *panel=new wxPanel(m_dlg, CTRLID_PAN, wxPoint(0, 0), wxDefaultSize, wxSUNKEN_BORDER|wxCLIP_CHILDREN);
    m_cal = new wxCalendarCtrl(panel, CTRLID_CAL, wxDefaultDateTime, wxPoint(0,0), wxDefaultSize, wxSUNKEN_BORDER);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_SEL_CHANGED, (wxObjectEventFunction)&wxDatePickerCtrl::OnSelChange, 0, this);
    m_cal->Connect(wxID_ANY, wxID_ANY, wxEVT_KEY_DOWN, (wxObjectEventFunction)&wxDatePickerCtrl::OnCalKey, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DOUBLECLICKED, (wxObjectEventFunction)&wxDatePickerCtrl::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_DAY_CHANGED, (wxObjectEventFunction)&wxDatePickerCtrl::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_MONTH_CHANGED, (wxObjectEventFunction)&wxDatePickerCtrl::OnSelChange, 0, this);
    m_cal->Connect(CTRLID_CAL, CTRLID_CAL, wxEVT_CALENDAR_YEAR_CHANGED, (wxObjectEventFunction)&wxDatePickerCtrl::OnSelChange, 0, this);

    wxWindow *yearControl = m_cal->GetYearControl();

    Connect(wxID_ANY, wxID_ANY, wxEVT_SET_FOCUS, (wxObjectEventFunction)&wxDatePickerCtrl::OnSetFocus);

    wxClientDC dc(yearControl);
    dc.SetFont(m_font);
    wxCoord width, dummy;
    dc.GetTextExtent(wxT("2000"), &width, &dummy);
    width += ConvertDialogToPixels(wxSize(20,0)).x;

    wxSize calSize = m_cal->GetBestSize();
    wxSize yearSize = yearControl->GetSize();
    yearSize.x = width;

    wxPoint yearPosition = yearControl->GetPosition();

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
    yearControl->SetSize(width-yearSize.x-CALBORDER/2, yearPosition.y, yearSize.x, yearSize.y);
    m_cal->GetMonthControl()->Move(0, 0);



    panel->SetClientSize(width+CALBORDER/2, calSize.y-2+CALBORDER);
    m_dlg->SetClientSize(panel->GetSize());

    return TRUE;
}


void wxDatePickerCtrl::Init()
{
    m_dlg = NULL;
    m_txt = NULL;
    m_cal = NULL;
    m_btn = NULL;

    m_dropped = false;
    m_ignoreDrop = false;
}


bool wxDatePickerCtrl::Destroy()
{
    if (m_cal)
        m_cal->Destroy();
    if (m_dlg)
        m_dlg->Destroy();
    if (m_txt)
        m_txt->Destroy();
    if (m_btn)
        m_btn->Destroy();

    m_dlg = NULL;
    m_txt = NULL;
    m_cal = NULL;
    m_btn = NULL;

    return wxControl::Destroy();
}

// ----------------------------------------------------------------------------
// overridden base class methods
// ----------------------------------------------------------------------------

void wxDatePickerCtrl::DoMoveWindow(int x, int y, int w, int h)
{
    wxControl::DoMoveWindow(x, y, w, h);
    wxSize bs=m_btn->GetBestSize();
    int eh=m_txt->GetBestSize().y;

    m_txt->SetSize(0, 0, w-bs.x-1, h > eh ? eh : h);
    m_btn->SetSize(w - bs.x, 0, bs.x, h > bs.y ? bs.y : h);

    if (m_dropped)
        DropDown();
}

wxSize wxDatePickerCtrl::DoGetBestSize() const
{
    int bh=m_btn->GetBestSize().y;
    int eh=m_txt->GetBestSize().y;
    return wxSize(DEFAULT_ITEM_WIDTH, bh > eh ? bh : eh);
}


bool wxDatePickerCtrl::Show(bool show)
{
    if ( !wxControl::Show(show) )
    {
        return FALSE;
    }

    if (!show)
    {
        if (m_dlg)
        {
            m_dlg->Hide();
            m_dropped = false;
        }
    }

    return TRUE;
}


bool wxDatePickerCtrl::Enable(bool enable)
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
// wxDatePickerCtrl API
// ----------------------------------------------------------------------------

bool wxDatePickerCtrl::SetFormat(const wxChar *fmt)
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
    }
    return true;
}


wxDateTime wxDatePickerCtrl::GetValue() const
{
    wxDateTime dt;
    wxString txt=m_txt->GetValue();

    if (!txt.IsEmpty())
        dt.ParseFormat(txt, m_format);

    return dt;
}


void wxDatePickerCtrl::SetValue(const wxDateTime& date)
{
    if (m_cal)
    {
        if (date.IsValid())
            m_txt->SetValue(date.FormatDate());
        else
            m_txt->SetValue(wxEmptyString);
    }
}


bool wxDatePickerCtrl::GetRange(wxDateTime *dt1, wxDateTime *dt2) const
{
    if (dt1)
        *dt1 = GetLowerDateLimit();
    if (dt1)
        *dt2 = GetUpperDateLimit();
    return true;
}


void wxDatePickerCtrl::SetRange(const wxDateTime &dt1, const wxDateTime &dt2)
{
    SetLowerDateLimit(dt1);
    SetUpperDateLimit(dt2);
}

// ----------------------------------------------------------------------------
// event handlers
// ----------------------------------------------------------------------------

void wxDatePickerCtrl::DropDown(bool down)
{
    if (m_dlg)
    {
        if (down)
        {
            if (m_txt->GetValue().IsEmpty())
                m_cal->SetDate(wxDateTime::Today());
            else
            {
                wxDateTime dt;
                dt.ParseFormat(m_txt->GetValue(), m_format);
                m_cal->SetDate(dt);
            }
            wxPoint pos=GetParent()->ClientToScreen(GetPosition());

            m_dlg->Move(pos.x, pos.y + GetSize().y);
            m_dlg->Show();
            m_dropped = true;
        }
        else
        {
            if (m_dropped)
                m_dlg->Hide();
            m_dropped = false;
        }
    }
}


void wxDatePickerCtrl::OnChildSetFocus(wxChildFocusEvent &ev)
{
    ev.Skip();
    m_ignoreDrop = false;

    wxWindow  *w=(wxWindow*)ev.GetEventObject();
    while (w)
    {
        if (w == m_dlg)
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


void wxDatePickerCtrl::OnClick(wxCommandEvent& event)
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


void wxDatePickerCtrl::OnSetFocus(wxFocusEvent &ev)
{
    if (m_txt)
    {
        m_txt->SetFocus();
        m_txt->SetSelection(0, 100);
    }
}


void wxDatePickerCtrl::OnKillFocus(wxFocusEvent &ev)
{
    ev.Skip();

    wxDateTime dt;
    dt.ParseFormat(m_txt->GetValue(), m_format);
    if (!dt.IsValid())
        m_txt->SetValue(wxEmptyString);
    else
        m_txt->SetValue(dt.Format(m_format));
}


void wxDatePickerCtrl::OnSelChange(wxCalendarEvent &ev)
{
    if (m_cal)
    {
        m_txt->SetValue(m_cal->GetDate().FormatDate());
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


void wxDatePickerCtrl::OnText(wxCommandEvent &ev)
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


void wxDatePickerCtrl::OnEditKey(wxKeyEvent & ev)
{
    if (ev.GetKeyCode() == WXK_DOWN && !ev.HasModifiers())
        DropDown();
    else
        ev.Skip();
}


void wxDatePickerCtrl::OnCalKey(wxKeyEvent & ev)
{
    if (ev.GetKeyCode() == WXK_ESCAPE && !ev.HasModifiers())
        DropDown(false);
    else
        ev.Skip();
}

