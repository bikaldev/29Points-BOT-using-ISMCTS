#include "./game.hpp"

GameEnv::GameEnv(PlayPayload &body) {
    
    cardsAtHand.push_back(body.cards);
    // std::cout<<"Cards at hand"<<cardsAtHand[0]<<std::endl;
    // arranging the order such that, we are always the 0th index player
    
    std::vector<PlayerID>::iterator itr = std::find(body.player_ids.begin(), body.player_ids.end(), body.player_id);
    int myIdx = std::distance(body.player_ids.begin(), itr);

    for(int i = 0; i < 4; i++) {
        playerNameList.push_back(body.player_ids[myIdx]);
        myIdx = (myIdx + 1) % 4;
    }

    // std::cout<<"player Name list"<<playerNameList<<std::endl;

    currentPlayer = 0;
    cardsAtTable = body.played;

    // std::cout<<"current player"<<currentPlayer<<std::endl;
    // std::cout<<"cards at table"<<cardsAtTable<<std::endl;

    // the trumpSuit status attributes
    if(std::holds_alternative<PlayPayload::RevealedObject>(body.trumpRevealed))
    {
        // with the assumption that trump has been revealed
        revealHand = std::get<PlayPayload::RevealedObject>(body.trumpRevealed).hand;
        trumpRevealer = std::get<PlayPayload::RevealedObject>(body.trumpRevealed).player_id;
        isTrumpRevealed = true;
        trumpSuit = std::get<Suit>(body.trumpSuit);

    } else {
        // our assumption was incorrect i.e the trump hasn't been revealed
        isTrumpRevealed = false;
        // we still might know the trump suit by being the bid winner
       if(std::holds_alternative<Suit>(body.trumpSuit)){
            trumpSuit = std::get<Suit>(body.trumpSuit);
            bidWinner = true;
        } else {
            bidWinner = false;
        }
    }

    // std::cout<<"istrump revealed"<<isTrumpRevealed<<std::endl;

    currentRound = body.hand_history.size() + 1;
    for(int i = 0; i < body.hand_history.size(); i++) {
        std::vector<Card> cards = body.hand_history[i].card;
        for(int j = 0; j < 4; j++) {
            cardsPlayed.push_back(cards[j]);
        }
    }

    for(int i = 0; i < cardsAtTable.size(); i++) {
        cardsPlayed.push_back(cardsAtTable[i]);
    }

    // std::cout<<"cardsPlayed: "<<cardsPlayed<<std::endl;

    done = false;

    nullScore = {0,0,0,0};
    winScore = {1,-1,1,-1};
    halfPointWinScore = {0.5, -0.5, 0.5, -0.5};
    halfPointLoseScore = {-0.5, 0.5, -0.5, 0.5};
    loseScore = {-1,1,-1,1};

    reward = nullScore;

    bool teamFound;
    PlayPayload::Teams team;
    for(int i = 0; i < 2; i++) {
        team = body.teams[i];
        teamFound = false;
        for(int j = 0; j < 2; j++) {
            if(team.players[j] == playerNameList[0]) {
                teamFound = true;
                myScore = team.won;
                myBid = team.bid;
            }
        }
        if(!teamFound) {
            // opp team
            oppScore = team.won;
            oppBid = team.bid;

        }
    }

    // std::cout<<"my score, opp score, my bid, opp bid"<<myScore<<" "<<oppScore<<" "<<myBid<<" "<<oppBid<<std::endl;

    // calculate cards remaining
    std::string rankList = "J91TKQ87";
    std::string suitList = "SHDC";
    for (int i = 0; i < 8; i++) {
        for(int j = 0; j < 4; j++) {
            char card[] = {'0',rankList[i],suitList[j], '\0'};
            // std::cout<<card<<std::endl;
            cardsRemaining.push_back(Card::FromStr(card));
        }
    }

    // std::cout<<"card remaining before removing"<<cardsRemaining<<std::endl;
    for(int i = 0; i < cardsPlayed.size(); i++) {
        auto newEnd = remove(cardsRemaining.begin(), cardsRemaining.end(), cardsPlayed[i]);
        cardsRemaining.erase(newEnd, cardsRemaining.end());
    }

    for (int i = 0; i < cardsAtHand[0].size(); i++) {
        auto newEnd = remove(cardsRemaining.begin(), cardsRemaining.end(), cardsAtHand[0][i]);
        cardsRemaining.erase(newEnd, cardsRemaining.end());
    } 

    // std::cout<<"cards remaining "<<cardsRemaining<<std::endl;

    // std::cout<<"constructor ends"<<std::endl;
}


