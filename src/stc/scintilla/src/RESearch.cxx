// Scintilla source code edit control
/** @file RESearch.cxx
 ** Regular expression search library.
 **/

/*
 * regex - Regular expression pattern matching  and replacement
 *
 * By:  Ozan S. Yigit (oz)
 *      Dept. of Computer Science
 *      York University
 *
 * Original code available from http://www.cs.yorku.ca/~oz/
 * Translation to C++ by Neil Hodgson neilh@scintilla.org
 * Removed all use of register.
 * Converted to modern function prototypes.
 * Put all global/static variables into an object so this code can be
 * used from multiple threads etc.
 *
 * These routines are the PUBLIC DOMAIN equivalents of regex
 * routines as found in 4.nBSD UN*X, with minor extensions.
 *
 * These routines are derived from various implementations found
 * in software tools books, and Conroy's grep. They are NOT derived
 * from licensed/restricted software.
 * For more interesting/academic/complicated implementations,
 * see Henry Spencer's regexp routines, or GNU Emacs pattern
 * matching module.
 *
 * Modification history:
 *
 * $Log$
 * Revision 1.5.2.1  2003/04/08 22:40:19  RD
 * Updated wxSTC to Scintilla 1.51
 *
 * Revision 1.7  2002/09/28 00:33:28  nyamatongwe
 * Fixed problem with character ranges caused by expansion to 8 bits.
 *
 * Revision 1.6  2001/04/29 13:32:10  nyamatongwe
 * Addition of new target methods - versions of ReplaceTarget that take counted
 * strings to allow for nulls, SearchInTarget and Get/SetSearchFlags to use a
 * series of calls rather than a structure.
 * Handling of \000 in search and replace.
 * Handling of /escapes within character ranges of regular expressions.
 * Some handling of bare ^ and $ regular expressions.
 *
 * Revision 1.5  2001/04/20 07:36:09  nyamatongwe
 * Removed DEBUG code that failed to compile on GTK+.
 *
 * Revision 1.4  2001/04/13 03:52:13  nyamatongwe
 * Added URL to find original code to comments.
 *
 * Revision 1.3  2001/04/06 12:24:21  nyamatongwe
 * Made regular expression searching work on a line by line basis, made ^ and
 * $ work, made [set] work, and added a case insensitive option.
 *
 * Revision 1.2  2001/04/05 01:58:04  nyamatongwe
 * Replace target functionality to make find and replace operations faster
 * by diminishing screen updates and allow for \d patterns in the replacement
 * text.
 *
 * Revision 1.1  2001/04/04 12:52:44  nyamatongwe
 * Moved to public domain regular expresion implementation.
 *
 * Revision 1.4  1991/10/17  03:56:42  oz
 * miscellaneous changes, small cleanups etc.
 *
 * Revision 1.3  1989/04/01  14:18:09  oz
 * Change all references to a dfa: this is actually an nfa.
 *
 * Revision 1.2  88/08/28  15:36:04  oz
 * Use a complement bitmap to represent NCL.
 * This removes the need to have seperate
 * code in the PMatch case block - it is
 * just CCL code now.
 *
 * Use the actual CCL code in the CLO
 * section of PMatch. No need for a recursive
 * PMatch call.
 *
 * Use a bitmap table to set char bits in an
 * 8-bit chunk.
 *
 * Interfaces:
 *      RESearch::Compile:        compile a regular expression into a NFA.
 *
 *			char *RESearch::Compile(s)
 *			char *s;
 *
 *      RESearch::Execute:        execute the NFA to match a pattern.
 *
 *			int RESearch::Execute(s)
 *			char *s;
 *
 *	RESearch::ModifyWord		change RESearch::Execute's understanding of what a "word"
 *			looks like (for \< and \>) by adding into the
 *			hidden word-syntax table.
 *
 *			void RESearch::ModifyWord(s)
 *			char *s;
 *
 *      RESearch::Substitute:	substitute the matched portions in a new string.
 *
 *			int RESearch::Substitute(src, dst)
 *			char *src;
 *			char *dst;
 *
 *	re_fail:	failure routine for RESearch::Execute.
 *
 *			void re_fail(msg, op)
 *			char *msg;
 *			char op;
 *
 * Regular Expressions:
 *
 *      [1]     char    matches itself, unless it is a special
 *                      character (metachar): . \ [ ] * + ^ $
 *
 *      [2]     .       matches any character.
 *
 *      [3]     \       matches the character following it, except
 *			when followed by a left or right round bracket,
 *			a digit 1 to 9 or a left or right angle bracket.
 *			(see [7], [8] and [9])
 *			It is used as an escape character for all
 *			other meta-characters, and itself. When used
 *			in a set ([4]), it is treated as an ordinary
 *			character.
 *
 *      [4]     [set]   matches one of the characters in the set.
 *                      If the first character in the set is "^",
 *                      it matches a character NOT in the set, i.e.
 *			complements the set. A shorthand S-E is
 *			used to specify a set of characters S upto
 *			E, inclusive. The special characters "]" and
 *			"-" have no special meaning if they appear
 *			as the first chars in the set.
 *                      examples:        match:
 *
 *                              [a-z]    any lowercase alpha
 *
 *                              [^]-]    any char except ] and -
 *
 *                              [^A-Z]   any char except uppercase
 *                                       alpha
 *
 *                              [a-zA-Z] any alpha
 *
 *      [5]     *       any regular expression form [1] to [4], followed by
 *                      closure char (*) matches zero or more matches of
 *                      that form.
 *
 *      [6]     +       same as [5], except it matches one or more.
 *
 *      [7]             a regular expression in the form [1] to [10], enclosed
 *                      as \(form\) matches what form matches. The enclosure
 *                      creates a set of tags, used for [8] and for
 *                      pattern substution. The tagged forms are numbered
 *			starting from 1.
 *
 *      [8]             a \ followed by a digit 1 to 9 matches whatever a
 *                      previously tagged regular expression ([7]) matched.
 *
 *	[9]	\<	a regular expression starting with a \< construct
 *		\>	and/or ending with a \> construct, restricts the
 *			pattern matching to the beginning of a word, and/or
 *			the end of a word. A word is defined to be a character
 *			string beginning and/or ending with the characters
 *			A-Z a-z 0-9 and _. It must also be preceded and/or
 *			followed by any character outside those mentioned.
 *
 *      [10]            a composite regular expression xy where x and y
 *                      are in the form [1] to [10] matches the longest
 *                      match of x followed by a match for y.
 *
 *      [11]	^	a regular expression starting with a ^ character
 *		$	and/or ending with a $ character, restricts the
 *                      pattern matching to the beginning of the line,
 *                      or the end of line. [anchors] Elsewhere in the
 *			pattern, ^ and $ are treated as ordinary characters.
 *
 *
 * Acknowledgements:
 *
 *	HCR's Hugh Redelmeier has been most helpful in various
 *	stages of development. He convinced me to include BOW
 *	and EOW constructs, originally invented by Rob Pike at
 *	the University of Toronto.
 *
 * References:
 *              Software tools			Kernighan & Plauger
 *              Software tools in Pascal        Kernighan & Plauger
 *              Grep [rsx-11 C dist]            David Conroy
 *		ed - text editor		Un*x Programmer's Manual
 *		Advanced editing on Un*x	B. W. Kernighan
 *		RegExp routines			Henry Spencer
 *
 * Notes:
 *
 *	This implementation uses a bit-set representation for character
 *	classes for speed and compactness. Each character is represented
 *	by one bit in a 128-bit block. Thus, CCL always takes a
 *	constant 16 bytes in the internal nfa, and RESearch::Execute does a single
 *	bit comparison to locate the character in the set.
 *
 * Examples:
 *
 *	pattern:	foo*.*
 *	compile:	CHR f CHR o CLO CHR o END CLO ANY END END
 *	matches:	fo foo fooo foobar fobar foxx ...
 *
 *	pattern:	fo[ob]a[rz]
 *	compile:	CHR f CHR o CCL bitset CHR a CCL bitset END
 *	matches:	fobar fooar fobaz fooaz
 *
 *	pattern:	foo\\+
 *	compile:	CHR f CHR o CHR o CHR \ CLO CHR \ END END
 *	matches:	foo\ foo\\ foo\\\  ...
 *
 *	pattern:	\(foo\)[1-3]\1	(same as foo[1-3]foo)
 *	compile:	BOT 1 CHR f CHR o CHR o EOT 1 CCL bitset REF 1 END
 *	matches:	foo1foo foo2foo foo3foo
 *
 *	pattern:	\(fo.*\)-\1
 *	compile:	BOT 1 CHR f CHR o CLO ANY END EOT 1 CHR - REF 1 END
 *	matches:	foo-foo fo-fo fob-fob foobar-foobar ...
 */

