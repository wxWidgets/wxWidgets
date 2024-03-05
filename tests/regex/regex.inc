/*
 * Test data for wxRegEx (UTF-8 encoded)
 * 
 * Generated Sun Mar 14 09:58:58 2021 by regex.pl from the following files:
 * 
 *   reg.test: Copyright (c) 1998, 1999 Henry Spencer. All rights reserved.
 *   wxreg.test: Copyright (c) 2004 Mike Wetherell.
 * 
 * Test types:
 *  	e	compile error expected
 *  	f	match failure expected
 *  	m	successful match
 *  	i	successful match with -indices (used in checking things like
 *  		nonparticipating subexpressions)
 *  	p	unsuccessful match with -indices (!!) (used in checking
 *  		partial-match reporting)
 * 
 * Flag characters:
 *  	-	no-op (placeholder)
 *  	+	provide fake xy equivalence class and ch collating element
 *  	%	force small state-set cache in matcher (to test cache replace)
 *  	^	beginning of string is not beginning of line
 *  	$	end of string is not end of line
 *  	*	test is Unicode-specific, needs big character set
 * 
 *  	&	test as both ARE and BRE
 *  	b	BRE
 *  	e	ERE
 *  	a	turn advanced-features bit on (error unless ERE already)
 *  	q	literal string, no metacharacters at all
 * 
 *  	i	case-independent matching
 *  	o	("opaque") no subexpression capture
 *  	p	newlines are half-magic, excluded from . and [^ only
 *  	w	newlines are half-magic, significant to ^ and $ only
 *  	n	newlines are fully magic, both effects
 *  	x	expanded RE syntax
 *  	t	incomplete-match reporting
 * 
 *  	A	backslash-_a_lphanumeric seen
 *  	B	ERE/ARE literal-_b_race heuristic used
 *  	E	backslash (_e_scape) seen within []
 *  	H	looka_h_ead constraint seen
 *  	I	_i_mpossible to match
 *  	L	_l_ocale-specific construct seen
 *  	M	unportable (_m_achine-specific) construct seen
 *  	N	RE can match empty (_n_ull) string
 *  	P	non-_P_OSIX construct seen
 *  	Q	{} _q_uantifier seen
 *  	R	back _r_eference seen
 *  	S	POSIX-un_s_pecified syntax seen
 *  	T	prefers shortest (_t_iny)
 *  	U	saw original-POSIX botch:  unmatched right paren in ERE (_u_gh)
 */


/*
 * 1 basic sanity checks
 */

TEST_CASE("regex::basic sanity checks", "[regex][regex_1][builtin]")
{
    CheckRE("m", "1", "&", "abc", "abc", "abc", NULL);
    CheckRE("f", "2", "&", "abc", "def", NULL);
    CheckRE("m", "3", "&", "abc", "xyabxabce", "abc", NULL);
}


/*
 * 2 invalid option combinations
 */

TEST_CASE("regex::invalid option combinations", "[regex][regex_2][builtin]")
{
    CheckRE("e", "1", "qe", "a", "INVARG", NULL);
    CheckRE("e", "2", "qa", "a", "INVARG", NULL);
    CheckRE("e", "3", "qx", "a", "INVARG", NULL);
    CheckRE("e", "4", "qn", "a", "INVARG", NULL);
    CheckRE("e", "5", "ba", "a", "INVARG", NULL);
}


/*
 * 3 basic syntax
 */

TEST_CASE("regex::basic syntax", "[regex][regex_3][builtin]")
{
    CheckRE("i", "1", "&NS", "", "a", "0 -1", NULL);
    CheckRE("m", "2", "NS", "a|", "a", "a", NULL);
    CheckRE("m", "3", "-", "a|b", "a", "a", NULL);
    CheckRE("m", "4", "-", "a|b", "b", "b", NULL);
    CheckRE("m", "5", "NS", "a||b", "b", "b", NULL);
    CheckRE("m", "6", "&", "ab", "ab", "ab", NULL);
}


/*
 * 4 parentheses
 */

TEST_CASE("regex::parentheses", "[regex][regex_4][builtin]")
{
    CheckRE("m", "1", "-", "(a)e", "ae", "ae", "a", NULL);
    CheckRE("m", "2", "o", "(a)e", "ae", NULL);
    CheckRE("m", "3", "b", "\\(a\\)b", "ab", "ab", "a", NULL);
    CheckRE("m", "4", "-", "a((b)c)", "abc", "abc", "bc", "b", NULL);
    CheckRE("m", "5", "-", "a(b)(c)", "abc", "abc", "b", "c", NULL);
    CheckRE("e", "6", "-", "a(b", "EPAREN", NULL);
    CheckRE("e", "7", "b", "a\\(b", "EPAREN", NULL);
    CheckRE("m", "8", "eU", "a)b", "a)b", "a)b", NULL);
    CheckRE("e", "9", "-", "a)b", "EPAREN", NULL);
    CheckRE("e", "10", "b", "a\\)b", "EPAREN", NULL);
    CheckRE("m", "11", "P", "a(?:b)c", "abc", "abc", NULL);
    CheckRE("e", "12", "e", "a(?:b)c", "BADRPT", NULL);
    CheckRE("i", "13", "S", "a()b", "ab", "0 1", "1 0", NULL);
    CheckRE("m", "14", "SP", "a(?:)b", "ab", "ab", NULL);
    CheckRE("i", "15", "S", "a(|b)c", "ac", "0 1", "1 0", NULL);
    CheckRE("m", "16", "S", "a(b|)c", "abc", "abc", "b", NULL);
}


/*
 * 5 simple one-char matching
 */

TEST_CASE("regex::simple one-char matching", "[regex][regex_5][builtin]")
{
    CheckRE("m", "1", "&", "a.b", "axb", "axb", NULL);
    CheckRE("f", "2", "&n", "a.b", "a\nb", NULL);
    CheckRE("m", "3", "&", "a[bc]d", "abd", "abd", NULL);
    CheckRE("m", "4", "&", "a[bc]d", "acd", "acd", NULL);
    CheckRE("f", "5", "&", "a[bc]d", "aed", NULL);
    CheckRE("f", "6", "&", "a[^bc]d", "abd", NULL);
    CheckRE("m", "7", "&", "a[^bc]d", "aed", "aed", NULL);
    CheckRE("f", "8", "&p", "a[^bc]d", "a\nd", NULL);
}


/*
 * 6 context-dependent syntax
 */

TEST_CASE("regex::context-dependent syntax", "[regex][regex_6][builtin]")
{
    CheckRE("e", "1", "-", "*", "BADRPT", NULL);
    CheckRE("m", "2", "b", "*", "*", "*", NULL);
    CheckRE("m", "3", "b", "\\(*\\)", "*", "*", "*", NULL);
    CheckRE("e", "4", "-", "(*)", "BADRPT", NULL);
    CheckRE("m", "5", "b", "^*", "*", "*", NULL);
    CheckRE("e", "6", "-", "^*", "BADRPT", NULL);
    CheckRE("f", "7", "&", "^b", "^b", NULL);
    CheckRE("m", "8", "b", "x^", "x^", "x^", NULL);
    CheckRE("f", "9", "I", "x^", "x", NULL);
    CheckRE("m", "10", "n", "\n^", "x\nb", "\n", NULL);
    CheckRE("f", "11", "bS", "\\(^b\\)", "^b", NULL);
    CheckRE("m", "12", "-", "(^b)", "b", "b", "b", NULL);
    CheckRE("m", "13", "&", "x$", "x", "x", NULL);
    CheckRE("m", "14", "bS", "\\(x$\\)", "x", "x", "x", NULL);
    CheckRE("m", "15", "-", "(x$)", "x", "x", "x", NULL);
    CheckRE("m", "16", "b", "x$y", "x$y", "x$y", NULL);
    CheckRE("f", "17", "I", "x$y", "xy", NULL);
    CheckRE("m", "18", "n", "x$\n", "x\n", "x\n", NULL);
    CheckRE("e", "19", "-", "+", "BADRPT", NULL);
    CheckRE("e", "20", "-", "?", "BADRPT", NULL);
}


