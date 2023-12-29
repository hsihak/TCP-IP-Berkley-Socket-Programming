
#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <memory>

#define PAYLOAD_SIZE 600


enum MessageType
{
    POST_REQUEST = 1,     // client sends a request to post a message
    POST_RESPONSE = 2,    // server responds to the post request by the client 
    GET_REQUEST = 3,     // client requests to access posts
    GET_RESPONSE = 4,    // server responds to client with a list of posts
    FILTER_REQUEST = 5,  //client send a request to filter the posts
    FILTER_RESPONSE = 6,  //server responds to client with the filtered posts
    QUIT_REQUEST = 7     // client is done and wants to close the application

};


struct Data
{
    std::string title;
    std::string author;
    std::string content;
};


struct Packet
{
    MessageType type;
    char payload[PAYLOAD_SIZE];
};

struct PostSubmissionResponse
{
    bool success;
};



std::string serializePosts(const std::vector<Data>& posts);
std::string serializePostSubmissionResponse(const PostSubmissionResponse& response);
std::vector<Data> deserializePosts(const char* payload);