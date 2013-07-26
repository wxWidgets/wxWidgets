#!/usr/bin/env perl
#############################################################################
# Name:        regex.pl
# Purpose:     Generate test code for wxRegEx from 'reg.test'
# Author:      Mike Wetherell
# Copyright:   (c) Mike Wetherell
# Licence:     wxWindows licence
#############################################################################

#
# Notes:
#   See './regex.pl -h' for usage
#
#   Output at the moment is C++ using the cppunit testing framework. The
#   language/framework specifics are separated, with the following 5
#   subs as an interface: 'begin_output', 'begin_section', 'write_test',
#   'end_section' and 'end_output'. So for a different language/framework,
#   implement 5 new similar subs.
# 
#   I've avoided using 'use encoding "UTF-8"', since this wasn't available
#   in perl 5.6.x. Instead I've used some hacks like 'pack "U0C*"'. Versions
#   earler than perl 5.6.0 aren't going to work.
#

use strict;
use warnings;
use File::Basename;
#use encoding "UTF-8";  # enable in the future when perl 5.6.x is just a memory

# if 0 output is wide characters, if 1 output is utf8 encoded
my $utf = 1;

# quote a parameter (C++ helper)
#
sub quotecxx {
    my %esc = ( "\a" => "a", "\b" => "b", "\f" => "f",
                "\n" => "n", "\r" => "r", "\t" => "t",
                "\013" => "v", '"' => '"', "\\" => "\\" );

    # working around lack of 'use encoding'
    if (!$utf) {
        $_ = pack "U0C*", unpack "C*", $_;
        use utf8;
    }

    s/[\000-\037"\\\177-\x{ffff}]/
        if ($esc{$&}) {
            "\\$esc{$&}";
        } elsif (ord($&) > 0x9f && !$utf) {
            sprintf "\\u%04x", ord($&);
        } else {
            sprintf "\\%03o", ord($&);
        }
    /ge;

    # working around lack of 'use encoding'
    if (!$utf) {
        no utf8;
        $_ = pack "C*", unpack "C*", $_;
    }

    return ($utf ? '"' : 'L"') . $_ . '"'
}

# start writing the output code (C++ interface)
#
sub begin_output {
    my ($from, $instructions) = @_;

    # embed it in the comment
    $from = "\n$from";
    $from =~ s/^(?:   )?/ * /mg;

    # $instructions contains information about the flags etc.
    if ($instructions) {
        $instructions = "\n$instructions";
        $instructions =~ s/^(?:   )?/ * /mg;
    }

    my $u = $utf ? " (UTF-8 encoded)" : "";

    print <<EOT;
/*
 * Test data for wxRegEx$u
$from$instructions */

EOT
}

my @classes;

# start a new section (C++ interface)
#
sub begin_section {
    my ($id, $title) = @_;
    my $class = "regextest_$id";
    $class =~ s/\W/_/g;
    push @classes, [$id, $class];

    print <<EOT;

/*
 * $id $title
 */

class $class : public RegExTestSuite
{
public:
    $class() : RegExTestSuite("regex.$id") { }
    static Test *suite();
};

Test *$class\::suite()
{
    RegExTestSuite *suite = new $class;

EOT
}

# output a test line (C++ interface)
#
sub write_test {
    my @args = @_;
    $_ = quotecxx for @args;
    print "    suite->add(" . (join ', ', @args) . ", NULL);\n"; 
}

# end a section (C++ interface)
#
sub end_section {
    my ($id, $class) = @{$classes[$#classes]};

    print <<EOT;

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION($class, "regex.$id");

EOT
}

# finish off the output (C++ interface)
#
sub end_output {
    print <<EOT;

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

EOT
    print "    suite->addTest(".$_->[1]."::suite());\n" for @classes;

    print <<EOT;

    return suite;
}

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(regextest, "regex");
CPPUNIT_TEST_SUITE_REGISTRATION(regextest);
EOT
}

# Parse a tcl string. Handles curly quoting and double quoting.
#
sub parsetcl {
    my ($curly, $quote);
    # recursively defined expression that can parse balanced braces
    # warning: uses experimental features of perl, see perlop(1)
    $curly = qr/\{(?:(?>(?:\\[{}]|[^{}])+)|(??{$curly}))*\}/;
    $quote = qr/"(?:\\"|[^"])*"/;
    my @tokens = shift =~ /($curly|$quote|\S+)/g;

    # now remove braces/quotes and unescape any escapes
    for (@tokens) {
        if (s/^{(.*)}$/$1/) {
            # for curly quoting, only unescape \{ and \}
            s/\\([{}])/$1/g;
        } else {
            s/^"(.*)"$/$1/;

            # unescape any escapes
            my %esc = ( "a" => "\a", "b" => "\b", "f" => "\f",
                        "n" => "\n", "r" => "\r", "t" => "\t",
                        "v" => "\013" );
            my $x = qr/[[:xdigit:]]/;

            s/\\([0-7]{1,3}|x$x+|u$x{1,4}|.)/
                if ($1 =~ m{^([0-7]+)}) {
                    chr(oct($1));
                } elsif ($1 =~ m{^x($x+)}) {
                    pack("C0U", hex($1) & 0xff);
                } elsif ($1 =~ m{^u($x+)}) {
                    pack("C0U", hex($1));
                } elsif ($esc{$1}) {
                    $esc{$1};
                } else {
                    $1;
                }
            /ge;
        }
    }

    return @tokens;
}

# helpers which keep track of whether begin_section has been called, so that
# end_section can be called when appropriate
#
my @doing = ("0", "");
my $in_section = 0;

sub handle_doing {
    end_section if $in_section;
    $in_section = 0;
    @doing = @_;
}

sub handle_test {
    begin_section(@doing) if !$in_section;
    $in_section = 1;
    write_test @_;
}

sub handle_end {
    end_section if $in_section;
    $in_section = 0;
    end_output;
}

# 'main' - start by parsing the command lines options.
#
my $badoption = !@ARGV;
my $utfdefault = $utf;
my $outputname;

for (my $i = 0; $i < @ARGV; ) {
    if ($ARGV[$i] !~ m{^-.}) {
        $i++;
        next;
    }

    if ($ARGV[$i] eq '--') {
        splice @ARGV, $i, 1;
        last;
    }

    if ($ARGV[$i] =~ s{^-(.*)o(.*)$}{-$1}i) {       # -o : output file
        $outputname = $2 || splice @ARGV, $i + 1, 1;
    }

    for (split //, substr($ARGV[$i], 1)) {
        if (/u/i) {                                 # -u : utf-8 output
            $utf = 1;
        } elsif (/w/i) {                            # -w : wide char output
            $utf = 0;
        } else {
            $badoption = 1;
        }
    }

    splice @ARGV, $i, 1;
}

# Display help
#
if ($badoption) {
    my $prog = basename $0;
    my ($w, $u) = (" (default)", "          ");
    ($w, $u) = ($u, $w) if $utfdefault;
    
    print <<EOT;
Usage: $prog [-u|-w] [-o OUTPUT] [FILE...]
Generate test code for wxRegEx from 'reg.test'
Example: $prog -o regex.inc reg.test wxreg.test 

 -w$w   Output will be wide characters.
 -u$u   Output will be UTF-8 encoded.

Input files should be in UTF-8. If no input files are specified input is
read from stdin. If no output file is specified output is written to stdout.
See the comments in reg.test for details of the input file format.
EOT
    exit 0;
}

# Open the output file
#
open STDOUT, ">$outputname" if $outputname;

# Read in the files and initially parse just the comments for copyright
# information and instructions on the tests
#
my @input;                  # slurped input files stripped of comments
my $files = "";             # copyright info from the input comments
my $instructions = "";      # test instructions from the input comments

do {
    my $inputname = basename $ARGV[0] if @ARGV;

    # slurp input
    undef $/;
    my $in = <>;

    # remove escaped newlines
    $in =~ s/(?<!\\)\\\n//g;

    # record the copyrights of the input files
    for ($in =~ /^#[\t ]*(.*copyright.*)$/mig) {
        s/[\s:]+/ /g;
        $files .= "  ";
        $files .= $inputname . ": " if $inputname && $inputname ne '-';
        $files .= "$_\n";
    }

    # Parse the comments for instructions on the tests, which look like this:
    #    i    successful match with -indices (used in checking things like
    #         nonparticipating subexpressions)
    if (!$instructions) {
        my $sp = qr{\t|   +};                   # tab or three or more spaces
        my @instructions = $in =~
            /\n(
                (?:
                    \#$sp\S?$sp\S[^\n]+\n       # instruction line
                    (?:\#$sp$sp\S[^\n]+\n)*     # continuation lines (if any)
                )+
            )/gx;

        if (@instructions) {
            $instructions[0] = "Test types:\n$instructions[0]";
            if (@instructions > 1) {
                $instructions[1] = "Flag characters:\n$instructions[1]";
            }
            $instructions = join "\n", @instructions;
            $instructions =~ s/^#([^\t]?)/ $1/mg;
        }
    }

    # @input is the input of all files (stipped of comments)
    $in =~ s/^#.*$//mg;
    push @input, $in;

} while $ARGV[0];

# Make a string naming the generator, the input files and copyright info
#
my $from = "Generated " . localtime() . " by " . basename $0;
$from =~ s/[\s]+/ /g;
if ($files) {
    if ($files =~ /:/) {
        $from .= " from the following files:";
    } else {
        $from .= " from work with the following copyright:";
    }
}
$from = join("\n", $from =~ /(.{0,76}(?:\s|$))/g);  # word-wrap
$from .= "\n$files" if $files;

# Now start to print the code
#
begin_output $from, $instructions;

# numbers for 'extra' sections
my $extra = 1;

for (@input)
{
    # Print the main tests
    #
    # Test lines look like this:
    # m  3  b       {\(a\)b}        ab      ab      a
    # 
    # Also looks for heading lines, e.g.:
    # doing 4 "parentheses"
    #
    for (split "\n") {
        if (/^doing\s+(\S+)\s+(\S.*)/) {
            handle_doing parsetcl "$1 $2";
        } elsif (/^[efimp]\s/) {
            handle_test parsetcl $_;
        }
    }

    # Extra tests
    #
    # The expression below matches something like this:
    #   test reg-33.8 {Bug 505048} {
    #       regexp -inline {\A\s*[^b]*b} ab
    #   } ab
    #   
    # The three subexpressions then return these parts: 
    #   $extras[$i]     = '{Bug 505048}',
    #   $extras[$i + 1] = '-inline {\A\s*[^b]*b} ab'
    #   $extras[$i + 2] = 'ab'
    #
    my @extras = /\ntest\s+\S+\s*(\{.*?\})\s*\{\n       # line 1
                  \s*regexp\s+([^\n]+)\n                # line 2
                  \}\s*(\S[^\n]*)/gx;                   # line 3

    handle_doing "extra_" . $extra++, "checks for bug fixes" if @extras;

    for (my $i = 0; $i < @extras; $i += 3) {
        my $id = $extras[$i];

        # further parse the middle line into options and the rest (i.e. $args)
        my ($opts, $args) = $extras[$i + 1] =~ /^\s*((?:-\S+\s+)*)([^\s-].*)/;

        my @args = parsetcl $args;
        $#args = 1;     # only want the first two

        # now handle the options
        my $test    = $opts =~ /-indices/ ? 'i' : $extras[$i + 2] ? 'm' : 'f';
        my $results = $opts =~ /-inline/ && $test ne 'f' ? $extras[$i+2] : '';

        # get them all in the right order and print
        unshift @args, $test, parsetcl($id), $results ? '-' : 'o';
        push @args, parsetcl(parsetcl($results)) if $results;
        handle_test @args;
    }
}

# finish
#
handle_end;