/*
 * 7 simple quantifiers
 */

TEST_CASE("regex::simple quantifiers", "[regex][regex_7][builtin]")
{
    CheckRE("m", "1", "&N", "a*", "aa", "aa", NULL);
    CheckRE("i", "2", "&N", "a*", "b", "0 -1", NULL);
    CheckRE("m", "3", "-", "a+", "aa", "aa", NULL);
    CheckRE("m", "4", "-", "a?b", "ab", "ab", NULL);
    CheckRE("m", "5", "-", "a?b", "b", "b", NULL);
    CheckRE("e", "6", "-", "**", "BADRPT", NULL);
    CheckRE("m", "7", "bN", "**", "***", "***", NULL);
    CheckRE("e", "8", "&", "a**", "BADRPT", NULL);
    CheckRE("e", "9", "&", "a**b", "BADRPT", NULL);
    CheckRE("e", "10", "&", "***", "BADRPT", NULL);
    CheckRE("e", "11", "-", "a++", "BADRPT", NULL);
    CheckRE("e", "12", "-", "a?+", "BADRPT", NULL);
    CheckRE("e", "13", "-", "a?*", "BADRPT", NULL);
    CheckRE("e", "14", "-", "a+*", "BADRPT", NULL);
    CheckRE("e", "15", "-", "a*+", "BADRPT", NULL);
}


/*
 * 8 braces
 */

TEST_CASE("regex::braces", "[regex][regex_8][builtin]")
{
    CheckRE("m", "1", "NQ", "a{0,1}", "", "", NULL);
    CheckRE("m", "2", "NQ", "a{0,1}", "ac", "a", NULL);
    CheckRE("e", "3", "-", "a{1,0}", "BADBR", NULL);
    CheckRE("e", "4", "-", "a{1,2,3}", "BADBR", NULL);
    CheckRE("e", "5", "-", "a{257}", "BADBR", NULL);
    CheckRE("e", "6", "-", "a{1000}", "BADBR", NULL);
    CheckRE("e", "7", "-", "a{1", "EBRACE", NULL);
    CheckRE("e", "8", "-", "a{1n}", "BADBR", NULL);
    CheckRE("m", "9", "BS", "a{b", "a{b", "a{b", NULL);
    CheckRE("m", "10", "BS", "a{", "a{", "a{", NULL);
    CheckRE("m", "11", "bQ", "a\\{0,1\\}b", "cb", "b", NULL);
    CheckRE("e", "12", "b", "a\\{0,1", "EBRACE", NULL);
    CheckRE("e", "13", "-", "a{0,1\\", "BADBR", NULL);
    CheckRE("m", "14", "Q", "a{0}b", "ab", "b", NULL);
    CheckRE("m", "15", "Q", "a{0,0}b", "ab", "b", NULL);
    CheckRE("m", "16", "Q", "a{0,1}b", "ab", "ab", NULL);
    CheckRE("m", "17", "Q", "a{0,2}b", "b", "b", NULL);
    CheckRE("m", "18", "Q", "a{0,2}b", "aab", "aab", NULL);
    CheckRE("m", "19", "Q", "a{0,}b", "aab", "aab", NULL);
    CheckRE("m", "20", "Q", "a{1,1}b", "aab", "ab", NULL);
    CheckRE("m", "21", "Q", "a{1,3}b", "aaaab", "aaab", NULL);
    CheckRE("f", "22", "Q", "a{1,3}b", "b", NULL);
    CheckRE("m", "23", "Q", "a{1,}b", "aab", "aab", NULL);
    CheckRE("f", "24", "Q", "a{2,3}b", "ab", NULL);
    CheckRE("m", "25", "Q", "a{2,3}b", "aaaab", "aaab", NULL);
    CheckRE("f", "26", "Q", "a{2,}b", "ab", NULL);
    CheckRE("m", "27", "Q", "a{2,}b", "aaaab", "aaaab", NULL);
}


/*
 * 9 brackets
 */

TEST_CASE("regex::brackets", "[regex][regex_9][builtin]")
{
    CheckRE("m", "1", "&", "a[bc]", "ac", "ac", NULL);
    CheckRE("m", "2", "&", "a[-]", "a-", "a-", NULL);
    CheckRE("m", "3", "&", "a[[.-.]]", "a-", "a-", NULL);
    CheckRE("m", "4", "&L", "a[[.zero.]]", "a0", "a0", NULL);
    CheckRE("m", "5", "&LM", "a[[.zero.]-9]", "a2", "a2", NULL);
    CheckRE("m", "6", "&M", "a[0-[.9.]]", "a2", "a2", NULL);
    CheckRE("m", "7", "&+L", "a[[=x=]]", "ax", "ax", NULL);
    CheckRE("m", "8", "&+L", "a[[=x=]]", "ay", "ay", NULL);
    CheckRE("f", "9", "&+L", "a[[=x=]]", "az", NULL);
    CheckRE("e", "10", "&", "a[0-[=x=]]", "ERANGE", NULL);
    CheckRE("m", "11", "&L", "a[[:digit:]]", "a0", "a0", NULL);
    CheckRE("e", "12", "&", "a[[:woopsie:]]", "ECTYPE", NULL);
    CheckRE("f", "13", "&L", "a[[:digit:]]", "ab", NULL);
    CheckRE("e", "14", "&", "a[0-[:digit:]]", "ERANGE", NULL);
    CheckRE("m", "15", "&LP", "[[:<:]]a", "a", "a", NULL);
    CheckRE("m", "16", "&LP", "a[[:>:]]", "a", "a", NULL);
    CheckRE("e", "17", "&", "a[[..]]b", "ECOLLATE", NULL);
    CheckRE("e", "18", "&", "a[[==]]b", "ECOLLATE", NULL);
    CheckRE("e", "19", "&", "a[[::]]b", "ECTYPE", NULL);
    CheckRE("e", "20", "&", "a[[.a", "EBRACK", NULL);
    CheckRE("e", "21", "&", "a[[=a", "EBRACK", NULL);
    CheckRE("e", "22", "&", "a[[:a", "EBRACK", NULL);
    CheckRE("e", "23", "&", "a[", "EBRACK", NULL);
    CheckRE("e", "24", "&", "a[b", "EBRACK", NULL);
    CheckRE("e", "25", "&", "a[b-", "EBRACK", NULL);
    CheckRE("e", "26", "&", "a[b-c", "EBRACK", NULL);
    CheckRE("m", "27", "&M", "a[b-c]", "ab", "ab", NULL);
    CheckRE("m", "28", "&", "a[b-b]", "ab", "ab", NULL);
    CheckRE("m", "29", "&M", "a[1-2]", "a2", "a2", NULL);
    CheckRE("e", "30", "&", "a[c-b]", "ERANGE", NULL);
    CheckRE("e", "31", "&", "a[a-b-c]", "ERANGE", NULL);
    CheckRE("m", "32", "&M", "a[--?]b", "a?b", "a?b", NULL);
    CheckRE("m", "33", "&", "a[---]b", "a-b", "a-b", NULL);
    CheckRE("m", "34", "&", "a[]b]c", "a]c", "a]c", NULL);
    CheckRE("m", "35", "EP", "a[\\]]b", "a]b", "a]b", NULL);
    CheckRE("f", "36", "bE", "a[\\]]b", "a]b", NULL);
    CheckRE("m", "37", "bE", "a[\\]]b", "a\\]b", "a\\]b", NULL);
    CheckRE("m", "38", "eE", "a[\\]]b", "a\\]b", "a\\]b", NULL);
    CheckRE("m", "39", "EP", "a[\\\\]b", "a\\b", "a\\b", NULL);
    CheckRE("m", "40", "eE", "a[\\\\]b", "a\\b", "a\\b", NULL);
    CheckRE("m", "41", "bE", "a[\\\\]b", "a\\b", "a\\b", NULL);
    CheckRE("e", "42", "-", "a[\\Z]b", "EESCAPE", NULL);
    CheckRE("m", "43", "&", "a[[b]c", "a[c", "a[c", NULL);
    CheckRE("m", "44", "EMP*", "a[\\u00fe-\\u0507][\\u00ff-\\u0300]b", "a\304\202\313\277b", "a\304\202\313\277b", NULL);
}


