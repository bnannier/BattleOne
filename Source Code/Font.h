///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Font.h
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
#ifndef _FONT_H_
#define _FONT_H_


// ----------------------------------------------------------------------------------------------------
// Class Font:	Used for drawing text
// ----------------------------------------------------------------------------------------------------
class Font{
private:
    ID3DXFont	   *f_Font;
public:
    Font(){f_Font = NULL;}
   ~Font(){Free();}

    BOOL			Free(){ReleaseCOM(f_Font); return TRUE;}
    ID3DXFont	   *GetFontCOM(){return f_Font;}

    BOOL			Create(Graphic *Graphics, char *Name, long Size = 16, BOOL Bold = FALSE, BOOL Italic = FALSE, BOOL Underline = FALSE, BOOL Strikeout = FALSE);	// Creates a font object indirectly for both a device and a font
    BOOL			Print(char *Text, long XPos, long YPos, long Width = 0, long Height = 0, D3DCOLOR Color = 0xFFFFFFFF, DWORD Format = 0);						// Draws formatted text
};
#endif