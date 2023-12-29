#include "protocol.h"

// Function to serialize a collection of posts to a custom delimiter-separated string


std::string serializePosts(const std::vector<Data>& posts) {
    std::ostringstream oss;

    for (const auto& post : posts) {
        oss << post.title << ',' << post.author << ',' << post.content << ';';
    }

    std::string result = oss.str();

    // Remove the trailing semicolon if the string is not empty
    if (!result.empty()) {
        result.pop_back();
    }

    return result;
}


/*td::string serializePosts(const std::vector<Data>& posts) {
    std::ostringstream oss;

    for (const auto& post : posts) {
        oss << post.title << ',' << post.author << ',' << post.content << ';';
    }

    return oss.str();
}*/


// Function to serialize a post submission response to a custom string
std::string serializePostSubmissionResponse(const PostSubmissionResponse& response) {
    return response.success ? "success" : "failure";
}


std::vector<Data> deserializePosts(const char* payload) {
    std::vector<Data> posts;

    std::string payloadString(payload);

    std::istringstream iss(payloadString);
    std::string postString;
   
    while (std::getline(iss, postString, ';')) {
        std::istringstream postStream(postString);

        std::string title, author, content;


        std::getline(postStream, title, ',');
        std::getline(postStream, author, ',');
        std::getline(postStream, content);


        posts.push_back({ title, author, content });
    }
    return posts;
}