///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/threadinfo.cpp
// Purpose:     declaration of wxThreadSpecificInfo: thread-specific information
// Author:      Vaclav Slavik
// Created:     2013-09-14
// Copyright:   (c) 2013 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_THREADS

// The threaded version for wxThreadSpecificInfo::Get() is defined in platform specific
// thread files: src/msw/thread.cpp or src/unix/threadpsx.cpp

#else

#include "wx/private/threadinfo.h"

wxThreadSpecificInfo& wxThreadSpecificInfo::Get()
{
    static wxThreadSpecificInfo s_instance;
    return s_instance;
}

#endif // wxUSE_THREADS/wxUSE_THREADS
