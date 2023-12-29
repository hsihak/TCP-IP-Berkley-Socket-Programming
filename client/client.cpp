/******************************************************************
 * Assignment 3 TCP/IP Lab - Course CSCN72020 - Sec3               *
 * Date: December, 2023                                            *
 * By: Saba Berenji and Sierra Erb                                 *
 * Description: Creating the client for a client/server application*
 *              of a discussion board                              *
 * Professor: Monti Sachdeva                                       *
 *******************************************************************/

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unordered_map>
#include <arpa/inet.h>
#include <vector>
#include <sstream>

#include "menu.h"
#include "protocol.h"
#include "authentication.h"

int main()
{

	// authenticating the user
	unordered_map<string, string> umap;
	umap = readCredentialsFile();
	authenticate(umap);

	// Creating a client by calling the Socket() function
	// Using UDP as the transport layer protocol
	int ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // AF_INET means socket will use IPv4
	// SOCK_STREAM determines that the socket is of type stream socket. IPPROTO_TCP determines the type of protocol which is TCP.

	// validating the creation of the socket
	if (ClientSocket == -1)
	{
		cout << "ERROR:  Failed to create ClientSocket" << std::endl;
		return 0;
	}

	cout << "Client Socket Sucessfully Created" << endl;

	// bind and configure a socket for listening
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;												// Socket will use IPv4
	SvrAddr.sin_port = htons(27000);										// The client application has the port number of 27000
	SvrAddr.sin_addr.s_addr = inet_addr("172.16.5.12"); // The IP address (IPV4) the server application is using

	// Starting the three-way handshak:
	// calling the connect() function in order to establish a connection with the server, checking the connection, closing the client socket if there is an error and then exiting the program
	if (connect(ClientSocket, (struct sockaddr *)&SvrAddr, sizeof(SvrAddr)) == -1)
	{
		close(ClientSocket);
		cout << "ERROR: Connection attempted failed" << std::endl;
		return 0;
	}

	cout << "Connection Sucessful" << endl;

	// printing out the menu and asking the user to choose an option from the menu
	char choice;
	menu();
	while ((choice = menuinput()) != 'd') // call the menuinput function and exit the loop if the input is 'd'
	{

		switch (choice) // using switch case statement for the menu options
		{
		case 'a': // this option if for submitting posts
		{

			std::vector<Data> posts;
			int count = 1; // a flag used for limiting the number of posts a user can submit to 3

			while (count <= 3)
			{
				Data post;

				// getting user input for the title of the post
				std::cout << "Enter a title for the post(limited to 30 characters): " << endl;
				std::getline(std::cin >> std::ws, post.title);

				// truncate the title to 30 characters
				if (post.title.length() > 30)
				{
					post.title = post.title.substr(0, 30);
				}

				// getting user input for the author of the post
				std::cout << "Enter the name of the author (enter N/A if you wish to remain anonymous): ";
				string author;
				std::getline(std::cin >> std::ws, author);
				if (author == "N/A")
					post.author = "Anonymous";
				else
					post.author = author;

				// truncate the title to 20 characters
				if (post.author.length() > 20)
				{
					post.author = post.author.substr(0, 20);
				}

				// getting user input for the content of the post
				std::cout << "Enter the content of the post: ";
				std::getline(std::cin >> std::ws, post.content);

				// truncate the title to 150 characters
				if (post.content.length() > 150)
				{
					post.content = post.content.substr(0, 150);
				}

				posts.push_back(post); // adding the post to the vector

				std::string answer;
				// ask the user if they want to create another post
				do
				{
					std::cout << "Do you want to create another post?\nYou can only create 3 posts max! (only enter yes/no): ";
					std::cin >> answer;

				} while (answer != "yes" && answer != "no");

				if (answer == "no" || count == 3)
				{
					// End the loop if the user doesn't want to create another post
					break;
				}

				count++;
			}

			// sending the posts to the server:
			Packet packet;
			packet.type = MessageType::POST_REQUEST; // specifying the type of the packet
			// encoding the collection of posts to a custom delimiter-separated string defined in protocol.cpp
			std::string serializedPosts = serializePosts(posts);
			serializedPosts[serializedPosts.size() - 1] = '\0';														// truncating the last character of the string
			std::memcpy(packet.payload, serializedPosts.c_str(), serializedPosts.size()); // copying the content of the serializedPosts string into the payload

			// sending the packet to the server
			int sendsize = send(ClientSocket, &packet, sizeof(Packet), 0);

			// validating if send was susccessful or not
			if (sendsize < 0)
			{
				cout << "Sending failed" << endl;
				close(ClientSocket); // close the socket
				return 0;
			}

			cout << "Posts successfully sent to the server!" << endl;

			// getting a confirmation message from the server to ensure posts are submitted:

			Packet responsePacket;
			int receivedSize = recv(ClientSocket, &responsePacket, sizeof(Packet), 0);

			if (receivedSize < 0)
			{
				std::cout << "Error: did not receive anything from server" << std::endl;
				close(ClientSocket);
				return 0;
			}

			// processing the response received from the server
			if (responsePacket.type == MessageType::POST_RESPONSE)
			{
				// deserializing the response
				PostSubmissionResponse response;

				responsePacket.payload[receivedSize] = '\0'; // ensure null-termination
				std::string responseStr(responsePacket.payload);

				response.success = (responseStr == "success");

				// check the payload message to determine the success of the submission
				if (response.success)
				{
					std::cout << "Posts submitted successfully!" << std::endl;
				}
				else
				{
					std::cout << "Failed to submit posts." << std::endl;
				}
			}
			else
			{
				std::cout << "Invalid response type" << std::endl;
			}
		}

		break;

		case 'b':
		{
			Packet view;
			view.type = MessageType::GET_REQUEST; // this message type requests to view all of the posts on the discussion board
			// Encoding the filter term to a custom delimiter-separated string defined in protocol.cpp

			string requestView = "show me the posts"; // message to be put in the payload

			std::memcpy(view.payload, requestView.c_str(), requestView.size()); // copying the requestView into the payload

			view.payload[strlen(view.payload) - 1] = '\0';

			// sending the packet to the server
			int sendsize = send(ClientSocket, &view, sizeof(Packet), 0); // sending the view packet to the server

			if (sendsize < 0) // checks if the data send is greater than 0, if not an error occured
			{
				cout << "Sending failed" << endl;
				close(ClientSocket); // close the socket
				return 0;
			}
			else
			{
				cout << "Sent sucessful: " << requestView << endl; // if the data send is greater than 0 then we want to print that the sending was sucessful
			}

			// receiving the confirmation message from the server
			Packet responseView;
			int receivedSize = recv(ClientSocket, &responseView, sizeof(Packet), 0); // recieving the response from the server

			responseView.payload[strlen(responseView.payload) - 1] = '\0';

			if (receivedSize < 0)
			{ // if receiveSize is less than 0 then client recieved nothing from the server
				std::cout << "Error: did not receive anything from server" << std::endl;
				close(ClientSocket);
				return 0;
			}

			// Process the response
			if (responseView.type == MessageType::GET_RESPONSE)
			{ // check to make sure that the reponse is what we are expecting (it is of type GET_RESPONSE)
				if (responseView.payload[0] != '\0')
				{ // make sure that the payload is not null
					std::cout << "All Posts: " << std::endl;

					// deserialize the payload
					std::vector<Data> posts = deserializePosts(responseView.payload);

					// display the posts
					for (const auto &post : posts)
					{
						std::cout << "Title: " << post.title << std::endl;
						std::cout << "Author: " << post.author << std::endl;
						std::cout << "Content: " << post.content << std::endl;
						std::cout << "-----------------------------------------------" << std::endl;
					}
				}

				else
				{ // enter this if the payload is null
					std::cout << "Received empty payload" << std::endl;
				}
			}
			else
			{
				std::cout << "Invalid response type" << std::endl; // response type was not what was expected
			}
		}

		break;

		case 'c':
		{
			Packet filter;
			filter.type = MessageType::FILTER_REQUEST; // message type is a FILTER_REQUEST

			char choice;
			string filterTerm;
			filterTerm[filterTerm.length() - 1] = '\0';

			filtermenu();
			choice = menuinput(); // call the menuinput function and exit the loop if the input is 'c'
			if (choice == 'a')
			{
				cout << "Enter the author of the post you want to find: ";
				getline(std::cin >> std::ws, filterTerm); // user enters the auhor or title they want to filter by
				filterTerm = "Author:" + filterTerm;
			}

			else if (choice == 'b')
			{
				cout << "Enter the title of the post you want to find: ";
				getline(std::cin >> std::ws, filterTerm); // user enters the auhor or title they want to filter by
				filterTerm = "Topic:" + filterTerm;
			}

			else
				break;

			filterTerm[filterTerm.length() + 1] = '\0';													// null terminate the filterTerm
			std::memcpy(filter.payload, filterTerm.c_str(), filterTerm.size()); // compy the filter term into the payload of the filter Packet
			// sending the filter packet to the server

			filter.payload[strlen(filter.payload) - 1] = '\0';

			int sendsize = send(ClientSocket, &filter, sizeof(Packet), 0);

			if (sendsize < 0) // checks if the data send is greater than 0, if not an error occured
			{
				cout << "Sending failed" << endl;
				close(ClientSocket); // close the socket
				return 0;
			}

			cout << "Filter request successfully sent to the server!" << endl;

			// receiving the confirmation message from the server
			Packet filterReponse;
			int receivedSize = recv(ClientSocket, &filterReponse, sizeof(Packet), 0);

			if (receivedSize < 0)
			{ // if receivedSize is less than 0 then nothing was recieved
				std::cout << "Error: did not receive anything from server" << std::endl;
				close(ClientSocket);
				return 0;
			}

			// process the response received from the server
			if (filterReponse.type == MessageType::FILTER_RESPONSE)
			{ // something was recieved and it shoule be of type FILTER_RESPONSE
				if (filterReponse.payload[0] != '\0')
				{ // check that the payload is not null
					std::cout << "Requested Post: " << std::endl;

					// Deserialize the payload
					std::vector<Data> posts = deserializePosts(filterReponse.payload);

					// Display the deserialized posts
					for (const auto &post : posts)
					{
						std::cout << "Title: " << post.title << std::endl;
						std::cout << "Author: " << post.author << std::endl;
						std::cout << "Content: " << post.content << std::endl;
						std::cout << "-----------------------------------------------" << std::endl;
					}
				}

				else
				{
					std::cout << "Received empty payload" << std::endl; // did not recieve any info in the payload
				}
			}

			else
			{
				std::cout << "Invalid response type" << std::endl; // message type was not FILTER_REQUEST
			}
		}

		break;

		default:
			cout << "Invalid input!" << endl; // prints when the user chooses an invalid option from the menu
			cout << "Please try again." << endl;
		}

		menu();
	}

	// This section will start executing once the user selects option 'd' from the menu which is 'quit'
	char quit_message[] = {"I want to close the discussion board."};

	// set the type of the packet to 'QUIT_REQUEST' and send it to the server
	Packet packet;
	packet.type = MessageType::QUIT_REQUEST;
	int sendsize = send(ClientSocket, &packet, sizeof(Packet), 0);

	if (sendsize < 0)
	{
		std::cout << "Sending failed" << std::endl;
		close(ClientSocket);
		return 0;
	}

	cout << "Quit request successfully sent to the server!" << endl;

	// Cleanup and close socket
	close(ClientSocket); // closes client socket

	return 0;
}