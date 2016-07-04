//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "src/TwitterClient/TwitterClient.h"
#include "externals/twitCurl/include/rapidjson/document.h"
#include "externals/twitCurl/include/rapidjson/writer.h"

/**
* Constructor for the Twitter Class
* @param[in] pAccount
* @param[in] name is Username
* @param[in] passwort is Userpassword
* @param[in] swap
*/
Twitter::Twitter(twitCurl* pAccount, std::string name, std::string passwort, bool swap) {

    mpAccount = pAccount;
    mpAccount->setTwitterUsername(name);
    mpAccount->setTwitterPassword(passwort);
    if (swap)
    {
        mpAccount->getOAuth().setConsumerKey(std::string("yofvdqAD5H9RUXuZyUnq4idgQ"));
        mpAccount->getOAuth().setConsumerSecret(std::string("RvUvOVfgZOlkSETOO0XxImKDAdmr8fAweuimx6F8wno3wxCEvD"));
    }
    else
    {
        mpAccount->getOAuth().setConsumerKey(std::string("Uv9fpnENhHvX74UedOiHpmiTA"));
        mpAccount->getOAuth().setConsumerSecret(std::string("CEwKqIqPlpkC7yytbkLlMjGAXe7uAYKWSaTsqpG2Kqtabu2Vdz"));
    }
    std::string authUrl;
    mpAccount->oAuthRequestToken(authUrl);
    std::cout << "authUrl is: " << authUrl;
    mpAccount->oAuthHandlePIN(authUrl);
    mpAccount->oAuthAccessToken();
}

//JSON ----------------------------------------------------------------------------------------------------------

/**
* A method to parse a string to a jsonObject
* @param[in] string to parse to a JSONObject
* @param[out] rapidjson::Document
*/
rapidjson::Document Twitter::toJSON(std::string string) {
    rapidjson::Document jObj;
    jObj.Parse(string.c_str());

    if (jObj.HasParseError()) {
        // Check for no internet connection:
        std::size_t found = string.find("Host unreachable");
        if (found != std::string::npos) {
            std::cout << "No internet connection (or Twitter is offline)!" << std::endl;
            return NULL;
        }

        int err = jObj.GetParseError();
        std::cout << "This string caused error while parsing it:" << std::endl;
        std::cout << string << std::endl;
        std::cout << "Error parsing string to JSON (Error: " << err << ")" << std::endl;
        return NULL;
    }

    if (jObj.IsObject()) {
        if (jObj.HasMember("errors")) {
            std::cout << "Error(s) in API call:" << std::endl;
            for (rapidjson::SizeType i = 0; i < jObj["errors"].Size(); i++) {
                std::cout << jObj["errors"][i]["message"].GetString() << " (Code: " << jObj["errors"][i]["code"].GetInt() << ")" << std::endl;
            }
            return NULL;
        }
    };

    return jObj;
}

