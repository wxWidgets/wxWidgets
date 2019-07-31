///////////////////////////////////////////////////////////////////////////////
// Name:        test.mm
// Purpose:     XCTest program for wxWidgets
// Author:      Mike Wetherell
// Copyright:   (c) 2019 Mike Wetherell & Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h"
// and "catch.hpp"
#include "testprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/apptrait.h"
#include "wx/cmdline.h"
#include <exception>
#include <iostream>
#include <inttypes.h>
#include <objc/message.h>

#ifdef __WXOSX__
#include "wx/osx/private.h"
#import <XCTest/XCTest.h>
#endif

#include "reporters/catch_reporter_bases.hpp"
#include "internal/catch_reporter_registrars.hpp"

#include "catch.hpp"

using namespace Catch;

@interface wxXCTestsWrapper : XCTestCase

@end

@interface wxXCTests : XCTestSuite

@end

@interface wxXCTestCase : XCTestCase {
    NSString *_testName;
}

@property (retain) NSString *testName;

@end

@implementation wxXCTestsWrapper

+ (wxXCTests*) defaultTestSuite
{
    return [[wxXCTests alloc] initWithName:@"wxTestsLib"];
}

@end

XCTestSuite* globalSuite = NULL;
XCTestSuiteRun* globalRun = NULL;

@implementation wxXCTests

- (id )initWithName:(NSString *)name
{
    if ( self = [super initWithName:name] )
    {
        globalSuite = self;
    }
    return self;
}

extern "C" int XCTmain(int argc, char **argv);

- (void)performTest:(XCTestSuiteRun *)testSuiteRun
{
    globalRun = testSuiteRun;
    
    int argc = 1;
    char* argv[] = { "/" } ;
    

    [testSuiteRun start];

    wxString cwd = wxGetCwd();
    wxSetWorkingDirectory( cwd + "/tests.xctest/Contents/Resources" ) ;

    XCTmain(argc, argv);
    
    if ( [testSuiteRun startDate] )
        [testSuiteRun stop];
}

@end

@implementation wxXCTestCase

- (void) dummyMethod
{
}

- (instancetype)initWithTest:(const TestCaseInfo*)testInfo section:(const SectionInfo*) sectionInfo invocation:(NSInvocation*) inv ;
{
    if (self = [super initWithInvocation:inv])
    {
        std::string logstring = testInfo->className + "::" + sectionInfo->name;
        _testName = [NSString stringWithUTF8String:logstring.c_str()];
    }
    return self;
}

- (NSString *)nameForLegacyLogging
{
    return _testName;
}

@end

//
// dynamic class and method generation
//

