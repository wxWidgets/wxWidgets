///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/pathlist.cpp
// Purpose:     Test wxPathList
// Author:      Francesco Montorsi (extracted from console sample)
// Created:     2010-06-02
// Copyright:   (c) 2010 wxWidgets team
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#include "wx/filefn.h"

TEST_CASE("wxPathList::FindValidPath", "[file][path]")
{
#ifdef __UNIX__
    #define CMD_IN_PATH "ls"
#else
    #define CMD_IN_PATH "cmd.exe"
#endif

    wxPathList pathlist;
    pathlist.AddEnvList(wxT("PATH"));

    wxString path = pathlist.FindValidPath(CMD_IN_PATH);
    INFO( CMD_IN_PATH " not found in " << wxGetenv("PATH") );
    CHECK( !path.empty() );
}
