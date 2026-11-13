///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/private.h
// Purpose:     Private helpers for the classic Mac OS (Retro68) code
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_PRIVATE_H_
#define _WX_RETRO68_PRIVATE_H_

#include "wx/string.h"

// Multiverse.h contains the declarations of all Toolbox functions, both for
// 68K and PowerPC (and Carbon).
#include <Multiverse.h>

// Some constants missing from Multiverse.h.
enum
{
    wxMAC_FS_ROOT_PARENT_ID = 1,        // fsRtParID
    wxMAC_FS_ROOT_DIR_ID    = 2,        // fsRtDirID

    wxMAC_IOFL_ATTRIB_LOCKED = 0x01,    // kioFlAttribLockedMask
    wxMAC_IOFL_ATTRIB_DIR    = 0x10,    // kioFlAttribDirMask

    wxMAC_ON_SYSTEM_DISK    = -32768    // kOnSystemDisk
};

// Number of seconds between the Mac (1904-01-01) and Unix (1970-01-01) epochs.
//
// Note that Mac OS uses local time for all the time stamps and so does the
// Retro68 runtime when implementing time(), so we don't adjust for the time
// zone neither.
#define wxMAC_EPOCH_OFFSET 2082844800UL

// ----------------------------------------------------------------------------
// String helpers
// ----------------------------------------------------------------------------

// Convert wxString to a Pascal string, truncating it to 255 characters (and
// using '?' for the characters not representable in Mac Roman encoding).
WXDLLIMPEXP_BASE void wxMacStringToPascal(const wxString& from, StringPtr to);

// Convert a Pascal string in system (Mac Roman) encoding to wxString.
WXDLLIMPEXP_BASE wxString wxMacMakeStringFromPascal(ConstStringPtr from);

// Convert a C string to a Pascal one without any encoding conversion, return
// false if it is too long.
WXDLLIMPEXP_BASE bool wxMacCToPascal(const char* from, StringPtr to);

// ----------------------------------------------------------------------------
// File system helpers
// ----------------------------------------------------------------------------

// Create FSSpec from the given (full or partial) path in Mac format, already
// in the system encoding.
//
// Note that, as with FSMakeFSSpec(), the returned FSSpec is valid even if
// fnfErr is returned, which just means that the object doesn't exist yet.
WXDLLIMPEXP_BASE OSErr wxMacPathToFSSpec(const char* path, FSSpec* spec);
WXDLLIMPEXP_BASE OSErr wxMacPathToFSSpec(const wxString& path, FSSpec* spec);

// Return the full path, in Mac format, corresponding to the given directory
// or file, empty string on error.
WXDLLIMPEXP_BASE wxString wxMacFSSpecToPath(const FSSpec& spec);
WXDLLIMPEXP_BASE wxString wxMacDirToPath(short vRefNum, long dirID);

// Get information about the file or directory, return noErr on success.
//
// The name buffer must remain valid for as long as pb is used.
WXDLLIMPEXP_BASE OSErr wxMacGetCatInfo(const FSSpec& spec,
                                       CInfoPBRec& pb,
                                       Str255 name);

// Set errno according to the given OSErr and return -1 (or 0 if there is no
// error).
WXDLLIMPEXP_BASE int wxMacSetErrno(OSErr err);

// Return the location of the given standard folder (one of k*FolderType
// constants) on the system disk, possibly creating it.
WXDLLIMPEXP_BASE wxString wxMacFindFolder(OSType folderType, bool create = true);

// Return the FSSpec of the currently running application.
WXDLLIMPEXP_BASE bool wxMacGetApplicationSpec(FSSpec* spec);

// Return the result of Gestalt() for the given selector or 0 if it failed.
WXDLLIMPEXP_BASE long wxMacGestalt(OSType selector);

#endif // _WX_RETRO68_PRIVATE_H_
