///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  MeshStructs.h
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
#ifndef _MESHSTRUCTS_H_
#define _MESHSTRUCTS_H_


// ----------------------------------------------------------------------------------------------------
// Struct _D3DXMESHCONTAINER_DERIVED:	Derived from the default mesh container
// ----------------------------------------------------------------------------------------------------
typedef struct _D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER{
	//Mesh variables
    LPDIRECT3DTEXTURE9*  ppTextures;		// Textures of the mesh
	D3DMATERIAL9*		 pMaterials9;		// Use the DirectX 9 Material type
	
	//Skinned mesh variables
	LPD3DXMESH           pSkinMesh;			// The skin mesh
	LPD3DXMATRIX		 pBoneOffsets;		// The bone matrix Offsets
	LPD3DXMATRIX*		 ppFrameMatrices;	// Pointer to the Frame Matrix

	// Attribute table stuff
	LPD3DXATTRIBUTERANGE pAttributeTable;	// The attribute table
    DWORD                NumAttributeGroups;// The number of attribute groups
} MESHCONTAINER, *LPMESHCONTAINER;


// ----------------------------------------------------------------------------------------------------
// Struct _D3DXFRAME_DERIVED:	Derived frame struct so it looks like a heirarchy
// ----------------------------------------------------------------------------------------------------
typedef struct _D3DXFRAME_DERIVED: public D3DXFRAME{
	D3DXMATRIX matCombined;	//Combined Transformation Matrix
} FRAME, *LPFRAME;
#endif