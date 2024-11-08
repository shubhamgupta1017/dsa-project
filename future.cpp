#ifndef FUTURE_CPP
#define FUTURE_CPP
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <unordered_map>
#include "spot.cpp"  // Assuming this file contains the 'getspotprice' definition

using namespace std;

// Constants
const int NUM_USERS_FUTURE = 100;
const int NUM_COMPANIES_FUTURE = 3;
const int DAYS_IN_WEEK_FUTURE = 1;
const double RISK_FREE_RATE_FUTURE = 0.05; // 5% annual risk-free rate
const double DIVIDEND_YIELD_FUTURE = 0.02; // 2% annual dividend yield
const double VOLATILITY_ADJUSTMENT_FUTURE = 0.01; // 1% adjustment for volatility impact

// Struct to hold each order's data
struct OrderFuture {
    int orderID_Future;
    int userID_Future;
    int companyID_Future;
    std::string date_Future;
    std::string timestamp_Future;
    std::string expirationDate_Future;
    double spotPrice_Future;
    double futurePrice_Future;
    std::string orderType_Future; // "Buy" or "Sell"
};

// Function to calculate future price with real trading factors
double calculateFuturePriceFuture(double spotPrice_Future, double timeToExpiration_Future, double rate_Future, double dividendYield_Future, double volatilityAdjustment_Future) {
    return spotPrice_Future * exp((rate_Future - dividendYield_Future + volatilityAdjustment_Future) * timeToExpiration_Future);
}

// Function to calculate expiration date (weekly or monthly)
std::string calculateExpirationDateFuture(int orderDay_Future) {
    int expirationDay_Future = (orderDay_Future + rand() % 2 + 1); // Add 1 to 7 for weekly, 30 for monthly
    if(expirationDay_Future > DAYS_IN_WEEK_FUTURE) {
        expirationDay_Future = expirationDay_Future % DAYS_IN_WEEK_FUTURE;
    }
    return to_string(expirationDay_Future);
}

// Function to generate timestamp in 9-digit format
std::string generateTimestampWithDayPrefixFuture(int day_Future) {
    int hour_Future = rand() % 6 + 9;  // Business hours from 9 AM to 3 PM
    int minute_Future = rand() % 60;
    int second_Future = rand() % 60;
    int milliseconds_Future = rand() % 1000;

    long long msSinceStart_Future = ((hour_Future - 9) * 3600 + minute_Future * 60 + second_Future) * 1000 + milliseconds_Future;

    // Increase the buffer size to accommodate the formatted string
    char formattedTimestamp_Future[20];  // Larger buffer size to avoid overflow

    // Use snprintf to ensure that the buffer size is respected
    snprintf(formattedTimestamp_Future, sizeof(formattedTimestamp_Future), "%d%08lld", day_Future, msSinceStart_Future);

    return std::string(formattedTimestamp_Future);
}

// Function to generate the futures trading order book and save it to a file
void generateFuturesTradingOrderBookFuture(const std::string& filename_Future, unordered_map<long long, vector<double>>& getspotprice) {
    std::ofstream file_Future(filename_Future);
    file_Future << "OrderID_Future,UserID_Future,CompanyID_Future,Date_Future,Timestamp_Future,ExpirationDate_Future,SpotPrice_Future,FuturePrice_Future,OrderType_Future\n";

    int orderID_Future = 1;

    std::vector<OrderFuture> orders_Future;

    for (int day_Future = 1; day_Future <= DAYS_IN_WEEK_FUTURE; ++day_Future) {
        std::vector<OrderFuture> dailyOrders_Future;

        for (int userID_Future = 1; userID_Future <= NUM_USERS_FUTURE; ++userID_Future) {
            // Randomly decide if the user will place an order today
            if (rand() % 2 == 0) continue; // 50% chance to skip placing an order
            // Generate order details
            std::string expirationDate_Future = calculateExpirationDateFuture(day_Future);
            std::string timestamp_Future = generateTimestampWithDayPrefixFuture(day_Future);
            long long time_future = stoll(timestamp_Future);
           
            string date_Future = to_string(day_Future);

            // Randomly select a company and calculate spot price
            int companyID_Future = rand() % NUM_COMPANIES_FUTURE;  // Assuming company IDs are 0-based for vector indexing

            // Access the spot price using the timestamp and company ID
           
                double spotPrice_Future = getspotprice[time_future][companyID_Future];
                
                // Calculate future price with weekly or monthly expiration
                bool isWeekly_Future = (rand() % 2 == 0);
                double timeToExpiration_Future = isWeekly_Future ? 1.0 / 52 : 1.0 / 12; // 1 week or 1 month
                double futurePrice_Future = calculateFuturePriceFuture(spotPrice_Future, timeToExpiration_Future, RISK_FREE_RATE_FUTURE, DIVIDEND_YIELD_FUTURE, VOLATILITY_ADJUSTMENT_FUTURE);

                // Randomly decide if the order is a "Buy" or "Sell"
                std::string orderType_Future = (rand() % 3 == 0) ? "Buy" : (rand() % 2 == 0) ? "Sell" : "Cancel";


                // Store the order in the daily list
                dailyOrders_Future.push_back({orderID_Future++, userID_Future, companyID_Future + 1, date_Future, timestamp_Future, expirationDate_Future, spotPrice_Future, futurePrice_Future, orderType_Future});
          
        }

        // Sort daily orders by timestamp
        std::sort(dailyOrders_Future.begin(), dailyOrders_Future.end(), [](const OrderFuture &a, const OrderFuture &b) {
            return a.timestamp_Future < b.timestamp_Future;
        });

        // Append daily orders to the main orders vector
        orders_Future.insert(orders_Future.end(), dailyOrders_Future.begin(), dailyOrders_Future.end());
    }

    // Write all sorted orders to the CSV file
    for (const auto &order_Future : orders_Future) {
        file_Future << order_Future.orderID_Future << ","
                    << order_Future.userID_Future << ","
                    << order_Future.companyID_Future << ","
                    << order_Future.date_Future << ","
                    << order_Future.timestamp_Future << ","
                    << order_Future.expirationDate_Future << ","
                    << std::fixed << std::setprecision(2) << order_Future.spotPrice_Future << ","
                    << std::fixed << std::setprecision(2) << order_Future.futurePrice_Future << ","
                    << order_Future.orderType_Future << "\n";
    }

    file_Future.close();
    std::cout << "Futures trading order book generated successfully." << std::endl;
}

#endif
