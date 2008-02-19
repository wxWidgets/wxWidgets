/////////////////////////////////////////////////////////////////////////////
// Name:        resyn
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/*!

 @page resyn_overview Syntax of the builtin regular expression library

 A @e regular expression describes strings of characters. It's a
 pattern that matches certain strings and doesn't match others.
 @b See also
 #wxRegEx
 @ref differentflavors_overview
 @ref resyntax_overview
 @ref resynbracket_overview
 #Escapes
 #Metasyntax
 #Matching
 @ref relimits_overview
 @ref resynbre_overview
 @ref resynchars_overview


 @section differentflavors Different Flavors of REs

 @ref resyn_overview
 Regular expressions ("RE''s), as defined by POSIX, come in two
 flavors: @e extended REs ("EREs'') and @e basic REs ("BREs''). EREs are roughly those
 of the traditional @e egrep, while BREs are roughly those of the traditional
  @e ed.  This implementation adds a third flavor, @e advanced REs ("AREs''), basically
 EREs with some significant extensions.
 This manual page primarily describes
 AREs. BREs mostly exist for backward compatibility in some old programs;
 they will be discussed at the #end. POSIX EREs are almost an exact subset
 of AREs. Features of AREs that are not present in EREs will be indicated.

 @section resyntax Regular Expression Syntax

 @ref resyn_overview
 These regular expressions are implemented using
 the package written by Henry Spencer, based on the 1003.2 spec and some
 (not quite all) of the Perl5 extensions (thanks, Henry!).  Much of the description
 of regular expressions below is copied verbatim from his manual entry.
 An ARE is one or more @e branches, separated by '@b |', matching anything that matches
 any of the branches.
 A branch is zero or more @e constraints or @e quantified
 atoms, concatenated. It matches a match for the first, followed by a match
 for the second, etc; an empty branch matches the empty string.
 A quantified atom is an @e atom possibly followed by a single @e quantifier. Without a quantifier,
 it matches a match for the atom. The quantifiers, and what a so-quantified
 atom matches, are:






 @b *




 a sequence of 0 or more matches of the atom





 @b +




 a sequence of 1 or more matches of the atom





 @b ?




 a sequence of 0 or 1 matches of the atom





 @b {m}




 a sequence of exactly @e m matches of the atom





 @b {m,}




 a sequence of @e m or more matches of the atom





 @b {m,n}




 a sequence of @e m through @e n (inclusive)
 matches of the atom; @e m may not exceed @e n





 @b *?  +?  ??  {m}?  {m,}?  {m,n}?




 @e non-greedy quantifiers,
 which match the same possibilities, but prefer the
 smallest number rather than the largest number of matches (see #Matching)





 The forms using @b { and @b } are known as @e bounds. The numbers @e m and @e n are unsigned
 decimal integers with permissible values from 0 to 255 inclusive.
 An atom is one of:






 @b (re)




 (where @e re is any regular expression) matches a match for
  @e re, with the match noted for possible reporting





 @b (?:re)




 as previous, but
 does no reporting (a "non-capturing'' set of parentheses)





 @b ()




 matches an empty
 string, noted for possible reporting





 @b (?:)




 matches an empty string, without reporting





 @b [chars]




 a @e bracket expression, matching any one of the @e chars
 (see @ref resynbracket_overview for more detail)





 @b .




 matches any single character





 @b \k




 (where @e k is a non-alphanumeric character)
 matches that character taken as an ordinary character, e.g. \\ matches a backslash
 character





 @b \c




 where @e c is alphanumeric (possibly followed by other characters),
 an @e escape (AREs only), see #Escapes below





 @b {




 when followed by a character
 other than a digit, matches the left-brace character '@b {'; when followed by
 a digit, it is the beginning of a @e bound (see above)





 @b x




 where @e x is a single
 character with no other significance, matches that character.





 A @e constraint matches an empty string when specific conditions are met. A constraint may
 not be followed by a quantifier. The simple constraints are as follows;
 some more constraints are described later, under #Escapes.






 @b ^




 matches at the beginning of a line





 @b $




 matches at the end of a line





 @b (?=re)




 @e positive lookahead
 (AREs only), matches at any point where a substring matching @e re begins





 @b (?!re)




 @e negative lookahead (AREs only),
 matches at any point where no substring matching @e re begins





 The lookahead constraints may not contain back references
 (see later), and all parentheses within them are considered non-capturing.
 An RE may not end with '@b \'.

 @section wxresynbracket Bracket Expressions

 @ref resyn_overview
 A @e bracket expression is a list
 of characters enclosed in '@b []'. It normally matches any single character from
 the list (but see below). If the list begins with '@b ^', it matches any single
 character (but see below) @e not from the rest of the list.
 If two characters
 in the list are separated by '@b -', this is shorthand for the full @e range of
 characters between those two (inclusive) in the collating sequence, e.g.
  @b [0-9] in ASCII matches any decimal digit. Two ranges may not share an endpoint,
 so e.g. @b a-c-e is illegal. Ranges are very collating-sequence-dependent, and portable
 programs should avoid relying on them.
 To include a literal @b ] or @b - in the
 list, the simplest method is to enclose it in @b [. and @b .] to make it a collating
 element (see below). Alternatively, make it the first character (following
 a possible '@b ^'), or (AREs only) precede it with '@b \'.
 Alternatively, for '@b -', make
 it the last character, or the second endpoint of a range. To use a literal
  @b - as the first endpoint of a range, make it a collating element or (AREs
 only) precede it with '@b \'. With the exception of these, some combinations using
  @b [ (see next paragraphs), and escapes, all other special characters lose
 their special significance within a bracket expression.
 Within a bracket
 expression, a collating element (a character, a multi-character sequence
 that collates as if it were a single character, or a collating-sequence
 name for either) enclosed in @b [. and @b .] stands for the
 sequence of characters of that collating element.
 @e wxWidgets: Currently no multi-character collating elements are defined.
 So in @b [.X.], @e X can either be a single character literal or
 the name of a character. For example, the following are both identical
  @b [[.0.]-[.9.]] and @b [[.zero.]-[.nine.]] and mean the same as
  @b [0-9].
  See @ref resynchars_overview.
 Within a bracket expression, a collating element enclosed in @b [= and @b =]
 is an equivalence class, standing for the sequences of characters of all
 collating elements equivalent to that one, including itself.
 An equivalence class may not be an endpoint of a range.
 @e wxWidgets: Currently no equivalence classes are defined, so
 @b [=X=] stands for just the single character @e X.
  @e X can either be a single character literal or the name of a character,
 see @ref resynchars_overview.
 Within a bracket expression,
 the name of a @e character class enclosed in @b [: and @b :] stands for the list
 of all characters (not all collating elements!) belonging to that class.
 Standard character classes are:






 @b alpha




 A letter.





 @b upper




 An upper-case letter.





 @b lower




 A lower-case letter.





 @b digit




 A decimal digit.





 @b xdigit




 A hexadecimal digit.





 @b alnum




 An alphanumeric (letter or digit).





 @b print




 An alphanumeric (same as alnum).





 @b blank




 A space or tab character.





 @b space




 A character producing white space in displayed text.





 @b punct




 A punctuation character.





 @b graph




 A character with a visible representation.





 @b cntrl




 A control character.





 A character class may not be used as an endpoint of a range.
 @e wxWidgets: In a non-Unicode build, these character classifications depend on the
 current locale, and correspond to the values return by the ANSI C 'is'
 functions: isalpha, isupper, etc. In Unicode mode they are based on
 Unicode classifications, and are not affected by the current locale.
 There are two special cases of bracket expressions:
 the bracket expressions @b [[::]] and @b [[::]] are constraints, matching empty
 strings at the beginning and end of a word respectively.  A word is defined
 as a sequence of word characters that is neither preceded nor followed
 by word characters. A word character is an @e alnum character or an underscore
 (@b _). These special bracket expressions are deprecated; users of AREs should
 use constraint escapes instead (see #Escapes below).

 @section wxresynescapes Escapes

 @ref resyn_overview
 Escapes (AREs only),
 which begin with a @b \ followed by an alphanumeric character, come in several
 varieties: character entry, class shorthands, constraint escapes, and back
 references. A @b \ followed by an alphanumeric character but not constituting
 a valid escape is illegal in AREs. In EREs, there are no escapes: outside
 a bracket expression, a @b \ followed by an alphanumeric character merely stands
 for that character as an ordinary character, and inside a bracket expression,
  @b \ is an ordinary character. (The latter is the one actual incompatibility
 between EREs and AREs.)
 Character-entry escapes (AREs only) exist to make
 it easier to specify non-printing and otherwise inconvenient characters
 in REs:






 @b \a




 alert (bell) character, as in C





 @b \b




 backspace, as in C





 @b \B




 synonym
 for @b \ to help reduce backslash doubling in some applications where there
 are multiple levels of backslash processing





 @b \c@e X




 (where X is any character)
 the character whose low-order 5 bits are the same as those of @e X, and whose
 other bits are all zero





 @b \e




 the character whose collating-sequence name is
 '@b ESC', or failing that, the character with octal value 033





 @b \f




 formfeed, as in C





 @b \n




 newline, as in C





 @b \r




 carriage return, as in C





 @b \t




 horizontal tab, as in C





 @b \u@e wxyz




 (where @e wxyz is exactly four hexadecimal digits)
 the Unicode
 character @b U+@e wxyz in the local byte ordering





 @b \U@e stuvwxyz




 (where @e stuvwxyz is
 exactly eight hexadecimal digits) reserved for a somewhat-hypothetical Unicode
 extension to 32 bits





 @b \v




 vertical tab, as in C are all available.





 @b \x@e hhh




 (where
  @e hhh is any sequence of hexadecimal digits) the character whose hexadecimal
 value is @b 0x@e hhh (a single character no matter how many hexadecimal digits
 are used).





 @b \0




 the character whose value is @b 0





 @b \@e xy




 (where @e xy is exactly two
 octal digits, and is not a @e back reference (see below)) the character whose
 octal value is @b 0@e xy





 @b \@e xyz




 (where @e xyz is exactly three octal digits, and is
 not a back reference (see below))
 the character whose octal value is @b 0@e xyz





 Hexadecimal digits are '@b 0'-'@b 9', '@b a'-'@b f', and '@b A'-'@b F'. Octal
 digits are '@b 0'-'@b 7'.
 The character-entry
 escapes are always taken as ordinary characters. For example, @b \135 is @b ] in
 ASCII, but @b \135 does not terminate a bracket expression. Beware, however,
 that some applications (e.g., C compilers) interpret  such sequences themselves
 before the regular-expression package gets to see them, which may require
 doubling (quadrupling, etc.) the '@b \'.
 Class-shorthand escapes (AREs only) provide
 shorthands for certain commonly-used character classes:






 @b \d




 @b [[:digit:]]





 @b \s




 @b [[:space:]]





 @b \w




 @b [[:alnum:]_] (note underscore)





 @b \D




 @b [^[:digit:]]





 @b \S




 @b [^[:space:]]





 @b \W




 @b [^[:alnum:]_] (note underscore)





 Within bracket expressions, '@b \d', '@b \s', and
 '@b \w' lose their outer brackets, and '@b \D',
 '@b \S', and '@b \W' are illegal. (So, for example,
  @b [a-c\d] is equivalent to @b [a-c[:digit:]].
 Also, @b [a-c\D], which is equivalent to
  @b [a-c^[:digit:]], is illegal.)
 A constraint escape (AREs only) is a constraint,
 matching the empty string if specific conditions are met, written as an
 escape:






 @b \A




 matches only at the beginning of the string
 (see #Matching, below,
 for how this differs from '@b ^')





 @b \m




 matches only at the beginning of a word





 @b \M




 matches only at the end of a word





 @b \y




 matches only at the beginning or end of a word





 @b \Y




 matches only at a point that is not the beginning or end of
 a word





 @b \Z




 matches only at the end of the string
 (see #Matching, below, for
 how this differs from '@b $')





 @b \@e m




 (where @e m is a nonzero digit) a @e back reference,
 see below





 @b \@e mnn




 (where @e m is a nonzero digit, and @e nn is some more digits,
 and the decimal value @e mnn is not greater than the number of closing capturing
 parentheses seen so far) a @e back reference, see below





 A word is defined
 as in the specification of @b [[::]] and @b [[::]] above. Constraint escapes are
 illegal within bracket expressions.
 A back reference (AREs only) matches
 the same string matched by the parenthesized subexpression specified by
 the number, so that (e.g.) @b ([bc])\1 matches @b bb or @b cc but not '@b bc'.
 The subexpression
 must entirely precede the back reference in the RE. Subexpressions are numbered
 in the order of their leading parentheses. Non-capturing parentheses do not
 define subexpressions.
 There is an inherent historical ambiguity between
 octal character-entry  escapes and back references, which is resolved by
 heuristics, as hinted at above. A leading zero always indicates an octal
 escape. A single non-zero digit, not followed by another digit, is always
 taken as a back reference. A multi-digit sequence not starting with a zero
 is taken as a back  reference if it comes after a suitable subexpression
 (i.e. the number is in the legal range for a back reference), and otherwise
 is taken as octal.

 @section remetasyntax Metasyntax

 @ref resyn_overview
 In addition to the main syntax described above,
 there are some special forms and miscellaneous syntactic facilities available.
 Normally the flavor of RE being used is specified by application-dependent
 means. However, this can be overridden by a @e director. If an RE of any flavor
 begins with '@b ***:', the rest of the RE is an ARE. If an RE of any flavor begins
 with '@b ***=', the rest of the RE is taken to be a literal string, with all
 characters considered ordinary characters.
 An ARE may begin with @e embedded options: a sequence @b (?xyz)
 (where @e xyz is one or more alphabetic characters)
 specifies options affecting the rest of the RE. These supplement, and can
 override, any options specified by the application. The available option
 letters are:






 @b b




 rest of RE is a BRE





 @b c




 case-sensitive matching (usual default)





 @b e




 rest of RE is an ERE





 @b i




 case-insensitive matching (see #Matching, below)





 @b m




 historical synonym for @b n





 @b n




 newline-sensitive matching (see #Matching, below)





 @b p




 partial newline-sensitive matching (see #Matching, below)





 @b q




 rest of RE
 is a literal ("quoted'') string, all ordinary characters





 @b s




 non-newline-sensitive matching (usual default)





 @b t




 tight syntax (usual default; see below)





 @b w




 inverse
 partial newline-sensitive ("weird'') matching (see #Matching, below)





 @b x




 expanded syntax (see below)





 Embedded options take effect at the @b ) terminating the
 sequence. They are available only at the start of an ARE, and may not be
 used later within it.
 In addition to the usual (@e tight) RE syntax, in which
 all characters are significant, there is an @e expanded syntax, available
 in AREs with the embedded
 x option. In the expanded syntax, white-space characters are ignored and
 all characters between a @b # and the following newline (or the end of the
 RE) are ignored, permitting paragraphing and commenting a complex RE. There
 are three exceptions to that basic rule:


 a white-space character or '@b #' preceded
 by '@b \' is retained
 white space or '@b #' within a bracket expression is retained
 white space and comments are illegal within multi-character symbols like
 the ARE '@b (?:' or the BRE '@b \('


 Expanded-syntax white-space characters are blank,
 tab, newline, and any character that belongs to the @e space character class.
 Finally, in an ARE, outside bracket expressions, the sequence '@b (?#ttt)' (where
  @e ttt is any text not containing a '@b )') is a comment, completely ignored. Again,
 this is not allowed between the characters of multi-character symbols like
 '@b (?:'. Such comments are more a historical artifact than a useful facility,
 and their use is deprecated; use the expanded syntax instead.
 @e None of these
 metasyntax extensions is available if the application (or an initial @b ***=
 director) has specified that the user's input be treated as a literal string
 rather than as an RE.

 @section wxresynmatching Matching

 @ref resyn_overview
 In the event that an RE could match more than
 one substring of a given string, the RE matches the one starting earliest
 in the string. If the RE could match more than one substring starting at
 that point, its choice is determined by its @e preference: either the longest
 substring, or the shortest.
 Most atoms, and all constraints, have no preference.
 A parenthesized RE has the same preference (possibly none) as the RE. A
 quantified atom with quantifier @b {m} or @b {m}? has the same preference (possibly
 none) as the atom itself. A quantified atom with other normal quantifiers
 (including @b {m,n} with @e m equal to @e n) prefers longest match. A quantified
 atom with other non-greedy quantifiers (including @b {m,n}? with @e m equal to
  @e n) prefers shortest match. A branch has the same preference as the first
 quantified atom in it which has a preference. An RE consisting of two or
 more branches connected by the @b | operator prefers longest match.
 Subject to the constraints imposed by the rules for matching the whole RE, subexpressions
 also match the longest or shortest possible substrings, based on their
 preferences, with subexpressions starting earlier in the RE taking priority
 over ones starting later. Note that outer subexpressions thus take priority
 over their component subexpressions.
 Note that the quantifiers @b {1,1} and
  @b {1,1}? can be used to force longest and shortest preference, respectively,
 on a subexpression or a whole RE.
 Match lengths are measured in characters,
 not collating elements. An empty string is considered longer than no match
 at all. For example, @b bb* matches the three middle characters
 of '@b abbbc', @b (week|wee)(night|knights)
 matches all ten characters of '@b weeknights', when @b (.*).* is matched against
  @b abc the parenthesized subexpression matches all three characters, and when
  @b (a*)* is matched against @b bc both the whole RE and the parenthesized subexpression
 match an empty string.
 If case-independent matching is specified, the effect
 is much as if all case distinctions had vanished from the alphabet. When
 an alphabetic that exists in multiple cases appears as an ordinary character
 outside a bracket expression, it is effectively transformed into a bracket
 expression containing both cases, so that @b x becomes '@b [xX]'. When it appears
 inside a bracket expression, all case counterparts of it are added to the
 bracket expression, so that @b [x] becomes @b [xX] and @b [^x] becomes '@b [^xX]'.
 If newline-sensitive
 matching is specified, @b . and bracket expressions using @b ^ will never match
 the newline character (so that matches will never cross newlines unless
 the RE explicitly arranges it) and @b ^ and @b $ will match the empty string after
 and before a newline respectively, in addition to matching at beginning
 and end of string respectively. ARE @b \A and @b \Z continue to match beginning
 or end of string @e only.
 If partial newline-sensitive matching is specified,
 this affects @b . and bracket expressions as with newline-sensitive matching,
 but not @b ^ and '@b $'.
 If inverse partial newline-sensitive matching is specified,
 this affects @b ^ and @b $ as with newline-sensitive matching, but not @b . and bracket
 expressions. This isn't very useful but is provided for symmetry.

 @section relimits Limits And Compatibility

 @ref resyn_overview
 No particular limit is imposed on the length of REs. Programs
 intended to be highly portable should not employ REs longer than 256 bytes,
 as a POSIX-compliant implementation can refuse to accept such REs.
 The only
 feature of AREs that is actually incompatible with POSIX EREs is that @b \
 does not lose its special significance inside bracket expressions. All other
 ARE features use syntax which is illegal or has undefined or unspecified
 effects in POSIX EREs; the @b *** syntax of directors likewise is outside
 the POSIX syntax for both BREs and EREs.
 Many of the ARE extensions are
 borrowed from Perl, but some have been changed to clean them up, and a
 few Perl extensions are not present. Incompatibilities of note include '@b \b',
 '@b \B', the lack of special treatment for a trailing newline, the addition of
 complemented bracket expressions to the things affected by newline-sensitive
 matching, the restrictions on parentheses and back references in lookahead
 constraints, and the longest/shortest-match (rather than first-match) matching
 semantics.
 The matching rules for REs containing both normal and non-greedy
 quantifiers have changed since early beta-test versions of this package.
 (The new rules are much simpler and cleaner, but don't work as hard at guessing
 the user's real intentions.)
 Henry Spencer's original 1986 @e regexp package, still in widespread use,
 implemented an early version of today's EREs. There are four incompatibilities between @e regexp's
 near-EREs ('RREs' for short) and AREs. In roughly increasing order of significance:


  In AREs, @b \ followed by an alphanumeric character is either an escape or
 an error, while in RREs, it was just another way of writing the  alphanumeric.
 This should not be a problem because there was no reason to write such
 a sequence in RREs.
  @b { followed by a digit in an ARE is the beginning of
 a bound, while in RREs, @b { was always an ordinary character. Such sequences
 should be rare, and will often result in an error because following characters
 will not look like a valid bound.
  In AREs, @b \ remains a special character
 within '@b []', so a literal @b \ within @b [] must be
 written '@b \\'. @b \\ also gives a literal
  @b \ within @b [] in RREs, but only truly paranoid programmers routinely doubled
 the backslash.
  AREs report the longest/shortest match for the RE, rather
 than the first found in a specified search order. This may affect some RREs
 which were written in the expectation that the first match would be reported.
 (The careful crafting of RREs to optimize the search order for fast matching
 is obsolete (AREs examine all possible matches in parallel, and their performance
 is largely insensitive to their complexity) but cases where the search
 order was exploited to deliberately  find a match which was @e not the longest/shortest
 will need rewriting.)



 @section wxresynbre Basic Regular Expressions

 @ref resyn_overview
 BREs differ from EREs in
 several respects.  '@b |', '@b +', and @b ? are ordinary characters and there is no equivalent
 for their functionality. The delimiters for bounds
 are @b \{ and '@b \}', with @b { and
  @b } by themselves ordinary characters. The parentheses for nested subexpressions
 are @b \( and '@b \)', with @b ( and @b ) by themselves
 ordinary characters. @b ^ is an ordinary
 character except at the beginning of the RE or the beginning of a parenthesized
 subexpression, @b $ is an ordinary character except at the end of the RE or
 the end of a parenthesized subexpression, and @b * is an ordinary character
 if it appears at the beginning of the RE or the beginning of a parenthesized
 subexpression (after a possible leading '@b ^'). Finally, single-digit back references
 are available, and @b \ and @b \ are synonyms
 for @b [[::]] and @b [[::]] respectively;
 no other escapes are available.

 @section wxresynchars Regular Expression Character Names

 @ref resyn_overview
 Note that the character names are case sensitive.






 NUL




 '\0'





 SOH




 '\001'





 STX




 '\002'





 ETX




 '\003'





 EOT




 '\004'





 ENQ




 '\005'





 ACK




 '\006'





 BEL




 '\007'





 alert




 '\007'





 BS




 '\010'





 backspace




 '\b'





 HT




 '\011'





 tab




 '\t'





 LF




 '\012'





 newline




 '\n'





 VT




 '\013'





 vertical-tab




 '\v'





 FF




 '\014'





 form-feed




 '\f'





 CR




 '\015'





 carriage-return




 '\r'





 SO




 '\016'





 SI




 '\017'





 DLE




 '\020'





 DC1




 '\021'





 DC2




 '\022'





 DC3




 '\023'





 DC4




 '\024'





 NAK




 '\025'





 SYN




 '\026'





 ETB




 '\027'





 CAN




 '\030'





 EM




 '\031'





 SUB




 '\032'





 ESC




 '\033'





 IS4




 '\034'





 FS




 '\034'





 IS3




 '\035'





 GS




 '\035'





 IS2




 '\036'





 RS




 '\036'





 IS1




 '\037'





 US




 '\037'





 space




 ' '





 exclamation-mark




 '!'





 quotation-mark




 '"'





 number-sign




 '#'





 dollar-sign




 '$'





 percent-sign




 '%'





 ampersand




 ''





 apostrophe




 '\''





 left-parenthesis




 '('





 right-parenthesis




 ')'





 asterisk




 '*'





 plus-sign




 '+'





 comma




 ','





 hyphen




 '-'





 hyphen-minus




 '-'





 period




 '.'





 full-stop




 '.'





 slash




 '/'





 solidus




 '/'





 zero




 '0'





 one




 '1'





 two




 '2'





 three




 '3'





 four




 '4'





 five




 '5'





 six




 '6'





 seven




 '7'





 eight




 '8'





 nine




 '9'





 colon




 ':'





 semicolon




 ';'





 less-than-sign




 ''





 equals-sign




 '='





 greater-than-sign




 ''





 question-mark




 '?'





 commercial-at




 '@'





 left-square-bracket




 '['





 backslash




 '\'





 reverse-solidus




 '\'





 right-square-bracket




 ']'





 circumflex




 '^'





 circumflex-accent




 '^'





 underscore




 '_'





 low-line




 '_'





 grave-accent




 '''





 left-brace




 '{'





 left-curly-bracket




 '{'





 vertical-line




 '|'





 right-brace




 '}'





 right-curly-bracket




 '}'





 tilde




 '~'





 DEL




 '\177'

 */


