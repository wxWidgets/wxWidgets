///////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/crt.h
// Purpose:     Replacements for the POSIX functions missing in Retro68
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_CRT_H_
#define _WX_RETRO68_CRT_H_

#include "wx/dlimpexp.h"

#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

struct utimbuf;

// Retro68 runtime only implements open(), read(), write(), lseek() and close()
// using the File Manager, all the other file-related functions are either not
// available at all or always fail. These functions implement them using the
// File Manager too, they take paths in Mac format (i.e. using ':' as path
// separator) and in the system encoding and behave as their POSIX equivalents.
WXDLLIMPEXP_BASE int wxRetro68Stat(const char* path, struct stat* st);
WXDLLIMPEXP_BASE int wxRetro68MkDir(const char* path, mode_t mode);
WXDLLIMPEXP_BASE int wxRetro68RmDir(const char* path);
WXDLLIMPEXP_BASE int wxRetro68Access(const char* path, int mode);
WXDLLIMPEXP_BASE int wxRetro68Chmod(const char* path, mode_t mode);
WXDLLIMPEXP_BASE int wxRetro68Remove(const char* path);
WXDLLIMPEXP_BASE int wxRetro68Rename(const char* oldpath, const char* newpath);
WXDLLIMPEXP_BASE int wxRetro68Utime(const char* path, const struct utimbuf* times);
WXDLLIMPEXP_BASE char* wxRetro68GetCwd(char* buf, size_t size);
WXDLLIMPEXP_BASE int wxRetro68ChDir(const char* path);
WXDLLIMPEXP_BASE int wxRetro68Eof(int fd);

#endif // _WX_RETRO68_CRT_H_
