///////////////////////////////////////////////////////////////////////////////
// Name:        wx/ribbon/control.h
// Purpose:     Extension of wxControl with common ribbon methods
// Author:      Peter Cawley
// Modified by:
// Created:     2009-06-05
// RCS-ID:      $Id$
// Copyright:   (C) Peter Cawley
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RIBBON_CONTROL_H_
#define _WX_RIBBON_CONTROL_H_

#include "wx/defs.h"

#if wxUSE_RIBBON

#include "wx/control.h"

class wxRibbonArtProvider;

class WXDLLIMPEXP_RIBBON wxRibbonControl : public wxControl
{
public:
	wxRibbonControl() { m_art = NULL; }

    wxRibbonControl(wxWindow *parent, wxWindowID id,
					const wxPoint& pos = wxDefaultPosition,
					const wxSize& size = wxDefaultSize, long style = 0,
					const wxValidator& validator = wxDefaultValidator,
					const wxString& name = wxControlNameStr)
	: wxControl(parent, id, pos, size, style, validator, name)
    {
		m_art = NULL;
    }

	virtual void SetArtProvider(wxRibbonArtProvider* art);
	wxRibbonArtProvider* GetArtProvider() const {return m_art;}

	virtual bool IsSizingContinuous() const {return true;}
	virtual wxSize GetNextSmallerSize(wxOrientation direction) const;
	virtual wxSize GetNextLargerSize(wxOrientation direction) const;

protected:
	wxRibbonArtProvider* m_art;

#ifndef SWIG
	DECLARE_CLASS(wxRibbonControl)
#endif
};

#endif // wxUSE_RIBBON

#endif // _WX_RIBBON_CONTROL_H_
