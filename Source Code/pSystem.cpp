///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  pSystem.cpp
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

#include <cstdlib>
#include "pSystem.h"

using namespace psys;

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

// ----------------------------------------------------------------------------------------------------
// PSystem(): PSystem Constructor
// ----------------------------------------------------------------------------------------------------
PSystem::PSystem(){
	_device = 0;
	_vb     = 0;
	_tex    = 0;
}

// ----------------------------------------------------------------------------------------------------
// PSystem(): PSystem Destructor
// ----------------------------------------------------------------------------------------------------
PSystem::~PSystem(){
	d3d::Release<IDirect3DVertexBuffer9*>(_vb);
	d3d::Release<IDirect3DTexture9*>(_tex);
}

// ----------------------------------------------------------------------------------------------------
// init(): Initialize a vertex buffer that can be rendered and render the geometry. 
// ----------------------------------------------------------------------------------------------------
bool PSystem::init(IDirect3DDevice9* device, char* texFileName){
	// Vertex buffer's size does not equal the number of particles in our system.
    // Use the vertex buffer to draw a portion of our particles at a time.
    // The arbitrary size we choose for the vertex buffer is specified by the _vbSize variable.

	_device = device; // save a ptr to the device

	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
		_vbSize * sizeof(Particle),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&_vb,
		0);
	
	if(FAILED(hr)){
		::MessageBox(0, "CreateVertexBuffer() - FAILED", "PSystem", 0);
		return false;
	}

	hr = D3DXCreateTextureFromFile(
		device,
		texFileName,
		&_tex);

	if(FAILED(hr)){
		::MessageBox(0, "D3DXCreateTextureFromFile() - FAILED", "PSystem", 0);
		return false;
	}

	return true;
}

// ----------------------------------------------------------------------------------------------------
// reset(): Reset Particles
// ----------------------------------------------------------------------------------------------------
void PSystem::reset(){
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++){
		resetParticle( &(*i) );
	}
}

// ----------------------------------------------------------------------------------------------------
// addParticle(): Add a single instance of a particle
// ----------------------------------------------------------------------------------------------------
void PSystem::addParticle(){
	Attribute attribute;

	resetParticle(&attribute);

	_particles.push_back(attribute);
}

