#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

const int PORT = 12345;
const int BUFFER_SIZE = 1024;
const int MAX_CONNECTIONS = 5;

vector<string> security_tips = {
    "Используйте двухфакторную аутентификацию для всех важных аккаунтов.",
    "Никогда не используйте один и тот же пароль для разных сервисов.",
    "Регулярно обновляйте программное обеспечение и операционную систему.",
    "Будьте осторожны с письмами от неизвестных отправителей, особенно с вложениями.",
    "Используйте VPN при подключении к публичным Wi-Fi сетям."
};

string generate_password(int length = 12) {
    const string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*()";
    string password;
    srand(time(0));
    for (int i = 0; i < length; ++i) {
        password += chars[rand() % chars.size()];
    }
    return password;
}

string process_command(const string& command) {
    if (command == "GET_TIP") {
        return security_tips[rand() % security_tips.size()];
    } else if (command == "GENERATE_PASSWORD") {
        return generate_password();
    } else if (command == "HELP") {
        return "Доступные команды: GET_TIP, GENERATE_PASSWORD, HELP";
    } else {
        return "Неизвестная команда. Введите HELP для списка команд.";
    }
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Ошибка создания сокета" << endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "Ошибка привязки сокета" << endl;
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, MAX_CONNECTIONS) == -1) {
        cerr << "Ошибка прослушивания" << endl;
        close(server_socket);
        return 1;
    }

    cout << "Сервер запущен и ожидает подключений на порту " << PORT << endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);

        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_size);
        if (client_socket == -1) {
            cerr << "Ошибка принятия соединения" << endl;
            continue;
        }

        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            string command(buffer);
            cout << "Получена команда: " << command << endl;

            string response = process_command(command);
            send(client_socket, response.c_str(), response.size(), 0);
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
