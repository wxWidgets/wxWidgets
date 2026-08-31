/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dialog.h
// Purpose:     wxDialog class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

#include "wx/panel.h"

extern WXDLLIMPEXP_DATA_CORE(const char) wxDialogNameStr[];

class WXDLLIMPEXP_FWD_CORE wxDialogModalData;

// Dialog boxes
class WXDLLIMPEXP_CORE wxDialog : public wxDialogBase
{
public:
    wxDialog() { Init(); }

    // full ctor
    wxDialog(wxWindow *parent, wxWindowID id,
             const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxASCII_STR(wxDialogNameStr))
    {
        Init();

        (void)Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxASCII_STR(wxDialogNameStr));

    virtual ~wxDialog();

    // return true if we're showing the dialog modally
    virtual bool IsModal() const override { return m_modalData != nullptr; }

    // show the dialog modally and return the value passed to EndModal()
    virtual int ShowModal() override;

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode) override;


    // implementation only from now on
    // -------------------------------

    // override some base class virtuals
    virtual bool Show(bool show = true) override;
    virtual void SetWindowStyleFlag(long style) override;

    // Windows callbacks
    WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam) override;
#if defined(__WXWINUI__) && wxUSE_WINUI3
    // WinUI dialogs use the regular frame window class and so don't have
    // DefDlgProc() to provide the usual dialog keyboard contract. Keep this
    // fallback on the dialog itself so island and native children use exactly
    // the same wx-level button/validation path.
    bool WinUIHandleEscapeKey();
    bool WinUIActivateDefaultButton();
    // Run the native wx modal loop without firing wxModalDialogHook again.
    // WinUI common-dialog presenters already fired the hook for their public
    // source object; their private shell must remain invisible to that API.
    int WinUIShowModalWithoutHook();
    bool Destroy() override;
    bool WinUICompleteDeferredModalDestroy();
    void WinUIBeginExternalModalLifetime();
    void WinUIArmExternalModalLifetime();
    void WinUIEndExternalModalLifetime();
    bool MSWTranslateMessage(WXMSG* msg) override;
    bool MSWProcessMessage(WXMSG* msg) override;
#endif

protected:
    // common part of all ctors
    void Init();

private:
    // these functions deal with the gripper window shown in the corner of
    // resizable dialogs
    void CreateGripper();
    void DestroyGripper();
    void ShowGripper(bool show);
    void ResizeGripper();

    // this function is used to adjust Z-order of new children relative to the
    // gripper if we have one
    void OnWindowCreate(wxWindowCreateEvent& event);

    // gripper window for a resizable dialog, nullptr if we're not resizable
    WXHWND m_hGripper;

    // this pointer is non-null only while the modal event loop is running
    wxDialogModalData *m_modalData;

#if defined(__WXWINUI__) && wxUSE_WINUI3
    void WinUIOnExternalParentDestroy(wxWindowDestroyEvent& event);

    unsigned m_winuiExternalModalDepth = 0;
    bool m_winuiExternalModalArmed = false;
    bool m_winuiExternalDestroyRequested = false;
    wxWindow *m_winuiExternalParent = nullptr;
    bool m_winuiExternalParentBound = false;
    bool m_winuiExternalParentDestroyDispatch = false;
#endif

    wxDECLARE_DYNAMIC_CLASS(wxDialog);
    wxDECLARE_NO_COPY_CLASS(wxDialog);
};

#endif
    // _WX_DIALOG_H_