// ----------------------------------------------------------------------------------------------------
// preRender(): Render a particle
// ----------------------------------------------------------------------------------------------------
void PSystem::preRender(){
	_device->SetRenderState(D3DRS_LIGHTING, false);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
	_device->SetRenderState(D3DRS_POINTSIZE, d3d::FtoDw(_size));
	_device->SetRenderState(D3DRS_POINTSIZE_MIN, d3d::FtoDw(0.0f));

	// control the size of the particle relative to distance
	_device->SetRenderState(D3DRS_POINTSCALE_A, d3d::FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_B, d3d::FtoDw(0.0f));
	_device->SetRenderState(D3DRS_POINTSCALE_C, d3d::FtoDw(1.0f));
		
	// use alpha from texture
	_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

// ----------------------------------------------------------------------------------------------------
// postRender(): Set Render States
// ----------------------------------------------------------------------------------------------------
void PSystem::postRender(){
	_device->SetRenderState(D3DRS_LIGHTING,          true);
	_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
	_device->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
	_device->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
}

// ----------------------------------------------------------------------------------------------------
// render(): Render the vertex buffer with data
// ----------------------------------------------------------------------------------------------------
void PSystem::render(){
	// The render method works by filling a section of the vertex buffer with data,
	// then we render that section.  While that section is rendering we lock a new
	// section and begin to fill that section.  Once that sections filled we render it.
	// This process continues until all the particles have been drawn.  The benifit
	// of this method is that we keep the video card and the CPU busy.  

	if( !_particles.empty() ){
		// set render states
		preRender();
		
		_device->SetTexture(0, _tex);
		_device->SetFVF(Particle::FVF);
		_device->SetStreamSource(0, _vb, 0, sizeof(Particle));

		// render batches one by one
		// start at beginning if we're at the end of the vb
		if(_vbOffset >= _vbSize)
			_vbOffset = 0;

		Particle* v = 0;

		_vb->Lock(
			_vbOffset    * sizeof( Particle ),
			_vbBatchSize * sizeof( Particle ),
			(void**)&v,
			_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		// Until all particles have been rendered.
		std::list<Attribute>::iterator i;
		for(i = _particles.begin(); i != _particles.end(); i++){
			if( i->_isAlive ){
				// Copy a batch of the living particles to the
				// next vertex buffer segment
				v->_position = i->_position;
				v->_color    = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; //increase batch counter

				// if this batch full?
				if(numParticlesInBatch == _vbBatchSize) {
					// Draw the last batch of particles that was
					// copied to the vertex buffer. 
					_vb->Unlock();

					_device->DrawPrimitive(
						D3DPT_POINTLIST,
						_vbOffset,
						_vbBatchSize);

					// While that batch is drawing, start filling the
					// next batch with particles.

					// move the offset to the start of the next batch
					_vbOffset += _vbBatchSize; 

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					if(_vbOffset >= _vbSize) 
						_vbOffset = 0;       

					_vb->Lock(
						_vbOffset    * sizeof( Particle ),
						_vbBatchSize * sizeof( Particle ),
						(void**)&v,
						_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					numParticlesInBatch = 0; // reset for new batch
				}	
			}
		}

		_vb->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.
		
		if( numParticlesInBatch )
		{
			_device->DrawPrimitive(
				D3DPT_POINTLIST,
				_vbOffset,
				numParticlesInBatch);
		}

		// next block
		_vbOffset += _vbBatchSize; 

		// reset render states
		postRender();
	}
}

// ----------------------------------------------------------------------------------------------------
// isEmpty(): Check if particles is empty
// ----------------------------------------------------------------------------------------------------
bool PSystem::isEmpty(){
	return _particles.empty();
}

// ----------------------------------------------------------------------------------------------------
// isDead(): Check if there is at least one particle in the system.
// ----------------------------------------------------------------------------------------------------
bool PSystem::isDead(){
	std::list<Attribute>::iterator i;
	for(i = _particles.begin(); i != _particles.end(); i++){
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if( i->_isAlive )
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}

// ----------------------------------------------------------------------------------------------------
// removeDeadParticles(): Remove all dead particles
// ----------------------------------------------------------------------------------------------------
void PSystem::removeDeadParticles(){
	std::list<Attribute>::iterator i;

	i = _particles.begin();

	while( i != _particles.end() ){
		if( i->_isAlive == false ){
			// erase returns the next iterator, so no need to
		    // incrememnt to the next one ourselves.
			i = _particles.erase(i); 
		}else{
			i++; // next in list
		}
	}
}

// ----------------------------------------------------------------------------------------------------
// Firework(): Fireworks Constructor
// ----------------------------------------------------------------------------------------------------
Firework::Firework(D3DXVECTOR3* origin, int numParticles){
	_origin        = *origin;
	_size          = 0.9f;
	_vbSize        = 2048;
	_vbOffset      = 0;   
	_vbBatchSize   = 512; 

	for(int i = 0; i < numParticles; i++)
		addParticle();
}

// ----------------------------------------------------------------------------------------------------
// resetParticle(): Rest particles for Fireworks
// ----------------------------------------------------------------------------------------------------
void Firework::resetParticle(Attribute* attribute){
	attribute->_isAlive  = true;
	attribute->_position = _origin;

	D3DXVECTOR3 min = D3DXVECTOR3(-1.0f, -1.0f, -1.0f);
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f,  1.0f,  1.0f);

	d3d::GetRandomVector(
		&attribute->_velocity,
		&min,
		&max);

	// normalize to make spherical
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity);

	attribute->_velocity *= 100.0f;

	attribute->_color = D3DXCOLOR(
		d3d::GetRandomFloat(0.0f, 1.0f),
		d3d::GetRandomFloat(0.0f, 1.0f),
		d3d::GetRandomFloat(0.0f, 1.0f),
		1.0f);

	attribute->_age      = 0.0f;
	attribute->_lifeTime = 2.0f; // lives for 2 seconds
}

// ----------------------------------------------------------------------------------------------------
// update(): Update Particles
// ----------------------------------------------------------------------------------------------------
void Firework::update(float timeDelta)
{
	std::list<Attribute>::iterator i;

	for(i = _particles.begin(); i != _particles.end(); i++)
	{
		// only update living particles
		if( i->_isAlive )
		{
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			if(i->_age > i->_lifeTime) // kill 
				i->_isAlive = false;
		}
	}
}

// ----------------------------------------------------------------------------------------------------
// preRender(): Read Particles and set render States
// ----------------------------------------------------------------------------------------------------
void Firework::preRender()
{
	PSystem::preRender();

	_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
    _device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// read, but don't write particles to z-buffer
	_device->SetRenderState(D3DRS_ZWRITEENABLE, false);
}

// ----------------------------------------------------------------------------------------------------
// postRender(): Set render states
// ----------------------------------------------------------------------------------------------------
void Firework::postRender()
{
	PSystem::postRender();

	_device->SetRenderState(D3DRS_ZWRITEENABLE, true);
}