/////////////////////////////////////////////////////////////////////////////
// Name:        mac/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

#ifdef    __GNUG__
#pragma implementation "mimetype.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/defs.h"
#endif

#ifndef WX_PRECOMP
  #include "wx/string.h"
  #if wxUSE_GUI
    #include "wx/icon.h"
  #endif
#endif //WX_PRECOMP


#include "wx/log.h"
#include "wx/file.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/confbase.h"

#include "wx/mac/mimetype.h"

// other standard headers
#include <ctype.h>

// in case we're compiling in non-GUI mode
class WXDLLEXPORT wxIcon;






bool wxFileTypeImpl::GetCommand(wxString *command, const char *verb) const
{
    return FALSE;
}

// @@ this function is half implemented
bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    return FALSE;
}

bool wxFileTypeImpl::GetMimeType(wxString *mimeType) const
{
    if ( m_strFileType.Length() > 0 )
    {
        *mimeType = m_strFileType ;
        return TRUE ;
    }
    else
    return FALSE;
}

bool wxFileTypeImpl::GetMimeTypes(wxArrayString& mimeTypes) const
{
    wxString s;
    
    if (GetMimeType(&s))
    {
        mimeTypes.Clear();
        mimeTypes.Add(s);
        return TRUE;
    }
    else 
        return FALSE;
}

bool wxFileTypeImpl::GetIcon(wxIcon *icon) const
{
    // no such file type or no value or incorrect icon entry
    return FALSE;
}

bool wxFileTypeImpl::GetDescription(wxString *desc) const
{
    return FALSE;
}

// extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& e)
{
    wxString ext = e ;
    ext = ext.Lower() ;
    if ( ext == "txt" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("text/text");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "htm" || ext == "html" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("text/html");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "gif" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/gif");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "png" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/png");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "jpg" || ext == "jpeg" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/jpeg");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "bmp" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/bmp");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "tif" || ext == "tiff" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/tiff");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "xpm" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/xpm");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else if ( ext == "xbm" )
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType("image/xbm");
        fileType->m_impl->SetExt(ext);
        return fileType;
    }

    // unknown extension
    return NULL;
}

// MIME type -> extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    return NULL;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxArrayString& mimetypes)
{
    wxFAIL_MSG( _T("TODO") ); // VZ: don't know anything about this for Mac

    return 0;
}

