
#ifndef __SETUPH_BASE__
#define __SETUPH_BASE__

// compatibility code, to be removed asap:

#if ! defined(__WXMSW__) && ! defined(__WXGTK__) && ! defined(__WXMOTIF__)
#  if defined(__WINDOWS__)
#    define __WXMSW__
#  elif defined(__GTK__)
#    define __WXGTK__
#  elif defined(__MOTIF__)
#    define __WXMOTIF__
#  elif defined(__QT__)
#    define __WXQT__
#  else
#    error No __WXxxx__ define set! Please define __WXGTK__,__WXMSW__ or __WXMOTIF__.
#  endif
#endif

#if defined(__WXMSW__)
#include "wx/msw/setup.h"
#else
#include "wx/../../install/unix/setup/setup.h"
#endif

#endif
	// __SETUPH_BASE__
