#ifndef __WX_DCCLIENT_H__
#define __WX_DCCLIENT_H__

#include "wx/dc.h"

class WXDLLEXPORT wxWindowDC : public wxDC {
public:
    wxWindowDC();
    virtual ~wxWindowDC();
    wxWindowDC(wxWindow *window);

private:
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
};

class WXDLLEXPORT wxClientDC : public wxWindowDC {
public:
    wxClientDC();
    wxClientDC(wxWindow *window);

private:
    DECLARE_DYNAMIC_CLASS(wxClientDC)
};

class WXDLLEXPORT wxPaintDC : public wxWindowDC {
public:
    wxPaintDC();
    wxPaintDC(wxWindow *window);

private:
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
};

#endif // __WX_DCCLIENT_H__
