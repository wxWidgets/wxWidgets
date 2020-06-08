/////////////////////////////////////////////////////////////////////////////
// Name:        src/xrc/xh_animatctrl.cpp
// Purpose:     XML resource handler for wxAnimationCtrl
// Author:      Francesco Montorsi
// Created:     2006-10-15
// Copyright:   (c) 2006 Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC && wxUSE_ANIMATIONCTRL

#include "wx/xrc/xh_animatctrl.h"
#include "wx/animate.h"
#include "wx/generic/animate.h"
#include "wx/scopedptr.h"

wxIMPLEMENT_DYNAMIC_CLASS(wxAnimationCtrlXmlHandler, wxXmlResourceHandler);

wxAnimationCtrlXmlHandler::wxAnimationCtrlXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxAC_NO_AUTORESIZE);
    XRC_ADD_STYLE(wxAC_DEFAULT_STYLE);
    AddWindowStyles();
}

wxObject *wxAnimationCtrlXmlHandler::DoCreateResource()
{
    wxAnimationCtrlBase *ctrl = NULL;
    if ( m_instance )
        ctrl = wxStaticCast(m_instance, wxAnimationCtrlBase);

    if ( !ctrl )
    {
        if ( m_class == "wxAnimationCtrl" )
        {
            ctrl = new wxAnimationCtrl(m_parentAsWindow,
                                       GetID(),
                                       wxNullAnimation,
                                       GetPosition(), GetSize(),
                                       GetStyle("style", wxAC_DEFAULT_STYLE),
                                       GetName());
        }
        else
        {
            ctrl = new wxGenericAnimationCtrl(m_parentAsWindow,
                                              GetID(),
                                              wxNullAnimation,
                                              GetPosition(), GetSize(),
                                              GetStyle("style", wxAC_DEFAULT_STYLE),
                                              GetName());
        }
    }
    if ( GetBool("hidden", 0) == 1 )
        ctrl->Hide();

    const wxString aniParam = "animation";
    wxString path = GetFilePath(GetParamNode(aniParam));
    // load the animation from file
    if ( !path.empty() )
    {
        wxAnimation ani = ctrl->CreateAnimation();
#if wxUSE_FILESYSTEM
        wxScopedPtr<wxFSFile> fsfile(
            GetCurFileSystem().OpenFile(path, wxFS_READ | wxFS_SEEKABLE)
        );
        if (fsfile)
        {
            ani.Load(*fsfile->GetStream());
        }
#else
        ani.LoadFile(name);
#endif

        if ( ani.IsOk() )
        {
            ctrl->SetAnimation(ani);
        }
        else
        {
            ReportParamError
            (
                aniParam,
                wxString::Format("cannot create animation from \"%s\"", path)
            );
        }
    }

    // if no inactive-bitmap has been provided, GetBitmap() will return wxNullBitmap
    // which just tells wxAnimationCtrl to use the default for inactive status
    ctrl->SetInactiveBitmap(GetBitmap(wxT("inactive-bitmap")));

    SetupWindow(ctrl);

    return ctrl;
}

bool wxAnimationCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxAnimationCtrl")) ||
           IsOfClass(node, wxT("wxGenericAnimationCtrl"));
}

#endif // wxUSE_XRC && wxUSE_ANIMATIONCTRL
