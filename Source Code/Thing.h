///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Thing.h
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
///////////////////////////////////////////////////////////////////////////////////////

#ifndef _THING_H_
#define _THING_H_

#include <d3d9.h>       // basic Direct3D
#include <d3dx9math.h>  // for C++ matrix algebra extensions, etc.

// ULTVERTEX structure: an unlit vertex with a texture coordinates.
//
struct ULTVERTEX {

	FLOAT x, y, z;       // coordinates for the vertex
	FLOAT nx, ny, nz;    // normal (perpendicular) vector for lighting
	FLOAT tu, tv;        // texture coordinates
};

// D3DFVF_ULTVERTEX: our selected flexible vertex format includes
// a position, a normal vector, a texture coordinate set of size 2
//
#define D3DFVF_ULTVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 |\
                          D3DFVF_TEXCOORDSIZE2(0))

// Distance below the FLOOR at which a VolumeThing falls out of existence
#define FINISHED 10
// Acceleration due to gravity in m/s^2
#define GRAVITY (- 9.8f)
// Angular Acceleration about z axis in rad/s
#define ANG_ACCEL_Z (- 2.0f)
// Particle Dimensions in m
#define SMALL (1.0f)
// Default maximum number of vertices
#define MAX_VERTICES 300
// Zero Tolerance
#define NEAR_ZERO (1.0E-10f)
// Moment of Inertia factor for a Brick
#define I_BRICK(a,b) (((a)*(a) + (b)*(b))/12.0f)
// Bounding Radius of a Brick
#define R_BRICK(a,b,c) (0.5f * sqrtf((a)*(a) + (b)*(b) + (c)*(c)))
// Collision tolerance - proximity
#define COLLISION_TOLERANCE 1.1f
// Contact tolerance - velocity orientation
#define CONTACT_TOLERANCE 0.9f
// Relative velocity^2 tolerance for static/kinetic distinction
#define RELATIVE_V_TOL 1.0E-3f
// maximum number of time sub-increments
#define MAX_SUB_INCREMENTS 20
// coefficients of restitution
#define PURELY_ELASTIC   (1.0f)
#define PARTLY_ELASTIC   (0.5f)
#define PURELY_INELASTIC (0.0f)
// contact tolerance
#define LAYER 0.0001f

// "Floor" of the universe - the lowest possible y value
// for any viewpoint
#define FLOOR 3

class Context;
class Model;
class VolumeThing;
class Box;

// a Thing in our 3-D universe is made of some material, may have an
// associated texture and can be rotated, moved, and drawn.
//
class Thing {
    D3DXMATRIX world;               // world transformation matrix
    int maxcount;                   // maximum number of vertices
    int count;                      // actual number of vertices
    int primitivecnt;               // how many primitives
    LPDIRECT3DVERTEXBUFFER9 vb;     // vertex buffer (stores vertices)
    D3DPRIMITIVETYPE type;          // e.g. point, triangle strip, etc.
    D3DMATERIAL9 mat;               // material that a Thing is made of
    LPDIRECT3DTEXTURE9 tex;         // texture spread over its surface

    Thing(const Thing &){}            // intentionally omitted...
    Thing &operator=(const Thing &){} // ... to prevent copies

  protected:
    // no need for public access - used by ctors only
    void addside(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3,
     D3DXVECTOR3 p4, D3DXVECTOR3 normal);
    void addBrick(FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx,
     FLOAT maxy, FLOAT maxz);

  public:
    Thing(LPDIRECT3DDEVICE9 c, D3DCOLORVALUE clr, D3DPRIMITIVETYPE t = D3DPT_POINTLIST, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    virtual ~Thing();
    virtual void add(FLOAT x, FLOAT y, FLOAT z, FLOAT nx, FLOAT ny, FLOAT nz, FLOAT tu, FLOAT tv, int newprimitive);
    virtual void draw(LPDIRECT3DDEVICE9 c);
    virtual void rotatex(float rad);
    virtual void rotatey(float rad);
    virtual void rotatez(float rad);
    virtual void rotate(D3DXMATRIX* rot);
    virtual void move(FLOAT x, FLOAT y, FLOAT z);
	virtual void moveCamera(FLOAT x, FLOAT y, FLOAT z);
    virtual void copyWorld(Thing* frame);
};

// An PlaneThing is a plane in 3-D that divides the space into two half
// spaces.
//
class PlaneThing : public Thing {
	D3DXVECTOR3 n;         // normal to the plane
	FLOAT p;               // plane constant
	bool allowPenetration; // penetration flag

