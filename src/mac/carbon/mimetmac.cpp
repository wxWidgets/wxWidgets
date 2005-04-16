/////////////////////////////////////////////////////////////////////////////
// Name:        mac/mimetype.cpp
// Purpose:     Mac Carbon implementation for wx mime-related classes
// Author:      Ryan Norton
// Modified by:
// Created:     04/16/2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Ryan Norton (<wxprojects@comcast.net>)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "mimetype.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/string.h"
  #if wxUSE_GUI
    #include "wx/icon.h"
  #endif
#endif //WX_PRECOMP


#if wxUSE_MIMETYPE

#include "wx/log.h"
#include "wx/file.h"
#include "wx/intl.h"
#include "wx/dynarray.h"
#include "wx/confbase.h"

#include "wx/mac/mimetype.h"
#include "wx/mac/private.h" //wxMacMakeStringFromPascal

// other standard headers
#include <ctype.h>
#include <InternetConfig.h> //For mime types

//
// On the mac there are two ways to open a file - one is through apple events and the
// finder, another is through mime types.
//
// So, really there are two ways to implement wxFileType...
//
// Mime types are only available on OS 8.1+ through the InternetConfig API
// 
// Much like the old-style file manager, it has 3 levels of flexibility for its methods -
// Low - which means you have to iterate yourself through the mime database
// Medium - which lets you sort of cache the database if you want to use lowlevel functions
// High - which requires access to the database every time
//
// We want to be efficient (i.e. professional :) ) about it, so we use a combo of low
// and mid-level functions
//
// TODO: Should we call ICBegin/ICEnd?  Then where?
//

// in case we're compiling in non-GUI mode
class WXDLLEXPORT wxIcon;

bool wxFileTypeImpl::SetCommand(const wxString& cmd, const wxString& verb, bool overwriteprompt)
{
    return FALSE;
}

bool wxFileTypeImpl::SetDefaultIcon(const wxString& strIcon, int index)
{
    return FALSE;
}

bool wxFileTypeImpl::GetOpenCommand(wxString *openCmd,
                               const wxFileType::MessageParameters& params) const
{
    wxString cmd = GetCommand(wxT("open"));

    *openCmd = wxFileType::ExpandCommand(cmd, params);

    return !openCmd->empty();
}

bool
wxFileTypeImpl::GetPrintCommand(wxString *printCmd,
                                const wxFileType::MessageParameters& params)
                                const
{
    wxString cmd = GetCommand(wxT("print"));

    *printCmd = wxFileType::ExpandCommand(cmd, params);

    return !printCmd->empty();
}

/*   START CODE SAMPLE FROM TECHNOTE 1002 (http://developer.apple.com/technotes/tn/tn1002.html) */

 /* IsRemoteVolume can be used to find out if the
    volume referred to by vRefNum is a remote volume
    located somewhere on a network. the volume's attribute
    flags (copied from the GetVolParmsInfoBuffer structure)
    are returned in the longword pointed to by vMAttrib. */
OSErr IsRemoteVolume(short vRefNum, Boolean *isRemote, long *vMAttrib) {
    HParamBlockRec volPB;
    GetVolParmsInfoBuffer volinfo;
    OSErr err;
    volPB.ioParam.ioVRefNum = vRefNum;
    volPB.ioParam.ioNamePtr = NULL;
    volPB.ioParam.ioBuffer = (Ptr) &volinfo;
    volPB.ioParam.ioReqCount = sizeof(volinfo);
    err = PBHGetVolParmsSync(&volPB);
    if (err == noErr) {
        *isRemote = (volinfo.vMServerAdr != 0);
        *vMAttrib = volinfo.vMAttrib;
    }
    return err;
}


    /* BuildVolumeList fills the array pointed to by vols with
    a list of the currently mounted volumes.  If includeRemote
    is true, then remote server volumes will be included in
    the list.  When remote server volumes are included in the
    list, they will be added to the end of the list.  On entry,
    *count should contain the size of the array pointed to by
    vols.  On exit, *count will be set to the number of id numbers
    placed in the array. If vMAttribMask is non-zero, then
    only volumes with matching attributes are added to the
    list of volumes. bits in the vMAttribMask should use the
    same encoding as bits in the vMAttrib field of
    the GetVolParmsInfoBuffer structure. */
