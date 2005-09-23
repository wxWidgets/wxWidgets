/////////////////////////////////////////////////////////////////////////////
// Name:        smapi.cpp
// Purpose:     Simple MAPI classes
// Author:      PJ Naughter <pjna@naughter.com>
// Modified by: Julian Smart
// Created:     2001-08-21
// RCS-ID:      $Id$
// Copyright:   (c) PJ Naughter
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef __WXMSW__

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/string.h"
#include "wx/msw/private.h"

// mapi.h in Cygwin's include directory isn't a full implementation and is
// not sufficient for this lib. However recent versions of Cygwin also
// have another mapi.h in include/w32api which can be used.
//
#ifdef __CYGWIN__
#include <w32api/mapi.h>
#else
#include <mapi.h>
#endif

#include "wx/net/smapi.h"

class WXDLLIMPEXP_NETUTILS wxMapiData
{
public:
    wxMapiData()
    {
        m_hSession = 0;
        m_nLastError = 0;
        m_hMapi = NULL;
        m_lpfnMAPILogon = NULL;
        m_lpfnMAPILogoff = NULL;
        m_lpfnMAPISendMail = NULL;
        m_lpfnMAPIResolveName = NULL;
        m_lpfnMAPIFreeBuffer = NULL;
    }

    //Data
    LHANDLE             m_hSession; //Mapi Session handle
    long                m_nLastError; //Last Mapi error value
    HINSTANCE           m_hMapi; //Instance handle of the MAPI dll
    LPMAPILOGON         m_lpfnMAPILogon; //MAPILogon function pointer
    LPMAPILOGOFF        m_lpfnMAPILogoff; //MAPILogoff function pointer
    LPMAPISENDMAIL      m_lpfnMAPISendMail; //MAPISendMail function pointer
    LPMAPIRESOLVENAME   m_lpfnMAPIResolveName; //MAPIResolveName function pointer
    LPMAPIFREEBUFFER    m_lpfnMAPIFreeBuffer; //MAPIFreeBuffer function pointer
};


////////////////////////////////// Implementation /////////////////////////////

wxMapiSession::wxMapiSession()
{
    m_data = new wxMapiData;
    
    Initialise();
}

wxMapiSession::~wxMapiSession()
{
    //Logoff if logged on
    Logoff();
    
    //Unload the MAPI dll
    Deinitialise();
    
    delete m_data;
}

void wxMapiSession::Initialise() 
{
    //First make sure the "WIN.INI" entry for MAPI is present aswell 
    //as the MAPI32 dll being present on the system
    bool bMapiInstalled = (GetProfileInt(_T("MAIL"), _T("MAPI"), 0) != 0) && 
        (SearchPath(NULL, _T("MAPI32.DLL"), NULL, 0, NULL, NULL) != 0);
    
    if (bMapiInstalled)
    {
        //Load up the MAPI dll and get the function pointers we are interested in
        m_data->m_hMapi = ::LoadLibrary(_T("MAPI32.DLL"));
        if (m_data->m_hMapi)
        {
            m_data->m_lpfnMAPILogon = (LPMAPILOGON) GetProcAddress(m_data->m_hMapi, "MAPILogon");
            m_data->m_lpfnMAPILogoff = (LPMAPILOGOFF) GetProcAddress(m_data->m_hMapi, "MAPILogoff");
            m_data->m_lpfnMAPISendMail = (LPMAPISENDMAIL) GetProcAddress(m_data->m_hMapi, "MAPISendMail");
            m_data->m_lpfnMAPIResolveName = (LPMAPIRESOLVENAME) GetProcAddress(m_data->m_hMapi, "MAPIResolveName");
            m_data->m_lpfnMAPIFreeBuffer = (LPMAPIFREEBUFFER) GetProcAddress(m_data->m_hMapi, "MAPIFreeBuffer");
            
            //If any of the functions are not installed then fail the load
            if (m_data->m_lpfnMAPILogon == NULL ||
                m_data->m_lpfnMAPILogoff == NULL ||
                m_data->m_lpfnMAPISendMail == NULL ||
                m_data->m_lpfnMAPIResolveName == NULL ||
                m_data->m_lpfnMAPIFreeBuffer == NULL)
            {
                wxLogDebug(_T("Failed to get one of the functions pointer in MAPI32.DLL\n"));
                Deinitialise();
            }
        }
    }
    else
        wxLogDebug(_T("Mapi is not installed on this computer\n"));
}

