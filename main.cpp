#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma warning(disable: 4996)

using namespace std;
//CLIENT

SOCKET Connection;

enum Packets {
	P_ChatMessage
};

bool ProcessPacket(Packets packetType) {
	switch (packetType)
	{
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connection, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connection, msg, msg_size, NULL);
		cout << msg << endl;
		delete[] msg;
		break;
	}
	default:
		cout << "Uncorected packets: " << packetType << endl;
		break;
	}
	return true;
}

void ClientHandler() {
	Packets packettype;
	while (true) {
		recv(Connection, (char*)&packettype, sizeof(Packets), NULL);

		if (!ProcessPacket(packettype)) {
			break;
		}
	}
	closesocket(Connection);
}

int main(int argc, char* argv[]) {
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Error" << endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0) {
		cout << "Error: failed connect tot server." << endl;
		return 1;
	}
	cout << "Connected!" << endl;

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);

	string msg1;
	while (true) {
		std::getline(std::cin, msg1);
		int msg_size = msg1.size();
		Packets packettype = P_ChatMessage;
		send(Connection, (char*)&packettype, sizeof(Packets), NULL);
		send(Connection, (char*)&msg_size, sizeof(int), NULL);
		send(Connection, msg1.c_str(), msg_size, NULL);
		Sleep(10);
	}

	system("pause");
	return 0;
}