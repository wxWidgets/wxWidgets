/////////////////////////////////////////////////////////////////////////////
// Name:        filesys.cpp
// Purpose:     wxFileSystem class - interface for opening files
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if !wxUSE_SOCKETS
    #undef wxUSE_FS_INET
    #define wxUSE_FS_INET 0
#endif

#if (wxUSE_HTML || wxUSE_FS_INET || wxUSE_FS_ZIP) && wxUSE_STREAMS

#include "wx/wfstream.h"
#include "wx/module.h"
#include "wx/filesys.h"

//--------------------------------------------------------------------------------
// wxFileSystemHandler
//--------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxFileSystemHandler, wxObject)

wxMimeTypesManager *wxFileSystemHandler::m_MimeMng = NULL;

void wxFileSystemHandler::CleanUpStatics()
{
    if (m_MimeMng) delete m_MimeMng;
    m_MimeMng = NULL;
}


wxString wxFileSystemHandler::GetMimeTypeFromExt(const wxString& location)
{
    wxString ext = wxEmptyString, mime = wxEmptyString;
    wxString loc = GetRightLocation(location);
    char c;
    int l = loc.Length(), l2;
    wxFileType *ft;

    l2 = l;
    for (int i = l-1; i >= 0; i--) {
        c = loc[(unsigned int) i];
        if (c == wxT('#')) l2 = i + 1;
        if (c == wxT('.')) {ext = loc.Right(l2-i-1); break;}
        if ((c == wxT('/')) || (c == wxT('\\')) || (c == wxT(':'))) {return wxEmptyString;}
    }

    if (m_MimeMng == NULL) {
        m_MimeMng = new wxMimeTypesManager;

        static const wxFileTypeInfo fallbacks[] =
        {
            wxFileTypeInfo("image/jpeg",
                           "",
                           "",
                           "JPEG image (from fallback)",
                           "jpg", "jpeg", NULL),
            wxFileTypeInfo("image/gif",
                           "",
                           "",
                           "GIF image (from fallback)",
                           "gif", NULL),
            wxFileTypeInfo("image/png",
                           "",
                           "",
                           "PNG image (from fallback)",
                           "png", NULL),
            wxFileTypeInfo("image/bmp",
                           "",
                           "",
                           "windows bitmap image (from fallback)",
                           "bmp", NULL),
            wxFileTypeInfo("text/html",
                           "",
                           "",
                           "HTML document (from fallback)",
                           "htm", "html", NULL),

            // must terminate the table with this!
            wxFileTypeInfo()
        };

        m_MimeMng -> AddFallbacks(fallbacks);
    }

    ft = m_MimeMng -> GetFileTypeFromExtension(ext);
    if (ft && (ft -> GetMimeType(&mime))) {
        delete ft; 
        return mime;
    }
    else {
        delete ft;
        return wxEmptyString;
    }
}



wxString wxFileSystemHandler::GetProtocol(const wxString& location) const
{
    wxString s = wxEmptyString;
    int i, l = location.Length();
    bool fnd;

    fnd = FALSE;
    for (i = l-1; (i >= 0) && ((location[i] != wxT('#')) || (!fnd)); i--) {
        if ((location[i] == wxT(':')) && (i != 1 /*win: C:\path*/)) fnd = TRUE;
    }
    if (!fnd) return wxT("file");
    for (++i; (i < l) && (location[i] != wxT(':')); i++) s << location[i];
    return s;
}


wxString wxFileSystemHandler::GetLeftLocation(const wxString& location) const
{
    int i;
    bool fnd;

    fnd = FALSE;
    for (i = location.Length()-1; i >= 0; i--) {
        if ((location[i] == wxT(':')) && (i != 1 /*win: C:\path*/)) fnd = TRUE;
        else if (fnd && (location[i] == wxT('#'))) return location.Left(i);
    }
    return wxEmptyString;
}

