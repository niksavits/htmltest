#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <regex>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

// Генерация случайного пароля
string generate_password(size_t length = 16) {
    static const string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "!@#$%^&*()";

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<> distribution(0, chars.size() - 1);

    string password;
    for (size_t i = 0; i < length; ++i) {
        password += chars[distribution(generator)];
    }
    return password;
}

// Проверка сложности пароля
string check_password_strength(const string& password) {
    int strength = 0;
    string feedback;

    if (password.length() >= 8) strength += 1;
    else feedback += "Пароль слишком короткий (минимум 8 символов)\n";

    if (regex_search(password, regex("[a-z]"))) strength += 1;
    else feedback += "Добавьте строчные буквы\n";

    if (regex_search(password, regex("[A-Z]"))) strength += 1;
    else feedback += "Добавьте заглавные буквы\n";

    if (regex_search(password, regex("[0-9]"))) strength += 1;
    else feedback += "Добавьте цифры\n";

    if (regex_search(password, regex("[^a-zA-Z0-9]"))) strength += 1;
    else feedback += "Добавьте специальные символы\n";

    int strength_percent = strength * 20;
    string level;

    if (strength_percent < 40) level = "Слабый";
    else if (strength_percent < 70) level = "Средний";
    else level = "Сильный";

    return "{\"strength\":" + to_string(strength_percent) +
           ",\"level\":\"" + level + "\"" +
           ",\"feedback\":\"" + feedback + "\"}";
}

// Обработка команд
string process_command(const string& command) {
    if (command == "GENERATE_PASSWORD") {
        return "PASSWORD:" + generate_password();
    }
    else if (command.find("CHECK_PASSWORD:") == 0) {
        string password = command.substr(15);
        return check_password_strength(password);
    }
    else if (command == "GET_QUIZ") {
        return "QUIZ:{\"questions\":["
               "{\"question\":\"Что такое фишинг?\",\"options\":[\"Мошенническая попытка получить личные данные\",\"Вид компьютерного вируса\",\"Способ шифрования данных\"],\"correct\":0},"
               "{\"question\":\"Какой пароль самый безопасный?\",\"options\":[\"123456\",\"qwerty\",\"J8#kL92!mNx$\"],\"correct\":2},"
               "{\"question\":\"Что делать с подозрительным письмом?\",\"options\":[\"Открыть вложение\",\"Не открывать и удалить\",\"Переслать другу\"],\"correct\":1}"
               "]}";
    }
    return "ERROR:Unknown command";
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Создание сокета
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Установка опций сокета
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(1234);

    // Привязка сокета к порту 1234
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Ожидание подключений
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "Сервер запущен. Ожидание подключений..." << endl;

    while (true) {
        // Принятие нового подключения
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // Чтение команды от клиента
        int valread = read(new_socket, buffer, 1024);
        string command(buffer, valread);
        command.erase(command.find_last_not_of("\r\n") + 1);

        cout << "Получена команда: " << command << endl;

        // Обработка команды
        string response = process_command(command) + "\n";

        // Отправка ответа клиенту
        send(new_socket, response.c_str(), response.size(), 0);

        // Закрытие соединения
        close(new_socket);
    }

    return 0;
}
