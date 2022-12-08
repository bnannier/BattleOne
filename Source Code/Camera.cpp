///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Camera.cpp
//	Start Date: 05/09/2005
//	End Date:   09/09/2005
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
// Camera(): Camera Constructor
// ----------------------------------------------------------------------------------------------------
Camera::Camera(){
	Move(0.0f,0.0f,0.0f);
	Rotate(0.0f,0.0f,0.0f);
	Update();
}


// ----------------------------------------------------------------------------------------------------
// Move(): Builds a matrix using the specified offsets
// ----------------------------------------------------------------------------------------------------
BOOL Camera::Move(float XPos, float YPos, float ZPos){
	c_XPos = XPos;
	c_YPos = YPos;
	c_ZPos = ZPos;

	D3DXMatrixTranslation(&c_matTranslation, -c_XPos, -c_YPos, -c_ZPos);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Rotate(): Builds a matrix that rotates around the x-axis
// ----------------------------------------------------------------------------------------------------
BOOL Camera::Rotate(float XRot, float YRot, float ZRot){
	D3DXMATRIX matXRotation, matYRotation, matZRotation;

	c_XRot = XRot;
	c_YRot = YRot;
	c_ZRot = ZRot;

	D3DXMatrixRotationX(&matXRotation, -c_XRot);
	D3DXMatrixRotationY(&matYRotation, -c_YRot);
	D3DXMatrixRotationZ(&matZRotation, -c_ZRot);

	c_matRotation = matZRotation;
	D3DXMatrixMultiply(&c_matRotation, &c_matRotation, &matYRotation);
	D3DXMatrixMultiply(&c_matRotation, &c_matRotation, &matXRotation);

	return TRUE;
}