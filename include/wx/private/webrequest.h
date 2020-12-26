///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/webrequest.h
// Purpose:     wxWebRequest implementation classes
// Author:      Vadim Zeitlin
// Created:     2020-12-26
// Copyright:   (c) 2020 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WEBREQUEST_H_
#define _WX_PRIVATE_WEBREQUEST_H_

// ----------------------------------------------------------------------------
// wxWebSessionFactory
// ----------------------------------------------------------------------------

class wxWebSessionFactory
{
public:
    virtual wxWebSession* Create() = 0;

    virtual ~wxWebSessionFactory() { }
};

#endif // _WX_PRIVATE_WEBREQUEST_H_
