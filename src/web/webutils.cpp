#include "wx/web/private/utils.h"
#include <json/json.h>

WebStateManager* WebStateManager::wsm = NULL;

void WebStateManager::Initialize() {
    wsm = new WebStateManager();
}

void WebStateManager::UpdateKeyState(const wxKeyEvent& evt) {
    int code = evt.GetKeyCode();
    if (evt.GetEventType() == wxEVT_KEY_DOWN) {
        wsm->m_keysPressed.Add(code);
    } else if (evt.GetEventType() == wxEVT_KEY_UP) {
        for (int i = wsm->m_keysPressed.Index(code); i != wxNOT_FOUND;
             i = wsm->m_keysPressed.Index(code)) {
            wsm->m_keysPressed.RemoveAt(i);
        }
    }
}

void WebStateManager::UpdateMouseState(const wxMouseEvent& evt) {
    wsm->m_mouseState.SetX(evt.m_x);
    wsm->m_mouseState.SetY(evt.m_y);

    wsm->m_mouseState.SetLeftDown(evt.m_leftDown);
    wsm->m_mouseState.SetMiddleDown(evt.m_middleDown);
    wsm->m_mouseState.SetRightDown(evt.m_rightDown);

    wsm->m_mouseState.SetControlDown(evt.m_controlDown);
    wsm->m_mouseState.SetShiftDown(evt.m_shiftDown);
    wsm->m_mouseState.SetAltDown(evt.m_altDown);
    wsm->m_mouseState.SetMetaDown(evt.m_metaDown);
}

bool WebStateManager::GetKeyState(wxKeyCode key) {
    return (wsm->m_keysPressed.Index(key) != wxNOT_FOUND);
}

void WebStateManager::GetMousePosition(int *x, int *y) {
    *x = wsm->m_mouseState.GetX();
    *y = wsm->m_mouseState.GetY();
}

Magick::Color GetMagickColor(const wxColour& wxcol) {
    Magick::ColorRGB mcol(wxcol.Red() / 255.0, wxcol.Green() / 255.0, wxcol.Blue() / 255.0);
    mcol.alpha(wxcol.Alpha() / 255.0);
    return mcol;
}

wxColour GetWxColour(const Magick::Color& mcol) {
    Magick::ColorRGB rgb(mcol);
    wxColour wxcol((char) rgb.red()*255, (char) rgb.green()*255, (char) rgb.blue()*255, (char)rgb.alpha()*255);
    return wxcol;
}

char * GetMagick(wxBitmapType type) {
    switch (type) {
        case wxBITMAP_TYPE_BMP:
            return "BMP";
        case wxBITMAP_TYPE_ICO: //Read-only
            return "ICO";
        case wxBITMAP_TYPE_CUR: //Read-only
            return "CUR";
        case wxBITMAP_TYPE_XBM: //Monochrome-only
            return "XBM";
        case wxBITMAP_TYPE_XPM:
            return "XPM";
        case wxBITMAP_TYPE_TIF:
            return "TIFF";
        case wxBITMAP_TYPE_GIF:
            return "GIF";
        case wxBITMAP_TYPE_PNG:
            return "PNG";
        case wxBITMAP_TYPE_PCX:
            return "PCX";
        case wxBITMAP_TYPE_PICT:
            return "PICT";
        case wxBITMAP_TYPE_ICON: //Read-only
            return "ICO"; //Is this correct?
        case wxBITMAP_TYPE_TGA:
            return "TGA";
        /* Unsupported formats
        case wxBITMAP_TYPE_ANI:
        case wxBITMAP_TYPE_IFF:
        case wxBITMAP_TYPE_MACCURSOR:
        */
        default:
            return "PNG";
    }
}

json_object* GetJsonPoint(wxPoint p) {
    json_object *jpt = json_object_new_array();
    json_object_array_add(jpt, json_object_new_int(p.x));
    json_object_array_add(jpt, json_object_new_int(p.y));
    return jpt;
}

