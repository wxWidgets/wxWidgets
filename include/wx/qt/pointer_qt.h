/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/pointer_qt.h
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_POINTER_H_
#define _WX_QT_POINTER_H_

#include <QtCore/QPointer>

// Extend QPointer with the ability to delete the object in its destructor. The
// normal behaviour of the QPointer makes sure that this is safe, because if somebody
// has deleted the object, then data() returns NULL and delete does nothing.

template < typename T >
class wxQtPointer : public QPointer< T >
{
public:
    inline wxQtPointer()
        : QPointer< T >()
        { }

    inline wxQtPointer< T > &operator = ( T *p )
        { QPointer< T >::operator = ( p ); return *this; }

    inline ~wxQtPointer()
        { delete this->data(); }
};

#endif // _WX_QT_POINTER_H_
