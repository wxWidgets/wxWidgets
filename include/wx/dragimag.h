#ifndef _WX_DRAGIMAG_H_BASE_
#define _WX_DRAGIMAG_H_BASE_

#if wxUSE_DRAGIMAG

#if defined(__WXMSW__)
#ifdef __WIN16__
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#define sm_classwxDragImage sm_classwxGenericDragImage

#else
#include "wx/msw/dragimag.h"
#endif
#elif defined(__WXMOTIF__)
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#define sm_classwxDragImage sm_classwxGenericDragImage

#elif defined(__WXGTK__)
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#define sm_classwxDragImage sm_classwxGenericDragImage

#elif defined(__WXQT__)
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#define sm_classwxDragImage sm_classwxGenericDragImage

#elif defined(__WXMAC__)
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#define sm_classwxDragImage sm_classwxGenericDragImage

#elif defined(__WXPM__)
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#define sm_classwxDragImage sm_classwxGenericDragImage

#elif defined(__WXSTUBS__)
#include "wx/generic/dragimgg.h"
#define wxDragImage wxGenericDragImage
#define sm_classwxDragImage sm_classwxGenericDragImage

#endif

#endif // wxUSE_DRAGIMAG

#endif
    // _WX_DRAGIMAG_H_BASE_
