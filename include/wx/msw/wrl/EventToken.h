///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/wrl/EventToken.h
// Purpose:     WRL EventRegistrationToken implementation
// Author:      Maarten Bent
// Created:     2021-02-26
// Copyright:   (c) 2021 wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __eventtoken_h__
#define __eventtoken_h__

typedef struct EventRegistrationToken
{
    __int64 value;
} EventRegistrationToken;

#endif // __eventtoken_h__
