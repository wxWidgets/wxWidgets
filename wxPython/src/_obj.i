/////////////////////////////////////////////////////////////////////////////
// Name:        _wxobj.i
// Purpose:     SWIG interface for wxObject
//
// Author:      Robin Dunn
//
// Created:     9-Aug-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------
%newgroup

class wxObject {
public:

    %extend {
        wxString GetClassName() {
            return self->GetClassInfo()->GetClassName();
        }

        void Destroy() {
            delete self;
        }
    }
};


//---------------------------------------------------------------------------
