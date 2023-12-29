#include "protocol.h"

// Function to serialize a collection of posts to a custom delimiter-separated string
std::string serializePosts(const std::vector<Data>& posts) {
    std::ostringstream oss;


    for (const auto& post : posts) {
        oss << post.title << ',' << post.author << ',' << post.content << ';';
    }
    // the oss steam will create the stream of bits of title, author and content.
    // a for loop is used to combine different posts together.

    std::string result = oss.str();

    if (!result.empty()) {
        result.pop_back();
    }

    return result;
}


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


// std::vector<Data> packetHandling(Packet& p, const PostSubmissionResponse& PSR) {
//     std::vector<Data> posts;

//     switch (p.type) {
//         case POST_REQUEST:
//             posts = deserializePosts(p.payload.get(), p.payloadSize);

//         case POST_RESPONSE:
//             std::string boolToStr = serializePostSubmissionResponse(PSR);

//             p.payloadSize = (size_t)boolToStr.size();

//             p.payload = std::make_unique<char[]>(p.payloadSize);
            
//             std::memcpy(p.payload.get(), boolToStr.c_str(), p.payloadSize);
//         }


//     return posts;
// }