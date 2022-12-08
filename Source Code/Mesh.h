///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Mesh.h
//	Start Date: 07/02/2006
//	End Date:   12/03/2006
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
#ifndef _MESH_H_
#define _MESH_H_

#define SHADOWFVF D3DFVF_XYZRHW | D3DFVF_DIFFUSE	// The FVF for the shadow vertex

#include "WinMain.h"

// ----------------------------------------------------------------------------------------------------
// Class Mesh:	This is a wrapper for the DirectX functions that work with models. This class allows
//				for loading, drawing, texturing, and model animation.
// ----------------------------------------------------------------------------------------------------
class Mesh{
protected:
	WorldPosition	m_Pos;
private:
	LPDIRECT3DDEVICE9	m_pd3dDevice;			//The d3d device to use

	//Model
	LPMESHCONTAINER		m_pFirstMesh;			// The first mesh in the hierarchy
	LPD3DXFRAME			m_pFrameRoot;			// Frame hierarchy of the model
	LPD3DXMATRIX		m_pBoneMatrices;		// Used when calculating the bone position
	D3DXVECTOR3			m_vecCenter;			// Center of bounding sphere of object
    float				m_fRadius;				// Radius of bounding sphere of object
	UINT				m_uMaxBones;			// The Max number of bones for the model

	//Animation
	DWORD							m_dwCurrentAnimation;	// The current animation
	DWORD							m_dwAnimationSetCount;	// Number of animation sets
	LPD3DXANIMATIONCONTROLLER		m_pAnimController;		// Controller for the animations

	// Purpose:		Go through each frame and draw the ones that have a mesh container that is not NULL.
	// Parameters:	LPFRAME pFrame				- The frame root
	// Returns:		
	// Notes:		
	void DrawFrame(LPFRAME pFrame);

	// Purpose:		Set up the bone matrices.
	// Parameters:	LPFRAME pFrame				- The frame root
	//				LPD3DXMATRIX pParentMatrix	- Parent matrix
	// Returns:		
	// Notes:		
	void SetupBoneMatrices(LPFRAME pFrame, LPD3DXMATRIX pParentMatrix);

	// Purpose:		Update the frame matrices after animation.
	// Parameters:	LPFRAME pFrame				- Frame to use
	//				LPD3DXMATRIX pParentMatrix	- Matrix passed in
	// Returns:		
	// Notes:		This function does not need to have a parent matrix passed when you
	//				are calling this function just pass the frame root.
	void UpdateFrameMatrices(LPFRAME pFrame, LPD3DXMATRIX pParentMatrix);

public:
	Mesh(LPDIRECT3DDEVICE9 pD3DDevice);		// Constructor
	virtual ~Mesh();						// Destructor

	// Purpose:		Return the center of the bounding sphere.
	// Parameters:	
	// Returns:		LPD3DXVECTOR3				- Pointer to the center vector
	// Notes:		
	inline LPD3DXVECTOR3 GetBoundingSphereCenter(){
		return &m_vecCenter;
	}

	// Purpose:		Return the Radius of the bounding sphere.
	// Parameters:	
	// Returns:		float						- The radius of the bounding sphere
	// Notes:		
	inline float GetBoundingSphereRadius(){
		return m_fRadius;
	}

	// Purpose:		Return the animation being used.
	// Parameters:	
	// Returns:		DWORD						- The current animation
	// Notes:		
	inline DWORD GetCurrentAnimation(){
		return m_dwCurrentAnimation;
	}

	// Purpose:		Set the current animation to the passed in flag.
	// Parameters:	DWORD dwAnimationFlag		- The animation set to use
	// Returns:		
	// Notes:		
	void SetCurrentAnimation(DWORD dwAnimationFlag);
	
	// Purpose:		Draw the model with the device given.
	// Parameters:	
	// Returns:		
	// Notes:		
	void Draw();

	// Purpose:		Load the model from the .X file given.
	// Parameters:	char* strFileName			- The file to load from
	// Returns:		
	// Notes:		
	void LoadXFile(char* strFileName);

// ----------------------------------------------------------------------------------------------------
// Purpose:		Update the model for animation.
// Parameters:	Double dElapsedTime			- The time
// Returns:		
// Notes:		
// ----------------------------------------------------------------------------------------------------
	void Update(double dElapsedTime);


	BOOL Move(float XPos, float YPos, float ZPos){return m_Pos.Move(XPos, YPos, ZPos);}
    BOOL MoveRel(float XAdd, float YAdd, float ZAdd){return m_Pos.MoveRel(XAdd, YAdd, ZAdd);}

    BOOL Rotate(float XRot, float YRot, float ZRot){return m_Pos.Rotate(XRot, YRot, ZRot);}
    BOOL RotateRel(float XAdd, float YAdd, float ZAdd){return m_Pos.RotateRel(XAdd, YAdd, ZAdd);}

    BOOL Scale(float XScale, float YScale, float ZScale){return m_Pos.Scale(XScale, YScale, ZScale);}
    BOOL ScaleRel(float XAdd, float YAdd, float ZAdd){return m_Pos.ScaleRel(XAdd, YAdd, ZAdd);}

	float GetXPos(){return m_Pos.GetXPos();}
    float GetYPos(){return m_Pos.GetYPos();}
    float GetZPos(){return m_Pos.GetZPos();}

	float GetXRotation(){return m_Pos.GetXRotation();}
    float GetYRotation(){return m_Pos.GetYRotation();}
    float GetZRotation(){return m_Pos.GetZRotation();}

	float GetXScale(){return m_Pos.GetXScale();}
    float GetYScale(){return m_Pos.GetYScale();}
    float GetZScale(){return m_Pos.GetZScale();}
};
#endif