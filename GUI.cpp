// ================================================ //
// File: GUI.cpp
// Author: Jordan Sparks
// COSC 4327 Operating Systems Lab 2, Dr. Burris
// ================================================ //
// Implements GUI helper functions.
// ================================================ //

#include "GUI.hpp"

// ================================================ //

const int InsertListviewItem(const HWND hList, const int index,
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

const BOOL SetListviewItem(const HWND hList, const int index, const int subitem,
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

const int GetListviewItemIndex(const HWND hList, const int subitem,
							   const std::string& text)
{
	int max = static_cast<int>(SendMessage(hList, LVM_GETITEMCOUNT, 0, 0));
	char buffer[255] = { 0 };
	LVITEM li = { 0 };
	li.mask = LVIF_TEXT;
	li.iSubItem = subitem;
	li.pszText = buffer;
	li.cchTextMax = sizeof(buffer);
	for (int index = 0; index < max; ++index){
		li.iItem = index;
		// Fill LVITEM struct.
		if (static_cast<int>(SendMessage(
			hList, LVM_GETITEM, 0, reinterpret_cast<LPARAM>(&li)))){
			// Return index if found string matches search string.
			if (std::string(li.pszText).compare(text) == 0){
				return index;
			}
		}
	}

	return -1;
}

// ================================================ //