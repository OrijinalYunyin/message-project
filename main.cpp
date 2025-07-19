
#include <zmq.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

int main() {
    //  Prepare our context and sockets
    zmq::context_t context(1);

    // Socket to talk to clients (publish messages)
    zmq::socket_t publisher(context, ZMQ_PUB);
    const std::string pub_port = "5556";
    try {
        publisher.bind("tcp://*:" + pub_port);
        std::cout << "Server (Publisher) is running on port: " << pub_port << std::endl;
    } catch (const zmq::error_t& e) {
        std::cerr << "Error binding publisher socket: " << e.what() << std::endl;
        return 1;
    }

    // Socket to receive messages from clients
    zmq::socket_t receiver(context, ZMQ_PULL);
    const std::string receiver_port = "5557";
    try {
        receiver.bind("tcp://*:" + receiver_port);
        std::cout << "Server is listening for messages from clients on port: " << receiver_port << std::endl;
    } catch (const zmq::error_t& e) {
        std::cerr << "Error binding receiver socket: " << e.what() << std::endl;
        return 1;
    }


    // In a real application, you might have a more sophisticated way
    // to handle the "slow joiner" syndrome. Here, we just sleep a bit.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Waiting for messages..." << std::endl;

    while (true) {
        try {
            zmq::message_t received_msg;
            // Wait for a message from any client
            auto result = receiver.recv(received_msg, zmq::recv_flags::none);
            if(result.has_value()){
                std::string message_str = received_msg.to_string();
                std::cout << "Received: " << message_str << std::endl;

                // Broadcast the received message to all subscribers
                std::cout << "Broadcasting: " << message_str << std::endl;
                publisher.send(zmq::buffer(message_str), zmq::send_flags::none);
            }
        } catch (const zmq::error_t& e) {
            std::cerr << "An error occurred: " << e.what() << std::endl;
            break;
        }
    }

    // The context will automatically close the sockets when it's destroyed.
    return 0;
}
