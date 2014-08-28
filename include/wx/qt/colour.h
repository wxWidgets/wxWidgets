/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colour.h
// Purpose:     wxColour class implementation for wxQt
// Author:      Peter Most, Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (C) 2010 Kolya Kosenko
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

    virtual ChannelType Red()   const { return m_qtColor.red();   }
    virtual ChannelType Green() const { return m_qtColor.green(); }
    virtual ChannelType Blue()  const { return m_qtColor.blue();  }
    virtual ChannelType Alpha() const { return m_qtColor.alpha(); }

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

    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif // _WX_QT_COLOUR_H_
