///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mahogany/art.h
// Purpose:     Art providers for ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MAHOGANY_ART_H_
#define _WX_MAHOGANY_ART_H_

#include "wx/defs.h"

#if wxUSE_MAHOGANY

class WXDLLIMPEXP_AUI wxMahoganyArtProvider
{
public:
	wxMahoganyArtProvider() { }
	virtual ~wxMahoganyArtProvider() { }

	virtual wxMahoganyArtProvider* Clone() = 0;
    virtual void SetFlags(unsigned int flags) = 0;
};

#endif // wxUSE_MAHOGANY

#endif // _WX_MAHOGANY_ART_H_
