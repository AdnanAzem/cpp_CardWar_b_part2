
#include "game.hpp"
#include <algorithm>
#include <random>
#include <chrono>

namespace ariel{
       
    Game::Game(Player &player1, Player &player2) : player1(player1), player2(player2), draw(0), rounds(0){
        this->deck = createDeck();
        shuffleDeck(this->deck);
        divideDeck(this->deck/*, this->player1, this->player2*/);
        gameover = false;
    }

    void Game::fillInfo(int playerNum, Card &playedCard1, Card &playedCard2, int points){
        if(playerNum == 1){ // fill info for player 1
            this->player1.setScore(points);
            this->winnerPerRound = player1.getName();
            this->lastRound = this->player1.getName() + " played " + playedCard1.to_string() + " " +this->player2.getName() + " played " + playedCard2.to_string() + ". " + winnerPerRound + " wins.\n";
            this->logs += this->lastRound;
        }
        else { // fill info for player 2
            this->player2.setScore(points);
            this->winnerPerRound = player2.getName();
            this->lastRound = this->player1.getName() + " played " + playedCard1.to_string() + " " + this->player2.getName() + " played " + playedCard2.to_string() + ". " + winnerPerRound + " wins.\n";
            this->logs += this->lastRound;
        }
    }

    void Game::addAllCards(Player &player, Card &card1, Card &card2, int points){
        player.AddWonCard(card1);
        player.AddWonCard(card2);
        player.setWins(points);
    }

    void Game::playTurn(){
        if(gameover){
            throw ("Game is over!!!");
            return;
        }
        if(&player1 == &player2){ throw ("Same Player!!!");}
        if(this->player1.stacksize() == 0 || this->player2.stacksize() == 0){
            gameover = true;
            return;
        }
        else{
            rounds++;
            Card card1 = this->player1.playCard();
            Card card2 = this->player2.playCard();
            if(card1.getValue() == 2 && card2.getValue() == 14){ // 2 wins against ace
                addAllCards(player1,card2,card1,1);
                fillInfo(1,card1,card2,1); 
            }
            else if(card1.getValue() == 14 && card2.getValue() == 2){ // 2 wins against ace
                addAllCards(player2,card1,card2,1);
                fillInfo(2,card1,card2,1);
            }
            else if(card1 > card2){ 
                addAllCards(player1,card2,card1,1);
                fillInfo(1,card1,card2,1);
            }
            else if(card1 < card2){
                addAllCards(player2,card1,card2,1);
                fillInfo(2,card1,card2,1);
            }
            else{ // draw
                draw++;
                this->lastRound = player1.getName() + " played " + card1.to_string() + " " +this->player2.getName() + " played " + card2.to_string() + ". Draw. ";
                this->logs += this->lastRound;
                vector<Card> jackpot;
                jackpot.push_back(card1);
                jackpot.push_back(card2);
                bool done = false;
                while(!done){
                  Card card1Closed, card1Open, card2Closed, card2Open;
                  if(this->player1.stacksize() < 2 || this->player2.stacksize() < 2){ // players have less than 2 cards
                    if(this->player1.stacksize() == 0 || this->player2.stacksize() == 0){
                        player1.AddWonCard(card1);
                        player2.AddWonCard(card2);
                        done = true;
                        break;
                    }
                    else{
                        player1.AddWonCard(card1);
                        card1 = this->player1.playCard();
                        player1.AddWonCard(card1);
                        player2.AddWonCard(card2);
                        card2 = this->player2.playCard();
                        player2.AddWonCard(card2);
                        done = true;
                        break;
                    }
                  }
                  card1Closed = this->player1.playCard();
                  card1Open = this->player1.playCard();
                  card2Closed = this->player2.playCard();
                  card2Open = this->player2.playCard();
                  if(card1Open > card2Open){
                    addToJackpot(jackpot, card1Open, card2Open, card1Closed, card2Closed);
                    for(Card card : jackpot){
                      this->player1.AddWonCard(card);
                    }
                    done = true;
                    fillInfo(1,card1Open,card2Open,3);
                  }
                  else if(card1Open < card2Open){
                    addToJackpot(jackpot, card2Open, card1Open, card2Closed, card1Closed);
                    for(Card card : jackpot){
                      this->player2.AddWonCard(card);
                      }
                    done = true;
                    fillInfo(2,card1Open,card2Open,3);
                  }
                  else{ // card1 = card2
                    draw++;
                    addToJackpot(jackpot, card1Closed, card2Closed, card1Open, card2Open);
                    this->lastRound = player1.getName() + " played " + card1.to_string() + " " +this->player2.getName() + " played " + card2.to_string() + ". draw. ";
                    this->logs += this->lastRound;
                  }
                }
            }
        }
    }

