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

    wxScopedPtr<wxAnimation> animation(GetAnimation("animation", ctrl));
    if ( animation )
        ctrl->SetAnimation(*animation);

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

wxAnimation* wxXmlResourceHandlerImpl::GetAnimation(const wxString& param,
                                                    wxAnimationCtrlBase* ctrl)
{
    wxString name = GetFilePath(GetParamNode(param));
    if ( name.empty() )
        return NULL;

    // load the animation from file
    wxScopedPtr<wxAnimation> ani(ctrl ? new wxAnimation(ctrl->CreateAnimation())
                                      : new wxAnimation);
#if wxUSE_FILESYSTEM
    wxFSFile * const
        fsfile = GetCurFileSystem().OpenFile(name, wxFS_READ | wxFS_SEEKABLE);
    if ( fsfile )
    {
        ani->Load(*fsfile->GetStream());
        delete fsfile;
    }
#else
    ani->LoadFile(name);
#endif

    if ( !ani->IsOk() )
    {
        ReportParamError
        (
            param,
            wxString::Format("cannot create animation from \"%s\"", name)
        );
        return NULL;
    }

    return ani.release();
}

#endif // wxUSE_XRC && wxUSE_ANIMATIONCTRL