wxString wxFileSystemHandler::GetRightLocation(const wxString& location) const
{
    int i, l = location.Length();
    int l2 = l + 1;
    for (i = l-1; (i >= 0) && ((location[i] != wxT(':')) || (i == 1) || (location[i-2] == wxT(':'))); i--) {if (location[i] == wxT('#')) l2 = i + 1;}
    if (i == 0) return wxEmptyString;
    else return location.Mid(i + 1, l2 - i - 2);
}

wxString wxFileSystemHandler::GetAnchor(const wxString& location) const
{
    char c;
    int l = location.Length();

    for (int i = l-1; i >= 0; i--) {
        c = location[i];
        if (c == wxT('#')) return location.Right(l-i-1);
        else if ((c == wxT('.')) || (c == wxT('/')) || (c == wxT('\\')) || (c == wxT(':'))) return wxEmptyString;
    }
    return wxEmptyString;
}


wxString wxFileSystemHandler::FindFirst(const wxString& WXUNUSED(spec),
                                        int WXUNUSED(flags))
{
    return wxEmptyString;
}

wxString wxFileSystemHandler::FindNext()
{
    return wxEmptyString;
}

//--------------------------------------------------------------------------------
// wxLocalFSHandler
//--------------------------------------------------------------------------------

class wxLocalFSHandler : public wxFileSystemHandler
{
    public:
        virtual bool CanOpen(const wxString& location);
        virtual wxFSFile* OpenFile(wxFileSystem& fs, const wxString& location);
        virtual wxString FindFirst(const wxString& spec, int flags = 0);
        virtual wxString FindNext();
};


bool wxLocalFSHandler::CanOpen(const wxString& location)
{
    return GetProtocol(location) == wxT("file");
}

wxFSFile* wxLocalFSHandler::OpenFile(wxFileSystem& WXUNUSED(fs), const wxString& location)
{
    wxString right = GetRightLocation(location);
    if (wxFileExists(right))
        return new wxFSFile(new wxFileInputStream(right),
                            right,
                            GetMimeTypeFromExt(location),
                            GetAnchor(location),
                            wxDateTime(wxFileModificationTime(right)));
    else return (wxFSFile*) NULL;
}

wxString wxLocalFSHandler::FindFirst(const wxString& spec, int flags)
{
    wxString right = GetRightLocation(spec);
    return wxFindFirstFile(right, flags);
}

wxString wxLocalFSHandler::FindNext()
{
    return wxFindNextFile();
}



//-----------------------------------------------------------------------------
// wxFileSystem
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFileSystem, wxObject)


wxList wxFileSystem::m_Handlers;



void wxFileSystem::ChangePathTo(const wxString& location, bool is_dir)
{
    int i, pathpos = -1;
    m_Path = location;

    for (i = m_Path.Length()-1; i >= 0; i--)
        if (m_Path[(unsigned int) i] == wxT('\\')) m_Path.GetWritableChar(i) = wxT('/');         // wanna be windows-safe

    if (is_dir)
    {
        if (m_Path.Length() > 0 && m_Path.Last() != wxT('/') && m_Path.Last() != wxT(':'))
	        m_Path << wxT('/');
    }
    
    else
    {
        for (i = m_Path.Length()-1; i >= 0; i--)
    	{
            if (m_Path[(unsigned int) i] == wxT('/'))
	        {
                if ((i > 1) && (m_Path[(unsigned int) (i-1)] == wxT('/')) && (m_Path[(unsigned int) (i-2)] == wxT(':')))
		        {
                    i -= 2;
                    continue;
                }
                else
        		{
                    pathpos = i;
                    break;
                }
            }
            else if (m_Path[(unsigned int) i] == wxT(':')) {
                pathpos = i;
                break;
            }
        }
        if (pathpos == -1)
    	{
            for (i = 0; i < (int) m_Path.Length(); i++)
    	    {
                if (m_Path[(unsigned int) i] == wxT(':'))
        		{
                    m_Path.Remove(i+1);
                    break;
                }
            }
            if (i == (int) m_Path.Length())
    	        m_Path = wxEmptyString;
        }
        else
    	{
            m_Path.Remove(pathpos+1);
        }
    }
}



