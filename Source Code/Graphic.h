///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Graphic.h
//	Start Date: 02/09/2005
//	End Date:   
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
#ifndef _GRAPHIC_H_
#define _GRAPHIC_H_


// ----------------------------------------------------------------------------------------------------
// Class Graphic:	Handles all D3D related opperations
// ----------------------------------------------------------------------------------------------------
class Graphic{
protected:
    HWND				g_hWnd;			// Window handle to the message
	LPDIRECT3D9		    g_D3D;			// This interface includes methods for enumerating and retrieving capabilities of the device
    LPDIRECT3DDEVICE9   g_D3DDevice;	// Creates an instance of an LPDIRECT3D9 object
	LPD3DXSPRITE		g_Sprite;		// This interface provides a set of methods that simplify the process of drawing sprites using Microsoft Direct3D

	D3DDISPLAYMODE		g_d3ddm;		// Describes the display mode

    BOOL				g_Windowed;		// If the application is windowed or not
    BOOL				g_ZBuffer;		// If zBuffer is enabled or not
	BOOL				g_HAL;			// D3D Device type

    long				g_Width;		// Width of window
    long				g_Height;		// Height of window
    char				g_BPP;			// Should only be set to 32 or 24 or 16 or 8

    char				g_AmbientRed;	// Red value of RGB
    char				g_AmbientGreen;	// Green value of RGB
    char				g_AmbientBlue;	// Blue Value of RGB
public:
	Graphic();																														// Graphics Constructor
   ~Graphic(){Shutdown();}																											// Graphics Destructor

	BOOL				Shutdown();																									// Release the COM objects (Related to Graphic only)
	BOOL				Init();																										// Create LPDIRECT3D9

	BOOL				SetMode(HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width = 0, long Height = 0, char BPP = 0);			// Setup all COM objects related to this class
	BOOL				SetPerspective(float FOV=D3DX_PI / 4.0f, float Aspect=1.3333f, float Near=1.0f, float Far=10000.0f);		// Setup a left-handed perspective projection matrix based on a field of view
    BOOL				SetCamera(Camera *Camera);																					// Setup camera view
    BOOL				SetLight(long Num, Light *Light);																			// Setup light
	BOOL				SetAmbientLight(char Red, char Green, char Blue);															// Setup the colour of the light

	// The following functions set the games render state
	BOOL				EnableLight(long Num, BOOL Enable = TRUE);																	
    BOOL				EnableLighting(BOOL Enable = TRUE);
    BOOL				EnableZBuffer(BOOL Enable = TRUE);
    BOOL				EnableAlphaBlending(BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);
    BOOL				EnableAlphaTesting(BOOL Enable = TRUE);

	// Get Functions, which retrive protected variables
    LPDIRECT3D9		    GetDirect3DCOM(){return g_D3D;}
    LPDIRECT3DDEVICE9   GetDeviceCOM(){return g_D3DDevice;}
	LPD3DXSPRITE	    GetSpriteCOM(){return g_Sprite;}
	char				GetFormatBPP(D3DFORMAT Format);
	HWND				GethWnd(){return g_hWnd;}			// Return the parent window handle

	BOOL				SetBPP(char format);
	D3DDISPLAYMODE		GetDisplayMode(){return g_d3ddm;}
	BOOL				SetDisplayMode(long width, long height){g_d3ddm.Width = width; g_d3ddm.Height = height; return TRUE;}

    BOOL				CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);														// Check whether a hardware accelerated device type can be used on this adapter
	BOOL				Clear(long Color = 0, float ZBuffer = 1.0f);																// The clear target is both the surface and the z-buffer
	BOOL				ClearDisplay(long Color = 0);																				// The clear target is the surface
    BOOL				ClearZBuffer(float ZBuffer = 1.0f);																			// The clear target is the z-buffer

	BOOL				BeginScene();																								// Begins a scene
    BOOL				EndScene();																									// End a scene
	BOOL				Display();																									// Presents the contents
};
#endif