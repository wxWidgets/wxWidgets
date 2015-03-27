/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/palette.h
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_QT_PALETTE_H_
#define WX_QT_PALETTE_H_

class WXDLLIMPEXP_CORE wxPalette : public wxPaletteBase
{
public:
    wxPalette();
    wxPalette(int n, unsigned char *red, unsigned char *green, unsigned char *blue);
    
    bool Create(int n, unsigned char *red, unsigned char *green, unsigned char *blue);
    
    bool GetRGB(int pixel, unsigned char *red, unsigned char *green, unsigned char *blue) const;
    int GetPixel(unsigned char red, unsigned char green, unsigned char blue) const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

};

#endif // _WX_QT_PALETTE_H_
