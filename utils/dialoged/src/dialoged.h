/////////////////////////////////////////////////////////////////////////////
// Name:        dialoged.h
// Purpose:     Dialog Editor application header file
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "dialoged.h"
#endif

#ifndef dialogedh
#define dialogedh

#include "proplist.h"
#include "reseditr.h"

class MyChild;

// Define a new application
class MyApp: public wxApp
{
  public:
    MyApp(void);
    bool OnInit(void);
};

DECLARE_APP(MyApp)

extern wxFrame *GetMainFrame(void);

#endif
