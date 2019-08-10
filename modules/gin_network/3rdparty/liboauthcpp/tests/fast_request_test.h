#ifndef __LIBOAUTHCPP_FAST_REQUEST_TEST_H__
#define __LIBOAUTHCPP_FAST_REQUEST_TEST_H__

#include "testutil.h"
#include <liboauthcpp/liboauthcpp.h>

using namespace OAuth;

namespace OAuthTest {

/** Tests possible issues with fast, repeated use.
 **/
class FastRequestTest {
public:
    static void run() {
        repeated_nonce_test();
    }

    /** Tries to check that nonces will not be reused. This tries to make sure
     *  we that we don't rely only on timestamps to generate unique.
     */
    static void repeated_nonce_test() {
        std::string consumer_key = "wwwwxxxxyyyyzzzz";
        std::string consumer_secret = "zzzzyyyyxxxxwwww";
        OAuth::Consumer consumer(consumer_key, consumer_secret);

        std::string oauth_token = "aaaabbbbccccdddd";
        std::string oauth_token_secret = "ddddccccbbbbaaaa";
        OAuth::Token token(oauth_token, oauth_token_secret);

        // We can't set the values here or else we'll always use the same
        // values. Instead, we initialize normally and run a large number of
        // identical requests. We save the info from each, then later extract
        // the nonces and verify that we don't have any repeats.
        Client::__resetInitialize();
        Client::initialize();
        OAuth::Client oauth(&consumer, &token);

        std::string resource_arg = "arg=foo";
        std::string resource = "resource?" + resource_arg;

        std::vector<std::string> queries;
        // Just save the output here so we can generate as many as quickly as we
        // can, hopefully increasing the likelihood of duplicates.
        int ntests = 1000;
        for(int i = 0; i < ntests; i++) {
            queries.push_back(oauth.getURLQueryString(OAuth::Http::Head, resource));
        }

        // Then process them all, extracting and counting the nonces.
        std::map<std::string, int> nonces;
        for(int i = 0; i < ntests; i++) {
            OAuth::KeyValuePairs kv = OAuth::ParseKeyValuePairs(queries[i]);
            ASSERT_EQUAL(kv.count("oauth_nonce"), 1, "oauth_nonce should appear exactly once in a generated query string");
            std::string nonce_val = kv.find("oauth_nonce")->second;
            if (nonces.find(nonce_val) == nonces.end())
                nonces[nonce_val] = 1;
            else
                nonces[nonce_val]++;
        }
        for(std::map<std::string, int>::iterator it = nonces.begin(); it != nonces.end(); it++) {
            ASSERT_EQUAL(1, it->second, "Found repeated nonce");
        }
    }
};

}

#endif
