///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Model.h
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

#ifndef _MODEL_H_
#define _MODEL_H_

#include <stdio.h>      // for FILE
#include <d3dx9math.h>  // for C++ matrix algebra extensions, etc.

#define M_VERTICES "Vertices"
#define M_FACES    "Faces"
#define M_NORMALS  "Normals"
#define M_COLOURS  "Colours"
#define M_TEXTURES "Textures"

class Thing;

// ----------------------------------------------------------------------------------------------------
// Model holds the model data for a simple Thing, including its vertices, and its triangles
// ----------------------------------------------------------------------------------------------------
class Model {
	D3DXVECTOR3* vertex;
	D3DXVECTOR3* fNormal;
	D3DXVECTOR3* vNormal;
	D3DXVECTOR3* texture;
	char* texFile;
	int* faceVertex1;
	int* faceVertex2;
	int* faceVertex3;
	int  noVertices;
	int  noFaces;
	bool success;
	bool dumpBuild;
	bool normalData;

    void release();
    bool extract(FILE* fp, const char* dataFile, const char* label,
     const char* value);

  public:
    Model(const char* dataFile, bool dump = false);
    operator bool() {return success;}
    int  nVertices() const {return 3 * noFaces;}
    void build(Thing* thing, FLOAT scale, bool smooth = false);
    ~Model() {release();}
};

#endif
