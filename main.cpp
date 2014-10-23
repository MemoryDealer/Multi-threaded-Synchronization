// ================================================ //
// File: main.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines function main(), entry point of program.
// ================================================ //

#include "TFC.hpp"
#include "Probe.hpp"
#include "resource.h"

// ================================================ //

static BOOL CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HBRUSH hBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);

	switch (msg){
	default:

		return FALSE;

	case WM_INITDIALOG:

		return FALSE;

	case WM_CTLCOLORDLG:
		return reinterpret_cast<LONG>(hBackground);

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		default:
			break;

		case IDOK:
			printf("OK!\n");
			break;
		}
		return FALSE;

	case WM_SYSCOMMAND:
		switch (wParam){
		default:
			break;

		case SC_MINIMIZE:

			return TRUE;
		}
		return FALSE;

	case WM_CLOSE:

		EndDialog(hwnd, 0);
		return FALSE;
	}

	return TRUE;
}

// ================================================ //

//int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, 
//					 LPSTR lpszArg, int nFunsterStil)
int main(int argc, char** argv)
{
	return DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN), NULL, MainProc);
	//// Initialize Winsock, begin using WS2_32.DLL.
	//WSAData wsaData;
	//if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
	//	return 1;
	//}

	//// Initialize TFC (server).
	//TFC tfc;

	//int probeID = 0;
	//// Prepare probes and launch them.
	//while (tfc.getNumProbes() < 12){
	//	Probe* probe = new Probe(probeID++, 0, Probe::Type::PHASER);
	//	probe->launch();
	//	printf("Waiting for probes...\n");
	//	Sleep(5000);

	//}

	//// Enter the asteroid field.
	//while (true){
	//	tfc.navigateAsteroidField();
	//}

	//// Terminate use of WS2_32.DLL.
	//WSACleanup();

	return 0;
}

// ================================================ //