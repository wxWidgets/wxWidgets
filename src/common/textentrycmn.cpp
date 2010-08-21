///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/textentrycmn.cpp
// Purpose:     wxTextEntryBase implementation
// Author:      Vadim Zeitlin
// Created:     2007-09-26
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_TEXTCTRL || wxUSE_COMBOBOX

#ifndef WX_PRECOMP
    #include "wx/window.h"
    #include "wx/dataobj.h"
#endif //WX_PRECOMP

#include "wx/textentry.h"
#include "wx/clipbrd.h"

// ----------------------------------------------------------------------------
// wxTextEntryHintData
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxTextEntryHintData wxBIND_OR_CONNECT_HACK_ONLY_BASE_CLASS
{
public:
    wxTextEntryHintData(wxTextEntryBase *entry, wxWindow *win)
        : m_entry(entry),
          m_win(win)
    {
        wxBIND_OR_CONNECT_HACK(win, wxEVT_SET_FOCUS, wxFocusEventHandler,
                                wxTextEntryHintData::OnSetFocus, this);
        wxBIND_OR_CONNECT_HACK(win, wxEVT_KILL_FOCUS, wxFocusEventHandler,
                                wxTextEntryHintData::OnKillFocus, this);

        // we don't have any hint yet
        m_showsHint = false;
    }

    // default dtor is ok

    // are we showing the hint right now?
    bool ShowsHint() const { return m_showsHint; }

    void SetHintString(const wxString& hint)
    {
        m_hint = hint;

        if ( m_showsHint )
        {
            // update it immediately
            m_entry->ChangeValue(hint);
        }
        //else: the new hint will be shown later
    }

    const wxString& GetHintString() const { return m_hint; }

private:
    void OnSetFocus(wxFocusEvent& event)
    {
        // hide the hint if we were showing it
        if ( m_showsHint )
        {
            // Clear() would send an event which we don't want, so do it like
            // this
            m_entry->ChangeValue(wxString());
            m_win->SetForegroundColour(m_colFg);

            m_showsHint = false;
        }

        event.Skip();
    }

    void OnKillFocus(wxFocusEvent& event)
    {
        // restore the hint if the user didn't do anything in the control
        if ( m_entry->IsEmpty() )
        {
            m_entry->ChangeValue(m_hint);

            m_colFg = m_win->GetForegroundColour();
            m_win->SetForegroundColour(*wxLIGHT_GREY);

            m_showsHint = true;
        }

        event.Skip();
    }

    // the text control we're associated with (as its interface and its window)
    wxTextEntryBase * const m_entry;
    wxWindow * const m_win;

    // the original foreground colour of m_win before we changed it
    wxColour m_colFg;

    // the hint passed to wxTextEntry::SetHint()
    wxString m_hint;

    // true if we're currently showing it, for this we must be empty and not
    // have focus
    bool m_showsHint;

    wxDECLARE_NO_COPY_CLASS(wxTextEntryHintData);
};

// ============================================================================
// wxTextEntryBase implementation
// ============================================================================

wxTextEntryBase::~wxTextEntryBase()
{
    delete m_hintData;
}

// ----------------------------------------------------------------------------
// text accessors
// ----------------------------------------------------------------------------

wxString wxTextEntryBase::GetValue() const
{
    return m_hintData && m_hintData->ShowsHint() ? wxString() : DoGetValue();
}

wxString wxTextEntryBase::GetRange(long from, long to) const
{
    wxString sel;
    wxString value = GetValue();

    if ( from < to && (long)value.length() >= to )
    {
        sel = value.substr(from, to - from);
    }

    return sel;
}

// ----------------------------------------------------------------------------
// text operations
// ----------------------------------------------------------------------------

void wxTextEntryBase::AppendText(const wxString& text)
{
    SetInsertionPointEnd();
    WriteText(text);
}

