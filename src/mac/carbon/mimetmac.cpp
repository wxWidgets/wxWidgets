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

OSErr FindApplication(OSType appCreator, Boolean includeRemote, Str255 appName, FSSpec* appSpec) {
    short rRefNums[kMaxVols];
    long i, volCount;
    DTPBRec desktopPB;
    OSErr err;
    
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

//yeah, duplicated code
pascal  OSErr  FSpGetFullPath(const FSSpec *spec,
                 short *fullPathLength,
                 Handle *fullPath)
{
  OSErr    result;
  OSErr    realResult;
  FSSpec    tempSpec;
  CInfoPBRec  pb;
  
  *fullPathLength = 0;
  *fullPath = NULL;
  
  
  /* Default to noErr */
  realResult = result = noErr;
  
  /* work around Nav Services "bug" (it returns invalid FSSpecs with empty names) */
/*
  if ( spec->name[0] == 0 )
  {
    result = FSMakeFSSpecCompat(spec->vRefNum, spec->parID, spec->name, &tempSpec);
  }
  else
  {
*/
    /* Make a copy of the input FSSpec that can be modified */
    BlockMoveData(spec, &tempSpec, sizeof(FSSpec));
/*  }*/
  
  if ( result == noErr )
  {
    if ( tempSpec.parID == fsRtParID )
    {
      /* The object is a volume */
      
      /* Add a colon to make it a full pathname */
      ++tempSpec.name[0];
      tempSpec.name[tempSpec.name[0]] = ':';
      
      /* We're done */
      result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
    }
    else
    {
      /* The object isn't a volume */
      
      /* Is the object a file or a directory? */
      pb.dirInfo.ioNamePtr = tempSpec.name;
      pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
      pb.dirInfo.ioDrDirID = tempSpec.parID;
      pb.dirInfo.ioFDirIndex = 0;
      result = PBGetCatInfoSync(&pb);
      /* Allow file/directory name at end of path to not exist. */
      realResult = result;
      if ( (result == noErr) || (result == fnfErr) )
      {
        /* if the object is a directory, append a colon so full pathname ends with colon */
        if ( (result == noErr) && (pb.hFileInfo.ioFlAttrib & kioFlAttribDirMask) != 0 )
        {
          ++tempSpec.name[0];
          tempSpec.name[tempSpec.name[0]] = ':';
        }
        
        /* Put the object name in first */
        result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
        if ( result == noErr )
        {
          /* Get the ancestor directory names */
          pb.dirInfo.ioNamePtr = tempSpec.name;
          pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
          pb.dirInfo.ioDrParID = tempSpec.parID;
          do  /* loop until we have an error or find the root directory */
          {
            pb.dirInfo.ioFDirIndex = -1;
            pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;
            result = PBGetCatInfoSync(&pb);
            if ( result == noErr )
            {
              /* Append colon to directory name */
              ++tempSpec.name[0];
              tempSpec.name[tempSpec.name[0]] = ':';
              
              /* Add directory name to beginning of fullPath */
              (void) Munger(*fullPath, 0, NULL, 0, &tempSpec.name[1], tempSpec.name[0]);
              result = MemError();
            }
          } while ( (result == noErr) && (pb.dirInfo.ioDrDirID != fsRtDirID) );
        }
      }
    }
  }
  
  if ( result == noErr )
  {
    /* Return the length */
    *fullPathLength = GetHandleSize(*fullPath);
    result = realResult;  /* return realResult in case it was fnfErr */
  }
  else
  {
    /* Dispose of the handle and return NULL and zero length */
    if ( *fullPath != NULL )
    {
      DisposeHandle(*fullPath);
    }
    *fullPath = NULL;
    *fullPathLength = 0;
  }
  
  return ( result );
}                
                 
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

// debug helper
inline void wxLogMimeDebug(const wxChar* szMsg, OSStatus status)
{
    wxLogDebug(wxString::Format(wxT("%s  LINE:%i  OSERROR:%i"), szMsg, __LINE__, (int)status));
}

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

//
// Internet Config vs. Launch Services
//
// From OS 8 on there was internet config...
// However, OSX and its finder does not use info 
// from Internet Config at all - the Internet Config
// database ONLY CONTAINS APPS THAT ARE CLASSIC APPS
// OR REGISTERED THROUGH INTERNET CONFIG
//
// Therefore on OSX in order for the open command to be useful
// we need to go straight to launch services
//

#if defined(__DARWIN__)

//on darwin, use launch services
#include "LaunchServices.h"

wxString wxFileTypeImpl::GetCommand(const wxString& verb) const
{
    wxASSERT(m_manager);
    
    if(!m_manager)
        return wxEmptyString;
        
    if(verb == wxT("open"))
    {
        ICMapEntry entry;
        ICGetMapEntry( (ICInstance) m_manager->m_hIC, 
                       (Handle) m_manager->m_hDatabase, 
                       m_lIndex, &entry);
        
        wxString sCurrentExtension = wxMacMakeStringFromPascal(entry.extension);
        sCurrentExtension = sCurrentExtension.Right(sCurrentExtension.Length()-1 );

        //type, creator, ext, roles, outapp (FSRef), outappurl
        CFURLRef cfurlAppPath;
        OSStatus status = LSGetApplicationForInfo (kLSUnknownType,
            kLSUnknownCreator, 
            wxMacCFStringHolder(sCurrentExtension, wxLocale::GetSystemEncoding()), 
            kLSRolesAll, 
            NULL,
            &cfurlAppPath);
        
        if(status == noErr)
        {
            CFStringRef cfsUnixPath = CFURLCopyFileSystemPath(cfurlAppPath, kCFURLPOSIXPathStyle);
            CFRelease(cfurlAppPath);
                
            //PHEW!  Success!
            if(cfsUnixPath)
                return wxMacCFStringHolder(cfsUnixPath).AsString(wxLocale::GetSystemEncoding());            
        }
        else
        {
            wxLogDebug(wxString::Format(wxT("%i - %s - %i"), 
            __LINE__, 
            wxT("LSGetApplicationForInfo failed."),
            (int)status)); 
        }
    }
    
    return wxEmptyString;
}

#else //carbon/classic implementation

wxString wxFileTypeImpl::GetCommand(const wxString& verb) const
{
    wxASSERT(m_manager);
    
    if(!m_manager)
        return wxEmptyString;
        
    if(verb == wxT("open"))
    {
        ICMapEntry entry;
        ICGetMapEntry( (ICInstance) m_manager->m_hIC, 
                       (Handle) m_manager->m_hDatabase, 
                       m_lIndex, &entry);
        
        //The entry in the mimetype database only contains the app
        //that's registered - it may not exist... we need to remap the creator
        //type and find the right application
                
        // THIS IS REALLY COMPLICATED :\.  There are a lot of conversions going
        // on here.
        Str255 outName;
        FSSpec outSpec;
        if(FindApplication(entry.fileCreator, false, outName, &outSpec) != noErr)
            return wxEmptyString;
            
        Handle outPathHandle;
        short outPathSize;
        OSErr err = FSpGetFullPath(&outSpec, &outPathSize, &outPathHandle);
        
        if(err == noErr)
        {
            char* szPath = *outPathHandle;
            wxString sClassicPath(szPath, wxConvLocal, outPathSize);
#if defined(__DARWIN__)
            //Classic Path --> Unix (OSX) Path
            CFURLRef finalURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, 
                        wxMacCFStringHolder(sClassicPath, wxLocale::GetSystemEncoding()), 
                        kCFURLHFSPathStyle,
                        false); //false == not a directory

            //clean up memory from the classic path handle
            DisposeHandle(outPathHandle);
            
            if(finalURL)
            {
                CFStringRef cfsUnixPath = CFURLCopyFileSystemPath(finalURL, kCFURLPOSIXPathStyle);
                CFRelease(finalURL);
                
                //PHEW!  Success!
                if(cfsUnixPath)
                    return wxMacCFStringHolder(cfsUnixPath).AsString(wxLocale::GetSystemEncoding());
            }
#else //classic HFS path acceptable
            return sClassicPath;
#endif
        }
        else
        {
            wxLogMimeDebug(wxT("FSpGetFullPath failed."), (OSStatus)err);
        }
    }
    return wxEmptyString;
}
#endif //!DARWIN

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
        wxLogDebug(wxT("Could not initialize wxMimeTypesManager!"));
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
        wxLogDebug(wxT("Corrupt Mime Database!"));
        return;
    }

    //obtain the number of entries in the map
    status = ICCountMapEntries( (ICInstance) m_hIC, (Handle) m_hDatabase, &m_lCount );
    wxASSERT( status == noErr );
    /*
    //debug stuff
    ICMapEntry entry;
    long pos;
    
    for(long i = 1; i <= m_lCount; ++i)
    {
        OSStatus status = ICGetIndMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, i, &pos, &entry);
        
        if(status == noErr)
        {       
            wxString sCreator = wxMacMakeStringFromPascal(entry.creatorAppName);
            wxString sCurrentExtension = wxMacMakeStringFromPascal(entry.extension);
            wxString sMIMEType = wxMacMakeStringFromPascal(entry.MIMEType);
            
            wxFileTypeImpl impl;
            impl.Init(this, pos);
            
            if(sMIMEType == wxT("text/html") && sCurrentExtension == wxT(".html"))
            {
                wxString cmd;
                    impl.GetOpenCommand (&cmd, 
        wxFileType::MessageParameters (wxT("http://www.google.com"), 
                                       _T(""))); 

                wxPrintf(wxT("APP: [%s]\n"), cmd.c_str());
            }
        }
    }
        */
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
    long lStartCount = (long) mimetypes.GetCount();
    
    for(long i = 1; i <= m_lCount; ++i)
    {
        OSStatus status = ICGetIndMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, i, &pos, &entry);
        if( status == noErr )
            mimetypes.Add( wxMacMakeStringFromPascal(entry.MIMEType) );
    }
    
    return mimetypes.GetCount() - lStartCount;
}