  public:
    PlaneThing(bool allow, D3DXVECTOR3 normal, D3DXVECTOR3 point, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile = NULL,
     int maxvertices = MAX_VERTICES);
    virtual bool permeable() const {return allowPenetration;}
    virtual bool isHitBy(VolumeThing* other, FLOAT& lambda, FLOAT dt, D3DXVECTOR3* cn) const;
    virtual void collision(VolumeThing* other, D3DXVECTOR3* n, FLOAT dt);
};

// An BoundedPlane is a PlaneThing that is bounded by 4 planes
// so that it forms a rectangular area about centre
//
class BoundedPlane : public PlaneThing {
	D3DXVECTOR3 centre; // centre of the plane
	D3DXVECTOR3 nx;     // normal to the yl-zl plane
	D3DXVECTOR3 nz;     // normal to the xl-yl plane
	FLOAT sx;           // half side length in xl direction
	FLOAT sz;           // half side length in zl direction

  public:
    BoundedPlane(bool allow, FLOAT breadth, FLOAT depth, D3DXVECTOR3 normal, D3DXVECTOR3 point, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    virtual bool isHitBy(VolumeThing* other, FLOAT& lambda, FLOAT dt, D3DXVECTOR3* cn) const;
};

// An VolumeThing is a bounded volume. A VolumeThing has a centroid,
// a locally defined launch direction, a linear velocity and a linear
// acceleration, as well as a locally defined launch axis for angular
// motion, an angular velocity and an angular acceleration.
// A VolumeThing is the base class for each mobile thing in the scene.
//
class VolumeThing : public Thing {
    D3DXVECTOR3 centre;        // geometric centroid
    D3DXVECTOR3 launchDir;     // launch direction - locally defined
    D3DXVECTOR3 v;             // linear velocity
    D3DXVECTOR3 a;             // linear acceleration
    D3DXVECTOR3 launchAxis;    // launch axis - locally defined
    D3DXVECTOR3 omega;         // angular velocity
    D3DXVECTOR3 alpha;         // angular acceleration
    FLOAT       mass;          // amount of matter
    D3DXVECTOR3 centreMass;    // offset from centre to the centre of mass
    D3DXVECTOR3 impuForces;    // impulse forces on the body
    D3DXVECTOR3 bodyForces;    // non-contact forces on the body
    D3DXVECTOR3 moments;       // moments on the body
    D3DXMATRIX  mInertia;      // moment of inertia of the body
    D3DXMATRIX  mInertiaInv;   // inverse of the moment of inertia of the body
    D3DXMATRIX  rotation;      // rotation matrix for local coordinates
    FLOAT restitution;         // coefficient of restitution
    FLOAT radius;              // radius of the bounding sphere
    bool supported;            // supported - absorbs forces and moments

    void init(FLOAT m, FLOAT cr, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad);
    FLOAT staticFriction(VolumeThing* other) const {return 0.7f;}
    FLOAT kineticFriction(VolumeThing* other) const {return 0.4f;}

  public:
    VolumeThing(FLOAT m, FLOAT rr, FLOAT cr, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices);
    VolumeThing(FLOAT m, FLOAT rr, FLOAT cr, Model* data, FLOAT scale, bool smooth, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char *texfile = NULL);
    VolumeThing(FLOAT m, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad, FLOAT cr, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile, int maxvertices);
    VolumeThing(FLOAT m, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad, FLOAT cr, Model* data, FLOAT scale, bool smooth, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char *texfile = NULL);
    virtual void setOmega(D3DXVECTOR3 w) {omega = w;}
    virtual void setDirection(D3DXVECTOR3* d) {launchDir = *d;}
    virtual void rotate(D3DXMATRIX* rot);
    virtual void rotate(D3DXMATRIX* rot, D3DXVECTOR3* c);
    virtual void move(FLOAT delx, FLOAT dely, FLOAT delz);
	virtual void moveCamera(FLOAT delx, FLOAT dely, FLOAT delz);
    virtual void update(FLOAT dt);
    virtual void start(FLOAT speed, FLOAT aSpeed);
    virtual bool finished() const;
    virtual void support() {supported = true;}
    virtual void initialize(VolumeThing* frame);
    virtual void augment(VolumeThing* frame, D3DXVECTOR3* offset);
    virtual void applyForce(D3DXVECTOR3* f, D3DXVECTOR3* r = NULL);
    virtual void applyMoment(D3DXVECTOR3* m) {if (!supported) moments += *m;}
    virtual void applyBodyForce(D3DXVECTOR3* f) {if (!supported) bodyForces += *f;}
    virtual bool contains(D3DXVECTOR3* x) const;
    virtual bool contactWith(VolumeThing* other, FLOAT dt, bool& collision, bool& penetration, FLOAT& lambda, D3DXVECTOR3* n) const;
    virtual void contact(VolumeThing* other, D3DXVECTOR3* n);
    virtual void collision(VolumeThing* other, D3DXVECTOR3* n, FLOAT dt);
    virtual D3DXVECTOR3 contactPoint(VolumeThing* other) const;
    virtual D3DXVECTOR3 penetration(D3DXVECTOR3* x) const;
    virtual D3DXVECTOR3 position(FLOAT dt) const;
    virtual D3DXVECTOR3 clearance() const {return radius * D3DXVECTOR3(1,1,1);}
    virtual FLOAT boundingRadius() const {return radius;}
    D3DXVECTOR3 velocity() const {return v;}
    FLOAT getMass() const {return mass;}
    virtual FLOAT coefficientRestitution() const {return restitution;}
    // for derived classes only
    virtual void scale(FLOAT factor) {}
    virtual bool detachable() const {return false;}
    virtual VolumeThing* cut(FLOAT m, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char* texfile = NULL);

