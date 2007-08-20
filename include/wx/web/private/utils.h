// Copyright (c) 2007 John Wilmes

#include "wx/colour.h"
#include "wx/string.h"
#include "wx/pen.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/gdicmn.h"
#include "wx/dynarray.h"
#include "wx/utils.h"
#include "wx/event.h"
#include <Magick++.h>

WX_DEFINE_ARRAY_INT(int, WebKeyArray);

class WebStateManager {
private:
    WebStateManager() {}

public:
    WebKeyArray m_keysPressed;
    wxMouseState m_mouseState;

public:
    static void Initialize();
    static void UpdateKeyState(const wxKeyEvent& evt);
    static void UpdateMouseState(const wxMouseEvent& evt);
    static bool GetKeyState(wxKeyCode key);
    static wxMouseState GetMouseState() {return wsm->m_mouseState; }
    static void GetMousePosition(int* x, int* y);

protected:
    static WebStateManager* wsm;
};

Magick::Color GetMagickColor(const wxColour& wxcol);

wxColour GetWxColour(const Magick::Color& mcol);

char * GetMagick(wxBitmapType type);

wxString GetJsonPointsString(int n, wxPoint points[]);

wxString GetJsonColourString(const wxColour& col);

wxString GetJsonBrushString(const wxBrush& brush);

wxString GetJsonPenString(const wxPen& pen);

wxString GetJsonFontString(const wxFont& font);

Magick::LineCap GetMagickCap(int cap);

Magick::LineJoin GetMagickJoin(int join);

Magick::CompositeOperator GetMagickCompositeOp(int op);
