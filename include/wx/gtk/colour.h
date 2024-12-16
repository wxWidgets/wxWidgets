/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/colour.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_COLOUR_H_
#define _WX_GTK_COLOUR_H_

#ifdef __WXGTK3__
typedef struct _GdkRGBA GdkRGBA;
#endif

//-----------------------------------------------------------------------------
// wxColour
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWARN_UNUSED wxColour : public wxColourBase
{
public:
    // constructors
    // ------------
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS
    wxColour(const GdkColor& gdkColor);
#ifdef __WXGTK3__
    wxColour(const GdkRGBA& gdkRGBA);
#endif

    bool operator==(const wxColour& col) const;
    bool operator!=(const wxColour& col) const { return !(*this == col); }

    unsigned char Red() const override;
    unsigned char Green() const override;
    unsigned char Blue() const override;
    unsigned char Alpha() const override;

    // Implementation part
#ifdef __WXGTK3__
    operator const GdkRGBA*() const;
#else
    void CalcPixel( GdkColormap *cmap );
    int GetPixel() const;
#endif
    const GdkColor *GetColor() const;

protected:
    virtual void
    InitRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a) override;

    wxDECLARE_DYNAMIC_CLASS(wxColour);
};

#endif // _WX_GTK_COLOUR_H_
