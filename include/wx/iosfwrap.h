///////////////////////////////////////////////////////////////////////////////
// Name:        wx/iosfwrap.h
// Purpose:     includes the correct stream-related forward declarations
// Author:      Jan van Dijk
// Modified by:
// Created:     18.12.2002
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Jan van Dijk <jan@etpmod.phys.tue.nl>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#if wxUSE_STD_IOSTREAM

#if wxUSE_IOSTREAMH
    // There is no pre-ANSI iosfwd header so we include the full declarations.
#   include <iostream.h>
#else
#   include <iosfwd>
#endif

#endif // wxUSE_STD_IOSTREAM

