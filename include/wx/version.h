/**
*  Name:        wx/version.h
*  Purpose:     wxWidgets version numbers
*  Author:      Julian Smart
*  Modified by: Ryan Norton (Converted to C)
*  Created:     29/01/98
*  RCS-ID:      $Id$
*  Copyright:   (c) 1998 Julian Smart
*  Licence:     wxWindows licence
*/

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_VERSION_H_
#define _WX_VERSION_H_

/*  the constants below must be changed with each new version */
/*  ---------------------------------------------------------------------------- */

/*  NB: this file is parsed by Perl code in tmake templates in distrib/msw/tmake */
/*      so don't change its format too much or they could break */
#define wxMAJOR_VERSION      2
#define wxMINOR_VERSION      5
#define wxRELEASE_NUMBER     3
#define wxSUBRELEASE_NUMBER  1
#define wxVERSION_STRING   _T("wxWidgets 2.5.3")

/*  nothing to update below this line when updating the version */
/*  ---------------------------------------------------------------------------- */

/*  helpers for wxVERSION_NUM_XXX */
#define wxMAKE_VERSION_STRING(x, y, z) #x #y #z
#define wxMAKE_VERSION_DOT_STRING(x, y, z) #x "." #y "." #z

/*  these are used by src/msw/version.rc and should always be ASCII, not Unicode */
#define wxVERSION_NUM_STRING \
  wxMAKE_VERSION_STRING(wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER)
#define wxVERSION_NUM_DOT_STRING \
  wxMAKE_VERSION_DOT_STRING(wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER)

/*  some more defines, not really sure if they're [still] useful */
#define wxVERSION_NUMBER (wxMAJOR_VERSION * 1000) + (wxMINOR_VERSION * 100) + wxRELEASE_NUMBER
#define wxBETA_NUMBER      0
#define wxVERSION_FLOAT wxMAJOR_VERSION + (wxMINOR_VERSION/10.0) + (wxRELEASE_NUMBER/100.0) + (wxBETA_NUMBER/10000.0)

/*  check if the current version is at least major.minor.release */
#define wxCHECK_VERSION(major,minor,release) \
    (wxMAJOR_VERSION > (major) || \
    (wxMAJOR_VERSION == (major) && wxMINOR_VERSION > (minor)) || \
    (wxMAJOR_VERSION == (major) && wxMINOR_VERSION == (minor) && wxRELEASE_NUMBER >= (release)))

/* the same but check the subrelease also */
#define wxCHECK_VERSION_FULL(major,minor,release,subrel) \
    wxCHECK_VERSION(major, minor, release) && \
        ((major) != wxMAJOR_VERSION || \
            (minor) != wxMINOR_VERSION || \
                (release) != wxRELEASE_NUMBER || \
                    (subrel) <= wxSUBRELEASE_NUMBER)

#endif /*  _WX_VERSION_H_ */

