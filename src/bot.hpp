#pragma once
#include <cassert>
#include <iostream>
#include <variant>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <cmath>

#include "./util.hpp"
#include "./game.hpp"
#include "./node.hpp"


#if defined(__GNUC__)
#define UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE() __assume(false);
#endif








struct GameState
{
    // Keep any game related extra metadata here
    // like std::vector<Card> seen_cards;

    // Time remaining is in milliseconds
    static Suit    ChooseTrump(PlayerID myid, std::vector<PlayerID> player_ids, std::vector<Card> mycards,
        int32_t time_remaining, std::vector<BidEntry> bid_history);
    static int32_t Bid(PlayerID myid, std::vector<PlayerID> player_ids, std::vector<Card> mycards,
                       int32_t time_remaining, std::vector<BidEntry> bid_history, BidState const &bid_state);

    // If two players are to play through same instance, it might be troublesome (lol, that shouldn't happen though ig)
    PlayAction Play(PlayPayload payload);
};

GameState &GetGameInstance();
void       InitGameInstance();