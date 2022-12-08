///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  WorldPosition.cpp
//	Start Date: 12/09/2005
//	End Date:   13/09/2005
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
// WorldPosition(): WorldPosition Constructor
// ----------------------------------------------------------------------------------------------------
WorldPosition::WorldPosition(){
	m_Billboard = FALSE;
	m_matCombine1 = m_matCombine2 = NULL;

	Move(0.0f,0.0f,0.0f);
	Rotate(0.0f,0.0f,0.0f);
	Scale(1.0f, 1.0f, 1.0f);
	Update();
}


// ----------------------------------------------------------------------------------------------------
// Copy(): Copy
// ----------------------------------------------------------------------------------------------------
BOOL WorldPosition::Copy(WorldPosition *DestPos){
	DestPos->Move(m_XPos, m_YPos, m_ZPos);
	DestPos->Rotate(m_XRotation, m_YRotation, m_ZRotation);
	DestPos->Scale(m_XScale, m_YScale, m_ZScale);
	DestPos->EnableBillboard(m_Billboard);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Move(): Move
// ----------------------------------------------------------------------------------------------------
BOOL WorldPosition::Move(float XPos, float YPos, float ZPos){
	m_XPos = XPos;
	m_YPos = YPos;
	m_ZPos = ZPos;

	D3DXMatrixTranslation(&m_matTranslation, m_XPos, m_YPos, m_ZPos);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Rotate(): Rotate
// ----------------------------------------------------------------------------------------------------
BOOL WorldPosition::Rotate(float XRot, float YRot, float ZRot){
	m_XRotation = XRot;
	m_YRotation = YRot;
	m_ZRotation = ZRot;

	D3DXMatrixRotationYawPitchRoll(&m_matRotation, m_YRotation, m_XRotation, m_ZRotation);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Scale(): Scale
// ----------------------------------------------------------------------------------------------------
BOOL WorldPosition::Scale(float XScale, float YScale, float ZScale){
	m_XScale = XScale;
	m_YScale = YScale;
	m_ZScale = ZScale;

	D3DXMatrixScaling(&m_matScale, XScale, YScale, ZScale);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Update(): Update position
// ----------------------------------------------------------------------------------------------------
BOOL WorldPosition::Update(Graphic *Graphics){
	D3DXMATRIX matView, matTransposed;

	// Setup billboarding matrix
	if(m_Billboard == TRUE) {
		if(Graphics != NULL && Graphics->GetDeviceCOM() != NULL) {
			Graphics->GetDeviceCOM()->GetTransform(D3DTS_VIEW, &matView);
			D3DXMatrixTranspose(&matTransposed, &matView);
			matTransposed._41 = matTransposed._42 = matTransposed._43 = matTransposed._14 = matTransposed._24 = matTransposed._34 = 0.0f;
		} else {
			D3DXMatrixIdentity(&matTransposed);
		}
	}

	// Combine scaling and rotation matrices first
	D3DXMatrixMultiply(&m_matWorld, &m_matScale, &m_matRotation);

	// Apply billboard matrix
	if(m_Billboard == TRUE)
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &matTransposed);  

	// Combine with translation matrix
	D3DXMatrixMultiply(&m_matWorld, &m_matWorld, &m_matTranslation);

	// Combine with combined matrices (if any)
	if(m_matCombine1 != NULL) 
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine1);
	if(m_matCombine2 != NULL) 
		D3DXMatrixMultiply(&m_matWorld, &m_matWorld, m_matCombine2);
	return TRUE;
}