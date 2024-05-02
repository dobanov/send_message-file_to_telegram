# tg_send_message-file
send message or file to telegram

g++ -o tg_send_message-file tg_send_message-file.cpp -lcurl

This program sends messages or files to Telegram using the provided bot and chat IDs.
Options:
-c <chat_id> : Specify the chat ID to send the message or file to.
-b <bot_id> : Specify the bot ID for authentication.
-m <message> : Specify the message to send.
-f <file_path> : Specify the path to the file to send.
-d : Enable debug mode.
-h : Display this help message.
