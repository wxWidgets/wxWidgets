/*
     File:       FullPath.h
 
     Contains:   Routines for dealing with full pathnames... if you really must.
 
     Version:    Technology: MoreFiles
                 Release:    1.5.2
 
     Copyright:  © 1995-2001 by Apple Computer, Inc., all rights reserved.
 
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

/*
    IMPORTANT NOTE:
    
    The use of full pathnames is strongly discouraged. Full pathnames are
    particularly unreliable as a means of identifying files, directories
    or volumes within your application, for two primary reasons:
    
    •   The user can change the name of any element in the path at
        virtually any time.
    •   Volume names on the Macintosh are *not* unique. Multiple
        mounted volumes can have the same name. For this reason, the use of
        a full pathname to identify a specific volume may not produce the
        results you expect. If more than one volume has the same name and
        a full pathname is used, the File Manager currently uses the first
        mounted volume it finds with a matching name in the volume queue.
    
    In general, you should use a file’s name, parent directory ID, and
    volume reference number to identify a file you want to open, delete,
    or otherwise manipulate.
    
    If you need to remember the location of a particular file across
    subsequent system boots, use the Alias Manager to create an alias
    record describing the file. If the Alias Manager is not available, you
    can save the file’s name, its parent directory ID, and the name of the
    volume on which it’s located. Although none of these methods is
    foolproof, they are much more reliable than using full pathnames to
    identify files.
    
    Nonetheless, it is sometimes useful to display a file’s full pathname
    to the user. For example, a backup utility might display a list of full
    pathnames of files as it copies them onto the backup medium. Or, a
    utility might want to display a dialog box showing the full pathname of
    a file when it needs the user’s confirmation to delete the file. No
    matter how unreliable full pathnames may be from a file-specification
    viewpoint, users understand them more readily than volume reference
    numbers or directory IDs. (Hint: Use the TruncString function from
    TextUtils.h with truncMiddle as the truncWhere argument to shorten
    full pathnames to a displayable length.)
    
    The following technique for constructing the full pathname of a file is
    intended for display purposes only. Applications that depend on any
    particular structure of a full pathname are likely to fail on alternate
    foreign file systems or under future system software versions.
*/

#ifndef __FULLPATH__
#define __FULLPATH__

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

EXTERN_API( OSErr )
GetFullPath(
  short              vRefNum,
  long               dirID,
  ConstStr255Param   name,
  short *            fullPathLength,
  Handle *           fullPath);


/*
    The GetFullPath function builds a full pathname to the specified
    object. The full pathname is returned in the newly created handle
    fullPath and the length of the full pathname is returned in
    fullPathLength. Your program is responsible for disposing of the
    fullPath handle.
    
    Note that a full pathname can be made to a file/directory that does not
    yet exist if all directories up to that file/directory exist. In this case,
    GetFullPath will return a fnfErr.
    
    vRefNum         input:  Volume specification.
    dirID           input:  Directory ID.
    name            input:  Pointer to object name, or nil when dirID
                            specifies a directory that's the object.
    fullPathLength  output: The number of characters in the full pathname.
                            If the function fails to create a full
                            pathname, it sets fullPathLength to 0.
    fullPath        output: A handle to the newly created full pathname
                            buffer. If the function fails to create a
                            full pathname, it sets fullPath to NULL.
    
    Result Codes
        noErr               0       No error    
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File or directory does not exist (fullPath
                                    and fullPathLength are still valid)
        paramErr            -50     No default volume
        memFullErr          -108    Not enough memory
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   FSpGetFullPath
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpGetFullPath(
  const FSSpec *  spec,
  short *         fullPathLength,
  Handle *        fullPath);


/*
    The GetFullPath function builds a full pathname to the specified
    object. The full pathname is returned in the newly created handle
    fullPath and the length of the full pathname is returned in
    fullPathLength. Your program is responsible for disposing of the
    fullPath handle.
    
    Note that a full pathname can be made to a file/directory that does not
    yet exist if all directories up to that file/directory exist. In this case,
    FSpGetFullPath will return a fnfErr.
    
    IMPORTANT: The definition of a FSSpec is a volume reference number (not a
    drive number, working directory number, or 0), a parent directory ID (not 0),
    and the name of a file or folder (not an empty name, a full pathname, or
    a partial pathname containing one or more colon (:) characters).
    FSpGetFullPath assumes it is getting a FSSpec that matches the rules.
    If you have an FSSpec record that wasn't created by FSMakeFSSpec (or
    FSMakeFSSpecCompat from FSpCompat in MoreFiles which correctly builds
    FSSpecs), you should call GetFullPath instead of FSpGetFullPath.
    
    spec            input:  An FSSpec record specifying the object.
    fullPathLength  output: The number of characters in the full pathname.
                            If the function fails to create a full pathname,
                            it sets fullPathLength to 0.
    fullPath        output: A handle to the newly created full pathname
                            buffer. If the function fails to create a
                            full pathname, it sets fullPath to NULL.
    
    Result Codes
        noErr               0       No error    
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File or directory does not exist (fullPath
                                    and fullPathLength are still valid)
        paramErr            -50     No default volume
        memFullErr          -108    Not enough memory
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
    
    __________
    
    See also:   GetFullPath
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpLocationFromFullPath(
  short         fullPathLength,
  const void *  fullPath,
  FSSpec *      spec);


/*
    The FSpLocationFromFullPath function returns a FSSpec to the object
    specified by full pathname. This function requires the Alias Manager.
    
    fullPathLength  input:  The number of characters in the full pathname
                            of the target.
    fullPath        input:  A pointer to a buffer that contains the full
                            pathname of the target. The full pathname
                            starts with the name of the volume, includes
                            all of the directory names in the path to the
                            target, and ends with the target name.
    spec            output: An FSSpec record specifying the object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     The volume is not mounted
        fnfErr              -43     Target not found, but volume and parent
                                    directory found
        paramErr            -50     Parameter error
        usrCanceledErr      -128    The user canceled the operation
    
    __________
    
    See also:   LocationFromFullPath
*/

/*****************************************************************************/

EXTERN_API( OSErr )
LocationFromFullPath(
  short         fullPathLength,
  const void *  fullPath,
  short *       vRefNum,
  long *        parID,
  Str31         name);


/*
    The LocationFromFullPath function returns the volume reference number,
    parent directory ID and name of the object specified by full pathname.
    This function requires the Alias Manager.
    
    fullPathLength  input:  The number of characters in the full pathname
                            of the target.
    fullPath        input:  A pointer to a buffer that contains the full
                            pathname of the target. The full pathname starts
                            with the name of the volume, includes all of
                            the directory names in the path to the target,
                            and ends with the target name.
    vRefNum         output: The volume reference number.
    parID           output: The parent directory ID of the specified object.
    name            output: The name of the specified object.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     The volume is not mounted
        fnfErr              -43     Target not found, but volume and parent
                                    directory found
        paramErr            -50     Parameter error
        usrCanceledErr      -128    The user canceled the operation
    
    __________
    
    See also:   FSpLocationFromFullPath
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

#endif /* __FULLPATH__ */

