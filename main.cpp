// ================================================ //
// File: main.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines main(), the entry point of program.
// ================================================ //

#include "TFC.hpp"
#include "Probe.hpp"
#include "Timer.hpp"
#include "GUI.hpp"
#include "resource.h"

// ================================================ //

static void UpdateLog(HWND hwnd, const std::string& str)
{
	// Add message to log listbox.
	SendDlgItemMessage(hwnd, IDC_LIST_TFC_UPDATES, LB_ADDSTRING, 0,
					   reinterpret_cast<LPARAM>(str.c_str()));

	// Scroll to last message.
	int count = SendDlgItemMessage(hwnd, IDC_LIST_TFC_UPDATES, LB_GETCOUNT, 0, 0);
	SendDlgItemMessage(hwnd, IDC_LIST_TFC_UPDATES, LB_SETCURSEL,
					   static_cast<WPARAM>(count - 1), 0);
}

// ================================================ //

static void UpdateShields(HWND hwnd, const Uint shields)
{
	// Link progress bar to shield power.
	SendDlgItemMessage(hwnd, IDC_PROGRESS_SHIELDS, PBM_SETPOS,
					   static_cast<WPARAM>(shields), 0);

	// Update number of hits on shields.
	std::string buffer = "Shields (Hits: " + toString(5 - shields) + ")";
	SetDlgItemText(hwnd, IDC_STATIC_SHIELDS, buffer.c_str());
}

// ================================================ //

static void AddProbeToList(HWND hList, const Uint id, 
						   const Uint type, const unsigned state)
{	
	static Uint index = 0;
	std::string str;

	// Insert initial item (from GUI.hpp).
	InsertListviewItem(hList, index, toString(id));

	switch (type){
	default:
	case Probe::Type::SCOUT:
		str = "Scout";
		break;

	case Probe::Type::PHOTON:
		str = "Photon Defense";
		break;

	case Probe::Type::PHASER:
		str = "Phaser Defense";
		break;
	}

	SetListviewItem(hList, index, 1, str);

	++index;
}