OSErr BuildVolumeList(Boolean includeRemote, short *vols,
        long *count, long vMAttribMask) {
    HParamBlockRec volPB;
    Boolean isRemote;
    OSErr err;
    long nlocal, nremote;
    long vMAttrib;

        /* set up and check parameters */
    volPB.volumeParam.ioNamePtr = NULL;
    nlocal = nremote = 0;
    if (*count == 0) return noErr;

        /* iterate through volumes */
    for (volPB.volumeParam.ioVolIndex = 1;
        PBHGetVInfoSync(&volPB) == noErr;
        volPB.volumeParam.ioVolIndex++) {

                /* skip remote volumes, if necessary */
        err = IsRemoteVolume(volPB.volumeParam.ioVRefNum, &isRemote, &vMAttrib);
        if (err != noErr) goto bail;
        if ( ( includeRemote || ! isRemote )
        && (vMAttrib & vMAttribMask) == vMAttribMask ) {

                /* add local volumes at the front, remote
                volumes at the end */
            if (isRemote)
                vols[nlocal + nremote++] = volPB.volumeParam.ioVRefNum;
            else {
                if (nremote > 0)
                    BlockMoveData(vols+nlocal, vols+nlocal+1,
                        nremote*sizeof(short));
                vols[nlocal++] = volPB.volumeParam.ioVRefNum;
            }

                /* list full? */
            if ((nlocal + nremote) >= *count) break;
        }
    }
bail:
    *count = (nlocal + nremote);
    return err;
}


    /* FindApplication iterates through mounted volumes
    searching for an application with the given creator
    type.  If includeRemote is true, then remote volumes
    will be searched (after local ones) for an application
    with the creator type. */

#define kMaxVols 20

/* Hacked to output to appName */

OSErr FindApplication(OSType appCreator, Boolean includeRemote, Str255 appName) {
    short rRefNums[kMaxVols];
    long i, volCount;
    DTPBRec desktopPB;
    OSErr err;
    FSSpec realappSpec;
    FSSpec *appSpec = &realappSpec;
    
        /* get a list of volumes - with desktop files */
    volCount = kMaxVols;
    err = BuildVolumeList(includeRemote, rRefNums, &volCount,
        (1<<bHasDesktopMgr) );
    if (err != noErr) return err;

        /* iterate through the list */
    for (i=0; i<volCount; i++) {

            /* has a desktop file? */
        desktopPB.ioCompletion = NULL;
        desktopPB.ioVRefNum = rRefNums[i];
        desktopPB.ioNamePtr = NULL;
        desktopPB.ioIndex = 0;
        err = PBDTGetPath(&desktopPB);
        if (err != noErr) continue;

            /* has the correct app?? */
        desktopPB.ioFileCreator = appCreator;
        desktopPB.ioNamePtr = appName;
        err = PBDTGetAPPLSync(&desktopPB);
        if (err != noErr) continue;

            /* make a file spec referring to it */
        err = FSMakeFSSpec(rRefNums[i],
              desktopPB.ioAPPLParID, appName,
              appSpec);
        if (err != noErr) continue;

           /* found it! */
        return noErr;

    }
    return fnfErr;
}

/*   END CODE SAMPLE FROM TECHNOTE 1002 (http://developer.apple.com/technotes/tn/tn1002.html) */

