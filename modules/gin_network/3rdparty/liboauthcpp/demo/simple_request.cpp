#include <iostream>
#include <string>
#include <liboauthcpp/liboauthcpp.h>

/* These are input settings that make this demo actually work -- you need to get
 * these, e.g. by referring to the Twitter documentation and by registering an
 * application with them. Here we have examples from Twitter. If you
 * don't enter any, you'll be prompted to enter them at runtime.
 */
std::string consumer_key = ""; // Key from Twitter
std::string consumer_secret = ""; // Secret from Twitter
std::string oauth_token = ""; // User-specific token acquired by auth process
std::string oauth_token_secret = ""; // User-specific secret acquired by auth process.
// An example resource to be accessed, along with parameters for the request
std::string oauth_protected_resource = "https://api.twitter.com/1.1/statuses/home_timeline.json";
std::string oauth_protected_resource_params = "count=5";

std::string getUserString(std::string prompt) {
    std::cout << prompt << " ";

    std::string res;
    std::cin >> res;
    std::cout << std::endl;
    return res;
}

int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == std::string("--debug"))
        OAuth::SetLogLevel(OAuth::LogLevelDebug);

    // Initialization
    if (consumer_key.empty()) consumer_key = getUserString("Enter consumer key:");
    if (consumer_secret.empty()) consumer_secret = getUserString("Enter consumer secret:");
    // We assume you have gotten the access token. You may have e.g., used
    // simple_auth to get it.
    if (oauth_token.empty()) oauth_token = getUserString("Enter access token:");
    if (oauth_token_secret.empty()) oauth_token_secret = getUserString("Enter access token secret:");
    OAuth::Consumer consumer(consumer_key, consumer_secret);
    OAuth::Token token(oauth_token, oauth_token_secret);
    OAuth::Client oauth(&consumer, &token);

    // Get the query string. Note that we pass in the URL as if we were
    // accessing the resource, but *the output query string includes the
    // parameters you passed in*. Below, we append the result only to the base
    // URL, not the entire URL we passed in here.
    std::string oAuthQueryString =
        oauth.getURLQueryString(OAuth::Http::Get, oauth_protected_resource + "?" + oauth_protected_resource_params);

    std::cout << "Enter the following in your browser to access the resource: " << std::endl;
    std::cout << oauth_protected_resource << "?" << oAuthQueryString << std::endl;
    std::cout << std::endl;

    return 0;
}
