#ifndef __LIBOAUTHCPP_URLENCODE_TEST_H__
#define __LIBOAUTHCPP_URLENCODE_TEST_H__

#include "testutil.h"
#include <liboauthcpp/liboauthcpp.h>

using namespace OAuth;

namespace OAuthTest {

/** Tests URLEncode function. See http://tools.ietf.org/html/rfc3986 for
 *  details, especially Section 2. This is all based on OAuth 1.0a,
 *  which says[http://oauth.net/core/1.0a/] that all unreserved
 *  (approximately normal alphanumerics) should *not* be encoded, but
 *  *everything else* should be.
 **/
class URLEncodeTest {
public:
    static void run() {
        complete_encode_test();
        http_path_encode_test();
        http_query_key_encode_test();
        http_query_value_encode_test();
    }


    // Tests the API for doing complete encoding. Maintained for
    // backward compatibility, but also exercises the internal API
    // which is needed to encode the base signature string for the
    // OAuth protocol.
    static void complete_encode_test() {
        // Unreserved set *MUST NOT* be encoded
        ASSERT_EQUAL(
            PercentEncode("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            std::string("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            "URLEncoding unreserved characters (normal alphanumerics) should be a nop (complete encoding)"
        );

        // Everything else must be encoded

        // Reserved gen-delims
        ASSERT_EQUAL(PercentEncode(":"), "%3A", "Reserved gen-delim ':' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("/"), "%2F", "Reserved gen-delim '/' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("?"), "%3F", "Reserved gen-delim '?' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("#"), "%23", "Reserved gen-delim '#' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("["), "%5B", "Reserved gen-delim ']' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("]"), "%5D", "Reserved gen-delim '[' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("@"), "%40", "Reserved gen-delim '@' should be percent encoded (complete encoding)");
        // Reserved sub-delims
        ASSERT_EQUAL(PercentEncode("!"), "%21", "Reserved sub-delim '!' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("$"), "%24", "Reserved sub-delim '$' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("%"), "%25", "Reserved sub-delim '%' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("&"), "%26", "Reserved sub-delim '&' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("'"), "%27", "Reserved sub-delim ''' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("("), "%28", "Reserved sub-delim '(' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode(")"), "%29", "Reserved sub-delim ')' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("*"), "%2A", "Reserved sub-delim '*' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("+"), "%2B", "Reserved sub-delim '+' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode(","), "%2C", "Reserved sub-delim ',' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode(";"), "%3B", "Reserved sub-delim ';' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("="), "%3D", "Reserved sub-delim '=' should be percent encoded (complete encoding)");


        // Try to cover a reasonable set of non-unreserved
        // characters to make sure we're encoding what we should. We
        // can add more here as necessary if we find errors.
        ASSERT_EQUAL(PercentEncode(" "), "%20", "Non-unreserved character ' ' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("\""), "%22", "Non-unreserved character '\"' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("<"), "%3C", "Non-unreserved character '<' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode(">"), "%3E", "Non-unreserved character '>' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("\\"), "%5C", "Non-unreserved character '\\' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("^"), "%5E", "Non-unreserved character '^' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("`"), "%60", "Non-unreserved character '`' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("{"), "%7B", "Non-unreserved character '{' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("|"), "%7C", "Non-unreserved character '|' should be percent encoded (complete encoding)");
        ASSERT_EQUAL(PercentEncode("}"), "%7D", "Non-unreserved character '}' should be percent encoded (complete encoding)");
    }


    /** Tests encoding of the path of an HTTP URI, allowing some
     * reserved subdelimiters to be used unencoded.
     */
    static void http_path_encode_test() {
        // Unreserved set *MUST NOT* be encoded
        ASSERT_EQUAL(
            HttpEncodePath("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            std::string("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            "URLEncoding unreserved characters (normal alphanumerics) should be a nop (http path encoding)"
        );

        // Reserved gen-delims
        ASSERT_EQUAL(HttpEncodePath(":"), "%3A", "Reserved gen-delim ':' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("/"), "%2F", "Reserved gen-delim '/' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("?"), "%3F", "Reserved gen-delim '?' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("#"), "%23", "Reserved gen-delim '#' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("["), "%5B", "Reserved gen-delim ']' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("]"), "%5D", "Reserved gen-delim '[' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("@"), "%40", "Reserved gen-delim '@' should be percent encoded (http path encoding)");
        // Reserved sub-delims
        ASSERT_EQUAL(HttpEncodePath("!"), "!", "Reserved sub-delim '!' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("$"), "$", "Reserved sub-delim '$' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("%"), "%25", "Reserved sub-delim '%' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("&"), "&", "Reserved sub-delim '&' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("'"), "'", "Reserved sub-delim ''' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("("), "(", "Reserved sub-delim '(' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath(")"), ")", "Reserved sub-delim ')' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("*"), "*", "Reserved sub-delim '*' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("+"), "+", "Reserved sub-delim '+' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath(","), ",", "Reserved sub-delim ',' should be a nop (http path encoding)");
        //  - ';' and '=' can be used for "matrix" parameters
        ASSERT_EQUAL(HttpEncodePath(";"), ";", "Reserved sub-delim ';' should be a nop (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("="), "=", "Reserved sub-delim '=' should be a nop (http path encoding)");


        // Try to cover a reasonable set of non-unreserved
        // characters to make sure we're encoding what we should. We
        // can add more here as necessary if we find errors.
        ASSERT_EQUAL(HttpEncodePath(" "), "%20", "Non-unreserved character ' ' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("\""), "%22", "Non-unreserved character '\"' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("<"), "%3C", "Non-unreserved character '<' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath(">"), "%3E", "Non-unreserved character '>' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("\\"), "%5C", "Non-unreserved character '\\' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("^"), "%5E", "Non-unreserved character '^' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("`"), "%60", "Non-unreserved character '`' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("{"), "%7B", "Non-unreserved character '{' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("|"), "%7C", "Non-unreserved character '|' should be percent encoded (http path encoding)");
        ASSERT_EQUAL(HttpEncodePath("}"), "%7D", "Non-unreserved character '}' should be percent encoded (http path encoding)");
    }



    /** Tests encoding of a query string key component of an HTTP URI,
     *  allowing a different set of reserved subdelimiters to be used
     *  unencoded as compared to the path portion of the URI.
     */
    static void http_query_key_encode_test() {
        // Unreserved set *MUST NOT* be encoded
        ASSERT_EQUAL(
            HttpEncodeQueryKey("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            std::string("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            "URLEncoding unreserved characters (normal alphanumerics) should be a nop (http query string encoding)"
        );

        // Reserved gen-delims
        ASSERT_EQUAL(HttpEncodeQueryKey(":"), "%3A", "Reserved gen-delim ':' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("/"), "%2F", "Reserved gen-delim '/' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("?"), "%3F", "Reserved gen-delim '?' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("#"), "%23", "Reserved gen-delim '#' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("["), "%5B", "Reserved gen-delim ']' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("]"), "%5D", "Reserved gen-delim '[' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("@"), "%40", "Reserved gen-delim '@' should be percent encoded (http query string encoding)");
        // Reserved sub-delims
        ASSERT_EQUAL(HttpEncodeQueryKey("!"), "%21", "Reserved sub-delim '!' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("$"), "%24", "Reserved sub-delim '$' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("%"), "%25", "Reserved sub-delim '%' should be percent encoded (http query string encoding)");
        //  - '&' is the query arg separator
        ASSERT_EQUAL(HttpEncodeQueryKey("&"), "%26", "Reserved sub-delim '&' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("'"), "%27", "Reserved sub-delim ''' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("("), "%28", "Reserved sub-delim '(' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey(")"), "%29", "Reserved sub-delim ')' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("*"), "%2A", "Reserved sub-delim '*' should be percent encoded (http query string encoding)");
        //  - '+' has to be encoded in query strings because spaces historically
        //    were convereted to '+'.
        ASSERT_EQUAL(HttpEncodeQueryKey("+"), "%2B", "Reserved sub-delim '+' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey(","), "%2C", "Reserved sub-delim ',' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey(";"), "%3B", "Reserved sub-delim ';' should be percent encoded (http query string encoding)");
        //  - '=' is the key=value separator
        ASSERT_EQUAL(HttpEncodeQueryKey("="), "%3D", "Reserved sub-delim '=' should be percent encoded (http query string encoding)");


        // Try to cover a reasonable set of non-unreserved
        // characters to make sure we're encoding what we should. We
        // can add more here as necessary if we find errors.
        ASSERT_EQUAL(HttpEncodeQueryKey(" "), "%20", "Non-unreserved character ' ' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("\""), "%22", "Non-unreserved character '\"' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("<"), "%3C", "Non-unreserved character '<' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey(">"), "%3E", "Non-unreserved character '>' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("\\"), "%5C", "Non-unreserved character '\\' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("^"), "%5E", "Non-unreserved character '^' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("`"), "%60", "Non-unreserved character '`' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("{"), "%7B", "Non-unreserved character '{' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("|"), "%7C", "Non-unreserved character '|' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryKey("}"), "%7D", "Non-unreserved character '}' should be percent encoded (http query string encoding)");
    }


    /** Tests encoding of a query string key component of an HTTP URI,
     *  allowing a different set of reserved subdelimiters to be used
     *  unencoded as compared to the path portion of the URI.
     */
    static void http_query_value_encode_test() {
        // Unreserved set *MUST NOT* be encoded
        ASSERT_EQUAL(
            HttpEncodeQueryValue("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            std::string("abcdefghijklmnopqrstuvwxyz0123456789-._~"),
            "URLEncoding unreserved characters (normal alphanumerics) should be a nop (http query string encoding)"
        );

        // Reserved gen-delims
        ASSERT_EQUAL(HttpEncodeQueryValue(":"), "%3A", "Reserved gen-delim ':' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("/"), "%2F", "Reserved gen-delim '/' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("?"), "%3F", "Reserved gen-delim '?' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("#"), "%23", "Reserved gen-delim '#' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("["), "%5B", "Reserved gen-delim ']' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("]"), "%5D", "Reserved gen-delim '[' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("@"), "%40", "Reserved gen-delim '@' should be percent encoded (http query string encoding)");
        // Reserved sub-delims
        ASSERT_EQUAL(HttpEncodeQueryValue("!"), "%21", "Reserved sub-delim '!' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("$"), "%24", "Reserved sub-delim '$' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("%"), "%25", "Reserved sub-delim '%' should be percent encoded (http query string encoding)");
        //  - '&' is the query arg separator
        ASSERT_EQUAL(HttpEncodeQueryValue("&"), "%26", "Reserved sub-delim '&' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("'"), "%27", "Reserved sub-delim ''' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("("), "%28", "Reserved sub-delim '(' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue(")"), "%29", "Reserved sub-delim ')' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("*"), "%2A", "Reserved sub-delim '*' should be percent encoded (http query string encoding)");
        //  - '+' has to be encoded in query strings because spaces historically
        //    were convereted to '+'.
        ASSERT_EQUAL(HttpEncodeQueryValue("+"), "%2B", "Reserved sub-delim '+' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue(","), "%2C", "Reserved sub-delim ',' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue(";"), "%3B", "Reserved sub-delim ';' should be percent encoded (http query string encoding)");
        //  - '=' is the key=value separator, but is safe in the values
        ASSERT_EQUAL(HttpEncodeQueryValue("="), "%3D", "Reserved sub-delim '=' should be percent encoded (http query string encoding)");


        // Try to cover a reasonable set of non-unreserved
        // characters to make sure we're encoding what we should. We
        // can add more here as necessary if we find errors.
        ASSERT_EQUAL(HttpEncodeQueryValue(" "), "%20", "Non-unreserved character ' ' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("\""), "%22", "Non-unreserved character '\"' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("<"), "%3C", "Non-unreserved character '<' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue(">"), "%3E", "Non-unreserved character '>' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("\\"), "%5C", "Non-unreserved character '\\' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("^"), "%5E", "Non-unreserved character '^' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("`"), "%60", "Non-unreserved character '`' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("{"), "%7B", "Non-unreserved character '{' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("|"), "%7C", "Non-unreserved character '|' should be percent encoded (http query string encoding)");
        ASSERT_EQUAL(HttpEncodeQueryValue("}"), "%7D", "Non-unreserved character '}' should be percent encoded (http query string encoding)");
    }
};

}

#endif
