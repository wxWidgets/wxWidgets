/*
     File:       MoreFilesExtras.h
 
     Contains:   A collection of useful high-level File Manager routines.
 
     Version:    Technology: MoreFiles
                 Release:    1.5.2
 
     Copyright:  © 1992-2001 by Apple Computer, Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/

/*
    You may incorporate this sample code into your applications without
    restriction, though the sample code has been provided "AS IS" and the
    responsibility for its operation is 100% yours.  However, what you are
    not permitted to do is to redistribute the source as "DSC Sample Code"
    after having made changes. If you're going to re-distribute the source,
    we require that you make it clear in the source that the code was
    descended from Apple Sample Code, but that you've made changes.
*/

#ifndef __MOREFILESEXTRAS__
#define __MOREFILESEXTRAS__

#ifndef __MACTYPES__
#include <MacTypes.h>
#endif

#ifndef __FILES__
#include <Files.h>
#endif

#include "Optimization.h"


#if PRAGMA_ONCE
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if PRAGMA_IMPORT
#pragma import on
#endif

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=mac68k
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(push, 2)
#elif PRAGMA_STRUCT_PACK
    #pragma pack(2)
#endif

/*****************************************************************************/

/*
**  Bit masks and macros to get common information out of ioACUser returned
**  by PBGetCatInfo (remember to clear ioACUser before calling PBGetCatInfo
**  since some file systems don't bother to set this field).
**
**  Use the GetDirAccessRestrictions or FSpGetDirAccessRestrictions
**  functions to retrieve the ioACUser access restrictions byte for
**  a folder.
**
**  Note:   The access restriction byte returned by PBGetCatInfo is the
**          2's complement of the user's privileges byte returned in
**          ioACAccess by PBHGetDirAccess.
*/

enum {
                                        /* mask for just the access restriction bits */
  acUserAccessMask              = (kioACUserNoSeeFolderMask + kioACUserNoSeeFilesMask + kioACUserNoMakeChangesMask), /* common access privilege settings */
  acUserFull                    = 0x00, /* no access restiction bits on */
  acUserNone                    = acUserAccessMask, /* all access restiction bits on */
  acUserDropBox                 = kioACUserNoSeeFolderMask + kioACUserNoSeeFilesMask, /* make changes, but not see files or folders */
  acUserBulletinBoard           = kioACUserNoMakeChangesMask /* see files and folders, but not make changes */
};


/*****************************************************************************/

/*
**  Deny mode permissions for use with the HOpenAware, HOpenRFAware,
**  FSpOpenAware, and FSpOpenRFAware functions.
**  Note: Common settings are the ones with comments.
*/

enum {
  dmNone                        = 0x0000,
  dmNoneDenyRd                  = fsRdDenyPerm,
  dmNoneDenyWr                  = fsWrDenyPerm,
  dmNoneDenyRdWr                = (fsRdDenyPerm + fsWrDenyPerm),
  dmRd                          = fsRdPerm, /* Single writer, multiple readers; the readers */
  dmRdDenyRd                    = (fsRdPerm + fsRdDenyPerm),
  dmRdDenyWr                    = (fsRdPerm + fsWrDenyPerm), /* Browsing - equivalent to fsRdPerm */
  dmRdDenyRdWr                  = (fsRdPerm + fsRdDenyPerm + fsWrDenyPerm),
  dmWr                          = fsWrPerm,
  dmWrDenyRd                    = (fsWrPerm + fsRdDenyPerm),
  dmWrDenyWr                    = (fsWrPerm + fsWrDenyPerm),
  dmWrDenyRdWr                  = (fsWrPerm + fsRdDenyPerm + fsWrDenyPerm),
  dmRdWr                        = fsRdWrPerm, /* Shared access - equivalent to fsRdWrShPerm */
  dmRdWrDenyRd                  = (fsRdWrPerm + fsRdDenyPerm),
  dmRdWrDenyWr                  = (fsRdWrPerm + fsWrDenyPerm), /* Single writer, multiple readers; the writer */
  dmRdWrDenyRdWr                = (fsRdWrPerm + fsRdDenyPerm + fsWrDenyPerm) /* Exclusive access - equivalent to fsRdWrPerm */
};


/*****************************************************************************/

/*
**  For those times where you need to use more than one kind of File Manager parameter
**  block but don't feel like wasting stack space, here's a parameter block you can reuse.
*/


union UniversalFMPB {
  ParamBlockRec       PB;
  CInfoPBRec          ciPB;
  DTPBRec             dtPB;
  HParamBlockRec      hPB;
  CMovePBRec          cmPB;
  WDPBRec             wdPB;
  FCBPBRec            fcbPB;
  XVolumeParam        xPB;
};
typedef union UniversalFMPB             UniversalFMPB;
typedef UniversalFMPB *                 UniversalFMPBPtr;
typedef UniversalFMPBPtr *              UniversalFMPBHandle;

/*
**  Used by GetUGEntries to return user or group lists
*/

struct UGEntry {
  short               objType;                /* object type: -1 = group; 0 = user */
  long                objID;                  /* the user or group ID */
  Str31               name;                   /* the user or group name */
};
typedef struct UGEntry                  UGEntry;
typedef UGEntry *                       UGEntryPtr;
typedef UGEntryPtr *                    UGEntryHandle;

/*
**  I use the following records instead of the AFPVolMountInfo and AFPXVolMountInfo structures in Files.h
*/
typedef unsigned char                   Str8[9];

struct MyAFPVolMountInfo {
  short               length;                 /* length of this record */
  VolumeType          media;                  /* type of media, always AppleShareMediaType */
  short               flags;                  /* 0 = normal mount; set bit 0 to inhibit greeting messages */
  char                nbpInterval;            /* NBP interval parameter; 7 is a good choice */
  char                nbpCount;               /* NBP count parameter; 5 is a good choice */
  short               uamType;                /* User Authentication Method */
  short               zoneNameOffset;         /* offset from start of record to zoneName */
  short               serverNameOffset;       /* offset from start of record to serverName */
  short               volNameOffset;          /* offset from start of record to volName */
  short               userNameOffset;         /* offset from start of record to userName */
  short               userPasswordOffset;     /* offset from start of record to userPassword */
  short               volPasswordOffset;      /* offset from start of record to volPassword */
  Str32               zoneName;               /* server's AppleTalk zone name */
  char                filler1;                /* to word align volPassword */
  Str32               serverName;             /* server name */
  char                filler2;                /* to word align volPassword */
  Str27               volName;                /* volume name */
  Str31               userName;               /* user name (zero length Pascal string for guest) */
  Str8                userPassword;           /* user password (zero length Pascal string if no user password) */
  char                filler3;                /* to word align volPassword */
  Str8                volPassword;            /* volume password (zero length Pascal string if no volume password) */
  char                filler4;                /* to end record on word boundry */
};
typedef struct MyAFPVolMountInfo        MyAFPVolMountInfo;
typedef MyAFPVolMountInfo *             MyAFPVolMountInfoPtr;
typedef MyAFPVolMountInfoPtr *          MyAFPVolMountInfoHandle;

struct MyAFPXVolMountInfo {
  short               length;                 /* length of this record */
  VolumeType          media;                  /* type of media, always AppleShareMediaType */
  short               flags;                  /* bits for no messages, no reconnect, etc */
  char                nbpInterval;            /* NBP interval parameter; 7 is a good choice */
  char                nbpCount;               /* NBP count parameter; 5 is a good choice */
  short               uamType;                /* User Authentication Method */
  short               zoneNameOffset;         /* offset from start of record to zoneName */
  short               serverNameOffset;       /* offset from start of record to serverName */
  short               volNameOffset;          /* offset from start of record to volName */
  short               userNameOffset;         /* offset from start of record to userName */
  short               userPasswordOffset;     /* offset from start of record to userPassword */
  short               volPasswordOffset;      /* offset from start of record to volPassword */
  short               extendedFlags;          /* extended flags word */
  short               uamNameOffset;          /* offset to a pascal UAM name string */
  short               alternateAddressOffset; /* offset to Alternate Addresses in tagged format */
  Str32               zoneName;               /* server's AppleTalk zone name */
  char                filler1;                /* to word align volPassword */
  Str32               serverName;             /* server name */
  char                filler2;                /* to word align volPassword */
  Str27               volName;                /* volume name */
  Str31               userName;               /* user name (zero length Pascal string for guest) */
  Str8                userPassword;           /* user password (zero length Pascal string if no user password) */
  char                filler3;                /* to word align volPassword */
  Str8                volPassword;            /* volume password (zero length Pascal string if no volume password) */
  char                filler4;                /* to word align uamNameOffset */
  Str32               uamName;                /* UAM name */
  char                filler5;                /* to word align alternateAddress */
  char                alternateAddress[1];    /* AFPAlternateAddress */
};
typedef struct MyAFPXVolMountInfo       MyAFPXVolMountInfo;
typedef MyAFPXVolMountInfo *            MyAFPXVolMountInfoPtr;
typedef MyAFPXVolMountInfoPtr *         MyAFPXVolMountInfoHandle;

/*****************************************************************************/

/* Functions to get information out of GetVolParmsInfoBuffer. */

/* version 1 field getters */