// ================================================ //
// A thread to process GUI updates.
static void UpdateGUI(HWND hwnd, TFC* tfc)
{
	// Used to update elapsed time every second.
	Timer update(true);
	// Index counter for asteroid listview.
	Uint asteroidIndex = 0;

	while (hwnd != nullptr){		
		UpdateWindow(hwnd);						

		// Check for GUI events from TFC.
		if (tfc->hasGUIEvent()){
			GUIEvent e = tfc->getNextGUIEvent();
			switch (e.type){
			default:
			case GUIEventType::NONE:
				break;

			case GUIEventType::ASTEROID_FOUND:
				{
					// Insert asteroid data into listview.
					HWND hList = GetDlgItem(hwnd, IDC_LIST_ASTEROIDS);
					InsertListviewItem(hList, asteroidIndex, toString(e.asteroid.id));
					SetListviewItem(hList, asteroidIndex, 1, toString(e.asteroid.mass));
					++asteroidIndex;

					// Update the TFC log.
					std::string buffer = "Asteroid " + toString(e.asteroid.id) + " discovered.";
					UpdateLog(hwnd, buffer);

					// Update number of asteroids in stack.
					buffer = "Asteroids (Count: " + toString(asteroidIndex) + ")";
					SetDlgItemText(hwnd, IDC_STATIC_LIST_ASTEROIDS_TITLE, buffer.c_str());

					printf("***Asteroid %d added to list\n\n", e.asteroid.id);
				}
				break;

			case GUIEventType::ASTEROID_REMOVED:				
				{
					// Find listview item for removed asteroid and delete it.
					HWND hList = GetDlgItem(hwnd, IDC_LIST_ASTEROIDS);
					// Find index of listview item with asteroid (e.x holds ID).
					int index = GetListviewItemIndex(hList, 0, toString(e.x));
					if (index != -1){
						SendMessage(hList, LVM_DELETEITEM, static_cast<WPARAM>(index), 0);
					}
					--asteroidIndex;

					// Update asteroids awaiting destruction.
					std::string buffer = "Asteroids Awaiting Destruction: " + 
						toString(55 - tfc->getNumAsteroidsDestroyed());
					SetDlgItemText(hwnd, IDC_STATIC_NUMASTEROIDS, buffer.c_str());

					// Update asteroids destroyed.
					buffer = "Asteroids Successfully Destroyed: " +
						toString(tfc->getNumAsteroidsDestroyed());
					SetDlgItemText(hwnd, IDC_STATIC_ASTEROIDS_DESTROYED, buffer.c_str());

					// Update TFC log.
					buffer = "Asteroid " + toString(e.x) + " removed from queue.";
					UpdateLog(hwnd, buffer);

					// Update number of asteroids in stack.
					buffer = "Asteroids (Count: " + toString(asteroidIndex) + ")";
					SetDlgItemText(hwnd, IDC_STATIC_LIST_ASTEROIDS_TITLE, buffer.c_str());
				}
				break;

			case GUIEventType::ASTEROID_DESTROYED:
				{
					// Update asteroids awaiting destruction.
					std::string buffer = "Asteroids Awaiting Destruction: " + 
						toString(55 - tfc->getNumAsteroidsDestroyed());
					SetDlgItemText(hwnd, IDC_STATIC_NUMASTEROIDS, buffer.c_str());

					// Update asteroids destroyed.
					buffer = "Asteroids Successfully Destroyed: " +
						toString(tfc->getNumAsteroidsDestroyed());
					SetDlgItemText(hwnd, IDC_STATIC_ASTEROIDS_DESTROYED, buffer.c_str());

					buffer = "Asteroid " + toString(e.x) + " destroyed by probe " + 
						toString(e.id);
					UpdateLog(hwnd, buffer);
				}
				break;

			case GUIEventType::ASTEROID_COLLISION:
				UpdateShields(hwnd, tfc->getShields());
				{
					std::string buffer = "Asteroid " + toString(e.id) +
						" collided with TFC!";
					UpdateLog(hwnd, buffer);
				}
				break;

			case GUIEventType::PROBE_TERMINATED:			
				{
					// Remove probe from listview.
					HWND hList = GetDlgItem(hwnd, IDC_LIST_PROBES);
					int index = GetListviewItemIndex(hList, 0, toString(e.id));
					if (index != -1){
						SendMessage(hList, LVM_DELETEITEM, static_cast<WPARAM>(index), 0);
					}

					// Update number of probes.
					std::string buffer = "Launched Probes (Count: " +
						toString(tfc->getNumProbes()) + ")";
					SetDlgItemText(hwnd, IDC_STATIC_LIST_PROBES_TITLE, buffer.c_str());

					buffer = "Probe " + toString(e.id) + " lost to asteroid " +
						toString(e.x) + "!";
					UpdateLog(hwnd, buffer);
				}
				break;

			case GUIEventType::FLEET_DESTROYED:						
				SetDlgItemText(hwnd, IDC_STATIC_STATUS, "Fleet Status: Destroyed");	
				{
					std::string msg = "The fleet has been destroyed.\r\n\r\n" +
						toString(tfc->getNumPhaserProbesLaunched()) + " phaser probes used.";
					MessageBox(hwnd, msg.c_str(), "Failure",
							   MB_OK | MB_ICONWARNING | MB_SETFOREGROUND);
				}							
				UpdateLog(hwnd, "Fleet destroyed!");
				break;

			case GUIEventType::FLEET_SURVIVED:
				SetDlgItemText(hwnd, IDC_STATIC_STATUS, "Fleet Status: Arrived");
				{
					std::string msg = "The fleet successfully nagivated the asteroid field"
						" and has arrived at Talos IV!\r\n\r\n" + 
						toString(tfc->getNumPhaserProbesLaunched()) +
						" phaser probes used.";
					MessageBox(hwnd, msg.c_str(), "Success",
							   MB_OK | MB_ICONINFORMATION | MB_SETFOREGROUND);
				}				
				UpdateLog(hwnd, "Fleet arrived!");
				break;
			}
		}

		// Update elapsed time every one second.
		if (tfc->isInAsteroidField()){
			if (update.getTicks() > 1000){
				Uint time = tfc->getCurrentTime() / 1000;
				std::string buf("Elapsed Time: " + toString(time));
				SetDlgItemText(hwnd, IDC_STATIC_TIME, buf.c_str());
				update.restart();
			}
		}
	}
}

