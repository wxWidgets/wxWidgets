/////////////////////////////////////////////////////////////////////////////
// Name:        forcelnk.h
// Purpose:     see bellow
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*

DESCRPITON:

mod_*.cpp files contain handlers for tags. These files are modules - they contain
one wxTagModule class and it's OnInit() method is called from wxApp's init method.
The module is called even if you only link it into the executable, so everything
seems wonderful.

The problem is that we have these modules in LIBRARY and mod_*.cpp files contain
no method nor class which is known out of the module. So the linker won't
link these .o/.obj files into executable because it detected that it is not used
by the program.

To workaround this I introduced set of macros FORCE_LINK_ME and FORCE_LINK. These
macros are generic and are not limited to mod_*.cpp files. You may find them quite
useful somewhere else...

How to use them:
let's suppose you want to always link file foo.cpp and that you have module
always.cpp that is certainly always linked (e.g. the one with main() function
or htmlwin.cpp in wxHtml library).

Place FORCE_LINK_ME(foo) somewhere in foo.cpp and FORCE_LINK(foo) somewhere
in always.cpp
See mod_*.cpp and htmlwin.cpp for example :-)

*/


#ifndef _WX_FORCELNK_H_
#define _WX_FORCELNK_H_



// This must be part of the module you want to force:
#define FORCE_LINK_ME(module_name)                                    \
                int _link_dummy_func_##module_name ()                 \
                {                                                     \
                    return 1;                                         \
                }


// And this must be somewhere where it certainly will be linked:
#define FORCE_LINK(module_name)                                       \
                extern int _link_dummy_func_##module_name ();         \
                static int _link_dummy_var_##module_name =            \
                               _link_dummy_func_##module_name ();


#endif // _WX_FORCELNK_H_
