///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/checkbox.h
// Purpose:     Persistence support for wxCheckBox.
// Author:      Alexander Koshelev, Vadim Zeitlin
// Created:     2025-06-11
// Copyright:   (c) 2025 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PERSIST_CHECKBOX_H_
#define _WX_PERSIST_CHECKBOX_H_

#include "wx/persist/window.h"

#include "wx/checkbox.h"

// ----------------------------------------------------------------------------
// string constants used by wxPersistentCheckBox
// ----------------------------------------------------------------------------

#define wxPERSIST_CHECKBOX_KIND wxASCII_STR("Checkbox")

#define wxPERSIST_CHECKBOX_VALUE wxASCII_STR("Value")

// ----------------------------------------------------------------------------
// wxPersistentCheckBox: supports saving/restoring checkbox state
// ----------------------------------------------------------------------------

// Implementation note: we depend on the values of wxCheckBoxState enum not
// changing, as we save them directly as numbers, but this seems a safe bet as
// it's hard to imagine adding any more values to it.

class wxPersistentCheckBox : public wxPersistentWindow<wxCheckBox>
{
public:
    explicit wxPersistentCheckBox(wxCheckBox* checkbox)
        : wxPersistentWindow<wxCheckBox>(checkbox)
    {
    }

    virtual void Save() const override
    {
        const wxCheckBox* const checkbox = Get();
        const auto value = checkbox->Get3StateValue();
        SaveValue(wxPERSIST_CHECKBOX_VALUE, static_cast<int>(value));
    }

    virtual bool Restore() override
    {
        int value;
        if ( !RestoreValue(wxPERSIST_CHECKBOX_VALUE, &value) )
            return false;

        switch ( value )
        {
            case wxCHK_UNCHECKED:
            case wxCHK_CHECKED:
                // These values are always valid.
                break;

            case wxCHK_UNDETERMINED:
                if ( Get()->Is3State() )
                {
                    // This value is valid for this checkbox.
                    break;
                }

                wxFALLTHROUGH;

            default:
                // Silently ignore invalid values, as it's not really clear
                // what else can we do about them.
                return false;
        }

        Get()->Set3StateValue(static_cast<wxCheckBoxState>(value));

        return true;
    }

    virtual wxString GetKind() const override { return wxPERSIST_CHECKBOX_KIND; }
};

inline wxPersistentObject *wxCreatePersistentObject(wxCheckBox* checkbox)
{
    return new wxPersistentCheckBox(checkbox);
}

#endif // _WX_PERSIST_CHECKBOX_H_
