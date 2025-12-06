/////////////////////////////////////////////////////////////////////////////
// Name:        wx/colour.h
// Purpose:     wxColourBase definition
// Author:      Julian Smart
// Modified by: Francesco Montorsi
// Created:
// Copyright:   Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_BASE_
#define _WX_COLOUR_H_BASE_

#include "wx/defs.h"
#include "wx/gdiobj.h"
#include "wx/variant.h"

class WXDLLIMPEXP_FWD_CORE wxColour;

// flags for wxColour -> wxString conversion (see wxColour::GetAsString)
enum {
    wxC2S_NAME             = 1,   // return colour name, when possible
    wxC2S_CSS_SYNTAX       = 2,   // return colour in rgb(r,g,b) syntax
    wxC2S_HTML_SYNTAX      = 4    // return colour in #rrggbb syntax
};

const unsigned char wxALPHA_TRANSPARENT = 0;
const unsigned char wxALPHA_OPAQUE = 0xff;

// a valid but fully transparent colour
#define wxTransparentColour wxColour(0, 0, 0, wxALPHA_TRANSPARENT)
#define wxTransparentColor wxTransparentColour

//-----------------------------------------------------------------------------
// wxColourBase: this class has no data members, just some functions to avoid
//               code redundancy in all native wxColour implementations
//-----------------------------------------------------------------------------

/*  Transition from wxGDIObject to wxObject is incomplete.  If your port does
    not need the wxGDIObject machinery to handle colors, please add it to the
    list of ports which do not need it.
 */
#if defined( __WXMSW__ ) || defined( __WXQT__ )
#define wxCOLOUR_IS_GDIOBJECT 0
#else
#define wxCOLOUR_IS_GDIOBJECT 1
#endif

class WXDLLIMPEXP_CORE wxColourBase : public
#if wxCOLOUR_IS_GDIOBJECT
    wxGDIObject
#else
    wxObject
#endif
{
public:
    // type of a single colour component
    typedef unsigned char ChannelType;

    wxColourBase() = default;
    virtual ~wxColourBase() = default;


    // Set() functions
    // ---------------

    void Set(ChannelType red,
             ChannelType green,
             ChannelType blue,
             ChannelType alpha = wxALPHA_OPAQUE)
        { InitRGBA(red, green, blue, alpha); }

    // implemented in colourcmn.cpp
    bool Set(const wxString &str)
        { return FromString(str); }

    void Set(unsigned long colRGB)
    {
        // we don't need to know sizeof(long) here because we assume that the three
        // least significant bytes contain the R, G and B values
        Set((ChannelType)(0xFF & colRGB),
            (ChannelType)(0xFF & (colRGB >> 8)),
            (ChannelType)(0xFF & (colRGB >> 16)));
    }



    // accessors
    // ---------

    virtual ChannelType Red() const = 0;
    virtual ChannelType Green() const = 0;
    virtual ChannelType Blue() const = 0;
    virtual ChannelType Alpha() const
        { return wxALPHA_OPAQUE ; }

    // These getters return the values as unsigned int, which avoids promoting
    // them to (signed) int in arithmetic expressions, unlike the ones above.
    unsigned int GetRed() const { return Red(); }
    unsigned int GetGreen() const { return Green(); }
    unsigned int GetBlue() const { return Blue(); }
    unsigned int GetAlpha() const { return Alpha(); }

    virtual bool IsSolid() const
        { return true; }

    bool IsTransparent() const
        { return GetAlpha() == wxALPHA_TRANSPARENT; }

    bool IsOpaque() const
        { return GetAlpha() == wxALPHA_OPAQUE; }

    bool IsTranslucent() const
        { return GetAlpha() > wxALPHA_TRANSPARENT && GetAlpha() < wxALPHA_OPAQUE; }

    // implemented in colourcmn.cpp
    virtual wxString GetAsString(long flags = wxC2S_NAME | wxC2S_CSS_SYNTAX) const;

    void SetRGB(wxUint32 colRGB)
    {
        Set((ChannelType)(0xFF & colRGB),
            (ChannelType)(0xFF & (colRGB >> 8)),
            (ChannelType)(0xFF & (colRGB >> 16)));
    }

    void SetRGBA(wxUint32 colRGBA)
    {
        Set((ChannelType)(0xFF & colRGBA),
            (ChannelType)(0xFF & (colRGBA >> 8)),
            (ChannelType)(0xFF & (colRGBA >> 16)),
            (ChannelType)(0xFF & (colRGBA >> 24)));
    }

    wxUint32 GetRGB() const
        { return GetRed() | (GetGreen() << 8) | (GetBlue() << 16); }

    wxUint32 GetRGBA() const
        { return GetRed() | (GetGreen() << 8) | (GetBlue() << 16) | (GetAlpha() << 24); }

#if !wxCOLOUR_IS_GDIOBJECT
    virtual bool IsOk() const= 0;

    // older version, for backwards compatibility only (but not deprecated
    // because it's still widely used)
    bool Ok() const { return IsOk(); }
#endif

    // Return the perceived brightness of the colour, with 0 for black and 1
    // for white.
    double GetLuminance() const;

    // manipulation
    // ------------

    // These methods are static because they are mostly used
    // within tight loops (where we don't want to instantiate wxColour's)

    static void          MakeMono    (unsigned char* r, unsigned char* g, unsigned char* b, bool on);
    static void          MakeDisabled(unsigned char* r, unsigned char* g, unsigned char* b, unsigned char brightness = 255);
    static void          MakeGrey    (unsigned char* r, unsigned char* g, unsigned char* b); // integer version
    static void          MakeGrey    (unsigned char* r, unsigned char* g, unsigned char* b,
                                      double weight_r, double weight_g, double weight_b); // floating point version
    static unsigned char AlphaBlend  (unsigned char fg, unsigned char bg, double alpha);
    static void          ChangeLightness(unsigned char* r, unsigned char* g, unsigned char* b, int ialpha);

    wxColour ChangeLightness(int ialpha) const;
    wxColour& MakeDisabled(unsigned char brightness = 255);

    wxDECLARE_VARIANT_OBJECT_EXPORTED(wxColour, WXDLLIMPEXP_CORE);

protected:
    virtual void
    InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a) = 0;

    virtual bool FromString(const wxString& s);

