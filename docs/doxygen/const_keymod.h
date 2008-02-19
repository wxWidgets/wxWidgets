/////////////////////////////////////////////////////////////////////////////
// Name:        keymod.h
// Purpose:     key modifiers
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


/*!

 @page keymodifiers Key Modifiers
 
 @header{wx/defs.h}
 
 The following key modifier constants are defined:
 
 @verbatim
     enum wxKeyModifier
     {
         wxMOD_NONE      = 0x0000,
         wxMOD_ALT       = 0x0001,
         wxMOD_CONTROL   = 0x0002,
         wxMOD_ALTGR     = wxMOD_ALT | wxMOD_CONTROL,
         wxMOD_SHIFT     = 0x0004,
         wxMOD_META      = 0x0008,
     #if defined(__WXMAC__) || defined(__WXCOCOA__)
         wxMOD_CMD       = wxMOD_META,
     #else
         wxMOD_CMD       = wxMOD_CONTROL,
     #endif
         wxMOD_ALL       = 0xffff
     };
 @endverbatim
 
 Notice that @c wxMOD_CMD should be used instead of @c wxMOD_CONTROL 
 in portable code to account for the fact that although 
 @c Control modifier exists under Mac OS, it is not used for the same
 purpose as under Windows or Unix there while the special Mac-specific 
 @c Command modifier is used in exactly the same way.

*/
