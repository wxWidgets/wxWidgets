/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/colour.h
// Author:      Peter Most
// RCS-ID:      $Id$
// Copyright:   Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_COLOUR_H_
#define _WX_QT_COLOUR_H_

class WXDLLIMPEXP_CORE wxColour : public wxColourBase
{
public:
    DEFINE_STD_WXCOLOUR_CONSTRUCTORS
    
    virtual ChannelType Red() const;
    virtual ChannelType Green() const;
    virtual ChannelType Blue() const;

    bool operator != ( const wxColour & ) const;
    bool operator == ( const wxColour & ) const;
    
protected:
    // void Init();
    virtual void InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a);
    
private:
};

#endif // _WX_QT_COLOUR_H_
