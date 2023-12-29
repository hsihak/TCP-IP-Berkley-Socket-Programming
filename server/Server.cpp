/******************************************************************
 * Assignment 3 TCP/IP Lab - Course CSCN72020 - Sec3               *
 * Date: December, 2023                                            *
 * By: Sudhan Dahake, Hangsihak Sin, Michelle Gordon               *
 * Description: Creating the server for a client/server application*
 *              of a discussion board                              *
 * Professor: Monti Sachdeva                                       *
 *******************************************************************/

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <fstream>

#include "protocol.h"
#include "File.h"

using namespace std;

int main(void)
{

    // Creating a socket by defining the Socket() function
    // Using TCP as the transport layer service
    int ServerSocket;

    ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET means IPV4 address is assigned to the socket.
    // SOCK_STREAM determines that the socket is of type stream socket. IPPROTO_TCP determines the type of protocol which is TCP.

    if (ServerSocket == -1)
    {
        return 0;
    }

    // display a message to indicate success
    cout << "Server Socket Created" << endl;

    // Binding the server application to accept connection from any IP address (INADDER_ANY)
    // and link the server to a pre-selected port number
    sockaddr_in SvrAddr;
    SvrAddr.sin_family = AF_INET; // AF_INET means IPV4 address is assigned to the socket.
    SvrAddr.sin_addr.s_addr = INADDR_ANY;
    SvrAddr.sin_port = htons(27000); // The server application has the port number of 27000

    // close the ServerSocket if the bind function returns -1
    if (bind(ServerSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == -1) // (struct sockaddr *)&SvrAddr: contains server IP address and port number
    {
        close(ServerSocket);
        cout << "ERROR: Failed to bind ServerSocket" << std::endl;
        return 0;
    }

    // printing out a message if the process is succesful
    cout << "Successfully Binded" << endl;

    // Configure the socket to listen to incoming request from client (SYN request) in order to start the three-way handshake
    // close the server socket if the listen function returns -1
    if (listen(ServerSocket, 1) == -1)
    { // setting the backlog que (que of pending connecitons) to 1 -> maximum number of clients that can be queued
        close(ServerSocket);
        cout << "ERROR: listen failed to configure ServerSocket" << std::endl;
        return 0;
    }

    // printing out a message if the process is succesful
    cout << "Server is listening" << endl;

    // Complete the handshaking process and provide the conneciton with a unique socket and port number (accept the connection)
    int ConnectionSocket;
    ConnectionSocket = -1;
    if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == -1)
    {
        close(ServerSocket);
        return 0;
    }

    cout << "Connection accepted and server is now connected to the client" << endl;

    while (true)
    {
        Packet receivedPacket;

        // Receive the packet from the client
        int recvSize = recv(ConnectionSocket, (char *)&receivedPacket, sizeof(Packet), 0);

        receivedPacket.payload[strlen(receivedPacket.payload) - 1] = '\0';

        if (recvSize < 0)
        {
            std::cout << "Receiving failed" << std::endl;
            close(ConnectionSocket);
            close(ServerSocket);
            return 0;
        }

        std::vector<Data> posts;
        std::vector<Data> existingData;

        // Process the received packet based on its type
        switch (receivedPacket.type)
        {
        case MessageType::POST_REQUEST:
        {
            // Check if the payload is empty
            if (receivedPacket.payload[0] != '\0')
            {

                // Print the received payload
                std::cout << "Received Payload: " << receivedPacket.payload << std::endl;

                existingData = deserializePosts(receivedPacket.payload);

                posts.insert(posts.end(), existingData.begin(), existingData.end());

                std::istringstream payloadStream(receivedPacket.payload);

                // Rest of your code for deserialization

                // Send a success response back to the client
                Packet responsePacket;
                responsePacket.type = MessageType::POST_RESPONSE;
                PostSubmissionResponse response;
                response.success = true;

                std::string responseStr = serializePostSubmissionResponse(response);
                // serializing a vector to get a response string

                std::memcpy(responsePacket.payload, responseStr.c_str(), responseStr.size());
                // mem copying the string onto the buffer.

                int sendSize = send(ConnectionSocket, (char *)&responsePacket, sizeof(Packet), 0);
                // sending the packet over to the client

                savePostToFile(posts);

                if (sendSize < 0)
                {
                    std::cout << "Sending failed" << std::endl;
                }
                else
                {
                    std::cout << "Success response sent to the client" << std::endl;
                }
            }
            else
            {
                std::cout << "Received empty payload" << std::endl;
            }
            break;
        }

        case MessageType::GET_REQUEST:
        {

            posts = readPostToFile();

            std::string strOfPosts = serializePosts(posts);

            Packet responseRequestPacket;

            responseRequestPacket.type = MessageType::GET_RESPONSE;

            std::memcpy(responseRequestPacket.payload, strOfPosts.c_str(), strOfPosts.size());

            int sendSize = send(ConnectionSocket, (char *)&responseRequestPacket, sizeof(Packet), 0);

            if (sendSize < 0)
            {
                std::cout << "Sending Failed" << std::endl;
            }

            else
            {
                std::cout << "List of Posts successfully sent!!!" << std::endl;
            }

            break;
        }

        case MessageType::FILTER_REQUEST:
        {

            posts = readPostToFile();
            // calling the File module to populate the file with Data Contents

            Packet responseFilteredPacket;
            // creating a response packet

            std::string filterMsg = receivedPacket.payload;
            // copying the received payload onto a string

            std::istringstream iss(filterMsg);
            // creating a stream of string

            std::string key, value;

            std::getline(iss, key, ':');
            std::getline(iss, value);
            // separating the actual value from the header
            // key is the header here and value is the actual value

            std::ostringstream oss;

            if (!value.empty())
            {
                value.pop_back();
            }

            for (const auto &sd : posts)
            {

                if (key == "Author")
                {

                    if (value.compare(sd.author) == 0)
                    {
                        oss << sd.title << ',' << sd.author << ',' << sd.content << ';';
                    }
                }
                else if (key == "Topic")
                {

                    if (value.compare(sd.title) == 0)
                    {
                        oss << sd.title << ',' << sd.author << ',' << sd.content << ';';
                    }
                }
            }

            std::string result = oss.str();

            if (!result.empty())
            {
                result.pop_back();
            }

            responseFilteredPacket.type = MessageType::FILTER_RESPONSE;

            std::memcpy(responseFilteredPacket.payload, result.c_str(), result.size());

            int sendSize = send(ConnectionSocket, (char *)&responseFilteredPacket, sizeof(Packet), 0);

            if (sendSize < 0)
            {

                std::cout << "Filtering Failed" << std::endl;
            }

            else
            {
                std::cout << "Filtered Post Returned Successfully!!!" << std::endl;
            }

            break;
        }

        case MessageType::QUIT_REQUEST:
        {

            std::cout << "Received QUIT_REQUEST. Closing server." << std::endl;
            close(ConnectionSocket);
            close(ServerSocket);
            return 0;
        }

        default:
            std::cout << "Invalid message type received" << std::endl;
        }
    }

    return 0;
}
