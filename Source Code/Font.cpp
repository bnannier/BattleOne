///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Font.cpp
//	Start Date: 03/10/2005
//	End Date:   05/10/2005
//
//	Assignment:	BattleOne
//	Subject:    GAM666, GAM670, GAM671
//	Programmer: Bobby Nannier
//	Student #:  020 208 013
//	Instructor: Evan Weaver			- GAM666 (Introduction To Game Programming)
//									- GAM670 (Game Programming Techniques)
//				Chris Szalwinski	- GAM671 (Physics)
//				Catherine Leung		- GAM671 (Artificial Intelligence)
//
//	Purpose:	
//
//	Notes:		
///////////////////////////////////////////////////////////////////////////////////////
#include ".\WinMain.h"


// ----------------------------------------------------------------------------------------------------
// Create(): Creates a font object indirectly for both a device and a font
// ----------------------------------------------------------------------------------------------------
BOOL Font::Create(Graphic *Graphics, char *Name, long Size, BOOL Bold, BOOL Italic, BOOL Underline, BOOL Strikeout){
	D3DXFONT_DESC lf;

	if(Graphics == NULL || Name == NULL){
		MessageBox(Graphics->GethWnd(), "Error 1", "Font::Create()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	if(Graphics->GetDeviceCOM() == NULL){
		MessageBox(Graphics->GethWnd(), "Error 2", "Font::Create()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Clear out the font structure
	ZeroMemory(&lf, sizeof(D3DXFONT_DESC));

	// Set the font name and height
	strcpy(lf.FaceName, Name);
	lf.Height = -Size;
	lf.Weight = (Bold == TRUE) ? 700 : 0;
	lf.Italic = Italic;

	// Create the font object
	if(FAILED(D3DXCreateFontIndirect(Graphics->GetDeviceCOM(), &lf, &f_Font))){
		MessageBox(Graphics->GethWnd(), "Error 3", "Font::Create()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Print(): Draws formatted text
// ----------------------------------------------------------------------------------------------------
BOOL Font::Print(char *Text, long XPos, long YPos, long Width, long Height, D3DCOLOR Color, DWORD Format){
	RECT Rect;

	if(f_Font == NULL)
		return FALSE;

	if(!Width)
		Width = 65535;
	if(!Height)
		Height = 65536;

	Rect.left   = XPos;
	Rect.top    = YPos;
	Rect.right  = Rect.left + Width;
	Rect.bottom = Rect.top + Height;

	if(FAILED(f_Font->DrawText(NULL, Text, -1, &Rect, Format, Color)))
		return FALSE;
	return TRUE;
}