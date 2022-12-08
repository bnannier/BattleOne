///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Utility.cpp
//	Start Date: 07/09/2005
//	End Date:   04/05/2006
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
#include "Utility.h"
#include <stdio.h>      // for fopen, fprintf, fclose

// vertex formats
const DWORD d3d::Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


D3DLIGHT9 d3d::InitDirectionalLight(D3DXVECTOR3* direction, D3DXCOLOR* color){
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_DIRECTIONAL;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Direction = *direction;

	return light;
}

D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* color){
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	return light;
}

D3DLIGHT9 d3d::InitSpotLight(D3DXVECTOR3* position, D3DXVECTOR3* direction, D3DXCOLOR* color){
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));

	light.Type      = D3DLIGHT_SPOT;
	light.Ambient   = *color * 0.4f;
	light.Diffuse   = *color;
	light.Specular  = *color * 0.6f;
	light.Position  = *position;
	light.Direction = *direction;
	light.Range        = 1000.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;
	light.Theta        = 0.5f;
	light.Phi          = 0.7f;

	return light;
}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, D3DXCOLOR s, D3DXCOLOR e, float p){
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = p;
	return mtrl;
}

d3d::BoundingBox::BoundingBox(){
	// infinite small 
	_min.x = d3d::INFINITY;
	_min.y = d3d::INFINITY;
	_min.z = d3d::INFINITY;

	_max.x = -d3d::INFINITY;
	_max.y = -d3d::INFINITY;
	_max.z = -d3d::INFINITY;
}

bool d3d::BoundingBox::isPointInside(D3DXVECTOR3& p){
	if(p.x >= _min.x && p.y >= _min.y && p.z >= _min.z && p.x <= _max.x && p.y <= _max.y && p.z <= _max.z)
		return true;
	else
		return false;
}

d3d::BoundingSphere::BoundingSphere(){
	_radius = 0.0f;
}