	float GetYPosition(){return centre.y;}
};

// A BoundedBody is a VolumeThing with a bounding box
//
class BoundedBody : public VolumeThing {
	D3DXVECTOR3 nx;         // normal to the y-z plane
	D3DXVECTOR3 ny;         // normal to the x-z plane
	D3DXVECTOR3 nz;         // normal to the x-y plane
	FLOAT sx;               // half side length in x direction
	FLOAT sy;               // half side length in y direction
	FLOAT sz;               // half side length in z direction
	D3DXVECTOR3 centre;     // geometric centre

    bool intersects(D3DXVECTOR3* cb, D3DXVECTOR3* ce, const D3DXVECTOR3* na, FLOAT sa, const D3DXVECTOR3* nb, FLOAT sb, const D3DXVECTOR3* nc, FLOAT sc, FLOAT& lambda, D3DXVECTOR3* n, bool& collision) const;

  public:
    BoundedBody(FLOAT m, FLOAT cr, FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx, FLOAT maxy, FLOAT maxz, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    void rotate(D3DXMATRIX* rot);
    void rotate(D3DXMATRIX* rot, D3DXVECTOR3* c);
    void move(FLOAT delx, FLOAT dely, FLOAT delz);
    bool contactWith(VolumeThing* other, FLOAT dt, bool& collision, bool& penetration, FLOAT& lambda, D3DXVECTOR3* n) const;
    bool inContactWith(VolumeThing* other) const;
    bool contains(D3DXVECTOR3* x) const;
    D3DXVECTOR3 contactPoint(VolumeThing* other) const;
    D3DXVECTOR3 penetration(D3DXVECTOR3* x) const;
    D3DXVECTOR3 clearance() const {return D3DXVECTOR3(sx, sy, sz);}
};

// An AttachableVolume is a VolumeThing that revolves around another
// VolumeThing until the user detaches the AttachableVolume (through
// a call to start)
//
class AttachableVolume : public VolumeThing {
	bool attached;         // still attached to frame?
	VolumeThing* frame;    // frame around which object revolves
	D3DXVECTOR3 offset;    // offset from frame origin

	void init(VolumeThing* parent, D3DXVECTOR3 offset); // called by ctors

