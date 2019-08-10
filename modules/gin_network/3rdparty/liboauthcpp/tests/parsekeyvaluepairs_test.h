#ifndef __LIBOAUTHCPP_PARSEKEYVALUEPAIRS_TEST_H__
#define __LIBOAUTHCPP_PARSEKEYVALUEPAIRS_TEST_H__

#include "testutil.h"
#include <liboauthcpp/liboauthcpp.h>

using namespace OAuth;

namespace OAuthTest {

/** Tests parsing of KeyValuePairs into *encoded* parts.
 **/
class ParseKeyValuePairsTest {
public:
    static void run() {
        test_valid();
        test_invalid();
    }

    static void test_valid() {
        // Empty
        KeyValuePairs pairs = ParseKeyValuePairs("");
        ASSERT_EQUAL(pairs.size(), 0, "Empty string should result in empty key-value pairs");

        // 1 parameter
        pairs = ParseKeyValuePairs("foo=bar");
        ASSERT_EQUAL(pairs.size(), 1, "Single parameter key-value pair should result in one entry");
        ASSERT_IN("foo", pairs, "foo should be in dictionary");
        ASSERT_EQUAL(pairs.find("foo")->second, "bar", "foo should equal bar");

        // 2 parameters
        pairs = ParseKeyValuePairs("foo=bar&baz=bam");
        ASSERT_EQUAL(pairs.size(), 2, "Two parameters key-value pair should result in two entries");
        ASSERT_IN("foo", pairs, "foo should be in dictionary");
        ASSERT_EQUAL(pairs.find("foo")->second, "bar", "foo should equal bar");
        ASSERT_IN("baz", pairs, "baz should be in dictionary");
        ASSERT_EQUAL(pairs.find("baz")->second, "bam", "baz should equal bam");

        // 3 parameters, 1 repeated twice
        pairs = ParseKeyValuePairs("foo=bar&baz=bam&foo=tar");
        ASSERT_EQUAL(pairs.size(), 3, "Three parameters key-value pair should result in three entries");
        ASSERT_IN("foo", pairs, "foo should be in dictionary");
        ASSERT_EQUAL(pairs.find("foo")->second, "bar", "foo should equal bar");
        ASSERT_EQUAL((++pairs.find("foo"))->first, "foo", "foo should occur twice");
        ASSERT_EQUAL((++pairs.find("foo"))->second, "tar", "second foo should equal tar");
        ASSERT_IN("baz", pairs, "baz should be in dictionary");
        ASSERT_EQUAL(pairs.find("baz")->second, "bam", "baz should equal bam");

        // Single parameter, special chars
        pairs = ParseKeyValuePairs("foo=bar%20%3D");
        ASSERT_EQUAL(pairs.size(), 1, "Single parameter key-value pair should result in one entry");
        ASSERT_IN("foo", pairs, "foo should be in dictionary");
        ASSERT_EQUAL(pairs.find("foo")->second, "bar%20%3D", "foo should equal bar%20%3D");
    }

    static void test_invalid() {
        ASSERT_THROWS(ParseKeyValuePairs("foo=bar&"), ParseError, "Ampersand followed by nothing should cause parse error");
        ASSERT_THROWS(ParseKeyValuePairs("foo=bar&baz"), ParseError, "Ampersand followed by only key should cause parse error");
    }
};

}

#endif
