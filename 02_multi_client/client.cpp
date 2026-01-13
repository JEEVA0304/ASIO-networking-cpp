#include <asio.hpp>
#include <iostream>
#include <thread>
#include <string>

using asio::ip::tcp;

void reader(tcp::socket& socket) {
    try {
        asio::streambuf buffer;
        while (true) {
            asio::read_until(socket, buffer, '\n');
            std::istream is(&buffer);
            std::string msg;
            std::getline(is, msg);
            std::cout << msg << "\n";
        }
    } catch (...) {
        std::cout << "Disconnected from server.\n";
    }
}

int main() {
    try {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        std::cout << "Enter your name: ";
        std::string name;
        std::getline(std::cin, name);
        asio::write(socket, asio::buffer(name + "\n"));

        std::thread t(reader, std::ref(socket));

        while (true) {
            std::string msg;
            std::getline(std::cin, msg);
            if (!msg.empty()) {
                asio::write(socket, asio::buffer(msg + "\n"));
            }
        }

        t.join();
    }
    catch (std::exception& e) {
        std::cerr << "Client error: " << e.what() << "\n";
    }

    return 0;
}