#include "RESearch.h"

#define OKP     1
#define NOP     0

#define CHR     1
#define ANY     2
#define CCL     3
#define BOL     4
#define EOL     5
#define BOT     6
#define EOT     7
#define BOW	8
#define EOW	9
#define REF     10
#define CLO     11

#define END     0

/*
 * The following defines are not meant to be changeable.
 * They are for readability only.
 */
#define BLKIND	0170
#define BITIND	07

#define ASCIIB	0177

const char bitarr[] = {1,2,4,8,16,32,64,'\200'};

#define badpat(x)	(*nfa = END, x)

RESearch::RESearch() {
	Init();
}

RESearch::~RESearch() {
	Clear();
}

void RESearch::Init() {
	sta = NOP;               	/* status of lastpat */
	bol = 0;
	for (int i=0; i<MAXTAG; i++)
		pat[i] = 0;
	for (int j=0; j<BITBLK; j++)
		bittab[j] = 0;
}

void RESearch::Clear() {
	for (int i=0; i<MAXTAG; i++) {
		delete []pat[i];
		pat[i] = 0;
		bopat[i] = NOTFOUND;
		eopat[i] = NOTFOUND;
	}
}

bool RESearch::GrabMatches(CharacterIndexer &ci) {
	bool success = true;
	for (unsigned int i=0; i<MAXTAG; i++) {
		if ((bopat[i] != NOTFOUND) && (eopat[i] != NOTFOUND)) {
			unsigned int len = eopat[i] - bopat[i];
			pat[i] = new char[len + 1];
			if (pat[i]) {
				for (unsigned int j=0; j<len; j++)
					pat[i][j] = ci.CharAt(bopat[i] + j);
				pat[i][len] = '\0';
			} else {
				success = false;
			}
		}
	}
	return success;
}

