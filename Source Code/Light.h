///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Light.h
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
#ifndef _LIGHT_H_
#define _LIGHT_H_


// ----------------------------------------------------------------------------------------------------
// Class Light:	Used for Lights
// ----------------------------------------------------------------------------------------------------
class Light{
protected:
    D3DLIGHT9 m_Light;
public:
    Light();												// Light Constructor
	BOOL Move(float XPos, float YPos, float ZPos);			// Set light's position
	BOOL SetRange(float Range);								// Set light's range
	BOOL SetAttenuation0(float Attenuation);				// Set light's Attenuation
	BOOL SetType(D3DLIGHTTYPE Type);						// Set light's type
	BOOL SetDiffuseColor(char Red, char Green, char Blue);	// Set light's diffusion colour
	BOOL SetAmbientColor(char Red, char Green, char Blue);	// Set light's ambient colour
	D3DLIGHT9 *GetLight(){return &m_Light;}
};
#endif