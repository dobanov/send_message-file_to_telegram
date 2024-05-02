#include <iostream>
#include <curl/curl.h>
#include <string>

using namespace std;

size_t writeCallback(char* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

void sendTextToTelegram(string botId, string chatId, string message) {
    CURL* curl = curl_easy_init();
    if (curl) {
        string url = "https://api.telegram.org/bot" + botId + "/sendMessage";
        string data = "chat_id=" + chatId + "&text=" + message;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
}

void sendFileToTelegram(string botId, string chatId, string filePath) {
    CURL* curl = curl_easy_init();
    if (curl) {
        struct curl_httppost* formpost = NULL;
        struct curl_httppost* lastptr = NULL;

        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "chat_id", CURLFORM_COPYCONTENTS, chatId.c_str(), CURLFORM_END);
        curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "document", CURLFORM_FILE, filePath.c_str(), CURLFORM_END);

        string url = "https://api.telegram.org/bot" + botId + "/sendDocument";
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        curl_easy_perform(curl);

        curl_formfree(formpost);
        curl_easy_cleanup(curl);
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        cerr << "Usage: " << argv[0] << " -c <chat_id> -b <bot_id> [-m <message>] [-f <file_path>] [-d] [-h]" << endl;
        return 0;
    }

    string botId, chatId, message, filePath;
    bool debugMode = false;

    for (int i = 1; i < argc; i++) {
        string flag = argv[i];
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
            cout << "Help: This program sends messages or files to Telegram using the provided bot and chat IDs." << endl;
            cout << "Options:" << endl;
            cout << "-c <chat_id> : Specify the chat ID to send the message or file to." << endl;
            cout << "-b <bot_id> : Specify the bot ID for authentication." << endl;
            cout << "-m <message> : Specify the message to send." << endl;
            cout << "-f <file_path> : Specify the path to the file to send." << endl;
            cout << "-d : Enable debug mode." << endl;
            cout << "-h : Display this help message." << endl;
            return 0;
        } else {
            cerr << "Invalid option: " << flag << endl;
            cerr << "Usage: " << argv[0] << " -c <chat_id> -b <bot_id> [-m <message>] [-f <file_path>] [-d] [-h]" << endl;
            return 1;
        }
    }

    if (chatId.empty() || botId.empty()) {
        cerr << "Error: Both chat ID and bot ID are required." << endl;
        return 1;
    }

    if (debugMode) {
        cout << "Bot ID: " << botId << endl;
        cout << "Chat ID: " << chatId << endl;
        if (!message.empty()) {
            cout << "Message: " << message << endl;
        }
        if (!filePath.empty()) {
            cout << "File path: " << filePath << endl;
        }
    }

    if (!message.empty()) {
        sendTextToTelegram(botId, chatId, message);
    }

    if (!filePath.empty()) {
        sendFileToTelegram(botId, chatId, filePath);
    }

    return 0;
}