void RESearch::ChSet(char c) {
	bittab[((c) & BLKIND) >> 3] |= bitarr[(c) & BITIND];
}

void RESearch::ChSetWithCase(char c, bool caseSensitive) {
	if (caseSensitive) {
		ChSet(c);
	} else {
		if ((c >= 'a') && (c <= 'z')) {
			ChSet(c);
			ChSet(static_cast<char>(c - 'a' + 'A'));
		} else if ((c >= 'A') && (c <= 'Z')) {
			ChSet(c);
			ChSet(static_cast<char>(c - 'A' + 'a'));
		} else {
			ChSet(c);
		}
	}
}

const char escapeValue(char ch) {
	switch (ch) {
	case 'a':	return '\a';
	case 'b':	return '\b';
	case 'f':	return '\f';
	case 'n':	return '\n';
	case 'r':	return '\r';
	case 't':	return '\t';
	case 'v':	return '\v';
	}
	return 0;
}

const char *RESearch::Compile(const char *pat, int length, bool caseSensitive) {
	char *mp=nfa;          /* nfa pointer       */
	char *lp;              /* saved pointer..   */
	char *sp=nfa;          /* another one..     */

	int tagi = 0;          /* tag stack index   */
	int tagc = 1;          /* actual tag count  */

	int n;
	char mask;		/* xor mask -CCL/NCL */
	int c1, c2;

	if (!pat || !length)
		if (sta)
			return 0;
		else
			return badpat("No previous regular expression");
	sta = NOP;

	const char *p=pat;               /* pattern pointer   */
	for (int i=0; i<length; i++, p++) {
		lp = mp;
		switch(*p) {

		case '.':               /* match any char..  */
			*mp++ = ANY;
			break;

		case '^':               /* match beginning.. */
			if (p == pat)
				*mp++ = BOL;
			else {
				*mp++ = CHR;
				*mp++ = *p;
			}
			break;

		case '$':               /* match endofline.. */
			if (!*(p+1))
				*mp++ = EOL;
			else {
				*mp++ = CHR;
				*mp++ = *p;
			}
			break;

		case '[':               /* match char class..*/
			*mp++ = CCL;

			i++;
			if (*++p == '^') {
				mask = '\377';
				i++;
				p++;
			} else
				mask = 0;

			if (*p == '-') {		/* real dash */
				i++;
				ChSet(*p++);
			}
			if (*p == ']') {	/* real brace */
				i++;
				ChSet(*p++);
			}
			while (*p && *p != ']') {
				if (*p == '-' && *(p+1) && *(p+1) != ']') {
					i++;
					p++;
					c1 = *(p-2) + 1;
					i++;
					c2 = *p++;
					while (c1 <= c2) {
						ChSetWithCase(static_cast<char>(c1++), caseSensitive);
					}
				} else if (*p == '\\' && *(p+1)) {
					i++;
					p++;
					char escape = escapeValue(*p);
					if (escape)
						ChSetWithCase(escape, caseSensitive);
					else
						ChSetWithCase(*p, caseSensitive);
					i++;
					p++;
				} else {
					i++;
					ChSetWithCase(*p++, caseSensitive);
				}
			}
			if (!*p)
				return badpat("Missing ]");

			for (n = 0; n < BITBLK; bittab[n++] = (char) 0)
				*mp++ = static_cast<char>(mask ^ bittab[n]);

			break;

		case '*':               /* match 0 or more.. */
		case '+':               /* match 1 or more.. */
			if (p == pat)
				return badpat("Empty closure");
			lp = sp;		/* previous opcode */
			if (*lp == CLO)		/* equivalence..   */
				break;
			switch(*lp) {

			case BOL:
			case BOT:
			case EOT:
			case BOW:
			case EOW:
			case REF:
				return badpat("Illegal closure");
			default:
				break;
			}

			if (*p == '+')
				for (sp = mp; lp < sp; lp++)
					*mp++ = *lp;

			*mp++ = END;
			*mp++ = END;
			sp = mp;
			while (--mp > lp)
				*mp = mp[-1];
			*mp = CLO;
			mp = sp;
			break;

		case '\\':              /* tags, backrefs .. */
			i++;
			switch(*++p) {

			case '(':
				if (tagc < MAXTAG) {
					tagstk[++tagi] = tagc;
					*mp++ = BOT;
					*mp++ = static_cast<char>(tagc++);
				}
				else
					return badpat("Too many \\(\\) pairs");
				break;
			case ')':
				if (*sp == BOT)
					return badpat("Null pattern inside \\(\\)");
				if (tagi > 0) {
					*mp++ = static_cast<char>(EOT);
					*mp++ = static_cast<char>(tagstk[tagi--]);
				}
				else
					return badpat("Unmatched \\)");
				break;
			case '<':
				*mp++ = BOW;
				break;
			case '>':
				if (*sp == BOW)
					return badpat("Null pattern inside \\<\\>");
				*mp++ = EOW;
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				n = *p-'0';
				if (tagi > 0 && tagstk[tagi] == n)
					return badpat("Cyclical reference");
				if (tagc > n) {
					*mp++ = static_cast<char>(REF);
					*mp++ = static_cast<char>(n);
				}
				else
					return badpat("Undetermined reference");
				break;
			case 'a':
			case 'b':
			case 'n':
			case 'f':
			case 'r':
			case 't':
			case 'v':
				*mp++ = CHR;
				*mp++ = escapeValue(*p);
				break;
			default:
				*mp++ = CHR;
				*mp++ = *p;
			}
			break;

		default :               /* an ordinary char  */
			if (caseSensitive) {
				*mp++ = CHR;
				*mp++ = *p;
			} else {
				*mp++ = CCL;
				mask = 0;
				ChSetWithCase(*p, false);
				for (n = 0; n < BITBLK; bittab[n++] = (char) 0)
					*mp++ = static_cast<char>(mask ^ bittab[n]);
			}
			break;
		}
		sp = lp;
	}
	if (tagi > 0)
		return badpat("Unmatched \\(");
	*mp = END;
	sta = OKP;
	return 0;
}