EXTERN_API( short )
GetVolParmsInfoVersion(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( long )
GetVolParmsInfoAttrib(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Handle )
GetVolParmsInfoLocalHand(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( long )
GetVolParmsInfoServerAdr(const GetVolParmsInfoBuffer * volParms);



/* version 2 field getters (assume zero result if version < 2) */

EXTERN_API( long )
GetVolParmsInfoVolumeGrade(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( long )
GetVolParmsInfoForeignPrivID(const GetVolParmsInfoBuffer * volParms);



/* version 3 field getters (assume zero result if version < 3) */

EXTERN_API( long )
GetVolParmsInfoExtendedAttributes(const GetVolParmsInfoBuffer * volParms);



/* attribute bits supported by all versions of GetVolParmsInfoBuffer */

EXTERN_API( Boolean )
isNetworkVolume(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasLimitFCBs(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasLocalWList(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasNoMiniFndr(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasNoVNEdit(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasNoLclSync(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasTrshOffLine(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasNoSwitchTo(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasNoDeskItems(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasNoBootBlks(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasAccessCntl(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasNoSysDir(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasExtFSVol(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasOpenDeny(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasCopyFile(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasMoveRename(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasDesktopMgr(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasShortName(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasFolderLock(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasPersonalAccessPrivileges(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasUserGroupList(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasCatSearch(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasFileIDs(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasBTreeMgr(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
hasBlankAccessPrivileges(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
supportsAsyncRequests(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
supportsTrashVolumeCache(const GetVolParmsInfoBuffer * volParms);



/* attribute bits supported by version 3 and greater versions of GetVolParmsInfoBuffer */

EXTERN_API( Boolean )
volIsEjectable(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupportsHFSPlusAPIs(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupportsFSCatalogSearch(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupportsFSExchangeObjects(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupports2TBFiles(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupportsLongNames(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupportsMultiScriptNames(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupportsNamedForks(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volSupportsSubtreeIterators(const GetVolParmsInfoBuffer * volParms);


EXTERN_API( Boolean )
volL2PCanMapFileBlocks(const GetVolParmsInfoBuffer * volParms);



/*****************************************************************************/

/* Functions for testing ioACUser bits. */

EXTERN_API( Boolean )
userIsOwner(SInt8 ioACUser);


EXTERN_API( Boolean )
userHasFullAccess(SInt8 ioACUser);


EXTERN_API( Boolean )
userHasDropBoxAccess(SInt8 ioACUser);


EXTERN_API( Boolean )
userHasBulletinBoard(SInt8 ioACUser);


EXTERN_API( Boolean )
userHasNoAccess(SInt8 ioACUser);



/*****************************************************************************/

EXTERN_API( void )
TruncPString(
  StringPtr          destination,
  ConstStr255Param   source,
  short              maxLength);


/*
    The TruncPString function copies up to maxLength characters from
    the source Pascal string to the destination Pascal string. TruncPString
    ensures that the truncated string ends on a single-byte character, or on
    the last byte of a multi-byte character.
    
    destination     output: destination Pascal string.
    source          input:  source Pascal string.
    maxLength       output: The maximum allowable length of the destination
                            string.
*/

/*****************************************************************************/

EXTERN_API( Ptr )
GetTempBuffer(
  long    buffReqSize,
  long *  buffActSize);


/*
    The GetTempBuffer function allocates a temporary buffer for file system
    operations which is at least 1024 bytes (1K) and a multiple of
    1024 bytes.
    
    buffReqSize     input:  Size you'd like the buffer to be.
    buffActSize     output: Size of buffer allocated.
    function result output: Pointer to memory allocated or nil if no memory
                            was available. The caller is responsible for
                            disposing of this buffer with DisposePtr.
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetVolumeInfoNoName(
  ConstStr255Param   pathname,
  short              vRefNum,
  HParmBlkPtr        pb);


/*
    GetVolumeInfoNoName uses pathname and vRefNum to call PBHGetVInfoSync
    in cases where the returned volume name is not needed by the caller.
    The pathname and vRefNum parameters are not touched, and the pb
    parameter is initialized by PBHGetVInfoSync except that ioNamePtr in
    the parameter block is always returned as NULL (since it might point
    to GetVolumeInfoNoName's local variable tempPathname).

    I noticed using this code in several places, so here it is once.
    This reduces the code size of MoreFiles.

    pathName    input:  Pointer to a full pathname or nil.  If you pass in a 
                        partial pathname, it is ignored. A full pathname to a
                        volume must end with a colon character (:).
    vRefNum     input:  Volume specification (volume reference number, working
                        directory number, drive number, or 0).
    pb          input:  A pointer to HParamBlockRec.
                output: The parameter block as filled in by PBHGetVInfoSync
                        except that ioNamePtr will always be NULL.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume, or pb was NULL
*/

/*****************************************************************************/

EXTERN_API( OSErr )
XGetVolumeInfoNoName(
  ConstStr255Param   pathname,
  short              vRefNum,
  XVolumeParamPtr    pb);


/*
    XGetVolumeInfoNoName uses pathname and vRefNum to call PBXGetVolInfoSync
    in cases where the returned volume name is not needed by the caller.
    The pathname and vRefNum parameters are not touched, and the pb
    parameter is initialized by PBXGetVolInfoSync except that ioNamePtr in
    the parameter block is always returned as NULL (since it might point
    to XGetVolumeInfoNoName's local variable tempPathname).

    pathName    input:  Pointer to a full pathname or nil.  If you pass in a 
                        partial pathname, it is ignored. A full pathname to a
                        volume must end with a colon character (:).
    vRefNum     input:  Volume specification (volume reference number, working
                        directory number, drive number, or 0).
    pb          input:  A pointer to HParamBlockRec.
                output: The parameter block as filled in by PBXGetVolInfoSync
                        except that ioNamePtr will always be NULL.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume, or pb was NULL
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetCatInfoNoName(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  CInfoPBPtr         pb);


/*
    GetCatInfoNoName uses vRefNum, dirID and name to call PBGetCatInfoSync
    in cases where the returned object is not needed by the caller.
    The vRefNum, dirID and name parameters are not touched, and the pb
    parameter is initialized by PBGetCatInfoSync except that ioNamePtr in
    the parameter block is always returned as NULL (since it might point
    to GetCatInfoNoName's local variable tempName).

    I noticed using this code in several places, so here it is once.
    This reduces the code size of MoreFiles.

    vRefNum         input:  Volume specification.
    dirID           input:  Directory ID.
    name            input:  Pointer to object name, or nil when dirID
                            specifies a directory that's the object.
    pb              input:  A pointer to CInfoPBRec.
                    output: The parameter block as filled in by
                            PBGetCatInfoSync except that ioNamePtr will
                            always be NULL.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
        
*/

/*****************************************************************************/

EXTERN_API( OSErr )
DetermineVRefNum(
  ConstStr255Param   pathname,
  short              vRefNum,
  short *            realVRefNum);


/*
    The DetermineVRefNum function determines the volume reference number of
    a volume from a pathname, a volume specification, or a combination
    of the two.
    WARNING: Volume names on the Macintosh are *not* unique -- Multiple
    mounted volumes can have the same name. For this reason, the use of a
    volume name or full pathname to identify a specific volume may not
    produce the results you expect.  If more than one volume has the same
    name and a volume name or full pathname is used, the File Manager
    currently uses the first volume it finds with a matching name in the
    volume queue.

    pathName    input:  Pointer to a full pathname or nil.  If you pass in a 
                        partial pathname, it is ignored. A full pathname to a
                        volume must end with a colon character (:).
    vRefNum     input:  Volume specification (volume reference number, working
                        directory number, drive number, or 0).
    realVRefNum output: The real volume reference number.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume
*/

/*****************************************************************************/

EXTERN_API( OSErr )
HGetVInfo(
  short            volReference,
  StringPtr        volName,
  short *          vRefNum,
  unsigned long *  freeBytes,
  unsigned long *  totalBytes);


/*
    The HGetVInfo function returns the name, volume reference number,
    available space (in bytes), and total space (in bytes) for the
    specified volume. You can specify the volume by providing its drive
    number, volume reference number, or 0 for the default volume.
    This routine is compatible with volumes up to 4 gigabytes.
    
    volReference    input:  The drive number, volume reference number,
                            or 0 for the default volume.
    volName         input:  A pointer to a buffer (minimum Str27) where
                            the volume name is to be returned or must
                            be nil.
                    output: The volume name.
    vRefNum         output: The volume reference number.
    freeBytes       output: The number of free bytes on the volume.
                            freeBytes is an unsigned long value.
    totalBytes      output: The total number of bytes on the volume.
                            totalBytes is an unsigned long value.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume
    
    __________
    
    Also see:   XGetVInfo
*/

/*****************************************************************************/

EXTERN_API( OSErr )
XGetVInfo(
  short       volReference,
  StringPtr   volName,
  short *     vRefNum,
  UInt64 *    freeBytes,
  UInt64 *    totalBytes);


/*
    The XGetVInfo function returns the name, volume reference number,
    available space (in bytes), and total space (in bytes) for the
    specified volume. You can specify the volume by providing its drive
    number, volume reference number, or 0 for the default volume.
    This routine is compatible with volumes up to 2 terabytes.
    
    volReference    input:  The drive number, volume reference number,
                            or 0 for the default volume.
    volName         input:  A pointer to a buffer (minimum Str27) where
                            the volume name is to be returned or must
                            be nil.
                    output: The volume name.
    vRefNum         output: The volume reference number.
    freeBytes       output: The number of free bytes on the volume.
                            freeBytes is an UnsignedWide value.
    totalBytes      output: The total number of bytes on the volume.
                            totalBytes is an UnsignedWide value.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume
    
    __________
    
    Also see:   HGetVInfo
*/

/*****************************************************************************/

EXTERN_API( OSErr )
CheckVolLock(
  ConstStr255Param   pathname,
  short              vRefNum);


/*
    The CheckVolLock function determines if a volume is locked - either by
    hardware or by software. If CheckVolLock returns noErr, then the volume
    is not locked.

    pathName    input:  Pointer to a full pathname or nil.  If you pass in a 
                        partial pathname, it is ignored. A full pathname to a
                        volume must end with a colon character (:).
    vRefNum     input:  Volume specification (volume reference number, working
                        directory number, drive number, or 0).
    
    Result Codes
        noErr               0       No error - volume not locked
        nsvErr              -35     No such volume
        wPrErr              -44     Volume locked by hardware
        vLckdErr            -46     Volume locked by software
        paramErr            -50     No default volume
*/

/*****************************************************************************/
/*
**  The following routines call Mac OS routines that are not supported by
**  Carbon:
**  
**      GetDriverName
**      FindDrive
**      GetDiskBlocks
**      GetVolState
*/

#if !TARGET_API_MAC_CARBON  //  {

/*****************************************************************************/

EXTERN_API( OSErr )
GetDriverName(
  short    driverRefNum,
  Str255   driverName);


/*
    The GetDriverName function returns a device driver's name.

    driverRefNum    input:  The driver reference number.
    driverName      output: The driver's name.
    
    Result Codes
        noErr               0       No error
        badUnitErr          -21     Bad driver reference number
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FindDrive(
  ConstStr255Param   pathname,
  short              vRefNum,
  DrvQElPtr *        driveQElementPtr);


/*
    The FindDrive function returns a pointer to a mounted volume's
    drive queue element.

    pathName            input:  Pointer to a full pathname or nil. If you
                                pass in a partial pathname, it is ignored.
                                A full pathname to a volume must end with
                                a colon character (:).
    vRefNum             input:  Volume specification (volume reference
                                number, working directory number, drive
                                number, or 0).
    driveQElementPtr    output: Pointer to a volume's drive queue element
                                in the drive queue. DO NOT change the
                                DrvQEl.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume
        nsDrvErr            -56     No such drive
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetDiskBlocks(
  ConstStr255Param   pathname,
  short              vRefNum,
  unsigned long *    numBlocks);


/*
    The GetDiskBlocks function returns the number of physical disk
    blocks on a disk drive. NOTE: This is not the same as volume
    allocation blocks!

    pathName    input:  Pointer to a full pathname or nil. If you
                        pass in a partial pathname, it is ignored.
                        A full pathname to a volume must end with
                        a colon character (:).
    vRefNum     input:  Volume specification (volume reference
                        number, working directory number, drive
                        number, or 0).
    numBlocks   output: The number of physical disk blocks on the disk drive.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume, driver reference
                                    number is zero, ReturnFormatList
                                    returned zero blocks, DriveStatus
                                    returned an unknown value, or
                                    driveQElementPtr->qType is unknown
        nsDrvErr            -56     No such drive
        statusErr           Ð18     Driver does not respond to this
                                    status request
        badUnitErr          Ð21     Driver reference number does not
                                    match unit table
        unitEmptyErr        Ð22     Driver reference number specifies
                                    a nil handle in unit table
        abortErr            Ð27     Request aborted by KillIO
        notOpenErr          Ð28     Driver not open
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetVolState(
  ConstStr255Param   pathname,
  short              vRefNum,
  Boolean *          volumeOnline,
  Boolean *          volumeEjected,
  Boolean *          driveEjectable,
  Boolean *          driverWantsEject);


/*
    The GetVolState function determines if a volume is online or offline,
    if an offline volume is ejected, and if the volume's driver is
    ejectable or wants eject calls.
    
    pathName            input:  Pointer to a full pathname or nil.
    vRefNum             input:  Volume specification (volume reference number,
                                working directory number, drive number, or 0).
    volumeOnline        output: True if the volume is online;
                                False if the volume is offline.
    volumeEjected       output: True if the volume is ejected (ejected
                                volumes are always offline); False if the
                                volume is not ejected.
    driveEjectable      output: True if the volume's drive is ejectable;
                                False if the volume's drive is not ejectable.
    driverWantsEject    output: True if the volume's driver wants an Eject
                                request after unmount (even if the drive
                                is not ejectable); False if the volume's
                                driver does not need an eject request.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume, or pb was NULL
*/

/*****************************************************************************/

#endif  //  }   !TARGET_API_MAC_CARBON

/*****************************************************************************/

EXTERN_API( OSErr )
GetVolFileSystemID(
  ConstStr255Param   pathname,
  short              vRefNum,
  short *            fileSystemID);


/*
    The GetVolFileSystemID function returned the file system ID of
    a mounted volume. The file system ID identifies the file system
    that handles requests to a particular volume. Here's a partial list
    of file system ID numbers (only Apple's file systems are listed):
        FSID    File System
        -----   -----------------------------------------------------
        $0000   Macintosh HFS or MFS
        $0100   ProDOS File System
        $0101   PowerTalk Mail Enclosures
        $4147   ISO 9660 File Access (through Foreign File Access)
        $4242   High Sierra File Access (through Foreign File Access)
        $464D   QuickTake File System (through Foreign File Access)
        $4953   Macintosh PC Exchange (MS-DOS)
        $4A48   Audio CD Access (through Foreign File Access)
        $4D4B   Apple Photo Access (through Foreign File Access)
    
    See the Technical Note "FL 35 - Determining Which File System
    Is Active" and the "Guide to the File System Manager" for more
    information.
    
    pathName        input:  Pointer to a full pathname or nil.  If you pass
                            in a partial pathname, it is ignored. A full
                            pathname to a volume must contain at least
                            one colon character (:) and must not start with
                            a colon character.
    vRefNum         input:  Volume specification (volume reference number,
                            working directory number, drive number, or 0).
    fileSystemID    output: The volume's file system ID.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        paramErr            -50     No default volume, or pb was NULL
*/

/*****************************************************************************/

EXTERN_API( OSErr )
UnmountAndEject(
  ConstStr255Param   pathname,
  short              vRefNum);


/*
    The UnmountAndEject function unmounts and ejects a volume. The volume
    is ejected only if it is ejectable and not already ejected.
    
    pathName    input:  Pointer to a full pathname or nil.  If you pass in a 
                        partial pathname, it is ignored. A full pathname to a
                        volume must end with a colon character (:).
    vRefNum     input:  Volume specification (volume reference number, working
                        directory number, drive number, or 0).
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad volume name
        fBsyErr             -47     One or more files are open
        paramErr            -50     No default volume
        nsDrvErr            -56     No such drive
        extFSErr            -58     External file system error - no file
                                    system claimed this call.
*/

/*****************************************************************************/

EXTERN_API( OSErr )
OnLine(
  FSSpecPtr   volumes,
  short       reqVolCount,
  short *     actVolCount,
  short *     volIndex);


/*
    The OnLine function returns the list of volumes currently mounted in
    an array of FSSpec records.
    
    A noErr result indicates that the volumes array was filled
    (actVolCount == reqVolCount) and there may be additional volumes
    mounted. A nsvErr result indicates that the end of the volume list
    was found and actVolCount volumes were actually found this time.

    volumes     input:  Pointer to array of FSSpec where the volume list
                        is returned.
    reqVolCount input:  Maximum number of volumes to return (the number of
                        elements in the volumes array).
    actVolCount output: The number of volumes actually returned.
    volIndex    input:  The current volume index position. Set to 1 to
                        start with the first volume.
                output: The volume index position to get the next volume.
                        Pass this value the next time you call OnLine to
                        start where you left off.
    
    Result Codes
        noErr               0       No error, but there are more volumes
                                    to list
        nsvErr              -35     No more volumes to be listed
        paramErr            -50     volIndex was <= 0
*/

/*****************************************************************************/

EXTERN_API( OSErr )
SetDefault(
  short    newVRefNum,
  long     newDirID,
  short *  oldVRefNum,
  long *   oldDirID);


/*
    The SetDefault function sets the default volume and directory to the
    volume specified by newVRefNum and the directory specified by newDirID.
    The current default volume reference number and directory ID are
    returned in oldVRefNum and oldDir and must be used to restore the
    default volume and directory to their previous state *as soon as
    possible* with the RestoreDefault function. These two functions are
    designed to be used as a wrapper around Standard I/O routines where
    the location of the file is implied to be the default volume and
    directory. In other words, this is how you should use these functions:
    
        error = SetDefault(newVRefNum, newDirID, &oldVRefNum, &oldDirID);
        if ( error == noErr )
        {
            // call the Stdio functions like remove, rename, tmpfile,
            // fopen, freopen, etc. or non-ANSI extensions like
            // fdopen,fsetfileinfo, -- create, open, unlink, etc. here!
            
            error = RestoreDefault(oldVRefNum, oldDirID);
        }
    
    By using these functions as a wrapper, you won't need to open a working
    directory (because SetDefault and RestoreDefault use HSetVol) and you
    won't have to worry about the effects of using HSetVol (documented in
    Technical Note "FL 11 - PBHSetVol is Dangerous" and in the
    Inside Macintosh: Files book in the description of the HSetVol and 
    PBHSetVol functions) because the default volume/directory is restored
    before giving up control to code that might be affected by HSetVol.
    
    newVRefNum  input:  Volume specification (volume reference number,
                        working directory number, drive number, or 0) of
                        the new default volume.
    newDirID    input:  Directory ID of the new default directory.
    oldVRefNum  output: The volume specification to save for use with
                        RestoreDefault.
    oldDirID    output: The directory ID to save for use with
                        RestoreDefault.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        bdNamErr            -37     Bad volume name
        fnfErr              -43     Directory not found
        paramErr            -50     No default volume
        afpAccessDenied     -5000   User does not have access to the directory
    
    __________
    
    Also see:   RestoreDefault
*/

/*****************************************************************************/

EXTERN_API( OSErr )
RestoreDefault(
  short   oldVRefNum,
  long    oldDirID);


/*
    The RestoreDefault function restores the default volume and directory
    to the volume specified by oldVRefNum and the directory specified by 
    oldDirID. The oldVRefNum and oldDirID parameters were previously
    obtained from the SetDefault function. These two functions are designed
    to be used as a wrapper around Standard C I/O routines where the
    location of the file is implied to be the default volume and directory.
    In other words, this is how you should use these functions:
    
        error = SetDefault(newVRefNum, newDirID, &oldVRefNum, &oldDirID);
        if ( error == noErr )
        {
            // call the Stdio functions like remove, rename, tmpfile,
            // fopen, freopen, etc. or non-ANSI extensions like
            // fdopen,fsetfileinfo, -- create, open, unlink, etc. here!
            
            error = RestoreDefault(oldVRefNum, oldDirID);
        }
    
    By using these functions as a wrapper, you won't need to open a working
    directory (because SetDefault and RestoreDefault use HSetVol) and you
    won't have to worry about the effects of using HSetVol (documented in
    Technical Note "FL 11 - PBHSetVol is Dangerous" and in the
    Inside Macintosh: Files book in the description of the HSetVol and 
    PBHSetVol functions) because the default volume/directory is restored
    before giving up control to code that might be affected by HSetVol.
    
    oldVRefNum  input: The volume specification to restore.
    oldDirID    input:  The directory ID to restore.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        bdNamErr            -37     Bad volume name
        fnfErr              -43     Directory not found
        paramErr            -50     No default volume
        rfNumErr            -51     Bad working directory reference number
        afpAccessDenied     -5000   User does not have access to the directory
    
    __________
    
    Also see:   SetDefault
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetDInfo(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  DInfo *            fndrInfo);


/*
    The GetDInfo function gets the finder information for a directory.

    vRefNum         input:  Volume specification.
    dirID           input:  Directory ID.
    name            input:  Pointer to object name, or nil when dirID
                            specifies a directory that's the object.
    fndrInfo        output: If the object is a directory, then its DInfo.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
        
    __________
    
    Also see:   FSpGetDInfo, FSpGetFInfoCompat
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpGetDInfo(
  const FSSpec *  spec,
  DInfo *         fndrInfo);


/*
    The FSpGetDInfo function gets the finder information for a directory.

    spec        input:  An FSSpec record specifying the directory.
    fndrInfo    output: If the object is a directory, then its DInfo.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
        
    __________
    
    Also see:   FSpGetFInfoCompat, GetDInfo
*/

/*****************************************************************************/

EXTERN_API( OSErr )
SetDInfo(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  const DInfo *      fndrInfo);


/*
    The SetDInfo function sets the finder information for a directory.

    vRefNum         input:  Volume specification.
    dirID           input:  Directory ID.
    name            input:  Pointer to object name, or nil when dirID
                            specifies a directory that's the object.
    fndrInfo        input:  The DInfo.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    Also see:   FSpSetDInfo, FSpSetFInfoCompat
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpSetDInfo(
  const FSSpec *  spec,
  const DInfo *   fndrInfo);


/*
    The FSpSetDInfo function sets the finder information for a directory.

    spec        input:  An FSSpec record specifying the directory.
    fndrInfo    input:  The DInfo.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    Also see:   FSpSetFInfoCompat, SetDInfo
*/

/*****************************************************************************/

#if OLDROUTINENAMES
    #define GetDirID(vRefNum, dirID, name, theDirID, isDirectory) GetDirectoryID(vRefNum, dirID, name, theDirID, isDirectory)
#endif
EXTERN_API( OSErr )
GetDirectoryID(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  long *             theDirID,
  Boolean *          isDirectory);


/*
    The GetDirectoryID function gets the directory ID number of the
    directory specified.  If a file is specified, then the parent
    directory of the file is returned and isDirectory is false.  If
    a directory is specified, then that directory's ID number is
    returned and isDirectory is true.
    WARNING: Volume names on the Macintosh are *not* unique -- Multiple
    mounted volumes can have the same name. For this reason, the use of a
    volume name or full pathname to identify a specific volume may not
    produce the results you expect.  If more than one volume has the same
    name and a volume name or full pathname is used, the File Manager
    currently uses the first volume it finds with a matching name in the
    volume queue.
    
    vRefNum         input:  Volume specification.
    dirID           input:  Directory ID.
    name            input:  Pointer to object name, or nil when dirID
                            specifies a directory that's the object.
    theDirID        output: If the object is a file, then its parent directory
                            ID. If the object is a directory, then its ID.
    isDirectory     output: True if object is a directory; false if
                            object is a file.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
*/

/*****************************************************************************/

#if OLDROUTINENAMES
    #define DirIDFromFSSpec(spec, theDirID, isDirectory) FSpGetDirectoryID(spec, theDirID, isDirectory)
#endif
EXTERN_API( OSErr )
FSpGetDirectoryID(
  const FSSpec *  spec,
  long *          theDirID,
  Boolean *       isDirectory);


/*
    The FSpGetDirectoryID function gets the directory ID number of the
    directory specified by spec. If spec is to a file, then the parent
    directory of the file is returned and isDirectory is false.  If
    spec is to a directory, then that directory's ID number is
    returned and isDirectory is true.
    
    spec            input:  An FSSpec record specifying the directory.
    theDirID        output: The directory ID.
    isDirectory     output: True if object is a directory; false if
                            object is a file.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetDirName(
  short   vRefNum,
  long    dirID,
  Str31   name);


/*
    The GetDirName function gets the name of a directory from its
    directory ID.

    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    name        output: Points to a Str31 where the directory name is to be
                        returned.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume or
                                    name parameter was NULL
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetIOACUser(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  SInt8 *            ioACUser);


/*
    GetIOACUser returns a directory's access restrictions byte.
    Use the masks and macro defined in MoreFilesExtras to check for
    specific access priviledges.
    
    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    name        input:  Pointer to object name, or nil when dirID
                        specifies a directory that's the object.
    ioACUser    output: The access restriction byte
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpGetIOACUser(
  const FSSpec *  spec,
  SInt8 *         ioACUser);


/*
    FSpGetIOACUser returns a directory's access restrictions byte.
    Use the masks and macro defined in MoreFilesExtras to check for
    specific access priviledges.
    
    spec        input:  An FSSpec record specifying the directory.
    ioACUser    output: The access restriction byte
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetParentID(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  long *             parID);


/*
    The GetParentID function gets the parent directory ID number of the
    specified object.
    
    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    name        input:  Pointer to object name, or nil when dirID specifies
                        a directory that's the object.
    parID       output: The parent directory ID of the specified object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetFilenameFromPathname(
  ConstStr255Param   pathname,
  Str255             filename);


/*
    The GetFilenameFromPathname function gets the file (or directory) name
    from the end of a full or partial pathname. Returns notAFileErr if the
    pathname is nil, the pathname is empty, or the pathname cannot refer to
    a filename (with a noErr result, the pathname could still refer to a
    directory).
    
    pathname    input:  A full or partial pathname.
    filename    output: The file (or directory) name.
    
    Result Codes
        noErr               0       No error
        notAFileErr         -1302   The pathname is nil, the pathname
                                    is empty, or the pathname cannot refer
                                    to a filename
    
    __________
    
    See also:   GetObjectLocation.
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetObjectLocation(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   pathname,
  short *            realVRefNum,
  long *             realParID,
  Str255             realName,
  Boolean *          isDirectory);


/*
    The GetObjectLocation function gets a file system object's location -
    that is, its real volume reference number, real parent directory ID,
    and name. While we're at it, determine if the object is a file or directory.
    If GetObjectLocation returns fnfErr, then the location information
    returned is valid, but it describes an object that doesn't exist.
    You can use the location information for another operation, such as
    creating a file or directory.
    
    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    pathname    input:  Pointer to object name, or nil when dirID specifies
                        a directory that's the object.
    realVRefNum output: The real volume reference number.
    realParID   output: The parent directory ID of the specified object.
    realName    output: The name of the specified object (the case of the
                        object name may not be the same as the object's
                        catalog entry on disk - since the Macintosh file
                        system is not case sensitive, it shouldn't matter).
    isDirectory output: True if object is a directory; false if object
                        is a file.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        notAFileErr         -1302   The pathname is nil, the pathname
                                    is empty, or the pathname cannot refer
                                    to a filename
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSMakeFSSpecCompat
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetDirItems(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  Boolean            getFiles,
  Boolean            getDirectories,
  FSSpecPtr          items,
  short              reqItemCount,
  short *            actItemCount,
  short *            itemIndex);


/*
    The GetDirItems function returns a list of items in the specified
    directory in an array of FSSpec records. File, subdirectories, or
    both can be returned in the list.
    
    A noErr result indicates that the items array was filled
    (actItemCount == reqItemCount) and there may be additional items
    left in the directory. A fnfErr result indicates that the end of
    the directory list was found and actItemCount items were actually
    found this time.

    vRefNum         input:  Volume specification.
    dirID           input:  Directory ID.
    name            input:  Pointer to object name, or nil when dirID
                            specifies a directory that's the object.
    getFiles        input:  Pass true to have files added to the items list.
    getDirectories  input:  Pass true to have directories added to the
                            items list.
    items           input:  Pointer to array of FSSpec where the item list
                            is returned.
    reqItemCount    input:  Maximum number of items to return (the number
                            of elements in the items array).
    actItemCount    output: The number of items actually returned.
    itemIndex       input:  The current item index position. Set to 1 to
                            start with the first item in the directory.
                    output: The item index position to get the next item.
                            Pass this value the next time you call
                            GetDirItems to start where you left off.
    
    Result Codes
        noErr               0       No error, but there are more items
                                    to list
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found, there are no more items
                                    to be listed.
        paramErr            -50     No default volume or itemIndex was <= 0
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
*/

/*****************************************************************************/

EXTERN_API( OSErr )
DeleteDirectoryContents(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The DeleteDirectoryContents function deletes the contents of a directory.
    All files and subdirectories in the specified directory are deleted.
    If a locked file or directory is encountered, it is unlocked and then
    deleted.  If any unexpected errors are encountered,
    DeleteDirectoryContents quits and returns to the caller.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to directory name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        wPrErr              -44     Hardware volume lock    
        fLckdErr            -45     File is locked  
        vLckdErr            -46     Software volume lock    
        fBsyErr             -47     File busy, directory not empty, or working directory control block open 
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    Also see:   DeleteDirectory
*/

/*****************************************************************************/

EXTERN_API( OSErr )
DeleteDirectory(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The DeleteDirectory function deletes a directory and its contents.
    All files and subdirectories in the specified directory are deleted.
    If a locked file or directory is encountered, it is unlocked and then
    deleted.  After deleting the directories contents, the directory is
    deleted. If any unexpected errors are encountered, DeleteDirectory
    quits and returns to the caller.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to directory name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        wPrErr              -44     Hardware volume lock
        fLckdErr            -45     File is locked
        vLckdErr            -46     Software volume lock
        fBsyErr             -47     File busy, directory not empty, or working directory control block open 
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    Also see:   DeleteDirectoryContents
*/

/*****************************************************************************/

EXTERN_API( OSErr )
CheckObjectLock(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The CheckObjectLock function determines if a file or directory is locked.
    If CheckObjectLock returns noErr, then the file or directory
    is not locked. If CheckObjectLock returns fLckdErr, the it is locked.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    Also see:   FSpCheckObjectLock
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpCheckObjectLock(const FSSpec * spec);


/*
    The FSpCheckObjectLock function determines if a file or directory is locked.
    If FSpCheckObjectLock returns noErr, then the file or directory
    is not locked.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    Also see:   CheckObjectLock
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetFileSize(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   fileName,
  long *             dataSize,
  long *             rsrcSize);


/*
    The GetFileSize function returns the logical size of a file's
    data and resource fork.
    
    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    name        input:  The name of the file.
    dataSize    output: The number of bytes in the file's data fork.
    rsrcSize    output: The number of bytes in the file's resource fork.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErrdirNFErr    -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpGetFileSize
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpGetFileSize(
  const FSSpec *  spec,
  long *          dataSize,
  long *          rsrcSize);


/*
    The FSpGetFileSize function returns the logical size of a file's
    data and resource fork.
    
    spec        input:  An FSSpec record specifying the file.
    dataSize    output: The number of bytes in the file's data fork.
    rsrcSize    output: The number of bytes in the file's resource fork.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume
        dirNFErrdirNFErr    -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   GetFileSize
*/

/*****************************************************************************/

EXTERN_API( OSErr )
BumpDate(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The BumpDate function changes the modification date of a file or
    directory to the current date/time.  If the modification date is already
    equal to the current date/time, then add one second to the
    modification date.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpBumpDate
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpBumpDate(const FSSpec * spec);


/*
    The FSpBumpDate function changes the modification date of a file or
    directory to the current date/time.  If the modification date is already
    equal to the current date/time, then add one second to the
    modification date.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   BumpDate
*/

/*****************************************************************************/

EXTERN_API( OSErr )
ChangeCreatorType(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  OSType             creator,
  OSType             fileType);


/*
    The ChangeCreatorType function changes the creator or file type of a file.

    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    name        input:  The name of the file.
    creator     input:  The new creator type or 0x00000000 to leave
                        the creator type alone.
    fileType    input:  The new file type or 0x00000000 to leave the
                        file type alone.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        notAFileErr         -1302   Name was not a file
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpChangeCreatorType
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpChangeCreatorType(
  const FSSpec *  spec,
  OSType          creator,
  OSType          fileType);


/*
    The FSpChangeCreatorType function changes the creator or file type of a file.

    spec        input:  An FSSpec record specifying the file.
    creator     input:  The new creator type or 0x00000000 to leave
                        the creator type alone.
    fileType    input:  The new file type or 0x00000000 to leave the
                        file type alone.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        notAFileErr         -1302   Name was not a file
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   ChangeCreatorType
*/

/*****************************************************************************/

EXTERN_API( OSErr )
ChangeFDFlags(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  Boolean            setBits,
  unsigned short     flagBits);


/*
    The ChangeFDFlags function sets or clears Finder Flag bits in the
    fdFlags field of a file or directory's FInfo record.
    
    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    name        input:  Pointer to object name, or nil when dirID specifies
                        a directory that's the object.
    setBits     input:  If true, then set the bits specified in flagBits.
                        If false, then clear the bits specified in flagBits.
    flagBits    input:  The flagBits parameter specifies which Finder Flag
                        bits to set or clear. If a bit in flagBits is set,
                        then the same bit in fdFlags is either set or
                        cleared depending on the state of the setBits
                        parameter.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpChangeFDFlags
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpChangeFDFlags(
  const FSSpec *   spec,
  Boolean          setBits,
  unsigned short   flagBits);


/*
    The FSpChangeFDFlags function sets or clears Finder Flag bits in the
    fdFlags field of a file or directory's FInfo record.
    
    spec        input:  An FSSpec record specifying the object.
    setBits     input:  If true, then set the bits specified in flagBits.
                        If false, then clear the bits specified in flagBits.
    flagBits    input:  The flagBits parameter specifies which Finder Flag
                        bits to set or clear. If a bit in flagBits is set,
                        then the same bit in fdFlags is either set or
                        cleared depending on the state of the setBits
                        parameter.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   ChangeFDFlags
*/

/*****************************************************************************/

EXTERN_API( OSErr )
SetIsInvisible(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The SetIsInvisible function sets the invisible bit in the fdFlags
    word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpSetIsInvisible, ClearIsInvisible, FSpClearIsInvisible
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpSetIsInvisible(const FSSpec * spec);


/*
    The FSpSetIsInvisible function sets the invisible bit in the fdFlags
    word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetIsInvisible, ClearIsInvisible, FSpClearIsInvisible
*/

/*****************************************************************************/

EXTERN_API( OSErr )
ClearIsInvisible(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The ClearIsInvisible function clears the invisible bit in the fdFlags
    word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetIsInvisible, FSpSetIsInvisible, FSpClearIsInvisible
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpClearIsInvisible(const FSSpec * spec);


/*
    The FSpClearIsInvisible function clears the invisible bit in the fdFlags
    word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetIsInvisible, FSpSetIsInvisible, ClearIsInvisible
*/

/*****************************************************************************/

EXTERN_API( OSErr )
SetNameLocked(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The SetNameLocked function sets the nameLocked bit in the fdFlags word
    of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpSetNameLocked, ClearNameLocked, FSpClearNameLocked
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpSetNameLocked(const FSSpec * spec);


/*
    The FSpSetNameLocked function sets the nameLocked bit in the fdFlags word
    of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetNameLocked, ClearNameLocked, FSpClearNameLocked
*/

/*****************************************************************************/

EXTERN_API( OSErr )
ClearNameLocked(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The ClearNameLocked function clears the nameLocked bit in the fdFlags
    word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetNameLocked, FSpSetNameLocked, FSpClearNameLocked
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpClearNameLocked(const FSSpec * spec);


/*
    The FSpClearNameLocked function clears the nameLocked bit in the fdFlags
    word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetNameLocked, FSpSetNameLocked, ClearNameLocked
*/

/*****************************************************************************/

EXTERN_API( OSErr )
SetIsStationery(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The SetIsStationery function sets the isStationery bit in the
    fdFlags word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpSetIsStationery, ClearIsStationery, FSpClearIsStationery
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpSetIsStationery(const FSSpec * spec);


/*
    The FSpSetIsStationery function sets the isStationery bit in the
    fdFlags word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetIsStationery, ClearIsStationery, FSpClearIsStationery
*/

/*****************************************************************************/

EXTERN_API( OSErr )
ClearIsStationery(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The ClearIsStationery function clears the isStationery bit in the
    fdFlags word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetIsStationery, FSpSetIsStationery, FSpClearIsStationery
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpClearIsStationery(const FSSpec * spec);


/*
    The FSpClearIsStationery function clears the isStationery bit in the
    fdFlags word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetIsStationery, FSpSetIsStationery, ClearIsStationery
*/

/*****************************************************************************/

EXTERN_API( OSErr )
SetHasCustomIcon(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The SetHasCustomIcon function sets the hasCustomIcon bit in the
    fdFlags word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpSetHasCustomIcon, ClearHasCustomIcon, FSpClearHasCustomIcon
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpSetHasCustomIcon(const FSSpec * spec);


/*
    The FSpSetHasCustomIcon function sets the hasCustomIcon bit in the
    fdFlags word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetHasCustomIcon, ClearHasCustomIcon, FSpClearHasCustomIcon
*/

/*****************************************************************************/

EXTERN_API( OSErr )
ClearHasCustomIcon(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The ClearHasCustomIcon function clears the hasCustomIcon bit in the
    fdFlags word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetHasCustomIcon, FSpSetHasCustomIcon, FSpClearHasCustomIcon
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpClearHasCustomIcon(const FSSpec * spec);


/*
    The FSpClearHasCustomIcon function clears the hasCustomIcon bit in the
    fdFlags word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   SetHasCustomIcon, FSpSetHasCustomIcon, ClearHasCustomIcon
*/

/*****************************************************************************/

EXTERN_API( OSErr )
ClearHasBeenInited(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name);


/*
    The ClearHasBeenInited function clears the hasBeenInited bit in the
    fdFlags word of the specified file or directory's finder information.
    
    vRefNum input:  Volume specification.
    dirID   input:  Directory ID.
    name    input:  Pointer to object name, or nil when dirID specifies
                    a directory that's the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpClearHasBeenInited
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpClearHasBeenInited(const FSSpec * spec);


/*
    The FSpClearHasBeenInited function clears the hasBeenInited bit in the
    fdFlags word of the specified file or directory's finder information.
    
    spec    input:  An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   ClearHasBeenInited
*/

/*****************************************************************************/

EXTERN_API( OSErr )
CopyFileMgrAttributes(
  short              srcVRefNum,
  long               srcDirID,
  ConstStr255Param   srcName,
  short              dstVRefNum,
  long               dstDirID,
  ConstStr255Param   dstName,
  Boolean            copyLockBit);


/*
    The CopyFileMgrAttributes function copies all File Manager attributes
    from the source file or directory to the destination file or directory.
    If copyLockBit is true, then set the locked state of the destination
    to match the source.

    srcVRefNum  input:  Source volume specification.
    srcDirID    input:  Source directory ID.
    srcName     input:  Pointer to source object name, or nil when
                        srcDirID specifies a directory that's the object.
    dstVRefNum  input:  Destination volume specification.
    dstDirID    input:  Destination directory ID.
    dstName     input:  Pointer to destination object name, or nil when
                        dstDirID specifies a directory that's the object.
    copyLockBit input:  If true, set the locked state of the destination
                        to match the source.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpCopyFileMgrAttributes
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpCopyFileMgrAttributes(
  const FSSpec *  srcSpec,
  const FSSpec *  dstSpec,
  Boolean         copyLockBit);


/*
    The FSpCopyFileMgrAttributes function copies all File Manager attributes
    from the source file or directory to the destination file or directory.
    If copyLockBit is true, then set the locked state of the destination
    to match the source.

    srcSpec     input:  An FSSpec record specifying the source object.
    dstSpec     input:  An FSSpec record specifying the destination object.
    copyLockBit input:  If true, set the locked state of the destination
                        to match the source.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     No default volume
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   CopyFileMgrAttributes
*/

/*****************************************************************************/

EXTERN_API( OSErr )
HOpenAware(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   fileName,
  short              denyModes,
  short *            refNum);


/*
    The HOpenAware function opens the data fork of a file using deny mode
    permissions instead the normal File Manager permissions.  If OpenDeny
    is not available, then HOpenAware translates the deny modes to the
    closest File Manager permissions and tries to open the file with
    OpenDF first, and then Open if OpenDF isn't available. By using
    HOpenAware with deny mode permissions, a program can be "AppleShare
    aware" and fall back on the standard File Manager open calls
    automatically.

    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    fileName    input:  The name of the file.
    denyModes   input:  The deny modes access under which to open the file.
    refNum      output: The file reference number of the opened file.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        tmfoErr             -42     Too many files open
        fnfErr              -43     File not found
        wPrErr              -44     Volume locked by hardware
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        opWrErr             -49     File already open for writing
        paramErr            -50     No default volume
        permErr             -54     File is already open and cannot be opened using specified deny modes
        afpAccessDenied     -5000   User does not have the correct access to the file
        afpDenyConflict     -5006   Requested access permission not possible
    
    __________
    
    See also:   FSpOpenAware, HOpenRFAware, FSpOpenRFAware
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpOpenAware(
  const FSSpec *  spec,
  short           denyModes,
  short *         refNum);


/*
    The FSpOpenAware function opens the data fork of a file using deny mode
    permissions instead the normal File Manager permissions.  If OpenDeny
    is not available, then FSpOpenAware translates the deny modes to the
    closest File Manager permissions and tries to open the file with
    OpenDF first, and then Open if OpenDF isn't available. By using
    FSpOpenAware with deny mode permissions, a program can be "AppleShare
    aware" and fall back on the standard File Manager open calls
    automatically.

    spec        input:  An FSSpec record specifying the file.
    denyModes   input:  The deny modes access under which to open the file.
    refNum      output: The file reference number of the opened file.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        tmfoErr             -42     Too many files open
        fnfErr              -43     File not found
        wPrErr              -44     Volume locked by hardware
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        opWrErr             -49     File already open for writing
        paramErr            -50     No default volume
        permErr             -54     File is already open and cannot be opened using specified deny modes
        afpAccessDenied     -5000   User does not have the correct access to the file
        afpDenyConflict     -5006   Requested access permission not possible
    
    __________
    
    See also:   HOpenAware, HOpenRFAware, FSpOpenRFAware
*/

/*****************************************************************************/

EXTERN_API( OSErr )
HOpenRFAware(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   fileName,
  short              denyModes,
  short *            refNum);


/*
    The HOpenRFAware function opens the resource fork of a file using deny
    mode permissions instead the normal File Manager permissions.  If
    OpenRFDeny is not available, then HOpenRFAware translates the deny
    modes to the closest File Manager permissions and tries to open the
    file with OpenRF. By using HOpenRFAware with deny mode permissions,
    a program can be "AppleShare aware" and fall back on the standard
    File Manager open calls automatically.

    vRefNum     input:  Volume specification.
    dirID       input:  Directory ID.
    fileName    input:  The name of the file.
    denyModes   input:  The deny modes access under which to open the file.
    refNum      output: The file reference number of the opened file.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        tmfoErr             -42     Too many files open
        fnfErr              -43     File not found
        wPrErr              -44     Volume locked by hardware
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        opWrErr             -49     File already open for writing
        paramErr            -50     No default volume
        permErr             -54     File is already open and cannot be opened using specified deny modes
        afpAccessDenied     -5000   User does not have the correct access to the file
        afpDenyConflict     -5006   Requested access permission not possible
    
    __________
    
    See also:   HOpenAware, FSpOpenAware, FSpOpenRFAware
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpOpenRFAware(
  const FSSpec *  spec,
  short           denyModes,
  short *         refNum);


/*
    The FSpOpenRFAware function opens the resource fork of a file using deny
    mode permissions instead the normal File Manager permissions.  If
    OpenRFDeny is not available, then FSpOpenRFAware translates the deny
    modes to the closest File Manager permissions and tries to open the
    file with OpenRF. By using FSpOpenRFAware with deny mode permissions,
    a program can be "AppleShare aware" and fall back on the standard
    File Manager open calls automatically.

    spec        input:  An FSSpec record specifying the file.
    denyModes   input:  The deny modes access under which to open the file.
    refNum      output: The file reference number of the opened file.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        tmfoErr             -42     Too many files open
        fnfErr              -43     File not found
        wPrErr              -44     Volume locked by hardware
        fLckdErr            -45     File is locked
        vLckdErr            -46     Volume is locked or read-only
        opWrErr             -49     File already open for writing
        paramErr            -50     No default volume
        permErr             -54     File is already open and cannot be opened using specified deny modes
        afpAccessDenied     -5000   User does not have the correct access to the file
        afpDenyConflict     -5006   Requested access permission not possible
    
    __________
    
    See also:   HOpenAware, FSpOpenAware, HOpenRFAware
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSReadNoCache(
  short   refNum,
  long *  count,
  void *  buffPtr);


/*
    The FSReadNoCache function reads any number of bytes from an open file
    while asking the file system to bypass its cache mechanism.
    
    refNum  input:  The file reference number of an open file.
    count   input:  The number of bytes to read.
            output: The number of bytes actually read.
    buffPtr input:  A pointer to the data buffer into which the bytes are
                    to be read.
    
    Result Codes
        noErr               0       No error
        readErr             Ð19     Driver does not respond to read requests
        badUnitErr          Ð21     Driver reference number does not
                                    match unit table
        unitEmptyErr        Ð22     Driver reference number specifies a
                                    nil handle in unit table
        abortErr            Ð27     Request aborted by KillIO
        notOpenErr          Ð28     Driver not open
        ioErr               Ð36     Data does not match in read-verify mode
        fnOpnErr            -38     File not open
        rfNumErr            -51     Bad reference number
        afpAccessDenied     -5000   User does not have the correct access to
                                    the file

    __________
    
    See also:   FSWriteNoCache
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSWriteNoCache(
  short         refNum,
  long *        count,
  const void *  buffPtr);


/*
    The FSReadNoCache function writes any number of bytes to an open file
    while asking the file system to bypass its cache mechanism.
    
    refNum  input:  The file reference number of an open file.
    count   input:  The number of bytes to write to the file.
            output: The number of bytes actually written.
    buffPtr input:  A pointer to the data buffer from which the bytes are
                    to be written.
    
    Result Codes
        noErr               0       No error
        writErr             Ð20     Driver does not respond to write requests
        badUnitErr          Ð21     Driver reference number does not
                                    match unit table
        unitEmptyErr        Ð22     Driver reference number specifies a
                                    nil handle in unit table
        abortErr            Ð27     Request aborted by KillIO
        notOpenErr          Ð28     Driver not open
        dskFulErr           -34     Disk full   
        ioErr               Ð36     Data does not match in read-verify mode
        fnOpnErr            -38     File not open
        wPrErr              -44     Hardware volume lock    
        fLckdErr            -45     File is locked  
        vLckdErr            -46     Software volume lock    
        rfNumErr            -51     Bad reference number
        wrPermErr           -61     Read/write permission doesnÕt
                                    allow writing   
        afpAccessDenied     -5000   User does not have the correct access to
                                    the file

    __________
    
    See also:   FSReadNoCache
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSWriteVerify(
  short         refNum,
  long *        count,
  const void *  buffPtr);


/*
    The FSWriteVerify function writes any number of bytes to an open file
    and then verifies that the data was actually written to the device.
    
    refNum  input:  The file reference number of an open file.
    count   input:  The number of bytes to write to the file.
            output: The number of bytes actually written and verified.
    buffPtr input:  A pointer to the data buffer from which the bytes are
                    to be written.
    
    Result Codes
        noErr               0       No error
        readErr             Ð19     Driver does not respond to read requests
        writErr             Ð20     Driver does not respond to write requests
        badUnitErr          Ð21     Driver reference number does not
                                    match unit table
        unitEmptyErr        Ð22     Driver reference number specifies a
                                    nil handle in unit table
        abortErr            Ð27     Request aborted by KillIO
        notOpenErr          Ð28     Driver not open
        dskFulErr           -34     Disk full   
        ioErr               Ð36     Data does not match in read-verify mode
        fnOpnErr            -38     File not open
        eofErr              -39     Logical end-of-file reached
        posErr              -40     Attempt to position mark before start
                                    of file
        wPrErr              -44     Hardware volume lock    
        fLckdErr            -45     File is locked  
        vLckdErr            -46     Software volume lock    
        rfNumErr            -51     Bad reference number
        gfpErr              -52     Error during GetFPos
        wrPermErr           -61     Read/write permission doesnÕt
                                    allow writing   
        memFullErr          -108    Not enough room in heap zone to allocate
                                    verify buffer
        afpAccessDenied     -5000   User does not have the correct access to
                                    the file
*/

/*****************************************************************************/

EXTERN_API( OSErr )
CopyFork(
  short   srcRefNum,
  short   dstRefNum,
  void *  copyBufferPtr,
  long    copyBufferSize);


/*
    The CopyFork function copies all data from the source fork to the
    destination fork of open file forks and makes sure the destination EOF
    is equal to the source EOF.
    
    srcRefNum       input:  The source file reference number.
    dstRefNum       input:  The destination file reference number.
    copyBufferPtr   input:  Pointer to buffer to use during copy. The
                            buffer should be at least 512-bytes minimum.
                            The larger the buffer, the faster the copy.
    copyBufferSize  input:  The size of the copy buffer.
    
    Result Codes
        noErr               0       No error
        readErr             Ð19     Driver does not respond to read requests
        writErr             Ð20     Driver does not respond to write requests
        badUnitErr          Ð21     Driver reference number does not
                                    match unit table
        unitEmptyErr        Ð22     Driver reference number specifies a
                                    nil handle in unit table
        abortErr            Ð27     Request aborted by KillIO
        notOpenErr          Ð28     Driver not open
        dskFulErr           -34     Disk full   
        ioErr               Ð36     Data does not match in read-verify mode
        fnOpnErr            -38     File not open
        wPrErr              -44     Hardware volume lock    
        fLckdErr            -45     File is locked  
        vLckdErr            -46     Software volume lock    
        rfNumErr            -51     Bad reference number
        wrPermErr           -61     Read/write permission doesnÕt
                                    allow writing   
        afpAccessDenied     -5000   User does not have the correct access to
                                    the file
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetFileLocation(
  short       refNum,
  short *     vRefNum,
  long *      dirID,
  StringPtr   fileName);


/*
    The GetFileLocation function gets the location (volume reference number,
    directory ID, and fileName) of an open file.

    refNum      input:  The file reference number of an open file.
    vRefNum     output: The volume reference number.
    dirID       output: The parent directory ID.
    fileName    input:  Points to a buffer (minimum Str63) where the
                        filename is to be returned or must be nil.
                output: The filename.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     Specified volume doesnÕt exist
        fnOpnErr            -38     File not open
        rfNumErr            -51     Reference number specifies nonexistent
                                    access path
    
    __________
    
    See also:   FSpGetFileLocation
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpGetFileLocation(
  short     refNum,
  FSSpec *  spec);


/*
    The FSpGetFileLocation function gets the location of an open file in
    an FSSpec record.

    refNum      input:  The file reference number of an open file.
    spec        output: FSSpec record containing the file name and location.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     Specified volume doesnÕt exist
        fnOpnErr            -38     File not open
        rfNumErr            -51     Reference number specifies nonexistent
                                    access path
    
    __________
    
    See also:   GetFileLocation
*/

/*****************************************************************************/

EXTERN_API( OSErr )
CopyDirectoryAccess(
  short              srcVRefNum,
  long               srcDirID,
  ConstStr255Param   srcName,
  short              dstVRefNum,
  long               dstDirID,
  ConstStr255Param   dstName);


/*
    The CopyDirectoryAccess function copies the AFP directory access
    privileges from one directory to another. Both directories must be on
    the same file server, but not necessarily on the same server volume.
    
    srcVRefNum  input:  Source volume specification.
    srcDirID    input:  Source directory ID.
    srcName     input:  Pointer to source directory name, or nil when
                        srcDirID specifies the directory.
    dstVRefNum  input:  Destination volume specification.
    dstDirID    input:  Destination directory ID.
    dstName     input:  Pointer to destination directory name, or nil when
                        dstDirID specifies the directory.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     Volume not found
        fnfErr              -43     Directory not found
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     Volume doesn't support this function
        afpAccessDenied     -5000   User does not have the correct access
                                    to the directory
        afpObjectTypeErr    -5025   Object is a file, not a directory
    
    __________
    
    See also:   FSpCopyDirectoryAccess
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpCopyDirectoryAccess(
  const FSSpec *  srcSpec,
  const FSSpec *  dstSpec);


/*
    The FSpCopyDirectoryAccess function copies the AFP directory access
    privileges from one directory to another. Both directories must be on
    the same file server, but not necessarily on the same server volume.

    srcSpec     input:  An FSSpec record specifying the source directory.
    dstSpec     input:  An FSSpec record specifying the destination directory.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     Volume not found
        fnfErr              -43     Directory not found
        vLckdErr            -46     Volume is locked or read-only
        paramErr            -50     Volume doesn't support this function
        afpAccessDenied     -5000   User does not have the correct access
                                    to the directory
        afpObjectTypeErr    -5025   Object is a file, not a directory
    
    __________
    
    See also:   CopyDirectoryAccess
*/

/*****************************************************************************/

EXTERN_API( OSErr )
HMoveRenameCompat(
  short              vRefNum,
  long               srcDirID,
  ConstStr255Param   srcName,
  long               dstDirID,
  ConstStr255Param   dstpathName,
  ConstStr255Param   copyName);


/*
    The HMoveRenameCompat function moves a file or directory and optionally
    renames it.  The source and destination locations must be on the same
    volume. This routine works even if the volume doesn't support MoveRename.
    
    vRefNum     input:  Volume specification.
    srcDirID    input:  Source directory ID.
    srcName     input:  The source object name.
    dstDirID    input:  Destination directory ID.
    dstName     input:  Pointer to destination directory name, or
                        nil when dstDirID specifies a directory.
    copyName    input:  Points to the new name if the object is to be
                        renamed or nil if the object isn't to be renamed.
    
    Result Codes
        noErr               0       No error
        dirFulErr           -33     File directory full
        dskFulErr           -34     Disk is full
        nsvErr              -35     Volume not found
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename or attempt to move into
                                    a file
        fnfErr              -43     Source file or directory not found
        wPrErr              -44     Hardware volume lock
        fLckdErr            -45     File is locked
        vLckdErr            -46     Destination volume is read-only
        fBsyErr             -47     File busy, directory not empty, or
                                    working directory control block open
        dupFNErr            -48     Destination already exists
        paramErr            -50     Volume doesn't support this function,
                                    no default volume, or source and
        volOfflinErr        -53     Volume is offline
        fsRnErr             -59     Problem during rename
        dirNFErr            -120    Directory not found or incomplete pathname
        badMovErr           -122    Attempted to move directory into
                                    offspring
        wrgVolTypErr        -123    Not an HFS volume (it's a MFS volume)
        notAFileErr         -1302   The pathname is nil, the pathname
                                    is empty, or the pathname cannot refer
                                    to a filename
        diffVolErr          -1303   Files on different volumes
        afpAccessDenied     -5000   The user does not have the right to
                                    move the file  or directory
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
        afpSameObjectErr    -5038   Source and destination files are the same
    
    __________
    
    See also:   FSpMoveRenameCompat
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpMoveRenameCompat(
  const FSSpec *     srcSpec,
  const FSSpec *     dstSpec,
  ConstStr255Param   copyName);


/*
    The FSpMoveRenameCompat function moves a file or directory and optionally
    renames it.  The source and destination locations must be on the same
    volume. This routine works even if the volume doesn't support MoveRename.
    
    srcSpec     input:  An FSSpec record specifying the source object.
    dstSpec     input:  An FSSpec record specifying the destination
                        directory.
    copyName    input:  Points to the new name if the object is to be
                        renamed or nil if the object isn't to be renamed.
    
    Result Codes
        noErr               0       No error
        dirFulErr           -33     File directory full
        dskFulErr           -34     Disk is full
        nsvErr              -35     Volume not found
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename or attempt to move into
                                    a file
        fnfErr              -43     Source file or directory not found
        wPrErr              -44     Hardware volume lock
        fLckdErr            -45     File is locked
        vLckdErr            -46     Destination volume is read-only
        fBsyErr             -47     File busy, directory not empty, or
                                    working directory control block open
        dupFNErr            -48     Destination already exists
        paramErr            -50     Volume doesn't support this function,
                                    no default volume, or source and
        volOfflinErr        -53     Volume is offline
        fsRnErr             -59     Problem during rename
        dirNFErr            -120    Directory not found or incomplete pathname
        badMovErr           -122    Attempted to move directory into
                                    offspring
        wrgVolTypErr        -123    Not an HFS volume (it's a MFS volume)
        notAFileErr         -1302   The pathname is nil, the pathname
                                    is empty, or the pathname cannot refer
                                    to a filename
        diffVolErr          -1303   Files on different volumes
        afpAccessDenied     -5000   The user does not have the right to
                                    move the file  or directory
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
        afpSameObjectErr    -5038   Source and destination files are the same
    
    __________
    
    See also:   HMoveRenameCompat
*/

/*****************************************************************************/

EXTERN_API( OSErr )
BuildAFPVolMountInfo(
  short                 flags,
  char                  nbpInterval,
  char                  nbpCount,
  short                 uamType,
  Str32                 zoneName,
  Str31                 serverName,
  Str27                 volName,
  Str31                 userName,
  Str8                  userPassword,
  Str8                  volPassword,
  AFPVolMountInfoPtr *  afpInfoPtr);


/*
    The BuildAFPVolMountInfo function allocates and initializes the fields
    of an AFPVolMountInfo record before using that record to call
    the VolumeMount function.
    
    flags           input:  The AFP mounting flags. 0 = normal mount;
                            set bit 0 to inhibit greeting messages.
    nbpInterval     input:  The interval used for VolumeMount's
                            NBP Lookup call. 7 is a good choice.
    nbpCount        input:  The retry count used for VolumeMount's
                            NBP Lookup call. 5 is a good choice.
    uamType         input:  The user authentication method to use.
    zoneName        input:  The AppleTalk zone name of the server.
    serverName      input:  The AFP server name.
    volName         input:  The AFP volume name.
    userName        input:  The user name (zero length Pascal string for
                            guest).
    userPassWord    input:  The user password (zero length Pascal string
                            if no user password)
    volPassWord     input:  The volume password (zero length Pascal string
                            if no volume password)
    afpInfoPtr      output: A pointer to the newly created and initialized
                            AFPVolMountInfo record. If the function fails to
                            create an AFPVolMountInfo record, it sets
                            afpInfoPtr to NULL and the function result is
                            memFullErr. Your program is responsible
                            for disposing of this pointer when it is finished
                            with it.
    
    Result Codes
        noErr               0       No error
        memFullErr          -108    memory full error
    
    __________
    
    Also see:   GetVolMountInfoSize, GetVolMountInfo, VolumeMount,
                RetrieveAFPVolMountInfo, BuildAFPXVolMountInfo,
                RetrieveAFPXVolMountInfo
*/

/*****************************************************************************/

EXTERN_API( OSErr )
RetrieveAFPVolMountInfo(
  AFPVolMountInfoPtr   afpInfoPtr,
  short *              flags,
  short *              uamType,
  StringPtr            zoneName,
  StringPtr            serverName,
  StringPtr            volName,
  StringPtr            userName);


/*
    The RetrieveAFPVolMountInfo function retrieves the AFP mounting
    information returned in an AFPVolMountInfo record by the
    GetVolMountInfo function.
    
    afpInfoPtr      input:  Pointer to AFPVolMountInfo record that contains
                            the AFP mounting information.
    flags           output: The AFP mounting flags.
    uamType         output: The user authentication method used.
    zoneName        output: The AppleTalk zone name of the server.
    serverName      output: The AFP server name.
    volName         output: The AFP volume name.
    userName        output: The user name (zero length Pascal string for
                            guest).
    
    Result Codes
        noErr               0       No error
        paramErr            -50     media field in AFP mounting information
                                    was not AppleShareMediaType
    
    __________
    
    Also see:   GetVolMountInfoSize, GetVolMountInfo, VolumeMount,
                BuildAFPVolMountInfo, BuildAFPXVolMountInfo,
                RetrieveAFPXVolMountInfo
*/

/*****************************************************************************/

EXTERN_API( OSErr )
BuildAFPXVolMountInfo(
  short                  flags,
  char                   nbpInterval,
  char                   nbpCount,
  short                  uamType,
  Str32                  zoneName,
  Str31                  serverName,
  Str27                  volName,
  Str31                  userName,
  Str8                   userPassword,
  Str8                   volPassword,
  Str32                  uamName,
  unsigned long          alternateAddressLength,
  void *                 alternateAddress,
  AFPXVolMountInfoPtr *  afpXInfoPtr);


/*
    The BuildAFPXVolMountInfo function allocates and initializes the fields
    of an AFPXVolMountInfo record before using that record to call
    the VolumeMount function.
    
    flags                   input:  The AFP mounting flags.
    nbpInterval             input:  The interval used for VolumeMount's
                                    NBP Lookup call. 7 is a good choice.
    nbpCount                input:  The retry count used for VolumeMount's
                                    NBP Lookup call. 5 is a good choice.
    uamType                 input:  The user authentication method to use.
    zoneName                input:  The AppleTalk zone name of the server.
    serverName              input:  The AFP server name.
    volName                 input:  The AFP volume name.
    userName                input:  The user name (zero length Pascal string
                                    for guest).
    userPassWord            input:  The user password (zero length Pascal
                                    string if no user password)
    volPassWord             input:  The volume password (zero length Pascal
                                    string if no volume password)
    uamName                 input:  The User Authentication Method name.
    alternateAddressLength  input:  Length of alternateAddress data.
    alternateAddress        input   The AFPAlternateAddress (variable length)
    afpXInfoPtr             output: A pointer to the newly created and
                                    initialized AFPVolMountInfo record.
                                    If the function fails to create an
                                    AFPVolMountInfo record, it sets
                                    afpInfoPtr to NULL and the function
                                    result is memFullErr. Your program is
                                    responsible for disposing of this pointer
                                    when it is finished with it.
    
    Result Codes
        noErr               0       No error
        memFullErr          -108    memory full error
    
    __________
    
    Also see:   GetVolMountInfoSize, GetVolMountInfo, VolumeMount,
                BuildAFPVolMountInfo, RetrieveAFPVolMountInfo,
                RetrieveAFPXVolMountInfo
*/

/*****************************************************************************/

EXTERN_API( OSErr )
RetrieveAFPXVolMountInfo(
  AFPXVolMountInfoPtr     afpXInfoPtr,
  short *                 flags,
  short *                 uamType,
  StringPtr               zoneName,
  StringPtr               serverName,
  StringPtr               volName,
  StringPtr               userName,
  StringPtr               uamName,
  unsigned long *         alternateAddressLength,
  AFPAlternateAddress **  alternateAddress);


/*
    The RetrieveAFPXVolMountInfo function retrieves the AFP mounting
    information returned in an AFPXVolMountInfo record by the
    GetVolMountInfo function.
    
    afpXInfoPtr             input:  Pointer to AFPXVolMountInfo record that
                                    contains the AFP mounting information.
    flags                   output: The AFP mounting flags.
    uamType                 output: The user authentication method used.
    zoneName                output: The AppleTalk zone name of the server.
    serverName              output: The AFP server name.
    volName                 output: The AFP volume name.
    userName                output: The user name (zero length Pascal
                                    string for guest).
    uamName                 output: The User Authentication Method name.
    alternateAddressLength  output: Length of alternateAddress data returned.
    alternateAddress:       output: A pointer to the newly created and
                                    AFPAlternateAddress record (a variable
                                    length record). If the function fails to
                                    create an AFPAlternateAddress record,
                                    it sets alternateAddress to NULL and the
                                    function result is memFullErr. Your
                                    program is responsible for disposing of
                                    this pointer when it is finished with it.
    
    Result Codes
        noErr               0       No error
        paramErr            -50     media field in AFP mounting information
                                    was not AppleShareMediaType
        memFullErr          -108    memory full error
    
    __________
    
    Also see:   GetVolMountInfoSize, GetVolMountInfo, VolumeMount,
                BuildAFPVolMountInfo, RetrieveAFXVolMountInfo,
                BuildAFPXVolMountInfo
*/

/*****************************************************************************/

EXTERN_API( OSErr )
GetUGEntries(
  short        objType,
  UGEntryPtr   entries,
  long         reqEntryCount,
  long *       actEntryCount,
  long *       objID);


/*
    The GetUGEntries functions retrieves a list of user or group entries
    from the local file server.

    objType         input:  The object type: -1 = group; 0 = user
    UGEntries       input:  Pointer to array of UGEntry records where the list
                            is returned.
    reqEntryCount   input:  The number of elements in the UGEntries array.
    actEntryCount   output: The number of entries returned.
    objID           input:  The current index position. Set to 0 to start with
                            the first entry.
                    output: The index position to get the next entry. Pass this
                            value the next time you call GetUGEntries to start
                            where you left off.
    
    Result Codes
        noErr               0       No error    
        fnfErr              -43     No more users or groups 
        paramErr            -50     Function not supported; or, ioObjID is
                                    negative    

    __________
    
    Also see:   GetUGEntry
*/

/*****************************************************************************/



#include "OptimizationEnd.h"

#if PRAGMA_STRUCT_ALIGN
    #pragma options align=reset
#elif PRAGMA_STRUCT_PACKPUSH
    #pragma pack(pop)
#elif PRAGMA_STRUCT_PACK
    #pragma pack()
#endif

#ifdef PRAGMA_IMPORT_OFF
#pragma import off
#elif PRAGMA_IMPORT
#pragma import reset
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MOREFILESEXTRAS__ */

