//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "externals/twitCurl/twitcurl.h"
#include "externals/twitCurl/include/rapidjson/document.h"
#include <cstdio>
#include <iostream>
#include <fstream>

class Twitter {

public:

    twitCurl* mpAccount;
    Twitter(twitCurl* pAccount, std::string name, std::string passwort, bool swap);
    rapidjson::Document toJSON(std::string); // This way?

    // STATUS
    rapidjson::Document statusUpdate(std::string text);
    rapidjson::Document reply(std::string text, std::string id);
    rapidjson::Document retweet(std::string userId);
    rapidjson::Document statusShow(std::string id);
    rapidjson::Document statusDestroy(std::string id);

    // SEARCH
    rapidjson::Document search(std::string text, std::string limits);
    rapidjson::Document userSearch(std::string text, std::string limits);

    // TIMELINE
    rapidjson::Document getTimelineHome(std::string sinceId);
    rapidjson::Document getTimelineUser(bool trimUser, bool includeRetweets, unsigned int tweetCount, std::string userInfo, bool isUserId);
    rapidjson::Document getMentions(std::string sinceId);

    // MESSAGES
    rapidjson::Document getDirectMessages();
    rapidjson::Document getDirectMessagesSent();
    rapidjson::Document sendDirectMessage(std::string text, std::string userId, bool isUserId);
    rapidjson::Document destroyDirectMessage(std::string id);

    // BLOCK
    rapidjson::Document blockCreate(std::string userInfo, bool isUserId);
    rapidjson::Document blockDestroy(std::string userInfo, bool isUserId);
    rapidjson::Document blockListGet(std::string nextCursor, bool includeEntities, bool skipStatus);
    rapidjson::Document blockIdsGet(std::string nextCursor, bool stringifyIds);

    // USER
    rapidjson::Document showUser(std::string userId, bool isUserId);
    rapidjson::Document suggestionsGetSlugs(std::string lang = "");
    rapidjson::Document suggestionsGet(std::string slug, std::string lang = "");

    // FRIENDSHIPS
    rapidjson::Document createFriendship(std::string userId, bool isUserId);
    rapidjson::Document destroyFriendship(std::string userId, bool isUserId);
    rapidjson::Document showFriendship(std::string userId, bool isUserId);

    // FAVORITES
    rapidjson::Document createFavorites(std::string id);
    rapidjson::Document destroyFavorites(std::string id);
    rapidjson::Document getListOfFavorites();

    // TRENDS
    rapidjson::Document getTrendsAvailable();
    rapidjson::Document getCurrentTrends();
    rapidjson::Document getDailyTrends();
    rapidjson::Document getWeeklyTrends();

    // SAVED SEARCHES
    rapidjson::Document getListOfSavedSearches();
    rapidjson::Document showSavedSearch(std::string id);
    rapidjson::Document createSavedSearch(std::string query);
    rapidjson::Document destroySavedSearch(std::string id);

    // ACCOUNT
    rapidjson::Document getAccountLimit();
    bool isLimit();
};
