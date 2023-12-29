#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "protocol.h"

std::vector<Data> readPostToFile();
void savePostToFile(const std::vector<Data>& posts);