  public:
    AttachableVolume(FLOAT m, FLOAT rr, FLOAT cr, VolumeThing* parent, D3DXVECTOR3 offset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    AttachableVolume(FLOAT m, FLOAT xx, FLOAT yy, FLOAT zz, FLOAT rad, FLOAT cr, VolumeThing* parent, D3DXVECTOR3 offset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    virtual void rotate(D3DXMATRIX* rot);
    virtual void rotate(D3DXMATRIX* rot, D3DXVECTOR3* c);
	virtual void update(FLOAT dt);
    virtual bool detachable() const {return attached;}
    virtual void start(FLOAT speed, FLOAT aSpeed);
};

// A SheddingVolume is a VolumeThing that releases a Revolving Brick at an
// offset from the centre of the SheddingVolume.  A SheddingVolume itself
// has the shape of a brick.
//
class SheddingVolume : public VolumeThing {
	FLOAT bWidth;        // width of revolving brick
	FLOAT bDepth;        // depth of revovling brick
	FLOAT bLength;       // length of revolving brick
	D3DXVECTOR3 offset;  // offset of launch chamber from the centre

  public:
    SheddingVolume(FLOAT m, FLOAT width, FLOAT length, FLOAT cr, FLOAT clearance, FLOAT aspect, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    SheddingVolume(FLOAT m, FLOAT cube, FLOAT cr, D3DXVECTOR3 offset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    virtual void rotate(D3DXMATRIX* rot);
    virtual void rotate(D3DXMATRIX* rot, D3DXVECTOR3* c);
    virtual void scale(FLOAT factor) {offset *= factor;}
    VolumeThing* cut(FLOAT m, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, const char* texfile = NULL);
};

// A Box is a Thing that can be specified with two points - lower close
// left, and upper faraway right. A Box has a colour and possibly a texture.
//
class Box : public Thing {
	D3DXVECTOR3 nx;    // normal to the y-z plane
	D3DXVECTOR3 ny;    // normal to the x-z plane
	D3DXVECTOR3 nz;    // normal to the x-y plane
	D3DXVECTOR3 centre;
	FLOAT sx;          // half side length in x direction
	FLOAT sy;          // half side length in y direction
	FLOAT sz;          // half side length in z direction
	FLOAT radius;      // radius of bounding sphere

  public:
    Box(FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx, FLOAT maxy, FLOAT maxz, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL);
    void rotate(D3DXMATRIX* rot);
    void move(FLOAT delx, FLOAT dely, FLOAT delz);
    virtual D3DXVECTOR3 centreOf() const {return centre;}
    virtual FLOAT radiusOf() const {return radius;}
};

// A RevolvingBrick is a AttachableVolume in the shape of a brick
//
class RevolvingBrick : public AttachableVolume {
  public:
    RevolvingBrick(FLOAT m, FLOAT width, FLOAT depth, FLOAT length, FLOAT cr, VolumeThing* parent, D3DXVECTOR3 offset, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL);
};

// A Brick is a BoundedBody with the shape of a brick
//
class Brick : public BoundedBody {
	D3DXVECTOR3 centre; // geometric centre
	mutable D3DXVECTOR3 cn;  // normal to contact surface

  public:
    Brick(FLOAT m, FLOAT minx, FLOAT miny, FLOAT minz, FLOAT maxx, FLOAT maxy, FLOAT maxz, FLOAT cr, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL);
    Brick(FLOAT m, FLOAT cube, FLOAT cr, D3DCOLORVALUE color, LPDIRECT3DDEVICE9 c, const char *texfile = NULL);
    void move(FLOAT delx, FLOAT dely, FLOAT delz);
};

// A FuzzyPlaneThing. Just before drawing it, bilinear filtering, both
// when the texture has to be magnified and when it has to be shrunk,
// is turned on. After drawing, filtering is turned off.
//
class FuzzyPlaneThing : public PlaneThing {
  public:
    FuzzyPlaneThing(D3DXVECTOR3 n, D3DXVECTOR3 p, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t = D3DPT_POINTLIST, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    void draw(LPDIRECT3DDEVICE9 c);
};

// A BoundedFuzzyPlaneThing is a FuzzyPlaneThing with 4 bounding planes.
//
class BoundedFuzzyPlaneThing : public BoundedPlane {
  public:
    BoundedFuzzyPlaneThing(FLOAT breadth, FLOAT depth, D3DXVECTOR3 n, D3DXVECTOR3 p, LPDIRECT3DDEVICE9 c, D3DCOLORVALUE color, D3DPRIMITIVETYPE t = D3DPT_POINTLIST, const char *texfile = NULL, int maxvertices = MAX_VERTICES);
    void draw(LPDIRECT3DDEVICE9 c);
};

// the grass around the outside of the road
// draw just below FLOOR so we'll always see it
//
class grass : public FuzzyPlaneThing {
  public:
    grass(LPDIRECT3DDEVICE9 c, const char *texfile = NULL);
};

// the road around the edge of the lake
// draw just below FLOOR so we'll always see it
//
class road : public FuzzyPlaneThing {
  public:
    road(LPDIRECT3DDEVICE9 c, const char *texfile = NULL);
};

// the lake inside the road
// draw just below FLOOR so we'll always see it
//
class lake : public BoundedFuzzyPlaneThing {
  public:
    lake(LPDIRECT3DDEVICE9 c, const char *texfile = NULL);
};

bool contactBetween(VolumeThing* mi, VolumeThing* mj, FLOAT dt, bool& collide, FLOAT& lambda, D3DXVECTOR3* cn);

#endif
