#ifndef _WX_LEDNUMBERCTRL_H_
#define _WX_LEDNUMBERCTRL_H_

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "wxLEDNumberCtrl.h"
#endif

#include "wx/gizmos/gizmos.h"

#include <wx/window.h>
#include <wx/control.h>

class wxEraseEvent;
class wxPaintEvent;
class wxSizeEvent;

// ----------------------------------------------------------------------------
// enum and styles
// ----------------------------------------------------------------------------

enum wxLEDValueAlign
{
    wxLED_ALIGN_LEFT   = 0x01,
    wxLED_ALIGN_RIGHT  = 0x02,
    wxLED_ALIGN_CENTER = 0x04,

    wxLED_ALIGN_MASK   = 0x04
};

#define wxLED_DRAW_FADED 0x08

// ----------------------------------------------------------------------------
// wxLEDNumberCtrl
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_GIZMOS wxLEDNumberCtrl : public wxControl
{
public:
    // Constructors.
    wxLEDNumberCtrl();
    wxLEDNumberCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxLED_ALIGN_LEFT | wxLED_DRAW_FADED);

    // Create functions.
    bool Create(wxWindow *parent, wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0);

    wxLEDValueAlign GetAlignment() const { return m_Alignment; }
    bool GetDrawFaded() const { return m_DrawFaded; }
    const wxString &GetValue() const { return m_Value; }

    void SetAlignment(wxLEDValueAlign Alignment, bool Redraw = true);
    void SetDrawFaded(bool DrawFaded, bool Redraw = true);
    void SetValue(const wxString &Value, bool Redraw = true);

private:
    // Members.
    wxString m_Value;
    wxLEDValueAlign m_Alignment;

    int m_LineMargin;
    int m_DigitMargin;
    int m_LineLength;
    int m_LineWidth;
    bool m_DrawFaded;
    int m_LeftStartPos;

    // Functions.
    void DrawDigit(wxDC &Dc, int Digit, int Column);
    void RecalcInternals(const wxSize &CurrentSize);

    // Events.
    DECLARE_EVENT_TABLE()

    void OnEraseBackground(wxEraseEvent &Event);
    void OnPaint(wxPaintEvent &Event);
    void OnSize(wxSizeEvent &Event);
};

#endif