pascal  OSStatus  MoreProcGetProcessTypeSignature(
            const ProcessSerialNumberPtr pPSN,
            OSType *pProcessType,
            OSType *pCreator)
{
  OSStatus      anErr = noErr;  
  ProcessInfoRec    infoRec;
  ProcessSerialNumber localPSN;
  
  infoRec.processInfoLength = sizeof(ProcessInfoRec);
  infoRec.processName = nil;
  infoRec.processAppSpec = nil;

  if ( pPSN == nil ) {
    localPSN.highLongOfPSN = 0;
    localPSN.lowLongOfPSN  = kCurrentProcess;
  } else {
    localPSN = *pPSN;
  }
  
  anErr = GetProcessInformation(&localPSN, &infoRec);
  if (anErr == noErr)
  {
    *pProcessType = infoRec.processType;
    *pCreator = infoRec.processSignature;
  }
  
  return anErr;
}//end MoreProcGetProcessTypeSignature

wxFileType *
wxMimeTypesManagerImpl::Associate(const wxFileTypeInfo& ftInfo)
{
#if 1
    wxFAIL_MSG(wxT("Associate not ready for production use"));
    return NULL;
#else
    //on mac you have to embed it into the mac's file reference resource ('FREF' I believe)
    //or, alternately, you could just add an entry to m_hDatabase, but you'd need to get
    //the app's signature somehow...

    OSType processType,
           creator;
    OSStatus status = MoreProcGetProcessTypeSignature(NULL,&processType, &creator);
    
    if(status == noErr)
    {
        Str255 psCreatorName;
        FSSpec dummySpec;
        status = FindApplication(creator, false, psCreatorName, &dummySpec);
        
        if(status == noErr)
        {

            //get the file type if it exists - 
            //if it really does then modify the database then save it,
            //otherwise we need to create a whole new entry
            wxFileType* pFileType = GetFileTypeFromMimeType(ftInfo.GetMimeType());
            if(pFileType)
            {
                ICMapEntry entry;
                ICGetMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, 
                                pFileType->m_impl->m_lIndex, &entry);
                
                entry.creatorAppName = psCreatorName; 
                entry.fileCreator = creator;
                    
                status = ICSetMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, 
                                pFileType->m_impl->m_lIndex, &entry);

                //success
                if(status == noErr)
                    return pFileType;
                else 
                { 
                    wxLogDebug(wxString::Format(wxT("%i - %s"), __LINE__, wxT("ICSetMapEntry failed."))); 
                }
                
                //failure - cleanup
                delete pFileType;
            }
            else
            {
                //TODO: Maybe force all 3 of these to be non-empty?
                Str255 psExtension;
                Str255 psMimeType;
                Str255 psDescription;
                
                wxMacStringToPascal(ftInfo.GetExtensions()[0], 	psExtension);
                wxMacStringToPascal(ftInfo.GetMimeType(), 		psMimeType);
                wxMacStringToPascal(ftInfo.GetDescription(), 	psDescription);

                Str255 psPostCreatorName;
                wxMacStringToPascal(wxT(""), psPostCreatorName);


                //add the entry to the database
                //TODO:  Does this work?
                ICMapEntry entry;
                entry.totalLength = sizeof(ICMapEntry);
                entry.fixedLength = kICMapFixedLength;
                entry.version = 0;
                entry.fileType = 0; //TODO:  File type?
                entry.fileCreator = creator;
                entry.postCreator = 0;
                entry.flags = kICMapDataForkBit; //TODO:  Maybe resource is valid by default too?
                entry.extension = psExtension;
                entry.creatorAppName = psCreatorName;
                entry.postAppName = psPostCreatorName;
                entry.MIMEType = psMimeType;
                entry.entryName = psDescription;
                
                status = ICAddMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, &entry);
                
                if(status == noErr)
                {
                    //kICAttrNoChange means we don't care about attributes such as
                    //locking in the database
                    status = ICSetPrefHandle((ICInstance) m_hIC, kICMapping, 
                                             kICAttrNoChange, (Handle) m_hDatabase);
                                             
                    //return the entry in the database if successful
                    if(status == noErr)
                        return GetFileTypeFromMimeType(ftInfo.GetMimeType());
                    else 
                    { 
                        wxLogDebug(wxString::Format(wxT("%i - %s"), __LINE__, wxT("ICSetPrefHandle failed."))); 
                    }
                }
                else 
                { 
                    wxLogDebug(wxString::Format(wxT("%i - %s"), __LINE__, wxT("ICAppMapEntry failed."))); 
                }
            }
        } //end if FindApplcation was successful
        else 
        { 
            wxLogDebug(wxString::Format(wxT("%i - %s"), __LINE__, wxT("FindApplication failed."))); 
        }
    } //end if it could obtain app's signature
    else 
    { 
        wxLogDebug(wxString::Format(wxT("%i - %s"), __LINE__, wxT("GetProcessSignature failed."))); 
    }
    return NULL;
#endif
}

bool
wxMimeTypesManagerImpl::Unassociate(wxFileType *pFileType)
{
#if 1
    wxFAIL_MSG(wxT("Unassociate not ready for production use"));
    return FALSE;
#else
    //this should be as easy as removing the entry from the database and then saving 
    //the database
    OSStatus status = ICDeleteMapEntry( (ICInstance) m_hIC, (Handle) m_hDatabase, 
                            pFileType->m_impl->m_lIndex);
                                
    if(status == noErr)
    {
        //kICAttrNoChange means we don't care about attributes such as
        //locking in the database
        status = ICSetPrefHandle((ICInstance) m_hIC, kICMapping, 
                    kICAttrNoChange, (Handle) m_hDatabase);
            
        if(status == noErr)
            return TRUE;
        else
        { 
            wxLogDebug(wxString::Format(wxT("%i - %s"), __LINE__, wxT("ICSetPrefHandle failed."))); 
        }

    }
    else
    { 
        wxLogDebug(wxString::Format(wxT("%i - %s"), __LINE__, wxT("ICDeleteMapEntry failed."))); 
    }
    
    return FALSE;
#endif
}

#endif //wxUSE_MIMETYPE