wxString GetJsonPointsString(int n, wxPoint points[]) {
    json_object *array = json_object_new_array();
    for (int i = 0; i < n; ++i) {
        json_object_array_add(array, GetJsonPoint(points[i]));
    }
    return json_object_to_json_string(array);
}

json_object* GetJsonColour(const wxColour& col) {
    json_object *jcol = json_object_new_object();
    json_object_object_add(jcol, "red", json_object_new_int(col.Red()));
    json_object_object_add(jcol, "green", json_object_new_int(col.Green()));
    json_object_object_add(jcol, "blue", json_object_new_int(col.Blue()));
    json_object_object_add(jcol, "alpha", json_object_new_int(col.Alpha()));
    return jcol;
}

wxString GetJsonColourString(const wxColour& col) {
    return json_object_to_json_string(GetJsonColour(col));
}

wxString GetJsonBrushString(const wxBrush& brush) {
    json_object *jbrush = json_object_new_object();
    json_object_object_add(jbrush, "style", json_object_new_int(brush.GetStyle()));
    json_object_object_add(jbrush, "colour", GetJsonColour(brush.GetColour()));
    return json_object_to_json_string(jbrush);
}

wxString GetJsonPenString(const wxPen& pen) {
    json_object *jpen = json_object_new_object();
    json_object_object_add(jpen, "width", json_object_new_int(pen.GetWidth()));
    json_object_object_add(jpen, "style", json_object_new_int(pen.GetStyle()));
    json_object_object_add(jpen, "join", json_object_new_int(pen.GetJoin()));
    json_object_object_add(jpen, "cap", json_object_new_int(pen.GetCap()));
    json_object_object_add(jpen, "colour", GetJsonColour(pen.GetColour()));
    return json_object_to_json_string(jpen);
}

wxString GetJsonFontString(const wxFont& font) {
    json_object *jfont = json_object_new_object();
    json_object_object_add(jfont, "size", json_object_new_int(font.GetPointSize()));
    json_object_object_add(jfont, "family", json_object_new_int(font.GetFamily()));
    json_object_object_add(jfont, "style", json_object_new_int(font.GetStyle()));
    json_object_object_add(jfont, "weight", json_object_new_int(font.GetWeight()));
    json_object_object_add(jfont, "underlined", json_object_new_boolean(font.GetUnderlined()));
    //TODO -> const conversion issue, copy the string
    //json_object_object_add(jfont, "name", json_object_new_string((const char*)font.GetFaceName().c_str()));
    return json_object_to_json_string(jfont);
}

Magick::LineCap GetMagickCap(int cap) {
    switch (cap) {
        case wxCAP_PROJECTING:
            return Magick::SquareCap;
        case wxCAP_BUTT:
            return Magick::ButtCap;
        default:
            // including case wxCAP_ROUND
            return Magick::RoundCap;
    }
}

Magick::LineJoin GetMagickJoin(int join) {
    switch (join) {
        case wxJOIN_BEVEL:
            return Magick::BevelJoin;
        case wxJOIN_MITER:
            return Magick::MiterJoin;
        default:
            //including case wxJOIN_ROUND
            return Magick::RoundJoin;
    }
}

Magick::CompositeOperator GetMagickCompositeOp(int op) {
    switch (op) {
    //case wxAND
    //wxAND_INVERT          (NOT src) AND dst
    //wxAND_REVERSE         src AND (NOT dst)
    case wxCLEAR:
        return Magick::ClearCompositeOp;
    case wxCOPY:
        return Magick::OverCompositeOp;
    //wxEQUIV               (NOT src) XOR dst
    //wxINVERT              NOT dst
    //wxNAND                (NOT src) OR (NOT dst)
    //wxNOR                 (NOT src) AND (NOT dst)
    //wxNO_OP               dst
    //wxOR                  src OR dst
    //wxOR_INVERT           (NOT src) OR dst
    //wxOR_REVERSE          src OR (NOT dst)
    //wxSET                 1
    //wxSRC_INVERT          NOT src
    case wxXOR: //I don't think these are exactly equivalent
        return Magick::XorCompositeOp;
    default:
        return Magick::OverCompositeOp;
    }
}
