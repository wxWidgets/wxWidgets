/////////////////////////////////////////////////////////////////////////////
// Name:        helpbest.h
// Purpose:     Tries to load MS HTML Help, falls back to wxHTML upon failure
// Author:      Mattia Barbon
// Modified by:
// Created:     02/04/2001
// RCS-ID:      $Id$
// Copyright:   (c) Mattia Barbon
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPBEST_H_
#define _WX_HELPBEST_H_

#ifdef __GNUG__
#pragma interface "helpbest.h"
#endif

#include "wx/wx.h"

#if wxUSE_HELP && wxUSE_MS_HTML_HELP && defined(__WIN95__) && wxUSE_WXHTML_HELP

#include "wx/helpbase.h"

class WXDLLEXPORT wxBestHelpController: public wxHelpControllerBase
{
    DECLARE_DYNAMIC_CLASS(wxBestHelpController)
        
public:
    wxBestHelpController():m_helpControllerType( wxUseNone ),
        m_helpController( 0 ) {}
    ~wxBestHelpController() { delete m_helpController; }
    
    // Must call this to set the filename
    virtual bool Initialize(const wxString& file);
    
    // If file is "", reloads file given in Initialize
    virtual bool LoadFile(const wxString& file = wxEmptyString)
    {
        wxASSERT( m_helpController );
        return m_helpController->LoadFile( GetValidFilename( file ) );
    }

    virtual bool DisplayContents()
    {
        wxASSERT( m_helpController );
        return m_helpController->DisplayContents();
    }

    virtual bool DisplaySection(int sectionNo)
    {
        wxASSERT( m_helpController );
        return m_helpController->DisplaySection( sectionNo );
    }

    virtual bool DisplaySection(const wxString& section)
    {
        wxASSERT( m_helpController );
        return m_helpController->DisplaySection( section );
    }

    virtual bool DisplayBlock(long blockNo)
    {
        wxASSERT( m_helpController );
        return m_helpController->DisplayBlock( blockNo );
    }

    virtual bool DisplayContextPopup(int contextId)
    {
        wxASSERT( m_helpController );
        return m_helpController->DisplayContextPopup( contextId );
    }

    virtual bool DisplayTextPopup(const wxString& text, const wxPoint& pos)
    {
        wxASSERT( m_helpController );
        return m_helpController->DisplayTextPopup( text, pos );
    }

    virtual bool KeywordSearch(const wxString& k)
    {
        wxASSERT( m_helpController );
        return m_helpController->KeywordSearch( k );
    }

    virtual bool Quit()
    {
        wxASSERT( m_helpController );
        return m_helpController->Quit();
    }

    /// Allows one to override the default settings for the help frame.
    virtual void SetFrameParameters(const wxString& title,
        const wxSize& size,
        const wxPoint& pos = wxDefaultPosition,
        bool newFrameEachTime = FALSE)
    {
        wxASSERT( m_helpController );
        m_helpController->SetFrameParameters( title, size, pos,
                                              newFrameEachTime );
    }

    /// Obtains the latest settings used by the help frame and the help 
    /// frame.
    virtual wxFrame *GetFrameParameters(wxSize *size = NULL,
        wxPoint *pos = NULL,
        bool *newFrameEachTime = NULL)
    {
        wxASSERT( m_helpController );
        return m_helpController->GetFrameParameters( size, pos,
                                                     newFrameEachTime );
    }

protected:
    // Append/change extension if necessary.
    wxString GetValidFilename(const wxString& file) const;
    
protected:
    enum HelpControllerType { wxUseNone, wxUseHtmlHelp, wxUseChmHelp };

    HelpControllerType m_helpControllerType;
    wxHelpControllerBase* m_helpController;
};

#endif // wxUSE_HELP && wxUSE_MS_HTML_HELP && defined(__WIN95__) && wxUSE_WXHTML_HELP

#endif
    // _WX_HELPBEST_H_
