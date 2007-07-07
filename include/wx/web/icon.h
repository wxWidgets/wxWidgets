#ifndef __WX_ICON_H__
#define __WX_ICON_H__

#include "wx/bitmap.h"

class WXDLLEXPORT wxIcon: public wxBitmap
{
public:
    wxIcon();
    wxIcon(const char **bits, int width=-1, int height=-1);
    wxIcon(char **bits, int width=-1, int height=-1);

    wxIcon(const wxString& filename, int type = wxBITMAP_TYPE_ICO_RESOURCE,
           int WXUNUSED(desiredWidth)=-1, int WXUNUSED(desiredHeight)=-1 );

    wxIcon(const wxIconLocation& loc);

    void CopyFromBitmap(const wxBitmap& bmp);

private:
    DECLARE_DYNAMIC_CLASS(wxIcon)
};


#endif // __WX_ICON_H__
