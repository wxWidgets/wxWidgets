/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/tglbtn.h
// Purpose:     Declaration of the wxToggleButton class, which implements a
//              toggle button under wxGTK.
// Author:      John Norris, minor changes by Axel Schlueter
// Modified by:
// Created:     08.02.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Johnny C. Norris II
// License:     Rocketeer license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_TOGGLEBUTTON_H_
#define _WX_GTK_TOGGLEBUTTON_H_

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxToggleButton;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxCheckBoxNameStr;

//-----------------------------------------------------------------------------
// wxToggleButton
//-----------------------------------------------------------------------------

class wxToggleButton: public wxControl
{
public:
    // construction/destruction
    wxToggleButton() {}
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxCheckBoxNameStr)
    {
        Create(parent, id, label, pos, size, style, validator, name);
    }

    // Create the control
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    // Get/set the value
    void SetValue(bool state);
    bool GetValue() const;

    // Set the label
    void SetLabel(const wxString& label);
    bool Enable(bool enable = TRUE);

protected:
    // Callback function given to gtk
    static void gtk_togglebutton_clicked_callback(GtkWidget *widget,
                                                  wxToggleButton *win);

    // wx stuff
    void ApplyWidgetStyle();
    bool IsOwnGtkWindow(GdkWindow *window);

    virtual void OnInternalIdle();
    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_DYNAMIC_CLASS(wxToggleButton)
};

#endif // _WX_GTK_TOGGLEBUTTON_H_