/* Given a test case description, generate a valid Objective-C identifier. */
NSString* normalize_identifier(NSString* description)
{
    /* Split the description into individual components */
    NSArray* components = [description componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
    
    /* Camel case any elements that contain only lowercase letters (otherwise, we assume the case is already correct), and remove
     * any invalid characters that may not occur in an Objective-C identifier. */
    NSMutableArray* cleanedComponents = [NSMutableArray arrayWithCapacity:[components count]];
    NSCharacterSet* allowedChars = [NSCharacterSet characterSetWithCharactersInString:@"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"];
    for (NSUInteger i = 0; i < [components count]; i++)
    {
        NSString* component = components[i];
        NSString* newComponent = [[component componentsSeparatedByCharactersInSet:[allowedChars invertedSet]] componentsJoinedByString:@""];
        
        /* If the string is entirely lowercase, apply camel casing */
        if ([[newComponent lowercaseString] isEqualToString:newComponent])
        {
            newComponent = [newComponent capitalizedString];
        }
        
        /* Add to the result array. */
        [cleanedComponents addObject:newComponent];
    }
    
    /* Generate the identifier, stripping any leading numbers */
    NSString* identifier = [cleanedComponents componentsJoinedByString:@""];
    {
        NSRange range = [identifier rangeOfCharacterFromSet:[NSCharacterSet decimalDigitCharacterSet]];
        if (range.location == 0)
            identifier = [identifier substringFromIndex:range.length];
    }
    
    return identifier;
}

NSString* constructClassName(const char* description)
{
    //    const char* description = testInfo.name.c_str();
    NSString* clsName = normalize_identifier([NSString stringWithUTF8String:description]);
    
    /* If the class name is already in use, loop until we've got a unique name */
    if (NSClassFromString(clsName) != nil)
    {
        /* First, try appending 'Tests'; this handles the case where the tests have the same name as the class being tested. */
        if (![clsName hasSuffix:@"Test"] && ![clsName hasSuffix:@"Tests"])
        {
            NSString* testClassName = [clsName stringByAppendingString:@"Tests"];
            
            if (NSClassFromString(testClassName) == nil)
            {
                clsName = testClassName;
            }
        }
        
        /* Otherwise, try appending a unique number to the name */
        for (NSUInteger i = 0; i < NSUIntegerMax && NSClassFromString(clsName) != nil; i++)
        {
            if (i == NSUIntegerMax)
            {
                [NSException raise:NSInternalInconsistencyException format:@"Couldn't find a unique test name for %@. You must have an impressive number of tests.", clsName];
                __builtin_trap();
            }
            
            NSMutableString* proposedName = [NSMutableString stringWithFormat:@"%@%" PRIu64, clsName, (uint64_t)i];
            if (NSClassFromString(proposedName) == nil)
            {
                clsName = proposedName;
                break;
            }
        }
    }
    return clsName;
}

SEL createSelectorForName(Class cls, const char* description)
{
    /* Generate a valid selector for the description */
    NSString* selectorName = normalize_identifier([NSString stringWithUTF8String:description]);
    
    /* If the selector is already in use, loop until we have a unique name */
    while (class_getInstanceMethod(cls, NSSelectorFromString(selectorName)) != NULL)
    {
        for (NSUInteger i = 0; i < NSUIntegerMax; i++)
        {
            if (i == NSUIntegerMax)
            {
                [NSException raise:NSInternalInconsistencyException format:@"Couldn't find a unique selector name for %s. You must have an impressive number of tests.", description];
                __builtin_trap();
            }
            
            selectorName = [NSString stringWithFormat:@"%@%" PRIu64, selectorName, (uint64_t)i];
            if (class_getInstanceMethod(cls, NSSelectorFromString(selectorName)) == NULL)
                break;
        }
    }
    
    /* Register and return the SEL */
    SEL newSel = NSSelectorFromString(selectorName);
    return newSel;
}

/* Given a test case description and a target class, derive a unique selector selector from the camel cased description, and register it with the target class and imp. */

void dummy_method_implementation(id self, SEL _cmd)
{
}

SEL createTestMethodForName(Class cls, const char* description)
{
    SEL newSel = createSelectorForName(cls, description);
    
    if ( newSel )
    {
        NSString* typeEnc = [NSString stringWithFormat:@"%s%s%s", @encode(void), @encode(id), @encode(SEL)];
        class_addMethod(cls, newSel, (IMP)dummy_method_implementation, [typeEnc UTF8String]);
    }
    
    return newSel;
}

Class createTestClassForName(const char* description)
{
    /* Allocate the new class */
    NSString* clsName = constructClassName(description);
    
    Class cls = objc_allocateClassPair([wxXCTestCase class], [clsName UTF8String], 0);
    if (cls == nil)
    {
        [NSException raise:NSInternalInconsistencyException format:@"Could not allocate test class: %@", clsName];
        __builtin_trap();
    }
    return cls;
}

//
// catch integration
//

namespace Catch
{
    struct XCTestReporterAdapter;
    
    XCTestReporterAdapter* s_instance = NULL;
    
    struct XCTestReporterAdapter : StreamingReporterBase {
        XCTestReporterAdapter( ReporterConfig const& _config )
        :   StreamingReporterBase( _config )
        {
            s_instance = this;
        }
        
        virtual ~XCTestReporterAdapter()
        {
            s_instance = NULL;
        }
        
        static XCTestReporterAdapter* get()
        {
            return s_instance;
        }
        
        static std::string getDescription()
        {
            return "Reports test results for Xcode Tests";
        }
        
        virtual void assertionStarting( AssertionInfo const& ) CATCH_OVERRIDE;
        
        virtual bool assertionEnded( AssertionStats const& /*_assertionStats*/ ) CATCH_OVERRIDE;
        
        //
        
        virtual void testRunStarting( TestRunInfo const& _testRunInfo ) CATCH_OVERRIDE ;
        
        virtual void testGroupStarting( GroupInfo const& _groupInfo ) CATCH_OVERRIDE ;
        
        virtual void testCaseStarting( TestCaseInfo const& _testInfo ) CATCH_OVERRIDE ;
        
        virtual void sectionStarting( SectionInfo const& _sectionInfo ) CATCH_OVERRIDE ;
        
        virtual void sectionEnded( SectionStats const& /* _sectionStats */ ) CATCH_OVERRIDE ;
        
        virtual void testCaseEnded( TestCaseStats const& /* _testCaseStats */ ) CATCH_OVERRIDE ;
        
        virtual void testGroupEnded( TestGroupStats const& /* _testGroupStats */ ) CATCH_OVERRIDE ;
        
        virtual void testRunEnded( TestRunStats const& /* _testRunStats */ ) CATCH_OVERRIDE ;
        
        void startXCTestCase( XCTestCase *test );
        void endXCTestCase();

        void startXCTestSuite( XCTestSuite *test );
        void endXCTestSuite();
        
        void reportFailures( XCTestRun* run, const AssertionStats& as );

        std::vector<XCTestCase*> m_xcTests;
        std::vector<XCTestCaseRun*> m_xcTestCaseRuns;

        std::vector<XCTestSuite*> m_xcTestSuites;
        std::vector<XCTestSuiteRun*> m_xcTestSuiteRuns;
        
        Class m_testClass;
        
        std::vector<AssertionStats> m__collectedAssertions;
    };
    
    REGISTER_REPORTER( "XCTestReporter", XCTestReporterAdapter)
}

void XCTestReporterAdapter::assertionStarting( AssertionInfo const& )
{
}

bool XCTestReporterAdapter::assertionEnded( AssertionStats const& _assertionStats )
{
    XCTestRun* run = m_xcTestCaseRuns.back();
    
    reportFailures(run, _assertionStats);
    
    return true;
}

void XCTestReporterAdapter::testRunStarting( TestRunInfo const& _testRunInfo )
{
}

void XCTestReporterAdapter::testRunEnded( TestRunStats const& /* _testRunStats */ )
{
}

void XCTestReporterAdapter::testGroupStarting( GroupInfo const& _groupInfo )
{
}


void XCTestReporterAdapter::testGroupEnded( TestGroupStats const& /* _testGroupStats */ )
{
}

void XCTestReporterAdapter::testCaseStarting( TestCaseInfo const& _testInfo )
{
    StreamingReporterBase::testCaseStarting(_testInfo);
    XCTestSuite* testSuite = [XCTestSuite testSuiteWithName:[NSString stringWithUTF8String:_testInfo.name.c_str()]];
    
    m_testClass = createTestClassForName(_testInfo.name.c_str());
    
    startXCTestSuite(testSuite);
}

void XCTestReporterAdapter::testCaseEnded( TestCaseStats const& _testCaseStats )
{
    StreamingReporterBase::testCaseEnded( _testCaseStats );
    
    endXCTestSuite();
}

void XCTestReporterAdapter::startXCTestSuite( XCTestSuite * suite)
{
    m_xcTestSuites.push_back(suite);

    [globalSuite addTest:suite];

    XCTestSuiteRun* run = (XCTestSuiteRun*) [suite testRun];
    m_xcTestSuiteRuns.push_back(run);
    [run start];
}

void XCTestReporterAdapter::endXCTestSuite()
{
    XCTestSuite* suite = m_xcTestSuites.back();
    XCTestSuiteRun* run = m_xcTestSuiteRuns.back();

    if ( [run startDate ] )
        [run stop];
    [globalRun addTestRun:run];
}

void XCTestReporterAdapter::sectionStarting( SectionInfo const& _sectionInfo )
{
    StreamingReporterBase::sectionStarting(_sectionInfo);
    
    if (currentTestCaseInfo->name != _sectionInfo.name )
    {
        SEL newTestMethod = createTestMethodForName(m_testClass, _sectionInfo.name.c_str());
        
        NSMethodSignature* sig = [m_testClass instanceMethodSignatureForSelector: newTestMethod];
        NSInvocation* inv = [NSInvocation invocationWithMethodSignature:sig];
        [inv setSelector:newTestMethod];
        [inv setTarget:m_testClass];
        
        wxXCTestCase* currentTest = [[m_testClass alloc] initWithTest: &(*currentTestCaseInfo)
                                                              section: &_sectionInfo
                                                           invocation:inv];
        startXCTestCase(currentTest);
    }
}

void XCTestReporterAdapter::sectionEnded( SectionStats const& _sectionStats )
{
    if (currentTestCaseInfo->name != _sectionStats.sectionInfo.name )
    {
        endXCTestCase();
    }
    
    StreamingReporterBase::sectionEnded( _sectionStats );
}

void XCTestReporterAdapter::startXCTestCase( XCTestCase* testCase )
{
    XCTestSuite* suite = m_xcTestSuites.back();
    [suite addTest:testCase];
    
    m_xcTests.push_back(testCase);
    
    XCTestCaseRun* run = (XCTestCaseRun*) [testCase testRun];
    m_xcTestCaseRuns.push_back(run);
    
    [run start];
}

void XCTestReporterAdapter::endXCTestCase()
{
    XCTestCase* testCase = m_xcTests.back();
    XCTestRun* run = m_xcTestCaseRuns.back();
    
    if ([run startDate])
        [run stop];
    
    m_xcTestCaseRuns.pop_back();
    XCTestSuiteRun* tsr = (XCTestSuiteRun*) m_xcTestSuiteRuns.back();
    [tsr addTestRun:run];
}

void XCTestReporterAdapter::reportFailures( XCTestRun* run, const AssertionStats& as)
{
    auto result = as.assertionResult;
    bool expected = true;
    std::string description = "";
    std::string cause = "";
    
    /* Skip successful tests; we don't need to report anything */
    if (result.getResultType() == ResultWas::Ok)
        return;
    
    /* Skip expected failures; those are a pass */
    if (result.getResultType() == ResultWas::ExpressionFailed && result.isOk())
        return;
    
    switch (result.getResultType())
    {
        case ResultWas::Ok:
            /* Handled above; should never be hit here. */
            __builtin_trap();
            
        case ResultWas::ExpressionFailed:
            if (as.infoMessages.size() == 1)
            {
                cause = "expression failed with message";
            }
            else if (as.infoMessages.size() > 1)
            {
                cause = "expression failed with messages";
            }
            else
            {
                cause = "expression did not evaluate to true";
            }
            break;
            
        case ResultWas::ExplicitFailure:
            cause = "failed";
            break;
            
        case ResultWas::DidntThrowException:
            cause = "the expected expression was not thrown";
            break;
            
        case ResultWas::ThrewException:
            cause = "an unexpected exception was thrown";
            expected = false;
            break;
            
        case ResultWas::FatalErrorCondition:
            cause = "an unexpected error occured";
            expected = false;
            break;
            
        case ResultWas::Info:
            cause = "info";
            break;
            
        case ResultWas::Warning:
            cause = "warning";
            break;
            
            /* Quiesce the compiler; we shouldn't actually hit these */
        case ResultWas::Unknown:
        case ResultWas::FailureBit:
        case ResultWas::Exception:
            cause = "internal error";
            expected = false;
            break;
    }
    
    /* Formulate the error description */
    if (result.hasExpression())
    {
        description.append(result.getExpression());
        
        if (result.hasExpandedExpression())
        {
            description.append(" (expands to: " + result.getExpandedExpression() + ")");
        }
    }
    description.append(": " + cause);
    
    /* Report the base error if there are no additional messages. */
    if (as.infoMessages.size() == 0)
    {
        auto msg = description;
        if (result.hasMessage())
        {
            msg.append(": " + result.getMessage());
        }
        
        /* Append a terminating period, if necessary */
        if (msg[msg.size() - 1] != '.')
            msg.append(".");
        
        NSString* desc = [NSString stringWithUTF8String:msg.c_str()];
        NSString* file = [NSString stringWithUTF8String:result.getSourceInfo().file];
        
        [run recordFailureWithDescription:desc inFile:file atLine:result.getSourceInfo().line expected:expected];
    }
    
    /* Report a failure for each one of the info message strings. */
    for (auto&& info : as.infoMessages)
    {
        auto msg = description;
        msg.append(": " + info.message);
        
        /* Append a terminating period, if necessary */
        if (msg[msg.size() - 1] != '.')
            msg.append(".");
        
        NSString* desc = [NSString stringWithUTF8String:msg.c_str()];
        NSString* file = [NSString stringWithUTF8String:info.lineInfo.file];
        
        [run recordFailureWithDescription:desc inFile:file atLine:info.lineInfo.line expected:expected];
    }
}