GameEnv::GameEnv(GameEnv const & env) {
    cardsAtHand.push_back(env.cardsAtHand[0]);
    currentPlayer = env.currentPlayer;
    currentRound = env.currentRound;
    isTrumpRevealed = env.isTrumpRevealed;
    if(isTrumpRevealed) {
        trumpRevealer = env.trumpRevealer;
        revealHand = env.revealHand;
        trumpSuit = env.trumpSuit;
    } else {
        bidWinner = env.bidWinner;
        if(env.bidWinner) {
            trumpSuit = env.trumpSuit;
        }
    }
    
    cardsAtTable = env.cardsAtTable;
    cardsPlayed = env.cardsPlayed;
    cardsRemaining = env.cardsRemaining;
    playerNameList = env.playerNameList;
    myScore = env.myScore;
    myBid = env.myBid;
    oppScore = env.oppScore;
    oppBid = env.oppBid;

    reward = env.reward;
    nullScore = env.nullScore;
    winScore = env.winScore;
    loseScore = env.loseScore;
    halfPointLoseScore = env.halfPointLoseScore;
    halfPointWinScore = env.halfPointWinScore;
    
    done = env.done;

}

void GameEnv::resetToEnv(GameEnv& env)
{
    // cardsAtHand.pop_back();
    // cardsAtHand.pop_back();
    // cardsAtHand.pop_back();
    // cardsAtHand.pop_back();
    
    cardsAtHand.clear();
    cardsAtHand.push_back(env.cardsAtHand[0]);

    currentPlayer = env.currentPlayer;
    currentRound = env.currentRound;
    isTrumpRevealed = env.isTrumpRevealed;
    
    if(isTrumpRevealed) {
        trumpRevealer = env.trumpRevealer;
        revealHand = env.revealHand;
        trumpSuit = env.trumpSuit;
    } else {
        bidWinner = env.bidWinner;
        if(env.bidWinner) {
            trumpSuit = env.trumpSuit;
        }
    }

    cardsAtTable = env.cardsAtTable;
    cardsPlayed = env.cardsPlayed;
    cardsRemaining = env.cardsRemaining;
    playerNameList = env.playerNameList;
    myScore = env.myScore;
    oppScore = env.oppScore;

    reward = env.reward;
    
    done = env.done;

}

bool GameEnv::checkForSuit(std::vector<Card> cards, Suit suit) {
    for(int i = 0; i < cards.size(); i++) {
        if(cards[i].suit == suit)
            return true;
    }

    return false;
}


bool GameEnv::compareCard(Card c1, Card c2) {
    if(c1.suit == c2.suit) {
        return RankOrder(c1.rank) > RankOrder(c2.rank);
    } 
        
    if(isTrumpRevealed){
        if(c1.suit == trumpSuit) return true;
    }
    
    return false;
}


Card GameEnv::getMaxCard(std::vector<Card> cards){
    // std::cout<<"--getmaxcard--"<<std::endl;
    Card maxCard = cards[0];
    for(int i = 1; i < cards.size(); i++) {
        // std::cout<<"cards in question: "<<cards[i]<<" "<<maxCard<<std::endl;
        if(compareCard(cards[i],maxCard)) {
            // std::cout<<"c1 > c2"<<std::endl;
            maxCard = cards[i];
        }
            
    }
    return maxCard;
}

