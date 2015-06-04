/////////////////////////////////////////////////////////////////////////////
// Name:        src/msdos/mimetype.cpp
// Purpose:     classes and functions to manage MIME types
// Author:      Vadim Zeitlin
// Modified by:
// Created:     23.09.98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence (part of wxExtra library)
/////////////////////////////////////////////////////////////////////////////

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_MIMETYPE

#include "wx/msdos/mimetype.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #if wxUSE_GUI
        #include "wx/icon.h"
    #endif
#endif //WX_PRECOMP

#include "wx/file.h"
#include "wx/confbase.h"

// other standard headers
#include <ctype.h>

// in case we're compiling in non-GUI mode
class WXDLLEXPORT wxIcon;

bool wxFileTypeImpl::SetCommand(const wxString& WXUNUSED(cmd),
                                const wxString& WXUNUSED(verb),
                                bool WXUNUSED(overwriteprompt))
{
    return false;
}

bool wxFileTypeImpl::SetDefaultIcon(const wxString& WXUNUSED(strIcon),
                                    int WXUNUSED(index))
{
    return false;
}

bool wxFileTypeImpl::GetCommand(wxString *WXUNUSED(command),
                                const char *WXUNUSED(verb)) const
{
    return false;
}

bool wxFileTypeImpl::GetExtensions(wxArrayString& WXUNUSED(extensions))
{
    return false;
}

bool wxFileTypeImpl::GetMimeType(wxString *mimeType) const
{
    if ( !m_strFileType.empty() )
    {
        *mimeType = m_strFileType ;
        return true ;
    }
    else
    return false;
}

bool wxFileTypeImpl::GetMimeTypes(wxArrayString& mimeTypes) const
{
    wxString s;

    if (GetMimeType(&s))
    {
        mimeTypes.Clear();
        mimeTypes.Add(s);
        return true;
    }
    else
        return false;
}

bool wxFileTypeImpl::GetIcon(wxIconLocation *WXUNUSED(icon)) const
{
    // no such file type or no value or incorrect icon entry
    return false;
}

bool wxFileTypeImpl::GetDescription(wxString *WXUNUSED(desc)) const
{
    return false;
}

size_t
wxFileTypeImpl::GetAllCommands(wxArrayString * WXUNUSED(verbs),
                               wxArrayString * WXUNUSED(commands),
                               const wxFileType::MessageParameters& WXUNUSED(params)) const
{
    wxFAIL_MSG( wxT("wxFileTypeImpl::GetAllCommands() not yet implemented") );
    return 0;
}

void
wxMimeTypesManagerImpl::Initialize(int WXUNUSED(mailcapStyles),
                                   const wxString& WXUNUSED(extraDir))
{
    wxFAIL_MSG( wxT("wxMimeTypesManagerImpl::Initialize() not yet implemented") );
}

void
wxMimeTypesManagerImpl::ClearData()
{
    wxFAIL_MSG( wxT("wxMimeTypesManagerImpl::ClearData() not yet implemented") );
}

// extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& e)
{
    wxString ext = e, str_type;
    ext = ext.Lower();
    if ( ext == wxT("txt") )
        str_type = wxT("text/text");
    else if ( ext == wxT("htm") || ext == wxT("html") )
        str_type = wxT("text/html");
    else if ( ext == wxT("gif") )
        str_type = wxT("image/gif");
    else if ( ext == wxT("png" ))
        str_type = wxT("image/png");
    else if ( ext == wxT("jpg" ) || ext == wxT("jpeg") )
        str_type = wxT("image/jpeg");
    else if ( ext == wxT("bmp") )
        str_type = wxT("image/bmp");
    else if ( ext == wxT("tif") || ext == wxT("tiff") )
        str_type = wxT("image/tiff");
    else if ( ext == wxT("xpm") )
        str_type = wxT("image/xpm");
    else if ( ext == wxT("xbm") )
        str_type = wxT("image/xbm");
    
    if(!str_type.empty())
    {
        wxFileType *fileType = new wxFileType;
        fileType->m_impl->SetFileType(str_type);
        fileType->m_impl->SetExt(ext);
        return fileType;
    }
    else // unknown extension
        return NULL;
}

// MIME type -> extension -> file type
wxFileType *
wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& WXUNUSED(mimeType))
{
    return NULL;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxArrayString& WXUNUSED(mimetypes))
{
    // VZ: don't know anything about this for Mac
    wxFAIL_MSG( wxT("wxMimeTypesManagerImpl::EnumAllFileTypes() not yet implemented") );

    return 0;
}

wxFileType *
wxMimeTypesManagerImpl::Associate(const wxFileTypeInfo& WXUNUSED(ftInfo))
{
    wxFAIL_MSG( wxT("wxMimeTypesManagerImpl::Associate() not yet implemented") );

    return NULL;
}

bool
wxMimeTypesManagerImpl::Unassociate(wxFileType *WXUNUSED(ft))
{
    return false;
}

#endif // wxUSE_MIMETYPE
