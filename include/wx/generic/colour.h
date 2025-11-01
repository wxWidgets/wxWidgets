/////////////////////////////////////////////////////////////////////////////
// Name:        wx/generic/colour.h
// Purpose:     wxColourImpl class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GENERIC_COLOUR_H_
#define _WX_GENERIC_COLOUR_H_

#include "wx/object.h"

// Colour
class WXDLLIMPEXP_CORE wxColourImpl: public wxColourBase
{
public:
    // constructors
    // ------------
    wxColourImpl() = default;

    // copy ctors and assignment operators
    wxColourImpl(const wxColourImpl& col)
    {
        *this = col;
    }

    wxColourImpl& operator=(const wxColourImpl& col);

    // accessors
    virtual bool IsOk() const { return m_isInit; }

    unsigned char Red() const { return m_red; }
    unsigned char Green() const { return m_green; }
    unsigned char Blue() const { return m_blue; }
    unsigned char Alpha() const { return m_alpha; }

    // comparison
    bool operator==(const wxColourImpl& colour) const
    {
        return (m_red == colour.m_red &&
                m_green == colour.m_green &&
                m_blue == colour.m_blue &&
                m_alpha == colour.m_alpha &&
                m_isInit == colour.m_isInit);
    }

    bool operator!=(const wxColourImpl& colour) const { return !(*this == colour); }

protected:
    virtual void
    InitRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

private:
    bool m_isInit = false;
    unsigned char m_red = 0;
    unsigned char m_blue = 0;
    unsigned char m_green = 0;
    unsigned char m_alpha = wxALPHA_OPAQUE;
};

#endif // _WX_GENERIC_COLOUR_H_
