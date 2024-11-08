#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <map>  // Using map for ordered keys
using namespace std;

// Constants for the model
const int NUM_USERS = 1000;               // Number of users
const int NUM_COMPANIES = 5;            // Number of companies (assets)
const int DAYS_IN_MONTH = 30; 
const int day_in_week = 7;            // Number of days in a month
const double RISK_FREE_RATE = 0.05;      // 5% annual risk-free rate
const double VOLATILITY = 0.3;           // 30% annual volatility
const double MATURITY_TIME = 30.0 / 365; // Time to maturity for options (30 days -> ~0.0822 years)
const double ORDER_PROBABILITY = 0.3;    // Probability of a user placing an order on any given day
const double MIN_CONTRACT_PRICE = 1.0;   // Minimum option price
const double SPOT_PRICE_START = 1000.0;  // Starting price for the underlying asset

// Helper function to generate random double in a range
double randomDouble(double min, double max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

// Helper function to generate random integer in a range
int randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// CDF of standard normal distribution
double N(double x) {
    return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
}

// PDF of standard normal distribution
double N_prime(double x) {
    return std::exp(-0.5 * x * x) / std::sqrt(2.0 * M_PI);
}

// Black-Scholes formula for Call options
double blackScholesCall(double spotPrice, double strikePrice, double maturity, double rate, double volatility) {
    double d1 = (log(spotPrice / strikePrice) + (rate + 0.5 * volatility * volatility) * maturity) / (volatility * sqrt(maturity));
    double d2 = d1 - volatility * sqrt(maturity);

    // Option price
    double callPrice = spotPrice * N(d1) - strikePrice * exp(-rate * maturity) * N(d2);

    // Option price can't be lower than MIN_CONTRACT_PRICE
    return std::max(callPrice, MIN_CONTRACT_PRICE);
}

// Black-Scholes formula for Put options
double blackScholesPut(double spotPrice, double strikePrice, double maturity, double rate, double volatility) {
    double d1 = (log(spotPrice / strikePrice) + (rate + 0.5 * volatility * volatility) * maturity) / (volatility * sqrt(maturity));
    double d2 = d1 - volatility * sqrt(maturity);

    // Option price
    double putPrice = strikePrice * exp(-rate * maturity) * N(-d2) - spotPrice * N(-d1);

    // Option price can't be lower than MIN_CONTRACT_PRICE
    return std::max(putPrice, MIN_CONTRACT_PRICE);
}

// Function to simulate the next spot price using Geometric Brownian Motion (GBM)
double simulateSpotPrice(double currentPrice, double volatility, double riskFreeRate, double timeStep) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::normal_distribution<> dis(0, 1);  // Standard normal distribution

    double drift = (riskFreeRate - 0.5 * volatility * volatility) * timeStep;
    double diffusion = volatility * sqrt(timeStep) * dis(gen);

    return currentPrice * exp(drift + diffusion);
}

// Function to calculate expiration date (set to third Friday of each month for simplicity)
std::string calculateExpirationDate(int day) {
     int expirationDay_Future = (day + rand() % 6 + 1); // Add 1 to 7 for weekly, 30 for monthly
    if(expirationDay_Future > day_in_week) {
        expirationDay_Future = expirationDay_Future % day_in_week;
    }
    return to_string(expirationDay_Future);
}

// Function to choose strike price based on the spot price and option type
double chooseStrikePrice(double spotPrice) {
    return round(spotPrice);  // Strike price as rounded spot price
}

// Function to generate random timestamp in HH:MM:SS format
std::string generateRandomTimestamp(int day) {
        int hour_Future = rand() % 6 + 9;  // Business hours from 9 AM to 3 PM
    int minute_Future = rand() % 60;
    int second_Future = rand() % 60;
    int milliseconds_Future = rand() % 1000;

    long long msSinceStart_Future = ((hour_Future - 9) * 3600 + minute_Future * 60 + second_Future) * 1000 + milliseconds_Future;

    // Increase the buffer size to accommodate the formatted string
    char formattedTimestamp_Future[20];  // Larger buffer size to avoid overflow

    // Use snprintf to ensure that the buffer size is respected
    snprintf(formattedTimestamp_Future, sizeof(formattedTimestamp_Future), "%d%08lld", day, msSinceStart_Future);

    return std::string(formattedTimestamp_Future);
}

