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
std::string request_token_url = "https://api.twitter.com/oauth/request_token";
std::string request_token_query_args = "oauth_callback=oob";
std::string authorize_url = "https://api.twitter.com/oauth/authorize";
std::string access_token_url = "https://api.twitter.com/oauth/access_token";


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
    OAuth::Consumer consumer(consumer_key, consumer_secret);
    OAuth::Client oauth(&consumer);

    // Step 1: Get a request token. This is a temporary token that is used for
    // having the user authorize an access token and to sign the request to
    // obtain said access token.
    std::string base_request_token_url = request_token_url + (request_token_query_args.empty() ? std::string("") : (std::string("?")+request_token_query_args) );
    std::string oAuthQueryString =
        oauth.getURLQueryString( OAuth::Http::Get, base_request_token_url);

    std::cout << "Enter the following in your browser to get the request token: " << std::endl;
    // Note that getting the query string includes the arguments we
    // passed in, so we don't need to include request_token_query_args
    // again.
    std::cout << request_token_url << "?" << oAuthQueryString << std::endl;
    std::cout << std::endl;

    // Extract the token and token_secret from the response
    std::string request_token_resp = getUserString("Enter the response:");
    // This time we pass the response directly and have the library do the
    // parsing (see next extractToken call for alternative)
    OAuth::Token request_token = OAuth::Token::extract( request_token_resp );

    // Get access token and secret from OAuth object
    std::cout << "Request Token:" << std::endl;
    std::cout << "    - oauth_token        = " << request_token.key() << std::endl;
    std::cout << "    - oauth_token_secret = " << request_token.secret() << std::endl;
    std::cout << std::endl;

    // Step 2: Redirect to the provider. Since this is a CLI script we
    // do not redirect. In a web application you would redirect the
    // user to the URL below.
    std::cout << "Go to the following link in your browser to authorize this application on a user's account:" << std::endl;
    std::cout << authorize_url << "?oauth_token=" << request_token.key() << std::endl;

    // After the user has granted access to you, the consumer, the
    // provider will redirect you to whatever URL you have told them
    // to redirect to. You can usually define this in the
    // oauth_callback argument as well.
    std::string pin = getUserString("What is the PIN?");
    request_token.setPin(pin);

    // Step 3: Once the consumer has redirected the user back to the
    // oauth_callback URL you can request the access token the user
    // has approved. You use the request token to sign this
    // request. After this is done you throw away the request token
    // and use the access token returned. You should store the oauth
    // token and token secret somewhere safe, like a database, for
    // future use.
    oauth = OAuth::Client(&consumer, &request_token);
    // Note that we explicitly specify an empty body here (it's a GET) so we can
    // also specify to include the oauth_verifier parameter
    oAuthQueryString = oauth.getURLQueryString( OAuth::Http::Get, access_token_url, std::string( "" ), true );
    std::cout << "Enter the following in your browser to get the final access token & secret: " << std::endl;
    std::cout << access_token_url << "?" << oAuthQueryString;
    std::cout << std::endl;

    // Once they've come back from the browser, extract the token and token_secret from the response
    std::string access_token_resp = getUserString("Enter the response:");
    // On this extractToken, we do the parsing ourselves (via the library) so we
    // can extract additional keys that are sent back, in the case of twitter,
    // the screen_name
    OAuth::KeyValuePairs access_token_resp_data = OAuth::ParseKeyValuePairs(access_token_resp);
    OAuth::Token access_token = OAuth::Token::extract( access_token_resp_data );

    std::cout << "Access token:" << std::endl;
    std::cout << "    - oauth_token        = " << access_token.key() << std::endl;
    std::cout << "    - oauth_token_secret = " << access_token.secret() << std::endl;
    std::cout << std::endl;
    std::cout << "You may now access protected resources using the access tokens above." << std::endl;
    std::cout << std::endl;

    std::pair<OAuth::KeyValuePairs::iterator, OAuth::KeyValuePairs::iterator> screen_name_its = access_token_resp_data.equal_range("screen_name");
    for(OAuth::KeyValuePairs::iterator it = screen_name_its.first; it != screen_name_its.second; it++)
        std::cout << "Also extracted screen name from access token response: " << it->second << std::endl;

    // E.g., to use the access token, you'd create a new OAuth using
    // it, discarding the request_token:
    // oauth = OAuth::Client(&consumer, &access_token);

    return 0;
}
