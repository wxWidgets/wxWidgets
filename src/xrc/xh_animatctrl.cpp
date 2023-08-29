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


#if wxUSE_XRC && wxUSE_ANIMATIONCTRL

#include "wx/xrc/xh_animatctrl.h"
#include "wx/animate.h"
#include "wx/generic/animate.h"

#include <memory>

wxIMPLEMENT_DYNAMIC_CLASS(wxAnimationCtrlXmlHandler, wxXmlResourceHandler);

wxAnimationCtrlXmlHandler::wxAnimationCtrlXmlHandler() : wxXmlResourceHandler()
{
    XRC_ADD_STYLE(wxAC_NO_AUTORESIZE);
    XRC_ADD_STYLE(wxAC_DEFAULT_STYLE);
    AddWindowStyles();
}

wxObject *wxAnimationCtrlXmlHandler::DoCreateResource()
{
    wxAnimationCtrlBase *ctrl = nullptr;
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

    const auto animations = GetAnimations("animation", ctrl);
    if ( animations.IsOk() )
        ctrl->SetAnimation(animations);

    // if no inactive-bitmap has been provided, GetBitmapBundle() will return
    // an empty bundle, which just tells wxAnimationCtrl to use the default
    // bitmap for inactive status
    ctrl->SetInactiveBitmap(GetBitmapBundle(wxT("inactive-bitmap")));

    SetupWindow(ctrl);

    return ctrl;
}

bool wxAnimationCtrlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxAnimationCtrl")) ||
           IsOfClass(node, wxT("wxGenericAnimationCtrl"));
}

wxAnimationBundle
wxXmlResourceHandlerImpl::GetAnimations(const wxString& param,
                                        wxAnimationCtrlBase* ctrl)
{
    wxString paths = GetFilePath(GetParamNode(param));
    if ( paths.empty() )
        return {};

    wxAnimationBundle animations;
    for ( const auto& name: wxSplit(paths, ';', '\0') )
    {
        // create compatible animation object
        wxAnimation ani;
        if ( ctrl )
            ani = ctrl->CreateAnimation();

        // load the animation from file
#if wxUSE_FILESYSTEM
        wxFSFile * const
            fsfile = GetCurFileSystem().OpenFile(name, wxFS_READ | wxFS_SEEKABLE);
        if ( fsfile )
        {
            ani.Load(*fsfile->GetStream());
            delete fsfile;
        }
#else
        ani.LoadFile(name);
#endif

        if ( !ani.IsOk() )
        {
            ReportParamError
            (
                param,
                wxString::Format("cannot create animation from \"%s\"", name)
            );
            return {};
        }

        animations.Add(ani);
    }

    return animations;
}

#if WXWIN_COMPATIBILITY_3_2

wxAnimation* wxXmlResourceHandlerImpl::GetAnimation(const wxString& param,
                                                    wxAnimationCtrlBase* ctrl)
{
    const auto animations = GetAnimations(param, ctrl);

    return animations.IsOk() ? new wxAnimation(animations.GetAll()[0]) : nullptr;
}

#endif // WXWIN_COMPATIBILITY_3_2

#endif // wxUSE_XRC && wxUSE_ANIMATIONCTRL