// STATUS ----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/post/statuses/update
*Updates the authenticating user’s current status, also known as Tweeting.
*
* @param[in] text - The text of your status update, typically up to 140 characters.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::statusUpdate(std::string text) {
    std::string replyMsg = "";
    if (mpAccount->statusUpdate(text)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    else {
        mpAccount->getLastCurlError(replyMsg);
        replyMsg = "Error: " + replyMsg;
    }
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*A method to reply to an other tweet
*
* @param[in] text - The text of your status update, typically up to 140 characters, id - the id of the statusUpdate you want to reply to
* @param[out] rapidjson::Document
*/
rapidjson::Document Twitter::reply(std::string text, std::string id) {
    mpAccount->statusUpdate(text, id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/statuses/retweet/%3Aid
*Retweets a tweet. Returns the original tweet with retweet details embedded.
*
* @param[in] text - The text of your status update, typically up to 140 characters.
* @param[in] id - The numerical ID of the desired status.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::retweet(std::string userId) {
    std::string replyMsg = "";
    if (mpAccount->retweetById(userId)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/get/statuses/show/%3Aid
*Returns a single Tweet, specified by the id parameter.
*The Tweet’s author will also be embedded within the tweet.
*
* @param[in] id - The numerical ID of the desired Tweet.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::statusShow(std::string id) {
    mpAccount->statusShowById(id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/statuses/destroy/%3Aid
*Destroys the status specified by the required ID parameter.
*The authenticating user must be the author of the specified status. Returns the destroyed status if successful.
*
* @param[in] id - The numerical ID of the desired Tweet.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::statusDestroy(std::string id) {
    mpAccount->statusDestroyById(id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

// SEARCH ----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/get/search/tweets
*Returns a collection of relevant Tweets matching a specified query.
*
* @param[in] text - A search query of 500 characters maximum, including operators. Queries may additionally be limited by complexity.
* @param[in] limits - The number of tweets to return per page
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::search(std::string text, std::string limits) {
    mpAccount->search(text, limits);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

// USERSEARCH ----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/get/search/tweets
*Returns a collection of User matching a specified query.
*
* @param[in] text - A search query of 500 characters maximum, including operators. Queries may additionally be limited by complexity.
* @param[in] limits - The number of tweets to return per page
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::userSearch(std::string text, std::string limits) {
    mpAccount->userSearch(text, limits);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

// TIMELINE ----------------------------------------------------------------------------------------------------------

/**
* https://dev.twitter.com/rest/reference/get/statuses/home_timeline
* Returns a collection of the most recent Tweets and retweets posted by the authenticating user and the users they follow.
* The home timeline is central to how most users interact with the Twitter service.
*
* @param[in] sinceId - String specifying since id parameter
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getTimelineHome(std::string sinceId) {
    std::string replyMsg = "";
    if (mpAccount->timelineHomeGet(sinceId)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    else {
        mpAccount->getLastCurlError(replyMsg);
        replyMsg = "Error: " + replyMsg;
    }

    return toJSON(replyMsg);
}

/**
* https://dev.twitter.com/rest/reference/get/statuses/user_timeline
* Returns a collection of the most recent Tweets posted by the user indicated by the screen_name or user_id parameters.
*
* @param[in] trimUser - Trim user name if true
* @param[in] includeRetweets - Inlcude retweets if true
* @param[in] tweetCount - Number of tweets to get. Max 200.
* @param[in] userInfo - screen name or user id in string format,
* @param[in] isUserId - true if userInfo contains an id
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getTimelineUser(bool trimUser, bool includeRetweets, unsigned int tweetCount,
                                        std::string userInfo, bool isUserId) {
    std::string replyMsg = "";
    if (mpAccount->timelineUserGet(trimUser, includeRetweets, tweetCount, userInfo, isUserId)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    else {
        mpAccount->getLastCurlError(replyMsg);
        replyMsg = "Error: " + replyMsg;
    }

    return toJSON(replyMsg);
}

/**
* https://dev.twitter.com/rest/reference/get/statuses/mentions_timeline
* Returns the 20 most recent mentions (tweets containing a users’s @screen_name) for the authenticating user.
*
* @param[in] sinceId - String specifying since id parameter
* @param[out] rapidjson::Documen
**/
rapidjson::Document Twitter::getMentions(std::string sinceId) {
    std::string replyMsg = "";
    if (mpAccount->mentionsGet(sinceId)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    else {
        mpAccount->getLastCurlError(replyMsg);
        replyMsg = "Error: " + replyMsg;
    }

    return toJSON(replyMsg);
}

// MESSAGES----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/get/direct_messages
* Returns the 20 most recent direct messages sent to the authenticating user. Includes detailed information about the sender and recipient user.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getDirectMessages() {
    mpAccount->directMessageGet();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/get/direct_messages/sent
*Returns the 20 most recent direct messages sent by the authenticating user.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getDirectMessagesSent(){
    mpAccount->directMessageGetSent();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/direct_messages/new
*Sends a new direct message to the specified user from the authenticating user.
*
* @param[in] text - The text of your direct message
* @param[in] userId  - optionalThe ID of the user who should receive the direct message or screen_name - The screen name of the user who should receive the direct message
* @param[in] isUserId - Flag if userId argument is ID or screen name of receiver
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::sendDirectMessage(std::string text, std::string userId, bool isUserId) {
    mpAccount->directMessageSend(userId, text, isUserId);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/direct_messages/destroy
*Destroys the direct message specified in the required ID parameter.
*The authenticating user must be the recipient of the specified direct message.
*
* @param[in] id  - The ID of the direct message to delete.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::destroyDirectMessage(std::string id) {
    mpAccount->directMessageDestroyById(id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

// USERS ----------------------------------------------------------------------------------------------------------
/**
*https://dev.twitter.com/rest/reference/get/users/show
*Returns a variety of information about the user specified by the required user_id or screen_name parameter.
*The author’s most recent Tweet will be returned inline when possible.
*
* @param[in] userId or screen_name
* @param[in] isUserId - Flag if userId argument is ID or screen name of receiver
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::showUser(std::string userId, bool isUserId) {
    std::string replyMsg = "";
    if (mpAccount->userGet(userId, isUserId)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    return toJSON(replyMsg);
}

/**
* https://dev.twitter.com/rest/reference/get/users/suggestions
* Access to Twitter’s suggested user list. This returns the list of suggested user categories.
* The category can be used in Twitter::suggestionsGet() to get the users in that category.
*
* @param[in] lang - Restricts the suggested categories to the requested language (should also be set in Twitter::suggestionsGet) (https://dev.twitter.com/rest/reference/get/help/languages)
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::suggestionsGetSlugs(std::string lang) {
    std::string replyMsg = "";
    if (mpAccount->suggestionsGetSlugs(lang)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    return toJSON(replyMsg);
}

/**
* https://dev.twitter.com/rest/reference/get/users/suggestions/%3Aslug
* Access the users in a given category of the Twitter suggested user list.
*
* @param[in] slug - The short name of list or a category (you can get this with Twitter::suggestionsGetSlugs())
* @param[in] lang - Restrict results to this language
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::suggestionsGet(std::string slug, std::string lang) {
    std::string replyMsg = "";
    if (mpAccount->suggestionsGet(slug, lang)) {
        mpAccount->getLastWebResponse(replyMsg);
    }
    return toJSON(replyMsg);
}

// FRIENDSHIP ----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/post/friendships/create
*Allows the authenticating users to follow the user specified in the ID parameter.
*
* @param[in] userId - The ID of the user for whom to befriend or screen_name- The screen name of the user for whom to befriend.
* @param[in] isUserId - Flag if userId argument is ID or screen name of receiver
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::createFriendship(std::string userId, bool isUserId) {
    mpAccount->friendshipCreate(userId, isUserId);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);

    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/friendships/destroy
*Allows the authenticating user to unfollow the user specified in the ID parameter.
*
* @param[in] userId - The ID of the user for whom to unfollow or screen_name- The screen name of the user for whom to unfollow.
* @param[in] isUserId - Flag if userId argument is ID or screen name of receiver
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::destroyFriendship(std::string userId, bool isUserId) {
    mpAccount->friendshipDestroy(userId, isUserId);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);

    return toJSON(replyMsg);
}
/**
*https://dev.twitter.com/rest/reference/get/friendships/show
*Returns detailed information about the relationship between two arbitrary users.
*
* @param[in] userId - The user_id of the target user or screen_name - The screen_name of the target user.
* @param[in] isUserId - Flag if userId argument is ID or screen name of receiver
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::showFriendship(std::string userId, bool isUserId) {
    mpAccount->friendshipShow(userId, isUserId);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);

    return toJSON(replyMsg);
}

// BLOCK ----------------------------------------------------------------------------------------------------------------

/**
* https://dev.twitter.com/rest/reference/post/blocks/create
* Blocks the specified user from following the authenticating user.
* In addition the blocked user will not show in the authenticating users mentions or timeline (unless retweeted by another user).
* If a follow or friend relationship exists it is destroyed.
*
* @param[in] userInfo - The ID of the potentially blocked user. Helpful for disambiguating when a valid user ID is also a valid screen name.
* @param[in] isUserId - Flag if userInfo argument is ID or screen name of receiver
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::blockCreate(std::string userInfo, bool isUserId) {
    //mpAccount->blockCreate()
    mpAccount->blockCreate(userInfo, isUserId);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);

    return toJSON(replyMsg);
}

/**
* https://dev.twitter.com/rest/reference/post/blocks/destroy
* Un-blocks the user specified in the ID parameter for the authenticating user.
* Returns the un-blocked user in the requested format when successful.
* If relationships existed before the block was instated, they will not be restored.
*
* @param[in] userInfo - The ID of the potentially blocked user. Helpful for disambiguating when a valid user ID is also a valid screen name.
* @param[in] isUserId - Flag if userInfo argument is ID or screen name of receiver
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::blockDestroy(std::string userInfo, bool isUserId) {
    mpAccount->blockDestroy(userInfo, isUserId);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);

    return toJSON(replyMsg);
}

/**
* https://dev.twitter.com/rest/reference/get/blocks/list
* Returns a collection of user objects that the authenticating user is blocking.
*
* Important: This method is cursored, meaning your app must make multiple requests in order to receive all blocks correctly.
* See Using cursors to navigate collections for more details on how cursoring works.
*
* @param[in] nextCursor - next cursor string returned from a previous call to this API, otherwise an empty string
* @param[in] includeEntities - indicates whether or not to include 'entities' node
* @param[in] skipStatus - indicates whether or not to include status for returned users
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::blockListGet(std::string nextCursor, bool includeEntities, bool skipStatus) {
    mpAccount->blockListGet(nextCursor, includeEntities, skipStatus);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);

    return toJSON(replyMsg);
}

/**
* https://dev.twitter.com/rest/reference/get/blocks/ids
* Returns an array of numeric user ids the authenticating user is blocking.
*
* Important: This method is cursored, meaning your app must make multiple requests in order to receive all blocks correctly.
* See Using cursors to navigate collections for more details on how cursoring works.
*
* @param[in] nextCursor - next cursor string returned from a previous call to this API, otherwise an empty string
* @param[in] stringifyIds - indicates whether or not returned ids should be in string format
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::blockIdsGet(std::string nextCursor, bool stringifiyIds) {
    mpAccount->blockIdsGet(nextCursor, stringifiyIds);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);

    return toJSON(replyMsg);
}


// SOCIAL GRAPH ----------------------------------------------------------------------------------------------------------

// ACCOUNT METHODS
/*
* Returns the current account rate limit
* @param[out] rapidjson::Document
*/
rapidjson::Document Twitter::getAccountLimit() {
    std::string replyMsg = "";
    mpAccount->accountRateLimitGet();
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/*
* Returns if the current limit is reached
* @param[out] boolean
*/
    bool Twitter::isLimit() {
    rapidjson::Document jObj;
    std::string replyMsg = "";
    mpAccount->accountRateLimitGet();
    mpAccount->getLastWebResponse(replyMsg);
    jObj = toJSON(replyMsg);
    bool limitReached = false;
    const rapidjson::Value& resources = jObj["resources"];
    for (rapidjson::Value::ConstMemberIterator it1 = resources.MemberBegin(); it1 != resources.MemberEnd(); it1++) {
        for (rapidjson::Value::ConstMemberIterator it2 = it1->value.MemberBegin(); it2 != it1->value.MemberEnd(); it2++) {
            if (it2->value["remaining"].GetInt() == 0) {
                limitReached = true;
            }
        }
    }
    return limitReached;
}

// FAVORITES ----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/post/favorites/create
*Note: the like action was known as favorite before November 3, 2015; the historical naming remains in API methods and object properties.
*Likes the status specified in the ID parameter as the authenticating user. Returns the liked status when successful.
*
* @param[in] id - The numerical ID of the desired status, i.e. 123
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::createFavorites(std::string id) {
    mpAccount->favoriteCreate(id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/favorites/destroy
*Un-likes the status specified in the ID parameter as the authenticating user. Returns the un-liked status in the requested format when successful.
*
* @param[in] id - The numerical ID of the desired status, i.e. 123
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::destroyFavorites(std::string id) {
    mpAccount->favoriteDestroy(id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/get/favorites/list
*Returns the 20 most recent Tweets liked by the authenticating user.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getListOfFavorites() {
    mpAccount->favoriteGet();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

// TRENDS ----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/get/trends/available
*Returns the locations that Twitter has trending topic information for.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getTrendsAvailable() {
    mpAccount->trendsAvailableGet();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*Returns current trends
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getCurrentTrends() {
    mpAccount->trendsGet();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*Returns daily trends
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getDailyTrends() {
    mpAccount->trendsDailyGet();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*Returns weeklyTrends
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getWeeklyTrends() {
    mpAccount->trendsWeeklyGet();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

// SAVED SEARCH ----------------------------------------------------------------------------------------------------------

/**
*https://dev.twitter.com/rest/reference/get/saved_searches/list
*Returns the authenticated user’s saved search queries.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::getListOfSavedSearches() {
    mpAccount->savedSearchGet();
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/get/saved_searches/show/%3Aid
*Retrieve the information for the saved search represented by the given id. The authenticating user must be the owner of saved search ID being requested.
*
* @param[in] id - The ID of the saved search, i.e. 313006
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::showSavedSearch(std::string id) {
    mpAccount->savedSearchShow(id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/saved_searches/create
*Create a new saved search for the authenticated user. A user may only have 25 saved searches.
*
* @param[in] query - The query of the search the user would like to save.
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::createSavedSearch(std::string query) {
    mpAccount->savedSearchCreate(query);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}

/**
*https://dev.twitter.com/rest/reference/post/saved_searches/destroy/%3Aid
*Destroys a saved search for the authenticating user. The authenticating user must be the owner of saved search id being destroyed.
*
* @param[in] id - The ID of the saved search. Example Values: 313006
* @param[out] rapidjson::Document
**/
rapidjson::Document Twitter::destroySavedSearch(std::string id) {
    mpAccount->savedSearchDestroy(id);
    std::string replyMsg = "";
    mpAccount->getLastWebResponse(replyMsg);
    return toJSON(replyMsg);
}
