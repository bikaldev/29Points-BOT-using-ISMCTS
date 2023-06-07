#include "./util.hpp"

std::ostream &operator<<(std::ostream &os, Card const &card)
{
    return os << Card::ToStr(card) << std::endl;
}

std::ostream &operator<<(std::ostream &os, PlayPayload const &payload)
{
    os << "Player ID : " << payload.player_id << std::endl;
    os << "Player IDs : ";
    for (auto const &player : payload.player_ids)
        os << player << "    ";

    os << "\nCards \n";
    for (auto card : payload.cards)
        os << card << "   ";

    os << "\nPlayed : \n";
    for (auto card : payload.played)
        os << card << "  \n";

    os << "\nBid History : \n";
    for (auto const &entry : payload.bid_history)
    {
        os << "player    : " << entry.player_id << "\n"
           << "Bid value : " << entry.bid_value << std::endl;
    }

    // Hand history
    for (auto const &x : payload.hand_history)
    {
        os << "Initiator : " << x.initiator;
        os << "\nWinner   : " << x.winner;
        os << "\nCards Played : ";
        for (auto card : x.card)
            os << card << "   ";
        os << std::endl;
    }
    return os;
}
