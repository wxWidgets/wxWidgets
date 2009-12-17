/*
 * Name:        wx/symbian/chkconf.h
 * Purpose:     Symbian-specific config settings consistency checks
 * Author:      Andrei Matuk
 * RCS-ID:      $Id$
 * Copyright:   (c) 2009 Andrei Matuk <Veon.UA@gmail.com>
 * Licence:     wxWindows licence
 */

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#undef wxUSE_GUI
#define wxUSE_GUI 0

#undef wxUSE_WXHTML_HELP
#define wxUSE_WXHTML_HELP 0

#undef wxUSE_ON_FATAL_EXCEPTION
#define wxUSE_ON_FATAL_EXCEPTION 0

#undef wxUSE_CRASHREPORT
#define wxUSE_CRASHREPORT 0

#undef wxUSE_STACKWALKER
#define wxUSE_STACKWALKER 0

#undef wxUSE_ACCEL
#define wxUSE_ACCEL 0

#undef wxUSE_TOOLTIPS
#define wxUSE_TOOLTIPS 0

#undef HAVE_SYS_SELECT_H
#define HAVE_SYS_SELECT_H 1

#if wxUSE_CONSOLE_EVENTLOOP
#   if !wxUSE_SELECT_DISPATCHER && !wxUSE_EPOLL_DISPATCHER
/*#       ifdef wxABORT_ON_CONFIG_ERROR
  #           error "wxSelect/EpollDispatcher needed for console event loop"
  #       else*/
#           undef wxUSE_SELECT_DISPATCHER
#           define wxUSE_SELECT_DISPATCHER 1
/*#       endif*/
#   endif
#endif /* wxUSE_CONSOLE_EVENTLOOP */


