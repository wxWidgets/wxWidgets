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

class QColor;

class WXDLLIMPEXP_CORE wxColour : public wxColourBase
{
public:
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS
    wxColour(const QColor& color);

    virtual bool IsOk() const;

    unsigned char Red() const;
    unsigned char Green() const;
    unsigned char Blue()  const;
    unsigned char Alpha() const;

    bool operator==(const wxColour& color) const;
    bool operator!=(const wxColour& color) const;

    int GetPixel() const;

    QColor GetQColor() const;

protected:
    void Init();
    virtual void InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a);

private:
    unsigned char red, green, blue, alpha;
    bool valid;

    wxDECLARE_DYNAMIC_CLASS(wxColour);
};

#endif // _WX_QT_COLOUR_H_
