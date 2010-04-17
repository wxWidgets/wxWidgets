/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/icon.h
// Purpose:     wxIcon header
// Author:      Peter Most
// Copyright:   (c) Peter Most
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_ICON_H_
#define _WX_QT_ICON_H_

#include "wx/bitmap.h"

class WXDLLIMPEXP_CORE wxIcon : public wxBitmap
{
public:
    wxIcon();
    wxIcon(const char* const* bits);
#ifdef wxNEEDS_CHARPP
    wxIcon(char **bits);
#endif
    wxIcon( const wxString& filename,
            wxBitmapType type = wxICON_DEFAULT_TYPE,
            int desiredWidth = -1, int desiredHeight = -1 );

    wxIcon(const wxIconLocation& loc);

    void CopyFromBitmap(const wxBitmap& bmp);
    
protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;
    
private:
    DECLARE_DYNAMIC_CLASS(wxIcon)
};


#endif // _WX_QT_ICON_H_
