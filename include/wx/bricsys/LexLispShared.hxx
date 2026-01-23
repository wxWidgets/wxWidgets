// LexLispShared.h : shared constants for Bricsys AutoLISP + DCL lexers (public, wx-free)
//
// Copyright © Menhirs NV. All rights reserved.

#pragma once

// "LineState" to indicate 'defun' + 'dialog' identifiers
enum
{
    stateNoState        = 0x0000,
    stateInComment      = 0x0010,
    stateInBlockComment = 0x0020,

    stateIdDefun        = 0x0100,
    stateIdDialog       = 0x0200,
    stateLspLoad        = 0x0400,
    stateLspCmdCall     = 0x0800,
    stateLspSetvar      = 0x1000,
    stateLspVlx         = 0x2000,
    stateLspBB          = 0x4000,
    stateDclLoad        = 0x8000,

    stateLineMask = stateIdDefun   | stateIdDialog | stateLspLoad | stateLspCmdCall |
                    stateLspSetvar | stateLspVlx   | stateLspBB   | stateDclLoad,
};

// Scintilla style ids for the custom DCL lexer (public constants)
#define SCE_DCL_DEFAULT 0
#define SCE_DCL_COMMENT 1
#define SCE_DCL_NUMBER 2
#define SCE_DCL_KEYWORD 3
#define SCE_DCL_KEYWORD_KW 4
#define SCE_DCL_SYMBOL 5
#define SCE_DCL_STRING 6
#define SCE_DCL_STRINGEOL 8
#define SCE_DCL_IDENTIFIER 9
#define SCE_DCL_OPERATOR 10
#define SCE_DCL_SPECIAL 11
#define SCE_DCL_MULTI_COMMENT 12

