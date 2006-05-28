#include "wx/wxprec.h"

#include <string>

// need this to be able to use CPPUNIT_ASSERT_EQUAL with wxString objects
//
// NB: at least for VC7.1 the specialization must be done before cppunit
//     headers inclusion as otherwise it's just ignored!
namespace CppUnit
{

template <typename T> struct assertion_traits;

template <>
struct assertion_traits<wxString>
{
    static bool equal(const wxString& s1, const wxString& s2)
    {
      return s1 == s2;
    }

    static std::string toString(const wxString& s)
    {
      return std::string(s.mb_str());
    }
};

} // namespace CppUnit

#include "wx/cppunit.h"

// define wxHAVE_U_ESCAPE if the compiler supports \uxxxx character constants
#if (defined(__VISUALC__) && (__VISUALC__ >= 1300)) || \
    (defined(__GNUC__) && (__GNUC__ >= 3))
    #define wxHAVE_U_ESCAPE
#endif

#define CPPUNIT_ASSERT_STR_EQUAL(s1, s2)                                      \
    CPPUNIT_ASSERT_EQUAL( wxString(s1), wxString(s2) )

