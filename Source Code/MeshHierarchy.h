///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  MeshHierarchy.h
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
#ifndef _MESHHIERARCHY_H_
#define _MESHHIERARCHY_H_
#include "WinMain.h"


// ----------------------------------------------------------------------------------------------------
// Class MeshHierarchy:	This is an Allocation class that is used with the D3DXLoadMeshHierarchyFromX
//						function. It handles the Creation and Deletion of Frames and Mesh Containers.
//						The overloaded functions are callbacks so there is no need to call any of the
//						functions in written code just pass an Object of this class to the function.
// ----------------------------------------------------------------------------------------------------
class MeshHierarchy: public ID3DXAllocateHierarchy{
public:
	// Purpose:		Create a frame.
	// Parameters:	THIS_ LPCTSTR Name								- The name of the frame
	//				LPD3DXFRAME *ppNewFrame							- The output new frame
	// Returns:		
	// Notes:		
	STDMETHOD(CreateFrame)(THIS_ LPCTSTR Name, LPD3DXFRAME *ppNewFrame);
    

	// Purpose:		Create a Mesh Container.
	// Parameters:	THIS_ LPCSTR Name								- Name of the Mesh
	//				CONST D3DXMESHDATA *pMeshData					- The mesh Data
	//				CONST D3DXMATERIAL *pMaterials					- That materials of the mesh
	//				CONST D3DXEFFECTINSTANCE *pEffectInstances		- The effects on the mesh
	//				DWORD NumMaterials								- The number of meterials in the mesh
	//				CONST DWORD *pAdjacency							- The adjacency array for the mesh
	//				LPD3DXSKININFO pSkinInfo						- The skin information for the mesh
	//				LPD3DXMESHCONTAINER *ppNewMeshContainer			- The output mesh container
	// Returns:		
	// Notes:		
	STDMETHOD(CreateMeshContainer)(THIS_ LPCSTR Name, CONST D3DXMESHDATA *pMeshData, CONST D3DXMATERIAL *pMaterials, CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials, CONST DWORD *pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER *ppNewMeshContainer);


	// Purpose:		Destroy a frame.
	// Parameters:	THIS_ LPD3DXFRAME pFrameToFree					- The frame to delete
	// Returns:		
	// Notes:		
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
    

	// Purpose:		Destroy a mesh container.
	// Parameters:	THIS_ LPD3DXMESHCONTAINER pMeshContainerBase	-The container to destroy
	// Returns:		
	// Notes:		
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);
};
#endif