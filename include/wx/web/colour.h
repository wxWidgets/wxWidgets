#ifndef _WX_COLOUR_H_
#define _WX_COLOUR_H_

#include "wx/colour.h"

class WXDLLEXPORT wxColour: public wxColourBase {
public:
    wxColour();
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS

    wxColour(const wxColour& col);
    wxColour& operator = (const wxColour& col);
    ~wxColour();

    bool operator == (const wxColour& colour) const;
    bool operator != (const wxColour& colour) const;

    unsigned char Alpha() const { return m_alpha; }
    unsigned char Red() const { return m_red; }
    unsigned char Green() const { return m_green; }
    unsigned char Blue() const { return m_blue; }
    bool IsOk() const;

protected:
    virtual void InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a);
private:
    unsigned char m_alpha;
    unsigned char m_red;
    unsigned char m_blue;
    unsigned char m_green;

    DECLARE_DYNAMIC_CLASS(wxColour)
};

#endif // _WX_COLOUR_H_