wxString wxFileTypeImpl::GetCommand(const wxString& verb) const
{
    if(!m_manager)
        return wxEmptyString;
        
    if(verb == wxT("open"))
    {
        ICMapEntry entry;
        ICGetMapEntry( (ICInstance) m_manager->m_hIC, 
                       (Handle) m_manager->m_hDatabase, 
                       m_lIndex, &entry);
        
        //Technote 1002 is a godsend in launching apps :)
        //The entry in the mimetype database only contains the app
        //that's registered - it may not exist... we need to remap the creator
        //type and find the right application
        Str255 outName;
        if(FindApplication(entry.fileCreator, false, outName) != noErr)
            return wxEmptyString;
            
        //TODO: this is only partially correct - 
        //it should go to the os-specific application path folder (using wxStdPaths maybe?),
        //then go to the bundled app and return that full path
        return wxMacMakeStringFromPascal(outName);
    }
    return wxEmptyString;
}

bool wxFileTypeImpl::GetExtensions(wxArrayString& extensions)
{
    if(!m_manager)
        return false;
    
    ICMapEntry entry;
    ICGetMapEntry( (ICInstance) m_manager->m_hIC, 
                   (Handle) m_manager->m_hDatabase, 
                   m_lIndex, &entry);
    
    //entry has period in it
    wxString sCurrentExtension = wxMacMakeStringFromPascal(entry.extension);
    extensions.Add( sCurrentExtension.Right(sCurrentExtension.Length()-1) );
    return true;
}

bool wxFileTypeImpl::GetMimeType(wxString *mimeType) const
{
    if(!m_manager)
        return false;
    
    ICMapEntry entry;
    ICGetMapEntry( (ICInstance) m_manager->m_hIC, 
                   (Handle) m_manager->m_hDatabase, 
                   m_lIndex, &entry);
    
    *mimeType = wxMacMakeStringFromPascal(entry.MIMEType);
    return true;
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

bool wxFileTypeImpl::GetIcon(wxIconLocation *WXUNUSED(icon)) const
{
    // no such file type or no value or incorrect icon entry
    return FALSE;
}

bool wxFileTypeImpl::GetDescription(wxString *desc) const
{
    if(!m_manager)
        return false;
    
    ICMapEntry entry;
    ICGetMapEntry( (ICInstance) m_manager->m_hIC, 
                   (Handle) m_manager->m_hDatabase, 
                   m_lIndex, &entry);
    
    *desc = wxString((char*)entry.entryName, wxConvLocal);
    return true;
}

size_t wxFileTypeImpl::GetAllCommands(wxArrayString * verbs, wxArrayString * commands,
                   const wxFileType::MessageParameters& params) const
{
    wxFAIL_MSG( _T("wxFileTypeImpl::GetAllCommands() not yet implemented") );
    return 0;
}

void wxMimeTypesManagerImpl::Initialize(int mailcapStyles, const wxString& extraDir)
{
    wxASSERT_MSG(m_hIC == NULL, wxT("Already initialized wxMimeTypesManager!"));

    //start internet config - log if there's an error
    //the second param is the signature of the application, also known
    //as resource ID 0.  However, as per some recent discussions, we may not
    //have a signature for this app, so a generic 'APPL' which is the executable 
    //type will work for now
    OSStatus status = ICStart( (ICInstance*) &m_hIC, 'APPL'); 
    
    if(status != noErr)
    {
        wxLogSysError(wxT("Could not initialize wxMimeTypesManager!"));
        wxASSERT( false );
        return;
    }
    
    ICAttr attr;
    m_hDatabase = (void**) NewHandle(0);
    status = ICFindPrefHandle( (ICInstance) m_hIC, kICMapping, &attr, (Handle) m_hDatabase );

    //the database file can be corrupt (on OSX its
    //~/Library/Preferences/com.apple.internetconfig.plist)
    //- bail if it is
    if(status != noErr)
    {
        ClearData();
        wxLogSysError(wxT("Bad Mime Database!"));
        return;
    }

    //obtain the number of entries in the map
    status = ICCountMapEntries( (ICInstance) m_hIC, (Handle) m_hDatabase, &m_lCount );
    wxASSERT( status == noErr );
}

void wxMimeTypesManagerImpl::ClearData()
{
    if(m_hIC != NULL)
    {
        DisposeHandle((Handle)m_hDatabase);
        //this can return an error, but we don't really care that much about it
        ICStop( (ICInstance) m_hIC );
        m_hIC = NULL;
    }
}

// extension -> file type
wxFileType* wxMimeTypesManagerImpl::GetFileTypeFromExtension(const wxString& e)
{
    wxASSERT_MSG( m_hIC != NULL, wxT("wxMimeTypesManager not Initialized!") );
    
//    ICMapEntry entry;
//    OSStatus status = ICMapEntriesFilename( (ICInstance) m_hIC, (Handle) m_hDatabase,
//                                            (unsigned char*) e.mb_str(wxConvLocal), &entry );

//    if (status != noErr)
//        return NULL; //err or ext not known
    //low level functions - iterate through the database    
    ICMapEntry entry;
    wxFileType* pFileType;
    long pos;
    
    for(long i = 1; i <= m_lCount; ++i)
    {
        OSStatus status = ICGetIndMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, i, &pos, &entry);
        
        if(status == noErr)
        {       
            wxString sCurrentExtension = wxMacMakeStringFromPascal(entry.extension);
            if( sCurrentExtension.Right(sCurrentExtension.Length()-1) == e ) //entry has period in it
            {
                pFileType = new wxFileType();
                pFileType->m_impl->Init((wxMimeTypesManagerImpl*)this, pos);
                break;
            }
        }
    }
    
    return pFileType;    
}

