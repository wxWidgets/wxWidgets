///////////////////////////////////////////////////////////////////////////////
// Name:        tests/winui/test-support/aui-notebook-test-access.h
// Purpose:     Private, non-installed wxAUI notebook test adapter
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_AUI_NOTEBOOK_TEST_ACCESS_H
#define WX_AUI_NOTEBOOK_TEST_ACCESS_H

#ifndef WXAUI_NOTEBOOK_TEST_SUPPORT
#error Enable the private wxAUI notebook test support to use this header
#endif

#include "wx/aui/auibook.h"

class WXDLLIMPEXP_AUI wxAuiNotebookTestAccess final
{
public:
    static void EndDrag(wxAuiNotebook* book,
                        wxAuiTabCtrl* source,
                        int sourceIndex,
                        wxAuiTabCtrl* destination,
                        int destinationIndex,
                        bool createNewPane = false);

    static void ClickWindowList(wxAuiTabCtrl* tabs);
};

#endif // WX_AUI_NOTEBOOK_TEST_ACCESS_H
