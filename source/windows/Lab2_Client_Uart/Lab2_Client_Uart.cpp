#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

// ������� ��������� JSON
string createJSON(const string& message) {
    return "{\"message\":\"" + message + "\"}";
}

// ������� �������� JSON
string parseJSON(const string& json) {
    size_t start = json.find("\"message\":\"") + 11; // 11 - ������� ����� "message" � �������
    size_t end = json.find("\"", start);

    if (start != string::npos && end != string::npos) {
        return json.substr(start, end - start);
    }

    return ""; // ���� JSON �����������
}

int main() {
    /*����� ������ COM-����� � �����������*/
    int portNumber;
    cout << "Enter COM port number (e.g., 6 for COM6): ";
    cin >> portNumber;

    // ���������� ����� �����
    string portName_s = "COM" + to_string(portNumber);
    const char* portName = portName_s.c_str();

    // ³������� COM-�����
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

    // ������������ ��������� �����
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

    // ������������ ����-����
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

    // ���� ��� ��������� �������� ����������
    while (true) {
        string message = "HELLO world";  // ����������� ��� ��������
        string messageJSON = createJSON(message);
        DWORD bytesWritten;

        // ³������� ����������� �� Arduino
        if (!WriteFile(hSerial, messageJSON.c_str(), messageJSON.size(), &bytesWritten, NULL)) {
            cerr << "Error writing to port" << endl;
            break;
        }
        cout << "Sent to Arduino:       " << message << " (" << messageJSON << ")" << endl;

        // ������� ������ �� Arduino
        char buffer[128];
        DWORD bytesRead;

        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            buffer[bytesRead] = '\0';
            if (bytesRead > 0) {
                cout << "Received from Arduino: " << parseJSON(buffer) << " (" << buffer << ")" << endl << endl;
            }
        }
        else {
            cerr << "Error reading from port" << endl;
            break;
        }

        // �������� ����� ��������� �������
        Sleep(1000);
    }

    // �������� COM-�����
    CloseHandle(hSerial);
    return 0;
}
