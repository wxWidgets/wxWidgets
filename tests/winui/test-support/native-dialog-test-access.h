/////////////////////////////////////////////////////////////////////////////
// Private, non-installed native-dialog test adapters.
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WINUI_NATIVE_DIALOG_TEST_ACCESS_H
#define WX_WINUI_NATIVE_DIALOG_TEST_ACCESS_H

#ifndef WXWINUI_TEST_SUPPORT
#error Link the private wx_winui_test_support target to use this header
#endif

#include "wx/arrstr.h"
#include "wx/msw/private.h"
#include "wx/msw/wrapcctl.h"
#include "wx/msw/wrapshl.h"

#if defined(__WXWINUI__) && wxUSE_WINUI3

namespace wxMSWImpl
{

// Substitute only the final IFileDialog::Show() boundary. Production still
// creates and configures the real shell object before reaching this adapter.
struct wxIFileDialogShowHookForTesting
{
    void* context = nullptr;
    int (*show)(void *,
                WXHWND,
                int,
                wxArrayString *,
                wxString *) = nullptr;
};

WXDLLIMPEXP_CORE void SetIFileDialogShowHookForTesting(
    const wxIFileDialogShowHookForTesting& hook);
WXDLLIMPEXP_CORE void ResetIFileDialogShowHookForTesting();
WXDLLIMPEXP_CORE unsigned long GetIFileDialogOwnerCloseCountForTesting();

} // namespace wxMSWImpl

namespace wxMSWMessageDialog
{

// Substitute only TaskDialogIndirect(). The real TASKDIALOGCONFIG continues
// to be produced by the RichMessage and ProgressDialog implementations.
struct wxTaskDialogIndirectHookForTesting
{
    void *context = nullptr;
    HRESULT (WINAPI *invoke)(void *,
                             const TASKDIALOGCONFIG *,
                             int *,
                             int *,
                             BOOL *) = nullptr;
};

WXDLLIMPEXP_CORE void SetTaskDialogIndirectHookForTesting(
    const wxTaskDialogIndirectHookForTesting& hook);
WXDLLIMPEXP_CORE void ResetTaskDialogIndirectHookForTesting();

} // namespace wxMSWMessageDialog

#endif // __WXWINUI__ && wxUSE_WINUI3

#endif // WX_WINUI_NATIVE_DIALOG_TEST_ACCESS_H
