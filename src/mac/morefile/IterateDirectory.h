/*
     File:       IterateDirectory.h
 
     Contains:   File Manager directory iterator routines.
 
     Version:    Technology: MoreFiles
                 Release:    1.5.2
 
     Copyright:  © 1995-2001 by Jim Luther and Apple Computer, Inc., all rights reserved.
 
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

#ifndef __ITERATEDIRECTORY__
#define __ITERATEDIRECTORY__

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

typedef CALLBACK_API( void , IterateFilterProcPtr )(const CInfoPBRec *cpbPtr, Boolean *quitFlag, void *yourDataPtr);
/*
    This is the prototype for the IterateFilterProc function which is
    called once for each file and directory found by IterateDirectory. The
    IterateFilterProc gets a pointer to the CInfoPBRec that IterateDirectory
    used to call PBGetCatInfo. The IterateFilterProc can use the read-only
    data in the CInfoPBRec for whatever it wants.
    
    If the IterateFilterProc wants to stop IterateDirectory, it can set
    quitFlag to true (quitFlag will be passed to the IterateFilterProc
    false).
    
    The yourDataPtr parameter can point to whatever data structure you might
    want to access from within the IterateFilterProc.

    cpbPtr      input:  A pointer to the CInfoPBRec that IterateDirectory
                        used to call PBGetCatInfo. The CInfoPBRec and the
                        data it points to must not be changed by your
                        IterateFilterProc.
    quitFlag    output: Your IterateFilterProc can set quitFlag to true
                        if it wants to stop IterateDirectory.
    yourDataPtr input:  A pointer to whatever data structure you might
                        want to access from within the IterateFilterProc.
    
    __________
    
    Also see:   IterateDirectory, FSpIterateDirectory
*/
#define CallIterateFilterProc(userRoutine, cpbPtr, quitFlag, yourDataPtr) \
    (*(userRoutine))((cpbPtr), (quitFlag), (yourDataPtr))

/*****************************************************************************/

EXTERN_API( OSErr )
IterateDirectory(
  short                  vRefNum,
  long                   dirID,
  ConstStr255Param       name,
  unsigned short         maxLevels,
  IterateFilterProcPtr   iterateFilter,
  void *                 yourDataPtr);


/*
    The IterateDirectory function performs a recursive iteration (scan) of
    the specified directory and calls your IterateFilterProc function once
    for each file and directory found.
    
    The maxLevels parameter lets you control how deep the recursion goes.
    If maxLevels is 1, IterateDirectory only scans the specified directory;
    if maxLevels is 2, IterateDirectory scans the specified directory and
    one subdirectory below the specified directory; etc. Set maxLevels to
    zero to scan all levels.
    
    The yourDataPtr parameter can point to whatever data structure you might
    want to access from within the IterateFilterProc.

    vRefNum         input:  Volume specification.
    dirID           input:  Directory ID.
    name            input:  Pointer to object name, or nil when dirID
                            specifies a directory that's the object.
    maxLevels       input:  Maximum number of directory levels to scan or
                            zero to scan all directory levels.
    iterateFilter   input:  A pointer to the routine you want called once
                            for each file and directory found by
                            IterateDirectory.
    yourDataPtr     input:  A pointer to whatever data structure you might
                            want to access from within the IterateFilterProc.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume or iterateFilter was NULL
        dirNFErr            -120    Directory not found or incomplete pathname
                                    or a file was passed instead of a directory
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
        
    __________
    
    See also:   IterateFilterProcPtr, FSpIterateDirectory
*/

/*****************************************************************************/

EXTERN_API( OSErr )
FSpIterateDirectory(
  const FSSpec *         spec,
  unsigned short         maxLevels,
  IterateFilterProcPtr   iterateFilter,
  void *                 yourDataPtr);


/*
    The FSpIterateDirectory function performs a recursive iteration (scan)
    of the specified directory and calls your IterateFilterProc function once
    for each file and directory found.
    
    The maxLevels parameter lets you control how deep the recursion goes.
    If maxLevels is 1, FSpIterateDirectory only scans the specified directory;
    if maxLevels is 2, FSpIterateDirectory scans the specified directory and
    one subdirectory below the specified directory; etc. Set maxLevels to
    zero to scan all levels.
    
    The yourDataPtr parameter can point to whatever data structure you might
    want to access from within the IterateFilterProc.

    spec            input:  An FSSpec record specifying the directory to scan.
    maxLevels       input:  Maximum number of directory levels to scan or
                            zero to scan all directory levels.
    iterateFilter   input:  A pointer to the routine you want called once
                            for each file and directory found by
                            FSpIterateDirectory.
    yourDataPtr     input:  A pointer to whatever data structure you might
                            want to access from within the IterateFilterProc.
    
    Result Codes
        noErr               0       No error
        nsvErr              -35     No such volume
        ioErr               -36     I/O error
        bdNamErr            -37     Bad filename
        fnfErr              -43     File not found
        paramErr            -50     No default volume or iterateFilter was NULL
        dirNFErr            -120    Directory not found or incomplete pathname
        afpAccessDenied     -5000   User does not have the correct access
        afpObjectTypeErr    -5025   Directory not found or incomplete pathname
        
    __________
    
    See also:   IterateFilterProcPtr, IterateDirectory
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

#endif /* __ITERATEDIRECTORY__ */

