/**
@file Rock_paper_scissors_client.cpp
*/
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // For system()
#include <fstream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

using namespace std;

/**
 * @brief Saves the current game state to a JSON file.
 *
 * @param gameState The current game state represented as a vector of integers.
 * @param resultText The result text of the game.
 * @param gameInfo Additional information about the game.
 * @param filename The filename for saving the game state as JSON (default is "game_config.json").
 */
void saveGameState(const vector<int>& gameState, const string& resultText, const string& gameInfo, const string& filename = "game_config.json") {
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
bool loadGameState(vector<int>& gameState, string& resultText, string& gameInfo, const string& filename = "game_config.json") {
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

/**
 * @brief The main function initializes serial communication with an Arduino, handles game modes, and processes game data.
 *
 * @return Exit code of the program (0 for success, 1 for failure).
 */
int main() {
    int portNumber;
    cout << "Enter COM port number (e.g., 6 for COM6): ";
    cin >> portNumber;

    string portName_s = "COM" + to_string(portNumber);
    const char* portName = portName_s.c_str();

    HANDLE hSerial = CreateFileA(portName,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Error opening port" << endl;
        system("pause");
        return 1;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error getting port state" << endl;
        system("pause");
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        cerr << "Error setting port parameters" << endl;
        system("pause");
        return 1;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        cerr << "Error setting timeouts!" << endl;
        system("pause");
        return 1;
    }

    Sleep(2000);
    vector<int> resultArray(5, 0);
    string resultText, gameInfo;

    while (true) {
        vector<int> resultArray(5, 0);
        cout << "Gamemodes:\n1-Man vs AI\n2-Man vs Man\n3-AI vs AI\n4-Load previous game\n";
        cout << "Enter Game mode: ";
        int gameMode;
        cin >> gameMode;

        if (gameMode == 4) {
            if (loadGameState(resultArray, resultText, gameInfo)) {
                cout << "Loaded Game Mode: " << resultArray[0] << endl;
                cout << "Current Score: " << resultArray[3] << "/3 : " << resultArray[4] << "/3" << endl;
                cout << "Result: " << resultText << endl;
                cout << "Info: " << gameInfo << endl;

                if (resultArray[3] == 3 || resultArray[4] == 3) {
                    cout << "Game already completed. Restart to play again." << endl;
                    continue;
                }
            }
            else {
                cout << "No previous game data found. Starting a new game." << endl;
            }
        }
        else {
            resultArray[0] = gameMode;
        }

        while (true) {
            if (resultArray[0] == 1) {
                cout << "\n1 - Rock\n2 - Paper\n3 - Scissors\nEnter:";
                cin >> resultArray[1];
            }
            else if (resultArray[0] == 2) {
                cout << "\n1 - Rock\n2 - Paper\n3 - Scissors\nEnter1:";
                cin >> resultArray[1];
                system("cls");
                cout << "1 - Rock\n2 - Paper\n3 - Scissors\nEnter2:";
                cin >> resultArray[2];
            }
            else {
                cout << "\nAI vs AI\n:";
            }

            string messageJSON = createJSON(resultArray);
            DWORD bytesWritten;

            if (!WriteFile(hSerial, messageJSON.c_str(), messageJSON.size(), &bytesWritten, NULL)) {
                cerr << "Error writing to port" << endl;
                break;
            }
            cout << "\nSent " << messageJSON << endl;

            string receivedMessage;
            while (true) {
                char buffer[256];
                DWORD bytesRead;

                if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                    buffer[bytesRead] = '\0';
                    receivedMessage += buffer;

                    if (receivedMessage.find('\n') != string::npos) {
                        receivedMessage.erase(receivedMessage.find('\n'));
                        break;
                    }
                }
            }

            vector<int> array;
            if (parseJSON(receivedMessage, array, resultText, gameInfo)) {
                cout << "Received:     [";
                for (size_t i = 0; i < array.size(); ++i) {
                    resultArray[i] = array[i];
                    cout << array[i];
                    if (i < array.size() - 1) cout << ",";
                }
                cout << "]" << endl;
                cout << gameInfo << endl;
                cout << "Score: " << resultArray[3] << "/3  :  " << resultArray[4] << "/3\n";
                cout << "Result: " << resultText << endl;
                saveGameState(resultArray, resultText, gameInfo);
                if (resultArray[3] == 3 || resultArray[4] == 3) {
                    cout << "\n";
                    break;
                }
            }
            else {
                cerr << "Error parsing JSON from Arduino" << endl;
            }

            Sleep(1000);
        }
    }
    CloseHandle(hSerial);
    return 0;
}
