#include "./bot.hpp"
#include <algorithm>

#ifdef RANGES_SUPPORT
#undef RANGES_SUPPORT
#endif

#if _HAS_CXX20
#define RANGES_SUPPORT 1
#include <ranges>
#endif




static GameState game_instance;

GameState& GetGameInstance()
{
    return game_instance;
}

void InitGameInstance()
{
    // Initialization code of game state goes here, if any
}

//
// Actual gameplay goes here
//
// These three functions are called in response to the http request calls /chooseTrump, /bid and /play respectively. 
// Logic of your code should go inside respective functions
//

Suit GameState::ChooseTrump(PlayerID myid, std::vector<PlayerID> player_ids, std::vector<Card> mycards,
                            int32_t time_remaining, std::vector<BidEntry> bid_history)
{
    int totalScore = 0;
    std::vector<int> suitScore = {0,0,0,0};
    std::vector<Suit> suitList = {Suit::CLUBS, Suit::DIAMONDS, Suit::HEARTS, Suit::SPADES};
    for(auto i : mycards) {
        suitScore[i.suit] += 1;
        totalScore += CardValue(i.rank);
    }

    int maxScore = suitScore[0];
    int maxIdx = 0;
    for(int j = 1; j < 4; j++) {
        if(suitScore[j] > maxScore){
            maxScore = suitScore[j];
            maxIdx = j;
        }
    }

    return suitList[maxIdx];
}

int GameState::Bid(PlayerID myid, std::vector<PlayerID> player_ids, std::vector<Card> mycards, int32_t time_remaining,
                   std::vector<BidEntry> bid_history, BidState const &bid_state)
{
    int MIN_BID = 16, PASS_BID = 0, totalScore = 0, MIN_POSSIBLE_BID, MAX_BID;
    std::vector<int> suitScore = {0,0,0,0};
    for(auto i : mycards) {
        suitScore[i.suit] += 1;
        totalScore += CardValue(i.rank);
    }

    int maxScore = suitScore[0];
    for(int j = 1; j < 4; j++) {
        if(suitScore[j] > maxScore){
            maxScore = suitScore[j];
        }
    }

    // int idx = 0;
    // for(auto i : player_ids)
    // {
    //     if(i == myid)
    //     {
    //         idx += 1;
    //         break;
    //     }
            
    // }

    // PlayerID partnerId = player_ids[(idx + 2) % 4];

    // std::cout<<"max score: "<<maxScore<<std::endl;
    // if(maxScore == 1 && totalScore > 4)
    //     MAX_BID = 16;
    if(maxScore == 2)
        MAX_BID = 16;
    else if(maxScore == 3)
        MAX_BID = 18;
    else if(maxScore == 4)
        MAX_BID = 22;
    else {
        MAX_BID = PASS_BID;
        return MAX_BID;
    }
        

    if(bid_history.empty()){
        // std::cout<<"Here first"<<std::endl;
        MIN_POSSIBLE_BID = MIN_BID;
    }
    else{
        // std::cout<<"defender and bid: "<<bid_state.defender.player_id<<" "<<bid_state.defender.bid_value<<std::endl;
        // std::cout<<"challenger and bid: "<<bid_state.challenger.player_id<<" "<<bid_state.challenger.bid_value<<std::endl;
        if(bid_state.defender.player_id == myid)
        {
            MIN_POSSIBLE_BID = bid_state.challenger.bid_value;
            if(MIN_POSSIBLE_BID == 0)
                MIN_POSSIBLE_BID = MIN_BID;
        }
        
        else if(bid_state.challenger.player_id == myid) {
            MIN_POSSIBLE_BID = bid_state.defender.bid_value + 1;
            if(MIN_POSSIBLE_BID == 1)
                MIN_POSSIBLE_BID = MIN_BID;
        }
        else 
        {
            return -1;
        }
    }

    // if(bid_state.defender.player_id == partnerId || bid_state.challenger.player_id == partnerId)
    // {
    //     return PASS_BID;
    // }

    if(MIN_POSSIBLE_BID <= MAX_BID)
        return MIN_POSSIBLE_BID;
    else
        return PASS_BID;
}

bool checkForNewActions(Node* focusNode, GameEnv* tempEnv) {
    std::vector<Card> totalActions = tempEnv->avail_actions();
    std::vector<Card> expandedActions;
    
    for(auto i = focusNode->children.begin(); i < focusNode->children.end(); i++) {
        expandedActions.push_back((*i) -> action);
    }
    for(int j = 0; j < totalActions.size(); j++) {
        std::vector<Card>::iterator itr = std::find(expandedActions.begin(), expandedActions.end(), totalActions[j]);
        if(itr == expandedActions.end()) {
            // there is an available action not yet expanded
            return true;
        }
    }
    return false;
}


double UCB1(Node* focusNode) {
    double n = focusNode->data.n;
    double n_avail = focusNode->data.n_avail;
    double c = sqrt(2);
    std::vector<float> rewardVec = focusNode->data.reward;
    double idx = focusNode->data.player;
    double rewardVal = rewardVec[idx];
    
    return rewardVal / n + c * sqrt(log(n_avail)/n);
}


