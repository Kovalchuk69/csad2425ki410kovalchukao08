/**
@file client_functions.cpp
*/

#include "Rock_paper_scissors_client.h"
#include "pch.h"
/**
 * @brief Saves the current game state to a JSON file.
 *
 * @param gameState The current game state represented as a vector of integers.
 * @param resultText The result text of the game.
 * @param gameInfo Additional information about the game.
 * @param filename The filename for saving the game state as JSON (default is "game_config.json").
 */
void saveGameState(const vector<int>& gameState, const string& resultText, const string& gameInfo, const string& filename) {
    json j;
    j["gameState"] = gameState;
    j["resultText"] = resultText;
    j["gameInfo"] = gameInfo;

    std::ofstream file(filename);
    if (file.is_open()) {
        file << j.dump(4);  // Saves JSON with indentation for readability
        file.close();
        cout << "Game state saved to " << filename << endl;
    }
    else {
        cerr << "Failed to open file for saving game state" << endl;
    }
}
/**
 * @brief Loads the game state from a JSON file.
 *
 * @param gameState Reference to a vector that will hold the loaded game state.
 * @param resultText Reference to a string that will hold the loaded result text.
 * @param gameInfo Reference to a string that will hold the loaded game information.
 * @param filename The filename to load the game state from (default is "game_config.json").
 * @return True if loading was successful; otherwise, false.
 */
bool loadGameState(vector<int>& gameState, string& resultText, string& gameInfo, const string& filename) {
    std::ifstream file(filename);
    if (file.is_open()) {
        json j;
        file >> j;

        if (j.contains("gameState") && j.contains("resultText") && j.contains("gameInfo")) {
            gameState = j["gameState"].get<vector<int>>();
            resultText = j["resultText"].get<string>();
            gameInfo = j["gameInfo"].get<string>();
            file.close();
            cout << "Game state loaded from " << filename << endl;
            return true;
        }
        else {
            cerr << "Invalid data in config file" << endl;
        }
    }
    else {
        cerr << "Config file not found" << endl;
    }
    return false;
}
/**
 * @brief Creates a JSON string from an integer array.
 *
 * @param array The array of integers to be converted into JSON.
 * @return A JSON string representation of the integer array.
 */
string createJSON(const vector<int>& array) {
    string json = "{\"array\":[";
    for (size_t i = 0; i < array.size(); ++i) {
        json += to_string(array[i]);
        if (i < array.size() - 1) json += ",";
    }
    json += "]}";
    return json;
}
/**
 * @brief Parses a JSON string to extract an integer array, result text, and game information.
 *
 * @param json The JSON string to parse.
 * @param array Reference to a vector to store the extracted integer array.
 * @param resultText Reference to a string to store the extracted result text.
 * @param gameInfo Reference to a string to store the extracted game information.
 * @return True if parsing was successful; otherwise, false.
 */

bool parseJSON(const string& json, vector<int>& array, string& resultText, string& gameInfo) {
    size_t arrayStart = json.find("\"array\":[");
    size_t resultStart = json.find("\"result\":\"");
    size_t infoStart = json.find("\"info\":\"");

    if (arrayStart != string::npos && resultStart != string::npos && infoStart != string::npos) {
        array.clear();
        size_t startPos = arrayStart + 9;
        size_t endPos = json.find("]", startPos);

        while (startPos < endPos) {
            size_t commaPos = json.find(",", startPos);
            if (commaPos == string::npos || commaPos > endPos) commaPos = endPos;
            array.push_back(stoi(json.substr(startPos, commaPos - startPos)));
            startPos = commaPos + 1;
        }

        size_t resultEnd = json.find("\"", resultStart + 10);
        resultText = json.substr(resultStart + 10, resultEnd - (resultStart + 10));

        size_t infoEnd = json.find("\"", infoStart + 8);
        gameInfo = json.substr(infoStart + 8, infoEnd - (infoStart + 8));

        return true;
    }

    return false;
}