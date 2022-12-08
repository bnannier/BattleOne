///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Mesh.cpp
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
#include "WinMain.h"


// ----------------------------------------------------------------------------------------------------
// Mesh(): Mesh Constructor.
// ----------------------------------------------------------------------------------------------------
Mesh::Mesh(LPDIRECT3DDEVICE9 pD3DDevice){
	m_pd3dDevice = pD3DDevice;
	m_pFrameRoot = NULL;
	m_pBoneMatrices = NULL;
	m_vecCenter = D3DXVECTOR3(0.0f,0.0f,0.0f);
    m_fRadius = 0.0f;
	m_dwCurrentAnimation = -1;
	m_dwAnimationSetCount = 0;
	m_uMaxBones = 0;
	m_pAnimController = NULL;
	m_pFirstMesh = NULL;

	Move(0.0f, 0.0f, 0.0f);
	Rotate(0.0f, 0.0f, 0.0f);
	Scale(1.0f, 1.0f, 1.0f);
}


// ----------------------------------------------------------------------------------------------------
// ~Mesh(): Mesh Destructor.
// ----------------------------------------------------------------------------------------------------
Mesh::~Mesh(){
	// Delete Animation Controller
	SAFE_RELEASE(m_pAnimController)
	
	// If there is a frame hierarchyo
	if(m_pFrameRoot){
		// Allocation class
		MeshHierarchy Alloc;
		D3DXFrameDestroy(m_pFrameRoot, &Alloc);
		m_pFrameRoot = NULL;
	}

	// Delete the bones
	SAFE_DELETE_ARRAY(m_pBoneMatrices)

	// Make the Device not point to the other device
	m_pd3dDevice = NULL;
}


// ----------------------------------------------------------------------------------------------------
// DrawFrame(): Go through each frame and draw the ones that have a mesh container that is not NULL.
// ----------------------------------------------------------------------------------------------------
void Mesh::DrawFrame(LPFRAME pFrame){
	LPMESHCONTAINER pMesh = (LPMESHCONTAINER)pFrame->pMeshContainer;

	// While there is a mesh try to draw it
	while(pMesh){
		// Select the mesh to draw
		LPD3DXMESH pDrawMesh = (pMesh->pSkinInfo) ? pMesh->pSkinMesh: pMesh->MeshData.pMesh;
		
		// Draw each mesh subset with correct materials and texture
		for (DWORD i = 0; i < pMesh->NumMaterials; ++i){
			m_pd3dDevice->SetMaterial(&pMesh->pMaterials9[i]);
			m_pd3dDevice->SetTexture(0, pMesh->ppTextures[i]);
			pDrawMesh->DrawSubset(i);
		}

		// Go to the next one
		pMesh = (LPMESHCONTAINER)pMesh->pNextMeshContainer;
	}

	// Check your Sister
	if(pFrame->pFrameSibling)
		DrawFrame((LPFRAME)pFrame->pFrameSibling);

	// Check your Son
	if(pFrame->pFrameFirstChild)
		DrawFrame((LPFRAME)pFrame->pFrameFirstChild);
}


// ----------------------------------------------------------------------------------------------------
// SetupBoneMatrices(): Set up the bone matrices.
// ----------------------------------------------------------------------------------------------------
void Mesh::SetupBoneMatrices(LPFRAME pFrame, LPD3DXMATRIX pParentMatrix){
	LPMESHCONTAINER pMesh = (LPMESHCONTAINER)pFrame->pMeshContainer;

	// Set up the bones on the mesh
	if(pMesh){
		if(!m_pFirstMesh)
			m_pFirstMesh = pMesh;
		
		// If there is a skinmesh, then setup the bone matrices
		if(pMesh->pSkinInfo){
			// Create a copy of the mesh
			pMesh->MeshData.pMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMesh->MeshData.pMesh->GetFVF(), m_pd3dDevice, &pMesh->pSkinMesh);

			if(m_uMaxBones < pMesh->pSkinInfo->GetNumBones()){
				// Get the number of bones
				m_uMaxBones = pMesh->pSkinInfo->GetNumBones();
			}

			LPFRAME pTempFrame = NULL;

			// For each bone 
			for (UINT i = 0; i < pMesh->pSkinInfo->GetNumBones(); i++){   
				// Find the frame
				pTempFrame = (LPFRAME)D3DXFrameFind(m_pFrameRoot, pMesh->pSkinInfo->GetBoneName(i));

				// Set the bone part
				pMesh->ppFrameMatrices[i] = &pTempFrame->matCombined;
			}
		}
	}

	// Check your Sister
	if(pFrame->pFrameSibling)
		SetupBoneMatrices((LPFRAME)pFrame->pFrameSibling, pParentMatrix);

	// Check your Son
	if(pFrame->pFrameFirstChild)
		SetupBoneMatrices((LPFRAME)pFrame->pFrameFirstChild, &pFrame->matCombined);
}


// ----------------------------------------------------------------------------------------------------
// UpdateFrameMatrices(): Update the frame matrices after animation.
// ----------------------------------------------------------------------------------------------------
void Mesh::UpdateFrameMatrices(LPFRAME pFrame, LPD3DXMATRIX pParentMatrix){	
	// Parent check
	if (pParentMatrix){
		D3DXMatrixMultiply(&pFrame->matCombined, &pFrame->TransformationMatrix, pParentMatrix);
	}else
		pFrame->matCombined = pFrame->TransformationMatrix;

	// Do the kid too
	if (pFrame->pFrameSibling){
		UpdateFrameMatrices((LPFRAME)pFrame->pFrameSibling, pParentMatrix);
	}

	// Make sure you get the first kid
	if (pFrame->pFrameFirstChild){
		UpdateFrameMatrices((LPFRAME)pFrame->pFrameFirstChild, &pFrame->matCombined);
	}
}


