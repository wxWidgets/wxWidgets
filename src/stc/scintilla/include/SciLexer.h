// Scintilla source code edit control
// SciLexer - interface to the added lexer functions in the SciLexer version of the edit control
// Copyright 1998-2000 by Neil Hodgson <neilh@scintilla.org>
// The License.txt file describes the conditions under which this software may be distributed.

#ifndef SCILEXER_H
#define SCILEXER_H

// SciLexer features - not in standard Scintilla

#define SCLEX_CONTAINER 0
#define SCLEX_NULL 1
#define SCLEX_PYTHON 2
#define SCLEX_CPP 3
#define SCLEX_HTML 4
#define SCLEX_XML 5
#define SCLEX_PERL 6
#define SCLEX_SQL 7
#define SCLEX_VB 8
#define SCLEX_PROPERTIES 9
#define SCLEX_ERRORLIST 10
#define SCLEX_MAKEFILE 11
#define SCLEX_BATCH 12

// Lexical states for SCLEX_PYTHON
#define SCE_P_DEFAULT 0
#define SCE_P_COMMENTLINE 1
#define SCE_P_NUMBER 2
#define SCE_P_STRING 3
#define SCE_P_CHARACTER 4
#define SCE_P_WORD 5
#define SCE_P_TRIPLE 6
#define SCE_P_TRIPLEDOUBLE 7
#define SCE_P_CLASSNAME 8
#define SCE_P_DEFNAME 9
#define SCE_P_OPERATOR 10
#define SCE_P_IDENTIFIER 11

// Lexical states for SCLEX_CPP, SCLEX_VB
#define SCE_C_DEFAULT 0
#define SCE_C_COMMENT 1
#define SCE_C_COMMENTLINE 2
#define SCE_C_COMMENTDOC 3
#define SCE_C_NUMBER 4
#define SCE_C_WORD 5
#define SCE_C_STRING 6
#define SCE_C_CHARACTER 7
#define SCE_C_PUNTUATION 8
#define SCE_C_PREPROCESSOR 9
#define SCE_C_OPERATOR 10
#define SCE_C_IDENTIFIER 11
#define SCE_C_STRINGEOL 12

// Lexical states for SCLEX_HTML, SCLEX_xML
#define SCE_H_DEFAULT 0
#define SCE_H_TAG 1
#define SCE_H_TAGUNKNOWN 2
#define SCE_H_ATTRIBUTE 3
#define SCE_H_ATTRIBUTEUNKNOWN 4
#define SCE_H_NUMBER 5
#define SCE_H_DOUBLESTRING 6
#define SCE_H_SINGLESTRING 7
#define SCE_H_OTHER 8
#define SCE_H_COMMENT 9
#define SCE_H_ENTITY 10
// Embedded Javascript
#define SCE_HJ_START 11
#define SCE_HJ_DEFAULT 12
#define SCE_HJ_COMMENT 13
#define SCE_HJ_COMMENTLINE 14
#define SCE_HJ_COMMENTDOC 15
#define SCE_HJ_NUMBER 16
#define SCE_HJ_WORD 17
#define SCE_HJ_KEYWORD 18
#define SCE_HJ_DOUBLESTRING 19
#define SCE_HJ_SINGLESTRING 20
#define SCE_HJ_SYMBOLS 21
#define SCE_HJ_STRINGEOL 28
// XML and ASP
#define SCE_H_TAGEND 22
#define SCE_H_XMLSTART 23
#define SCE_H_XMLEND 24
#define SCE_H_SCRIPT 25
#define SCE_H_ASP 26
#define SCE_H_ASPAT 27
// Embedded VBScript
#define SCE_HB_START 40
#define SCE_HB_DEFAULT 41
#define SCE_HB_COMMENTLINE 42
#define SCE_HB_NUMBER 43
#define SCE_HB_WORD 44
#define SCE_HB_STRING 45
#define SCE_HB_IDENTIFIER 46
#define SCE_HB_STRINGEOL 47
// Embedded Python
#define SCE_HP_START 50
#define SCE_HP_DEFAULT 51
#define SCE_HP_COMMENTLINE 52
#define SCE_HP_NUMBER 53
#define SCE_HP_STRING 54
#define SCE_HP_CHARACTER 55
#define SCE_HP_WORD 56
#define SCE_HP_TRIPLE 57
#define SCE_HP_TRIPLEDOUBLE 58
#define SCE_HP_CLASSNAME 59
#define SCE_HP_DEFNAME 60
#define SCE_HP_OPERATOR 61
#define SCE_HP_IDENTIFIER 62

// Lexical states for SCLEX_PERL
#define SCE_PL_DEFAULT 0
#define SCE_PL_HERE 1
#define SCE_PL_COMMENTLINE 2
#define SCE_PL_POD 3
#define SCE_PL_NUMBER 4
#define SCE_PL_WORD 5
#define SCE_PL_STRING 6
#define SCE_PL_CHARACTER 7
#define SCE_PL_PUNCTUATION 8
#define SCE_PL_PREPROCESSOR 9
#define SCE_PL_OPERATOR 10
#define SCE_PL_IDENTIFIER 11
#define SCE_PL_SCALAR 12
#define SCE_PL_ARRAY 13
#define SCE_PL_HASH 14
#define SCE_PL_SYMBOLTABLE 15
#define SCE_PL_REF 16
#define SCE_PL_REGEX 17
#define SCE_PL_REGSUBST 18
#define SCE_PL_LONGQUOTE 19
#define SCE_PL_BACKTICKS 20
#define SCE_PL_DATASECTION 21
    
#endif
