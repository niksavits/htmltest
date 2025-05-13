#include <iostream>
#include <string>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

const string SERVER_IP = "192.168.1.100";
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
        cerr << "Ошибка создания сокета: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        cerr << "Ошибка подключения к серверу: " << WSAGetLastError() << endl;
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

        if (send(client_socket, command.c_str(), command.size(), 0) == SOCKET_ERROR) {
            cerr << "Ошибка отправки данных: " << WSAGetLastError() << endl;
            break;
        }

        char buffer[BUFFER_SIZE];
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            cout << "Ответ сервера: " << buffer << endl;
        } else if (bytes_received == 0) {
            cout << "Сервер закрыл соединение" << endl;
            break;
        } else {
            cerr << "Ошибка приема данных: " << WSAGetLastError() << endl;
            break;
        }
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}
