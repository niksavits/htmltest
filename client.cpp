#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const string SERVER_IP = "127.0.0.1";
const int PORT = 12345;
const int BUFFER_SIZE = 1024;

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации Winsock" << endl;
        return 1;
    }

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());
    server_addr.sin_port = htons(PORT);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Ошибка подключения к серверу" << endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    cout << "Подключено к серверу " << SERVER_IP << ":" << PORT << endl;
    cout << "Доступные команды:" << endl;
    cout << "  GET_TIP - получить случайный совет по безопасности" << endl;
    cout << "  GENERATE_PASSWORD - сгенерировать надежный пароль" << endl;
    cout << "  HELP - показать список команд" << endl;
    cout << "  EXIT - завершить работу" << endl;

    while (true) {
        cout << "\nВведите команду: ";
        string command;
        getline(cin, command);

        if (command == "EXIT") {
            break;
        }

        send(client_socket, command.c_str(), command.size(), 0);

        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            cout << "Ответ сервера: " << buffer << endl;
        }
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}