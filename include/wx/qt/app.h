/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     wxApp class
// Author:      Peter Most
// Modified by:
// Created:     08/08/09
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_APP_H_
#define _WX_QT_APP_H_

#include "wx/qt/pointer_qt.h"
#include <QtGui/QApplication>

class WXDLLIMPEXP_CORE wxApp : public wxAppBase
{
public:
    wxApp();
    ~wxApp();
    
    virtual bool Initialize(int& argc, wxChar **argv);

private:
    wxQtPointer< QApplication > m_qtApplication;
    int m_qtArgc;
    char **m_qtArgv;
    
    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxApp );
};
    
#endif // _WX_QT_APP_H_
