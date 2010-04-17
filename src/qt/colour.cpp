/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/colour.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/colour.h"

wxColour::ChannelType wxColour::Red() const
{
    return ChannelType();
}

wxColour::ChannelType wxColour::Green() const
{
    return ChannelType();
}

wxColour::ChannelType wxColour::Blue() const
{
    return ChannelType();
}


bool wxColour::operator != ( const wxColour & ) const
{
    return false;
}

bool wxColour::operator == ( const wxColour & ) const
{
    return false;
}

void wxColour::InitRGBA(ChannelType r, ChannelType g, ChannelType b, ChannelType a)
{
}