Node* UCB1_max(Node* focusNode, GameEnv* tempEnv) {
    // std::cout<<"----UCB1 max-----"<<std::endl;
    double maxVal = -100.00;
    Node* maxNode;
    std::vector<Card> availActions = tempEnv->avail_actions();
    std::vector<std::string> stringActions;
    Card action;
    
    for(int j = 0; j < focusNode->children.size(); j++) {
        action = focusNode->children[j]->action;
        std::vector<Card>::iterator itr = std::find(availActions.begin(), availActions.end(), action);
        if(itr == availActions.end()) {
            continue;
        }
        
        double val = UCB1(focusNode->children[j]);
        // if(focusNode->action == "Root") {
        //     // debug section
        //     std::cout<<action<<"|"<<focusNode->children[j]->data.n<<"|"<<focusNode->children[j]->data.reward<<"|"<<focusNode->children[j]->data.n_avail<<"|"<<val<<std::endl;
        // }
        if(val > maxVal){
            maxVal = val;
            maxNode = focusNode->children[j];
        }
    }

    // if(focusNode->action == "Root")
    // {
    //     std::cout<<"The selected action is: "<<maxNode->action<<std::endl;
    // }

    return maxNode;

}

void displayNode(Node* node) {
    std::cout<<"data of "<<node->action<<std::endl;
    std::cout<<"n --> "<<node->data.n<<std::endl;
    std::cout<<"n_avail --> "<<node->data.n_avail<<std::endl;
    std::cout<<"reward --> "<<node->data.reward<<std::endl;
    std::cout<<"player --> "<<node->data.player<<std::endl;
    std::cout<<"_______________________________"<<std::endl;
}


Node* select(Node* focusNode, GameEnv* tempEnv) {
    while(! tempEnv->done && !checkForNewActions(focusNode, tempEnv)) 
    {
        // std::cout<<"--select--"<<std::endl;
        // displayNode(focusNode);
        // std::cout<<"The options are: "<<std::endl;
        // for(auto i = focusNode->children.begin(); i < focusNode->children.end(); i++) {
        //     displayNode(*i);
        // }
        focusNode = UCB1_max(focusNode, tempEnv);
        // std::cout<<"The selected node is: "<<focusNode->action<<std::endl;
        std::vector<Card> availActions = tempEnv->avail_actions();
        
        for(int i = 0; i < focusNode->parent->children.size(); i++) {
            Card action = focusNode->parent->children[i]->action;
            std::vector<Card>::iterator itr = std::find(availActions.begin(), availActions.end(), action);
            focusNode->parent->children[i]->data.avail = itr != availActions.end();
        }
        tempEnv->step(focusNode -> action);
    }

    return focusNode;
}



std::vector<Card> getNewActions(Node* focusNode, GameEnv* tempEnv) 
{
    std::vector<Card> totalActions = tempEnv->avail_actions();
   std::vector<Card> expandedActions, newActions;
    
    for(auto i = focusNode->children.begin(); i < focusNode->children.end(); i++) {
        expandedActions.push_back((*i) -> action);
    }
    for(int i = 0; i < totalActions.size(); i++) {
        std::vector<Card>::iterator itr = std::find(expandedActions.begin(), expandedActions.end(), totalActions[i]);
        if(itr == expandedActions.end()) {
            // there is an available action not yet expanded
            newActions.push_back(totalActions[i]);
        }
    }
    return newActions;
}

Node* expand(Node* focusNode, GameEnv* tempEnv) {
    std::vector<Card> newActions = getNewActions(focusNode, tempEnv);
    // std::cout<<"new actions are: "<<newActions<<std::endl;
    int idx = rand() % newActions.size();
    // std::cout<<"the random index is: "<<idx<<std::endl;
    Card action = newActions[idx];
    // std::cout<<"the new action is: "<<action<<std::endl;
    Node* childNode = new Node(action,tempEnv->currentPlayer);
    // std::cout<<"new node created"<<std::endl;
    focusNode->setChild(childNode);
    // std::cout<<"set child works"<<std::endl;
    // std::cout<<"card created successfully"<<std::endl;
    tempEnv->step(action);
    // std::cout<<"The step func works"<<std::endl;
    focusNode = childNode;

    return focusNode;
}

std::vector<float> simulate(GameEnv* tempEnv) {
    std::vector<float> reward = tempEnv->reward;
    std::vector<Card> actions;
    // std::cout<<"Current reward: "<<reward<<std::endl;
    while(!tempEnv->done)
    {
        actions = tempEnv->avail_actions();
       
        int idx = rand() % actions.size();
        // std::cout<<"Chosen action "<<actions[idx]<<std::endl;
       
        reward = tempEnv->step(actions[idx], true);
        // std::cout<<"observed reward "<<reward<<std::endl;
    }
    

    return reward;
}


