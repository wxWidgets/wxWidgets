/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/combo.h
// Purpose:     wxComboControl class
// Author:      Jaakko Salli
// Modified by:
// Created:     Apr-30-2006
// RCS-ID:      $Id$
// Copyright:   (c) Jaakko Salli
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOCONTROL_H_
#define _WX_COMBOCONTROL_H_

// NB: Definition of _WX_COMBOCONTROL_H_ is used in wx/generic/combo.h to
//     determine whether there is native wxComboControl, so make sure you
//     use it in all native wxComboControls.

#if wxUSE_COMBOCONTROL

// ----------------------------------------------------------------------------
// Native wxComboControl
// ----------------------------------------------------------------------------

// Define this only if native implementation includes all features
#define wxCOMBOCONTROL_FULLY_FEATURED

class WXDLLEXPORT wxComboControl : public wxComboControlBase
{
public:
    // ctors and such
    wxComboControl() : wxComboControlBase() { Init(); }

    wxComboControl(wxWindow *parent,
                   wxWindowID id = wxID_ANY,
                   const wxString& value = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxComboBoxNameStr)
        : wxComboControlBase()
    {
        Init();

        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    virtual ~wxComboControl();

    virtual void DrawFocusBackground( wxDC& dc, const wxRect& rect, int flags );

    static int GetFeatures() { return wxComboControlFeatures::All; }

protected:

    // customization
    virtual void OnResize();
    virtual wxCoord GetNativeTextIndent() const;
    virtual void OnThemeChange();

    // event handlers
    void OnPaintEvent( wxPaintEvent& event );
    void OnMouseEvent( wxMouseEvent& event );

private:
    void Init();

    DECLARE_EVENT_TABLE()

    DECLARE_DYNAMIC_CLASS(wxComboControl)
};


#endif // wxUSE_COMBOCONTROL
#endif
    // _WX_COMBOCONTROL_H_
