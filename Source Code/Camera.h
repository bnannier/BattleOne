///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Camera.h
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
#ifndef _CAMERA_H_
#define _CAMERA_H_


// ----------------------------------------------------------------------------------------------------
// Class Camera: Handles all user view related opperations
// ----------------------------------------------------------------------------------------------------
class Camera{
protected:
    float				c_XPos, c_YPos, c_ZPos;
    float				c_XRot, c_YRot, c_ZRot;

    D3DXMATRIX			c_matWorld;
    D3DXMATRIX			c_matTranslation;
    D3DXMATRIX			c_matRotation;
public:
    Camera();																					// Camera Constructor

	// Update transformation matrix
    BOOL				Update(){																// Determines the product of two matrices
							D3DXMatrixMultiply(&c_matWorld, &c_matTranslation, &c_matRotation);
							return TRUE;
						}

    BOOL				Move(float XPos, float YPos, float ZPos);								// Builds a matrix using the specified offsets
    BOOL				MoveRel(float XAdd, float YAdd, float ZAdd){
							return Move(c_XPos + XAdd, c_YPos + YAdd, c_ZPos + ZAdd);
						}

	BOOL				Rotate(float XRot, float YRot, float ZRot);								// Builds a matrix that rotates around the x-axis
	BOOL				RotateRel(float XAdd, float YAdd, float ZAdd){
							return Rotate(c_XRot + XAdd, c_YRot + YAdd, c_ZRot + ZAdd);
						}

    D3DXMATRIX		   *GetMatrix(){Update(); return &c_matWorld;}

	float				GetXRotation(){return c_XRot;}
	float				GetYRotation(){return c_YRot;}
	float				GetZRotation(){return c_ZRot;}

	void				SetXRotation(float set){c_XRot = set;}
	void				SetYRotation(float set){c_YRot = set;}
	void				SetZRotation(float set){c_ZRot = set;}

	float				GetXPosition(){return c_XPos;}
	float				GetYPosition(){return c_YPos;}
	float				GetZPosition(){return c_ZPos;}
	D3DXVECTOR3			GetPos(){return D3DXVECTOR3(c_XPos, c_YPos, c_ZPos);}

};
#endif