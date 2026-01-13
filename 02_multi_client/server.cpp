#include <asio.hpp>
#include <iostream>
#include <set>
#include <memory>
#include <string>

using asio::ip::tcp;

class Session;
std::set<std::shared_ptr<Session>> sessions;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read_name();
    }

    void deliver(const std::string& msg) {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(msg + "\n"),
            [this, self](std::error_code /*ec*/, std::size_t) {});
    }

private:
    void do_read_name() {
        auto self(shared_from_this());
        asio::async_read_until(socket_, buffer_, '\n',
            [this, self](std::error_code ec, std::size_t) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::getline(is, name_);
                    sessions.insert(self);
                    std::string join_msg = name_ + " joined the chat.";
                    broadcast(join_msg);
                    do_read_message();
                }
            });
    }

    void do_read_message() {
        auto self(shared_from_this());
        asio::async_read_until(socket_, buffer_, '\n',
            [this, self](std::error_code ec, std::size_t) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string msg;
                    std::getline(is, msg);
                    if (!msg.empty()) {
                        std::string full_msg = "[" + name_ + "]: " + msg;
                        broadcast(full_msg);
                    }
                    do_read_message();
                } else {
                    sessions.erase(self);
                    broadcast(name_ + " left the chat.");
                }
            });
    }

    void broadcast(const std::string& msg) {
        for (auto& s : sessions) {
            if (s.get() != this) {
                s->deliver(msg);
            }
        }
    }

    tcp::socket socket_;
    asio::streambuf buffer_;
    std::string name_;
};
 
int main() {
    try {
        asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 12345));
        std::cout << "Chat server started on port 12345\n";

        std::function<void()> do_accept;
        do_accept = [&]() {
            acceptor.async_accept(
                [&](std::error_code ec, tcp::socket socket) {
                    if (!ec) {
                        std::make_shared<Session>(std::move(socket))->start();
                    }
                    do_accept();
                });
        };

        do_accept();
        io_context.run();
    }
    catch (std::exception& e) {
        std::cerr << "Server error: " << e.what() << "\n";
    }

    return 0;
}
