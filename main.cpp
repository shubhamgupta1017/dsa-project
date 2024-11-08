#include "generating_function.cpp"

int main() {

    //create fresh stock prices file
    ofstream file("data/stock_prices.dat");
    file.clear();
    //FILE HEADER
    file << "Timestamp" << " " << "Company" << " " << "Price" << " " << "Buyer ID" << " " << "Seller ID" << "\n";
    file.close();
    //create fresh cancelled orders file
    ofstream file1("data/cancelled_orders.dat");
    file1.clear();
    file1.close();

    // Example: Generate 90000 orders for the simulation
    initializeTrie();
    int numDays = 7;
    int stampsDay = 45000;
    for(int i = 0; i < numDays; i++){
        int day = i+1;
        generateOrders(stampsDay, day);
        writeCancelledOrdersToFile(day);
    }

    //print the last trade prices for each stock
    for (auto it = last_trade_prices.begin(); it != last_trade_prices.end(); ++it) {
        cout << it->first << ": " << it->second << endl;
    }
    char s;
    cout<<"terminate Y/N ";
    cin>>s;
    while(s!='Y'){
        getUserInfo(last_trade_prices);
        cout<<"terminate Y/N ";
        cin>>s;
    }
}