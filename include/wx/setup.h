
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
#  else
#    error No __WXxxx__ define set! Please define __WXGTK__,__WXMSW__ or __WXMOTIF__.
#  endif
#endif

#if defined(__WXMSW__)
#include "wx/msw/setup.h"
#elif defined(__WXMOTIF__)
#include "wx/xt/setup.h"
#elif defined(__WXGTK__)
#include "wx/gtk/setup.h"
#endif

#endif
	// __SETUPH_BASE__
