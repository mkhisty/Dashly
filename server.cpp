#include "server.h"

WebServer::WebServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
    loadTemplate();
}

WebServer::~WebServer() {
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
}

void WebServer::loadTemplate() {
    std::ifstream file("templates/standard_graph.html");
    if (!file.is_open()) {
        throw std::runtime_error("Could not open template file");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    htmlTemplate = buffer.str();
}

std::string WebServer::generateJavaScriptArrays(
    const std::vector<double>& temperature,
    const std::vector<double>& humidity,
    const std::vector<double>& sunlight) {
    
    std::stringstream js;
    
    js << "const hours = Array.from({ length: " << temperature.size() 
       << " }, (_, i) => `Hour ${i + 1}`);\n";

    js << "const temperatureData = [";
    for (size_t i = 0; i < temperature.size(); ++i) {
        js << temperature[i];
        if (i < temperature.size() - 1) js << ", ";
    }
    js << "];\n";

    js << "const humidityData = [";
    for (size_t i = 0; i < humidity.size(); ++i) {
        js << humidity[i];
        if (i < humidity.size() - 1) js << ", ";
    }
    js << "];\n";

    js << "const sunlightData = [";
    for (size_t i = 0; i < sunlight.size(); ++i) {
        js << sunlight[i];
        if (i < sunlight.size() - 1) js << ", ";
    }
    js << "];\n";

    return js.str();
}

void WebServer::displaySensorData(
    const std::vector<double>& temperature,
    const std::vector<double>& humidity,
    const std::vector<double>& sunlight) {
    
    if (temperature.size() != humidity.size() || humidity.size() != sunlight.size()) {
        throw std::runtime_error("All sensor data arrays must have the same size");
    }

    std::string jsArrays = generateJavaScriptArrays(temperature, humidity, sunlight);

    size_t scriptPos = htmlTemplate.find("// Sample data");
    if (scriptPos == std::string::npos) {
        throw std::runtime_error("Could not find sample data marker in template");
    }

    size_t lineEnd = htmlTemplate.find('\n', scriptPos);
    size_t nextLineStart = lineEnd + 1;
    size_t sectionEnd = htmlTemplate.find("// Temperature Chart");

    currentResponse = htmlTemplate.substr(0, nextLineStart) + 
                     jsArrays + 
                     htmlTemplate.substr(sectionEnd);

    start();
}

void WebServer::start() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        throw std::runtime_error("setsockopt failed");
    }

    sockaddr_in serverAddr;
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        throw std::runtime_error("Bind failed with error: " + std::to_string(WSAGetLastError()));
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server listening on port " << PORT << std::endl;
    std::cout << "Open your web browser and navigate to http://localhost:" << PORT << std::endl;

    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket != INVALID_SOCKET) {
        handleClient(clientSocket);
    }
}

void WebServer::handleClient(SOCKET clientSocket) {
    std::vector<char> buffer(BUFFER_SIZE);
    
    int bytesReceived = recv(clientSocket, buffer.data(), buffer.size(), 0);
    if (bytesReceived > 0) {
        std::string fullResponse = "HTTP/1.1 200 OK\r\n"
                                 "Content-Type: text/html; charset=utf-8\r\n"
                                 "Connection: close\r\n"
                                 "\r\n" +
                                 currentResponse;
        
        int totalSent = 0;
        int remainingBytes = fullResponse.length();
        
        while (remainingBytes > 0) {
            int bytesSent = send(clientSocket, 
                               fullResponse.c_str() + totalSent, 
                               remainingBytes, 
                               0);
                               
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
                break;
            }
            
            totalSent += bytesSent;
            remainingBytes -= bytesSent;
        }
        
        shutdown(clientSocket, SD_SEND);
    }
    
    closesocket(clientSocket);
}