/////////////////////////////////////////////////////////////////////////////
// Name:        filedlg.h
// Purpose:     wxFileDialog class
// Author:      David Webster
// Modified by:
// Created:     10/05/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FILEDLG_H_
#define _WX_FILEDLG_H_

#include "wx/dialog.h"

/*
 * File selector
 */

class WXDLLEXPORT wxFileDialog: public wxDialog
{
DECLARE_DYNAMIC_CLASS(wxFileDialog)
public:
    wxFileDialog( wxWindow*       pParent
                 ,const wxString& rsMessage = wxFileSelectorPromptStr
                 ,const wxString& rsDefaultDir = ""
                 ,const wxString& rsDefaultFile = ""
                 ,const wxString& rsWildCard = wxFileSelectorDefaultWildcardStr
                 ,long            lStyle = 0
                 ,const wxPoint&  rPos = wxDefaultPosition
                );

    inline void SetMessage(const wxString& rsMessage) { m_sMessage = rsMessage; }
    inline void SetPath(const wxString& rsPath) { m_sPath = rsPath; }
    inline void SetDirectory(const wxString& rsDir) { m_sDir = rsDir; }
    inline void SetFilename(const wxString& rsName) { m_sFileName = rsName; }
    inline void SetWildcard(const wxString& rsWildCard) { m_sWildCard = rsWildCard; }
    inline void SetStyle(long lStyle) { m_lDialogStyle = lStyle; }
    inline void SetFilterIndex(int nFilterIndex) { m_nFilterIndex = nFilterIndex; }

    inline wxString GetMessage(void) const { return m_sMessage; }
    inline wxString GetPath(void) const { return m_sPath; }
           void     GetPaths(wxArrayString& rasPath) const;
    inline wxString GetDirectory(void) const { return m_sDir; }
    inline wxString GetFilename(void) const { return m_sFileName; }
    inline void     GetFilenames(wxArrayString& rasFilenames) { rasFilenames = m_asFileNames; }
    inline wxString GetWildcard(void) const { return m_sWildCard; }
    inline long     GetStyle(void) const { return m_lDialogStyle; }
    inline int      GetFilterIndex() const { return m_nFilterIndex ; }

    int ShowModal();

protected:
    wxString                        m_sMessage;
    long                            m_lDialogStyle;
    wxWindow*                       m_pParent;
    wxString                        m_sDir;
    wxString                        m_sPath; // Full path
    wxString                        m_sFileName;
    wxArrayString                   m_asFileNames;
    wxString                        m_sWildCard;
    int                             m_nFilterIndex;
    wxPoint                         m_vPos;
}; // end of CLASS wxFileDialog

#define wxOPEN              0x0001
#define wxSAVE              0x0002
#define wxOVERWRITE_PROMPT  0x0004
#define wxHIDE_READONLY     0x0008
#define wxFILE_MUST_EXIST   0x0010

//
// File selector - backward compatibility
//
WXDLLEXPORT wxString wxFileSelector( const char* pzMessage = wxFileSelectorPromptStr
                                    ,const char* pzDefaultPath = NULL
                                    ,const char* pzDefaultFilename = NULL
                                    ,const char* pzDefaultExtension = NULL
                                    ,const char* pzWildcard = wxFileSelectorDefaultWildcardStr
                                    ,int         nFlags = 0
                                    ,wxWindow*   pParent = NULL
                                    ,int         nX = -1
                                    ,int         nY = -1
                                   );

//
// An extended version of wxFileSelector

WXDLLEXPORT wxString wxFileSelectorEx( const char* pzMessage = wxFileSelectorPromptStr
                                      ,const char* pzDefaultPath = NULL
                                      ,const char* pzDefaultFilename = NULL
                                      ,int*        pnIndexDefaultExtension = NULL
                                      ,const char* pzWildcard = wxFileSelectorDefaultWildcardStr
                                      ,int         nFlags = 0
                                      ,wxWindow*   pParent = NULL
                                      ,int         nX = -1
                                      ,int         nY = -1
                                     );

//
// Generic file load dialog
//
WXDLLEXPORT wxString wxLoadFileSelector( const char* pzWhat
                                        ,const char* pzExtension
                                        ,const char* pzDefaultName = NULL
                                        ,wxWindow*   pParent = NULL
                                       );

//
// Generic file save dialog
//
WXDLLEXPORT wxString wxSaveFileSelector( const char* pzWhat
                                        ,const char* pzExtension
                                        ,const char* pzDefaultName = NULL
                                        ,wxWindow*   pParent = NULL
                                       );

#endif

// _WX_FILEDLG_H_
