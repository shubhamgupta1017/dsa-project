#ifndef SPOT_CPP
#define SPOT_CPP
#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include "manage_user.cpp"
using namespace std;

// Create a function to write the spot prices of each stock at each timestamp in a file
unordered_map<long long, vector<double>> getspotprice;

// Hash map for the last trade price of each stock
unordered_map<string, double> last_trade_prices;

// Global variable for last recorded timestamp for getspotprice
long long last_price_update_time = 0;

// Struct to represent an Order
struct Order {
    string company_name;
    int user_id;
    bool is_buy;
    double price;
    long long timestamp;
    double quantity;

    Order(string comp, int id, bool buy, double p, long long ts, double q = 1.0)
        : company_name(comp), user_id(id), is_buy(buy), price(p), timestamp(ts), quantity(q) {}
};

// Comparator for max-heap (buy orders)
struct BuyComparator {
    bool operator()(const Order &a, const Order &b) {
        return a.price < b.price || (a.price == b.price && a.timestamp > b.timestamp);
    }
};

// Comparator for min-heap (sell orders)
struct SellComparator {
    bool operator()(const Order &a, const Order &b) {
        return a.price > b.price || (a.price == b.price && a.timestamp > b.timestamp);
    }
};

// Priority queues for buy and sell orders
priority_queue<Order, vector<Order>, BuyComparator> buyOrders;
priority_queue<Order, vector<Order>, SellComparator> sellOrders;

// Function to execute a trade between buyer and seller
void executeTrade(int buyer_id, int seller_id, double price, double quantity, const string &company) {
    doTransaction(buyer_id, seller_id, price, quantity, company);
}

// Function to log the price of a stock to a file
void logPriceToFile(const string &company, long long timestamp, double price, int buyer_id, int seller_id) {
    ofstream file("stock_prices.dat", ios::app);
    file << timestamp << " " << company << " " << price << " " << buyer_id << " " << seller_id << "\n";
    file.close();
}

// Function to write all canceled orders to a file
void writeCancelledOrdersToFile(int day) {
    ofstream file("cancelled_orders.dat", ios::app);
    file << "Day " << day << "\n";

    while (!buyOrders.empty()) {
        Order order = buyOrders.top();
        buyOrders.pop();
        file << order.timestamp << " " << order.company_name << " " << order.price << " " 
             << order.user_id << " " << order.quantity << " Buy\n";
    }

    while (!sellOrders.empty()) {
        Order order = sellOrders.top();
        sellOrders.pop();
        file << order.timestamp << " " << order.company_name << " " << order.price << " " 
             << order.user_id << " " << order.quantity << " Sell\n";
    }
    
    file.close();
}

// Function to process a spot trade
void spotTrade(const string &company, int user_id, bool is_buy, double price, long long timestamp, double quantity) {
    Order order(company, user_id, is_buy, price, timestamp, quantity);

    if (is_buy) {
        while (!sellOrders.empty() && sellOrders.top().price <= price) {
            Order sellOrder = sellOrders.top();
            sellOrders.pop();
            if (sellOrder.company_name != company) continue;

            double tradeQuantity = min(order.quantity, sellOrder.quantity);
            executeTrade(user_id, sellOrder.user_id, sellOrder.price, tradeQuantity, company);
            last_trade_prices[company] = sellOrder.price;
            logPriceToFile(company, timestamp, sellOrder.price, user_id, sellOrder.user_id);

            order.quantity -= tradeQuantity;
            sellOrder.quantity -= tradeQuantity;

            if (sellOrder.quantity > 0) sellOrders.push(sellOrder);
            if (order.quantity == 0) return;
        }
        if (order.quantity > 0) buyOrders.push(order);
    } else {
        while (!buyOrders.empty() && buyOrders.top().price >= price) {
            Order buyOrder = buyOrders.top();
            buyOrders.pop();
            if (buyOrder.company_name != company) continue;

            double tradeQuantity = min(order.quantity, buyOrder.quantity);
            executeTrade(buyOrder.user_id, user_id, buyOrder.price, tradeQuantity, company);
            last_trade_prices[company] = buyOrder.price;
            logPriceToFile(company, timestamp, buyOrder.price, buyOrder.user_id, user_id);

            order.quantity -= tradeQuantity;
            buyOrder.quantity -= tradeQuantity;

            if (buyOrder.quantity > 0) buyOrders.push(buyOrder);
            if (order.quantity == 0) return;
        }
        if (order.quantity > 0) sellOrders.push(order);
    }

    // Record spot prices at each 500 ms interval
    if (timestamp - last_price_update_time >= 500) {
        vector<double> prices;
        for (const auto &entry : last_trade_prices) {
            prices.push_back(entry.second);
        }
        getspotprice[timestamp] = prices;
        last_price_update_time = timestamp;
    }
}

// Function to generate random orders
void generateOrders(int numOrders, int day) {
    srand(time(0));
    vector<string> companies = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA"};
    int numCompanies = companies.size();

    for (int i = 0; i < numCompanies; ++i) {
        last_trade_prices[companies[i]] = 150.0;
    }

    int maxUserId = 1000;
    double minPrice = 50.0, maxPrice = 250.0;
    int minQuantity = 1, maxQuantity = 50;
    long long timestamp = day * 100000000;

    for (int i = 0; i < numOrders; ++i) {
        string company = companies[rand() % numCompanies];
        int user_id = rand() % maxUserId + 1;
        bool is_buy = rand() % 2;
        double price = minPrice + (rand() / (double)RAND_MAX) * (maxPrice - minPrice);
        int quantity = rand() % (maxQuantity - minQuantity + 1) + minQuantity;

        spotTrade(company, user_id, is_buy, price, timestamp, quantity);
        timestamp += 500;
    }
}

#endif
