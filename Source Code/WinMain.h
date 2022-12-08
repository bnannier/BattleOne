///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  WinMain.h
//	Start Date: 02/09/2005
//	End Date:   10/05/2005
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
#ifndef _WINMAIN_H_
#define _WINMAIN_H_

#define DIRECTINPUT_VERSION 0x0800
#define _CRT_SECURE_NO_DEPRECATE

// Relese COM Objects
#define ReleaseCOM(x) if(x){x->Release(); x = NULL;}

// Delete an Array safely
#define SAFE_DELETE_ARRAY(p){if(p){delete[] (p); (p)=NULL;}}

// Delete an object pointer
#define SAFE_DELETE(p){if(p){delete (p); (p)=NULL;}}

// Release an object pointer
#define SAFE_RELEASE(p){if(p){(p)->Release(); (p)=NULL;}}

// Forward Decelaration Of Classes
class Graphic;
class Camera;
class Light;
class Font;
class Input;
class DerivedStructs;
class MeshHierarchy;
class Mesh;
class Terrain;
class WorldPosition;

// Windows includes
#include <windows.h>

// Standard ANSI-C includes
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <limits>

// DirectX includes
#include "d3d9.h"
#include "d3dx9.h"
#include "d3dx9anim.h"
#include "dmusici.h"
#include "dsound.h"
#include "dplay8.h"
#include "dpaddr.h"
#include "dinput.h"

// Game includes
#include "Graphic.h"
#include "WorldPosition.h"
#include "Camera.h"
#include "Light.h"
#include "Font.h"
#include "Input.h"
#include "MeshStructs.h"
#include "MeshHierarchy.h"
#include "Mesh.h"
#include "Terrain.h"
#include "pSystem.h"
#include "Thing.h"
using namespace psys;

// Defines for the main only (physics)
#define MAX_STATIONARIES	10
#define MAX_MOBILES			99
#define LIN_VELOCITY_0		500.0f	// Initial linear velocity m/s
#define ANG_VELOCITY_0		200.0f	// Initial angular velocity rad/s
#define MOVE_FACTOR			1.5f	// Move out/in factor
#define boxSpeed			0.01f	// Mobile[0]: Time used devide dt so box rotates at a slower rate
#define cannonSpeed			0.0005f // Moblie[1]: Time used devide dt so particle moves at a slower rate

// ----------------------------------------------------------------------------------------------------
// Class Application:	Main Application Class Used to Create the Game
// ----------------------------------------------------------------------------------------------------
class cApplication{
private:
	LPDIRECT3DDEVICE9 pD3DDevice;

	HINSTANCE		m_hInst;
    HWND			m_hWnd;
	HRESULT			hr;
	int				m_Life;
	int				flag;

    Graphic			m_Graphics;
    Camera			m_Camera;
    Light			m_Light;


	// Input devices
    Input			m_Input;
    InputDevice		m_Keyboard;
    InputDevice		m_Mouse;
	InputDevice		m_Joystick;
	LPDIRECTINPUTEFFECT   joystick;
	bool			m_ForceFeedback;

	// Terrain
	D3DXMATRIX		I;
	Terrain*		level;

	// Meshes

	// Particle Systems
	psys::PSystem*	m_Exp[6];

	// Fog Effects
	ID3DXEffect*	FogEffect;
	D3DXHANDLE		FogTechHandle;
	ID3DXBuffer*	errorBuffer;

	// Physics: Mobile Things
	// Red Box(s)
	VolumeThing	   *mobile[MAX_MOBILES];        // Mobile things
    int				nMobiles;                   // Number of mobile things
    int				nLastingMobiles;			// Number of mobiles that never die

	PlaneThing     *stationary[MAX_STATIONARIES];	// stationary plane things
	int				nStationaries;

	// Camera View cordinates
	float           m_XPos, m_YPos, m_ZPos;
	int				m_tex, m_res_w, m_res_h;

	// Used for displaying game stats
    Font			m_Font;
	DWORD			time;
	DWORD			Timer;
	char			Stats[256];
	int				Stats2;
protected:
	char			w_Class[MAX_PATH];
    char			w_Caption[MAX_PATH];

    WNDCLASSEX		w_wcex;

    DWORD			w_Style;
    DWORD			w_XPos;
    DWORD			w_YPos;
    DWORD			w_Width;
    DWORD			w_Height;
public:
    cApplication();

    HWND			GethWnd(){return m_hWnd;}
    HINSTANCE		GethInst(){return m_hInst;}

    BOOL			Run();
    BOOL			Move(long XPos, long YPos);
    BOOL			Resize(long Width, long Height);
    BOOL			ShowMouse(BOOL Show = TRUE);
	bool			nextEmpty(VolumeThing* mobile[], int nMobiles, int& next);
	
	BOOL			Init(BOOL val = FALSE);
    BOOL			Shutdown();
    BOOL			Frame();
	BOOL			MenuFrame();
};
#endif