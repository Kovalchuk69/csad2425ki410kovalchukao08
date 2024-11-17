/**
@file Rock_paper_scissors_client.cpp
*/
#include "Rock_paper_scissors_client.h"
//using json = nlohmann::json;

//using namespace std;

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
