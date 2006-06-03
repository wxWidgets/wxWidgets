/////////////////////////////////////////////////////////////////////////////
// Name:        wx/pickerbase.h
// Purpose:     wxPickerBase definition
// Author:      Francesco Montorsi (based on Vadim Zeitlin's code)
// Modified by:
// Created:     14/4/2006
// Copyright:   (c) Vadim Zeitlin, Francesco Montorsi
// RCS-ID:      $Id$
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PICKERBASE_H_BASE_
#define _WX_PICKERBASE_H_BASE_

#include "wx/control.h"

class WXDLLIMPEXP_CORE wxTextCtrl;

extern WXDLLEXPORT_DATA(const wxChar) wxButtonNameStr[];

// ----------------------------------------------------------------------------
// wxPickerBase is the base class for the picker controls which support
// a wxPB_USE_TEXTCTRL style; i.e. for those pickers which can use an auxiliary
// text control next to the 'real' picker.
//
// The wxTextPickerHelper class manages enabled/disabled state of the text control,
// its sizing and positioning.
// ----------------------------------------------------------------------------

#define wxPB_USE_TEXTCTRL           0x0002

class WXDLLIMPEXP_CORE wxPickerBase : public wxControl
{
public:
    // ctor: text is the associated text control
    wxPickerBase() : m_text(NULL), m_picker(NULL),
                     m_margin(5), m_textProportion(2) {}
    virtual ~wxPickerBase();


    // if present, intercepts wxPB_USE_TEXTCTRL style and creates the text control
    // The 3rd argument is the initial wxString to display in the text control
    bool CreateBase(wxWindow *parent, wxWindowID id,
        const wxString& text = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxButtonNameStr);


public:     // public API

    // margin between the text control and the picker
    void SetInternalMargin(int newmargin);
    int GetInternalMargin() const { return m_margin; }

    // proportion of the text control respect the picker
    // (which has a fixed proportion value of 1)
    void SetTextCtrlProportion(int prop) { wxASSERT(prop>=1); m_textProportion=prop; }
    int GetTextCtrlProportion() const { return m_textProportion; }

    bool HasTextCtrl() const
        { return m_text != NULL; }
    wxTextCtrl *GetTextCtrl()
        { return m_text; }
    wxControl *GetPickerCtrl()
        { return m_picker; }

public:     // wxWindow overrides

    void DoSetSizeHints(int minW, int minH,
                        int maxW = wxDefaultCoord, int maxH = wxDefaultCoord,
                        int incW = wxDefaultCoord, int incH = wxDefaultCoord );

protected:
    void DoSetSize(int x, int y,
                   int width, int height,
                   int sizeFlags = wxSIZE_AUTO);

    wxSize DoGetBestSize() const;


public:     // methods that derived class must/may override

    virtual void UpdatePickerFromTextCtrl() = 0;
    virtual void UpdateTextCtrlFromPicker() = 0;

protected:        // utility functions

    inline int GetTextCtrlWidth(int given);

    // event handlers
    void OnTextCtrlDelete(wxWindowDestroyEvent &);
    void OnTextCtrlUpdate(wxCommandEvent &);
    void OnTextCtrlKillFocus(wxFocusEvent &);

    // returns the set of styles for the attached wxTextCtrl
    // from given wxPickerBase's styles
    virtual long GetTextCtrlStyle(long style) const
        { return (style & wxWINDOW_STYLE_MASK); }

    // returns the set of styles for the m_picker
    virtual long GetPickerStyle(long style) const
        { return (style & wxWINDOW_STYLE_MASK); }

protected:
    wxTextCtrl *m_text;     // can be NULL
    wxControl *m_picker;

    int m_margin;           // distance between subcontrols
    int m_textProportion;   // proportion between textctrl and other item

private:
    DECLARE_ABSTRACT_CLASS(wxPickerBase)
};


#endif
    // _WX_PICKERBASE_H_BASE_
