/*
 * Name:        wx/motif/chkconf.h
 * Purpose:     Motif-specific config settings checks
 * Author:      Vadim Zeitlin
 * Modified by:
 * Created:     2005-04-05 (extracted from wx/chkconf.h)
 * RCS-ID:      $Id$
 * Copyright:   (c) 2005 Vadim Zeitlin <vadim@wxwidgets.org>
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#if wxUSE_UNICODE
#   ifdef wxABORT_ON_CONFIG_ERROR
#       error "wxUSE_UNICODE is not supported with wxMotif"
#   else
#       define wxUSE_UNICODE 0
#   endif
#endif

#if wxUSE_NOTEBOOK && !wxUSE_TAB_DIALOG
#   undef wxUSE_TAB_DIALOG
#   define wxUSE_TAB_DIALOG 1
#endif