// MIME type -> extension -> file type
wxFileType* wxMimeTypesManagerImpl::GetFileTypeFromMimeType(const wxString& mimeType)
{
    wxASSERT_MSG( m_hIC != NULL, wxT("wxMimeTypesManager not Initialized!") );

    //low level functions - iterate through the database    
    ICMapEntry entry;
    wxFileType* pFileType;
    
    long pos;
    
    for(long i = 1; i <= m_lCount; ++i)
    {
        OSStatus status = ICGetIndMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, i, &pos, &entry);
        wxASSERT_MSG( status == noErr, wxString::Format(wxT("Error: %d"), (int)status) );
        
        if(status == noErr)
        {        
            if( wxMacMakeStringFromPascal(entry.MIMEType) == mimeType)
            {
                pFileType = new wxFileType();
                pFileType->m_impl->Init((wxMimeTypesManagerImpl*)this, pos);
                break;
            }
        }
    }
    
    return pFileType;
}

size_t wxMimeTypesManagerImpl::EnumAllFileTypes(wxArrayString& mimetypes)
{
    wxASSERT_MSG( m_hIC != NULL, wxT("wxMimeTypesManager not Initialized!") );

    //low level functions - iterate through the database    
    ICMapEntry entry;
    
    long pos;
    
    for(long i = 1; i <= m_lCount; ++i)
    {
        OSStatus status = ICGetIndMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, i, &pos, &entry);
        if( status == noErr )
            mimetypes.Add( wxMacMakeStringFromPascal(entry.MIMEType) );
    }
    
    return m_lCount;
}

wxFileType *
wxMimeTypesManagerImpl::Associate(const wxFileTypeInfo& ftInfo)
{
    wxFAIL_MSG( _T("wxMimeTypesManagerImpl::Associate() not yet implemented") );
    //on mac you have to embed it into the mac's file reference resource ('FREF' I believe)
    //or, alternately, you could just add an entry to m_hDatabase, but you'd need to get
    //the app's signature somehow...

    return NULL;
}

bool
wxMimeTypesManagerImpl::Unassociate(wxFileType *ft)
{
    //this should be as easy as removing the entry from the database and then saving 
    //the database
    return FALSE;
}

#endif //wxUSE_MIMETYPE