/*
 * RESearch::Execute:
 * 	execute nfa to find a match.
 *
 *	special cases: (nfa[0])
 *		BOL
 *			Match only once, starting from the
 *			beginning.
 *		CHR
 *			First locate the character without
 *			calling PMatch, and if found, call
 *			PMatch for the remaining string.
 *		END
 *			RESearch::Compile failed, poor luser did not
 *			check for it. Fail fast.
 *
 *	If a match is found, bopat[0] and eopat[0] are set
 *	to the beginning and the end of the matched fragment,
 *	respectively.
 *
 */

int RESearch::Execute(CharacterIndexer &ci, int lp, int endp) {
	char c;
	int ep = NOTFOUND;
	char *ap = nfa;

	bol = lp;
	failure = 0;

	Clear();

	switch(*ap) {

	case BOL:			/* anchored: match from BOL only */
		ep = PMatch(ci, lp, endp, ap);
		break;
	case EOL:			/* just searching for end of line normal path doesn't work */
		if (*(ap+1) == END) {
			lp = endp;
			ep = lp;
			break;
		} else {
			return 0;
		}
	case CHR:			/* ordinary char: locate it fast */
		c = *(ap+1);
		while ((lp < endp) && (ci.CharAt(lp) != c))
			lp++;
		if (lp >= endp)		/* if EOS, fail, else fall thru. */
			return 0;
	default:			/* regular matching all the way. */
		while (lp < endp) {
			ep = PMatch(ci, lp, endp, ap);
			if (ep != NOTFOUND)
				break;
			lp++;
		}
		break;
	case END:			/* munged automaton. fail always */
		return 0;
	}
	if (ep == NOTFOUND)
		return 0;

	bopat[0] = lp;
	eopat[0] = ep;
	return 1;
}

