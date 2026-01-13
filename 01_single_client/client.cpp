#include <asio.hpp>
#include <iostream>

using asio::ip::tcp;

int main() {
    try {
        asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "12345");
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        std::cout << "Connected to server.\n";

        for (;;) {
            std::cout << "Enter message: ";
            std::string msg;
            std::getline(std::cin, msg);

            if (msg.empty()) continue;

            asio::write(socket, asio::buffer(msg));

            char reply[1024];
            size_t reply_length = asio::read(socket, asio::buffer(reply, msg.length()));
            std::cout << "Server replied: ";
            std::cout.write(reply, reply_length);
            std::cout << "\n";
        }
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
