///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Thing.cpp
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
//	Purpose:	Represents a 3D terrain.
//
//	Notes:
//  Feb 19 2006 - compatibility between contact and collision partly resolved
//                by monitoring direction of relative velocity and returning
//                penetration flag from contactWith - needs further work for
//                improve robustness and capture penetration within BoundedBody
//                version of contactWith - should also reduce need for double
//                call to contactWith from contactBtwn
//  Feb 14 2006 - added coefficient of restitution
//                added BoundedBody class - collision works for boxes now
//                added BoundedPlane class - collision works on the lake
//  Feb  9 2006 - added collision detection and collision response - but
//                had to disable friction - preliminary version only needs
//                further work on detection and including friction
//  Feb  7 2006 - added contact friction
//  Feb  2 2006 - uncoupled ThingModel class into a separate module called
//                Model - changed class name to Model
//              - added moments, moment of inertia, and ctor parameters to
//                accept moment of inertia data; also applyMoment()
//  Jan 31 2006 - added forces (impulse and body), mass, centre of mass to
//                VolumeThing, along with applyForce(), applyBodyForce();
//                ctors now accept mass as 1st parameter; rotate transforms
//                centreMass; update determines acceleration from forces;
//                start and augment apply a force instead of an acceleration;
//                clonePart() changed to cut(), which handles mass reduction;
//              - combined ParticleBox & BoxeyCannon into SheddingVolume class
//              - fixed BoxeyCannon launching
//              - Box now derived from Thing
//              - added smooth to the Model class to smooth normals
//  Jan 29 2006 - major rewrite to accomodate and consolidate angular
//                motion capabilities
//              - Thing base class: added copyWorld, addBrick, calls to
//                rotate from rotatex,y,z, removed derived class functions
//                for rotatex,y,z
//              - added a new Brick class
//              - combined ChamberBox and Particle Box logic into a
//                new ContainerVolume class - derived SheddingVolume and
//                SheddingVolume from this new ContainerVolume
//              - added AttachableVolume as the base class for RevolvingBrick
//                (previously RevolvingParticle)
//              - removed unused classes
//  Jan 24 2006 - added angular motion capabilities
//              - revised VolumeThing and ChamberedBox classes
//              - added ParticleBox and Revolving Particle classes
//  Jan 17 2006 - added rotate function to Thing class
//              - added velocity, launch direction and acceleration
//                instance variables to VolumeThing class plus
//                rotate, start, update, finished member functions
//                for mobile VolumeThings
//              - added constructor to Box class for specifying the
//                initial instance variables and world transformation
//                matrix externally
//              - derived ChamberedBox class from Box for tracking the
//                position of a launch chamber within a Box and for
//               launching new Boxes from within the chamber itself
///////////////////////////////////////////////////////////////////////////////////////

#include "Model.h"
#include "Thing.h"
#include "Utility.h"  // for error(const char*, HWND)


// ----------------------------------------------------------------------------------------------------
// a Thing in our 3-D universe is made of some material, may have an
// associated texture and can be rotated, moved, and drawn.
// ----------------------------------------------------------------------------------------------------
Thing::Thing(LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices){
    type = t;
    maxcount = maxvertices;
    primitivecnt = count = 0;

    D3DXMatrixIdentity(&world);

    if (FAILED(c->CreateVertexBuffer(maxcount * sizeof(ULTVERTEX), 0, D3DFVF_ULTVERTEX, D3DPOOL_DEFAULT, &vb, NULL))) {
        error("Couldn't make vertex buffer");
        vb = NULL;
    }

    tex = NULL;
    if (texfile && FAILED(D3DXCreateTextureFromFile(c, texfile, &tex))) {
        error("Couldn't load texture");
        tex = NULL;
    }

    // make a shiny thing of the specified color
    ZeroMemory(&mat, sizeof(mat));
    mat.Ambient = D3DXCOLOR(color.r*0.7f, color.g*0.7f, color.b*0.7f, color.a);
    mat.Diffuse = color;                            // reflected from lights
    mat.Specular = D3DXCOLOR(1, 1, 1, color.a);     // shine from lights
    mat.Power = 100; // 0 if it shouldn't be shiny
}


// ----------------------------------------------------------------------------------------------------
// Destructor
// ----------------------------------------------------------------------------------------------------
Thing::~Thing() {
    if (vb)
        vb->Release();
    if (tex)
        tex->Release();
}


// ----------------------------------------------------------------------------------------------------
// add adds one vertex {x, y, z} plus a normal vector {nx, ny, nz} to the
// vertex buffer.  newprimitive indicates whether or not this vertex
// completes a primitive (e.g. triangle, line segment).
//
// Note how the vertex buffer needs to be locked and unlocked when
// writing to it.
// ----------------------------------------------------------------------------------------------------
void Thing::add(FLOAT x, FLOAT y, FLOAT z, FLOAT nx, FLOAT ny, FLOAT nz, FLOAT tu, FLOAT tv, int newprimitive) {
    ULTVERTEX *pv;
    if (count < maxcount && vb && SUCCEEDED(vb->Lock(count*sizeof(ULTVERTEX), sizeof(ULTVERTEX), (void**)&pv, 0))) {
        pv->x = x;
        pv->y = y;
        pv->z = z;
        pv->nx = nx;
        pv->ny = ny;
        pv->nz = nz;
        pv->tu = tu;
        pv->tv = tv;
        vb->Unlock();
        count++;
        if (newprimitive)
            primitivecnt++;
    }
}


// ----------------------------------------------------------------------------------------------------
// addside adds a side of a Brick to the vertex buffer (see Thing::add)
// ----------------------------------------------------------------------------------------------------
void Thing::addside(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3, D3DXVECTOR3 p4, D3DXVECTOR3 normal) {
    add(p1.x, p1.y, p1.z, normal.x, normal.y, normal.z, 0, 0, 0);
    add(p2.x, p2.y, p2.z, normal.x, normal.y, normal.z, 0, 1, 0);
    add(p3.x, p3.y, p3.z, normal.x, normal.y, normal.z, 1, 1, 1);
    add(p1.x, p1.y, p1.z, normal.x, normal.y, normal.z, 0, 0, 0);
    add(p3.x, p3.y, p3.z, normal.x, normal.y, normal.z, 1, 1, 0);
    add(p4.x, p4.y, p4.z, normal.x, normal.y, normal.z, 1, 0, 1);
}


// ----------------------------------------------------------------------------------------------------
// addBrick adds a brick to the vertex buffer.  Used by the ctors only. (see addside)
// ----------------------------------------------------------------------------------------------------
void Thing::addBrick(FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx, FLOAT maxy, FLOAT maxz) {
    D3DXVECTOR3 p1 = D3DXVECTOR3(minx, miny, minz),
                p2 = D3DXVECTOR3(minx, maxy, minz),
                p3 = D3DXVECTOR3(maxx, maxy, minz),
                p4 = D3DXVECTOR3(maxx, miny, minz),
                p5 = D3DXVECTOR3(minx, miny, maxz),
                p6 = D3DXVECTOR3(minx, maxy, maxz),
                p7 = D3DXVECTOR3(maxx, maxy, maxz),
                p8 = D3DXVECTOR3(maxx, miny, maxz);
    addside(p1, p2, p3, p4, D3DXVECTOR3(0, 0, -1)); // front
    addside(p4, p3, p7, p8, D3DXVECTOR3(1, 0, 0));  // right
    addside(p8, p7, p6, p5, D3DXVECTOR3(0, 0, 1));  // back
    addside(p6, p2, p1, p5, D3DXVECTOR3(-1, 0, 0)); // left
    addside(p1, p4, p8, p5, D3DXVECTOR3(0, -1, 0)); // bottom
    addside(p2, p6, p7, p3, D3DXVECTOR3(0, 1, 0));  // top
}


