#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;
unordered_map<string, string> readCredentialsFile(void);
void authenticate(unordered_map<string, string> umap);