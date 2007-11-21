/*
 * Test data for wxRegEx (UTF-8 encoded)
 * 
 * Generated Thu Jun 30 10:26:38 2005 by regex.pl from the following files:
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

class regextest_1 : public RegExTestSuite
{
public:
    regextest_1() : RegExTestSuite("regex.1") { }
    static Test *suite();
};

Test *regextest_1::suite()
{
    RegExTestSuite *suite = new regextest_1;

    suite->add("m", "1", "&", "abc", "abc", "abc", NULL);
    suite->add("f", "2", "&", "abc", "def", NULL);
    suite->add("m", "3", "&", "abc", "xyabxabce", "abc", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_1, "regex.1");


/*
 * 2 invalid option combinations
 */

class regextest_2 : public RegExTestSuite
{
public:
    regextest_2() : RegExTestSuite("regex.2") { }
    static Test *suite();
};

Test *regextest_2::suite()
{
    RegExTestSuite *suite = new regextest_2;

    suite->add("e", "1", "qe", "a", "INVARG", NULL);
    suite->add("e", "2", "qa", "a", "INVARG", NULL);
    suite->add("e", "3", "qx", "a", "INVARG", NULL);
    suite->add("e", "4", "qn", "a", "INVARG", NULL);
    suite->add("e", "5", "ba", "a", "INVARG", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_2, "regex.2");


/*
 * 3 basic syntax
 */

class regextest_3 : public RegExTestSuite
{
public:
    regextest_3() : RegExTestSuite("regex.3") { }
    static Test *suite();
};

Test *regextest_3::suite()
{
    RegExTestSuite *suite = new regextest_3;

    suite->add("i", "1", "&NS", "", "a", "0 -1", NULL);
    suite->add("m", "2", "NS", "a|", "a", "a", NULL);
    suite->add("m", "3", "-", "a|b", "a", "a", NULL);
    suite->add("m", "4", "-", "a|b", "b", "b", NULL);
    suite->add("m", "5", "NS", "a||b", "b", "b", NULL);
    suite->add("m", "6", "&", "ab", "ab", "ab", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_3, "regex.3");


/*
 * 4 parentheses
 */

class regextest_4 : public RegExTestSuite
{
public:
    regextest_4() : RegExTestSuite("regex.4") { }
    static Test *suite();
};

Test *regextest_4::suite()
{
    RegExTestSuite *suite = new regextest_4;

    suite->add("m", "1", "-", "(a)e", "ae", "ae", "a", NULL);
    suite->add("m", "2", "o", "(a)e", "ae", NULL);
    suite->add("m", "3", "b", "\\(a\\)b", "ab", "ab", "a", NULL);
    suite->add("m", "4", "-", "a((b)c)", "abc", "abc", "bc", "b", NULL);
    suite->add("m", "5", "-", "a(b)(c)", "abc", "abc", "b", "c", NULL);
    suite->add("e", "6", "-", "a(b", "EPAREN", NULL);
    suite->add("e", "7", "b", "a\\(b", "EPAREN", NULL);
    suite->add("m", "8", "eU", "a)b", "a)b", "a)b", NULL);
    suite->add("e", "9", "-", "a)b", "EPAREN", NULL);
    suite->add("e", "10", "b", "a\\)b", "EPAREN", NULL);
    suite->add("m", "11", "P", "a(?:b)c", "abc", "abc", NULL);
    suite->add("e", "12", "e", "a(?:b)c", "BADRPT", NULL);
    suite->add("i", "13", "S", "a()b", "ab", "0 1", "1 0", NULL);
    suite->add("m", "14", "SP", "a(?:)b", "ab", "ab", NULL);
    suite->add("i", "15", "S", "a(|b)c", "ac", "0 1", "1 0", NULL);
    suite->add("m", "16", "S", "a(b|)c", "abc", "abc", "b", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_4, "regex.4");


/*
 * 5 simple one-char matching
 */

class regextest_5 : public RegExTestSuite
{
public:
    regextest_5() : RegExTestSuite("regex.5") { }
    static Test *suite();
};

Test *regextest_5::suite()
{
    RegExTestSuite *suite = new regextest_5;

    suite->add("m", "1", "&", "a.b", "axb", "axb", NULL);
    suite->add("f", "2", "&n", "a.b", "a\nb", NULL);
    suite->add("m", "3", "&", "a[bc]d", "abd", "abd", NULL);
    suite->add("m", "4", "&", "a[bc]d", "acd", "acd", NULL);
    suite->add("f", "5", "&", "a[bc]d", "aed", NULL);
    suite->add("f", "6", "&", "a[^bc]d", "abd", NULL);
    suite->add("m", "7", "&", "a[^bc]d", "aed", "aed", NULL);
    suite->add("f", "8", "&p", "a[^bc]d", "a\nd", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_5, "regex.5");


/*
 * 6 context-dependent syntax
 */

class regextest_6 : public RegExTestSuite
{
public:
    regextest_6() : RegExTestSuite("regex.6") { }
    static Test *suite();
};

Test *regextest_6::suite()
{
    RegExTestSuite *suite = new regextest_6;

    suite->add("e", "1", "-", "*", "BADRPT", NULL);
    suite->add("m", "2", "b", "*", "*", "*", NULL);
    suite->add("m", "3", "b", "\\(*\\)", "*", "*", "*", NULL);
    suite->add("e", "4", "-", "(*)", "BADRPT", NULL);
    suite->add("m", "5", "b", "^*", "*", "*", NULL);
    suite->add("e", "6", "-", "^*", "BADRPT", NULL);
    suite->add("f", "7", "&", "^b", "^b", NULL);
    suite->add("m", "8", "b", "x^", "x^", "x^", NULL);
    suite->add("f", "9", "I", "x^", "x", NULL);
    suite->add("m", "10", "n", "\n^", "x\nb", "\n", NULL);
    suite->add("f", "11", "bS", "\\(^b\\)", "^b", NULL);
    suite->add("m", "12", "-", "(^b)", "b", "b", "b", NULL);
    suite->add("m", "13", "&", "x$", "x", "x", NULL);
    suite->add("m", "14", "bS", "\\(x$\\)", "x", "x", "x", NULL);
    suite->add("m", "15", "-", "(x$)", "x", "x", "x", NULL);
    suite->add("m", "16", "b", "x$y", "x$y", "x$y", NULL);
    suite->add("f", "17", "I", "x$y", "xy", NULL);
    suite->add("m", "18", "n", "x$\n", "x\n", "x\n", NULL);
    suite->add("e", "19", "-", "+", "BADRPT", NULL);
    suite->add("e", "20", "-", "?", "BADRPT", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_6, "regex.6");


/*
 * 7 simple quantifiers
 */

class regextest_7 : public RegExTestSuite
{
public:
    regextest_7() : RegExTestSuite("regex.7") { }
    static Test *suite();
};

Test *regextest_7::suite()
{
    RegExTestSuite *suite = new regextest_7;

    suite->add("m", "1", "&N", "a*", "aa", "aa", NULL);
    suite->add("i", "2", "&N", "a*", "b", "0 -1", NULL);
    suite->add("m", "3", "-", "a+", "aa", "aa", NULL);
    suite->add("m", "4", "-", "a?b", "ab", "ab", NULL);
    suite->add("m", "5", "-", "a?b", "b", "b", NULL);
    suite->add("e", "6", "-", "**", "BADRPT", NULL);
    suite->add("m", "7", "bN", "**", "***", "***", NULL);
    suite->add("e", "8", "&", "a**", "BADRPT", NULL);
    suite->add("e", "9", "&", "a**b", "BADRPT", NULL);
    suite->add("e", "10", "&", "***", "BADRPT", NULL);
    suite->add("e", "11", "-", "a++", "BADRPT", NULL);
    suite->add("e", "12", "-", "a?+", "BADRPT", NULL);
    suite->add("e", "13", "-", "a?*", "BADRPT", NULL);
    suite->add("e", "14", "-", "a+*", "BADRPT", NULL);
    suite->add("e", "15", "-", "a*+", "BADRPT", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_7, "regex.7");


/*
 * 8 braces
 */

class regextest_8 : public RegExTestSuite
{
public:
    regextest_8() : RegExTestSuite("regex.8") { }
    static Test *suite();
};

Test *regextest_8::suite()
{
    RegExTestSuite *suite = new regextest_8;

    suite->add("m", "1", "NQ", "a{0,1}", "", "", NULL);
    suite->add("m", "2", "NQ", "a{0,1}", "ac", "a", NULL);
    suite->add("e", "3", "-", "a{1,0}", "BADBR", NULL);
    suite->add("e", "4", "-", "a{1,2,3}", "BADBR", NULL);
    suite->add("e", "5", "-", "a{257}", "BADBR", NULL);
    suite->add("e", "6", "-", "a{1000}", "BADBR", NULL);
    suite->add("e", "7", "-", "a{1", "EBRACE", NULL);
    suite->add("e", "8", "-", "a{1n}", "BADBR", NULL);
    suite->add("m", "9", "BS", "a{b", "a{b", "a{b", NULL);
    suite->add("m", "10", "BS", "a{", "a{", "a{", NULL);
    suite->add("m", "11", "bQ", "a\\{0,1\\}b", "cb", "b", NULL);
    suite->add("e", "12", "b", "a\\{0,1", "EBRACE", NULL);
    suite->add("e", "13", "-", "a{0,1\\", "BADBR", NULL);
    suite->add("m", "14", "Q", "a{0}b", "ab", "b", NULL);
    suite->add("m", "15", "Q", "a{0,0}b", "ab", "b", NULL);
    suite->add("m", "16", "Q", "a{0,1}b", "ab", "ab", NULL);
    suite->add("m", "17", "Q", "a{0,2}b", "b", "b", NULL);
    suite->add("m", "18", "Q", "a{0,2}b", "aab", "aab", NULL);
    suite->add("m", "19", "Q", "a{0,}b", "aab", "aab", NULL);
    suite->add("m", "20", "Q", "a{1,1}b", "aab", "ab", NULL);
    suite->add("m", "21", "Q", "a{1,3}b", "aaaab", "aaab", NULL);
    suite->add("f", "22", "Q", "a{1,3}b", "b", NULL);
    suite->add("m", "23", "Q", "a{1,}b", "aab", "aab", NULL);
    suite->add("f", "24", "Q", "a{2,3}b", "ab", NULL);
    suite->add("m", "25", "Q", "a{2,3}b", "aaaab", "aaab", NULL);
    suite->add("f", "26", "Q", "a{2,}b", "ab", NULL);
    suite->add("m", "27", "Q", "a{2,}b", "aaaab", "aaaab", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_8, "regex.8");


/*
 * 9 brackets
 */

class regextest_9 : public RegExTestSuite
{
public:
    regextest_9() : RegExTestSuite("regex.9") { }
    static Test *suite();
};

Test *regextest_9::suite()
{
    RegExTestSuite *suite = new regextest_9;

    suite->add("m", "1", "&", "a[bc]", "ac", "ac", NULL);
    suite->add("m", "2", "&", "a[-]", "a-", "a-", NULL);
    suite->add("m", "3", "&", "a[[.-.]]", "a-", "a-", NULL);
    suite->add("m", "4", "&L", "a[[.zero.]]", "a0", "a0", NULL);
    suite->add("m", "5", "&LM", "a[[.zero.]-9]", "a2", "a2", NULL);
    suite->add("m", "6", "&M", "a[0-[.9.]]", "a2", "a2", NULL);
    suite->add("m", "7", "&+L", "a[[=x=]]", "ax", "ax", NULL);
    suite->add("m", "8", "&+L", "a[[=x=]]", "ay", "ay", NULL);
    suite->add("f", "9", "&+L", "a[[=x=]]", "az", NULL);
    suite->add("e", "10", "&", "a[0-[=x=]]", "ERANGE", NULL);
    suite->add("m", "11", "&L", "a[[:digit:]]", "a0", "a0", NULL);
    suite->add("e", "12", "&", "a[[:woopsie:]]", "ECTYPE", NULL);
    suite->add("f", "13", "&L", "a[[:digit:]]", "ab", NULL);
    suite->add("e", "14", "&", "a[0-[:digit:]]", "ERANGE", NULL);
    suite->add("m", "15", "&LP", "[[:<:]]a", "a", "a", NULL);
    suite->add("m", "16", "&LP", "a[[:>:]]", "a", "a", NULL);
    suite->add("e", "17", "&", "a[[..]]b", "ECOLLATE", NULL);
    suite->add("e", "18", "&", "a[[==]]b", "ECOLLATE", NULL);
    suite->add("e", "19", "&", "a[[::]]b", "ECTYPE", NULL);
    suite->add("e", "20", "&", "a[[.a", "EBRACK", NULL);
    suite->add("e", "21", "&", "a[[=a", "EBRACK", NULL);
    suite->add("e", "22", "&", "a[[:a", "EBRACK", NULL);
    suite->add("e", "23", "&", "a[", "EBRACK", NULL);
    suite->add("e", "24", "&", "a[b", "EBRACK", NULL);
    suite->add("e", "25", "&", "a[b-", "EBRACK", NULL);
    suite->add("e", "26", "&", "a[b-c", "EBRACK", NULL);
    suite->add("m", "27", "&M", "a[b-c]", "ab", "ab", NULL);
    suite->add("m", "28", "&", "a[b-b]", "ab", "ab", NULL);
    suite->add("m", "29", "&M", "a[1-2]", "a2", "a2", NULL);
    suite->add("e", "30", "&", "a[c-b]", "ERANGE", NULL);
    suite->add("e", "31", "&", "a[a-b-c]", "ERANGE", NULL);
    suite->add("m", "32", "&M", "a[--?]b", "a?b", "a?b", NULL);
    suite->add("m", "33", "&", "a[---]b", "a-b", "a-b", NULL);
    suite->add("m", "34", "&", "a[]b]c", "a]c", "a]c", NULL);
    suite->add("m", "35", "EP", "a[\\]]b", "a]b", "a]b", NULL);
    suite->add("f", "36", "bE", "a[\\]]b", "a]b", NULL);
    suite->add("m", "37", "bE", "a[\\]]b", "a\\]b", "a\\]b", NULL);
    suite->add("m", "38", "eE", "a[\\]]b", "a\\]b", "a\\]b", NULL);
    suite->add("m", "39", "EP", "a[\\\\]b", "a\\b", "a\\b", NULL);
    suite->add("m", "40", "eE", "a[\\\\]b", "a\\b", "a\\b", NULL);
    suite->add("m", "41", "bE", "a[\\\\]b", "a\\b", "a\\b", NULL);
    suite->add("e", "42", "-", "a[\\Z]b", "EESCAPE", NULL);
    suite->add("m", "43", "&", "a[[b]c", "a[c", "a[c", NULL);
    suite->add("m", "44", "EMP*", "a[\\u00fe-\\u0507][\\u00ff-\\u0300]b", "a\304\202\313\277b", "a\304\202\313\277b", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_9, "regex.9");


/*
 * 10 anchors and newlines
 */

class regextest_10 : public RegExTestSuite
{
public:
    regextest_10() : RegExTestSuite("regex.10") { }
    static Test *suite();
};

Test *regextest_10::suite()
{
    RegExTestSuite *suite = new regextest_10;

    suite->add("m", "1", "&", "^a", "a", "a", NULL);
    suite->add("f", "2", "&^", "^a", "a", NULL);
    suite->add("i", "3", "&N", "^", "a", "0 -1", NULL);
    suite->add("i", "4", "&", "a$", "aba", "2 2", NULL);
    suite->add("f", "5", "&$", "a$", "a", NULL);
    suite->add("i", "6", "&N", "$", "ab", "2 1", NULL);
    suite->add("m", "7", "&n", "^a", "a", "a", NULL);
    suite->add("m", "8", "&n", "^a", "b\na", "a", NULL);
    suite->add("i", "9", "&w", "^a", "a\na", "0 0", NULL);
    suite->add("i", "10", "&n^", "^a", "a\na", "2 2", NULL);
    suite->add("m", "11", "&n", "a$", "a", "a", NULL);
    suite->add("m", "12", "&n", "a$", "a\nb", "a", NULL);
    suite->add("i", "13", "&n", "a$", "a\na", "0 0", NULL);
    suite->add("i", "14", "N", "^^", "a", "0 -1", NULL);
    suite->add("m", "15", "b", "^^", "^", "^", NULL);
    suite->add("i", "16", "N", "$$", "a", "1 0", NULL);
    suite->add("m", "17", "b", "$$", "$", "$", NULL);
    suite->add("m", "18", "&N", "^$", "", "", NULL);
    suite->add("f", "19", "&N", "^$", "a", NULL);
    suite->add("i", "20", "&nN", "^$", "a\n\nb", "2 1", NULL);
    suite->add("m", "21", "N", "$^", "", "", NULL);
    suite->add("m", "22", "b", "$^", "$^", "$^", NULL);
    suite->add("m", "23", "P", "\\Aa", "a", "a", NULL);
    suite->add("m", "24", "^P", "\\Aa", "a", "a", NULL);
    suite->add("f", "25", "^nP", "\\Aa", "b\na", NULL);
    suite->add("m", "26", "P", "a\\Z", "a", "a", NULL);
    suite->add("m", "27", "$P", "a\\Z", "a", "a", NULL);
    suite->add("f", "28", "$nP", "a\\Z", "a\nb", NULL);
    suite->add("e", "29", "-", "^*", "BADRPT", NULL);
    suite->add("e", "30", "-", "$*", "BADRPT", NULL);
    suite->add("e", "31", "-", "\\A*", "BADRPT", NULL);
    suite->add("e", "32", "-", "\\Z*", "BADRPT", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_10, "regex.10");


/*
 * 11 boundary constraints
 */

class regextest_11 : public RegExTestSuite
{
public:
    regextest_11() : RegExTestSuite("regex.11") { }
    static Test *suite();
};

Test *regextest_11::suite()
{
    RegExTestSuite *suite = new regextest_11;

    suite->add("m", "1", "&LP", "[[:<:]]a", "a", "a", NULL);
    suite->add("m", "2", "&LP", "[[:<:]]a", "-a", "a", NULL);
    suite->add("f", "3", "&LP", "[[:<:]]a", "ba", NULL);
    suite->add("m", "4", "&LP", "a[[:>:]]", "a", "a", NULL);
    suite->add("m", "5", "&LP", "a[[:>:]]", "a-", "a", NULL);
    suite->add("f", "6", "&LP", "a[[:>:]]", "ab", NULL);
    suite->add("m", "7", "bLP", "\\<a", "a", "a", NULL);
    suite->add("f", "8", "bLP", "\\<a", "ba", NULL);
    suite->add("m", "9", "bLP", "a\\>", "a", "a", NULL);
    suite->add("f", "10", "bLP", "a\\>", "ab", NULL);
    suite->add("m", "11", "LP", "\\ya", "a", "a", NULL);
    suite->add("f", "12", "LP", "\\ya", "ba", NULL);
    suite->add("m", "13", "LP", "a\\y", "a", "a", NULL);
    suite->add("f", "14", "LP", "a\\y", "ab", NULL);
    suite->add("m", "15", "LP", "a\\Y", "ab", "a", NULL);
    suite->add("f", "16", "LP", "a\\Y", "a-", NULL);
    suite->add("f", "17", "LP", "a\\Y", "a", NULL);
    suite->add("f", "18", "LP", "-\\Y", "-a", NULL);
    suite->add("m", "19", "LP", "-\\Y", "-%", "-", NULL);
    suite->add("f", "20", "LP", "\\Y-", "a-", NULL);
    suite->add("e", "21", "-", "[[:<:]]*", "BADRPT", NULL);
    suite->add("e", "22", "-", "[[:>:]]*", "BADRPT", NULL);
    suite->add("e", "23", "b", "\\<*", "BADRPT", NULL);
    suite->add("e", "24", "b", "\\>*", "BADRPT", NULL);
    suite->add("e", "25", "-", "\\y*", "BADRPT", NULL);
    suite->add("e", "26", "-", "\\Y*", "BADRPT", NULL);
    suite->add("m", "27", "LP", "\\ma", "a", "a", NULL);
    suite->add("f", "28", "LP", "\\ma", "ba", NULL);
    suite->add("m", "29", "LP", "a\\M", "a", "a", NULL);
    suite->add("f", "30", "LP", "a\\M", "ab", NULL);
    suite->add("f", "31", "ILP", "\\Ma", "a", NULL);
    suite->add("f", "32", "ILP", "a\\m", "a", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_11, "regex.11");


/*
 * 12 character classes
 */

class regextest_12 : public RegExTestSuite
{
public:
    regextest_12() : RegExTestSuite("regex.12") { }
    static Test *suite();
};

Test *regextest_12::suite()
{
    RegExTestSuite *suite = new regextest_12;

    suite->add("m", "1", "LP", "a\\db", "a0b", "a0b", NULL);
    suite->add("f", "2", "LP", "a\\db", "axb", NULL);
    suite->add("f", "3", "LP", "a\\Db", "a0b", NULL);
    suite->add("m", "4", "LP", "a\\Db", "axb", "axb", NULL);
    suite->add("m", "5", "LP", "a\\sb", "a b", "a b", NULL);
    suite->add("m", "6", "LP", "a\\sb", "a\tb", "a\tb", NULL);
    suite->add("m", "7", "LP", "a\\sb", "a\nb", "a\nb", NULL);
    suite->add("f", "8", "LP", "a\\sb", "axb", NULL);
    suite->add("m", "9", "LP", "a\\Sb", "axb", "axb", NULL);
    suite->add("f", "10", "LP", "a\\Sb", "a b", NULL);
    suite->add("m", "11", "LP", "a\\wb", "axb", "axb", NULL);
    suite->add("f", "12", "LP", "a\\wb", "a-b", NULL);
    suite->add("f", "13", "LP", "a\\Wb", "axb", NULL);
    suite->add("m", "14", "LP", "a\\Wb", "a-b", "a-b", NULL);
    suite->add("m", "15", "LP", "\\y\\w+z\\y", "adze-guz", "guz", NULL);
    suite->add("m", "16", "LPE", "a[\\d]b", "a1b", "a1b", NULL);
    suite->add("m", "17", "LPE", "a[\\s]b", "a b", "a b", NULL);
    suite->add("m", "18", "LPE", "a[\\w]b", "axb", "axb", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_12, "regex.12");


/*
 * 13 escapes
 */

class regextest_13 : public RegExTestSuite
{
public:
    regextest_13() : RegExTestSuite("regex.13") { }
    static Test *suite();
};

Test *regextest_13::suite()
{
    RegExTestSuite *suite = new regextest_13;

    suite->add("e", "1", "&", "a\\", "EESCAPE", NULL);
    suite->add("m", "2", "-", "a\\<b", "a<b", "a<b", NULL);
    suite->add("m", "3", "e", "a\\<b", "a<b", "a<b", NULL);
    suite->add("m", "4", "bAS", "a\\wb", "awb", "awb", NULL);
    suite->add("m", "5", "eAS", "a\\wb", "awb", "awb", NULL);
    suite->add("m", "6", "PL", "a\\ab", "a\ab", "a\ab", NULL);
    suite->add("m", "7", "P", "a\\bb", "a\bb", "a\bb", NULL);
    suite->add("m", "8", "P", "a\\Bb", "a\\b", "a\\b", NULL);
    suite->add("m", "9", "MP", "a\\chb", "a\bb", "a\bb", NULL);
    suite->add("m", "10", "MP", "a\\cHb", "a\bb", "a\bb", NULL);
    suite->add("m", "11", "LMP", "a\\e", "a\033", "a\033", NULL);
    suite->add("m", "12", "P", "a\\fb", "a\fb", "a\fb", NULL);
    suite->add("m", "13", "P", "a\\nb", "a\nb", "a\nb", NULL);
    suite->add("m", "14", "P", "a\\rb", "a\rb", "a\rb", NULL);
    suite->add("m", "15", "P", "a\\tb", "a\tb", "a\tb", NULL);
    suite->add("m", "16", "P", "a\\u0008x", "a\bx", "a\bx", NULL);
    suite->add("e", "17", "-", "a\\u008x", "EESCAPE", NULL);
    suite->add("m", "18", "P", "a\\u00088x", "a\b8x", "a\b8x", NULL);
    suite->add("m", "19", "P", "a\\U00000008x", "a\bx", "a\bx", NULL);
    suite->add("e", "20", "-", "a\\U0000008x", "EESCAPE", NULL);
    suite->add("m", "21", "P", "a\\vb", "a\vb", "a\vb", NULL);
    suite->add("m", "22", "MP", "a\\x08x", "a\bx", "a\bx", NULL);
    suite->add("e", "23", "-", "a\\xq", "EESCAPE", NULL);
    suite->add("m", "24", "MP", "a\\x0008x", "a\bx", "a\bx", NULL);
    suite->add("e", "25", "-", "a\\z", "EESCAPE", NULL);
    suite->add("m", "26", "MP", "a\\010b", "a\bb", "a\bb", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_13, "regex.13");


/*
 * 14 back references
 */

class regextest_14 : public RegExTestSuite
{
public:
    regextest_14() : RegExTestSuite("regex.14") { }
    static Test *suite();
};

Test *regextest_14::suite()
{
    RegExTestSuite *suite = new regextest_14;

    suite->add("m", "1", "RP", "a(b*)c\\1", "abbcbb", "abbcbb", "bb", NULL);
    suite->add("m", "2", "RP", "a(b*)c\\1", "ac", "ac", "", NULL);
    suite->add("f", "3", "RP", "a(b*)c\\1", "abbcb", NULL);
    suite->add("m", "4", "RP", "a(b*)\\1", "abbcbb", "abb", "b", NULL);
    suite->add("m", "5", "RP", "a(b|bb)\\1", "abbcbb", "abb", "b", NULL);
    suite->add("m", "6", "RP", "a([bc])\\1", "abb", "abb", "b", NULL);
    suite->add("f", "7", "RP", "a([bc])\\1", "abc", NULL);
    suite->add("m", "8", "RP", "a([bc])\\1", "abcabb", "abb", "b", NULL);
    suite->add("f", "9", "RP", "a([bc])*\\1", "abc", NULL);
    suite->add("f", "10", "RP", "a([bc])\\1", "abB", NULL);
    suite->add("m", "11", "iRP", "a([bc])\\1", "abB", "abB", "b", NULL);
    suite->add("m", "12", "RP", "a([bc])\\1+", "abbb", "abbb", "b", NULL);
    suite->add("m", "13", "QRP", "a([bc])\\1{3,4}", "abbbb", "abbbb", "b", NULL);
    suite->add("f", "14", "QRP", "a([bc])\\1{3,4}", "abbb", NULL);
    suite->add("m", "15", "RP", "a([bc])\\1*", "abbb", "abbb", "b", NULL);
    suite->add("m", "16", "RP", "a([bc])\\1*", "ab", "ab", "b", NULL);
    suite->add("m", "17", "RP", "a([bc])(\\1*)", "ab", "ab", "b", "", NULL);
    suite->add("e", "18", "-", "a((b)\\1)", "ESUBREG", NULL);
    suite->add("e", "19", "-", "a(b)c\\2", "ESUBREG", NULL);
    suite->add("m", "20", "bR", "a\\(b*\\)c\\1", "abbcbb", "abbcbb", "bb", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_14, "regex.14");


/*
 * 15 octal escapes vs back references
 */

class regextest_15 : public RegExTestSuite
{
public:
    regextest_15() : RegExTestSuite("regex.15") { }
    static Test *suite();
};

Test *regextest_15::suite()
{
    RegExTestSuite *suite = new regextest_15;

    suite->add("m", "1", "MP", "a\\010b", "a\bb", "a\bb", NULL);
    suite->add("m", "2", "MP", "a\\0070b", "a\a0b", "a\a0b", NULL);
    suite->add("m", "3", "MP", "a\\07b", "a\ab", "a\ab", NULL);
    suite->add("m", "4", "MP", "a(b)(b)(b)(b)(b)(b)(b)(b)(b)(b)\\07c", "abbbbbbbbbb\ac", "abbbbbbbbbb\ac", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", NULL);
    suite->add("e", "5", "-", "a\\7b", "ESUBREG", NULL);
    suite->add("m", "6", "MP", "a\\10b", "a\bb", "a\bb", NULL);
    suite->add("m", "7", "MP", "a\\101b", "aAb", "aAb", NULL);
    suite->add("m", "8", "RP", "a(b)(b)(b)(b)(b)(b)(b)(b)(b)(b)\\10c", "abbbbbbbbbbbc", "abbbbbbbbbbbc", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", NULL);
    suite->add("e", "9", "-", "a((((((((((b\\10))))))))))c", "ESUBREG", NULL);
    suite->add("m", "10", "MP", "a\\12b", "a\nb", "a\nb", NULL);
    suite->add("e", "11", "b", "a\\12b", "ESUBREG", NULL);
    suite->add("m", "12", "eAS", "a\\12b", "a12b", "a12b", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_15, "regex.15");


/*
 * 16 expanded syntax
 */

class regextest_16 : public RegExTestSuite
{
public:
    regextest_16() : RegExTestSuite("regex.16") { }
    static Test *suite();
};

Test *regextest_16::suite()
{
    RegExTestSuite *suite = new regextest_16;

    suite->add("m", "1", "xP", "a b c", "abc", "abc", NULL);
    suite->add("m", "2", "xP", "a b #oops\nc\td", "abcd", "abcd", NULL);
    suite->add("m", "3", "x", "a\\ b\\\tc", "a b\tc", "a b\tc", NULL);
    suite->add("m", "4", "xP", "a b\\#c", "ab#c", "ab#c", NULL);
    suite->add("m", "5", "xP", "a b[c d]e", "ab e", "ab e", NULL);
    suite->add("m", "6", "xP", "a b[c#d]e", "ab#e", "ab#e", NULL);
    suite->add("m", "7", "xP", "a b[c#d]e", "abde", "abde", NULL);
    suite->add("m", "8", "xSPB", "ab{ d", "ab{d", "ab{d", NULL);
    suite->add("m", "9", "xPQ", "ab{ 1 , 2 }c", "abc", "abc", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_16, "regex.16");


/*
 * 17 misc syntax
 */

class regextest_17 : public RegExTestSuite
{
public:
    regextest_17() : RegExTestSuite("regex.17") { }
    static Test *suite();
};

Test *regextest_17::suite()
{
    RegExTestSuite *suite = new regextest_17;

    suite->add("m", "1", "P", "a(?#comment)b", "ab", "ab", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_17, "regex.17");


/*
 * 18 unmatchable REs
 */

class regextest_18 : public RegExTestSuite
{
public:
    regextest_18() : RegExTestSuite("regex.18") { }
    static Test *suite();
};

Test *regextest_18::suite()
{
    RegExTestSuite *suite = new regextest_18;

    suite->add("f", "1", "I", "a^b", "ab", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_18, "regex.18");


/*
 * 19 case independence
 */

class regextest_19 : public RegExTestSuite
{
public:
    regextest_19() : RegExTestSuite("regex.19") { }
    static Test *suite();
};

Test *regextest_19::suite()
{
    RegExTestSuite *suite = new regextest_19;

    suite->add("m", "1", "&i", "ab", "Ab", "Ab", NULL);
    suite->add("m", "2", "&i", "a[bc]", "aC", "aC", NULL);
    suite->add("f", "3", "&i", "a[^bc]", "aB", NULL);
    suite->add("m", "4", "&iM", "a[b-d]", "aC", "aC", NULL);
    suite->add("f", "5", "&iM", "a[^b-d]", "aC", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_19, "regex.19");


/*
 * 20 directors and embedded options
 */

class regextest_20 : public RegExTestSuite
{
public:
    regextest_20() : RegExTestSuite("regex.20") { }
    static Test *suite();
};

Test *regextest_20::suite()
{
    RegExTestSuite *suite = new regextest_20;

    suite->add("e", "1", "&", "***?", "BADPAT", NULL);
    suite->add("m", "2", "q", "***?", "***?", "***?", NULL);
    suite->add("m", "3", "&P", "***=a*b", "a*b", "a*b", NULL);
    suite->add("m", "4", "q", "***=a*b", "***=a*b", "***=a*b", NULL);
    suite->add("m", "5", "bLP", "***:\\w+", "ab", "ab", NULL);
    suite->add("m", "6", "eLP", "***:\\w+", "ab", "ab", NULL);
    suite->add("e", "7", "&", "***:***=a*b", "BADRPT", NULL);
    suite->add("m", "8", "&P", "***:(?b)a+b", "a+b", "a+b", NULL);
    suite->add("m", "9", "P", "(?b)a+b", "a+b", "a+b", NULL);
    suite->add("e", "10", "e", "(?b)\\w+", "BADRPT", NULL);
    suite->add("m", "11", "bAS", "(?b)\\w+", "(?b)w+", "(?b)w+", NULL);
    suite->add("m", "12", "iP", "(?c)a", "a", "a", NULL);
    suite->add("f", "13", "iP", "(?c)a", "A", NULL);
    suite->add("m", "14", "APS", "(?e)\\W+", "WW", "WW", NULL);
    suite->add("m", "15", "P", "(?i)a+", "Aa", "Aa", NULL);
    suite->add("f", "16", "P", "(?m)a.b", "a\nb", NULL);
    suite->add("m", "17", "P", "(?m)^b", "a\nb", "b", NULL);
    suite->add("f", "18", "P", "(?n)a.b", "a\nb", NULL);
    suite->add("m", "19", "P", "(?n)^b", "a\nb", "b", NULL);
    suite->add("f", "20", "P", "(?p)a.b", "a\nb", NULL);
    suite->add("f", "21", "P", "(?p)^b", "a\nb", NULL);
    suite->add("m", "22", "P", "(?q)a+b", "a+b", "a+b", NULL);
    suite->add("m", "23", "nP", "(?s)a.b", "a\nb", "a\nb", NULL);
    suite->add("m", "24", "xP", "(?t)a b", "a b", "a b", NULL);
    suite->add("m", "25", "P", "(?w)a.b", "a\nb", "a\nb", NULL);
    suite->add("m", "26", "P", "(?w)^b", "a\nb", "b", NULL);
    suite->add("m", "27", "P", "(?x)a b", "ab", "ab", NULL);
    suite->add("e", "28", "-", "(?z)ab", "BADOPT", NULL);
    suite->add("m", "29", "P", "(?ici)a+", "Aa", "Aa", NULL);
    suite->add("e", "30", "P", "(?i)(?q)a+", "BADRPT", NULL);
    suite->add("m", "31", "P", "(?q)(?i)a+", "(?i)a+", "(?i)a+", NULL);
    suite->add("m", "32", "P", "(?qe)a+", "a", "a", NULL);
    suite->add("m", "33", "xP", "(?q)a b", "a b", "a b", NULL);
    suite->add("m", "34", "P", "(?qx)a b", "a b", "a b", NULL);
    suite->add("m", "35", "P", "(?qi)ab", "Ab", "Ab", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_20, "regex.20");


/*
 * 21 capturing
 */

class regextest_21 : public RegExTestSuite
{
public:
    regextest_21() : RegExTestSuite("regex.21") { }
    static Test *suite();
};

Test *regextest_21::suite()
{
    RegExTestSuite *suite = new regextest_21;

    suite->add("m", "1", "-", "a(b)c", "abc", "abc", "b", NULL);
    suite->add("m", "2", "P", "a(?:b)c", "xabc", "abc", NULL);
    suite->add("m", "3", "-", "a((b))c", "xabcy", "abc", "b", "b", NULL);
    suite->add("m", "4", "P", "a(?:(b))c", "abcy", "abc", "b", NULL);
    suite->add("m", "5", "P", "a((?:b))c", "abc", "abc", "b", NULL);
    suite->add("m", "6", "P", "a(?:(?:b))c", "abc", "abc", NULL);
    suite->add("i", "7", "Q", "a(b){0}c", "ac", "0 1", "-1 -1", NULL);
    suite->add("m", "8", "-", "a(b)c(d)e", "abcde", "abcde", "b", "d", NULL);
    suite->add("m", "9", "-", "(b)c(d)e", "bcde", "bcde", "b", "d", NULL);
    suite->add("m", "10", "-", "a(b)(d)e", "abde", "abde", "b", "d", NULL);
    suite->add("m", "11", "-", "a(b)c(d)", "abcd", "abcd", "b", "d", NULL);
    suite->add("m", "12", "-", "(ab)(cd)", "xabcdy", "abcd", "ab", "cd", NULL);
    suite->add("m", "13", "-", "a(b)?c", "xabcy", "abc", "b", NULL);
    suite->add("i", "14", "-", "a(b)?c", "xacy", "1 2", "-1 -1", NULL);
    suite->add("m", "15", "-", "a(b)?c(d)?e", "xabcdey", "abcde", "b", "d", NULL);
    suite->add("i", "16", "-", "a(b)?c(d)?e", "xacdey", "1 4", "-1 -1", "3 3", NULL);
    suite->add("i", "17", "-", "a(b)?c(d)?e", "xabcey", "1 4", "2 2", "-1 -1", NULL);
    suite->add("i", "18", "-", "a(b)?c(d)?e", "xacey", "1 3", "-1 -1", "-1 -1", NULL);
    suite->add("m", "19", "-", "a(b)*c", "xabcy", "abc", "b", NULL);
    suite->add("i", "20", "-", "a(b)*c", "xabbbcy", "1 5", "4 4", NULL);
    suite->add("i", "21", "-", "a(b)*c", "xacy", "1 2", "-1 -1", NULL);
    suite->add("m", "22", "-", "a(b*)c", "xabbbcy", "abbbc", "bbb", NULL);
    suite->add("m", "23", "-", "a(b*)c", "xacy", "ac", "", NULL);
    suite->add("f", "24", "-", "a(b)+c", "xacy", NULL);
    suite->add("m", "25", "-", "a(b)+c", "xabcy", "abc", "b", NULL);
    suite->add("i", "26", "-", "a(b)+c", "xabbbcy", "1 5", "4 4", NULL);
    suite->add("m", "27", "-", "a(b+)c", "xabbbcy", "abbbc", "bbb", NULL);
    suite->add("i", "28", "Q", "a(b){2,3}c", "xabbbcy", "1 5", "4 4", NULL);
    suite->add("i", "29", "Q", "a(b){2,3}c", "xabbcy", "1 4", "3 3", NULL);
    suite->add("f", "30", "Q", "a(b){2,3}c", "xabcy", NULL);
    suite->add("m", "31", "LP", "\\y(\\w+)\\y", "-- abc-", "abc", "abc", NULL);
    suite->add("m", "32", "-", "a((b|c)d+)+", "abacdbd", "acdbd", "bd", "b", NULL);
    suite->add("m", "33", "N", "(.*).*", "abc", "abc", "abc", NULL);
    suite->add("m", "34", "N", "(a*)*", "bc", "", "", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_21, "regex.21");


/*
 * 22 multicharacter collating elements
 */

class regextest_22 : public RegExTestSuite
{
public:
    regextest_22() : RegExTestSuite("regex.22") { }
    static Test *suite();
};

Test *regextest_22::suite()
{
    RegExTestSuite *suite = new regextest_22;

    suite->add("m", "1", "&+L", "a[c]e", "ace", "ace", NULL);
    suite->add("f", "2", "&+IL", "a[c]h", "ach", NULL);
    suite->add("m", "3", "&+L", "a[[.ch.]]", "ach", "ach", NULL);
    suite->add("f", "4", "&+L", "a[[.ch.]]", "ace", NULL);
    suite->add("m", "5", "&+L", "a[c[.ch.]]", "ac", "ac", NULL);
    suite->add("m", "6", "&+L", "a[c[.ch.]]", "ace", "ac", NULL);
    suite->add("m", "7", "&+L", "a[c[.ch.]]", "ache", "ach", NULL);
    suite->add("f", "8", "&+L", "a[^c]e", "ace", NULL);
    suite->add("m", "9", "&+L", "a[^c]e", "abe", "abe", NULL);
    suite->add("m", "10", "&+L", "a[^c]e", "ache", "ache", NULL);
    suite->add("f", "11", "&+L", "a[^[.ch.]]", "ach", NULL);
    suite->add("m", "12", "&+L", "a[^[.ch.]]", "ace", "ac", NULL);
    suite->add("m", "13", "&+L", "a[^[.ch.]]", "ac", "ac", NULL);
    suite->add("m", "14", "&+L", "a[^[.ch.]]", "abe", "ab", NULL);
    suite->add("f", "15", "&+L", "a[^c[.ch.]]", "ach", NULL);
    suite->add("f", "16", "&+L", "a[^c[.ch.]]", "ace", NULL);
    suite->add("f", "17", "&+L", "a[^c[.ch.]]", "ac", NULL);
    suite->add("m", "18", "&+L", "a[^c[.ch.]]", "abe", "ab", NULL);
    suite->add("m", "19", "&+L", "a[^b]", "ac", "ac", NULL);
    suite->add("m", "20", "&+L", "a[^b]", "ace", "ac", NULL);
    suite->add("m", "21", "&+L", "a[^b]", "ach", "ach", NULL);
    suite->add("f", "22", "&+L", "a[^b]", "abe", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_22, "regex.22");


/*
 * 23 lookahead constraints
 */

class regextest_23 : public RegExTestSuite
{
public:
    regextest_23() : RegExTestSuite("regex.23") { }
    static Test *suite();
};

Test *regextest_23::suite()
{
    RegExTestSuite *suite = new regextest_23;

    suite->add("m", "1", "HP", "a(?=b)b*", "ab", "ab", NULL);
    suite->add("f", "2", "HP", "a(?=b)b*", "a", NULL);
    suite->add("m", "3", "HP", "a(?=b)b*(?=c)c*", "abc", "abc", NULL);
    suite->add("f", "4", "HP", "a(?=b)b*(?=c)c*", "ab", NULL);
    suite->add("f", "5", "HP", "a(?!b)b*", "ab", NULL);
    suite->add("m", "6", "HP", "a(?!b)b*", "a", "a", NULL);
    suite->add("m", "7", "HP", "(?=b)b", "b", "b", NULL);
    suite->add("f", "8", "HP", "(?=b)b", "a", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_23, "regex.23");


/*
 * 24 non-greedy quantifiers
 */

class regextest_24 : public RegExTestSuite
{
public:
    regextest_24() : RegExTestSuite("regex.24") { }
    static Test *suite();
};

Test *regextest_24::suite()
{
    RegExTestSuite *suite = new regextest_24;

    suite->add("m", "1", "PT", "ab+?", "abb", "ab", NULL);
    suite->add("m", "2", "PT", "ab+?c", "abbc", "abbc", NULL);
    suite->add("m", "3", "PT", "ab*?", "abb", "a", NULL);
    suite->add("m", "4", "PT", "ab*?c", "abbc", "abbc", NULL);
    suite->add("m", "5", "PT", "ab??", "ab", "a", NULL);
    suite->add("m", "6", "PT", "ab??c", "abc", "abc", NULL);
    suite->add("m", "7", "PQT", "ab{2,4}?", "abbbb", "abb", NULL);
    suite->add("m", "8", "PQT", "ab{2,4}?c", "abbbbc", "abbbbc", NULL);
    suite->add("m", "9", "-", "3z*", "123zzzz456", "3zzzz", NULL);
    suite->add("m", "10", "PT", "3z*?", "123zzzz456", "3", NULL);
    suite->add("m", "11", "-", "z*4", "123zzzz456", "zzzz4", NULL);
    suite->add("m", "12", "PT", "z*?4", "123zzzz456", "zzzz4", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_24, "regex.24");


/*
 * 25 mixed quantifiers
 */

class regextest_25 : public RegExTestSuite
{
public:
    regextest_25() : RegExTestSuite("regex.25") { }
    static Test *suite();
};

Test *regextest_25::suite()
{
    RegExTestSuite *suite = new regextest_25;

    suite->add("m", "1", "PNT", "^(.*?)(a*)$", "xyza", "xyza", "xyz", "a", NULL);
    suite->add("m", "2", "PNT", "^(.*?)(a*)$", "xyzaa", "xyzaa", "xyz", "aa", NULL);
    suite->add("m", "3", "PNT", "^(.*?)(a*)$", "xyz", "xyz", "xyz", "", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_25, "regex.25");


/*
 * 26 tricky cases
 */

class regextest_26 : public RegExTestSuite
{
public:
    regextest_26() : RegExTestSuite("regex.26") { }
    static Test *suite();
};

Test *regextest_26::suite()
{
    RegExTestSuite *suite = new regextest_26;

    suite->add("m", "1", "-", "(week|wee)(night|knights)", "weeknights", "weeknights", "wee", "knights", NULL);
    suite->add("m", "2", "RP", "a(bc*).*\\1", "abccbccb", "abccbccb", "b", NULL);
    suite->add("m", "3", "-", "a(b.[bc]*)+", "abcbd", "abcbd", "bd", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_26, "regex.26");


/*
 * 27 implementation misc.
 */

class regextest_27 : public RegExTestSuite
{
public:
    regextest_27() : RegExTestSuite("regex.27") { }
    static Test *suite();
};

Test *regextest_27::suite()
{
    RegExTestSuite *suite = new regextest_27;

    suite->add("m", "1", "P", "a(?:b|b)c", "abc", "abc", NULL);
    suite->add("m", "2", "&", "[ab][ab][ab]", "aba", "aba", NULL);
    suite->add("m", "3", "&", "[ab][ab][ab][ab][ab][ab][ab]", "abababa", "abababa", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_27, "regex.27");


/*
 * 28 boundary busters etc.
 */

class regextest_28 : public RegExTestSuite
{
public:
    regextest_28() : RegExTestSuite("regex.28") { }
    static Test *suite();
};

Test *regextest_28::suite()
{
    RegExTestSuite *suite = new regextest_28;

    suite->add("m", "1", "&", "abcdefghijkl", "abcdefghijkl", "abcdefghijkl", NULL);
    suite->add("m", "2", "P", "a(?:b|c|d|e|f|g|h|i|j|k|l|m)n", "agn", "agn", NULL);
    suite->add("m", "3", "-", "a(((((((((((((b)))))))))))))c", "abc", "abc", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", "b", NULL);
    suite->add("m", "4", "Q", "ab{1,100}c", "abbc", "abbc", NULL);
    suite->add("m", "5", "Q", "ab{1,100}c", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", NULL);
    suite->add("m", "6", "Q", "ab{1,100}c", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc", NULL);
    suite->add("m", "7", "LP", "\\w+abcdefgh", "xyzabcdefgh", "xyzabcdefgh", NULL);
    suite->add("m", "8", "%LP", "\\w+abcdefgh", "xyzabcdefgh", "xyzabcdefgh", NULL);
    suite->add("m", "9", "%LP", "\\w+abcdefghijklmnopqrst", "xyzabcdefghijklmnopqrst", "xyzabcdefghijklmnopqrst", NULL);
    suite->add("i", "10", "%LP", "\\w+(abcdefgh)?", "xyz", "0 2", "-1 -1", NULL);
    suite->add("i", "11", "%LP", "\\w+(abcdefgh)?", "xyzabcdefg", "0 9", "-1 -1", NULL);
    suite->add("i", "12", "%LP", "\\w+(abcdefghijklmnopqrst)?", "xyzabcdefghijklmnopqrs", "0 21", "-1 -1", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_28, "regex.28");


/*
 * 29 incomplete matches
 */

class regextest_29 : public RegExTestSuite
{
public:
    regextest_29() : RegExTestSuite("regex.29") { }
    static Test *suite();
};

Test *regextest_29::suite()
{
    RegExTestSuite *suite = new regextest_29;

    suite->add("p", "1", "t", "def", "abc", "3 2", "", NULL);
    suite->add("p", "2", "t", "bcd", "abc", "1 2", "", NULL);
    suite->add("p", "3", "t", "abc", "abab", "0 3", "", NULL);
    suite->add("p", "4", "t", "abc", "abdab", "3 4", "", NULL);
    suite->add("i", "5", "t", "abc", "abc", "0 2", "0 2", NULL);
    suite->add("i", "6", "t", "abc", "xyabc", "2 4", "2 4", NULL);
    suite->add("p", "7", "t", "abc+", "xyab", "2 3", "", NULL);
    suite->add("i", "8", "t", "abc+", "xyabc", "2 4", "2 4", NULL);
    suite->add("i", "10", "t", "abc+", "xyabcdd", "2 4", "7 6", NULL);
    suite->add("p", "11", "tPT", "abc+?", "xyab", "2 3", "", NULL);
    suite->add("i", "12", "tPT", "abc+?", "xyabc", "2 4", "5 4", NULL);
    suite->add("i", "13", "tPT", "abc+?", "xyabcc", "2 4", "6 5", NULL);
    suite->add("i", "14", "tPT", "abc+?", "xyabcd", "2 4", "6 5", NULL);
    suite->add("i", "15", "tPT", "abc+?", "xyabcdd", "2 4", "7 6", NULL);
    suite->add("i", "16", "t", "abcd|bc", "xyabc", "3 4", "2 4", NULL);
    suite->add("p", "17", "tn", ".*k", "xx\nyyy", "3 5", "", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_29, "regex.29");


/*
 * 30 misc. oddities and old bugs
 */

class regextest_30 : public RegExTestSuite
{
public:
    regextest_30() : RegExTestSuite("regex.30") { }
    static Test *suite();
};

Test *regextest_30::suite()
{
    RegExTestSuite *suite = new regextest_30;

    suite->add("e", "1", "&", "***", "BADRPT", NULL);
    suite->add("m", "2", "N", "a?b*", "abb", "abb", NULL);
    suite->add("m", "3", "N", "a?b*", "bb", "bb", NULL);
    suite->add("m", "4", "&", "a*b", "aab", "aab", NULL);
    suite->add("m", "5", "&", "^a*b", "aaaab", "aaaab", NULL);
    suite->add("m", "6", "&M", "[0-6][1-2][0-3][0-6][1-6][0-6]", "010010", "010010", NULL);
    suite->add("m", "7", "s", "abc", "abcd", "abc", NULL);
    suite->add("f", "8", "s", "abc", "xabcd", NULL);
    suite->add("m", "9", "HLP", "(?n)^(?![t#])\\S+", "tk\n\n#\n#\nit0", "it0", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_30, "regex.30");


/*
 * extra_1 checks for bug fixes
 */

class regextest_extra_1 : public RegExTestSuite
{
public:
    regextest_extra_1() : RegExTestSuite("regex.extra_1") { }
    static Test *suite();
};

Test *regextest_extra_1::suite()
{
    RegExTestSuite *suite = new regextest_extra_1;

    suite->add("m", "Bug 230589", "o", "[ ]*(^|[^%])%V", "*%V2", NULL);
    suite->add("m", "Bug 504785", "-", "([^_.]*)([^.]*)\\.(..)(.).*", "bbcos_001_c01.q1la", "bbcos_001_c01.q1la", "bbcos", "_001_c01", "q1", "l", NULL);
    suite->add("m", "Bug 505048", "o", "\\A\\s*[^<]*\\s*<([^>]+)>", "a<a>", NULL);
    suite->add("m", "Bug 505048", "o", "\\A\\s*([^b]*)b", "ab", NULL);
    suite->add("m", "Bug 505048", "o", "\\A\\s*[^b]*(b)", "ab", NULL);
    suite->add("m", "Bug 505048", "o", "\\A(\\s*)[^b]*(b)", "ab", NULL);
    suite->add("m", "Bug 505048", "o", "\\A\\s*[^b]*b", "ab", NULL);
    suite->add("m", "Bug 505048", "-", "\\A\\s*[^b]*b", "ab", "ab", NULL);
    suite->add("i", "Bug 505048", "-", "\\A\\s*[^b]*b", "ab", "0 1", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_extra_1, "regex.extra_1");


/*
 * wx_1 character classification: ascii
 */

class regextest_wx_1 : public RegExTestSuite
{
public:
    regextest_wx_1() : RegExTestSuite("regex.wx_1") { }
    static Test *suite();
};

Test *regextest_wx_1::suite()
{
    RegExTestSuite *suite = new regextest_wx_1;

    suite->add("m", "1", "&", "[^[:alnum:]]", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!", "!", NULL);
    suite->add("m", "2", "&", "[[:alnum:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~X", "X", NULL);
    suite->add("m", "3", "&", "[^[:alpha:]]", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!", "!", NULL);
    suite->add("m", "4", "&", "[[:alpha:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`{|}~X", "X", NULL);
    suite->add("m", "5", "&", "[^[:cntrl:]]", "\a\b\t\n\v\f\r!", "!", NULL);
    suite->add("m", "6", "&", "[[:cntrl:]]", " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n", "\n", NULL);
    suite->add("m", "7", "&", "[^[:digit:]]", "0123456789!", "!", NULL);
    suite->add("m", "8", "&", "[[:digit:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ 0", "0", NULL);
    suite->add("m", "9", "&", "[^[:graph:]]", "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n", "\n", NULL);
    suite->add("m", "10", "&", "[[:graph:]]", "\a\b\t\n\v\f\r !", "!", NULL);
    suite->add("m", "11", "&", "[^[:lower:]]", "abcdefghijklmnopqrstuvwxyz!", "!", NULL);
    suite->add("m", "12", "&", "[[:lower:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`{|}~x", "x", NULL);
    suite->add("m", "13", "&", "[^[:print:]]", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n", "\n", NULL);
    suite->add("m", "14", "&", "[[:print:]]", "\a\b\n\v\f\rX", "X", NULL);
    suite->add("m", "15", "&", "[^[:punct:]]", "!\"#%&'()*,-./:;?@[\\]_{}X", "X", NULL);
    suite->add("m", "16", "&", "[[:punct:]]", "\a\b\t\n\v\f\r 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!", "!", NULL);
    suite->add("m", "17", "&", "[^[:space:]]", "\t\n\v\f\r X", "X", NULL);
    suite->add("m", "18", "&", "[[:space:]]", "\a\b!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\n", "\n", NULL);
    suite->add("m", "19", "&", "[^[:upper:]]", "ABCDEFGHIJKLMNOPQRSTUVWXYZ!", "!", NULL);
    suite->add("m", "20", "&", "[[:upper:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./0123456789:;<=>?@[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~X", "X", NULL);
    suite->add("m", "21", "&", "[^[:xdigit:]]", "0123456789ABCDEFabcdef!", "!", NULL);
    suite->add("m", "22", "&", "[[:xdigit:]]", "\a\b\t\n\v\f\r !\"#$%&'()*+,-./:;<=>?@GHIJKLMNOPQRSTUVWXYZ[\\]^_`ghijklmnopqrstuvwxyz{|}~a", "a", NULL);
    suite->add("i", "23", "&i", "AbCdEfGhIjKlMnOpQrStUvWxYz", "aBcDeFgHiJkLmNoPqRsTuVwXyZ", "0 25", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_wx_1, "regex.wx_1");


/*
 * wx_2 character classification: western european
 */

class regextest_wx_2 : public RegExTestSuite
{
public:
    regextest_wx_2() : RegExTestSuite("regex.wx_2") { }
    static Test *suite();
};

Test *regextest_wx_2::suite()
{
    RegExTestSuite *suite = new regextest_wx_2;

    suite->add("m", "1", "&*", "[^[:alpha:]]", "\303\200\303\201\303\202\303\203\303\204\303\205\303\206\303\207\303\210\303\211\303\212\303\213\303\214\303\215\303\216\303\217\303\220\303\221\303\222\303\223\303\224\303\225\303\226\303\230\303\231\303\232\303\233\303\234\303\235\303\236\303\237\303\240\303\241\303\242\303\243\303\244\303\245\303\246\303\247\303\250\303\251\303\252\303\253\303\254\303\255\303\256\303\257\303\260\303\261\303\262\303\263\303\264\303\265\303\266\303\270\303\271\303\272\303\273\303\274\303\275\303\276\303\277!", "!", NULL);
    suite->add("m", "2", "&*", "[[:alpha:]]", "\302\240\302\241\302\242\302\243\302\244\302\245\302\246\302\247\302\250\302\251\302\253\302\254\302\255\302\256\302\257\302\260\302\261\302\262\302\263\302\264\302\266\302\267\302\270\302\271\302\273\302\274\302\275\302\276\302\277\303\227\303\267X", "X", NULL);
    suite->add("m", "3", "&*", "[^[:lower:]]", "\303\237\303\240\303\241\303\242\303\243\303\244\303\245\303\246\303\247\303\250\303\251\303\252\303\253\303\254\303\255\303\256\303\257\303\260\303\261\303\262\303\263\303\264\303\265\303\266\303\270\303\271\303\272\303\273\303\274\303\275\303\276\303\277!", "!", NULL);
    suite->add("m", "4", "&*", "[[:lower:]]", "\302\240\302\241\302\242\302\243\302\244\302\245\302\246\302\247\302\250\302\251\302\253\302\254\302\255\302\256\302\257\302\260\302\261\302\262\302\263\302\264\302\266\302\267\302\270\302\271\302\273\302\274\302\275\302\276\302\277\303\200\303\201\303\202\303\203\303\204\303\205\303\206\303\207\303\210\303\211\303\212\303\213\303\214\303\215\303\216\303\217\303\220\303\221\303\222\303\223\303\224\303\225\303\226\303\227\303\230\303\231\303\232\303\233\303\234\303\235\303\236\303\267x", "x", NULL);
    suite->add("m", "5", "&*", "[^[:upper:]]", "\303\200\303\201\303\202\303\203\303\204\303\205\303\206\303\207\303\210\303\211\303\212\303\213\303\214\303\215\303\216\303\217\303\220\303\221\303\222\303\223\303\224\303\225\303\226\303\230\303\231\303\232\303\233\303\234\303\235\303\236!", "!", NULL);
    suite->add("m", "6", "&*", "[[:upper:]]", "\302\240\302\241\302\242\302\243\302\244\302\245\302\246\302\247\302\250\302\251\302\253\302\254\302\255\302\256\302\257\302\260\302\261\302\262\302\263\302\264\302\266\302\267\302\270\302\271\302\273\302\274\302\275\302\276\302\277\303\227\303\237\303\240\303\241\303\242\303\243\303\244\303\245\303\246\303\247\303\250\303\251\303\252\303\253\303\254\303\255\303\256\303\257\303\260\303\261\303\262\303\263\303\264\303\265\303\266\303\267\303\270\303\271\303\272\303\273\303\274\303\275\303\276\303\277X", "X", NULL);
    suite->add("i", "7", "&i*", "\303\200\303\241\303\202\303\243\303\204\303\245\303\206\303\247\303\210\303\251\303\212\303\253\303\214\303\255\303\216\303\257\303\220\303\261\303\222\303\263\303\224\303\265\303\226\303\270\303\231\303\272\303\233\303\274\303\235\303\276", "\303\240\303\201\303\242\303\203\303\244\303\205\303\246\303\207\303\250\303\211\303\252\303\213\303\254\303\215\303\256\303\217\303\260\303\221\303\262\303\223\303\264\303\225\303\266\303\230\303\271\303\232\303\273\303\234\303\275\303\236", "0 29", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_wx_2, "regex.wx_2");


/*
 * wx_3 character classification: cyrillic
 */

class regextest_wx_3 : public RegExTestSuite
{
public:
    regextest_wx_3() : RegExTestSuite("regex.wx_3") { }
    static Test *suite();
};

Test *regextest_wx_3::suite()
{
    RegExTestSuite *suite = new regextest_wx_3;

    suite->add("m", "1", "&*", "[^[:alpha:]]", "\321\221\320\201\321\216\320\260\320\261\321\206\320\264\320\265\321\204\320\263\321\205\320\270\320\271\320\272\320\273\320\274\320\275\320\276\320\277\321\217\321\200\321\201\321\202\321\203\320\266\320\262\321\214\321\213\320\267\321\210\321\215\321\211\321\207\321\212\320\256\320\220\320\221\320\246\320\224\320\225\320\244\320\223\320\245\320\230\320\231\320\232\320\233\320\234\320\235\320\236\320\237\320\257\320\240\320\241\320\242\320\243\320\226\320\222\320\254\320\253\320\227\320\250\320\255\320\251\320\247\320\252!", "!", NULL);
    suite->add("m", "2", "&*", "[^[:lower:]]", "\321\221\321\216\320\260\320\261\321\206\320\264\320\265\321\204\320\263\321\205\320\270\320\271\320\272\320\273\320\274\320\275\320\276\320\277\321\217\321\200\321\201\321\202\321\203\320\266\320\262\321\214\321\213\320\267\321\210\321\215\321\211\321\207\321\212!", "!", NULL);
    suite->add("m", "3", "&*", "[[:lower:]]", "\320\201\320\256\320\220\320\221\320\246\320\224\320\225\320\244\320\223\320\245\320\230\320\231\320\232\320\233\320\234\320\235\320\236\320\237\320\257\320\240\320\241\320\242\320\243\320\226\320\222\320\254\320\253\320\227\320\250\320\255\320\251\320\247\320\252x", "x", NULL);
    suite->add("m", "4", "&*", "[^[:upper:]]", "\320\201\320\256\320\220\320\221\320\246\320\224\320\225\320\244\320\223\320\245\320\230\320\231\320\232\320\233\320\234\320\235\320\236\320\237\320\257\320\240\320\241\320\242\320\243\320\226\320\222\320\254\320\253\320\227\320\250\320\255\320\251\320\247\320\252!", "!", NULL);
    suite->add("m", "5", "&*", "[[:upper:]]", "\321\221\321\216\320\260\320\261\321\206\320\264\320\265\321\204\320\263\321\205\320\270\320\271\320\272\320\273\320\274\320\275\320\276\320\277\321\217\321\200\321\201\321\202\321\203\320\266\320\262\321\214\321\213\320\267\321\210\321\215\321\211\321\207\321\212X", "X", NULL);
    suite->add("i", "6", "&i*", "\320\201\321\216\320\220\320\261\320\246\320\264\320\225\321\204\320\223\321\205\320\230\320\271\320\232\320\273\320\234\320\275\320\236\320\277\320\257\321\200\320\241\321\202\320\243\320\266\320\222\321\214\320\253\320\267\320\250\321\215\320\251\321\207\320\252", "\321\221\320\256\320\260\320\221\321\206\320\224\320\265\320\244\320\263\320\245\320\270\320\231\320\272\320\233\320\274\320\235\320\276\320\237\321\217\320\240\321\201\320\242\321\203\320\226\320\262\320\254\321\213\320\227\321\210\320\255\321\211\320\247\321\212", "0 32", NULL);

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest_wx_3, "regex.wx_3");


/*
 * A suite containing all the above suites
 */

class regextest : public TestSuite
{
public:
    regextest() : TestSuite("regex") { }
    static Test *suite();
};

Test *regextest::suite()
{
    TestSuite *suite = new regextest;

    suite->addTest(regextest_1::suite());
    suite->addTest(regextest_2::suite());
    suite->addTest(regextest_3::suite());
    suite->addTest(regextest_4::suite());
    suite->addTest(regextest_5::suite());
    suite->addTest(regextest_6::suite());
    suite->addTest(regextest_7::suite());
    suite->addTest(regextest_8::suite());
    suite->addTest(regextest_9::suite());
    suite->addTest(regextest_10::suite());
    suite->addTest(regextest_11::suite());
    suite->addTest(regextest_12::suite());
    suite->addTest(regextest_13::suite());
    suite->addTest(regextest_14::suite());
    suite->addTest(regextest_15::suite());
    suite->addTest(regextest_16::suite());
    suite->addTest(regextest_17::suite());
    suite->addTest(regextest_18::suite());
    suite->addTest(regextest_19::suite());
    suite->addTest(regextest_20::suite());
    suite->addTest(regextest_21::suite());
    suite->addTest(regextest_22::suite());
    suite->addTest(regextest_23::suite());
    suite->addTest(regextest_24::suite());
    suite->addTest(regextest_25::suite());
    suite->addTest(regextest_26::suite());
    suite->addTest(regextest_27::suite());
    suite->addTest(regextest_28::suite());
    suite->addTest(regextest_29::suite());
    suite->addTest(regextest_30::suite());
    suite->addTest(regextest_extra_1::suite());
    suite->addTest(regextest_wx_1::suite());
    suite->addTest(regextest_wx_2::suite());
    suite->addTest(regextest_wx_3::suite());

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest, "regex");
CPPUNIT_TEST_SUITE_REGISTRATION(regextest);
