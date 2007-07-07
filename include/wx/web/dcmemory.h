#ifndef __WX_DCMEMORY_H__
#define __WX_DCMEMORY_H__

#include "wx/dc.h"
#include "wx/dcclient.h"

class WXDLLEXPORT wxMemoryDC : public wxWindowDC, public wxMemoryDCBase {
public:
    wxMemoryDC();
    wxMemoryDC(wxDC *dc);
    ~wxMemoryDC();

protected:
    virtual void DoSelect(const wxBitmap& bmp);

private:
    DECLARE_DYNAMIC_CLASS(wxMemoryDC)
};

#endif // __WX_DCMEMORY_H__