// ----------------------------------------------------------------------------------------------------
// draw draws a Thing; assumes that BeginScene() has been called
// ----------------------------------------------------------------------------------------------------
void Thing::draw(LPDIRECT3DDEVICE9 c) {
    c->SetStreamSource(0, vb, 0, sizeof(ULTVERTEX));
    c->SetTransform(D3DTS_WORLD, &world);
    c->SetMaterial(&mat);
    if (tex)
        c->SetTexture(0, tex);
    c->DrawPrimitive(type, 0, primitivecnt);
    if (tex)
        c->SetTexture(0, NULL);
}


// ----------------------------------------------------------------------------------------------------
// rotatex,y,z rotate a Thing "rad" radians about the x,y,z axis
// ----------------------------------------------------------------------------------------------------
void Thing::rotatex(float rad) {
    D3DXMATRIX rot;
    D3DXMatrixRotationX(&rot, rad);
    rotate(&rot);
}

void Thing::rotatey(float rad) {
    D3DXMATRIX rot;
    D3DXMatrixRotationY(&rot, rad);
    rotate(&rot);
}

void Thing::rotatez(float rad) {
    D3DXMATRIX rot;
    D3DXMatrixRotationZ(&rot, rad);
    rotate(&rot);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a Thing using the rotation matrix *rot
// ----------------------------------------------------------------------------------------------------
void Thing::rotate(D3DXMATRIX* rot) {
	world *= *rot;
}


// ----------------------------------------------------------------------------------------------------
// move translates a Thing by vector [delx, dely, delz]
// ----------------------------------------------------------------------------------------------------
void Thing::move(FLOAT delx, FLOAT dely, FLOAT delz) {
    D3DXMATRIX trans;
    world *= *D3DXMatrixTranslation(&trans, delx, dely, delz);
}


// ----------------------------------------------------------------------------------------------------
// move with camera [delx, dely, delz]
// ----------------------------------------------------------------------------------------------------
void Thing::moveCamera(FLOAT delx, FLOAT dely, FLOAT delz) {
    D3DXMATRIX trans;
    world = *D3DXMatrixTranslation(&trans, delx, dely, delz);
}


// ----------------------------------------------------------------------------------------------------
// copy copies the position and the orientation of the frame Thing
// ----------------------------------------------------------------------------------------------------
void Thing::copyWorld(Thing* frame) {
	world = frame->world;
}

// ----------------------------------------------------------------------------------------------------
// PlaneThing Member Functions
// ----------------------------------------------------------------------------------------------------
PlaneThing::PlaneThing(bool allow, D3DXVECTOR3 normal, D3DXVECTOR3 point, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE clr, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices)
		   : Thing(c, clr, t, texfile, maxvertices) {
	 n = normal;
	 p = - D3DXVec3Dot(&n, &point);
	 allowPenetration = allow;
}


// ----------------------------------------------------------------------------------------------------
// isHitBy returns whether the current object will be hit by a VolumeThing
// and, if so, returns the fraction lambda of the time step dt at which that
// collision will occur
// assumes that the acceleration of the VolumeThing contributes negligibly
// to the calculation of the instant of collision
// ----------------------------------------------------------------------------------------------------
bool PlaneThing::isHitBy(VolumeThing* other, FLOAT& lambda, FLOAT dt, D3DXVECTOR3* cn) const {
	bool collision = false;
	if (!allowPenetration) {
	    FLOAT ncp = D3DXVec3Dot(&n, &other->position(0)) + p;
	    FLOAT nv  = D3DXVec3Dot(&n, &other->velocity());
	    if (ncp < other->boundingRadius()) {
	    	// possible interpenetration
	    	// assume that collision occurs at the
	    	// start of the time increment - setting
	    	// lambda to 0 will generate an infinite
	    	// force: use small positive value instead
	    	lambda = 1.0f / MAX_SUB_INCREMENTS;
	    	collision = true;
		    *cn = n;
	    } else if (nv < 0.0f && other->boundingRadius() >= ncp + nv * dt) {
	    	// centres are approaching one another and
	    	// close enough for a collision to occur
	    	collision = true;
	    	// fraction of dt at which collision occurs
	    	lambda = (other->boundingRadius() - ncp) / (nv * dt);
	    	// move slightly beyond the collision instant
	    	lambda *= 1.01f;
	    	if (lambda > 1.0f)
	    	    // but don't go beyond the
	    	    // time increment itself
	    	    lambda = 1.0f;
	    	else if (lambda < 1.0f / MAX_SUB_INCREMENTS)
	    	    // and don't go beyond the
	    	    // cutoff for interpenetration
	    	    lambda = 1.0f / MAX_SUB_INCREMENTS;
	    	*cn = n;
		}
	}
	return collision;
}


// ----------------------------------------------------------------------------------------------------
// collision determines the impulse that arises during the collision
// of a VolumeThing with the PlaneThing and applies the corresponding
// force to the VolumeThing
// ----------------------------------------------------------------------------------------------------
void PlaneThing::collision(VolumeThing* other, D3DXVECTOR3* n, FLOAT dt) {
	D3DXVECTOR3 force;
	// magnitude of the normal velocity
	FLOAT vn = D3DXVec3Dot(&other->velocity(), n);
	// coefficient of restitution
	FLOAT e = 0.5f * (1.0f + other->coefficientRestitution());
	// magnitude of the impulse
	FLOAT j = - vn * (1.0f + e) / (1.0f / other->getMass());
	// force generated by the impulse
	// assume that dt is the duration
	// of the corresponding force
	force = j * *n / dt;
	other->applyForce(&force);
}


// ----------------------------------------------------------------------------------------------------
// BoundedPlane Member Functions
// ----------------------------------------------------------------------------------------------------
BoundedPlane::BoundedPlane(bool allow, FLOAT breadth, FLOAT depth, D3DXVECTOR3 normal, D3DXVECTOR3 point, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE clr, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices)
		     : PlaneThing(allow, normal, point, c, clr, t, texfile, maxvertices) {
	 centre = point;
	 nx = D3DXVECTOR3(1, 0, 0);
	 nz = D3DXVECTOR3(0, 0, 1);
	 sx = breadth / 2;
	 sz = depth / 2;
}


// ----------------------------------------------------------------------------------------------------
// isHitBy returns whether the current object will be hit by a VolumeThing
// and, if so, returns the fraction lambda of the time step dt at which that
// collision will occur
// assumes that the acceleration of the VolumeThing contributes negligibly
// to the calculation of the instant of collision
// ----------------------------------------------------------------------------------------------------
bool BoundedPlane::isHitBy(VolumeThing* other, FLOAT& lambda, FLOAT dt, D3DXVECTOR3* cn) const {
	bool collision = false;
	if (PlaneThing::isHitBy(other, lambda, dt, cn)) {
		D3DXVECTOR3 c = other->position(lambda * dt);
	    FLOAT nxp = D3DXVec3Dot(&nx, &c) - D3DXVec3Dot(&nx, &centre);
	    FLOAT nzp = D3DXVec3Dot(&nz, &c) - D3DXVec3Dot(&nz, &centre);
		collision = nxp >= -sx && nxp <= sx && nzp >= -sz && nzp <= sz;
	}
	return collision;
}


// ----------------------------------------------------------------------------------------------------
// VolumeThing Member Functions
// ----------------------------------------------------------------------------------------------------
VolumeThing::VolumeThing(FLOAT m, FLOAT r, FLOAT cr, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices)
		    : Thing(c, color, t, texfile, maxvertices) {
    FLOAT rr = 0.4f * r * r;
    init(m, cr, rr, rr, rr, r);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
VolumeThing::VolumeThing(FLOAT m, FLOAT r, FLOAT cr, Model* data, FLOAT scale, bool smooth, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char *texfile)
		    : Thing(c, color, D3DPT_TRIANGLELIST, texfile, data->nVertices()) {
    data->build(this, scale, smooth);
    FLOAT rr = 0.4f * r * r * scale * scale;
    init(m, cr, rr, rr, rr, r * scale);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
VolumeThing::VolumeThing(FLOAT m, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad, FLOAT cr, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices)
		    : Thing(c, color, t, texfile, maxvertices) {
    init(m, cr, xx, yy, zz, rad);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
VolumeThing::VolumeThing(FLOAT m, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad, FLOAT cr, Model* data, FLOAT scale, bool smooth, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char *texfile)
		    : Thing(c, color, D3DPT_TRIANGLELIST, texfile, data->nVertices()) {
    data->build(this, scale, smooth);
    init(m, cr, xx, yy, zz, rad);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void VolumeThing::init(FLOAT m, FLOAT cr, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad) {
    centre    = D3DXVECTOR3(0, 0, 0);  // centre at world origin
    v         = D3DXVECTOR3(0, 0, 0);  // no linear velocity
    a         = D3DXVECTOR3(0, 0, 0);  // no linear acceleration
    launchDir = D3DXVECTOR3(0, 1, 0);  // local launch direction - vertical

    omega      = D3DXVECTOR3(0, 0, 0); // no angular velocity
    alpha      = D3DXVECTOR3(0, 0, 0); // no angular acceleration
    launchAxis = D3DXVECTOR3(0, 0, 1); // local launch axis - in-page

    mass       = (m < NEAR_ZERO) ? NEAR_ZERO : m; // initial mass of the body
    centreMass = D3DXVECTOR3(0, 0, 0); // assume uniform distribution of mass
    impuForces = D3DXVECTOR3(0, 0, 0); // no net impulse forces
    bodyForces = D3DXVECTOR3(0, 0, 0); // no net sustained forces
    moments    = D3DXVECTOR3(0, 0, 0); // no net moments
    D3DXMatrixIdentity(&mInertia);
    mInertia._11 = (mass * ((xx < NEAR_ZERO) ? NEAR_ZERO : xx));
    mInertia._22 = (mass * ((yy < NEAR_ZERO) ? NEAR_ZERO : yy));
    mInertia._33 = (mass * ((zz < NEAR_ZERO) ? NEAR_ZERO : zz));
    D3DXMatrixIdentity(&mInertiaInv);
    mInertiaInv._11 = 1.0f / mInertia._11;
    mInertiaInv._22 = 1.0f / mInertia._22;
    mInertiaInv._33 = 1.0f / mInertia._33;
    D3DXMatrixIdentity(&rotation);     // initially aligned with world axes
    radius = rad;
    restitution = cr;
    supported = false;
}


// ----------------------------------------------------------------------------------------------------
// initialize copies the position, orientation and motion of the frame
// VolumeThing
// ----------------------------------------------------------------------------------------------------
void VolumeThing::initialize(VolumeThing* frame) {
	if (frame) {
	    copyWorld(frame);
	    centre = frame->centre;
		v      = frame->v;
	    a      = frame->a;
	    omega  = frame->omega;
	    alpha  = frame->alpha;
	}
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a VolumeThing given the rotation matrix *rot
// ----------------------------------------------------------------------------------------------------
void VolumeThing::rotate(D3DXMATRIX* rot) {
    D3DXVECTOR3 newdir;
    launchDir  = *D3DXVec3TransformCoord(&newdir, &launchDir, rot);
    launchAxis = *D3DXVec3TransformCoord(&newdir, &launchAxis, rot);
    centreMass = *D3DXVec3TransformCoord(&newdir, &centreMass, rot);
    D3DXMATRIX  newmat;
    rotation   = *D3DXMatrixMultiply(&newmat, rot, &rotation);
    Thing::move(- centre.x, - centre.y, - centre.z);
    Thing::rotate(rot);
    Thing::move(centre.x, centre.y, centre.z);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a VolumeThing about a centre of rotation *c.
// *rot is the rotation matrix describing the rotation
// ----------------------------------------------------------------------------------------------------
void VolumeThing::rotate(D3DXMATRIX* rot, D3DXVECTOR3* c) {
	D3DXVECTOR3 newarm, arm = centre - *c;
	move(- arm.x, - arm.y, - arm.z);
	VolumeThing::rotate(rot);
	D3DXVec3TransformCoord(&newarm, &arm, rot);
	move(newarm.x, newarm.y, newarm.z);
}


// ----------------------------------------------------------------------------------------------------
// move translates a VolumeThing by [delx, dely, delz]
// ----------------------------------------------------------------------------------------------------
void VolumeThing::move(FLOAT delx, FLOAT dely, FLOAT delz) {
    centre += D3DXVECTOR3(delx, dely, delz);
    Thing::move(delx, dely, delz);
}


// ----------------------------------------------------------------------------------------------------
// move translates a VolumeThing by [delx, dely, delz]
// ----------------------------------------------------------------------------------------------------
void VolumeThing::moveCamera(FLOAT delx, FLOAT dely, FLOAT delz) {
    centre = D3DXVECTOR3(delx, dely, delz);
    Thing::moveCamera(delx, dely, delz);
}


// ----------------------------------------------------------------------------------------------------
// applyForce applies a force (and moment) to a VolumeThing. r if supplied
// specifies the arm from the centre of mass at which the force is applied.
// ----------------------------------------------------------------------------------------------------
void VolumeThing::applyForce(D3DXVECTOR3* f, D3DXVECTOR3* r) {
	if (!supported) {
		impuForces += *f;
	    if (r) {
		    D3DXVECTOR3 temp;
	        moments += *D3DXVec3Cross(&temp, r, f);
	    }
	}
}


// ----------------------------------------------------------------------------------------------------
// update updates the position, orientation and velocities of a VolumeThing
// after time increment dt - assumes constant accelerations throughout the
// increment - both linear and angular
// ----------------------------------------------------------------------------------------------------
void VolumeThing::update(FLOAT dt) {
	// to determine the linear acceleration:
	// add the impulse and body forces and divide the sum by the mass
	//
	// a = (impuForces + bodyForces) / mass
	//
	if (mass < NEAR_ZERO)
		a = D3DXVECTOR3(0, 0, 0);
	else
		a = (impuForces + bodyForces) / mass;
    impuForces = D3DXVECTOR3(0, 0, 0);
	// linear displacement
	D3DXVECTOR3 u = v * dt + 0.5f * a * dt * dt;
	v += a * dt;
	move(u.x, u.y, u.z);


	// to determine the angular acceleration:
	// transform moments to local coordinates, premultiply by inverse of
	// moment of inertia matrix, transform back to world coordinates
	//
	// alpha = rotation * mInertiaInv * rotation^T * moments
	//
	D3DXVECTOR3 temp;
	D3DXMATRIX  newmat, dRotByDt;
	D3DXVec3TransformCoord(&alpha,
	 D3DXVec3TransformCoord(&temp,
	 D3DXVec3TransformCoord(&temp, &moments,
	 D3DXMatrixTranspose(&newmat, &rotation)), &mInertiaInv), &rotation);
	moments = D3DXVECTOR3(0, 0, 0);


	/*// rotate to original orientation
	D3DXMATRIX rot, w(0, -omega.z, omega.y, 0,
	                  omega.z, 0, -omega.x, 0,
	                  -omega.y, omega.x, 0, 0,
	                  0,       0,        0, 0);
	D3DXMatrixTranspose(&rot, &rotation);
	D3DXMatrixMultiply(&dRotByDt, &w, &rotation);
	rotation += dRotByDt * dt;
	D3DXMatrixMultiply(&rotNew, &rot, &rotation);
	rotate(&rotNew);*/


	// angular displacement
	D3DXVECTOR3 psi = omega * dt + 0.5f * alpha * dt * dt;
	FLOAT theta = D3DXVec3Length(&psi);
	if (fabsf(theta) > NEAR_ZERO) {
	    D3DXMATRIX  rot;
	    D3DXVECTOR3 axis = psi / theta;
	    D3DXMatrixRotationAxis(&rot, &axis, theta);
	    rotate(&rot);
	    omega += alpha * dt;
	}
}


// ----------------------------------------------------------------------------------------------------
// start 'launches' a VolumeThing by adding a linear velocity with
// a local launch direction plus a body force due to gravity
// as well as an angular velocity about a local launch axis plus an
// angular acceleration about the world z axis
// ----------------------------------------------------------------------------------------------------
void VolumeThing::start(FLOAT speed, FLOAT spin) {
	// linear motion
	v += speed * launchDir;
	bodyForces += mass * D3DXVECTOR3(0, GRAVITY, 0);

    // angular motion
	D3DXVECTOR3 angAccel(0, 0, ANG_ACCEL_Z), temp;
	omega += spin * launchAxis;
	moments += *D3DXVec3TransformCoord(&temp, &angAccel, &mInertia);
}


// ----------------------------------------------------------------------------------------------------
// finished tells whether or not a perishable VolumeThing is finished,
// that is, at a distance beyond FINISHED below the FLOOR
// ----------------------------------------------------------------------------------------------------
bool VolumeThing::finished() const {
	return centre.y < FLOOR - FINISHED;
}


// ----------------------------------------------------------------------------------------------------
// augment adds to the current linear velocity and impulse forces
// the tangential velocity and the force due to accelerating revolution
// about frame - to simulate a detachment of the current object from the frame
// ----------------------------------------------------------------------------------------------------
void VolumeThing::augment(VolumeThing* frame, D3DXVECTOR3* offset) {
	if (frame) {
		D3DXVECTOR3 newdir;
		v += *D3DXVec3Cross(&newdir, &frame->omega, offset);
	    impuForces += mass * *D3DXVec3Cross(&newdir, &frame->alpha, offset);
	}
}


// ----------------------------------------------------------------------------------------------------
// cut removes a mass m from VolumeThing and returns the address of
// VolumeThing if VolumeThing has sufficient mass, returns NULL otherwise
// ----------------------------------------------------------------------------------------------------
VolumeThing* VolumeThing::cut(FLOAT m, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char* texfile) {
    VolumeThing* remainingThing;
    if (mass - m >= NEAR_ZERO) {
		FLOAT factor = (mass - m) / mass;
		mass -= m;
		mInertia *= factor;
		mInertiaInv /= factor;
		remainingThing = this;
    } else
        remainingThing = NULL;
    return remainingThing;
}


// ----------------------------------------------------------------------------------------------------
// contains determines if the VolumeThing is close to *x
// ----------------------------------------------------------------------------------------------------
bool VolumeThing::contains(D3DXVECTOR3* x) const {
	D3DXVECTOR3 s = *x - centre;
	return D3DXVec3Dot(&s, &s) <= radius * radius;
}


// ----------------------------------------------------------------------------------------------------
// contactPoint returns the point at which the bounding sphere
// intersects with the vector from *other to the current object
// ----------------------------------------------------------------------------------------------------
D3DXVECTOR3 VolumeThing::contactPoint(VolumeThing* other) const {
    D3DXVECTOR3 newx, x = centre - other->centre;
	return - radius * *D3DXVec3Normalize(&newx, &x) + centre;
}


// ----------------------------------------------------------------------------------------------------
// penetration returns the vector by which *x should be moved
// to place *x on the bounding sphere
// ----------------------------------------------------------------------------------------------------
D3DXVECTOR3 VolumeThing::penetration(D3DXVECTOR3* x) const {
	D3DXVECTOR3 p(0, 0, 0), cx = *x - centre;
	FLOAT rr = D3DXVec3Dot(&cx, &cx);
	if (rr < radius * radius) {
		rr = sqrtf(rr);
		p = cx * (radius / rr - 1.0f);
	}
	return p;
}


// ----------------------------------------------------------------------------------------------------
// contactWith determines the existence of a contact of any duration between
// the current object and another VolumeThing within the time increment dt
// and, if not sustained, the fraction lambda of the time step dt at which
// the collision will occur
// assumes that the relative acceleration contributes negligibly to the
// calculation of the instant of collision
// ----------------------------------------------------------------------------------------------------
bool VolumeThing::contactWith(VolumeThing* other, FLOAT dt, bool& collision, bool& penetration, FLOAT& lambda, D3DXVECTOR3* n) const {
	bool contact;
	D3DXVECTOR3 c, vr;
	FLOAT cr, cc, r, cvr, vr2, rr;

	contact   = false;
	collision = false;
	penetration = false;
	lambda = 1.0f;
	c   = centre - other->centre;
	r   = radius + other->radius;
	vr  = v - other->v;
	rr  = r * r;
	cc  = D3DXVec3Dot(&c, &c);
	cr  = cc - rr;
	cvr = D3DXVec3Dot(&c, &vr);
	vr2 = D3DXVec3Dot(&vr, &vr);
	if (cr <= 0.0f) {
		// check for contact or interpenetration
		// by comparing relative velocity with the
		// vector connecting other->centre to centre
		if (cvr * cvr <= NEAR_ZERO * cc * vr2) {
			// relative velocity is zero or perpendicular to
		    // the vector connecting other->centre to centre
			lambda = 1.0f;
		    contact = true;
			// the contact surface normal is parallel to
			// vector connecting other->centre to centre
		    D3DXVec3Normalize(n, &c);
		} else if (cvr < 0.0f) {
			// relative velocity is in a direction obtuse
			// to the vector connecting other->centre to
			// centre - objects are approaching one another
		    // assume that their collision occured at the
		    // start of the time increment - setting
		    // lambda to 0 will generate an infinite
		    // force: so use small positive value instead
		    lambda = 1.0f / MAX_SUB_INCREMENTS;
			collision = true;
			contact = true;
			// the contact surface normal is parallel to
			// vector connecting other->centre to centre
		    D3DXVec3Normalize(n, &c);
		}
		penetration = true;
	} else if (cvr < 0.0f && vr2 * dt * dt + 2.0f * cvr * dt + cr <= 0.0f) {
		// centres are approaching one another and
		// close enough for a collision to occur
		contact = true;
		collision = true;
		// fraction of dt at which collision occurs
		lambda = (- cvr - sqrtf(cvr * cvr - vr2 * cr)) / (vr2 * dt);
		// move slightly beyond the collision instant
		lambda = 1.01f * lambda;
		if (lambda > 1.0f)
		    // but don't go beyond the
		    // time increment itself
		    lambda = 1.0f;
		else if (lambda < 1.0f / MAX_SUB_INCREMENTS)
		    // and don't go beyond the
		    // cutoff for interpenetration
		    lambda = 1.0f / MAX_SUB_INCREMENTS;
		D3DXVec3Normalize(n, &c);
	}
	return contact;
}


// ----------------------------------------------------------------------------------------------------
// contact applies the force at the contact with *other
// ----------------------------------------------------------------------------------------------------
void VolumeThing::contact(VolumeThing* other, D3DXVECTOR3* n) {
	if (!supported || !other->supported) {
		D3DXVECTOR3 fn, ft, f, cf, cfn, cft;

		// compensate for penetration, if any
		// (this should be done using forces)
		D3DXVECTOR3 a, off;
		a = contactPoint(other);
		off = other->penetration(&a);
		move(-off.x, -off.y, -off.z);
		a = centre - contactPoint(other);

		// decompose net force acting on the current object
		// fn - force component normal to the contact surface
		// ft - force component tangential to the contact surface
		f  = impuForces + bodyForces;
		fn = D3DXVec3Dot(&f, n) * *n;
		ft = f - fn;
		// assume that all forces are transferred across the contact surface
		// cfn - normal force acting on the current object at the contact
		// cft - tangential force acting on the current object at the contact
		cfn = -fn;
		cft = -ft;
		// skip checks if there is no normal contact force
		FLOAT cfncfn = D3DXVec3Dot(&cfn, &cfn);
		if (cfncfn > NEAR_ZERO) {
			// can contact surface sustain the reaction forces?
			//
			// distinguish sliding and stationary cases
			// sliding exists if there is some relative velocity
			D3DXVECTOR3 vr = v - other->v;
			if (D3DXVec3Dot(&vr, &vr) < RELATIVE_V_TOL) {
				// the stationary contact case
				FLOAT ratio2 = D3DXVec3Dot(&cft, &cft) / cfncfn;
				FLOAT mu = staticFriction(other);
				// check that |cft| does not exceeds mu|cfn|
				if (ratio2 > mu * mu)
					// exceeds, so reduce |cft| to mu * |cfn|
					cft *= mu / sqrtf(ratio2);
			} else {
				// the sliding contact case
				// cft is directed opposite to the relative velocity
				// |cft| = mu * |fn|
				FLOAT mu = kineticFriction(other);
				vr = vr - D3DXVec3Dot(&vr, n) * *n;
				D3DXVECTOR3 t;
				D3DXVec3Normalize(&t, &vr);
				cft = - mu * sqrtf(cfncfn) * t;
			}
		}
		// apply the reaction forces to the contact objects
		cf = cfn + cft;
		applyForce(&cf);
		cf = -cf;
		other->applyForce(&cf);
	}
}


// ----------------------------------------------------------------------------------------------------
// collision applies the impulse that arises during the collision
// between the current object and *other - *n is the normal to the
// collision surface and dt is the time increment over which the
// impulse is applied
// ----------------------------------------------------------------------------------------------------
void VolumeThing::collision(VolumeThing* other, D3DXVECTOR3* n, FLOAT dt) {
	FLOAT vrn, j, e;
	D3DXVECTOR3 force, vr = v - other->v;
	// normal relative velocity
	vrn = D3DXVec3Dot(&vr, n);
	// coefficient of restitution
	e = 0.5f * (restitution + other->restitution);
	// magnitude of the impulse
	j = - vrn * (1.0f + e) / (1.0f / mass + 1.0f / other->mass);
	// force generated by the impulse
	// assume that dt is the duration
	// of the corresponding force
	force = j * *n / dt;
	applyForce(&force);
	force = -force;
	other->applyForce(&force);
}


// ----------------------------------------------------------------------------------------------------
// position returns the position of the current object after time increment dt
// ----------------------------------------------------------------------------------------------------
D3DXVECTOR3 VolumeThing::position(FLOAT dt) const {
	// to determine the linear acceleration:
	// add the impulse and body forces and divide the sum by the mass
	//
	// a = (impuForces + bodyForces) / mass
	//
	D3DXVECTOR3 a;
	if (mass < NEAR_ZERO)
		a = D3DXVECTOR3(0, 0, 0);
	else
		a = (impuForces + bodyForces) / mass;
	return centre + v * dt + 0.5f * a * dt * dt;
}


// ----------------------------------------------------------------------------------------------------
// BoundedBody Member Functions
// ----------------------------------------------------------------------------------------------------
BoundedBody::BoundedBody(FLOAT m, FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx, FLOAT maxy, FLOAT maxz, FLOAT cr, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices)
		    : VolumeThing(m, I_BRICK(maxy-miny, maxz-minz), I_BRICK(maxx-minx, maxz-minz), I_BRICK(maxx-minx, maxy-miny), R_BRICK(maxx-minx, maxy-miny, maxz-minz), cr, c, color, t, texfile, maxvertices) {
    sx = (maxx - minx) / 2;
    sy = (maxy - miny) / 2;
    sz = (maxz - minz) / 2;
    nx = D3DXVECTOR3(1, 0, 0);
    ny = D3DXVECTOR3(0, 1, 0);
    nz = D3DXVECTOR3(0, 0, 1);
    centre = D3DXVECTOR3(0, 0, 0);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a BoundedBody given the rotation matrix *rot
// ----------------------------------------------------------------------------------------------------
void BoundedBody::rotate(D3DXMATRIX* rot) {
    D3DXVECTOR3 newdir;
    nx = *D3DXVec3TransformCoord(&newdir, &nx, rot);
    ny = *D3DXVec3TransformCoord(&newdir, &ny, rot);
    nz = *D3DXVec3TransformCoord(&newdir, &nz, rot);
    VolumeThing::rotate(rot);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a BoundedBody about a centre of rotation *c.
// *rot is the rotation matrix describing that rotation
// ----------------------------------------------------------------------------------------------------
void BoundedBody::rotate(D3DXMATRIX* rot, D3DXVECTOR3* c) {
    D3DXVECTOR3 newdir;
    nx = *D3DXVec3TransformCoord(&newdir, &nx, rot);
    ny = *D3DXVec3TransformCoord(&newdir, &ny, rot);
    nz = *D3DXVec3TransformCoord(&newdir, &nz, rot);
    VolumeThing::rotate(rot, c);
}


// ----------------------------------------------------------------------------------------------------
// move translates a BoundedBody by [delx, dely, delz]
// ----------------------------------------------------------------------------------------------------
void BoundedBody::move(FLOAT delx, FLOAT dely, FLOAT delz) {
    centre += D3DXVECTOR3(delx, dely, delz);
    VolumeThing::move(delx, dely, delz);
}


// ----------------------------------------------------------------------------------------------------
// contactWith returns whether the current object will collide with another
// VolumeThing and, if so, the fraction lambda of the time step dt at which
// that collision will occur
// ----------------------------------------------------------------------------------------------------
bool BoundedBody::contactWith(VolumeThing* other, FLOAT dt, bool& collision, bool& penetration, FLOAT& lambda, D3DXVECTOR3* n) const {
	bool contact = false;

	// bounding sphere contact ?
	if (VolumeThing::contactWith(other, dt, collision, penetration, lambda, n)
	 || penetration) {
        // check for any intersection of the path
        // of *other with the current object
		collision = false;
		lambda = 1.0001f;
		D3DXVECTOR3 cb = other->position(0);
		D3DXVECTOR3 ce = other->position(dt);
		D3DXVECTOR3 r  = other->clearance();
		contact =
		 intersects(&cb, &ce, &nx, sx + r.x, &ny, sy, &nz, sz, lambda, n,
		 collision) ||
		 intersects(&cb, &ce, &ny, sy + r.y, &nx, sx, &nz, sz, lambda, n,
		 collision) ||
		 intersects(&cb, &ce, &nz, sz + r.z, &nx, sx, &ny, sy, lambda, n,
		 collision);
	}
	return contact;
}


// ----------------------------------------------------------------------------------------------------
// intersects determines whether the path cb->ce touches/crosses the plane
// with normal *na at a distance sa from centre and, if so, whether, the
// intersection lies within the planes described by *nb and *nc at distances
// +-sb and +-sc from the centre of the contacted surface.
// returns true for any contact with collision set to true if the contact
// is a collision, false if the contact is sustained; sets lambda to the
// fraction of the path at which the collision occurs and in the case of
// sustained contact the fraction of the path over which the contact is
// sustained; returns the normal *n to the contact surface
// ----------------------------------------------------------------------------------------------------
bool BoundedBody::intersects(D3DXVECTOR3* cb, D3DXVECTOR3* ce, const D3DXVECTOR3* na, FLOAT sa, const D3DXVECTOR3* nb, FLOAT sb, const D3DXVECTOR3* nc, FLOAT sc, FLOAT& lambda, D3DXVECTOR3* n, bool& collision) const {
	bool intersects;
	D3DXVECTOR3 x, xc;
	FLOAT nca, ncbms, ncems, ncbps, nceps, kappa, qb, qc;

	intersects = false;
	collision  = false;
	nca   = D3DXVec3Dot(na, &centre);
	ncbms = D3DXVec3Dot(na, cb) - nca - sa;
	ncems = D3DXVec3Dot(na, ce) - nca - sa;
	ncbps = ncbms + sa + sa;
	nceps = ncems + sa + sa;
	if (ncbms >= - LAYER * sa && ncbms <= LAYER * sa
	 && ncems >= - LAYER * sa && ncems <= LAYER * sa) {
	    // the path cb->ce glides along the surface at + sa
	    // that is, remains within +-LAYER * sa of the surface
		lambda = 1.0f;
		*n = *na;
		collision = false;
		intersects = true;
	} else if (ncbps >= - LAYER * sa && ncbps <= LAYER * sa
	 && nceps >= - LAYER * sa && nceps <= LAYER * sa) {
	    // the path cb->ce glides along the surface at - sa
	    // that is, remains within +-LAYER * sa of the surface
		lambda = 1.0f;
		*n = - *na;
		collision = false;
		intersects = true;
	} else if (ncbms > LAYER * sa && ncems < - LAYER * sa) {
	    // the path cb->ce cross the surface at + sa
		// so, find the point of crossing x
		kappa = - ncbms / (ncems - ncbms);
		x  = *cb + kappa * (*ce - *cb);
		// cross point relative to centre of the bounding surface
		xc = x - centre - *na * sa;
		qb = D3DXVec3Dot(nb, &xc);
		qc = D3DXVec3Dot(nc, &xc);
		if (qb <= sb && qb >= -sb && qc <= sc && qc >= -sc && kappa < lambda) {
			lambda = kappa;
			*n = *na;
			collision = true;
			intersects = true;
		}
	} else if (ncbps < - LAYER * sa && nceps > LAYER * sa) {
	    // the path cb->ce cross the surface at - sa
		// so, find the point of crossing x
		kappa = - ncbps / (nceps - ncbps);
		x  = *cb + kappa * (*ce - *cb);
		// cross point relative to centre of the bounding surface
		xc = x - centre + *na * sa;
		qb = D3DXVec3Dot(nb, &xc);
		qc = D3DXVec3Dot(nc, &xc);
		if (qb <= sb && qb >= -sb && qc <= sc && qc >= -sc && kappa < lambda) {
			lambda = kappa;
			*n = - *na;
			collision = true;
			intersects = true;
		}
	}
	return intersects;
}


// ----------------------------------------------------------------------------------------------------
// inContactWith determines whether the BoundedBody is in contact with
// another VolumeThing; that is, whether the midpoint on a
// contact face is contained by *other
// ----------------------------------------------------------------------------------------------------
bool BoundedBody::inContactWith(VolumeThing* other) const {
	bool contact = false;

	// proximity and velocity check
	//if (VolumeThing::inContactWith(other)) {
		// contact surface checks
		D3DXVECTOR3 x;
		x = centre + sx * nx;
		if (other->contains(&x))
		    contact = true;
		else {
			x = centre - sx * nx;
			if (other->contains(&x))
			    contact = true;
			else {
				x = centre + sy * ny;
				if (other->contains(&x))
				    contact = true;
				else {
					x = centre - sy * ny;
					if (other->contains(&x))
					    contact = true;
					else {
						x = centre + sz * nz;
						if (other->contains(&x))
						    contact = true;
						else {
							x = centre - sz * nz;
							if (other->contains(&x))
							    contact = true;
						}
					}
				}
			}
		}
	//}
    return contact;
}


// ----------------------------------------------------------------------------------------------------
// contains determines if point x lies within the bounding surfaces of the BoundedBody
// ----------------------------------------------------------------------------------------------------
bool BoundedBody::contains(D3DXVECTOR3* x) const {
	FLOAT lambdaX = D3DXVec3Dot(&nx, x) - D3DXVec3Dot(&nx, &centre);
	FLOAT lambdaY = D3DXVec3Dot(&ny, x) - D3DXVec3Dot(&ny, &centre);
	FLOAT lambdaZ = D3DXVec3Dot(&nz, x) - D3DXVec3Dot(&nz, &centre);
	return lambdaX <= sx && lambdaX >= -sx && lambdaY <= sy && lambdaY >= -sy
	 && lambdaZ <= sz && lambdaZ >= -sz;
}


// ----------------------------------------------------------------------------------------------------
// contactPoint returns the point of contact with *other
// ----------------------------------------------------------------------------------------------------
D3DXVECTOR3 BoundedBody::contactPoint(VolumeThing* other) const {
    D3DXVECTOR3 x;
	bool fake = true;
	x = centre + sx * nx;
	if (other->contains(&x))
		;
	else {
		x = centre - sx * nx;
		if (other->contains(&x))
			;
		else {
			x = centre + sy * ny;
			if (other->contains(&x))
				;
			else {
				x = centre - sy * ny;
				if (other->contains(&x))
					;
				else {
					x = centre + sz * nz;
					if (other->contains(&x))
						;
					else {
						x = centre - sz * nz;
						if (other->contains(&x))
							fake = false;
					}
				}
			}
		}
	}
    return x;
}


// ----------------------------------------------------------------------------------------------------
// penetration returns the vector by which *x has penetrated
// the BoundedBody
// ----------------------------------------------------------------------------------------------------
D3DXVECTOR3 BoundedBody::penetration(D3DXVECTOR3* x) const {
	D3DXVECTOR3 p(0, 0, 0);
	FLOAT lambdaX = D3DXVec3Dot(&nx, x) - D3DXVec3Dot(&nx, &centre);
	FLOAT lambdaY = D3DXVec3Dot(&ny, x) - D3DXVec3Dot(&ny, &centre);
	FLOAT lambdaZ = D3DXVec3Dot(&nz, x) - D3DXVec3Dot(&nz, &centre);
	if (fabsf(lambdaX - sx) < 0.1 * sx)
	    p = nx * (lambdaX - sx) / sx;
	else if (fabsf(lambdaX + sx) < 0.1 * sx)
	    p = -nx * (lambdaX + sx) / sx;
	else if (fabsf(lambdaY - sy) < 0.1 * sy)
	    p = ny * (lambdaY - sy) / sy;
	else if (fabsf(lambdaY + sy) < 0.1 * sy)
	    p = -ny * (lambdaY + sy) / sy;
	else if (fabsf(lambdaZ + sz) < 0.1 * sz)
	    p = nz * (lambdaZ - sz) / sz;
	else if (fabsf(lambdaZ - sz) < 0.1 * sz)
	    p = -nz * (lambdaZ + sz) / sz;
	return p;
}


// ----------------------------------------------------------------------------------------------------
// AttachableVolume Member Functions -------------------------------------------
// ----------------------------------------------------------------------------------------------------
AttachableVolume::AttachableVolume(FLOAT m, FLOAT rr, FLOAT cr, VolumeThing* parent, D3DXVECTOR3 offset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile, int maxvertices)
				 : VolumeThing(m, rr, cr, c, color, D3DPT_TRIANGLELIST, texfile, maxvertices) {
    init(parent, offset);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
AttachableVolume::AttachableVolume(FLOAT m, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad, FLOAT cr, VolumeThing* parent, D3DXVECTOR3 offset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile, int maxvertices)
				 : VolumeThing(m, xx, yy, zz, rad, cr, c, color, D3DPT_TRIANGLELIST, texfile, maxvertices) {
    init(parent, offset);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void AttachableVolume::init(VolumeThing* parent, D3DXVECTOR3 pOffset) {
    offset = pOffset;
	frame = parent;	                    // frame around which particle revolves
	attached = true;                    // attach particle to this frame
	initialize(frame);                  // copy world matrix & motion of frame
    move(offset.x, offset.y, offset.z); // move particle to its offset
    D3DXVECTOR3 launch;
    D3DXVec3Normalize(&launch, &offset);
    setDirection(&launch);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates an AttachableVolume given the rotation matrix *rot
// ----------------------------------------------------------------------------------------------------
void AttachableVolume::rotate(D3DXMATRIX* rot) {
    D3DXVECTOR3 newdir;
    offset = *D3DXVec3TransformCoord(&newdir, &offset, rot);
    VolumeThing::rotate(rot);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates an AttachableVolume about a centre of rotation *c.
// *rot is the rotation matrix describing that rotation
// ----------------------------------------------------------------------------------------------------
void AttachableVolume::rotate(D3DXMATRIX* rot, D3DXVECTOR3* c) {
    D3DXVECTOR3 newdir;
    offset = *D3DXVec3TransformCoord(&newdir, &offset, rot);
    VolumeThing::rotate(rot, c);
}


// ----------------------------------------------------------------------------------------------------
// update updates the position and orientation of an AttachableVolume
// after time increment dt - if attached to a frame, update first
// moves the AttachableVolume to the origin of the frame, updates
// the object and then moves it back to its offset from the origin
// where the offset will have been rotated into its new position by rotate
// ----------------------------------------------------------------------------------------------------
void AttachableVolume::update(FLOAT dt) {
	if (attached) {
		move(- offset.x, - offset.y, - offset.z);
		VolumeThing::update(dt);
		move(offset.x, offset.y, offset.z);
    } else {
	    VolumeThing::update(dt);
	}
}


// ----------------------------------------------------------------------------------------------------
// start detaches an AttachableVolume from frame and imparts to the
// object an initial speed and spin
// ----------------------------------------------------------------------------------------------------
void AttachableVolume::start(FLOAT speed, FLOAT spin) {
	if (attached) {
    	VolumeThing::augment(frame, &offset);
    	frame = NULL;
    	attached = false;
    	offset = D3DXVECTOR3(0, 0, 0);
	}
    VolumeThing::start(speed, spin);
}


// ----------------------------------------------------------------------------------------------------
// SheddingVolume Member Function
// ----------------------------------------------------------------------------------------------------
SheddingVolume::SheddingVolume(FLOAT m, FLOAT wid, FLOAT len, FLOAT cr, FLOAT clearance, FLOAT aspect, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile, int maxvertices)
			   : VolumeThing(m, I_BRICK(wid, len), I_BRICK(wid, wid), I_BRICK(wid, len), R_BRICK(wid, wid, len), cr, c, color, D3DPT_TRIANGLELIST, texfile, maxvertices) {
    bWidth = wid - 2.0f * clearance;
    bDepth = bWidth;
    bLength = aspect * bWidth;
    offset = D3DXVECTOR3(0, 0.5f * (len - bLength), 0);
    wid *= 0.5f;
    len *= 0.5f;
    addBrick(- wid, - len, - wid, wid, len, wid);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
SheddingVolume::SheddingVolume(FLOAT m, FLOAT cube, FLOAT cr, D3DXVECTOR3 pOffset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile, int maxvertices)
			   : VolumeThing(m, I_BRICK(cube, cube), I_BRICK(cube, cube), I_BRICK(cube, cube), R_BRICK(cube, cube, cube), cr, c, color, D3DPT_TRIANGLELIST, texfile, maxvertices) {
	 bWidth = 2 * SMALL;
	 bDepth = SMALL;
	 bLength = SMALL;
	 offset = pOffset;
	 addBrick(-cube/2, - cube/2, -cube/2, cube/2, cube/2, cube/2);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a SheddingVolume given the rotation matrix *rot
// ----------------------------------------------------------------------------------------------------
void SheddingVolume::rotate(D3DXMATRIX* rot) {
    D3DXVECTOR3 newdir;
    offset = *D3DXVec3TransformCoord(&newdir, &offset, rot);
    VolumeThing::rotate(rot);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a SheddingVolume about a centre of rotation *c.
// *rot is the rotation matrix describing that rotation
// ----------------------------------------------------------------------------------------------------
void SheddingVolume::rotate(D3DXMATRIX* rot, D3DXVECTOR3* c) {
    D3DXVECTOR3 newdir;
    offset = *D3DXVec3TransformCoord(&newdir, &offset, rot);
    VolumeThing::rotate(rot, c);
}


// ----------------------------------------------------------------------------------------------------
// cut extracts from a SheddingVolume a Revolving Brick that revolves around
// the centre of the SheddingVolume at a distance 'offset'
// ----------------------------------------------------------------------------------------------------
VolumeThing* SheddingVolume::cut(FLOAT m, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char* texfile) {
    VolumeThing* brick;
    if (VolumeThing::cut(m, c, color, texfile))
        brick = new RevolvingBrick(m, bWidth, bLength, bDepth, PURELY_ELASTIC,
        // Factor of 2 added to start brick outside the
        // bounding sphere of the Shedding Volume
         (VolumeThing*)this, 2.0f*offset, color, c, texfile);
    else
        brick = NULL;
    return brick;
}


// ----------------------------------------------------------------------------------------------------
// Box Member Functions
// ----------------------------------------------------------------------------------------------------
Box::Box(FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx, FLOAT maxy, FLOAT maxz, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile)
	: Thing(c, color, D3DPT_TRIANGLELIST, texfile, 36) {
    addBrick(minx, miny, minz, maxx, maxy, maxz);
    centre = D3DXVECTOR3((minx+maxx)/2, (miny+maxy)/2, (minz+maxz)/2);
}


// ----------------------------------------------------------------------------------------------------
// rotate rotates a Box given the rotation matrix *rot
// ----------------------------------------------------------------------------------------------------
void Box::rotate(D3DXMATRIX* rot) {
    Thing::move(- centre.x, - centre.y, - centre.z);
    Thing::rotate(rot);
    Thing::move(centre.x, centre.y, centre.z);
}


// ----------------------------------------------------------------------------------------------------
// move translates a Box by [delx, dely, delz]
// ----------------------------------------------------------------------------------------------------
void Box::move(FLOAT delx, FLOAT dely, FLOAT delz) {
    centre += D3DXVECTOR3(delx, dely, delz);
    Thing::move(delx, dely, delz);
}


// ----------------------------------------------------------------------------------------------------
// Brick Member Functions
// ----------------------------------------------------------------------------------------------------
Brick::Brick(FLOAT m, FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx, FLOAT maxy, FLOAT maxz, FLOAT cr, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile)
	  : BoundedBody(m, minx, miny, minz, maxx, maxy, maxz, cr, c, color, D3DPT_TRIANGLELIST, texfile, 36) {
    // create as if centred on the world origin
    centre = D3DXVECTOR3(0, 0, 0);
    addBrick(-(maxx - minx)/2, -(maxy - miny)/2, -(maxz - minz)/2,
     (maxx - minx)/2, (maxy - miny)/2, (maxz - minz)/2);
    // move to originally specified centre
    move((minx+maxx)/2, (miny+maxy)/2, (minz+maxz)/2);
}


// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
Brick::Brick(FLOAT m, FLOAT cube, FLOAT cr, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile)
	  : BoundedBody(m, -cube/2, -cube/2, -cube/2, cube/2, cube/2, cube/2, cr, c, color, D3DPT_TRIANGLELIST, texfile, 36) {

    // create centred on the world origin
    centre = D3DXVECTOR3(0, 0, 0);
    addBrick(-cube/2, -cube/2, -cube/2, cube/2, cube/2, cube/2);
}


// ----------------------------------------------------------------------------------------------------
// move translates a Brick by [delx, dely, delz]
// ----------------------------------------------------------------------------------------------------
void Brick::move(FLOAT delx, FLOAT dely, FLOAT delz) {
    centre += D3DXVECTOR3(delx, dely, delz);
    BoundedBody::move(delx, dely, delz);
}


// ----------------------------------------------------------------------------------------------------
// RevolvingBrick Member Function
// ----------------------------------------------------------------------------------------------------
RevolvingBrick::RevolvingBrick(FLOAT m, FLOAT width, FLOAT depth, FLOAT length, FLOAT cr, VolumeThing* p, D3DXVECTOR3 offset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile)
			   : AttachableVolume(m, I_BRICK(length, depth), I_BRICK(depth, width), I_BRICK(width, length), R_BRICK(width, depth, length), cr, p, offset, color, c, texfile, 36) {
    width  *= 0.5f;
    length *= 0.5f;
    depth  *= 0.5f;
    addBrick(- width, - length, - depth, width, length, depth);
}





// ----------------------------------------------------------------------------------------------------
// FuzzyPlaneThing Member Functions
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// A fuzzy plane thing. Just before drawing it, bilinear filtering, both
// when the texture has to be magnified and when it has to be shrunk,
// is turned on. After drawing, filtering is turned off.
// ----------------------------------------------------------------------------------------------------
FuzzyPlaneThing::FuzzyPlaneThing(D3DXVECTOR3 n, D3DXVECTOR3 p, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices)
				: PlaneThing(true, n, p, c, color, t, texfile, maxvertices) {}


void FuzzyPlaneThing::draw(LPDIRECT3DDEVICE9 c) {
    c->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    c->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    Thing::draw(c);
    c->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    c->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
}




// ----------------------------------------------------------------------------------------------------
// BoundedFuzzyPlaneThing Member Functions
// ----------------------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------------------
// A BoundedFuzzyPlaneThing is a FuzzyPlaneThing with 4 bounding planes.
// ----------------------------------------------------------------------------------------------------
BoundedFuzzyPlaneThing::BoundedFuzzyPlaneThing(FLOAT breadth, FLOAT depth, D3DXVECTOR3 n, D3DXVECTOR3 p, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices)
					   : BoundedPlane(false, breadth, depth, n, p, c, color, t, texfile, maxvertices) {}


void BoundedFuzzyPlaneThing::draw(LPDIRECT3DDEVICE9 c) {
    c->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    c->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    Thing::draw(c);
    c->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    c->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
}


// ----------------------------------------------------------------------------------------------------
// the grass around the outside of the road
// draw just below FLOOR so we'll always see it
// ----------------------------------------------------------------------------------------------------
grass::grass(LPDIRECT3DDEVICE9 c, const char *texfile)
	  : FuzzyPlaneThing(
 D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(0, FLOOR - 3, 0), c,
 D3DXCOLOR(1, 1, 1, 1), D3DPT_TRIANGLESTRIP, texfile, 18) {
    add( 170, FLOOR - 3, -100, 0, 1, 0, 0, 0, 0);
    add( 500, FLOOR - 3, -500, 0, 1, 0, 0, 1, 0);
    add( 150, FLOOR - 3, -120, 0, 1, 0, 1, 0, 1);
    add(   0, FLOOR - 3, -500, 0, 1, 0, 1, 1, 1);
    add(-100, FLOOR - 3, -120, 0, 1, 0, 0, 1, 1);
    add(-500, FLOOR - 3, -500, 0, 1, 0, 1, 0, 1);
    add(-120, FLOOR - 3, -100, 0, 1, 0, 0, 0, 1);
    add(-500, FLOOR - 3,    0, 0, 1, 0, 0, 1, 1);
    add(-120, FLOOR - 3,  100, 0, 1, 0, 1, 0, 1);
    add(-500, FLOOR - 3,  500, 0, 1, 0, 1, 1, 1);
    add( -80, FLOOR - 3,  140, 0, 1, 0, 0, 1, 1);
    add(   0, FLOOR - 3,  500, 0, 1, 0, 1, 0, 1);
    add(  80, FLOOR - 3,  140, 0, 1, 0, 0, 0, 1);
    add( 500, FLOOR - 3,  500, 0, 1, 0, 0, 1, 1);
    add( 170, FLOOR - 3,    0, 0, 1, 0, 1, 0, 1);
    add( 500, FLOOR - 3,    0, 0, 1, 0, 1, 1, 1);
    add( 170, FLOOR - 3, -100, 0, 1, 0, 0, 1, 1);
    add( 500, FLOOR - 3, -500, 0, 1, 0, 1, 0, 1);
}


// ----------------------------------------------------------------------------------------------------
// the road around the edge of the lake
// draw just below FLOOR so we'll always see it
// ----------------------------------------------------------------------------------------------------
road::road(LPDIRECT3DDEVICE9 c, const char *texfile)
     : FuzzyPlaneThing(
 D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(0, FLOOR - 3, 0), c,
 D3DXCOLOR(1, 1, 1, 1), D3DPT_TRIANGLESTRIP, texfile, 22) {
    add(  80, FLOOR - 3,  120, 0, 1, 0, 0, 0, 0);
    add( 125, FLOOR - 3,   70, 0, 1, 0, 0, 1, 0);
    add( 150, FLOOR - 3,    0, 0, 1, 0, 1, 0, 1);
    add( 170, FLOOR - 3,    0, 0, 1, 0, 1, 1, 1);
    add( 150, FLOOR - 3,  -50, 0, 1, 0, 0, 1, 1);
    add( 170, FLOOR - 3, -100, 0, 1, 0, 1, 0, 1);
    add( 150, FLOOR - 3, -100, 0, 1, 0, 0, 0, 1);
    add( 150, FLOOR - 3, -120, 0, 1, 0, 0, 1, 1);
    add(  25, FLOOR - 3, -100, 0, 1, 0, 1, 0, 1);
    add(-100, FLOOR - 3, -120, 0, 1, 0, 1, 1, 1);
    add(-100, FLOOR - 3, -100, 0, 1, 0, 0, 1, 1);
    add(-120, FLOOR - 3, -100, 0, 1, 0, 1, 0, 1);
    add(-100, FLOOR - 3,    0, 0, 1, 0, 0, 0, 1);
    add(-120, FLOOR - 3,  100, 0, 1, 0, 0, 1, 1);
    add(-100, FLOOR - 3,  100, 0, 1, 0, 1, 0, 1);
    add(-100, FLOOR - 3,  120, 0, 1, 0, 1, 1, 1);
    add( -80, FLOOR - 3,  120, 0, 1, 0, 0, 1, 1);
    add( -80, FLOOR - 3,  140, 0, 1, 0, 1, 0, 1);
    add(   0, FLOOR - 3,  120, 0, 1, 0, 0, 0, 1);
    add(  80, FLOOR - 3,  140, 0, 1, 0, 0, 1, 1);
    add(  80, FLOOR - 3,  120, 0, 1, 0, 1, 0, 1);
    add( 125, FLOOR - 3,   70, 0, 1, 0, 1, 1, 1);
}


// ----------------------------------------------------------------------------------------------------
// the lake inside the road
// draw just below FLOOR so we'll always see it
// ----------------------------------------------------------------------------------------------------
lake::lake(LPDIRECT3DDEVICE9 c, const char *texfile)
     : BoundedFuzzyPlaneThing(
 250, 220, D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(25, FLOOR - 3, 10), c,
 D3DXCOLOR(1, 1, 1, 1), D3DPT_TRIANGLEFAN, texfile, 8) {
    add(   0, FLOOR - 3,    0, 0, 1, 0, 0, 0, 0);
    add( 150, FLOOR - 3,    0, 0, 1, 0, 0, 1, 0);
    add( 150, FLOOR - 3, -100, 0, 1, 0, 1, 0, 1);
    add(-100, FLOOR - 3, -100, 0, 1, 0, 0, 1, 1);
    add(-100, FLOOR - 3,  100, 0, 1, 0, 1, 0, 1);
    add( -80, FLOOR - 3,  120, 0, 1, 0, 0, 1, 1);
    add(  80, FLOOR - 3,  120, 0, 1, 0, 1, 0, 1);
    add( 150, FLOOR - 3,    0, 0, 1, 0, 0, 1, 1);
}


// ----------------------------------------------------------------------------------------------------
// contactBetween determines the existence of a contact between *mi and *mj
// of any duration within the time increment dt and, if the contact will be
// just a collision, at what fraction of that time increment the collision will occur
// ----------------------------------------------------------------------------------------------------
bool contactBetween(VolumeThing* mi, VolumeThing* mj, FLOAT dt, bool& collision, FLOAT& lambda, D3DXVECTOR3* cn) {
	bool contact, contactij, contactji, collisionij, collisionji;
	bool penetrationij, penetrationji;
	FLOAT lambdaij, lambdaji;
	D3DXVECTOR3 ni, nj;

	collision = false;
	// two calls to collidesWith because collidesWith uses a bounding sphere
	// for the object passed as the argument in the call.
	// - first call compares to bounding sphere on mj
	// - second call compares to bounding sphere on mi
	// (this could be improved)
	contactij = mi->contactWith(mj, dt, collisionij, penetrationij, lambdaij,
	 &ni);
	contactji = mj->contactWith(mi, dt, collisionji, penetrationji, lambdaji,
	 &nj);
    if (collisionij || collisionji) {
		// a collision without sustained contact
		// select the closest approach as the collision
		if (lambdaij > lambdaji) {
		    lambda = lambdaij;
		    *cn = ni;
		} else {
	        lambda = lambdaji;
		    *cn = nj;
		}
		contact = true;
		collision = true;
	} else if (contactij && !collisionij || contactji && !collisionji) {
		contact = true;
        collision = false;
        if (contactij)
            *cn = ni;
        else
            *cn = nj;
	} else {
		contact = false;
		collision = false;
	}
	return contact;
}