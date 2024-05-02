#include <iostream>
#include <curl/curl.h>
#include <string>

size_t writeCallback(char* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

void sendTextToTelegram(const std::string& botId, const std::string& chatId, const std::string& message, bool debugMode) {
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string url = "https://api.telegram.org/bot" + botId + "/sendMessage";
        std::string data = "chat_id=" + chatId + "&text=" + message;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (debugMode) {
            std::cout << "Text message sent successfully." << std::endl;
        }
    }
}

void sendFileToTelegram(const std::string& botId, const std::string& chatId, const std::string& filePath, bool debugMode) {
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr = NULL;

        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "chat_id", CURLFORM_COPYCONTENTS, chatId.c_str(), CURLFORM_END);
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "document", CURLFORM_FILE, filePath.c_str(), CURLFORM_END);

        std::string url = "https://api.telegram.org/bot" + botId + "/sendDocument";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_perform(curl);

        curl_formfree(formpost);
        curl_easy_cleanup(curl);

        if (debugMode) {
            std::cout << "File sent successfully." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " -c <chat_id> -b <bot_id> [-m <message>] [-f <file_path>] [-d] [-h]" << std::endl;
        return 0;
    }

    std::string botId, chatId, message, filePath;
    bool debugMode = false;

    for (int i = 1; i < argc; i++) {
        std::string flag = argv[i];
        if (flag == "-c" && i + 1 < argc) {
            chatId = argv[++i];
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
            std::cout << "-c <chat_id> : Specify the chat ID to send the message or file to." << std::endl;
            std::cout << "-b <bot_id> : Specify the bot ID for authentication." << std::endl;
            std::cout << "-m <message> : Specify the message to send." << std::endl;
            std::cout << "-f <file_path> : Specify the path to the file to send." << std::endl;
            std::cout << "-d : Enable debug mode." << std::endl;
            std::cout << "-h : Display this help message." << std::endl;
            return 0;
        } else {
            std::cerr << "Invalid option: " << flag << std::endl;
            std::cerr << "Usage: " << argv[0] << " -c <chat_id> -b <bot_id> [-m <message>] [-f <file_path>] [-d] [-h]" << std::endl;
            return 1;
        }
    }

    if (chatId.empty() || botId.empty()) {
        std::cerr << "Error: Both chat ID and bot ID are required." << std::endl;
        return 1;
    }

    if (debugMode) {
        std::cout << "Bot ID: " << botId << std::endl;
        std::cout << "Chat ID: " << chatId << std::endl;
        if (!message.empty()) {
            std::cout << "Message: " << message << std::endl;
        }
        if (!filePath.empty()) {
            std::cout << "File path: " << filePath << std::endl;
        }
    }

    if (!message.empty()) {
        sendTextToTelegram(botId, chatId, message, debugMode);
    }

    if (!filePath.empty()) {
        sendFileToTelegram(botId, chatId, filePath, debugMode);
    }

    return 0;
}
