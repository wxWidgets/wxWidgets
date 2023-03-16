///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/webview.h
// Purpose:     wxWebView implementation classes
// Author:      Tobias Taschner
// Created:     2023-03-16
// Copyright:   (c) 2023 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WEBVIEW_H_
#define _WX_PRIVATE_WEBVIEW_H_

class WXDLLIMPEXP_WEBVIEW wxWebViewConfigurationImpl
{
public:
    virtual ~wxWebViewConfigurationImpl() = default;
    virtual void* GetNativeConfiguration() const { return nullptr; }
    virtual void SetDataPath(const wxString& WXUNUSED(path)) {}
    virtual wxString GetDataPath() const { return wxString{}; }
};

#endif // _WX_PRIVATE_WEBVIEW_H_