// ================================================ //

static BOOL CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Initialize the TFC here.
	static TFC tfc;
	// Array of smart pointers storing allocate Probe objects. They are 
	// automatically freed when execution leaves this scope.
	static std::vector<std::shared_ptr<Probe>> probes;
	HBRUSH hBackground = reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1);

	// Catch escape key here, processing its WM_KEYDOWN message would 
	// require subclassing the dialog's window procedure.
	if (GetAsyncKeyState(VK_ESCAPE)){
		PostMessage(hwnd, WM_QUIT, 0, 0);
	}

	// Process messages.
	switch (msg){
	default:
		return FALSE;

	case WM_INITDIALOG:	
		{
			// Setup GUI items.
			// Asteroid listview.						  			
			HWND hList = GetDlgItem(hwnd, IDC_LIST_ASTEROIDS);
			ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			// Setup columns.
			LV_COLUMN lc = { 0 };
			lc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lc.fmt = LVCFMT_LEFT;

			lc.iSubItem = 0;
			lc.pszText = "Mass";
			lc.cx = 150;
			SendMessage(hList, LVM_INSERTCOLUMN, 0, reinterpret_cast<LPARAM>(&lc));

			lc.iSubItem = 1;
			lc.pszText = "ID";
			lc.cx = 100;
			SendMessage(hList, LVM_INSERTCOLUMN, 0, reinterpret_cast<LPARAM>(&lc));

			// Defensive probe listview.
			hList = GetDlgItem(hwnd, IDC_LIST_PROBES);
			ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

			// Setup columns.
			lc.iSubItem = 0;
			lc.pszText = "Type";
			lc.cx = 200;
			SendMessage(hList, LVM_INSERTCOLUMN, 0, reinterpret_cast<LPARAM>(&lc));

			lc.iSubItem = 1;
			lc.pszText = "ID";
			lc.cx = 50;
			SendMessage(hList, LVM_INSERTCOLUMN, 0, reinterpret_cast<LPARAM>(&lc));

			// Shields progress bar.
			HWND hShields = GetDlgItem(hwnd, IDC_PROGRESS_SHIELDS);
			SendMessage(hShields, PBM_SETRANGE, 0, MAKELPARAM(0, 5));
			SendMessage(hShields, PBM_SETPOS, static_cast<WPARAM>(5), 0);

			// Create initial probes (one scout and two photon).
			// Scout probe.
			std::shared_ptr<Probe> probe(new Probe(Probe::Type::SCOUT));			
			if (probe->launch() == true){
				probes.push_back(probe);
				AddProbeToList(hList, probe->getID(), Probe::Type::SCOUT, probe->getState());
			}

			// Photon defense probes.
			for (int i = 0; i < 2; ++i){
				// Re-allocate a probe.
				probe.reset(new Probe(Probe::Type::PHOTON));				
				if (probe->launch() == true){
					probes.push_back(probe);
					AddProbeToList(hList, probe->getID(), Probe::Type::PHOTON, probe->getState());
				}
			}

			// Update probe listview title.
			std::string buf("Launched Probes (Count: " + toString(probes.size()) + 
							std::string(")"));
			SetDlgItemText(hwnd, IDC_STATIC_LIST_PROBES_TITLE, buf.c_str());

			// Allow horizontal scrolling of update listbox.
			SendDlgItemMessage(hwnd, IDC_LIST_TFC_UPDATES, LB_SETHORIZONTALEXTENT, 
							   static_cast<WPARAM>(500), 0);
			UpdateLog(hwnd, "Class M planet Talos IV found.");

			// Create thread for updating time.
			std::thread t(&UpdateGUI, hwnd, &tfc);
			t.detach();
		}
		return FALSE;

	case WM_CTLCOLORDLG:
		return reinterpret_cast<LONG>(hBackground);

	case WM_CTLCOLORSTATIC:
		SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
		if (reinterpret_cast<HWND>(lParam) == GetDlgItem(hwnd, IDC_STATIC_STATUS)){
			SetTextColor(reinterpret_cast<HDC>(wParam), RGB(215, 64, 64));
		}
		else if (reinterpret_cast<HWND>(lParam) == GetDlgItem(hwnd, IDC_STATIC_TIME)){
			SetTextColor(reinterpret_cast<HDC>(wParam), RGB(64, 64, 225));
		}
		return reinterpret_cast<LRESULT>(hBackground);

	case WM_CTLCOLORLISTBOX:
		// Only one listbox to color.
		SetBkColor(reinterpret_cast<HDC>(wParam), RGB(0, 0, 0));
		SetTextColor(reinterpret_cast<HDC>(wParam), RGB(51, 204, 51));
		return reinterpret_cast<LONG>(CreateSolidBrush(RGB(0, 0, 0)));

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		default:
			break;

		case IDC_BUTTON_START:
			{
				EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_ADD_PROBE), FALSE);
				EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_START), FALSE);
				SetDlgItemText(hwnd, IDC_STATIC_STATUS, "Fleet Status: Navigating Field");
				tfc.enterAsteroidField();
			}			
			break;

		case IDC_BUTTON_ADD_PROBE:
			{				
				// Allocate a new phaser probe and launch it
				// (the TFC is running and will handle it).
				std::shared_ptr<Probe> probe(new Probe(Probe::Type::PHASER));
				if (probe->launch() == true){
					probes.push_back(probe);

					AddProbeToList(GetDlgItem(hwnd, IDC_LIST_PROBES), probe->getID(),
								   Probe::Type::PHASER, probe->getState());

					// Update listview title.
					std::string buf("Launched Probes (Count: " + toString(probes.size()) + 
									std::string(")"));
					SetDlgItemText(hwnd, IDC_STATIC_LIST_PROBES_TITLE, buf.c_str());
				}
				else{
					MessageBox(hwnd, "Failed to launch probe!", "ERROR", 
							   MB_OK | MB_ICONEXCLAMATION);
				}
			}
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
		// Force all probe threads to close.
		TFC::RunArtificialProbes = false;
		EndDialog(hwnd, 0);
		return FALSE;
	}

	return TRUE;
}

// ================================================ //
#define _DEBUG
//int main(int argc, char** argv)
#ifdef _DEBUG
int main(int argc, char** argv)
#else
int APIENTRY WinMain(HINSTANCE hHinst, HINSTANCE hPrevInst, LPSTR args, int numArgs)
#endif
{
	// Initialize Winsock, begin using WS2_32.DLL.
	WSAData wsaData;
	if (WSAStartup(0x101, &wsaData) != 0){
		return 1;
	}

	// Initialize visual styles for GUI.
	INITCOMMONCONTROLSEX iccx;
	ZeroMemory(&iccx, sizeof(iccx));
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS |
		ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&iccx);

	// Create the dialog (GUI), this activates the callback MainProc().
	int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN), HWND_DESKTOP, MainProc);

	// Terminate use of WS2_32.DLL.
	WSACleanup();

	return ret;
}

// ================================================ //