void wxMapiSession::Deinitialise()
{
    if (m_data->m_hMapi)
    {
        //Unload the MAPI dll and reset the function pointers to NULL
        FreeLibrary(m_data->m_hMapi);
        m_data->m_hMapi = NULL;
        m_data->m_lpfnMAPILogon = NULL;
        m_data->m_lpfnMAPILogoff = NULL;
        m_data->m_lpfnMAPISendMail = NULL;
        m_data->m_lpfnMAPIResolveName = NULL;
        m_data->m_lpfnMAPIFreeBuffer = NULL;
    }
}

bool wxMapiSession::Logon(const wxString& sProfileName, const wxString& sPassword, wxWindow* pParentWnd)
{
    wxASSERT(MapiInstalled()); //MAPI must be installed
    wxASSERT(m_data->m_lpfnMAPILogon); //Function pointer must be valid
    
    //Initialise the function return value
    bool bSuccess = FALSE;
    
    //Just in case we are already logged in
    Logoff();
    
    //Setup the ascii versions of the profile name and password
    int nProfileLength = sProfileName.Length();
    
    LPSTR pszProfileName = NULL;
    LPSTR pszPassword = NULL;
    wxCharBuffer cbProfile(1),cbPassword(1);
    if (nProfileLength)
    {
#ifndef UNICODE
        pszProfileName = (LPSTR) sProfileName.c_str();
        pszPassword = (LPSTR) sPassword.c_str();
#else
        cbProfile = sProfileName.mb_str();
        cbPassword = sPassword.mb_str();
        pszProfileName = cbProfile.data();
        pszPassword = cbPassword.data();
#endif
    }
    
    //Setup the flags & UIParam parameters used in the MapiLogon call
    FLAGS flags = 0;
    ULONG nUIParam = 0;
    if (nProfileLength == 0)
    {
        //No profile name given, then we must interactively request a profile name
        if (pParentWnd)
        {
            nUIParam = (ULONG) (HWND) pParentWnd->GetHWND();
            flags |= MAPI_LOGON_UI;
        }
        else
        {
            //No window given, just use the main window of the app as the parent window
            if (wxTheApp->GetTopWindow())
            {
                nUIParam = (ULONG) (HWND) wxTheApp->GetTopWindow()->GetHWND();
                flags |= MAPI_LOGON_UI;
            }
        }
    }
    
    //First try to acquire a new MAPI session using the supplied settings using the MAPILogon functio
    ULONG nError = m_data->m_lpfnMAPILogon(nUIParam, pszProfileName, pszPassword, flags | MAPI_NEW_SESSION, 0, &m_data->m_hSession);
    if (nError != SUCCESS_SUCCESS && nError != MAPI_E_USER_ABORT)
    {
        //Failed to create a create mapi session, try to acquire a shared mapi session
        wxLogDebug(_T("Failed to logon to MAPI using a new session, trying to acquire a shared one\n"));
        nError = m_data->m_lpfnMAPILogon(nUIParam, NULL, NULL, 0, 0, &m_data->m_hSession);
        if (nError == SUCCESS_SUCCESS)
        {
            m_data->m_nLastError = SUCCESS_SUCCESS;
            bSuccess = TRUE;
        }
        else
        {
            wxLogDebug(_T("Failed to logon to MAPI using a shared session, Error:%ld\n"), nError);
            m_data->m_nLastError = nError;
        }
    }
    else if (nError == SUCCESS_SUCCESS)
    {
        m_data->m_nLastError = SUCCESS_SUCCESS;
        bSuccess = TRUE;
    }
    
    return bSuccess;
}

bool wxMapiSession::LoggedOn() const
{
    return (m_data->m_hSession != 0);
}

bool wxMapiSession::MapiInstalled() const
{
    return (m_data->m_hMapi != NULL);
}

bool wxMapiSession::Logoff()
{
    wxASSERT(MapiInstalled()); //MAPI must be installed
    wxASSERT(m_data->m_lpfnMAPILogoff); //Function pointer must be valid
    
    //Initialise the function return value
    bool bSuccess = FALSE;
    
    if (m_data->m_hSession)
    {
        //Call the MAPILogoff function
        ULONG nError = m_data->m_lpfnMAPILogoff(m_data->m_hSession, 0, 0, 0); 
        if (nError != SUCCESS_SUCCESS)
        {
            wxLogDebug(_T("Failed in call to MapiLogoff, Error:%ld"), nError);
            m_data->m_nLastError = nError;
            bSuccess = TRUE;
        }
        else
        {
            m_data->m_nLastError = SUCCESS_SUCCESS;
            bSuccess = TRUE;
        }
        m_data->m_hSession = 0;
    }
    
    return bSuccess;
}

