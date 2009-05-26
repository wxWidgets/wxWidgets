///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/art.h
// Purpose:     Art providers for ribbon-bar-style interface
// Author:      Peter Cawley
// Modified by:
// Created:     2009-05-25
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_ART_H_
#define _WX_RIBBON_ART_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

class WXDLLIMPEXP_AUI wxRibbonArtProvider
{
public:
	wxRibbonArtProvider() { }
	virtual ~wxRibbonArtProvider() { }

	virtual wxRibbonArtProvider* Clone() = 0;
    virtual void SetFlags(unsigned int flags) = 0;
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_ART_H_