void wxTextEntryBase::DoSetValue(const wxString& value, int flags)
{
    EventsSuppressor noeventsIf(this, !(flags & SetValue_SendEvent));

    SelectAll();
    WriteText(value);

    SetInsertionPoint(0);
}

void wxTextEntryBase::Replace(long from, long to, const wxString& value)
{
    {
        EventsSuppressor noevents(this);
        Remove(from, to);
    }

    SetInsertionPoint(from);
    WriteText(value);
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

bool wxTextEntryBase::HasSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    return from < to;
}

void wxTextEntryBase::RemoveSelection()
{
    long from, to;
    GetSelection(& from, & to);
    if (from != -1 && to != -1)
        Remove(from, to);
}

wxString wxTextEntryBase::GetStringSelection() const
{
    long from, to;
    GetSelection(&from, &to);

    return GetRange(from, to);
}

// ----------------------------------------------------------------------------
// clipboard
// ----------------------------------------------------------------------------

bool wxTextEntryBase::CanCopy() const
{
    return HasSelection();
}

bool wxTextEntryBase::CanCut() const
{
    return CanCopy() && IsEditable();
}

bool wxTextEntryBase::CanPaste() const
{
    if ( IsEditable() )
    {
#if wxUSE_CLIPBOARD
        // check if there is any text on the clipboard
        if ( wxTheClipboard->IsSupported(wxDF_TEXT)
#if wxUSE_UNICODE
                || wxTheClipboard->IsSupported(wxDF_UNICODETEXT)
#endif // wxUSE_UNICODE
           )
        {
            return true;
        }
#endif // wxUSE_CLIPBOARD
    }

    return false;
}

// ----------------------------------------------------------------------------
// hints support
// ----------------------------------------------------------------------------

bool wxTextEntryBase::SetHint(const wxString& hint)
{
    if ( !m_hintData )
        m_hintData = new wxTextEntryHintData(this, GetEditableWindow());

    m_hintData->SetHintString(hint);

    return true;
}

wxString wxTextEntryBase::GetHint() const
{
    return m_hintData ? m_hintData->GetHintString() : wxString();
}

// ----------------------------------------------------------------------------
// margins support
// ----------------------------------------------------------------------------

bool wxTextEntryBase::DoSetMargins(const wxPoint& WXUNUSED(pt))
{
    return false;
}

wxPoint wxTextEntryBase::DoGetMargins() const
{
    return wxPoint(-1, -1);
}

// ----------------------------------------------------------------------------
// mask support
// ----------------------------------------------------------------------------

void wxTextEntryBase::SetMask(const wxMaskedEdit& mask)
{
    m_maskCtrl = wxMaskedEdit(mask);
}

void wxTextEntryBase::OnApplyMask(int keycode)
{
    wxString string = GetValue();
    wxString formatString;
    unsigned int cursor = GetInsertionPoint();
    bool invalid;
    wxString tmp;


    // if the mask isn't empty
    if(string != m_maskCtrl.GetEmptyMask())
    {
        //apply the format code
        formatString = m_maskCtrl.ApplyFormatCodes(string.Mid(0, GetInsertionPoint()));

        //test if the entered character is valid
        invalid = !m_maskCtrl.IsCharValid(keycode, GetInsertionPoint());

        if(invalid)
        {
            Replace(cursor -1, cursor, m_maskCtrl.GetFillChar());
            SetInsertionPoint ( cursor - 1);
        }
        else
        {
            //If the test is upper or lower case after Applying formats codes
            if(formatString.Cmp(string) != 0)
            {
               if(m_maskCtrl.GetFormatCode().Find('-') == wxNOT_FOUND)
               {
                    Replace(0, formatString.Len() , formatString);
               }
            }
        }
    }
}

