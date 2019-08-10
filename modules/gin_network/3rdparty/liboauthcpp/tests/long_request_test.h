#ifndef __LIBOAUTHCPP_LONG_REQUEST_TEST_H__
#define __LIBOAUTHCPP_LONG_REQUEST_TEST_H__

#include "testutil.h"
#include <liboauthcpp/liboauthcpp.h>

using namespace OAuth;

namespace OAuthTest {

/** Tests long requests -- longer than a normal request such that they can cause
 * problems if buffers are not big enough, etc.
 **/
class LongRequestTest {
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

        // Generate a very long resource by adding very long parameters
        std::string resource_arg = "arg=" +
            std::string(16384, 'x') // 16K long
            ;
        std::string resource = "resource?" + resource_arg;

        // Test all request types, simple, unreserved chars in resource name, no parameters
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Head, resource),
            resource_arg + "&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=YnNugcEr0E4TDgkzR4ZFMFoHEgU%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate long HEAD request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Get, resource),
            resource_arg + "&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=5weTspQ0eMH5dFMDdsrGZlNrfPk%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate long GET request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Post, resource),
            resource_arg + "&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=thJwo%2ByzdRtxwrBqDXRCo2a1mcY%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate long POST request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Delete, resource),
            resource_arg + "&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=ONjZansHtzGD57pZ9S65s0a6aXs%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate long DELETE request signature"
        );
        ASSERT_EQUAL(
            oauth.getURLQueryString(OAuth::Http::Put, resource),
            resource_arg + "&oauth_consumer_key=wwwwxxxxyyyyzzzz&oauth_nonce=139026898664&oauth_signature=6FFgNsTsCl8ABh9i93rRN1m3csE%3D&oauth_signature_method=HMAC-SHA1&oauth_timestamp=1390268986&oauth_token=aaaabbbbccccdddd&oauth_version=1.0",
            "Validate long PUT request signature"
        );
    }
};

}

#endif
