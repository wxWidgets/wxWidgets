/////////////////////////////////////////////////////////////////////////////
// Name:        No names yet.
// Purpose:     Contrib. demo
// Author:      Aleksandras Gluchovas
// Modified by:
// Created:     23/11/98
// RCS-ID:      $Id$
// Copyright:   1998 (c) Aleksandras Gluchovas
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __WXINFO_G__
#define __WXINFO_G__

#include "wx/object.h"
#include "wx/treectrl.h"


/*
 * creates tree with hierarchically cauptured 
 * information about wxWindows dynamic classes (at "current run-time")
 */

void wxCreateClassInfoTree( wxTreeCtrl* pTree, 
						    wxTreeItemId parentBranchId,
							long classImageNo = -1
					      );

/*
 * creates tree with information about 
 * serializer-classes (at current run-time)
 * NOTE:: "objstore.cpp" should be compiled in
 */

#endif