void backpropagate(std::vector<float> reward, Node* focusNode) {
    // std::cout<<"Enter backprop"<<std::endl;
    // std::cout<<"reward: "<<reward<<std::endl;
    while(focusNode != nullptr) {
        focusNode->data.n += 1;
        // std::cout<<"Increased n"<<std::endl;
        for(int i = 0 ; i < 4; i++) {
            // std::cout<<"Reward vec increase iteration no "<<i<<std::endl;
            focusNode->data.reward[i] += reward[i];
        }
        // std::cout<<"mid backprop"<<std::endl;

        if(focusNode->parent != nullptr) {
            for(auto j = focusNode->parent->children.begin(); j < focusNode->parent->children.end(); j++){
                if((*j)->data.avail) {
                    (*j)->data.n_avail += 1;
                }
            }
        }
        // std::cout<<"end backprop"<<std::endl;
        focusNode = focusNode->parent;
    }
    // std::cout<<"Exit backprop"<<std::endl;
}




Card n_max(Node* rootNode) {
    float maxVal = -100000;
    Node* maxNode ;
    for(int i = 0; i < rootNode->children.size(); i++) {
        // std::cout<<"Inside nmax we have "<<rootNode->children[i]<<std::endl;
        int val = (rootNode->children[i])->data.n;
        if(val > maxVal) {
            maxVal = val;
            maxNode = rootNode->children[i];
        }
    }
    return maxNode->action;
}


PlayAction GameState::Play(PlayPayload payload)
{
    
    // std::cout << "Payload Received and parsed: \n" << payload << std::endl;

    PlayAction p_action;
    GameEnv env(payload);

    std::vector<Card> availActions = env.avail_actions();
    // std::cout<<"The available actions are: "<<availActions<<std::endl;
    if(availActions.size() == 1)
    {
        p_action.action = PlayAction::Action::PlayCard;
        // std::cout<<"The avail actions are: "<<availActions<<std::endl;
        p_action.played_card = availActions[0];
        // std::cout<<"Hello darkness my old friend";
        return p_action;
    }

    if(env.currentRound == 8 && availActions.size() == 2) {
        if(env.possibiltyList()[0] == 0) {
            p_action.action = PlayAction::Action::PlayCard;
            p_action.played_card = availActions[0];
            return p_action;
        }
        if(env.possibiltyList()[1] == 0)
        {
            p_action.action = PlayAction::Action::RevealTrump;
            return p_action;
        }
        
    }

    // std::cout<<"pass the preliminary checking"<<std::endl;
    
    std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now(), endTime = std::chrono::system_clock::now();
    Node rootNode(Card(Rank::RRT, Suit::SRT), 0, true);
    Node* focusNode = &rootNode;
    

    // float alocTime = (float)payload.remaining_time/ (float)(9 - env.currentRound + 1);
    float alocTime;
    // float remainingTime = (float)payload.remaining_time - 10;
    // float roundsRem = env.isTrumpRevealed? 8 - env.currentRound + 1: 9 - env.currentRound + 1;


    // alocTime = (env.currentRound == 8 && env.isTrumpRevealed)? remainingTime - 18 : 2 * (remainingTime - roundsRem * 20)/ roundsRem - 70;

    switch(env.currentRound) {
        case 1:
            alocTime = 260;
            break;
        case 2:
            alocTime = 200;
            break;
        case 3:
            alocTime = 150;
            break;
        case 4:
            alocTime = 150;
            break;
        case 5:
            alocTime = 70;
            break;
        case 6:
            alocTime = 70;
            break;
        case 7:
            alocTime = 60;
            break;
        case 8:
            // if(env.isTrumpRevealed)
            //     alocTime = (float)payload.remaining_time - 10;
            // else
            //     alocTime = (float)payload.remaining_time / 2 - 10;
            alocTime = 60;
            break;
        default:
            UNREACHABLE();
    }   

    // std::cout<<"The aloc time is: "<<alocTime<<std::endl; 
    
    // int count = 0;
    while((float)std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0 < alocTime) {
    // while(count < 2000) {
        
        GameEnv tempEnv(env);
        tempEnv.randomize();
        // for(int i = 0; i < 4; i++)
            // std::cout<<"The cards distributed to player "<<i<<" is: "<<tempEnv.cardsAtHand[i]<<std::endl;
        focusNode = &rootNode;
        focusNode = select(focusNode, &tempEnv);
        // std::cout<<"selects"<<std::endl;
        if(checkForNewActions(focusNode, &tempEnv)) {
            focusNode = expand(focusNode, &tempEnv);
            
        }
        // std::cout<<"expands"<<std::endl;
        std::vector<float> reward  = simulate(&tempEnv);
        // std::cout<<"simulates"<<std::endl;
        backpropagate(reward, focusNode);
        // std::cout<<"backprops"<<std::endl;
        endTime = std::chrono::system_clock::now();
        // count += 1;

    }

    // std::cout<<"Iterates smoothly"<<std::endl;
    
    Card action = n_max(&rootNode);

    // std::cout<<"count of round "<<env.currentRound<<" is : "<<count<<std::endl;

    // std::cout<<"The calculated action is: "<<std::endl; 
        
    if(action.rank == Rank::RRT) {
        p_action.action = PlayAction::Action::RevealTrump;
    } else {
        p_action.action = PlayAction::Action::PlayCard;
        p_action.played_card = action;
    }

    return p_action;

}