wxFSFile* wxFileSystem::OpenFile(const wxString& location)
{
    wxString loc = location;
    int i, ln;
    char meta;
    wxFSFile *s = NULL;
    wxNode *node;

    ln = loc.Length();
    meta = 0;
    for (i = 0; i < ln; i++)
    {
        if (loc[(unsigned int) i] == wxT('\\')) loc.GetWritableChar(i) = wxT('/');         // wanna be windows-safe
        if (!meta) 
            switch (loc[(unsigned int) i])
        	{
                case wxT('/') : case wxT(':') : case wxT('#') : meta = loc[(unsigned int) i];
            }
    }
    m_LastName = wxEmptyString;

    // try relative paths first :
    if (meta != wxT(':'))
    {
        node = m_Handlers.GetFirst();
        while (node)
    	{
            wxFileSystemHandler *h = (wxFileSystemHandler*) node -> GetData();
            if (h->CanOpen(m_Path + loc))
	        {
                s = h->OpenFile(*this, m_Path + loc);
                if (s) { m_LastName = m_Path + loc; break; }
            }
            node = node->GetNext();
        }
    }

    // if failed, try absolute paths :
    if (s == NULL)
    {
        node = m_Handlers.GetFirst();
        while (node)
    	{
            wxFileSystemHandler *h = (wxFileSystemHandler*) node->GetData();
            if (h->CanOpen(loc))
    	    {
                s = h->OpenFile(*this, loc);
                if (s) { m_LastName = loc; break; }
            }
            node = node->GetNext();
        }
    }
    return (s);
}



wxString wxFileSystem::FindFirst(const wxString& spec, int flags)
{
    wxNode *node;
    wxString spec2(spec);
    
    m_FindFileHandler = NULL;

    for (int i = spec2.Length()-1; i >= 0; i--)
        if (spec2[(unsigned int) i] == wxT('\\')) spec2.GetWritableChar(i) = wxT('/'); // wanna be windows-safe

    node = m_Handlers.GetFirst();
    while (node)
    {
        m_FindFileHandler = (wxFileSystemHandler*) node -> GetData();
        if (m_FindFileHandler -> CanOpen(m_Path + spec2)) 
            return m_FindFileHandler -> FindFirst(m_Path + spec2, flags);
        node = node->GetNext();
    } 

    node = m_Handlers.GetFirst();
    while (node)
    {
        m_FindFileHandler = (wxFileSystemHandler*) node -> GetData();
        if (m_FindFileHandler -> CanOpen(spec2))
            return m_FindFileHandler -> FindFirst(spec2, flags);
        node = node->GetNext();
    } 
    
    return wxEmptyString;   
}



wxString wxFileSystem::FindNext()
{
    if (m_FindFileHandler == NULL) return wxEmptyString;
    else return m_FindFileHandler -> FindNext();
}



void wxFileSystem::AddHandler(wxFileSystemHandler *handler)
{
    m_Handlers.Append(handler);
}


void wxFileSystem::CleanUpHandlers()
{
    m_Handlers.DeleteContents(TRUE);
    m_Handlers.Clear();
}




///// Module:

class wxFileSystemModule : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxFileSystemModule)

    public:
        virtual bool OnInit()
        {
            wxFileSystem::AddHandler(new wxLocalFSHandler);
            return TRUE;
        }
        virtual void OnExit()
    	{
    	    wxFileSystemHandler::CleanUpStatics();
            wxFileSystem::CleanUpHandlers();
    	}
};

IMPLEMENT_DYNAMIC_CLASS(wxFileSystemModule, wxModule)

#endif
  // (wxUSE_FS_INET || wxUSE_FS_ZIP) && wxUSE_STREAMS



