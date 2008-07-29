#if wxOSX_USE_CARBON
#include "wx/osx/carbon/region.h"
#else
#define wxRegionGeneric wxRegion
#define wxRegionIteratorGeneric wxRegionIterator

#include "wx/generic/region.h"
#endif
