#include <random>
#include <chrono>
#include "server.h"

std::vector<double> generateRandomData(double min, double max, int count) {
    // Use random device for seed
    std::random_device rd;
    // Mersenne Twister random number generator
    std::mt19937 gen(rd());
    // Distribution for the specified range
    std::uniform_real_distribution<double> dis(min, max);

    std::vector<double> data;
    for (int i = 0; i < count; i++) {
        data.push_back(dis(gen));
    }
    return data;
}

int main() {
    try {
        const int DATA_POINTS = 24; // 24 hours of data

        // Generate random data for each sensor
        // Temperature between 15°C and 30°C
        auto temperature = generateRandomData(15.0, 30.0, DATA_POINTS);
        
        // Humidity between 30% and 70%
        auto humidity = generateRandomData(30.0, 70.0, DATA_POINTS);
        
        // Sunlight between 0 and 1000 lux
        auto sunlight = generateRandomData(0.0, 1000.0, DATA_POINTS);

        std::cout << "Generated random sensor data for " << DATA_POINTS << " hours\n";
        std::cout << "Temperature range: 15°C to 30°C\n";
        std::cout << "Humidity range: 30% to 70%\n";
        std::cout << "Sunlight range: 0 to 1000 lux\n\n";
        
        // Create and start the web server
        WebServer server;
        std::cout << "Starting web server...\n";
        server.displaySensorData(temperature, humidity, sunlight);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}