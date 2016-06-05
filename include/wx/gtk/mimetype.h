/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/mimetype.h
// Purpose:     classes and functions to manage MIME types
// Author:      Hans Mackowiak
// Modified by:
// Created:     05.06.2016
// Copyright:   (c) 2016 Hans Mackowiak <hanmac@gmx.de>
// Licence:     wxWindows licence (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

#ifndef _GTK_MIMETYPE_IMPL_H
#define _GTK_MIMETYPE_IMPL_H

#include "wx/unix/mimetype.h"

#if wxUSE_MIMETYPE

class WXDLLIMPEXP_CORE wxGTKMimeTypesManagerImpl : public wxMimeTypesManagerImpl
{
protected:
    wxString GetIconFromMimeType(const wxString& mime) wxOVERRIDE;
};


class WXDLLIMPEXP_CORE wxGTKMimeTypesManagerFactory : public wxMimeTypesManagerFactory
{
public:
    wxGTKMimeTypesManagerFactory() {}
    virtual ~wxGTKMimeTypesManagerFactory() {}
    wxMimeTypesManagerImpl *CreateMimeTypesManagerImpl() wxOVERRIDE;
};

#endif // wxUSE_MIMETYPE

#endif // _GTK_MIMETYPE_IMPL_H


