/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/tglbtn.h
// Author:      Peter Most, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_TGLBTN_H_
#define _WX_QT_TGLBTN_H_

#include "wx/tglbtn.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxCheckBoxNameStr[];

class WXDLLIMPEXP_CORE wxBitmapToggleButton: public wxToggleButtonBase
{
public:
    wxBitmapToggleButton();
    wxBitmapToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxBitmap& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxCheckBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxBitmap& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    virtual void SetValue(bool state);
    virtual bool GetValue() const;

    virtual QWidget *GetHandle() const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxBitmapToggleButton);

};



class WXDLLIMPEXP_CORE wxToggleButton : public wxToggleButtonBase
{
public:
    wxToggleButton();
    wxToggleButton(wxWindow *parent,
                   wxWindowID id,
                   const wxString& label,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxCheckBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& label,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize, long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxCheckBoxNameStr);

    virtual void SetValue(bool state);
    virtual bool GetValue() const;

    virtual QWidget *GetHandle() const;

private:

};

#endif // _WX_QT_TGLBTN_H_
