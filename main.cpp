#include <string>
#include <chrono>
#include <thread>
#include <iostream>

#include <zmq.hpp>

int main()
{
    std::cout << "ALICI MISIN VERICI MI?" << std::endl;
    std::cout << "ALICI  : 1"<< std::endl;
    std::cout << "VERICI : 2"<< std::endl;
    using namespace std::chrono_literals;

    int x;

    std::cin >> x;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string message;
    std::getline(std::cin, message);

    if (1 == x)
    {
        // initialize the zmq context with a single IO thread
        zmq::context_t context{1};

        // construct a REP (reply) socket and bind to interface
        zmq::socket_t socket{context, zmq::socket_type::rep};
        socket.bind("tcp://*:5555");

        // prepare some static data for responses
        const std::string data{"World"};

        for (;;)
        {
            zmq::message_t request;

            // receive a request from client
            socket.recv(request, zmq::recv_flags::none);
            std::cout << "Received " << request.to_string() << std::endl;

            // simulate work
            std::this_thread::sleep_for(1s);

            // send the reply to the client
            socket.send(zmq::buffer(data), zmq::send_flags::none);
        }
    }

    {
        mq::context_t context{1};

        // construct a REQ (request) socket and connect to interface
        zmq::socket_t socket{context, zmq::socket_type::req};
        socket.connect("tcp://localhost:5555");

        // set up some static data to send
        const std::string data{"Hello"};

        for (auto request_num = 0; request_num < 10; ++request_num)
        {
            // send the request message
            std::cout << "Sending Hello " << request_num << "..." << std::endl;
            socket.send(zmq::buffer(data), zmq::send_flags::none);

            // wait for reply from server
            zmq::message_t reply{};
            socket.recv(reply, zmq::recv_flags::none);

            std::cout << "Received " << reply.to_string();
            std::cout << " (" << request_num << ")";
            std::cout << std::endl;
        }

    }

    return 0;
}