bool d3d::DrawBasicScene(IDirect3DDevice9* device, float scale){
	static IDirect3DVertexBuffer9* floor  = 0;
	static IDirect3DTexture9*      tex    = 0;
	static ID3DXMesh*              pillar = 0;

	HRESULT hr = 0;

	if(device == 0){
		if(floor && tex && pillar){
			// they already exist, destroy them
			d3d::Release<IDirect3DVertexBuffer9*>(floor);
			d3d::Release<IDirect3DTexture9*>(tex);
			d3d::Release<ID3DXMesh*>(pillar);
		}
	}
	else if(!floor && !tex && !pillar){
		// they don't exist, create them
		device->CreateVertexBuffer(
			6 * sizeof(d3d::Vertex),
			0, 
			d3d::Vertex::FVF,
			D3DPOOL_MANAGED,
			&floor,
			0);

		Vertex* v = 0;
		floor->Lock(0, 0, (void**)&v, 0);

		v[0] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[1] = Vertex(-20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
		v[2] = Vertex( 20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

		v[3] = Vertex(-20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
		v[4] = Vertex( 20.0f, -2.5f,  20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
		v[5] = Vertex( 20.0f, -2.5f, -20.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

		floor->Unlock();

		D3DXCreateCylinder(device, 0.5f, 0.5f, 5.0f, 20, 20, &pillar, 0);

		D3DXCreateTextureFromFile(
			device,
			"desert.bmp",
			&tex);
	}else{
		// Pre-Render Setup
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		D3DXVECTOR3 dir(0.707f, -0.707f, 0.707f);
		D3DXCOLOR col(1.0f, 1.0f, 1.0f, 1.0f);
		D3DLIGHT9 light = d3d::InitDirectionalLight(&dir, &col);

		device->SetLight(0, &light);
		device->LightEnable(0, true);
		device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
		device->SetRenderState(D3DRS_SPECULARENABLE, true);

		// Render
		D3DXMATRIX T, R, P, S;

		D3DXMatrixScaling(&S, scale, scale, scale);

		// used to rotate cylinders to be parallel with world's y-axis
		D3DXMatrixRotationX(&R, -D3DX_PI * 0.5f);

		// draw floor
		D3DXMatrixIdentity(&T);
		T = T * S;
		device->SetTransform(D3DTS_WORLD, &T);
		device->SetMaterial(&d3d::WHITE_MTRL);
		device->SetTexture(0, tex);
		device->SetStreamSource(0, floor, 0, sizeof(Vertex));
		device->SetFVF(Vertex::FVF);
		device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		
		// draw pillars
		device->SetMaterial(&d3d::BLUE_MTRL);
		device->SetTexture(0, 0);
		for(int i = 0; i < 5; i++){
			D3DXMatrixTranslation(&T, -5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);

			D3DXMatrixTranslation(&T, 5.0f, 0.0f, -15.0f + (i * 7.5f));
			P = R * T * S;
			device->SetTransform(D3DTS_WORLD, &P);
			pillar->DrawSubset(0);
		}
	}
	return true;
}

float d3d::GetRandomFloat(float lowBound, float highBound){
	if( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = (rand() % 10000) * 0.0001f; 

	// return float in [lowBound, highBound] interval. 
	return (f * (highBound - lowBound)) + lowBound; 
}

void d3d::GetRandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max){
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

DWORD d3d::FtoDw(float f){
	return *((DWORD*)&f);
}

float d3d::Lerp(float a, float b, float t){
	return a - (a*t) + (b*t);
}


// Game Programming Simulation GAM671/DPS906
//
// Utilities - Implementation File
// utilities.cpp
// Jan 29 2006
// Chris Szalwinski
//
// Jan 29 2006 - added logMsg message display for faulty model input
// Jan 24 2006 - added logVec vector display function for debugging
// Jan  9 2006 - original
// for description of original code see handler.cpp



// Displays a "DirectX Error" message box
// and logs the message msg to file error.log
//
void error(const char* msg, HWND hwnd) {

    MessageBox(hwnd, msg, "DirectX Error", MB_OK);
    FILE *fp = fopen("error.log", "a");
    fprintf(fp, "%s\n", msg);
    fclose(fp);
}

// logMsg logs a message to file error.log
//
void logMsg(const char* msg) {

    FILE *fp = fopen("error.log", "a");
    fprintf(fp, "%s\n", msg);
    fclose(fp);
}

// logVec logs the components of a vector to file error.log
//
void logVec(const char* msg, const D3DXVECTOR3* v) {

    FILE *fp = fopen("error.log", "a");
    fprintf(fp, "%10.3f %10.3f %10.3f %s\n", v->x, v->y, v->z, msg);
    fclose(fp);
}

// logInt logs a triple of ints to file error.log
//
void logInt(const char* msg, int i, int j, int k) {

    FILE *fp = fopen("error.log", "a");
    fprintf(fp, "%10d %10d %10d %s\n", i, j, k, msg);
    fclose(fp);
}

// orthoNormalize orthogonalizes and normalizes a 4 by 4 matrix in
// stores the result in *out and returns the address out
//
D3DXMATRIX* orthoNormalize(D3DXMATRIX* out, const D3DXMATRIX* in) {

	D3DXVECTOR3 r1(in->_11, in->_12, in->_13);
	D3DXVECTOR3 r2(in->_21, in->_22, in->_23);
	D3DXVECTOR3 r3(in->_31, in->_32, in->_33);
	D3DXVECTOR3 n1, n2, n3, x;
	D3DXVec3Normalize(&n1, &r1);
	D3DXVec3Cross(&x, &r2, &n1);
	D3DXVec3Normalize(&n2, &x);
	D3DXVec3Cross(&n3, &n2, &n1);
	D3DXMatrixIdentity(out);
	out->_11 = n1.x;
	out->_12 = n1.y;
	out->_13 = n1.z;
	out->_21 = n2.x;
	out->_22 = n2.y;
	out->_23 = n2.z;
	out->_31 = n3.x;
	out->_32 = n3.y;
	out->_33 = n3.z;
	return out;
}