// Order structure to represent a buy/sell/cancel order
struct Order {
    int orderID;
    int userID;
    int companyID;
    std::string optionType;  // Call or Put
    double spotPrice;
    double strikePrice;
    double optionPrice;
    std::string expirationDate;
    std::string timestamp;
    std::string action;  // "buy", "sell", "cancel"
    int lots;  // Number of lots for each order
};

// Store orders by their ID, using std::map for automatic sorting by orderID
std::map<int, Order> activeOrders;

// Function to process buy order
void processBuyOrder(int orderID, int userID, int companyID, double spotPrice, double strikePrice, double optionPrice, const std::string& expirationDate, const std::string& timestamp, int lots, const std::string& optionType) {
    Order newOrder = {orderID, userID, companyID, optionType, spotPrice, strikePrice, optionPrice, expirationDate, timestamp, "buy", lots};
    activeOrders[orderID] = newOrder;
}

// Function to process sell order
void processSellOrder(int orderID, int userID, int companyID, double spotPrice, double strikePrice, double optionPrice, const std::string& expirationDate, const std::string& timestamp, int lots, const std::string& optionType) {
    Order newOrder = {orderID, userID, companyID, optionType, spotPrice, strikePrice, optionPrice, expirationDate, timestamp, "sell", lots};
    activeOrders[orderID] = newOrder;
}

// Function to cancel an order
void cancelOrder(int orderID) {
    if (activeOrders.find(orderID) != activeOrders.end()) {
        activeOrders[orderID].action = "cancel";  // Mark order as cancelled
    }
}

// Function to generate the option order book and write to CSV
void generateOptionOrderBook(const std::string &filename) {
    std::ofstream file(filename);
    file << "OrderID,UserID,CompanyID,Date,Timestamp,ExpirationDate,OptionType,SpotPrice,StrikePrice,OptionPrice,Action,Lots\n";

    int orderID = 1;
    double spotPrice = SPOT_PRICE_START;  // Starting spot price

    for (int day = 1; day <= DAYS_IN_MONTH; ++day) {
        for (int userID = 1; userID <= NUM_USERS; ++userID) {
            if (randomDouble(0, 1) <= ORDER_PROBABILITY) {
                int companyID = randomInt(1, NUM_COMPANIES);
                std::string expirationDate = calculateExpirationDate(day);  // Random expiration date for each order
                std::string timestamp = generateRandomTimestamp(day);  // Random timestamp

                // Generate spot price and option price based on Black-Scholes
                double strikePrice = chooseStrikePrice(spotPrice);

                // Randomly choose option type (Call or Put)
                std::string optionType = randomDouble(0, 1) <= 0.5 ? "Call" : "Put";

                double optionPrice = (optionType == "Call")
                    ? blackScholesCall(spotPrice, strikePrice, MATURITY_TIME, RISK_FREE_RATE, VOLATILITY)
                    : blackScholesPut(spotPrice, strikePrice, MATURITY_TIME, RISK_FREE_RATE, VOLATILITY);

                // Randomly decide lots between 1 and 10
                int lots = randomInt(1, 10);

                // Process buy or sell orders
                if (randomDouble(0, 1) <= 0.5) {
                    processBuyOrder(orderID++, userID, companyID, spotPrice, strikePrice, optionPrice, expirationDate, timestamp, lots, optionType);
                } else {
                    processSellOrder(orderID++, userID, companyID, spotPrice, strikePrice, optionPrice, expirationDate, timestamp, lots, optionType);
                }
            }
        }
        spotPrice = simulateSpotPrice(spotPrice, VOLATILITY, RISK_FREE_RATE, 1.0);  // Update spot price for the next day
    }

    // Write all orders in ascending order of OrderID
    for (const auto& orderPair : activeOrders) {
        const Order& order = orderPair.second;
        file << order.orderID << ","
             << order.userID << ","
             << order.companyID << ","
             
             << order.timestamp << ","
             << order.expirationDate << ","
             << order.optionType << ","
             << std::fixed << std::setprecision(2) << order.spotPrice << ","
             << std::fixed << std::setprecision(2) << order.strikePrice << ","
             << std::fixed << std::setprecision(2) << order.optionPrice << ","
             << order.action << ","
             << order.lots << "\n";
    }

    file.close();
}

int main() {
    std::string filename = "option_order_book.csv";
    generateOptionOrderBook(filename);

    std::cout << "Option order book generated: " << filename << std::endl;
    return 0;
}
