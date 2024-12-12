#include <iostream>
#include <curl/curl.h>
#include <string>
#include <vector>
#include <sstream>

// Callback function for writing response data
size_t writeCallback(char* contents, size_t size, size_t nmemb, void* userp) {
    std::string* response = static_cast<std::string*>(userp);
    size_t totalSize = size * nmemb;
    response->append(contents, totalSize);
    return totalSize;
}

// Function to send text message to multiple Telegram chat IDs
void sendTextToTelegram(const std::string& botId, const std::vector<std::string>& chatIds, const std::string& message, bool debugMode) {
    CURL* curl = curl_easy_init();
    if (curl) {
        for (const auto& chatId : chatIds) {
            std::string url = "https://api.telegram.org/bot" + botId + "/sendMessage";
            std::string data = "chat_id=" + chatId + "&text=" + message;

            std::string responseBuffer;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

            CURLcode res = curl_easy_perform(curl);

            if (debugMode) {
                if (res != CURLE_OK) {
                    std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
                } else {
                    std::cout << "Response: " << responseBuffer << std::endl;
                }
                std::cout << "Sent message to chat ID " << chatId << ": " << message << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }
}

// Function to send a file with a message to multiple Telegram chat IDs
void sendFileToTelegram(const std::string& botId, const std::vector<std::string>& chatIds, const std::string& filePath, const std::string& message, bool debugMode) {
    CURL* curl = curl_easy_init();
    if (curl) {
        for (const auto& chatId : chatIds) {
            struct curl_httppost* formpost = NULL;
            struct curl_httppost* lastptr = NULL;

            curl_formadd(&formpost, &lastptr,
                         CURLFORM_COPYNAME, "chat_id",
                         CURLFORM_COPYCONTENTS, chatId.c_str(), CURLFORM_END);
            curl_formadd(&formpost, &lastptr,
                         CURLFORM_COPYNAME, "document",
                         CURLFORM_FILE, filePath.c_str(), CURLFORM_END);
            if (!message.empty()) {
                curl_formadd(&formpost, &lastptr,
                             CURLFORM_COPYNAME, "caption",
                             CURLFORM_COPYCONTENTS, message.c_str(), CURLFORM_END);
            }

            std::string url = "https://api.telegram.org/bot" + botId + "/sendDocument";
            std::string responseBuffer;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

            CURLcode res = curl_easy_perform(curl);

            curl_formfree(formpost);

            if (debugMode) {
                if (res != CURLE_OK) {
                    std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
                } else {
                    std::cout << "Response: " << responseBuffer << std::endl;
                }
                std::cout << "Sent file to chat ID " << chatId << " with message: " << message << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }
}

// Function to split a string by a delimiter and return a vector of strings
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " -c <chat_id1,chat_id2,...> -b <bot_id> [-m <message>] [-f <file_path>] [-d] [-h]" << std::endl;
        return 0;
    }

    std::string botId, chatIdsStr, message, filePath;
    bool debugMode = false;

    for (int i = 1; i < argc; i++) {
        std::string flag = argv[i];
        if (flag == "-c" && i + 1 < argc) {
            chatIdsStr = argv[++i];
        } else if (flag == "-b" && i + 1 < argc) {
            botId = argv[++i];
        } else if (flag == "-m" && i + 1 < argc) {
            message = argv[++i];
        } else if (flag == "-f" && i + 1 < argc) {
            filePath = argv[++i];
        } else if (flag == "-d") {
            debugMode = true;
        } else if (flag == "-h") {
            std::cout << "Help: This program sends messages or files to Telegram using the provided bot and chat IDs." << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "-c <chat_id1,chat_id2,...> : Specify the chat IDs to send the message or file to, separated by commas." << std::endl;
            std::cout << "-b <bot_id> : Specify the bot ID for authentication." << std::endl;
            std::cout << "-m <message> : Specify the message to send." << std::endl;
            std::cout << "-f <file_path> : Specify the path to the file to send." << std::endl;
            std::cout << "-d : Enable debug mode." << std::endl;
            std::cout << "-h : Display this help message." << std::endl;
            return 0;
        } else {
            std::cerr << "Invalid option: " << flag << std::endl;
            std::cerr << "Usage: " << argv[0] << " -c <chat_id1,chat_id2,...> -b <bot_id> [-m <message>] [-f <file_path>] [-d] [-h]" << std::endl;
            return 1;
        }
    }

    if (chatIdsStr.empty() || botId.empty()) {
        std::cerr << "Error: Both chat ID and bot ID are required." << std::endl;
        return 1;
    }

    std::vector<std::string> chatIds = split(chatIdsStr, ',');

    if (debugMode) {
        std::cout << "Bot ID: " << botId << std::endl;
        std::cout << "Chat IDs: ";
        for (const auto& chatId : chatIds) {
            std::cout << chatId << " ";
        }
        std::cout << std::endl;
        if (!message.empty()) {
            std::cout << "Message: " << message << std::endl;
        }
        if (!filePath.empty()) {
            std::cout << "File path: " << filePath << std::endl;
        }
    }

    if (!message.empty()) {
        sendTextToTelegram(botId, chatIds, message, debugMode);
    }

    if (!filePath.empty()) {
        sendFileToTelegram(botId, chatIds, filePath, message, debugMode);
    }

    return 0;
}