bool wxMapiSession::Resolve(const wxString& sName, void* lppRecip1)
{
    lpMapiRecipDesc* lppRecip = (lpMapiRecipDesc*) lppRecip1;
    
    wxASSERT(MapiInstalled()); //MAPI must be installed
    wxASSERT(m_data->m_lpfnMAPIResolveName); //Function pointer must be valid
    wxASSERT(LoggedOn()); //Must be logged on to MAPI
    wxASSERT(m_data->m_hSession); //MAPI session handle must be valid
    
    //Call the MAPIResolveName function
#ifndef UNICODE
    LPSTR lpszAsciiName = (LPSTR) sName.c_str();
#else
    wxCharBuffer cbName(1);
    cbName = sName.mb_str();
    LPSTR lpszAsciiName = cbName.data();
#endif
    ULONG nError = m_data->m_lpfnMAPIResolveName(m_data->m_hSession, 0, lpszAsciiName, 0, 0, lppRecip);
    if (nError != SUCCESS_SUCCESS)
    {
        wxLogDebug(_T("Failed to resolve the name: %s, Error:%ld\n"),
                   sName.c_str(), nError);
        m_data->m_nLastError = nError;
    }
    
    return (nError == SUCCESS_SUCCESS);
}

bool wxMapiSession::Send(wxMailMessage& message)
{
    wxASSERT(MapiInstalled()); //MAPI must be installed
    wxASSERT(m_data->m_lpfnMAPISendMail); //Function pointer must be valid
    wxASSERT(m_data->m_lpfnMAPIFreeBuffer); //Function pointer must be valid
    wxASSERT(LoggedOn()); //Must be logged on to MAPI
    wxASSERT(m_data->m_hSession); //MAPI session handle must be valid
    
    //Initialise the function return value
    bool bSuccess = FALSE;  
    
    //Create the MapiMessage structure to match the message parameter send into us
    MapiMessage mapiMessage;
    ZeroMemory(&mapiMessage, sizeof(mapiMessage));
#ifndef UNICODE
    mapiMessage.lpszSubject = (LPSTR) message.m_subject.c_str();
    mapiMessage.lpszNoteText = (LPSTR) message.m_body.c_str();
#else
    wxCharBuffer cbSubject(1),cbBody(1),cbOriginator(1);
    cbSubject = message.m_subject.mb_str();
    cbBody = message.m_body.mb_str();
    mapiMessage.lpszSubject = cbSubject.data();
    mapiMessage.lpszNoteText = cbBody.data();
#endif
    mapiMessage.nRecipCount = message.m_to.GetCount() + message.m_cc.GetCount() + message.m_bcc.GetCount();
    wxASSERT(mapiMessage.nRecipCount); //Must have at least 1 recipient!
    
    //Allocate the recipients array
    mapiMessage.lpRecips = new MapiRecipDesc[mapiMessage.nRecipCount];

    // If we have a 'From' field, use it
    if (!message.m_from.IsEmpty())
    {
        mapiMessage.lpOriginator = new MapiRecipDesc;
        ZeroMemory(mapiMessage.lpOriginator, sizeof(MapiRecipDesc));

        mapiMessage.lpOriginator->ulRecipClass = MAPI_ORIG;
        // TODO Do we have to call Resolve?
#ifndef UNICODE
        mapiMessage.lpOriginator->lpszName = (LPSTR) message.m_from.c_str();
#else
        cbOriginator = message.m_from.mb_str();
        mapiMessage.lpOriginator->lpszName = cbOriginator.data();
#endif
    }
    
    //Setup the "To" recipients
    int nRecipIndex = 0;
    int nToSize = message.m_to.GetCount();
    int i;
    for (i=0; i<nToSize; i++)
    {
        MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
        ZeroMemory(&recip, sizeof(MapiRecipDesc));
        recip.ulRecipClass = MAPI_TO;
        wxString& sName = message.m_to[i];

        //Try to resolve the name
        lpMapiRecipDesc lpTempRecip;  
        if (Resolve(sName, (void*) &lpTempRecip))
        {
            //Resolve worked, put the resolved name back into the sName
            sName = wxString(lpTempRecip->lpszName,*wxConvCurrent);
            
            //Don't forget to free up the memory MAPI allocated for us
            m_data->m_lpfnMAPIFreeBuffer(lpTempRecip);
        }
#ifndef UNICODE
        recip.lpszName = (LPSTR) sName.c_str();
#else
        recip.lpszName = sName.mb_str().release();
#endif
        
        ++nRecipIndex;
    }
    
    //Setup the "CC" recipients
    int nCCSize = message.m_cc.GetCount();
    for (i=0; i<nCCSize; i++)
    {
        MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
        ZeroMemory(&recip, sizeof(MapiRecipDesc));
        recip.ulRecipClass = MAPI_CC;
        wxString& sName = message.m_cc[i];

        //Try to resolve the name
        lpMapiRecipDesc lpTempRecip;  
        if (Resolve(sName, (void*) &lpTempRecip))
        {
            //Resolve worked, put the resolved name back into the sName
            sName = wxString(lpTempRecip->lpszName,*wxConvCurrent);
            
            //Don't forget to free up the memory MAPI allocated for us
            m_data->m_lpfnMAPIFreeBuffer(lpTempRecip);
        }
#ifndef UNICODE
        recip.lpszName = (LPSTR) sName.c_str();
#else
        recip.lpszName = sName.mb_str().release();
#endif
        
        ++nRecipIndex;
    }
    
    //Setup the "BCC" recipients
    int nBCCSize = message.m_bcc.GetCount();
    for (i=0; i<nBCCSize; i++)
    {
        MapiRecipDesc& recip = mapiMessage.lpRecips[nRecipIndex];
        ZeroMemory(&recip, sizeof(MapiRecipDesc));
        recip.ulRecipClass = MAPI_BCC;
        wxString& sName = message.m_bcc[i];

        //Try to resolve the name
        lpMapiRecipDesc lpTempRecip;  
        if (Resolve(sName, (void*) &lpTempRecip))
        {
            //Resolve worked, put the resolved name back into the sName
            sName = wxString(lpTempRecip->lpszName,wxConvCurrent);
            
            //Don't forget to free up the memory MAPI allocated for us
            m_data->m_lpfnMAPIFreeBuffer(lpTempRecip);
        }
#ifndef UNICODE
        recip.lpszName = (LPSTR) sName.c_str();
#else
        recip.lpszName = sName.mb_str().release();
#endif
        
        ++nRecipIndex;
    }
    
    //Setup the attachments 
    int nAttachmentSize = message.m_attachments.GetCount();
    int nTitleSize = message.m_attachmentTitles.GetCount();
    if (nTitleSize)
    { 
        wxASSERT(nTitleSize == nAttachmentSize); //If you are going to set the attachment titles then you must set 
        //the attachment title for each attachment
    }
    if (nAttachmentSize)
    {
        mapiMessage.nFileCount = nAttachmentSize;
        mapiMessage.lpFiles = new MapiFileDesc[nAttachmentSize];
        for (i=0; i<nAttachmentSize; i++)
        {
            MapiFileDesc& file = mapiMessage.lpFiles[i];
            ZeroMemory(&file, sizeof(MapiFileDesc));
            file.nPosition = 0xFFFFFFFF;
            wxString& sFilename = message.m_attachments[i];

#ifndef UNICODE
            file.lpszPathName = (LPSTR) sFilename.c_str();
#else
            file.lpszPathName = sFilename.mb_str().release();
#endif
            //file.lpszFileName = file.lpszPathName;
            file.lpszFileName = NULL;

            if (nTitleSize && !message.m_attachmentTitles[i].IsEmpty())
            {
                wxString& sTitle = message.m_attachmentTitles[i];
#ifndef UNICODE
                file.lpszFileName = (LPSTR) sTitle.c_str();
#else
                file.lpszFileName = sTitle.mb_str().release();
#endif
            }
        }
    }
    
    //Do the actual send using MAPISendMail
    ULONG nError = m_data->m_lpfnMAPISendMail(m_data->m_hSession, 0, &mapiMessage, MAPI_DIALOG, 0);
    if (nError == SUCCESS_SUCCESS)
    {
        bSuccess = TRUE;
        m_data->m_nLastError = SUCCESS_SUCCESS;
    }
    else
    {
        wxLogDebug(_T("Failed to send mail message, Error:%ld\n"), nError);
        m_data->m_nLastError = nError;
    }
    
    //Tidy up the Attachements
    if (nAttachmentSize)
    {
#ifdef UNICODE
        for (i = 0;i < nAttachmentSize;i++)
        {
            free(mapiMessage.lpFiles[i].lpszPathName);
            free(mapiMessage.lpFiles[i].lpszFileName);
        }
#endif
        delete [] mapiMessage.lpFiles;
    }
    
    //Free up the Recipients and Originator memory
#ifdef UNICODE
    for (i = 0;i < nRecipIndex;i++)
        free(mapiMessage.lpRecips[i].lpszName);
#endif
    delete [] mapiMessage.lpRecips;

    delete mapiMessage.lpOriginator;
    
    return bSuccess;
}

long wxMapiSession::GetLastError() const
{
    return m_data->m_nLastError;
}

#endif // __WXMSW__
