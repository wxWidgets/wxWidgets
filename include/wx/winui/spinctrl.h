/////////////////////////////////////////////////////////////////////////////
// Name:        wx/winui/spinctrl.h
// Purpose:     wxWinUI wxSpinCtrl / wxSpinCtrlDouble (WinUI NumberBox)
// Author:      wxWidgets development team
// Created:     2026-06-01
// Copyright:   (c) wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINUI_SPINCTRL_H_
#define _WX_WINUI_SPINCTRL_H_

#include <memory>

class wxWinUINumberBoxImpl;

// ----------------------------------------------------------------------------
// wxSpinCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSpinCtrl : public wxSpinCtrlBase
{
public:
    wxSpinCtrl();
    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = wxT("wxSpinCtrl"));
    ~wxSpinCtrl() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = wxT("wxSpinCtrl"));

    int GetValue() const { return m_value; }
    int GetMin() const { return m_min; }
    int GetMax() const { return m_max; }
    int GetIncrement() const { return m_increment; }

    void SetValue(int value);
    void SetValue(const wxString& value) override;
    void SetRange(int minVal, int maxVal);
    void SetIncrement(int inc);

    wxString GetTextValue() const override;
    bool GetSnapToTicks() const override { return m_snapToTicks; }
    void SetSnapToTicks(bool snap) override;
    int GetBase() const override { return m_numBase; }
    bool SetBase(int base) override;
    void SetSelection(long from, long to) override;

    // Implementation-only deterministic seams. Peer mutations traverse the
    // real NumberBox callbacks but don't require SendInput.
    bool WinUISetPeerValueForTesting(double value);
    bool WinUISetPeerTextForTesting(const wxString& text);
    bool WinUIGetPeerStateForTesting(double *minimum,
                                     double *maximum,
                                     double *increment,
                                     bool *wrap,
                                     wxString *text,
                                     double *value = nullptr) const;
    bool WinUIGetPeerSelectionForTesting(long *from, long *to);
    bool WinUIRetemplateForTesting();
    bool WinUIEnterForTesting();
    bool WinUIStepForTesting(int direction);

protected:
    wxSize DoGetBestSize() const override;

    bool ApplyToPeer();
    void ApplyProgrammaticChange(bool selectAll);
    void ResolveTextPart(bool updateLayout = true);
    void ApplySelectionToPeer();
    void OnPeerTextChanged(const wxString& text);
    void OnPeerValueChanged(double newValue);
    void OnPeerEnter();
    bool ParseText(const wxString& text, int *value) const;
    wxString FormatValue(int value) const;
    int AdjustValue(int value, bool wrap = false) const;
    int ValueAfterStep(int direction) const;

    std::unique_ptr<wxWinUINumberBoxImpl> m_winui;
    int m_value = 0;
    int m_min = 0;
    int m_max = 100;
    int m_increment = 1;
    int m_numBase = 10;
    bool m_snapToTicks = false;
    wxString m_textValue;
    long m_selectionFrom = 0;
    long m_selectionTo = 0;
    bool m_hasPendingSelection = false;

private:
    friend class wxWinUINumberBoxImpl;
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSpinCtrl);
};

// ----------------------------------------------------------------------------
// wxSpinCtrlDouble
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxSpinCtrlDouble : public wxSpinCtrlBase
{
public:
    wxSpinCtrlDouble();
    wxSpinCtrlDouble(wxWindow *parent,
                     wxWindowID id = wxID_ANY,
                     const wxString& value = wxEmptyString,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxSP_ARROW_KEYS,
                     double min = 0, double max = 100, double initial = 0,
                     double inc = 1,
                     const wxString& name = wxT("wxSpinCtrlDouble"));
    ~wxSpinCtrlDouble() override;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                double min = 0, double max = 100, double initial = 0,
                double inc = 1,
                const wxString& name = wxT("wxSpinCtrlDouble"));

    double GetValue() const { return m_value; }
    double GetMin() const { return m_min; }
    double GetMax() const { return m_max; }
    double GetIncrement() const { return m_increment; }
    unsigned GetDigits() const { return m_digits; }

    void SetValue(double value);
    void SetValue(const wxString& value) override;
    void SetRange(double minVal, double maxVal);
    void SetIncrement(double inc);
    void SetDigits(unsigned digits);

    wxString GetTextValue() const override;
    bool GetSnapToTicks() const override { return m_snapToTicks; }
    void SetSnapToTicks(bool snap) override;
    int GetBase() const override { return 10; }
    bool SetBase(int base) override { return base == 10; }
    void SetSelection(long from, long to) override;

    bool WinUISetPeerValueForTesting(double value);
    bool WinUISetPeerTextForTesting(const wxString& text);
    bool WinUIGetPeerStateForTesting(double *minimum,
                                     double *maximum,
                                     double *increment,
                                     bool *wrap,
                                     wxString *text,
                                     double *value = nullptr) const;
    bool WinUIGetPeerSelectionForTesting(long *from, long *to);
    bool WinUIRetemplateForTesting();
    bool WinUIEnterForTesting();
    bool WinUIStepForTesting(int direction);

protected:
    wxSize DoGetBestSize() const override;

    bool ApplyToPeer();
    void ApplyProgrammaticChange(bool selectAll);
    void ResolveTextPart(bool updateLayout = true);
    void ApplySelectionToPeer();
    void OnPeerTextChanged(const wxString& text);
    void OnPeerValueChanged(double newValue);
    void OnPeerEnter();
    bool ParseText(const wxString& text, double *value) const;
    wxString FormatValue(double value) const;
    double AdjustValue(double value, bool wrap = false) const;
    double CanonicalizeValue(double value, bool wrap = false) const;
    double ValueAfterStep(int direction) const;

    std::unique_ptr<wxWinUINumberBoxImpl> m_winui;
    double m_value = 0;
    double m_min = 0;
    double m_max = 100;
    double m_increment = 1;
    unsigned m_digits = 0;
    bool m_snapToTicks = false;
    wxString m_textValue;
    long m_selectionFrom = 0;
    long m_selectionTo = 0;
    bool m_hasPendingSelection = false;

private:
    friend class wxWinUINumberBoxImpl;
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxSpinCtrlDouble);
};

#endif // _WX_WINUI_SPINCTRL_H_
