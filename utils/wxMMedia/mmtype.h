// /////////////////////////////////////////////////////////////////////////////
// Name:       mmtype.h
// Purpose:    wxMMedia (imported from wxSocket)
// Author:     Guilhem Lavaux
// Created:    1997
// Updated:    1998
// Copyright:  (C) 1997, 1998, Guilhem Lavaux
// License:    wxWindows license
// /////////////////////////////////////////////////////////////////////////////

#ifndef __SOCKTYPEH__
#define __SOCKTYPEH__

#ifdef __UNIX__
#include <stl_config.h>
#endif

/// Type to use for 8 bits unsigned integers
typedef unsigned char wxUint8;
/// Type to use for 16 bits unsigned integers
typedef unsigned short wxUint16;
/// Type to use for 32 bits unsigned integers
typedef unsigned long wxUint32;
#if HAVE_UINT64
/// Type to use for 64 bits unsigned integers
typedef unsigned long long wxUint64;
#endif

/// Type to use for 8 bits signed integers
typedef char wxInt8;
/// Type to use for 16 bits signed integers
typedef short wxInt16;
/// Type to use for 32 bits signed integers
typedef long wxInt32;
#if HAVE_UINT64
/// Type to use for 64 bits signed integers
typedef long long wxInt64;
#endif

#endif

