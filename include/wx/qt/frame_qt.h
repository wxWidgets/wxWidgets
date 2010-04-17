/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/frame_qt.h
// Purpose:     wxQtFrame class interface
// Author:      Peter Most
// Modified by:
// Created:     09.08.09
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FRAME_QT_H_
#define _WX_QT_FRAME_QT_H_

#include <QtGui/QMainWindow>
#include "wx/dlimpexp.h"

class WXDLLIMPEXP_CORE wxQtFrame : public QMainWindow
{
    Q_OBJECT
    
public:
    wxQtFrame( QWidget *parent );
};

#endif
