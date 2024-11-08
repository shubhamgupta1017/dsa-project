#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Define a struct to hold the stock data
struct StockData {
    long long timestamp;
    string company;
    double price;
    int buyer_id;
    int seller_id;

    // Constructor to initialize StockData from a line in the file
    StockData(long long t, string c, double p, int b_id, int s_id)
        : timestamp(t), company(c), price(p), buyer_id(b_id), seller_id(s_id) {}
};

// Function to read data from file into a vector of StockData, only for AAPL
vector<StockData> readData(const string& filename) {
    vector<StockData> data;
    ifstream file(filename);
    string line;

    // Read the file line by line
    while (getline(file, line)) {
        stringstream ss(line);
        long long timestamp;
        string company;
        double price;
        int buyer_id, seller_id;

        // Extract data from each line
        ss >> timestamp >> company >> price >> buyer_id >> seller_id;

        // Only add the data for AAPL
        if (company == "AAPL") {
            data.push_back(StockData(timestamp, company, price, buyer_id, seller_id));
        }
    }
    return data;
}
int num=0;
// Function to write sorted data to a file
void writeData(const string& filename, const vector<StockData>& data) {
    ofstream file(filename);

    // Write each StockData object into the file
    for (const auto& stock : data) {
            file << num << " " << stock.company << " " << stock.price << " "
             << stock.buyer_id << " " << stock.seller_id << endl;
        num++;
        
    }
}

int main() {
    string inputFile = "data/stock_prices.dat";  // Input file name
    string outputFile = "data/sorted_AAPL_stock_prices.dat";  // Output file name for AAPL

    // Read data from the file, only for AAPL
    vector<StockData> stockData = readData(inputFile);

    // Sort the data based on the timestamp using a lambda function
    sort(stockData.begin(), stockData.end(), [](const StockData& a, const StockData& b) {
        return a.timestamp < b.timestamp;
    });

    // Write the sorted data back to a new file
    writeData(outputFile, stockData);

    cout << "Data for AAPL sorted and written to " << outputFile << endl;

    return 0;
}