#if wxCOLOUR_IS_GDIOBJECT
    // wxColour doesn't use reference counted data (at least not in all ports)
    // so provide stubs for the functions which need to be defined if we do use
    // them
    virtual wxGDIRefData *CreateGDIRefData() const override
    {
        wxFAIL_MSG( "must be overridden if used" );

        return nullptr;
    }

    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *WXUNUSED(data)) const override
    {
        wxFAIL_MSG( "must be overridden if used" );

        return nullptr;
    }
#endif
};


// wxColour <-> wxString utilities, used by wxConfig, defined in colourcmn.cpp
WXDLLIMPEXP_CORE wxString wxToString(const wxColourBase& col);
WXDLLIMPEXP_CORE bool wxFromString(const wxString& str, wxColourBase* col);



#if defined(__WXMSW__)
    #include "wx/msw/colour.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/colour.h"
#elif defined(__WXDFB__)
    #include "wx/generic/colour.h"
#elif defined(__WXX11__)
    #include "wx/x11/colour.h"
#elif defined(__WXMAC__)
    #include "wx/osx/colour.h"
#elif defined(__WXQT__)
    #include "wx/qt/colour.h"
#endif

#define wxColor wxColour

// Actual wxColour class, inheriting from the port-specific implementation
// class and defining all the overloaded constructors.
class WXDLLIMPEXP_CORE wxWARN_UNUSED wxColour : public wxColourImpl
{
public:
    wxColour() = default;
    wxColour(ChannelType red,
             ChannelType green,
             ChannelType blue,
             ChannelType alpha = wxALPHA_OPAQUE)
        { Set(red, green, blue, alpha); }
    wxColour(unsigned long colRGB) { Set(colRGB); }
    wxColour(long colRGB) : wxColour(static_cast<unsigned long>(colRGB)) {}
    wxColour(unsigned int colRGB) : wxColour(static_cast<unsigned long>(colRGB)) {}
    wxColour(int colRGB) : wxColour(static_cast<unsigned long>(colRGB)) {}
    wxColour(const wxString& colourName) { Set(colourName); }
    wxColour(const wchar_t *colourName) : wxColour(wxString(colourName)) {}
#ifndef wxNO_IMPLICIT_WXSTRING_ENCODING
    wxColour(const char *colourName) : wxColour(wxString(colourName)) {}
#endif
    wxColour(bool) = delete;

    // Also inherit port-specific constructors from the base class, if any.
    using wxColourImpl::wxColourImpl;

    // And define copy constructor and assignment operator in this class too.
    wxColour(const wxColour& col) : wxColourImpl(col) {}
    wxColour& operator=(const wxColour& col)
    {
        wxColourImpl::operator=(col);
        return *this;
    }

private:
    wxDECLARE_DYNAMIC_CLASS(wxColour);
};

#endif // _WX_COLOUR_H_BASE_