std::vector<Card> GameEnv::avail_actions() {
    // for(int i = 0 ; i < 4; i++) {
    //     std::cout<<"The cards in order of players are: "<<cardsAtHand[i]<<std::endl;
    // }
    // std::cout<<"cards at table: "<<cardsAtTable<<std::endl;
    // std::cout<<"current player: "<<currentPlayer<<std::endl;
    std::vector<Card> currentCards = cardsAtHand[currentPlayer];
    if(cardsAtTable.empty()) {
        // we are the first player, can play anything
        return currentCards;
    } else {
        std::vector<Card> actions;
        // we have to follow suit if possible
        Suit leadingSuit = cardsAtTable[0].suit;
        // std::cout<<"leading suit "<<leadingSuit<<std::endl;
        if(checkForSuit(currentCards, leadingSuit)){
            // std::cout<<"suit exists"<<std::endl;
            for(auto i = currentCards.begin(); i < currentCards.end(); i++) {
                // std::cout<<*i<<std::endl;
                if((*i).suit == leadingSuit)
                    actions.push_back(*i);
            }

            // std::cout<<"actions: "<<actions<<std::endl;

        } else if(!isTrumpRevealed) {
            // std::cout<<"trump is not revealed"<<std::endl;
            actions = currentCards;
            actions.push_back(Card(RRT, SRT));
            // we don't have any cards to follow suit and trump hasn't been revealed
        } else {
            // trump has been revealed
            // std::cout<<"trump is revealed"<<std::endl;
            // std::cout<<"the trump suit is: "<<trumpSuit<<std::endl;
            if(trumpRevealer == playerNameList[currentPlayer] && revealHand == currentRound) {
                // std::cout<<"it was revealed in this round by the current player"<<std::endl;
                // current player revealed trump in this round
                Card maxCard = getMaxCard(cardsAtTable);
                // std::cout<<"max card "<<maxCard<<std::endl;
                int32_t maxCardPlayer;
                std::vector<Card>::iterator itr = std::find(cardsAtTable.begin(),cardsAtTable.end(), maxCard);
                int32_t maxCardIdx = std::distance(cardsAtTable.begin(), itr);
                maxCardPlayer = ((currentPlayer + 4 - cardsAtTable.size()) % 4) + maxCardIdx;
                // std::cout<<"max card player "<<maxCardPlayer<<std::endl;
                if(maxCardPlayer == (currentPlayer + 2) % 4){
                    // std::cout<<"partner is maxcard player"<<std::endl;
                    std::vector<Card> trumpActions;
                    for(int i = 0; i < currentCards.size(); i++) {
                        if(currentCards[i].suit == trumpSuit)
                            trumpActions.push_back(currentCards[i]);
        
                    }
                    // std::cout<<"trump actions are: "<<trumpActions<<std::endl;
                    if(trumpActions.empty())
                        return currentCards;
                    else
                        return trumpActions;
                    // can play any trump actions
                } else {
                    std::vector<Card> trumpActions;
                    // std::cout<<"partner is not maxcard player"<<std::endl;
                    for(int i = 0; i < currentCards.size(); i++) {
                        if(currentCards[i].suit == trumpSuit && compareCard(currentCards[i], maxCard))
                            actions.push_back(currentCards[i]);
                        if(currentCards[i].suit == trumpSuit)
                            trumpActions.push_back(currentCards[i]);
                    }
                    // std::cout<<"the winning trumps are: "<<actions<<std::endl;
                    if(actions.empty()) {
                        if(trumpActions.empty())
                            return currentCards;
                        return trumpActions;
                    }
                    else
                        return actions;
                    // will have to play winning trump actions
                }
            }
            return currentCards;
        }
    return actions;
    }
}

int GameEnv::determineWinner(std::vector<Card> cards) {
    // std::cout<<"--Determine Winner function--"<<std::endl;
    Card maxCard = getMaxCard(cards);
    // std::cout<<"the input cards are: "<<cards<<std::endl;
    // std::cout<<"the max card is: "<<maxCard<<std::endl;
    std::vector<Card>::iterator itr = std::find(cards.begin(), cards.end(), maxCard);
    int maxCardIdx = std::distance(cards.begin(), itr);
    // std::cout<<"max card idx is: "<<maxCardIdx<<std::endl;
    return (((currentPlayer + 1) % 4) + maxCardIdx) % 4;
}

