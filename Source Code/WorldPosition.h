///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  WorldPosition.h
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
#ifndef _WORLDPOSITION_H_
#define _WORLDPOSITION_H_

// ----------------------------------------------------------------------------------------------------
// Class WorldPosition, Kepping track of View, Mesh, Objects position relative to the world
// ----------------------------------------------------------------------------------------------------
class WorldPosition{
protected:
    BOOL  m_Billboard;

    float m_XPos,      m_YPos,      m_ZPos;
    float m_XRotation, m_YRotation, m_ZRotation;
    float m_XScale,    m_YScale,    m_ZScale;
    
    D3DXMATRIX m_matWorld;
    D3DXMATRIX m_matScale;
    D3DXMATRIX m_matRotation;
    D3DXMATRIX m_matTranslation;
    D3DXMATRIX *m_matCombine1;
    D3DXMATRIX *m_matCombine2;
public:
    WorldPosition();

    D3DXMATRIX *GetMatrix(Graphic *Graphics = NULL){Update(Graphics); return &m_matWorld;}
    BOOL SetCombineMatrix1(D3DXMATRIX *Matrix = NULL){m_matCombine1 = Matrix; return TRUE;}
    BOOL SetCombineMatrix2(D3DXMATRIX *Matrix = NULL){m_matCombine2 = Matrix; return TRUE;}

    BOOL Copy(WorldPosition *DestPos);

    BOOL Move(float XPos, float YPos, float ZPos);
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd){return Move(m_XPos + XAdd, m_YPos + YAdd, m_ZPos + ZAdd);}

    BOOL Rotate(float XRot, float YRot, float ZRot);
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd){return Rotate(m_XRotation + XAdd, m_YRotation + YAdd, m_ZRotation + ZAdd);}

    BOOL Scale(float XScale, float YScale, float ZScale);
    BOOL ScaleRel(float XAdd, float YAdd, float ZAdd){return Scale(m_XScale + XAdd, m_YScale + YAdd, m_ZScale + ZAdd);}

    BOOL Update(Graphic *Graphics = NULL);
    BOOL EnableBillboard(BOOL Enable = TRUE){m_Billboard = Enable; return TRUE;}

	float GetXPos(){return m_XPos;}
    float GetYPos(){return m_YPos;}
    float GetZPos(){return m_ZPos;}
	float GetXRotation(){return m_XRotation;}
    float GetYRotation(){return m_YRotation;}
    float GetZRotation(){return m_ZRotation;}
	float GetXScale(){return m_XScale;}
    float GetYScale(){return m_YScale;}
    float GetZScale(){return m_ZScale;}
};
#endif