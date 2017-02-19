/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/mimetype.h
// Purpose:     classes and functions to manage MIME types
// Author:      Hans Mackowiak
// Created:     2016-06-05
// Copyright:   (c) 2016 Hans Mackowiak <hanmac@gmx.de>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_MIMETYPE_IMPL_H
#define _WX_GTK_MIMETYPE_IMPL_H

#include "wx/defs.h"

#if defined(__UNIX__)
#include "wx/unix/mimetype.h"
#elif defined(__WINDOWS__)
#include "wx/msw/mimetype.h"
#endif

#if wxUSE_MIMETYPE && defined(__UNIX__)

class WXDLLIMPEXP_CORE wxGTKMimeTypesManagerImpl : public wxMimeTypesManagerImpl
{
protected:
    wxString GetIconFromMimeType(const wxString& mime) wxOVERRIDE;
};


class WXDLLIMPEXP_CORE wxGTKMimeTypesManagerFactory : public wxMimeTypesManagerFactory
{
public:
    wxMimeTypesManagerImpl *CreateMimeTypesManagerImpl() wxOVERRIDE;
};

#endif // wxUSE_MIMETYPE && defined(__UNIX__)

#endif // _WX_GTK_MIMETYPE_IMPL_H
