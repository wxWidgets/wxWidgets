/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/colour.h
// Purpose:     wxColour class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#include "wx/object.h"

// ----------------------------------------------------------------------------
// wxColour
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxColour : public wxColourBase
{
public:
    // constructors
    // ------------

    wxColour() { Init(); }
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS

    // copy ctors and assignment operators
    wxColour(const wxColour& col);
    wxColour& operator=( const wxColour& col);

    // dtor
    ~wxColour();



    bool Ok() const { return m_isInit; }

    unsigned char Red() const { return m_red; }
    unsigned char Green() const { return m_green; }
    unsigned char Blue() const { return m_blue; }

    // comparison
    bool operator==(const wxColour& colour) const
    {
        return m_isInit == colour.m_isInit
            && m_red == colour.m_red
            && m_green == colour.m_green
            && m_blue == colour.m_blue;
    }

    bool operator != (const wxColour& colour) const { return !(*this == colour); }

    WXCOLORREF GetPixel() const { return m_pixel; };

public:
    WXCOLORREF m_pixel;

protected:
    // Helper function
    void Init();

    virtual void InitWith(unsigned char red, unsigned char green, unsigned char blue);

private:
    bool          m_isInit;
    unsigned char m_red;
    unsigned char m_blue;
    unsigned char m_green;

private:
    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif
        // _WX_COLOUR_H_
