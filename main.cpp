// ================================================ //
// File: main.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Defines function main(), entry point of program.
// ================================================ //

#include "TFC.hpp"
#include "Probe.hpp"
#include "Timer.hpp"
#include "GUI.hpp"
#include "resource.h"

// ================================================ //

static void AddProbeToList(HWND hList, const Uint id, 
						   const Uint type, const unsigned state)
{	
	static Uint index = 0;
	std::string str;

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

	str = "temp";
	SetListviewItem(hList, index, 2, str);

	++index;
}

// ================================================ //

static void UpdateGUI(HWND hwnd, TFC* tfc)
{
	Timer update(true);
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

			case GUIEventType::UPDATE:
				SendDlgItemMessage(hwnd, IDC_LIST_TFC_UPDATES, LB_ADDSTRING, 0,
								   reinterpret_cast<LPARAM>(&e.buffer));
				break;

			case GUIEventType::ASTEROID_FOUND:
				{
					// Insert asteroid data into listview.
					HWND hList = GetDlgItem(hwnd, IDC_LIST_ASTEROIDS);
					InsertListviewItem(hList, asteroidIndex, toString(e.asteroid.id));
					SetListviewItem(hList, asteroidIndex, 1, toString(e.asteroid.mass));
					++asteroidIndex;

					// Update the TFC log.
					SendDlgItemMessage(hwnd, IDC_LIST_TFC_UPDATES, LB_ADDSTRING, 0,
									reinterpret_cast<LPARAM>("New asteroid discovered!"));

					// Update number of asteroids in stack.
					std::string buffer = "Asteroid Stack (Size: " + toString(asteroidIndex) + ")";
					SetDlgItemText(hwnd, IDC_STATIC_LIST_ASTEROIDS_TITLE, buffer.c_str());
				}
				break;

			case GUIEventType::ASTEROID_REMOVED:
				--asteroidIndex;
				{
					// Find listview item for removed asteroid and delete it.
					HWND hList = GetDlgItem(hwnd, IDC_LIST_ASTEROIDS);
					// Find index of listview item with asteroid (e.x holds ID).
					int index = GetListviewItemIndex(hList, 0, toString(e.x));
					if (index != -1){
						SendMessage(hList, LVM_DELETEITEM, static_cast<WPARAM>(index), 0);
					}

					// Update number of asteroids in stack.
					std::string buffer = "Asteroid Stack (Size: " + toString(asteroidIndex) + ")";
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
				}
				break;

			case GUIEventType::SHIELDS_HIT:
				SendDlgItemMessage(hwnd, IDC_PROGRESS_SHIELDS, PBM_SETPOS,
								   static_cast<WPARAM>(tfc->getShields()), 0);
				break;

			case GUIEventType::PROBE_TERMINATED:
				SendDlgItemMessage(hwnd, IDC_PROGRESS_SHIELDS, PBM_SETPOS,
								   static_cast<WPARAM>(tfc->getShields()), 0);
				{
					HWND hList = GetDlgItem(hwnd, IDC_LIST_PROBES);
					int index = GetListviewItemIndex(hList, 0, toString(e.x));
					if (index != -1){
						SendMessage(hList, LVM_DELETEITEM, static_cast<WPARAM>(index), 0);
					}

					// Update number of probes.
					std::string buffer = "Launched Probes (Size: " +
						toString(tfc->getNumProbes()) + ")";
					SetDlgItemText(hwnd, IDC_STATIC_LIST_PROBES_TITLE, buffer.c_str());
				}
				break;
			}
		}

		// Update elapsed time.
		if (update.getTicks() > 1000){
			Uint time = tfc->getCurrentTime() / 1000;
			std::string buf("Current time: " + toString(time));
			SetDlgItemText(hwnd, IDC_STATIC_TIME, buf.c_str());
			update.restart();
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

	// Get next GUIEvent from TFC...
	// perhaps use a thread to check and then force GUI update

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
			lc.pszText = "Status";
			lc.cx = 200;
			SendMessage(hList, LVM_INSERTCOLUMN, 0, reinterpret_cast<LPARAM>(&lc));

			lc.iSubItem = 1;
			lc.pszText = "Type";
			lc.cx = 200;
			SendMessage(hList, LVM_INSERTCOLUMN, 0, reinterpret_cast<LPARAM>(&lc));

			lc.iSubItem = 2;
			lc.pszText = "ID";
			lc.cx = 50;
			SendMessage(hList, LVM_INSERTCOLUMN, 0, reinterpret_cast<LPARAM>(&lc));

			// Shields progress bar.
			HWND hShields = GetDlgItem(hwnd, IDC_PROGRESS_SHIELDS);
			SendMessage(hShields, PBM_SETRANGE, 0, MAKELPARAM(0, 5));
			SendMessage(hShields, PBM_SETPOS, static_cast<WPARAM>(5), 0);

			// Create initial probes.
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

			// Update listview title.
			std::string buf("Launched Probes (Size: " + toString(probes.size()) + std::string(")"));
			SetDlgItemText(hwnd, IDC_STATIC_LIST_PROBES_TITLE, buf.c_str());

			// Create thread for updating time.
			std::thread t(&UpdateGUI, hwnd, &tfc);
			t.detach();
		}
		return FALSE;

	case WM_CTLCOLORDLG:
		return reinterpret_cast<LONG>(hBackground);

	case WM_COMMAND:
		switch (LOWORD(wParam)){
		default:
			break;

		case IDC_BUTTON_START:
			{
				EnableWindow(GetDlgItem(hwnd, IDC_BUTTON_ADD_PROBE), FALSE);
				tfc.enterAsteroidField();
			}			
			break;

		case IDC_BUTTON_ADD_PROBE:
			{				
				std::shared_ptr<Probe> probe(new Probe(Probe::Type::PHASER));
				if (probe->launch() == true){
					probes.push_back(probe);

					AddProbeToList(GetDlgItem(hwnd, IDC_LIST_PROBES), probe->getID(),
								   Probe::Type::PHASER, probe->getState());

					// Update listview title.
					std::string buf("Launched Probes (Size: " + toString(probes.size()) + std::string(")"));
					SetDlgItemText(hwnd, IDC_STATIC_LIST_PROBES_TITLE, buf.c_str());
				}
				else{
					MessageBox(hwnd, "Failed to launch probe!", "ERROR", MB_OK | MB_ICONEXCLAMATION);
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

		EndDialog(hwnd, 0);
		return FALSE;
	}

	return TRUE;
}

// ================================================ //

#ifdef _DEBUG
	int main(int argc, char** argv)
#else
	int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, 
					 LPSTR lpszArg, int nFunsterStil)
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

	srand(GetTickCount());

	int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN), NULL, MainProc);

	// Terminate use of WS2_32.DLL.
	WSACleanup();

	return ret;
}

// ================================================ //