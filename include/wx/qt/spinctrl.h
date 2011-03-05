/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/spinctrl.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_SPINCTRL_H_
#define _WX_QT_SPINCTRL_H_

#include "wx/qt/pointer_qt.h"
#include <QtGui/QSpinBox>
#include <QtGui/QDoubleSpinBox>

// Define a derived helper class to get access to valueFromText:

template < typename Widget >
class WXDLLIMPEXP_CORE wxQtSpinBoxBase : public Widget
{
public:
    wxQtSpinBoxBase( QWidget *parent )
        : Widget( parent )
    { }

    using Widget::valueFromText;
};

class WXDLLIMPEXP_CORE wxQtSpinBox : public wxQtSpinBoxBase< QSpinBox >
{
public:
    wxQtSpinBox( QWidget *parent )
        : wxQtSpinBoxBase< QSpinBox >( parent )
    { }
};

class WXDLLIMPEXP_CORE wxQtDoubleSpinBox : public wxQtSpinBoxBase< QDoubleSpinBox >
{
public:
    wxQtDoubleSpinBox( QWidget *parent )
        : wxQtSpinBoxBase< QDoubleSpinBox >( parent )
    { }
};

// Take advantage of the Qt compile time polymorphy and use a template to avoid
// copy&paste code for the usage of QSpinBox/QDoubleSpinBox.

template < typename T, typename Widget >
class WXDLLIMPEXP_CORE wxQtSpinCtrlBase : public wxSpinCtrlBase
{
public:
    wxQtSpinCtrlBase();
    wxQtSpinCtrlBase( wxWindow *parent, wxWindowID id, const wxString& value,
        const wxPoint& pos, const wxSize& size, long style,
        T min, T max, T initial, T inc,
        const wxString& name );

    bool Create( wxWindow *parent, wxWindowID id, const wxString& value,
        const wxPoint& pos, const wxSize& size, long style,
        T min, T max, T initial, T inc,
        const wxString& name );

    virtual void SetValue(const wxString& value);

    virtual void SetSnapToTicks(bool snap_to_ticks);
    virtual bool GetSnapToTicks() const;

    virtual void SetSelection(long from, long to);

    void SetValue(T val);
    void SetRange(T minVal, T maxVal);
    void SetIncrement(T inc);

    T GetValue() const;
    T GetMin() const;
    T GetMax() const;
    T GetIncrement() const;

    virtual Widget *GetHandle() const;

protected:
    wxQtPointer< Widget > m_qtSpinBox;
};

class WXDLLIMPEXP_CORE wxSpinCtrl : public wxQtSpinCtrlBase< int, wxQtSpinBox >
{
public:
    wxSpinCtrl();
    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxT("wxSpinCtrl"));

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = wxT("wxSpinCtrl"));

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY( wxSpinCtrl )
};

class WXDLLIMPEXP_CORE wxSpinCtrlDouble : public wxQtSpinCtrlBase< double, wxQtDoubleSpinBox >
{
public:
    wxSpinCtrlDouble();
    wxSpinCtrlDouble(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                     double min = 0, double max = 100, double initial = 0,
                     double inc = 1,
                     const wxString& name = wxT("wxSpinCtrlDouble"));

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS | wxALIGN_RIGHT,
                double min = 0, double max = 100, double initial = 0,
                double inc = 1,
                const wxString& name = wxT("wxSpinCtrlDouble"));

    void SetDigits(unsigned digits);
    unsigned GetDigits() const;

private:
    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxSpinCtrlDouble );
};

#endif // _WX_QT_SPINCTRL_H_