int GameEnv::determineScore(std::vector<Card> cards) {
    int score = 0;
    for(int i = 0; i < 4; i++){
        score += CardValue(cards[i].rank);
    }

    return score;
}

std::vector<float> GameEnv::step(Card card, bool simul) {

    if(currentRound >= 4 && isTrumpRevealed && simul) {
        std::vector<int> possibList = possibiltyList();
        if(possibList[0] != 1 || possibList[1] != 1) {
            // a player has already won the game
            done = true;
            if(possibList[0] == 0)
                return loseScore;
            else
                return winScore;
        }
    }

    if(card.suit == SRT) {
        // rt action
        isTrumpRevealed = true;
        trumpRevealer = playerNameList[currentPlayer];
        revealHand = currentRound;
        return nullScore;
    } else {
        // any other card
        cardsAtTable.push_back(card);
        auto newEnd = remove(cardsAtHand[currentPlayer].begin(), cardsAtHand[currentPlayer].end(), card);
        cardsAtHand[currentPlayer].erase(newEnd, cardsAtHand[currentPlayer].end());
        if(cardsAtTable.size() == 4){
            // determine winner and move to next hand
            int winner = determineWinner(cardsAtTable);
            int score = determineScore(cardsAtTable);
            if(winner % 2 == 0)
                myScore += score;
            else
                oppScore += score;
            
            currentPlayer = winner;
            currentRound++;
            if(currentRound == 9) {
                // the game is over
                if((myBid > 0 && myScore >= myBid) || (oppBid > 0 && oppScore < oppBid)){
                    // I win in the case that trump has been revealed
                    if(isTrumpRevealed)
                        reward = winScore;
                    else
                        reward = loseScore;
                } else {
                    // I lose
                    if(isTrumpRevealed)
                        reward = loseScore;
                    else
                        reward = winScore;
                }
                done = true;
                return reward;
            }
            cardsAtTable.clear();
            return nullScore;

        } 
        
        // determine new player
        currentPlayer = (currentPlayer + 1) % 4;
        return nullScore;

    }
}

void GameEnv::distributeCards(std::vector<Card> cards) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(std::begin(cards), std::end(cards), std::default_random_engine(seed));
    int threshold = 3 - cardsAtTable.size();
    int size;
    for(int i = 1 ; i < 4; i++) {
        if(i <= threshold)
            size = 9 - currentRound;
        else
            size = 8 - currentRound;

        std::vector<Card> sample;
        std::sample(cards.begin(), cards.end(), std::back_inserter(sample), size,  std::mt19937{std::random_device{}()}); 
        for(int j = 0; j < size; j++) {
            auto newEnd = remove(cards.begin(), cards.end(), sample[j]);
            cards.erase(newEnd, cards.end());
        }
        cardsAtHand.push_back(sample);
    }
}

void GameEnv::randomize() {
    // std::cout<<"--randomize--"<<std::endl;
    distributeCards(cardsRemaining);
    if(!isTrumpRevealed) {
        if(!bidWinner) {
        Suit suitList[] = {Suit::SPADES, Suit::HEARTS, Suit::CLUBS, Suit::DIAMONDS};
        int idx = rand() % 4;
        trumpSuit = suitList[idx];
        // std::cout<<"The randomly selected trumpsuit is: "<<SuitToStr(trumpSuit)<<std::endl;
        }
    }
    // std::cout<<"randomized successfully"<<std::endl;
}

std::vector<int> GameEnv::possibiltyList() 
{
    int totalScore = myScore + oppScore;
    int remPoints = 28 - totalScore;
    int myProjScore = myScore + remPoints;
    int oppProjScore = oppScore + remPoints;
    std::vector<int> finalList;
    if(myBid > 0)
    {
        finalList.push_back((int)(myProjScore >= myBid));
        finalList.push_back((int)(myScore < myBid));
    }  else {
        finalList.push_back((int)(oppScore < oppBid));
        finalList.push_back((int)(oppProjScore >= oppBid));
    }

    return finalList;
}
