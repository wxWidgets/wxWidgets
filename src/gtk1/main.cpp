/////////////////////////////////////////////////////////////////////////////
// Name:        main.cpp
// Purpose:     Entry point
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/app.h"

int wxEntry(int argc, char *argv[]);


#if defined(AIX) || defined(AIX4) || defined(____HPUX__) || defined(NOMAIN)

  // main in IMPLEMENT_WX_MAIN in IMPLEMENT_APP in app.h

#else


int main(int argc, char* argv[])
{
  return wxEntry(argc, argv);
}

#endif