void wxTextEntryBase::OnKeyPressedMask(wxKeyEvent& event)
{
    int keycode = event.GetKeyCode();
    const int nextKey =m_maskCtrl.GetNextChoiceKey();
    const int previousKey = m_maskCtrl.GetPreviousChoiceKey();
    unsigned int cursor = GetInsertionPoint();
    unsigned int fieldIndex = m_maskCtrl.GetFieldIndex(cursor);
    unsigned int fieldMinPos = m_maskCtrl.GetMinFieldPosition(fieldIndex);
    wxString string = GetValue();
    unsigned int index = 0;
    wxString emptyMask = m_maskCtrl.GetEmptyMask();
    wxString tmp;
    wxString choice;
    wxString mask;
    wxChar fillChar = m_maskCtrl.GetFillChar();
    wxChar ch;

    if(keycode == nextKey || keycode == previousKey)
    {
        if(cursor == string.Len())
        {
            fieldIndex = m_maskCtrl.GetNumberOfFields() - 1;
            fieldMinPos = m_maskCtrl.GetMinFieldPosition(fieldIndex);
        }
        else if(cursor == fieldMinPos && fieldIndex != 0
        && !m_maskCtrl.GetChoices(fieldIndex - 1).IsEmpty())
        {
           fieldIndex--;
           fieldMinPos = m_maskCtrl.GetMinFieldPosition(fieldIndex);
        }

        if(keycode == nextKey)
        {
            choice = m_maskCtrl.GetNextChoices(fieldIndex);
            if(choice.Cmp(wxEmptyString) != 0)
            {
                Replace(fieldMinPos,
                    fieldMinPos + choice.Len() ,
                    choice);
                SetInsertionPoint(m_maskCtrl.GetMaxFieldPosition(fieldIndex) + 1);
            }
        }
        else
        {
            choice = m_maskCtrl.GetPreviousChoices(fieldIndex);

            if(choice.Cmp(wxEmptyString) != 0)
            {
                Replace(fieldMinPos,
                    fieldMinPos + choice.Len(),
                    choice);
            }
            SetInsertionPoint(m_maskCtrl.GetMaxFieldPosition(fieldIndex) + 1);

        }

    }
    else
    {
        mask = m_maskCtrl.GetEmptyMask()[cursor];

        switch(keycode)
        {
            case(WXK_LEFT):
            case(WXK_RIGHT):
                    event.Skip();
            break;
            case(WXK_BACK):
                if(cursor > 0)
                {
                    wxString mask = m_maskCtrl.GetEmptyMask()[cursor - 1];

                    if(mask == fillChar)
                    {
                        Replace(cursor - 1, cursor , m_maskCtrl.GetFillChar());

                        cursor = GetInsertionPoint();
                        SetInsertionPoint(cursor -1);
                    }
                    else
                    {
                        SetInsertionPoint(cursor -1);
                    }

                }
            break;
            case(WXK_RETURN):
                if(m_maskCtrl.IsNumber())
                {
                    unsigned int it;
                    unsigned int space = 0;
                    wxString newString;
                    for(it = 0; it < string.Len(); it++)
                    {
                        if(string[it] == m_maskCtrl.GetDecimalPoint())
                        {
                            tmp.Append(fillChar, space);
                            tmp.Append(newString);
                            tmp.Append(m_maskCtrl.GetDecimalPoint());
                            newString.Clear();
                            space = 0;
                        }
                        else if(string[it] == fillChar)
                            space++;
                        else
                            newString << string[it];
                    }

                    tmp.Append(newString);
                    tmp.Append(fillChar, space);

                    ChangeValue(tmp);
                }
            break;
            case(WXK_DELETE):
                if(cursor < GetValue().Len() + 1 && m_maskCtrl.GetFormatCodes(0).Contains('_'))
                {
                    wxString mask = m_maskCtrl.GetEmptyMask()[cursor];

                    if(mask == fillChar)
                    {
                        Replace(cursor , cursor + 1 , m_maskCtrl.GetFillChar() );

                        cursor = GetInsertionPoint();
                        SetInsertionPoint(cursor);
                    }
                }

            break;
            case (WXK_TAB):
                if( !event.ShiftDown() )
                {
                    for(unsigned it = cursor; it < string.Len(); it++)
                    {
                        if(emptyMask[it] != fillChar)
                        {
                            index = it + 1;
                            it = string.Len();
                        }
                    }

                    if(index == 0)
                    {
                        wxWindow* window = (wxWindow *) event.GetEventObject();
                        window->NavigateIn();
                        SetInsertionPoint(0);
                    }
                    else
                        SetInsertionPoint(index);

                }
                else
                {
                    unsigned int it;
                    if(cursor > 1)
                    {
                        for(it = cursor - 2; it > 0; it--)
                        {
                            if(emptyMask[it] != fillChar)
                            {
                                index = it + 1;
                                it = 1;
                            }
                        }
                    }

                    if(cursor == 0 || index == 0)
                        event.Skip();
                    else
                        SetInsertionPoint(index);


                }


            break;
            case (WXK_SPACE):
                ch = (wxChar)keycode;
                if(m_maskCtrl.GetFormatCode().Find('_') != wxNOT_FOUND)
                {

                      if(mask == fillChar)
                      {
                          Replace(cursor , cursor + 1 , ch);
                      }
                      else
                      {
                          cursor = GetInsertionPoint();
                          SetInsertionPoint(cursor + 1);
                      }

                  }
            break;
            case (WXK_NUMPAD0):
            case (WXK_NUMPAD1):
            case (WXK_NUMPAD2):
            case (WXK_NUMPAD3):
            case (WXK_NUMPAD4):
            case (WXK_NUMPAD5):
            case (WXK_NUMPAD6):
            case (WXK_NUMPAD7):
            case (WXK_NUMPAD8):
            case (WXK_NUMPAD9):
            default:
            {
                if((wxChar)keycode == '-' && m_maskCtrl.GetFormatCode().Find('-') != wxNOT_FOUND)
                {
                    if(string.Find('-') == wxNOT_FOUND)
                    {
                        for(unsigned int it = 0; it < string.Len(); it++)
                        {
                            if(string[it] != fillChar)
                            {
                                tmp << '-';
                                tmp.Append(string.Mid(it));
                                it = string.Len();
                            }
                            else
                                tmp << string[it];
                        }
                    }
                    else
                    {
                        for(unsigned int it = 0; it < string.Len(); it++)
                        {
                            if(string[it] != '-')
                                tmp << string[it];
                        }
                    }
                    ChangeValue(tmp);
                }


                else if( keycode < 256 && keycode >= 0 && wxIsprint(keycode))
                {

                    if( !event.ShiftDown() )
                    {
                        keycode = wxTolower(keycode);
                    }

                    while(string.Len() > cursor && ( m_maskCtrl.GetEmptyMask()[cursor] != fillChar
                        || m_maskCtrl.GetMask()[cursor] == fillChar))
                    {
                        Replace(cursor, cursor + 1, string[cursor]);
                        cursor++;
                    }

                    ch = (wxChar)keycode;

                    if(cursor!= emptyMask.Len())
                    {
                        Replace(cursor, cursor+1, ch);
                        OnApplyMask(keycode);
                    }
               }
               else
                    event.Skip();

            }
        }
    }

}


// ----------------------------------------------------------------------------
// events
// ----------------------------------------------------------------------------

/* static */
bool wxTextEntryBase::SendTextUpdatedEvent(wxWindow *win)
{
    wxCHECK_MSG( win, false, "can't send an event without a window" );

    wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, win->GetId());

    // do not do this as it could be very inefficient if the text control
    // contains a lot of text and we're not using ref-counted wxString
    // implementation -- instead, event.GetString() will query the control for
    // its current text if needed
    //event.SetString(win->GetValue());

    event.SetEventObject(win);
    return win->HandleWindowEvent(event);
}

#endif // wxUSE_TEXTCTRL || wxUSE_COMBOBOX
