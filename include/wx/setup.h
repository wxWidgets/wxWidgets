
#ifndef __SETUPH_BASE__
#define __SETUPH_BASE__

#if defined(__WINDOWS__)
#include "wx/msw/setup.h"
#elif defined(__MOTIF__)
#include "wx/xt/setup.h"
#elif defined(__GTK__)
#include "wx/gtk/setup.h"
#endif

#endif
	// __SETUPH_BASE__
