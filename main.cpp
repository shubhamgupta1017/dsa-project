#include "generating_function.cpp"
#include "fno.hpp"

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

    // create futures market
    FutureMarket futureMarket;

    ifstream file("Fdata.csv");  // Open the CSV file
    string line;
    
    // This will store the data
    vector<vector<string>> data;

    // Check if file is open
    if (!file.is_open()) {
        std::cerr << "Failed to open the file!" << std::endl;
        return 1;
    }

    // Reading each line from the CSV file
    while (getline(file, line)) {
        vector<string> row;  // Store a single row of data
        stringstream lineStream(line);  // Turn the line into a stream
        string cell;

        // Split the line by comma
        while (getline(lineStream, cell, ',')) {
            row.push_back(cell);  // Add each cell to the row
        }

        data.push_back(row);  // Add the row to the data
    }

    file.close();  // Close the file after reading

    // Output the data (for demonstration)
    for (const auto& row : data) {
        for (const auto& cell : row) {
            std::cout << cell << " ";
        }
        std::cout << std::endl;
    }
}