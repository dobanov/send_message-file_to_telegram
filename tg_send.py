import requests
import argparse

def send_text_to_telegram(bot_id, chat_ids, message, debug_mode):
    url = f"https://api.telegram.org/bot{bot_id}/sendMessage"
    for chat_id in chat_ids:
        data = {"chat_id": chat_id, "text": message}
        response = requests.post(url, data=data)
        if debug_mode:
            print(f"Sent message to chat ID {chat_id}: {message}")
            print(f"Response: {response.text}")

def send_file_to_telegram(bot_id, chat_ids, file_path, debug_mode):
    url = f"https://api.telegram.org/bot{bot_id}/sendDocument"
    for chat_id in chat_ids:
        files = {"document": open(file_path, "rb")}
        data = {"chat_id": chat_id}
        response = requests.post(url, files=files, data=data)
        if debug_mode:
            print(f"Sent file to chat ID {chat_id}: {file_path}")
            print(f"Response: {response.text}")

def main():
    parser = argparse.ArgumentParser(description="Send messages or files to Telegram using the provided bot and chat IDs.")
    parser.add_argument("-c", "--chat_ids", required=True, help="Specify the chat IDs to send the message or file to, separated by commas.")
    parser.add_argument("-b", "--bot_id", required=True, help="Specify the bot ID for authentication.")
    parser.add_argument("-m", "--message", help="Specify the message to send.")
    parser.add_argument("-f", "--file_path", help="Specify the path to the file to send.")
    parser.add_argument("-d", "--debug", action="store_true", help="Enable debug mode.")
    args = parser.parse_args()

    chat_ids = args.chat_ids.split(",")
    bot_id = args.bot_id
    message = args.message
    file_path = args.file_path
    debug_mode = args.debug

    if debug_mode:
        print(f"Bot ID: {bot_id}")
        print(f"Chat IDs: {', '.join(chat_ids)}")
        if message:
            print(f"Message: {message}")
        if file_path:
            print(f"File path: {file_path}")

    if message:
        send_text_to_telegram(bot_id, chat_ids, message, debug_mode)
    
    if file_path:
        send_file_to_telegram(bot_id, chat_ids, file_path, debug_mode)

if __name__ == "__main__":
    main()
