#ifndef __LIBOAUTHCPP_REQUEST_TEST_H__
#define __LIBOAUTHCPP_REQUEST_TEST_H__

#include "testutil.h"
#include <liboauthcpp/liboauthcpp.h>

using namespace OAuth;

namespace OAuthTest {

/** Tests simple requests given you already have an access key/token.
 **/
class RequestTest {
public:
    static void run() {
        std::string consumer_key = "wwwwxxxxyyyyzzzz";
        std::string consumer_secret = "zzzzyyyyxxxxwwww";
        OAuth::Consumer consumer(consumer_key, consumer_secret);

        std::string oauth_token = "aaaabbbbccccdddd";
        std::string oauth_token_secret = "ddddccccbbbbaaaa";
        OAuth::Token token(oauth_token, oauth_token_secret);

        // This sets up the client class to generate reproducible results.
        Client::__resetInitialize();
        Client::initialize(100, 1390268986);
        OAuth::Client oauth(&consumer, &token);

        // Test all request types, simple, unreserved chars in resource name, no parameters
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Head, "resource"),
            "oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=4J4T69RentaaJJ6R5nzlnidLzSY%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate simple HEAD request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Get, "resource"),
            "oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=Wo8bolvRL6CC2MyJEP7zkjtMvvs%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate simple GET request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Post, "resource"),
            "oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=0rv2KvH0O6DWbCrIb5Ra3QCH6lk%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate simple POST request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Delete, "resource"),
            "oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=CcAXXkwNhyEW4Y6QpovPxq56kkc%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate simple DELETE request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Put, "resource"),
            "oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=U%2BbprftQwfylqEU9IR%2Bps5KUnZc%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate simple PUT request signature"
        );


        // Test all request types with simple, unreserved chars in resource
        // name, a few parameters, and make them out of sorted order
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Head, "resource?z=1&a=2&d=baz"),
            "a=2&d=baz&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=AN6kLZkXmqJRKtSgPQipjp8cYaM%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0&z=1",
            "Validate simple HEAD request with unordered parameters signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Get, "resource?z=1&a=2&d=baz"),
            "a=2&d=baz&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=UCWLEts18HC8Uq9GsO1gO4YBops%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0&z=1",
            "Validate simple GET request with unordered parameters signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Post, "resource?z=1&a=2&d=baz"),
            "a=2&d=baz&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=VFd7tS7nnYIgU0g5biRA0%2BWhaRs%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0&z=1",
            "Validate simple POST request with unordered parameters signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Delete, "resource?z=1&a=2&d=baz"),
            "a=2&d=baz&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=PNHvv3P5WF%2BGCg6EOzwz7561tw8%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0&z=1",
            "Validate simple DELETE request with unordered parameters signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Put, "resource?z=1&a=2&d=baz"),
            "a=2&d=baz&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=yqbgvxoqF0eyFqNGUaivRRRld8U%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0&z=1",
            "Validate simple PUT request with unordered parameters signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Post, "resource?z=1&a=2&l=-74%2C40%2C-73%2C41"),
            "a=2&l=-74%2C40%2C-73%2C41&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=QXyCmnX%2FL3OR6wQ9HpZGrTk7ZG0%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0&z=1",
            "Validate simple POST request with sub-delimiter character ','"
        );
    }
};

}

#endif
