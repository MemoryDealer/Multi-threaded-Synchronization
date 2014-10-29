// ================================================ //
// File: GUI.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements GUI helper functions.
// ================================================ //

#include "GUI.hpp"

// ================================================ //

int InsertListviewItem(const HWND hList, const int index,
					   const std::string& text)
{
	LVITEM item = { 0 };

	item.mask = LVIF_TEXT;
	item.iItem = index;
	item.iSubItem = 0;
	item.pszText = const_cast<char*>(text.c_str());
	item.cchTextMax = text.length() + 1;

	return static_cast<int>(SendMessage(hList, LVM_INSERTITEM, 0,
		reinterpret_cast<LPARAM>(&item)));
}

// ================================================ //

BOOL SetListviewItem(const HWND hList, const int index, const int subitem,
					 const std::string& text)
{
	LVITEM item = { 0 };

	item.mask = LVIF_TEXT;
	item.iItem = index;
	item.iSubItem = subitem;
	item.pszText = const_cast<char*>(text.c_str());
	item.cchTextMax = text.length() + 1;

	return static_cast<BOOL>(SendMessage(hList, LVM_SETITEM, 0,
		reinterpret_cast<LPARAM>(&item)));
}

// ================================================ //