/*
 * 10 anchors and newlines
 */

TEST_CASE("regex::anchors and newlines", "[regex][regex_10][builtin]")
{
    CheckRE("m", "1", "&", "^a", "a", "a", NULL);
    CheckRE("f", "2", "&^", "^a", "a", NULL);
    CheckRE("i", "3", "&N", "^", "a", "0 -1", NULL);
    CheckRE("i", "4", "&", "a$", "aba", "2 2", NULL);
    CheckRE("f", "5", "&$", "a$", "a", NULL);
    CheckRE("i", "6", "&N", "$", "ab", "2 1", NULL);
    CheckRE("m", "7", "&n", "^a", "a", "a", NULL);
    CheckRE("m", "8", "&n", "^a", "b\na", "a", NULL);
    CheckRE("i", "9", "&w", "^a", "a\na", "0 0", NULL);
    CheckRE("i", "10", "&n^", "^a", "a\na", "2 2", NULL);
    CheckRE("m", "11", "&n", "a$", "a", "a", NULL);
    CheckRE("m", "12", "&n", "a$", "a\nb", "a", NULL);
    CheckRE("i", "13", "&n", "a$", "a\na", "0 0", NULL);
    CheckRE("i", "14", "N", "^^", "a", "0 -1", NULL);
    CheckRE("m", "15", "b", "^^", "^", "^", NULL);
    CheckRE("i", "16", "N", "$$", "a", "1 0", NULL);
    CheckRE("m", "17", "b", "$$", "$", "$", NULL);
    CheckRE("m", "18", "&N", "^$", "", "", NULL);
    CheckRE("f", "19", "&N", "^$", "a", NULL);
    CheckRE("i", "20", "&nN", "^$", "a\n\nb", "2 1", NULL);
    CheckRE("m", "21", "N", "$^", "", "", NULL);
    CheckRE("m", "22", "b", "$^", "$^", "$^", NULL);
    CheckRE("m", "23", "P", "\\Aa", "a", "a", NULL);
    CheckRE("m", "24", "^P", "\\Aa", "a", "a", NULL);
    CheckRE("f", "25", "^nP", "\\Aa", "b\na", NULL);
    CheckRE("m", "26", "P", "a\\Z", "a", "a", NULL);
    CheckRE("m", "27", "$P", "a\\Z", "a", "a", NULL);
    CheckRE("f", "28", "$nP", "a\\Z", "a\nb", NULL);
    CheckRE("e", "29", "-", "^*", "BADRPT", NULL);
    CheckRE("e", "30", "-", "$*", "BADRPT", NULL);
    CheckRE("e", "31", "-", "\\A*", "BADRPT", NULL);
    CheckRE("e", "32", "-", "\\Z*", "BADRPT", NULL);
}


/*
 * 11 boundary constraints
 */

TEST_CASE("regex::boundary constraints", "[regex][regex_11][builtin]")
{
    CheckRE("m", "1", "&LP", "[[:<:]]a", "a", "a", NULL);
    CheckRE("m", "2", "&LP", "[[:<:]]a", "-a", "a", NULL);
    CheckRE("f", "3", "&LP", "[[:<:]]a", "ba", NULL);
    CheckRE("m", "4", "&LP", "a[[:>:]]", "a", "a", NULL);
    CheckRE("m", "5", "&LP", "a[[:>:]]", "a-", "a", NULL);
    CheckRE("f", "6", "&LP", "a[[:>:]]", "ab", NULL);
    CheckRE("m", "7", "bLP", "\\<a", "a", "a", NULL);
    CheckRE("f", "8", "bLP", "\\<a", "ba", NULL);
    CheckRE("m", "9", "bLP", "a\\>", "a", "a", NULL);
    CheckRE("f", "10", "bLP", "a\\>", "ab", NULL);
    CheckRE("m", "11", "LP", "\\ya", "a", "a", NULL);
    CheckRE("f", "12", "LP", "\\ya", "ba", NULL);
    CheckRE("m", "13", "LP", "a\\y", "a", "a", NULL);
    CheckRE("f", "14", "LP", "a\\y", "ab", NULL);
    CheckRE("m", "15", "LP", "a\\Y", "ab", "a", NULL);
    CheckRE("f", "16", "LP", "a\\Y", "a-", NULL);
    CheckRE("f", "17", "LP", "a\\Y", "a", NULL);
    CheckRE("f", "18", "LP", "-\\Y", "-a", NULL);
    CheckRE("m", "19", "LP", "-\\Y", "-%", "-", NULL);
    CheckRE("f", "20", "LP", "\\Y-", "a-", NULL);
    CheckRE("e", "21", "-", "[[:<:]]*", "BADRPT", NULL);
    CheckRE("e", "22", "-", "[[:>:]]*", "BADRPT", NULL);
    CheckRE("e", "23", "b", "\\<*", "BADRPT", NULL);
    CheckRE("e", "24", "b", "\\>*", "BADRPT", NULL);
    CheckRE("e", "25", "-", "\\y*", "BADRPT", NULL);
    CheckRE("e", "26", "-", "\\Y*", "BADRPT", NULL);
    CheckRE("m", "27", "LP", "\\ma", "a", "a", NULL);
    CheckRE("f", "28", "LP", "\\ma", "ba", NULL);
    CheckRE("m", "29", "LP", "a\\M", "a", "a", NULL);
    CheckRE("f", "30", "LP", "a\\M", "ab", NULL);
    CheckRE("f", "31", "ILP", "\\Ma", "a", NULL);
    CheckRE("f", "32", "ILP", "a\\m", "a", NULL);
}


/*
 * 12 character classes
 */

TEST_CASE("regex::character classes", "[regex][regex_12][builtin]")
{
    CheckRE("m", "1", "LP", "a\\db", "a0b", "a0b", NULL);
    CheckRE("f", "2", "LP", "a\\db", "axb", NULL);
    CheckRE("f", "3", "LP", "a\\Db", "a0b", NULL);
    CheckRE("m", "4", "LP", "a\\Db", "axb", "axb", NULL);
    CheckRE("m", "5", "LP", "a\\sb", "a b", "a b", NULL);
    CheckRE("m", "6", "LP", "a\\sb", "a\tb", "a\tb", NULL);
    CheckRE("m", "7", "LP", "a\\sb", "a\nb", "a\nb", NULL);
    CheckRE("f", "8", "LP", "a\\sb", "axb", NULL);
    CheckRE("m", "9", "LP", "a\\Sb", "axb", "axb", NULL);
    CheckRE("f", "10", "LP", "a\\Sb", "a b", NULL);
    CheckRE("m", "11", "LP", "a\\wb", "axb", "axb", NULL);
    CheckRE("f", "12", "LP", "a\\wb", "a-b", NULL);
    CheckRE("f", "13", "LP", "a\\Wb", "axb", NULL);
    CheckRE("m", "14", "LP", "a\\Wb", "a-b", "a-b", NULL);
    CheckRE("m", "15", "LP", "\\y\\w+z\\y", "adze-guz", "guz", NULL);
    CheckRE("m", "16", "LPE", "a[\\d]b", "a1b", "a1b", NULL);
    CheckRE("m", "17", "LPE", "a[\\s]b", "a b", "a b", NULL);
    CheckRE("m", "18", "LPE", "a[\\w]b", "axb", "axb", NULL);
}


