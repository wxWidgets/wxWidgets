
//----------------------------------------------------------------------
//
// For MSW I keep my own copy of the glcanvas code.  This lets me build
// the main wxWindows library without OpenGL support and the DLL
// depenencies that go along with it.  The DLL dependencies will then
// be localized to this extension module, will not need to be loaded
// when the core is started up, and won't make the core unrunnable on
// systems that don't have OpenGL.
//
//----------------------------------------------------------------------

#if defined(__WXMSW__)
#include "msw/myglcanvas.h"
#else
#include <wx/glcanvas.h>
#endif
