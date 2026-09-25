///////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/statusbr.h
// Purpose:     native implementation of wxStatusBar.
//              Optional: can use generic version instead.
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATBAR_H_
#define _WX_STATBAR_H_

class WXDLLIMPEXP_CORE wxStatusBarMac : public wxStatusBarGeneric
{
public:
    wxStatusBarMac();
    wxStatusBarMac(wxWindow *parent, wxWindowID id = wxID_ANY,
           long style = wxSTB_DEFAULT_STYLE,
           const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    virtual ~wxStatusBarMac();

    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
              long style = wxSTB_DEFAULT_STYLE,
              const wxString& name = wxASCII_STR(wxStatusBarNameStr));

    // Implementation
    virtual void MacHiliteChanged() override;
    void OnPaint(wxPaintEvent& event);
    virtual bool GetFieldRect(int i, wxRect& rect) const override;

    void MacSetCornerInset(int inset);
    int MacGetCornerInset() const { return m_cornerInset; }

protected:
    virtual int GetEffectiveFieldStyle(int WXUNUSED(i)) const override { return wxSB_NORMAL; }

    virtual void DoUpdateStatusText(int number) override;

    // Let VoiceOver read the fields, which are not windows and so are invisible
    // to it otherwise. Does nothing if nothing has changed since the last call,
    // as replacing the elements would make VoiceOver lose its position.
    void UpdateAccessibleFields();

    virtual void InitColours() override;

    void InitCornerInset();

    // The labels and the rectangles used by the last UpdateAccessibleFields().
    wxArrayString m_accessibleLabels;
    wxVector<wxRect> m_accessibleRects;

    virtual int GetAvailableWidthForFields(int width) const override;

private:
    int m_cornerInset;
    wxColour m_textActive, m_textInactive, m_bgActive, m_bgInactive, m_separator;

    wxDECLARE_DYNAMIC_CLASS(wxStatusBarMac);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_STATBAR_H_