/*
 * 13 escapes
 */

TEST_CASE("regex::escapes", "[regex][regex_13][builtin]")
{
    CheckRE("e", "1", "&", "a\\", "EESCAPE", NULL);
    CheckRE("m", "2", "-", "a\\<b", "a<b", "a<b", NULL);
    CheckRE("m", "3", "e", "a\\<b", "a<b", "a<b", NULL);
    CheckRE("m", "4", "bAS", "a\\wb", "awb", "awb", NULL);
    CheckRE("m", "5", "eAS", "a\\wb", "awb", "awb", NULL);
    CheckRE("m", "6", "PL", "a\\ab", "a\ab", "a\ab", NULL);
    CheckRE("m", "7", "P", "a\\bb", "a\bb", "a\bb", NULL);
    CheckRE("m", "8", "P", "a\\Bb", "a\\b", "a\\b", NULL);
    CheckRE("m", "9", "MP", "a\\chb", "a\bb", "a\bb", NULL);
    CheckRE("m", "10", "MP", "a\\cHb", "a\bb", "a\bb", NULL);
    CheckRE("m", "11", "LMP", "a\\e", "a\033", "a\033", NULL);
    CheckRE("m", "12", "P", "a\\fb", "a\fb", "a\fb", NULL);
    CheckRE("m", "13", "P", "a\\nb", "a\nb", "a\nb", NULL);
    CheckRE("m", "14", "P", "a\\rb", "a\rb", "a\rb", NULL);
    CheckRE("m", "15", "P", "a\\tb", "a\tb", "a\tb", NULL);
    CheckRE("m", "16", "P", "a\\u0008x", "a\bx", "a\bx", NULL);
    CheckRE("e", "17", "-", "a\\u008x", "EESCAPE", NULL);
    CheckRE("m", "18", "P", "a\\u00088x", "a\b8x", "a\b8x", NULL);
    CheckRE("m", "19", "P", "a\\U00000008x", "a\bx", "a\bx", NULL);
    CheckRE("e", "20", "-", "a\\U0000008x", "EESCAPE", NULL);
    CheckRE("m", "21", "P", "a\\vb", "a\vb", "a\vb", NULL);
    CheckRE("m", "22", "MP", "a\\x08x", "a\bx", "a\bx", NULL);
    CheckRE("e", "23", "-", "a\\xq", "EESCAPE", NULL);
    CheckRE("m", "24", "MP", "a\\x0008x", "a\bx", "a\bx", NULL);
    CheckRE("e", "25", "-", "a\\z", "EESCAPE", NULL);
    CheckRE("m", "26", "MP", "a\\010b", "a\bb", "a\bb", NULL);
}


/*
 * 14 back references
 */

TEST_CASE("regex::back references", "[regex][regex_14][builtin]")
{
    CheckRE("m", "1", "RP", "a(b*)c\\1", "abbcbb", "abbcbb", "bb", NULL);
    CheckRE("m", "2", "RP", "a(b*)c\\1", "ac", "ac", "", NULL);
    CheckRE("f", "3", "RP", "a(b*)c\\1", "abbcb", NULL);
    CheckRE("m", "4", "RP", "a(b*)\\1", "abbcbb", "abb", "b", NULL);
    CheckRE("m", "5", "RP", "a(b|bb)\\1", "abbcbb", "abb", "b", NULL);
    CheckRE("m", "6", "RP", "a([bc])\\1", "abb", "abb", "b", NULL);
    CheckRE("f", "7", "RP", "a([bc])\\1", "abc", NULL);
    CheckRE("m", "8", "RP", "a([bc])\\1", "abcabb", "abb", "b", NULL);
    CheckRE("f", "9", "RP", "a([bc])*\\1", "abc", NULL);
    CheckRE("f", "10", "RP", "a([bc])\\1", "abB", NULL);
    CheckRE("m", "11", "iRP", "a([bc])\\1", "abB", "abB", "b", NULL);
    CheckRE("m", "12", "RP", "a([bc])\\1+", "abbb", "abbb", "b", NULL);
    CheckRE("m", "13", "QRP", "a([bc])\\1{3,4}", "abbbb", "abbbb", "b", NULL);
    CheckRE("f", "14", "QRP", "a([bc])\\1{3,4}", "abbb", NULL);
    CheckRE("m", "15", "RP", "a([bc])\\1*", "abbb", "abbb", "b", NULL);
    CheckRE("m", "16", "RP", "a([bc])\\1*", "ab", "ab", "b", NULL);
    CheckRE("m", "17", "RP", "a([bc])(\\1*)", "ab", "ab", "b", "", NULL);
    CheckRE("e", "18", "-", "a((b)\\1)", "ESUBREG", NULL);
    CheckRE("e", "19", "-", "a(b)c\\2", "ESUBREG", NULL);
    CheckRE("m", "20", "bR", "a\\(b*\\)c\\1", "abbcbb", "abbcbb", "bb", NULL);
}


/*
 * 15 octal escapes vs back references
 */

