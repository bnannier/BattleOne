///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Light.cpp
//	Start Date: 25/09/2005
//	End Date:   28/09/2005
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
// Light(): Light Constructor
// ----------------------------------------------------------------------------------------------------
Light::Light(){
	// Set a default light to point
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));
	SetType(D3DLIGHT_POINT);
	Move(0.0f, 0.0f, 0.0f);
	SetDiffuseColor(255,255,255);
	SetAmbientColor(255,255,255);
	SetRange(1000.0f);
	SetAttenuation0(1.0f);
}


// ----------------------------------------------------------------------------------------------------
// Move(): Set light's position
// ----------------------------------------------------------------------------------------------------
BOOL Light::Move(float XPos, float YPos, float ZPos){
	m_Light.Position.x = XPos;
	m_Light.Position.y = YPos;
	m_Light.Position.z = ZPos;
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetRange(): Set light's range
// ----------------------------------------------------------------------------------------------------
BOOL Light::SetRange(float Range){
	m_Light.Range = Range;
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetAttenuation0(): Set light's Attenuation
// ----------------------------------------------------------------------------------------------------
BOOL Light::SetAttenuation0(float Attenuation){
	m_Light.Attenuation0 = Attenuation;
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetType(): Set light's type
// ----------------------------------------------------------------------------------------------------
BOOL Light::SetType(D3DLIGHTTYPE Type){
	m_Light.Type = Type;
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetDiffuseColor(): Set light's diffusion colour
// ----------------------------------------------------------------------------------------------------
BOOL Light::SetDiffuseColor(char Red, char Green, char Blue){
	m_Light.Diffuse.r = 1.0f / 255.0f * (float)Red;
	m_Light.Diffuse.g = 1.0f / 255.0f * (float)Green;
	m_Light.Diffuse.b = 1.0f / 255.0f * (float)Blue;
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetAmbientColor(): Set light's ambient colour
// ----------------------------------------------------------------------------------------------------
BOOL Light::SetAmbientColor(char Red, char Green, char Blue){
	m_Light.Ambient.r = 1.0f / 255.0f * (float)Red;
	m_Light.Ambient.g = 1.0f / 255.0f * (float)Green;
	m_Light.Ambient.b = 1.0f / 255.0f * (float)Blue;
	return TRUE;
}