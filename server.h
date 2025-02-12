#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

class WebServer {
private:
    SOCKET serverSocket;
    const int PORT = 8080;
    const int BUFFER_SIZE = 4096;
    std::string htmlTemplate;
    std::string currentResponse;

    void loadTemplate();
    std::string generateJavaScriptArrays(
        const std::vector<double>& temperature,
        const std::vector<double>& humidity,
        const std::vector<double>& sunlight);
    void handleClient(SOCKET clientSocket);
    void start();

public:
    WebServer();
    ~WebServer();
    void displaySensorData(
        const std::vector<double>& temperature,
        const std::vector<double>& humidity,
        const std::vector<double>& sunlight);
};

#endif 