// ----------------------------------------------------------------------------------------------------
// SetCurrentAnimation(): Set the current animation to the passed in flag.
// ----------------------------------------------------------------------------------------------------
void Mesh::SetCurrentAnimation(DWORD dwAnimationFlag){
	// If the animation is not one that we are already using
	// and the passed in flag is not bigger than the number of animations
	if(dwAnimationFlag != m_dwCurrentAnimation && dwAnimationFlag < m_dwAnimationSetCount){ 
		m_dwCurrentAnimation = dwAnimationFlag;
		LPD3DXANIMATIONSET AnimSet = NULL;
		m_pAnimController->GetAnimationSet(m_dwCurrentAnimation, &AnimSet);
		m_pAnimController->SetTrackAnimationSet(0, AnimSet);
		SAFE_RELEASE(AnimSet)
	}
}


// ----------------------------------------------------------------------------------------------------
// Draw(): Draw the model with the device given.
// ----------------------------------------------------------------------------------------------------
void Mesh::Draw(){
	LPMESHCONTAINER pMesh = m_pFirstMesh;

	// While there is a mesh try to draw it
	while(pMesh){
		// Select the mesh to draw
		LPD3DXMESH pDrawMesh = (pMesh->pSkinInfo) ? pMesh->pSkinMesh : pMesh->MeshData.pMesh;
		
		// Draw each mesh subset with correct materials and texture
		for (DWORD i = 0; i < pMesh->NumMaterials; ++i){
			m_pd3dDevice->SetMaterial(&pMesh->pMaterials9[i]);
			m_pd3dDevice->SetTexture(0, pMesh->ppTextures[i]);
			pDrawMesh->DrawSubset(i);
		}

		// Set object world transformation
		m_pd3dDevice->SetTransform(D3DTS_WORLD, m_Pos.GetMatrix());

		// Go to the next one
		pMesh = (LPMESHCONTAINER)pMesh->pNextMeshContainer;
	}
}


// ----------------------------------------------------------------------------------------------------
// LoadXFile(): Load the model from the .X file given.
// ----------------------------------------------------------------------------------------------------
void Mesh::LoadXFile(char* strFileName){
	// Allocation class
	MeshHierarchy Alloc;
	
	// Load the mesh
	if(FAILED(D3DXLoadMeshHierarchyFromX(strFileName,			// File load
										D3DXMESH_MANAGED,		// Load Options
										m_pd3dDevice,			// D3D Device
										&Alloc,					// Hierarchy allocation class
										NULL,					// NO Effects
										&m_pFrameRoot,			// Frame hierarchy
										&m_pAnimController)))	// Animation Controller
	{
		MessageBox(NULL, strFileName, "Model Load Error", MB_OK);
	}	// End if

	if(m_pAnimController)
		m_dwAnimationSetCount = m_pAnimController->GetMaxNumAnimationSets();

	if(m_pFrameRoot){
		// Set the bones up
		SetupBoneMatrices((LPFRAME)m_pFrameRoot, NULL);

		// Setup the bone matrices array 
		m_pBoneMatrices  = new D3DXMATRIX[m_uMaxBones];
		ZeroMemory(m_pBoneMatrices, sizeof(D3DXMATRIX)*m_uMaxBones);

		// Calculate the Bounding Sphere
		D3DXFrameCalculateBoundingSphere(m_pFrameRoot, &m_vecCenter, &m_fRadius);
	}
}


// ----------------------------------------------------------------------------------------------------
// Update(): Update the model for animation.
// ----------------------------------------------------------------------------------------------------
void Mesh::Update(double dElapsedTime){
	// Set the time for animation
	if(m_pAnimController && m_dwCurrentAnimation != -1)
		m_pAnimController->AdvanceTime(dElapsedTime, NULL);

	// Update the frame hierarchy
	if(m_pFrameRoot){
		UpdateFrameMatrices((LPFRAME)m_pFrameRoot, NULL);
		
		LPMESHCONTAINER pMesh = m_pFirstMesh;
		if(pMesh){
			if(pMesh->pSkinInfo){
				UINT Bones = pMesh->pSkinInfo->GetNumBones();
				for (UINT i = 0; i < Bones; ++i){	
					D3DXMatrixMultiply(&m_pBoneMatrices[i], &pMesh->pBoneOffsets[i], pMesh->ppFrameMatrices[i]);
				}

				// Lock the meshes' vertex buffers
				void *SrcPtr, *DestPtr;
				pMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&SrcPtr);
				pMesh->pSkinMesh->LockVertexBuffer(0, (void**)&DestPtr);

				// Update the skinned mesh using provided transformations
				pMesh->pSkinInfo->UpdateSkinnedMesh(m_pBoneMatrices, NULL, SrcPtr, DestPtr);

				// Unlock the meshes vertex buffers
				pMesh->pSkinMesh->UnlockVertexBuffer();
				pMesh->MeshData.pMesh->UnlockVertexBuffer();
			}
		}
	}
}