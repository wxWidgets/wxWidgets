/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colour.h
// Purpose:     wxColour class implementation for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COLOUR_H_
#define _WX_QT_COLOUR_H_

#include <QtGui/QColor>

class WXDLLIMPEXP_CORE wxColour : public wxColourBase
{
public:
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS
    wxColour(const QColor& color) : m_qtColor(color) {}

    virtual bool IsOk() const { return m_qtColor.isValid(); }

    unsigned char Red() const { return m_qtColor.red();   }
    unsigned char Green() const { return m_qtColor.green(); }
    unsigned char Blue()  const { return m_qtColor.blue();  }
    unsigned char Alpha() const { return m_qtColor.alpha(); }

    bool operator==(const wxColour& color) const
        { return m_qtColor == color.m_qtColor; }
    bool operator!=(const wxColour& color) const
        { return m_qtColor != color.m_qtColor; }

    int GetPixel() const;

    QColor GetHandle() const { return m_qtColor; };

protected:
    virtual void
    InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
        { m_qtColor.setRgb(r, g, b, a); }

private:
    QColor m_qtColor;

    wxDECLARE_DYNAMIC_CLASS(wxColour);
};

#endif // _WX_QT_COLOUR_H_
