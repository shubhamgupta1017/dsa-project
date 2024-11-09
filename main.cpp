#ifndef MAIN_CPP
#define MAIN_CPP
#include "spot.cpp"
#include "future.cpp"
#include "options.cpp"


int main() {
    //create fresh stock prices file
    ofstream file("stock_prices.dat");
    file.clear();
    //FILE HEADER
    file << "Timestamp" << " " << "Company" << " " << "Price" << " " << "Buyer ID" << " " << "Seller ID" << "\n";
    file.close();
    //create fresh cancelled orders file
    ofstream file1("cancelled_orders.dat");
    file1.clear();
    file1.close();
    
    // Example: Generate 90000 orders for the simulation
    initializeTrie();
    int numDays = 7;
    int stampsDay = 43200;
    int number_user=1000;
    for(int i = 0; i < numDays; i++){
        int day = i+1;
        generateOrders(stampsDay, day);
        writeCancelledOrdersToFile(day);
    }
    // Generate the futures trading order book and save it to a CSV file
    generateFuturesTradingOrderBookFuture("futures_trading_orderbook_future.csv",getspotprice);
    OptionSimulator simulator(150, NUM_USERS_FUTURE,4, numDays);
    simulator.generateOrderBook(getspotprice);  

   

    //print the last trade prices for each stock
    for (auto it = last_trade_prices.begin(); it != last_trade_prices.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    char s;
    cout<<"terminate Y/N ";
    cin>>s;
    while(s!='Y'){
        getUserInfo();
        cout<<"terminate Y/N ";
        cin>>s;
    }
}
#endif