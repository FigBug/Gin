#ifndef __LIBOAUTHCPP_TESTUTIL_H__
#define __LIBOAUTHCPP_TESTUTIL_H__

#include <iostream>
#include <vector>
#include <sstream>

namespace OAuthTest {

class TestUtil {
private:
    struct Details {
        std::string cond_str;
        std::string cond_report;

        Details(const std::string& cond, const std::string& report)
         : cond_str(cond), cond_report(report)
        {}
    };

public:
    static void assert_true(bool is_true, const std::string& cond_str, const std::string& cond_report) {
        if (!is_true) {
            std::cout << "TEST: " << cond_report << std::endl;
            std::cout << "FAIL\t" << cond_str << std::endl;
            failed++;
            // Save info about failures for summary
            failed_details.push_back(Details(cond_report, cond_str));
        }
        else {
#ifdef LIBOAUTHCPP_DEBUG
            std::cout << "TEST:" << cond_report << std::endl;
            std::cout << "PASS\t" << cond_str << std::endl;
#endif
            passed++;
        }
    }

    /** Summarize the results and return the return code for */
    static int summary() {
        std::cout << std::endl << std::endl;
        std::cout << "Summary:" << std::endl;
        std::cout << "PASSED: " << passed << std::endl;
        std::cout << "FAILED: " << failed << std::endl;
        if (failed_details.size() > 0) {
            std::cout << "\tDetails: " << std::endl;
            for(std::size_t i = 0; i < failed_details.size(); i++)
                std::cout << "\t\t" << failed_details[i].cond_str << ": " << failed_details[i].cond_report << std::endl;
        }

        std::cout << std::endl << std::endl;
        // Returns 0 upon success, positive number (error) if anything fails.
        return failed;
    }

private:
    static int passed;
    static int failed;
    static std::vector<Details> failed_details;
};

#define ASSERT_BASE(cond, cond_str, msg)                                \
    do {                                                                \
        std::stringstream line_sstr;                                    \
        line_sstr << __LINE__;                                          \
        OAuthTest::TestUtil::assert_true(cond, std::string(cond_str) + std::string(" (" __FILE__) + std::string(":") + line_sstr.str() + std::string(")"), msg); \
    } while(0)

#define ASSERT_TRUE(cond, msg)                  \
    ASSERT_BASE(cond, #cond " == true", msg)

#define ASSERT_FALSE(cond, msg)                 \
    ASSERT_BASE(!cond, #cond " == false", msg)

#define ASSERT_EQUAL(a, b, msg)                                         \
    do {                                                                \
        std::stringstream as_eq_sstr;                                   \
        as_eq_sstr << #a " == " #b << "  (" << a << " == " << b << ")";   \
        ASSERT_BASE(a == b, as_eq_sstr.str(), msg);                     \
    } while(0)

#define ASSERT_NOTEQUAL(a, b, msg)                                      \
    do {                                                                \
        std::stringstream as_neq_sstr;                                  \
        as_neq_sstr << #a " != " #b << "(" << a << " != " << b << ")";  \
        ASSERT_BASE(a != b, as_neq_sstr.str(), msg);                    \
    } while(0)

#define ASSERT_IN(key, collection, msg)         \
    ASSERT_BASE(collection.find(key) != collection.end(), #key " in " #collection, msg)

#define ASSERT_THROWS(stmts, ExceptionType, msg)                 \
    do {                                                         \
        try {                                                    \
            stmts;                                               \
            ASSERT_BASE(false, "throws " #ExceptionType, msg);   \
        }                                                        \
        catch (ExceptionType e) {                                \
            ASSERT_BASE(true, "throws " #ExceptionType, msg);    \
        }                                                        \
    } while(0)

} // namespace OAuthTest

#endif
