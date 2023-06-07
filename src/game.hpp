#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <variant>
#include <random>
#include <cstdlib>

#include "./util.hpp"


class GameEnv {
    public:
        GameEnv(PlayPayload &);
        GameEnv(GameEnv const &);
    private:
       
        Suit trumpSuit;
        int32_t revealHand;
        PlayerID trumpRevealer;
        std::vector<PlayerID> playerNameList;
        
        std::vector<Card> cardsPlayed;
        
        
        bool bidWinner;
        int myScore;
        int oppScore;
        int myBid;
        int oppBid;
        std::vector<Card> cardsRemaining;

        bool checkForSuit(std::vector<Card>, Suit);
        Card getMaxCard(std::vector<Card>);
        bool compareCard(Card c1, Card c2);
        std::vector<float> nullScore;
        std::vector<float> winScore;
        std::vector<float> loseScore;
        std::vector<float> halfPointWinScore;
        std::vector<float> halfPointLoseScore;
        int determineWinner(std::vector<Card>);
        int determineScore(std::vector<Card>);
        void distributeCards(std::vector<Card>);
        
    public:
        bool isTrumpRevealed;
        std::vector<std::vector<Card>> cardsAtHand;
        std::vector<Card> cardsAtTable;
        bool done;
        std::vector<float> reward;
        int32_t currentPlayer;
        int32_t currentRound;
        std::vector<Card> avail_actions(); 
        void randomize();
        std::vector<float> step(Card, bool simul = false);
        std::vector<int> possibiltyList();
        void resetToEnv(GameEnv&);
};
