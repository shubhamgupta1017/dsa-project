#ifndef FNO_CPP
#define FNO_CPP

#include <bits/stdc++.h>
using namespace std;


class FutureMarket{
    struct Contract{
        string companyName;
        int companyID, lot, expiry;
        Contract(string companyName, int companyID, int lot, int expiry, int companyID)
            :companyID(companyID), lot(lot), expiry(expiry), companyName(companyName) {}
    };

    struct Bid{
        int strike, amt;
        Bid(int strike, int amt)
            :strike(strike), amt(amt) {}
    };

    struct User{
        int userID;
        // map from contract id -> bid
        map<int, Bid> buyOrders;
        map<int, Bid> sellOrders;

        // map from companyName -> margin
        map<string, int> margin;

        User(int ID):userID(ID) {}
    };

    // map from contractID -> contract
    map<int, Contract> contracts;
    // map from userID -> user
    map<int, User> users;
    
    void addContract(int ID, string companyName, int lot, int expiry, int companyID){
        contracts[ID] = Contract(companyName, lot, expiry, companyID);
    }


    void addBid(int userID, int contractID, int strike, int companyID, int futurePrice, int amt, int orderType){
        // check if user exists
        if(users.find(userID) == users.end()){
            users[userID] = User(userID);
        }
        
        // add bid to user
        if(orderType == 0){
            // if order doesnt exist, add it
            if (users[userID].buyOrders.find(contractID) == users[userID].buyOrders.end()){
                users[userID].buyOrders[contractID] = Bid(strike, amt);
            }else{
                // if order exists, update it
                int newAmt = users[userID].buyOrders[contractID].amt + amt;
                int newStrike = (users[userID].buyOrders[contractID].strike * users[userID].buyOrders[contractID].amt + strike * amt) / newAmt;
                users[userID].buyOrders[contractID] = Bid(newStrike, newAmt);
            }
        }else{
            // if order doesnt exist, add it
            if (users[userID].sellOrders.find(contractID) == users[userID].sellOrders.end()){
                users[userID].sellOrders[contractID] = Bid(strike, amt);
            }else{
                // if order exists, update it
                int newAmt = users[userID].sellOrders[contractID].amt + amt;
                int newStrike = (users[userID].sellOrders[contractID].strike * users[userID].sellOrders[contractID].amt + strike * amt) / newAmt;
                users[userID].sellOrders[contractID] = Bid(newStrike, newAmt);
            }
        }
    }

    void cancelBid(int userID, int contractID, int orderType, int amt, int time){
        // check if user exists
        if(users.find(userID) == users.end()){
            return;
        }

        // check if order exists
        if(users[userID].buyOrders.find(contractID) == users[userID].buyOrders.end()){
            return;
        }

        if(orderType == 0){
            // update margin
            int spotPrice = getspotprice[time][contracts[contractID].companyID];
            margin[contracts[contractID].companyName] += (spotPrice - users[userID].buyOrders[contractID].strike) * amt * contracts[contractID].lot;

            users[userID].buyOrders[contractID].amt -= amt;
            // if order is empty, remove it
            if(users[userID].buyOrders[contractID].amt == 0){
                users[userID].buyOrders.erase(contractID);
            }
        } else {
            // update margin
            int spotPrice = getspotprice[time][contracts[contractID].companyID];
            margin[contracts[contractID].companyName] -= (spotPrice - users[userID].sellOrders[contractID].strike) * amt * contracts[contractID].lot;

            users[userID].sellOrders[contractID].amt -= amt;
            // if order is empty, remove it
            if(users[userID].sellOrders[contractID].amt == 0){
                users[userID].sellOrders.erase(contractID);
            }
        }
    }
};


class OptionsMarket{
    struct Contract{
        string companyName;
        int lot, expiry;
        Contract(string companyName, int lot, int expiry)
            :lot(lot), expiry(expiry), companyName(companyName) {}
    };

    struct Bid{
        int strike, premium, amt;
        Bid(int strike, int premium, int amt)
            :strike(strike), premium(premium), amt(amt) {}
    };

    struct User{
        int userID;
        // map from contract id -> bid
        map<int, Bid> buyOrders;
        map<int, Bid> sellOrders;

        // map from contract id -> margin
        map<int, int> margin;

        User(int ID):userID(ID) {}
    };

    // map from contractID -> contract
    map<int, Contract> contracts;
    // map from userID -> user
    map<int, User> users;
    
    void addContract(int ID, string companyName, int lot, int expiry){
        contracts[ID] = Contract(companyName, lot, expiry);
    }

    void addBid(int userID, int contractID, int strike, int premium, int amt, int orderType){
        // check if user exists
        if(users.find(userID) == users.end()){
            users[userID] = User(userID);
        }
        
        // add bid to user
        if(orderType == 0){
            // if order doesnt exist, add it
            if (users[userID].buyOrders.find(contractID) == users[userID].buyOrders.end()){
                users[userID].buyOrders[contractID] = Bid(strike, premium, amt);
            } else {
                // if order exists, update it
                int newAmt = users[userID].buyOrders[contractID].amt + amt;
                int newStrike = (users[userID].buyOrders[contractID].strike * users[userID].buyOrders[contractID].amt + strike * amt) / newAmt;
                int newPremium = (users[userID].buyOrders[contractID].premium * users[userID].buyOrders[contractID].amt + premium * amt) / newAmt;
                users[userID].buyOrders[contractID] = Bid(newStrike, newPremium, newAmt);
            }
        } else {
            // if order doesnt exist, add it
            if (users[userID].sellOrders.find(contractID) == users[userID].sellOrders.end()){
                users[userID].sellOrders[contractID] = Bid(strike, premium, amt);
            }else{
                // if order exists, update it
                int newAmt = users[userID].sellOrders[contractID].amt + amt;
                int newStrike = (users[userID].sellOrders[contractID].strike * users[userID].sellOrders[contractID].amt + strike * amt) / newAmt;
                int newPremium = (users[userID].sellOrders[contractID].premium * users[userID].sellOrders[contractID].amt + premium * amt) / newAmt;
                users[userID].sellOrders[contractID] = Bid(newStrike, newPremium, newAmt);
            }
        }
    }

    void cancelBid(int userID, int contractID, int orderType, int amt, int time){
        // check if user exists
        if(users.find(userID) == users.end()){
            return;
        }

        // check if order exists
        if(users[userID].buyOrders.find(contractID) == users[userID].buyOrders.end()){
            return;
        }

        if(orderType == 0){
            // update margin
            spotPrice = getspotprice[time][contracts[contractID].companyID];
            users[userID].margin[contractID] += (users[userID].buyOrders[contractID].strike - spotPrice) * amt * contracts[contractID].lot;
            
            users[userID].buyOrders[contractID].amt -= amt;
            // if order is empty, remove it
            if(users[userID].buyOrders[contractID].amt == 0){
                users[userID].buyOrders.erase(contractID);
            }
        } else {
            // update margin

            users[userID].sellOrders[contractID].amt -= amt;
            // if order is empty, remove it
            if(users[userID].sellOrders[contractID].amt == 0){
                users[userID].sellOrders.erase(contractID);
            }
        }
    }
};

#endif