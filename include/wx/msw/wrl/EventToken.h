///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/wrl/EventToken.h
// Purpose:     WRL EventRegistrationToken implementation
// Author:      Maarten Bent
// Created:     2021-02-26
// Copyright:   (c) 2021 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// Note: this include guard uses the same name as winrt/EventToken.h in the
// Windows SDK to prevent a struct type redefinition when both headers are
// included.
#ifndef __eventtoken_h__
#define __eventtoken_h__

typedef struct EventRegistrationToken
{
    __int64 value;
} EventRegistrationToken;

#endif // __eventtoken_h__
