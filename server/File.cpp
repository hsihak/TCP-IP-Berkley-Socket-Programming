// File Module

#include "File.h"

std::vector<Data> readPostToFile() {
    std::vector<Data> posts;
    std::string fileName = "postlists.txt";

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cout << "Error: Unable to open the file " << fileName << std::endl;
        return posts;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<Data> existingData = deserializePosts(line.c_str());
        posts.insert(posts.end(), existingData.begin(), existingData.end());
    }

    file.close();

    return posts;
}



void savePostToFile(const std::vector<Data>& posts) {
    std::string fileName = "postlists.txt";
    std::ofstream fout;

    fout.open(fileName, std::ios::app);

    if(fout.is_open()) {
        for (const auto& post: posts) {
            fout << post.title << "," << post.author << "," << post.content << ";" << std::endl;
        }
        fout.close(); 
    } else {
        std::cout << "Error: Unable to create and save the file" << std::endl;
        exit(EXIT_FAILURE);
    }
}