/*
 * PMatch: internal routine for the hard part
 *
 * 	This code is partly snarfed from an early grep written by
 *	David Conroy. The backref and tag stuff, and various other
 *	innovations are by oz.
 *
 *	special case optimizations: (nfa[n], nfa[n+1])
 *		CLO ANY
 *			We KNOW .* will match everything upto the
 *			end of line. Thus, directly go to the end of
 *			line, without recursive PMatch calls. As in
 *			the other closure cases, the remaining pattern
 *			must be matched by moving backwards on the
 *			string recursively, to find a match for xy
 *			(x is ".*" and y is the remaining pattern)
 *			where the match satisfies the LONGEST match for
 *			x followed by a match for y.
 *		CLO CHR
 *			We can again scan the string forward for the
 *			single char and at the point of failure, we
 *			execute the remaining nfa recursively, same as
 *			above.
 *
 *	At the end of a successful match, bopat[n] and eopat[n]
 *	are set to the beginning and end of subpatterns matched
 *	by tagged expressions (n = 1 to 9).
 *
 */

extern void re_fail(char *,char);

/*
 * character classification table for word boundary operators BOW
 * and EOW. the reason for not using ctype macros is that we can
 * let the user add into our own table. see RESearch::ModifyWord. This table
 * is not in the bitset form, since we may wish to extend it in the
 * future for other character classifications.
 *
 *	TRUE for 0-9 A-Z a-z _
 */
static char chrtyp[MAXCHR] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 1, 0, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 0, 0, 0, 0, 0
	};

#define inascii(x)	(0177&(x))
#define iswordc(x) 	chrtyp[inascii(x)]
#define isinset(x,y) 	((x)[((y)&BLKIND)>>3] & bitarr[(y)&BITIND])

/*
 * skip values for CLO XXX to skip past the closure
 */

#define ANYSKIP	2 	/* [CLO] ANY END ...	     */
#define CHRSKIP	3	/* [CLO] CHR chr END ...     */
#define CCLSKIP 34	/* [CLO] CCL 32bytes END ... */

