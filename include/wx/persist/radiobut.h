///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/radiobut.h
// Purpose:     Persistence support for wxRadioButton.
// Author:      Vadim Zeitlin
// Created:     2025-06-12
// Copyright:   (c) 2025 Vadim Zeitlin
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PERSIST_RADIOBUT_H_
#define _WX_PERSIST_RADIOBUT_H_

#include "wx/persist/window.h"

#include "wx/radiobut.h"

#if wxUSE_RADIOBTN

// ----------------------------------------------------------------------------
// string constants used by wxPersistentRadioButton
// ----------------------------------------------------------------------------

#define wxPERSIST_RADIOBUTTON_KIND wxASCII_STR("RadioButton")

#define wxPERSIST_RADIOBUTTON_VALUE wxASCII_STR("Value")

// ----------------------------------------------------------------------------
// wxPersistentRadioButton: supports saving/restoring radio buttons state
// ----------------------------------------------------------------------------

// This class should be always used with the first button in a group of radio
// buttons and it saves its state as an integer indicating the offset of the
// selected radio button in this group.
//
// Currently radio buttons with wxRB_SINGLE style are not supported. While it
// wouldn't be very difficult to add support for them if anybody really needs
// it, it seems relatively unlikely, so for now we don't bother to do it.

class wxPersistentRadioButton : public wxPersistentWindow<wxRadioButton>
{
public:
    explicit wxPersistentRadioButton(wxRadioButton* radiobutton)
        : wxPersistentWindow<wxRadioButton>(radiobutton)
    {
        wxASSERT_MSG( !radiobutton->HasFlag(wxRB_SINGLE),
                      "wxPersistentRadioButton doesn't support wxRB_SINGLE" );

        wxASSERT_MSG( radiobutton->HasFlag(wxRB_GROUP),
                      "wxPersistentRadioButton should be used with the first "
                      "radio button in a group" );
    }

    virtual void Save() const override
    {
        const wxRadioButton* button = Get();
        for ( int n = 0;; ++n )
        {
            if ( button->GetValue() )
            {
                // We found the selected button, save its index.
                SaveValue(wxPERSIST_RADIOBUTTON_VALUE, n);
                break;
            }

            button = button->GetNextInGroup();
            if ( !button )
            {
                wxFAIL_MSG("Didn't find a selected radio button in the group?");
                break;
            }
        }
    }

    virtual bool Restore() override
    {
        int value;
        if ( !RestoreValue(wxPERSIST_RADIOBUTTON_VALUE, &value) )
            return false;

        wxRadioButton* button = Get();
        for ( int n = 0;; ++n )
        {
            if ( n == value )
            {
                // We found the button with the saved index, set its value.
                button->SetValue(true);
                break;
            }

            button = button->GetNextInGroup();

            if ( !button )
            {
                // The saved value is invalid, silently ignore it because there
                // doesn't seem to be much else that we can do in this case.
                break;
            }
        }

        return true;
    }

    virtual wxString GetKind() const override { return wxPERSIST_RADIOBUTTON_KIND; }
};

inline wxPersistentObject *wxCreatePersistentObject(wxRadioButton* radiobutton)
{
    return new wxPersistentRadioButton(radiobutton);
}

#endif // wxUSE_RADIOBTN

#endif // _WX_PERSIST_RADIOBUT_H_