TEST_CASE("regex::octal escapes vs back references", "[regex][regex_15][builtin]")
{
    CheckRE("m", "1", "MP", "a\\010b", "a\bb", "a\bb", NULL);
    CheckRE("m", "2", "MP", "a\\0070b", "a\a0b", "a\a0b", NULL);
    CheckRE("m", "3", "MP", "a\\07b", "a\ab", "a\ab", NULL);
    CheckRE("m", "4", "MP", "a(b)(b)(b)(b)(b)(b)(b)(b)(b)(b)\\07c", "abbbbbbbbbb\ac", "abbbbbbbbbb\ac", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", NULL);
    CheckRE("e", "5", "-", "a\\7b", "ESUBREG", NULL);
    CheckRE("m", "6", "MP", "a\\10b", "a\bb", "a\bb", NULL);
    CheckRE("m", "7", "MP", "a\\101b", "aAb", "aAb", NULL);
    CheckRE("m", "8", "RP", "a(b)(b)(b)(b)(b)(b)(b)(b)(b)(b)\\10c", "abbbbbbbbbbbc", "abbbbbbbbbbbc", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", NULL);
    CheckRE("e", "9", "-", "a((((((((((b\\10))))))))))c", "ESUBREG", NULL);
    CheckRE("m", "10", "MP", "a\\12b", "a\nb", "a\nb", NULL);
    CheckRE("e", "11", "b", "a\\12b", "ESUBREG", NULL);
    CheckRE("m", "12", "eAS", "a\\12b", "a12b", "a12b", NULL);
}


/*
 * 16 expanded syntax
 */

TEST_CASE("regex::expanded syntax", "[regex][regex_16][builtin]")
{
    CheckRE("m", "1", "xP", "a b c", "abc", "abc", NULL);
    CheckRE("m", "2", "xP", "a b #oops\nc\td", "abcd", "abcd", NULL);
    CheckRE("m", "3", "x", "a\\ b\\\tc", "a b\tc", "a b\tc", NULL);
    CheckRE("m", "4", "xP", "a b\\#c", "ab#c", "ab#c", NULL);
    CheckRE("m", "5", "xP", "a b[c d]e", "ab e", "ab e", NULL);
    CheckRE("m", "6", "xP", "a b[c#d]e", "ab#e", "ab#e", NULL);
    CheckRE("m", "7", "xP", "a b[c#d]e", "abde", "abde", NULL);
    CheckRE("m", "8", "xSPB", "ab{ d", "ab{d", "ab{d", NULL);
    CheckRE("m", "9", "xPQ", "ab{ 1 , 2 }c", "abc", "abc", NULL);
}


/*
 * 17 misc syntax
 */

TEST_CASE("regex::misc syntax", "[regex][regex_17][builtin]")
{
    CheckRE("m", "1", "P", "a(?#comment)b", "ab", "ab", NULL);
}


/*
 * 18 unmatchable REs
 */

TEST_CASE("regex::unmatchable REs", "[regex][regex_18][builtin]")
{
    CheckRE("f", "1", "I", "a^b", "ab", NULL);
}


/*
 * 19 case independence
 */

TEST_CASE("regex::case independence", "[regex][regex_19][builtin]")
{
    CheckRE("m", "1", "&i", "ab", "Ab", "Ab", NULL);
    CheckRE("m", "2", "&i", "a[bc]", "aC", "aC", NULL);
    CheckRE("f", "3", "&i", "a[^bc]", "aB", NULL);
    CheckRE("m", "4", "&iM", "a[b-d]", "aC", "aC", NULL);
    CheckRE("f", "5", "&iM", "a[^b-d]", "aC", NULL);
}


/*
 * 20 directors and embedded options
 */

TEST_CASE("regex::directors and embedded options", "[regex][regex_20][builtin]")
{
    CheckRE("e", "1", "&", "***?", "BADPAT", NULL);
    CheckRE("m", "2", "q", "***?", "***?", "***?", NULL);
    CheckRE("m", "3", "&P", "***=a*b", "a*b", "a*b", NULL);
    CheckRE("m", "4", "q", "***=a*b", "***=a*b", "***=a*b", NULL);
    CheckRE("m", "5", "bLP", "***:\\w+", "ab", "ab", NULL);
    CheckRE("m", "6", "eLP", "***:\\w+", "ab", "ab", NULL);
    CheckRE("e", "7", "&", "***:***=a*b", "BADRPT", NULL);
    CheckRE("m", "8", "&P", "***:(?b)a+b", "a+b", "a+b", NULL);
    CheckRE("m", "9", "P", "(?b)a+b", "a+b", "a+b", NULL);
    CheckRE("e", "10", "e", "(?b)\\w+", "BADRPT", NULL);
    CheckRE("m", "11", "bAS", "(?b)\\w+", "(?b)w+", "(?b)w+", NULL);
    CheckRE("m", "12", "iP", "(?c)a", "a", "a", NULL);
    CheckRE("f", "13", "iP", "(?c)a", "A", NULL);
    CheckRE("m", "14", "APS", "(?e)\\W+", "WW", "WW", NULL);
    CheckRE("m", "15", "P", "(?i)a+", "Aa", "Aa", NULL);
    CheckRE("f", "16", "P", "(?m)a.b", "a\nb", NULL);
    CheckRE("m", "17", "P", "(?m)^b", "a\nb", "b", NULL);
    CheckRE("f", "18", "P", "(?n)a.b", "a\nb", NULL);
    CheckRE("m", "19", "P", "(?n)^b", "a\nb", "b", NULL);
    CheckRE("f", "20", "P", "(?p)a.b", "a\nb", NULL);
    CheckRE("f", "21", "P", "(?p)^b", "a\nb", NULL);
    CheckRE("m", "22", "P", "(?q)a+b", "a+b", "a+b", NULL);
    CheckRE("m", "23", "nP", "(?s)a.b", "a\nb", "a\nb", NULL);
    CheckRE("m", "24", "xP", "(?t)a b", "a b", "a b", NULL);
    CheckRE("m", "25", "P", "(?w)a.b", "a\nb", "a\nb", NULL);
    CheckRE("m", "26", "P", "(?w)^b", "a\nb", "b", NULL);
    CheckRE("m", "27", "P", "(?x)a b", "ab", "ab", NULL);
    CheckRE("e", "28", "-", "(?z)ab", "BADOPT", NULL);
    CheckRE("m", "29", "P", "(?ici)a+", "Aa", "Aa", NULL);
    CheckRE("e", "30", "P", "(?i)(?q)a+", "BADRPT", NULL);
    CheckRE("m", "31", "P", "(?q)(?i)a+", "(?i)a+", "(?i)a+", NULL);
    CheckRE("m", "32", "P", "(?qe)a+", "a", "a", NULL);
    CheckRE("m", "33", "xP", "(?q)a b", "a b", "a b", NULL);
    CheckRE("m", "34", "P", "(?qx)a b", "a b", "a b", NULL);
    CheckRE("m", "35", "P", "(?qi)ab", "Ab", "Ab", NULL);
}


/*
 * 21 capturing
 */

TEST_CASE("regex::capturing", "[regex][regex_21][builtin]")
{
    CheckRE("m", "1", "-", "a(b)c", "abc", "abc", "b", NULL);
    CheckRE("m", "2", "P", "a(?:b)c", "xabc", "abc", NULL);
    CheckRE("m", "3", "-", "a((b))c", "xabcy", "abc", "b", "b", NULL);
    CheckRE("m", "4", "P", "a(?:(b))c", "abcy", "abc", "b", NULL);
    CheckRE("m", "5", "P", "a((?:b))c", "abc", "abc", "b", NULL);
    CheckRE("m", "6", "P", "a(?:(?:b))c", "abc", "abc", NULL);
    CheckRE("i", "7", "Q", "a(b){0}c", "ac", "0 1", "-1 -1", NULL);
    CheckRE("m", "8", "-", "a(b)c(d)e", "abcde", "abcde", "b", "d", NULL);
    CheckRE("m", "9", "-", "(b)c(d)e", "bcde", "bcde", "b", "d", NULL);
    CheckRE("m", "10", "-", "a(b)(d)e", "abde", "abde", "b", "d", NULL);
    CheckRE("m", "11", "-", "a(b)c(d)", "abcd", "abcd", "b", "d", NULL);
    CheckRE("m", "12", "-", "(ab)(cd)", "xabcdy", "abcd", "ab", "cd", NULL);
    CheckRE("m", "13", "-", "a(b)?c", "xabcy", "abc", "b", NULL);
    CheckRE("i", "14", "-", "a(b)?c", "xacy", "1 2", "-1 -1", NULL);
    CheckRE("m", "15", "-", "a(b)?c(d)?e", "xabcdey", "abcde", "b", "d", NULL);
    CheckRE("i", "16", "-", "a(b)?c(d)?e", "xacdey", "1 4", "-1 -1", "3 3", NULL);
    CheckRE("i", "17", "-", "a(b)?c(d)?e", "xabcey", "1 4", "2 2", "-1 -1", NULL);
    CheckRE("i", "18", "-", "a(b)?c(d)?e", "xacey", "1 3", "-1 -1", "-1 -1", NULL);
    CheckRE("m", "19", "-", "a(b)*c", "xabcy", "abc", "b", NULL);
    CheckRE("i", "20", "-", "a(b)*c", "xabbbcy", "1 5", "4 4", NULL);
    CheckRE("i", "21", "-", "a(b)*c", "xacy", "1 2", "-1 -1", NULL);
    CheckRE("m", "22", "-", "a(b*)c", "xabbbcy", "abbbc", "bbb", NULL);
    CheckRE("m", "23", "-", "a(b*)c", "xacy", "ac", "", NULL);
    CheckRE("f", "24", "-", "a(b)+c", "xacy", NULL);
    CheckRE("m", "25", "-", "a(b)+c", "xabcy", "abc", "b", NULL);
    CheckRE("i", "26", "-", "a(b)+c", "xabbbcy", "1 5", "4 4", NULL);
    CheckRE("m", "27", "-", "a(b+)c", "xabbbcy", "abbbc", "bbb", NULL);
    CheckRE("i", "28", "Q", "a(b){2,3}c", "xabbbcy", "1 5", "4 4", NULL);
    CheckRE("i", "29", "Q", "a(b){2,3}c", "xabbcy", "1 4", "3 3", NULL);
    CheckRE("f", "30", "Q", "a(b){2,3}c", "xabcy", NULL);
    CheckRE("m", "31", "LP", "\\y(\\w+)\\y", "-- abc-", "abc", "abc", NULL);
    CheckRE("m", "32", "-", "a((b|c)d+)+", "abacdbd", "acdbd", "bd", "b", NULL);
    CheckRE("m", "33", "N", "(.*).*", "abc", "abc", "abc", NULL);
    CheckRE("m", "34", "N", "(a*)*", "bc", "", "", NULL);
}


/*
 * 22 multicharacter collating elements
 */

TEST_CASE("regex::multicharacter collating elements", "[regex][regex_22][builtin]")
{
    CheckRE("m", "1", "&+L", "a[c]e", "ace", "ace", NULL);
    CheckRE("f", "2", "&+IL", "a[c]h", "ach", NULL);
    CheckRE("m", "3", "&+L", "a[[.ch.]]", "ach", "ach", NULL);
    CheckRE("f", "4", "&+L", "a[[.ch.]]", "ace", NULL);
    CheckRE("m", "5", "&+L", "a[c[.ch.]]", "ac", "ac", NULL);
    CheckRE("m", "6", "&+L", "a[c[.ch.]]", "ace", "ac", NULL);
    CheckRE("m", "7", "&+L", "a[c[.ch.]]", "ache", "ach", NULL);
    CheckRE("f", "8", "&+L", "a[^c]e", "ace", NULL);
    CheckRE("m", "9", "&+L", "a[^c]e", "abe", "abe", NULL);
    CheckRE("m", "10", "&+L", "a[^c]e", "ache", "ache", NULL);
    CheckRE("f", "11", "&+L", "a[^[.ch.]]", "ach", NULL);
    CheckRE("m", "12", "&+L", "a[^[.ch.]]", "ace", "ac", NULL);
    CheckRE("m", "13", "&+L", "a[^[.ch.]]", "ac", "ac", NULL);
    CheckRE("m", "14", "&+L", "a[^[.ch.]]", "abe", "ab", NULL);
    CheckRE("f", "15", "&+L", "a[^c[.ch.]]", "ach", NULL);
    CheckRE("f", "16", "&+L", "a[^c[.ch.]]", "ace", NULL);
    CheckRE("f", "17", "&+L", "a[^c[.ch.]]", "ac", NULL);
    CheckRE("m", "18", "&+L", "a[^c[.ch.]]", "abe", "ab", NULL);
    CheckRE("m", "19", "&+L", "a[^b]", "ac", "ac", NULL);
    CheckRE("m", "20", "&+L", "a[^b]", "ace", "ac", NULL);
    CheckRE("m", "21", "&+L", "a[^b]", "ach", "ach", NULL);
    CheckRE("f", "22", "&+L", "a[^b]", "abe", NULL);
}


/*
 * 23 lookahead constraints
 */

TEST_CASE("regex::lookahead constraints", "[regex][regex_23][builtin]")
{
    CheckRE("m", "1", "HP", "a(?=b)b*", "ab", "ab", NULL);
    CheckRE("f", "2", "HP", "a(?=b)b*", "a", NULL);
    CheckRE("m", "3", "HP", "a(?=b)b*(?=c)c*", "abc", "abc", NULL);
    CheckRE("f", "4", "HP", "a(?=b)b*(?=c)c*", "ab", NULL);
    CheckRE("f", "5", "HP", "a(?!b)b*", "ab", NULL);
    CheckRE("m", "6", "HP", "a(?!b)b*", "a", "a", NULL);
    CheckRE("m", "7", "HP", "(?=b)b", "b", "b", NULL);
    CheckRE("f", "8", "HP", "(?=b)b", "a", NULL);
}


/*
 * 24 non-greedy quantifiers
 */

TEST_CASE("regex::non-greedy quantifiers", "[regex][regex_24][builtin]")
{
    CheckRE("m", "1", "PT", "ab+?", "abb", "ab", NULL);
    CheckRE("m", "2", "PT", "ab+?c", "abbc", "abbc", NULL);
    CheckRE("m", "3", "PT", "ab*?", "abb", "a", NULL);
    CheckRE("m", "4", "PT", "ab*?c", "abbc", "abbc", NULL);
    CheckRE("m", "5", "PT", "ab??", "ab", "a", NULL);
    CheckRE("m", "6", "PT", "ab??c", "abc", "abc", NULL);
    CheckRE("m", "7", "PQT", "ab{2,4}?", "abbbb", "abb", NULL);
    CheckRE("m", "8", "PQT", "ab{2,4}?c", "abbbbc", "abbbbc", NULL);
    CheckRE("m", "9", "-", "3z*", "123zzzz456", "3zzzz", NULL);
    CheckRE("m", "10", "PT", "3z*?", "123zzzz456", "3", NULL);
    CheckRE("m", "11", "-", "z*4", "123zzzz456", "zzzz4", NULL);
    CheckRE("m", "12", "PT", "z*?4", "123zzzz456", "zzzz4", NULL);
}


/*
 * 25 mixed quantifiers
 */

TEST_CASE("regex::mixed quantifiers", "[regex][regex_25][builtin]")
{
    CheckRE("m", "1", "PNT", "^(.*?)(a*)$", "xyza", "xyza", "xyz", "a", NULL);
    CheckRE("m", "2", "PNT", "^(.*?)(a*)$", "xyzaa", "xyzaa", "xyz", "aa", NULL);
    CheckRE("m", "3", "PNT", "^(.*?)(a*)$", "xyz", "xyz", "xyz", "", NULL);
}


/*
 * 26 tricky cases
 */

TEST_CASE("regex::tricky cases", "[regex][regex_26][builtin]")
{
    CheckRE("m", "1", "-", "(week|wee)(night|knights)", "weeknights", "weeknights", "wee", "knights", NULL);
    CheckRE("m", "2", "RP", "a(bc*).*\\1", "abccbccb", "abccbccb", "b", NULL);
    CheckRE("m", "3", "-", "a(b.[bc]*)+", "abcbd", "abcbd", "bd", NULL);
}


/*
 * 27 implementation misc.
 */

TEST_CASE("regex::implementation misc.", "[regex][regex_27][builtin]")
{
    CheckRE("m", "1", "P", "a(?:b|b)c", "abc", "abc", NULL);
    CheckRE("m", "2", "&", "[ab][ab][ab]", "aba", "aba", NULL);
    CheckRE("m", "3", "&", "[ab][ab][ab][ab][ab][ab][ab]", "abababa", "abababa", NULL);
}


/*
 * 28 boundary busters etc.
 */

TEST_CASE("regex::boundary busters etc.", "[regex][regex_28][builtin]")
{
    CheckRE("m", "1", "&", "abcdefghijkl", "abcdefghijkl", "abcdefghijkl", NULL);
    CheckRE("m", "2", "P", "a(?:b|c|d|e|f|g|h|i|j|k|l|m)n", "agn", "agn", NULL);
    CheckRE("m", "3", "-", "a(((((((((((((b)))))))))))))c", "abc", "abc", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", NULL);
    CheckRE("m", "4", "Q", "ab{1,100}c", "abbc", "abbc", NULL);
    CheckRE("m", "5", "Q", "ab{1,100}c", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", NULL);
    CheckRE("m", "6", "Q", "ab{1,100}c", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", NULL);
    CheckRE("m", "7", "LP", "\\w+abcdefgh", "xyzabcdefgh", "xyzabcdefgh", NULL);
    CheckRE("m", "8", "%LP", "\\w+abcdefgh", "xyzabcdefgh", "xyzabcdefgh", NULL);
    CheckRE("m", "9", "%LP", "\\w+abcdefghijklmnopqrst", "xyzabcdefghijklmnopqrst", "xyzabcdefghijklmnopqrst", NULL);
    CheckRE("i", "10", "%LP", "\\w+(abcdefgh)?", "xyz", "0 2", "-1 -1", NULL);
    CheckRE("i", "11", "%LP", "\\w+(abcdefgh)?", "xyzabcdefg", "0 9", "-1 -1", NULL);
    CheckRE("i", "12", "%LP", "\\w+(abcdefghijklmnopqrst)?", "xyzabcdefghijklmnopqrs", "0 21", "-1 -1", NULL);
}


/*
 * 29 incomplete matches
 */

TEST_CASE("regex::incomplete matches", "[regex][regex_29][builtin]")
{
    CheckRE("p", "1", "t", "def", "abc", "3 2", "", NULL);
    CheckRE("p", "2", "t", "bcd", "abc", "1 2", "", NULL);
    CheckRE("p", "3", "t", "abc", "abab", "0 3", "", NULL);
    CheckRE("p", "4", "t", "abc", "abdab", "3 4", "", NULL);
    CheckRE("i", "5", "t", "abc", "abc", "0 2", "0 2", NULL);
    CheckRE("i", "6", "t", "abc", "xyabc", "2 4", "2 4", NULL);
    CheckRE("p", "7", "t", "abc+", "xyab", "2 3", "", NULL);
    CheckRE("i", "8", "t", "abc+", "xyabc", "2 4", "2 4", NULL);
    CheckRE("i", "10", "t", "abc+", "xyabcdd", "2 4", "7 6", NULL);
    CheckRE("p", "11", "tPT", "abc+?", "xyab", "2 3", "", NULL);
    CheckRE("i", "12", "tPT", "abc+?", "xyabc", "2 4", "5 4", NULL);
    CheckRE("i", "13", "tPT", "abc+?", "xyabcc", "2 4", "6 5", NULL);
    CheckRE("i", "14", "tPT", "abc+?", "xyabcd", "2 4", "6 5", NULL);
    CheckRE("i", "15", "tPT", "abc+?", "xyabcdd", "2 4", "7 6", NULL);
    CheckRE("i", "16", "t", "abcd|bc", "xyabc", "3 4", "2 4", NULL);
    CheckRE("p", "17", "tn", ".*k", "xx\nyyy", "3 5", "", NULL);
}


/*
 * 30 misc. oddities and old bugs
 */

TEST_CASE("regex::misc. oddities and old bugs", "[regex][regex_30][builtin]")
{
    CheckRE("e", "1", "&", "***", "BADRPT", NULL);
    CheckRE("m", "2", "N", "a?b*", "abb", "abb", NULL);
    CheckRE("m", "3", "N", "a?b*", "bb", "bb", NULL);
    CheckRE("m", "4", "&", "a*b", "aab", "aab", NULL);
    CheckRE("m", "5", "&", "^a*b", "aaaab", "aaaab", NULL);
    CheckRE("m", "6", "&M", "[0-6][1-2][0-3][0-6][1-6][0-6]", "010010", "010010", NULL);
    CheckRE("m", "7", "s", "abc", "abcd", "abc", NULL);
    CheckRE("f", "8", "s", "abc", "xabcd", NULL);
    CheckRE("m", "9", "HLP", "(?n)^(?![t#])\\S+", "tk\n\n#\n#\nit0", "it0", NULL);
}


/*
 * extra_1 checks for bug fixes
 */

TEST_CASE("regex::checks for bug fixes", "[regex][regex_extra_1][builtin]")
{
    CheckRE("m", "Bug 230589", "o", "[ ]*(^|[^%])%V", "*%V2", NULL);
    CheckRE("m", "Bug 504785", "-", "([^_.]*)([^.]*)\\.(..)(.).*", "bbcos_001_c01.q1la", "bbcos_001_c01.q1la", "bbcos", "_001_c01", "q1", "l", NULL);
    CheckRE("m", "Bug 505048", "o", "\\A\\s*[^<]*\\s*<([^>]+)>", "a<a>", NULL);
    CheckRE("m", "Bug 505048", "o", "\\A\\s*([^b]*)b", "ab", NULL);
    CheckRE("m", "Bug 505048", "o", "\\A\\s*[^b]*(b)", "ab", NULL);
    CheckRE("m", "Bug 505048", "o", "\\A(\\s*)[^b]*(b)", "ab", NULL);
    CheckRE("m", "Bug 505048", "o", "\\A\\s*[^b]*b", "ab", NULL);
    CheckRE("m", "Bug 505048", "-", "\\A\\s*[^b]*b", "ab", "ab", NULL);
    CheckRE("i", "Bug 505048", "-", "\\A\\s*[^b]*b", "ab", "0 1", NULL);
}


/*
 * wx_1 character classification: ascii
 */

TEST_CASE("regex::character classification: ascii", "[regex][regex_wx_1][builtin]")
{
    CheckRE("m", "1", "&", "[^[:alnum:]]", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!", "!", NULL);
    CheckRE("m", "2", "&", "[[:alnum:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~X", "X", NULL);
    CheckRE("m", "3", "&", "[^[:alpha:]]", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!", "!", NULL);
    CheckRE("m", "4", "&", "[[:alpha:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`{|}~X", "X", NULL);
    CheckRE("m", "5", "&", "[^[:cntrl:]]", "\a\b\t\n\v\f\r!", "!", NULL);
    CheckRE("m", "6", "&", "[[:cntrl:]]", " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n", "\n", NULL);
    CheckRE("m", "7", "&", "[^[:digit:]]", "0123456789!", "!", NULL);
    CheckRE("m", "8", "&", "[[:digit:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ 0", "0", NULL);
    CheckRE("m", "9", "&", "[^[:graph:]]", "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n", "\n", NULL);
    CheckRE("m", "10", "&", "[[:graph:]]", "\a\b\t\n\v\f\r !", "!", NULL);
    CheckRE("m", "11", "&", "[^[:lower:]]", "abcdefghijklmnopqrstuvwxyz!", "!", NULL);
    CheckRE("m", "12", "&", "[[:lower:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`{|}~x", "x", NULL);
    CheckRE("m", "13", "&", "[^[:print:]]", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n", "\n", NULL);
    CheckRE("m", "14", "&", "[[:print:]]", "\a\b\n\v\f\rX", "X", NULL);
    CheckRE("m", "15", "&", "[^[:punct:]]", "!\"#%&'()*,-./:;?@[\\]_{}X", "X", NULL);
    CheckRE("m", "16", "&", "[[:punct:]]", "\a\b\t\n\v\f\r 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!", "!", NULL);
    CheckRE("m", "17", "&", "[^[:space:]]", "\t\n\v\f\r X", "X", NULL);
    CheckRE("m", "18", "&", "[[:space:]]", "\a\b!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n", "\n", NULL);
    CheckRE("m", "19", "&", "[^[:upper:]]", "ABCDEFGHIJKLMNOPQRSTUVWXYZ!", "!", NULL);
    CheckRE("m", "20", "&", "[[:upper:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~X", "X", NULL);
    CheckRE("m", "21", "&", "[^[:xdigit:]]", "0123456789ABCDEFabcdef!", "!", NULL);
    CheckRE("m", "22", "&", "[[:xdigit:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@GHIJKLMNOPQRSTUVWXYZ[\\]^_`ghijklmnopqrstuvwxyz{|}~a", "a", NULL);
    CheckRE("i", "23", "&i", "AbCdEfGhIjKlMnOpQrStUvWxYz", "aBcDeFgHiJkLmNoPqRsTuVwXyZ", "0 25", NULL);
}


/*
 * wx_2 character classification: western european
 */

TEST_CASE("regex::character classification: western european", "[regex][regex_wx_2][builtin]")
{
    CheckRE("m", "1", "&*", "[^[:alpha:]]", "\303\200\303\201\303\202\303\203\303\204\303\205\303\206\303\207\303\210\303\211\303\212\303\213\303\214\303\215\303\216\303\217\303\220\303\221\303\222\303\223\303\224\303\225\303\226\303\230\303\231\303\232\303\233\303\234\303\235\303\236\303\237\303\240\303\241\303\242\303\243\303\244\303\245\303\246\303\247\303\250\303\251\303\252\303\253\303\254\303\255\303\256\303\257\303\260\303\261\303\262\303\263\303\264\303\265\303\266\303\270\303\271\303\272\303\273\303\274\303\275\303\276\303\277!", "!", NULL);
    CheckRE("m", "2", "&*", "[[:alpha:]]", "\302\240\302\241\302\242\302\243\302\244\302\245\302\246\302\247\302\250\302\251\302\253\302\254\302\255\302\256\302\257\302\260\302\261\302\262\302\263\302\264\302\266\302\267\302\270\302\271\302\273\302\274\302\275\302\276\302\277\303\227\303\267X", "X", NULL);
    CheckRE("m", "3", "&*", "[^[:lower:]]", "\303\237\303\240\303\241\303\242\303\243\303\244\303\245\303\246\303\247\303\250\303\251\303\252\303\253\303\254\303\255\303\256\303\257\303\260\303\261\303\262\303\263\303\264\303\265\303\266\303\270\303\271\303\272\303\273\303\274\303\275\303\276\303\277!", "!", NULL);
    CheckRE("m", "4", "&*", "[[:lower:]]", "\302\240\302\241\302\242\302\243\302\244\302\245\302\246\302\247\302\250\302\251\302\253\302\254\302\255\302\256\302\257\302\260\302\261\302\262\302\263\302\264\302\266\302\267\302\270\302\271\302\273\302\274\302\275\302\276\302\277\303\200\303\201\303\202\303\203\303\204\303\205\303\206\303\207\303\210\303\211\303\212\303\213\303\214\303\215\303\216\303\217\303\220\303\221\303\222\303\223\303\224\303\225\303\226\303\227\303\230\303\231\303\232\303\233\303\234\303\235\303\236\303\267x", "x", NULL);
    CheckRE("m", "5", "&*", "[^[:upper:]]", "\303\200\303\201\303\202\303\203\303\204\303\205\303\206\303\207\303\210\303\211\303\212\303\213\303\214\303\215\303\216\303\217\303\220\303\221\303\222\303\223\303\224\303\225\303\226\303\230\303\231\303\232\303\233\303\234\303\235\303\236!", "!", NULL);
    CheckRE("m", "6", "&*", "[[:upper:]]", "\302\240\302\241\302\242\302\243\302\244\302\245\302\246\302\247\302\250\302\251\302\253\302\254\302\255\302\256\302\257\302\260\302\261\302\262\302\263\302\264\302\266\302\267\302\270\302\271\302\273\302\274\302\275\302\276\302\277\303\227\303\237\303\240\303\241\303\242\303\243\303\244\303\245\303\246\303\247\303\250\303\251\303\252\303\253\303\254\303\255\303\256\303\257\303\260\303\261\303\262\303\263\303\264\303\265\303\266\303\267\303\270\303\271\303\272\303\273\303\274\303\275\303\276\303\277X", "X", NULL);
    CheckRE("i", "7", "&i*", "\303\200\303\241\303\202\303\243\303\204\303\245\303\206\303\247\303\210\303\251\303\212\303\253\303\214\303\255\303\216\303\257\303\220\303\261\303\222\303\263\303\224\303\265\303\226\303\270\303\231\303\272\303\233\303\274\303\235\303\276", "\303\240\303\201\303\242\303\203\303\244\303\205\303\246\303\207\303\250\303\211\303\252\303\213\303\254\303\215\303\256\303\217\303\260\303\221\303\262\303\223\303\264\303\225\303\266\303\230\303\271\303\232\303\273\303\234\303\275\303\236", "0 29", NULL);
}


/*
 * wx_3 character classification: cyrillic
 */

TEST_CASE("regex::character classification: cyrillic", "[regex][regex_wx_3][builtin]")
{
    CheckRE("m", "1", "&*", "[^[:alpha:]]", "\321\221\320\201\321\216\320\260\320\261\321\206\320\264\320\265\321\204\320\263\321\205\320\270\320\271\320\272\320\273\320\274\320\275\320\276\320\277\321\217\321\200\321\201\321\202\321\203\320\266\320\262\321\214\321\213\320\267\321\210\321\215\321\211\321\207\321\212\320\256\320\220\320\221\320\246\320\224\320\225\320\244\320\223\320\245\320\230\320\231\320\232\320\233\320\234\320\235\320\236\320\237\320\257\320\240\320\241\320\242\320\243\320\226\320\222\320\254\320\253\320\227\320\250\320\255\320\251\320\247\320\252!", "!", NULL);
    CheckRE("m", "2", "&*", "[^[:lower:]]", "\321\221\321\216\320\260\320\261\321\206\320\264\320\265\321\204\320\263\321\205\320\270\320\271\320\272\320\273\320\274\320\275\320\276\320\277\321\217\321\200\321\201\321\202\321\203\320\266\320\262\321\214\321\213\320\267\321\210\321\215\321\211\321\207\321\212!", "!", NULL);
    CheckRE("m", "3", "&*", "[[:lower:]]", "\320\201\320\256\320\220\320\221\320\246\320\224\320\225\320\244\320\223\320\245\320\230\320\231\320\232\320\233\320\234\320\235\320\236\320\237\320\257\320\240\320\241\320\242\320\243\320\226\320\222\320\254\320\253\320\227\320\250\320\255\320\251\320\247\320\252x", "x", NULL);
    CheckRE("m", "4", "&*", "[^[:upper:]]", "\320\201\320\256\320\220\320\221\320\246\320\224\320\225\320\244\320\223\320\245\320\230\320\231\320\232\320\233\320\234\320\235\320\236\320\237\320\257\320\240\320\241\320\242\320\243\320\226\320\222\320\254\320\253\320\227\320\250\320\255\320\251\320\247\320\252!", "!", NULL);
    CheckRE("m", "5", "&*", "[[:upper:]]", "\321\221\321\216\320\260\320\261\321\206\320\264\320\265\321\204\320\263\321\205\320\270\320\271\320\272\320\273\320\274\320\275\320\276\320\277\321\217\321\200\321\201\321\202\321\203\320\266\320\262\321\214\321\213\320\267\321\210\321\215\321\211\321\207\321\212X", "X", NULL);
    CheckRE("i", "6", "&i*", "\320\201\321\216\320\220\320\261\320\246\320\264\320\225\321\204\320\223\321\205\320\230\320\271\320\232\320\273\320\234\320\275\320\236\320\277\320\257\321\200\320\241\321\202\320\243\320\266\320\222\321\214\320\253\320\267\320\250\321\215\320\251\321\207\320\252", "\321\221\320\256\320\260\320\221\321\206\320\224\320\265\320\244\320\263\320\245\320\270\320\231\320\272\320\233\320\274\320\235\320\276\320\237\321\217\320\240\321\201\320\242\321\203\320\226\320\262\320\254\321\213\320\227\321\210\320\255\321\211\320\247\321\212", "0 32", NULL);
}

/*
 * End of generated test suite.
 */