    void Game::addToJackpot(vector<Card> &jackpot, Card &card1, Card &card2, Card &card3, Card &card4){
        jackpot.push_back(card1);
        jackpot.push_back(card2);
        jackpot.push_back(card3);
        jackpot.push_back(card4);
    }

    void Game::printLastTurn(){
        std::cout << this->lastRound << endl;
    }

    void Game::playAll(){
        while(!gameover || player1.stacksize() > 0){
            playTurn();
        }
    }

    void Game::printWiner(){
        if (player1.cardesTaken() > player2.cardesTaken()){
            winner = player1.getName();
            std::cout << "The winner is : " << winner << endl;
        }
        else if (player1.cardesTaken() < player2.cardesTaken()){
            winner = player2.getName();
            std::cout << "The winner is : " << winner << endl;
        }
        else{
            std::cout << "Draw Game!" << endl;
        }
    }

    void Game::printLog(){
        std::cout << this->logs << endl;
    }

    void Game::printStats(){
        string cards1;
        for(Card card : this->player1.getWonCards()){
            cards1 += "\t\t" + card.to_string() + "\n";
        }

        string cards2;
        for(Card card : this->player2.getWonCards()){
            cards2 += "\t\t" + card.to_string() + "\n";
        }

        stats = fillStats(player1,cards1) + fillStats(player2,cards2);
        std::cout << this->stats << endl;
    }

    string Game::fillStats(Player &player, string cards){
        string res;
        res = player.getName() + "{\n"
            + "\tamout of rounds : " + to_string(rounds) + "\n"
            + "\tscore : " + to_string(player.getScore()) + "\n"
            + "\twins : " + to_string(player.getWins()) + "\n"
            + "\tamount of draws : " + to_string(this->draw) + "\n"
            + "\tdraw rate : " + to_string((double)this->draw/rounds*100.0) + "%\n" 
            + "\twinRates : " + to_string(player.getWins()*100.0/rounds) + "%\n"
            + "\tWonCards : " + to_string(player.getWonCards().size()) + "\n"
            + "\tCards [ \n" 
            + cards + "\n"
            + "\t]\n"
            + "}\n\n";
        return res;
    }

    vector<Card> Game::createDeck(){
        vector<Card> deck;
        // create a standard deck of 52 cards
        for (int suit = Card::clubs; suit <= Card::spades; ++suit){
            for (int rank = Card::two; rank <= Card::ace; ++rank){
                deck.push_back(Card(static_cast<Card::Value>(rank), static_cast<Card::Suit>(suit)));
            }
        }
        return deck;
    }

    void Game::shuffleDeck(vector<Card> &deck){
        random_device rd;
        mt19937 g(rd());
        shuffle(deck.begin(), deck.end(), g);
    }

    void Game::divideDeck(vector<Card> &deck/*, Player &player1, Player &player2*/){
        int counter = 0;
        for (Card card : deck){
            if (counter % 2 == 0){
                this->player1.addCardToHand(card);
                counter++;
            }
            else{
                this->player2.addCardToHand(card);
                counter++;
            }
        }
    }
}
