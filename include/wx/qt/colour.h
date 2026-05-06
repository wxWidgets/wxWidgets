/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colour.h
// Purpose:     wxColourImpl class for wxQt
// Author:      Kolya Kosenko
// Created:     2010-05-12
// Copyright:   (c) 2010 Kolya Kosenko
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COLOUR_H_
#define _WX_QT_COLOUR_H_

class QColor;

class WXDLLIMPEXP_CORE wxColourImpl : public wxColourBase
{
public:
    wxColourImpl() = default;
    wxColourImpl(const QColor& color);

    virtual bool IsOk() const override { return m_valid; }

    ChannelType Red()   const override { return m_red;   }
    ChannelType Green() const override { return m_green; }
    ChannelType Blue()  const override { return m_blue;  }
    ChannelType Alpha() const override { return m_alpha; }

    bool operator==(const wxColourImpl& color) const;
    bool operator!=(const wxColourImpl& color) const;

    int GetPixel() const;

    QColor GetQColor() const;

protected:
    virtual void InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a) override;

private:
    ChannelType m_red = 0,
                m_green = 0,
                m_blue = 0,
                m_alpha = wxALPHA_TRANSPARENT;
    bool m_valid = false;
};

#endif // _WX_QT_COLOUR_H_