int RESearch::PMatch(CharacterIndexer &ci, int lp, int endp, char *ap) {
	int op, c, n;
	int e;		/* extra pointer for CLO */
	int bp;		/* beginning of subpat.. */
	int ep;		/* ending of subpat..	 */
	int are;			/* to save the line ptr. */

	while ((op = *ap++) != END)
		switch(op) {

		case CHR:
			if (ci.CharAt(lp++) != *ap++)
				return NOTFOUND;
			break;
		case ANY:
			if (lp++ >= endp)
				return NOTFOUND;
			break;
		case CCL:
			c = ci.CharAt(lp++);
			if (!isinset(ap,c))
				return NOTFOUND;
			ap += BITBLK;
			break;
		case BOL:
			if (lp != bol)
				return NOTFOUND;
			break;
		case EOL:
			if (lp < endp)
				return NOTFOUND;
			break;
		case BOT:
			bopat[*ap++] = lp;
			break;
		case EOT:
			eopat[*ap++] = lp;
			break;
 		case BOW:
			if (lp!=bol && iswordc(ci.CharAt(lp-1)) || !iswordc(ci.CharAt(lp)))
				return NOTFOUND;
			break;
		case EOW:
			if (lp==bol || !iswordc(ci.CharAt(lp-1)) || iswordc(ci.CharAt(lp)))
				return NOTFOUND;
			break;
		case REF:
			n = *ap++;
			bp = bopat[n];
			ep = eopat[n];
			while (bp < ep)
				if (ci.CharAt(bp++) != ci.CharAt(lp++))
					return NOTFOUND;
			break;
		case CLO:
			are = lp;
			switch(*ap) {

			case ANY:
				while (lp < endp)
					lp++;
				n = ANYSKIP;
				break;
			case CHR:
				c = *(ap+1);
				while ((lp < endp) && (c == ci.CharAt(lp)))
					lp++;
				n = CHRSKIP;
				break;
			case CCL:
				while ((lp < endp) && isinset(ap+1,ci.CharAt(lp)))
					lp++;
				n = CCLSKIP;
				break;
			default:
				failure = true;
				//re_fail("closure: bad nfa.", *ap);
				return NOTFOUND;
			}

			ap += n;

			while (lp >= are) {
				if ((e = PMatch(ci, lp, endp, ap)) != NOTFOUND)
					return e;
				--lp;
			}
			return NOTFOUND;
		default:
			//re_fail("RESearch::Execute: bad nfa.", static_cast<char>(op));
			return NOTFOUND;
		}
	return lp;
}

/*
 * RESearch::ModifyWord:
 *	add new characters into the word table to change RESearch::Execute's
 *	understanding of what a word should look like. Note that we
 *	only accept additions into the word definition.
 *
 *	If the string parameter is 0 or null string, the table is
 *	reset back to the default containing A-Z a-z 0-9 _. [We use
 *	the compact bitset representation for the default table]
 */

static char deftab[16] = {
	0, 0, 0, 0, 0, 0, '\377', 003, '\376', '\377', '\377', '\207',
	'\376', '\377', '\377', 007
};

void RESearch::ModifyWord(char *s) {
	int i;

	if (!s || !*s) {
		for (i = 0; i < MAXCHR; i++)
			if (!isinset(deftab,i))
				iswordc(i) = 0;
	}
	else
		while(*s)
			iswordc(*s++) = 1;
}

/*
 * RESearch::Substitute:
 *	substitute the matched portions of the src in dst.
 *
 *	&	substitute the entire matched pattern.
 *
 *	\digit	substitute a subpattern, with the given	tag number.
 *		Tags are numbered from 1 to 9. If the particular
 *		tagged subpattern does not exist, null is substituted.
 */
int RESearch::Substitute(CharacterIndexer &ci, char *src, char *dst) {
	char c;
	int  pin;
	int bp;
	int ep;

	if (!*src || !bopat[0])
		return 0;

	while ((c = *src++) != 0) {
		switch(c) {

		case '&':
			pin = 0;
			break;

		case '\\':
			c = *src++;
			if (c >= '0' && c <= '9') {
				pin = c - '0';
				break;
			}

		default:
			*dst++ = c;
			continue;
		}

		if ((bp = bopat[pin]) != 0 && (ep = eopat[pin]) != 0) {
			while (ci.CharAt(bp) && bp < ep)
				*dst++ = ci.CharAt(bp++);
			if (bp < ep)
				return 0;
		}
	}
	*dst = (char) 0;
	return 1;
}
