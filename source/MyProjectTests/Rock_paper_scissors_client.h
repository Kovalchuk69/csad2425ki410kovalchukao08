#pragma once
#include <iostream>
#include <windows.h>

#include <string>
#include <vector>
#include <cstdlib> // For system()
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

bool loadGameState(vector<int>& gameState, string& resultText, string& gameInfo, const string& filename);
string createJSON(const vector<int>& array);
bool parseJSON(const string& json, vector<int>& array, string& resultText, string& gameInfo);
void saveGameState(const vector<int>& gameState, const string& resultText, const string& gameInfo, const string& filename);