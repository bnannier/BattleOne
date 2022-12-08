///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  WinMain.cpp
//	Start Date: 02/09/2005
//	End Date:   10/05/2005
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
// MsgProc(): Default processing for any window messages that an application does not process.
// ----------------------------------------------------------------------------------------------------
LRESULT FAR PASCAL MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


// ----------------------------------------------------------------------------------------------------
// WindowProc(): The message procedure - empty except for destroy message.
// ----------------------------------------------------------------------------------------------------
long FAR PASCAL AppWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	switch(uMsg){
		case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		default: return MsgProc(hWnd, uMsg, wParam, lParam);
	}
}


// ----------------------------------------------------------------------------------------------------
// WinMain(): Application starting point.
// ----------------------------------------------------------------------------------------------------
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow){
	cApplication App;
	return App.Run();
}


// ----------------------------------------------------------------------------------------------------
// Application(): Constructor which sets all variables within this class to defaults.
// ----------------------------------------------------------------------------------------------------
cApplication::cApplication(){
	// Get the instance handle
	m_hInst = GetModuleHandle(NULL);
	strcpy(w_Class, "BattleOne");
	strcpy(w_Caption, "BattleOne by Bobby Nannier");

	// Set default window style, position, width, height, BPP
	w_Style  = WS_OVERLAPPEDWINDOW;
	w_XPos   = 0;
	w_YPos   = 0;
	w_Width  = 256;
	w_Height = 256;
	m_tex = 32;

	// Set default WNDCLASSEX structure
	w_wcex.cbSize        = sizeof(WNDCLASSEX);
	w_wcex.style         = CS_CLASSDC;
	w_wcex.lpfnWndProc   = AppWindowProc;
	w_wcex.cbClsExtra    = 0;
	w_wcex.cbWndExtra    = 0;
	w_wcex.hInstance     = m_hInst;
	w_wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	w_wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	w_wcex.hbrBackground = NULL;
	w_wcex.lpszMenuName  = NULL;
	w_wcex.lpszClassName = w_Class;
	w_wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	w_Width  = 640; 
	w_Height = 480;
	w_Style  = WS_BORDER | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;

	time = 0;
	m_Life = 100;

	level = NULL;			// Level Terrain
	m_ForceFeedback = false;
	flag = 3;
}


// ----------------------------------------------------------------------------------------------------
// Run(): Runs the Application, Enters the message pump and Does per-frame processing.
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::Run(){
	MSG Msg;
	time = timeGetTime();

	// Register window class
	if(!RegisterClassEx(&w_wcex))
		return FALSE;

	// Create the Main Window
	m_hWnd = CreateWindow(w_Class, w_Caption, w_Style, w_XPos, w_YPos, w_Width, w_Height, NULL, NULL, m_hInst, NULL);

	if(!m_hWnd)
		return FALSE;

	// Show and update the window
	ShowWindow(m_hWnd, SW_NORMAL);
	UpdateWindow(m_hWnd);

	// Make sure client area is correct size
	Resize(w_Width, w_Height);

	// Initialize COM
	CoInitialize(NULL);

	if(Init(FALSE) == TRUE){
		// Enter the message pump
		ZeroMemory(&Msg, sizeof(MSG));
		while(Msg.message != WM_QUIT){
			// Handle Windows messages (if any)
			if(PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)){
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}else{
				// Do per-frame processing, break on FALSE return value
				if(Frame() == FALSE)
                    break;
			}
		}
	}

	Shutdown();
	// Shutdown COM
	CoUninitialize();

	// Unregister the window class
	UnregisterClass(w_Class, m_hInst);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Init(): Initializes all the devices.
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::Init(BOOL val){
	if(MessageBox(m_hWnd,"Turn Force Feedback ON?", "Joystick", MB_YESNO) == 6)
            m_ForceFeedback = true;
    
	// Initialize the graphics device and set display mode
	m_Graphics.Init();
	if(val == FALSE)
        m_Graphics.SetMode(GethWnd(), FALSE, TRUE, 0, 0,32);
	else if (val == TRUE)
		m_Graphics.SetMode(GethWnd(), FALSE, TRUE,(long)m_res_w,(long)m_res_h,(char)m_tex);

    m_Graphics.SetPerspective(D3DX_PI/4, 1.3333f, 1.0f, 1000.0f);
	ShowMouse(FALSE);

	// Create a font
    m_Font.Create(&m_Graphics, "Arial");

	// Enable lighting and setup light
	m_Graphics.EnableLighting(TRUE);
	m_Graphics.SetAmbientLight(10,10,10);
	m_Graphics.EnableLight(0, TRUE);
	m_Light.SetAttenuation0(0.4f);
	m_Light.SetRange(1000.0f);

	// Initialize input and input devices
	m_Input.Init(GethWnd(), GethInst());
	m_Keyboard.Create(&m_Input, KEYBOARD);
	m_Mouse.Create(&m_Input, MOUSE, TRUE);

	// Force Feedback ON
	if(m_ForceFeedback){
		if(!m_Joystick.Create(&m_Input, JOYSTICK, TRUE))
			return false;

		joystick = m_Joystick.GetEffects();
		joystick->Start(1, 0);
	}

	// Load Terrain from RAW file
	level = new Terrain(m_Graphics.GetDeviceCOM(), "terrain.raw", 64, 64, 16, 1.0f);
	D3DXVECTOR3		lightDirection(0.0f, 1.0f, 0.0f);
	level->genTexture(&lightDirection);

	// Mesh

	// Position view at origin
	m_XPos = m_YPos = m_ZPos = -400.0f;

	// Rotate view at origin
	m_Camera.SetXRotation(0.0f);
	m_Camera.SetYRotation(0.0f);
	m_Camera.SetZRotation(0.0f);

	// Particle System
	// Create the Firework system.
	for(int i = 0; i < 6; i++){
        m_Exp[i] = 0;	
	}
	D3DXVECTOR3 origin0(-400.0f, 300.0f, -400.0f);
	m_Exp[0] = new psys::Firework(&origin0, 6000);
	m_Exp[0]->init(m_Graphics.GetDeviceCOM(), "flare.bmp");

	D3DXVECTOR3 origin1(-380.0f, 300.0f, -400.0f);
	m_Exp[1] = new psys::Firework(&origin1, 6000);
	m_Exp[1]->init(m_Graphics.GetDeviceCOM(), "flare.bmp");

	D3DXVECTOR3 origin2(-300.0f, 300.0f, -400.0f);
	m_Exp[2] = new psys::Firework(&origin2, 6000);
	m_Exp[2]->init(m_Graphics.GetDeviceCOM(), "flare.bmp");

	D3DXVECTOR3 origin3(-340.0f, 300.0f, -400.0f);
	m_Exp[3] = new psys::Firework(&origin3, 6000);
	m_Exp[3]->init(m_Graphics.GetDeviceCOM(), "flare.bmp");

	D3DXVECTOR3 origin4(-320.0f, 300.0f, -400.0f);
	m_Exp[4] = new psys::Firework(&origin4, 6000);
	m_Exp[4]->init(m_Graphics.GetDeviceCOM(), "flare.bmp");

	D3DXVECTOR3 origin5(-300.0f, 300.0f, -400.0f);
	m_Exp[5] = new psys::Firework(&origin5, 6000);
	m_Exp[5]->init(m_Graphics.GetDeviceCOM(), "flare.bmp");

	// Fog effects
	FogEffect   = 0;
	errorBuffer = 0;
	FogTechHandle = 0;
	hr = D3DXCreateEffectFromFile(m_Graphics.GetDeviceCOM(), "fog.txt", 0, 0, D3DXSHADER_DEBUG, 0, &FogEffect, &errorBuffer);
	FogTechHandle = FogEffect->GetTechniqueByName("Fog");

// PHYSICS: Things
	// Describe the vertex format
    m_Graphics.GetDeviceCOM()->SetFVF(D3DFVF_ULTVERTEX);

	// Init Red Box
    mobile[0] = new Brick(100, -10,-10, -10, 10, 10, 10, PURELY_ELASTIC, D3DXCOLOR(1.0, 0.5, 0.5, 1), m_Graphics.GetDeviceCOM(), "brown.jpg");	// Make a rotateable red box
	mobile[0]->move(-213.0f, level->getHeight(-213.0f, -137.0f)+ 18.0f, -137.0f);

	// Make a small yellow cannon that yields cannonboxes
    mobile[1] = new SheddingVolume(100.0f, 2.0f, 10.0f, PURELY_INELASTIC, 0.25f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 0.05f, 1), m_Graphics.GetDeviceCOM(), "red.jpg");
	mobile[1]->moveCamera(m_XPos, level->getHeight(m_XPos, m_ZPos), m_ZPos);
	// Rotate the cannon
    D3DXMATRIX rot;
    D3DXMatrixRotationZ(&rot, 0.7f);
    D3DXVECTOR3 centreOfRotation(m_XPos, m_YPos, m_ZPos);
    mobile[1]->rotate(&rot, &centreOfRotation);
	mobile[1]->rotatex(1.57f);
	mobile[1]->rotatey(0.785f);

	// Make a large plank
    mobile[2] = new Brick(50, -50, -1, -5, 50, 1, 5, PURELY_ELASTIC, D3DXCOLOR(1.0, 1.0, 0.5, 1), m_Graphics.GetDeviceCOM(), "gray.jpg");
	mobile[2]->move(-140.0f, level->getHeight(-140.0f, -200.0f)+4, -200.0f);

    // Make a sliding brick for the large plank
    mobile[3] = new Brick(50, -5, -2.5, -2.5, 5, 2.5, 2.5, PURELY_ELASTIC, D3DXCOLOR(1.0, 0.5, 0.5, 1), m_Graphics.GetDeviceCOM(), "green.jpg");
	mobile[3]->move(-140.0f, level->getHeight(-140.0f, -200.0f)+7.5f, -200.0f);
	// Apply gravity to the sliding brick
    D3DXVECTOR3 f(0, -490, 0);
    mobile[3]->applyBodyForce(&f);

	// Make a falling brick
    mobile[4] = new Brick(50, -5, -2.5, -2.5, 5, 2.5, 2.5, PURELY_ELASTIC, D3DXCOLOR(1.0, 0.5, 0.5, 1), m_Graphics.GetDeviceCOM(), "green.jpg");
	mobile[4]->move(-300.0f, level->getHeight(-300.0f, -300.0f)+300.0f, -300.0f);
	// Apply gravity to the falling brick
    f = D3DXVECTOR3(0, -490, 0);
    mobile[4]->applyBodyForce(&f);

	// Make an orange cube that will release revolving particles
    mobile[5] = new SheddingVolume(50.0f, 8.0f, PARTLY_ELASTIC, D3DXVECTOR3(4.0f, 4.0f, -4.1f), D3DXCOLOR(1.0f, 0.5f, 0.2f, 1), m_Graphics.GetDeviceCOM(), "orange.jpg");
	mobile[5]->move(-350.0f, level->getHeight(-350.0f, -250.0f)+30.0f, -250.0f);
	mobile[5]->setOmega(D3DXVECTOR3(0, 0, 3));

	nLastingMobiles = 6;
	nStationaries = 0;

	// Total number of mobile things
	nMobiles = nLastingMobiles;

	// Mobiles are supported that is, they absorb forces and transfer them into the supports
    for (int i = 0; i < nMobiles; i++)
        mobile[i]->support();
// PHYSICS: END
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Shutdown(): Shutdown all devices.
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::Shutdown(){
	// Delete Mobiles
	for (int i = 0; i < nMobiles; i++) {
        if (mobile[i]) {
            delete mobile[i];
            mobile[i] = NULL;
        }
    }

	// Shutdown input devices
	if(m_ForceFeedback)
        m_Joystick.Free();
	m_Mouse.Free();
	m_Keyboard.Free();
	m_Input.Shutdown();

	// Free font
	m_Font.Free();

	// Shutdown graphics
	m_Graphics.Shutdown();

	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Frame(): Per-frame processing
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::Frame(){
	// Lights
	Light				Light;

	// Camera
	float				XMove, YMove, ZMove;
	D3DXVECTOR2			vecDir;

	// Calculate Elapsed Time
	unsigned	long	Elapsed;
	static int now = 0;
	float nowtry, dt;
	Elapsed = (timeGetTime() - Timer);
	Timer = timeGetTime();
	nowtry = (float)timeGetTime();

	// Plank left, right, true
	bool cp;
	float dp;

	// Limit fps to 200 for better accuracy
	if(nowtry - now >= 5){
        now = (int)nowtry;
        dt = (float)now - (float)time;

		// Convert from milliseconds received to seconds
		dt *= 0.000001f;

// START: Get Keyboard and Mouse Input
		// Acquire input
		m_Mouse.Acquire(TRUE);
		if(m_ForceFeedback)
			m_Joystick.Acquire(TRUE);

		// Get input
		m_Keyboard.Read();
		m_Mouse.Read();
		if(m_ForceFeedback)
			m_Joystick.Read();
// END: Get Keyboard and Mouse Input

// START: Enter Menu or Quit Game
		if(m_Keyboard.GetKeyState(KEY_M) == TRUE || m_Keyboard.GetKeyState(KEY_O) == TRUE){
			ShowMouse(TRUE);
			// Get values for teture and resolution
			m_tex = (int)m_Graphics.GetFormatBPP(m_Graphics.GetDisplayMode().Format);
			m_res_w = (int)m_Graphics.GetDisplayMode().Width;
			m_res_h = (int)m_Graphics.GetDisplayMode().Height;

			while(MenuFrame()== TRUE);
			Init(TRUE);
			ShowMouse(FALSE);
		}

		// ESC quits program
		if(m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
			return FALSE;

		ShowMouse(FALSE);
// END: Enter Menu or Quit Game

// START: Character Movement (Camera View)
		// Process movement
		XMove = YMove = ZMove = 0.0f;

		// Move Forwards
		if(m_Keyboard.GetKeyState(KEY_UP) == TRUE){
			XMove = ((float)sin(m_Camera.GetYRotation()) * Elapsed)/20;
			ZMove = ((float)cos(m_Camera.GetYRotation()) * Elapsed)/20;
		}

		// Move Backwards
		if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE){
			XMove = -((float)sin(m_Camera.GetYRotation()) * Elapsed)/20;
			ZMove = -((float)cos(m_Camera.GetYRotation()) * Elapsed)/20;
		}

		// Strafe Left
		if(m_Keyboard.GetKeyState(KEY_LEFT) == TRUE){
			XMove = ((float)sin(m_Camera.GetYRotation() - 1.57f) * Elapsed)/20;
			ZMove = ((float)cos(m_Camera.GetYRotation() - 1.57f) * Elapsed)/20;
		}

		// Strafe Right
		if(m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE){
			XMove = ((float)sin(m_Camera.GetYRotation() + 1.57f) * Elapsed)/20;
			ZMove = ((float)cos(m_Camera.GetYRotation() + 1.57f) * Elapsed)/20;
		}

		// Move Forwards, while Strafing Left
		if(m_Keyboard.GetKeyState(KEY_UP) == TRUE && m_Keyboard.GetKeyState(KEY_LEFT) == TRUE){
			XMove = ((float)sin(m_Camera.GetYRotation() - 0.785f) * Elapsed)/20;
			ZMove = ((float)cos(m_Camera.GetYRotation() - 0.785f) * Elapsed)/20;
		}

		// Move Forwards, while Strafing Right
		if(m_Keyboard.GetKeyState(KEY_UP) == TRUE && m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE){
			XMove = ((float)sin(m_Camera.GetYRotation() + 0.785f) * Elapsed)/20;
			ZMove = ((float)cos(m_Camera.GetYRotation() + 0.785f) * Elapsed)/20;
		}

		// Move Backwards, while Strafing Left
		if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE && m_Keyboard.GetKeyState(KEY_LEFT) == TRUE){
			XMove = -((float)sin(m_Camera.GetYRotation() + 0.785f) * Elapsed)/20;
			ZMove = -((float)cos(m_Camera.GetYRotation() + 0.785f) * Elapsed)/20;
		}

		// Move Backwards, while Strafing Right
		if(m_Keyboard.GetKeyState(KEY_DOWN) == TRUE && m_Keyboard.GetKeyState(KEY_RIGHT) == TRUE){
			XMove = -((float)sin(m_Camera.GetYRotation() - 0.785f) * Elapsed)/20;
			ZMove = -((float)cos(m_Camera.GetYRotation() - 0.785f) * Elapsed)/20;
		}

		// Update view coordinates
		m_XPos += XMove;
		m_YPos =  level->getHeight(m_XPos, m_ZPos); // 0.0f; //
		m_ZPos += ZMove;

		// Position camera
		m_Camera.Move(m_XPos, m_YPos + 6.0f, m_ZPos);
		mobile[1]->moveCamera(m_XPos, level->getHeight(m_XPos, m_ZPos)+8, m_ZPos);	// Set cannon at camera view
		//mobile[1]->moveCamera(-400.0f, level->getHeight(-400.0f, -400.0f)+10, -400.0f);	// Set cannon at camera view
		m_Graphics.SetCamera(&m_Camera);

		// Position light on top of Camera
		m_Light.Move(m_XPos, m_YPos+60.0f, m_ZPos);
		m_Graphics.SetLight(0, &m_Light);
// END: Character Movement (Camera View)

// START: Character Rotation (Camera View)
		////DirectX 3D///////////////////////Mouse Delta////////////////////
		//								 //								  //
		//	             y+				 //	              y+			  //
		//	             |    z+		 //	              |				  //
		//	             |   /			 //	              |   			  //
		//	             |  /			 //	              |  			  //
		//	             | /			 //	              | 			  //
		//	             |/				 //	              |				  //
		//  x- ---------- ---------- x+  //  x- ---------- ---------- x+  //
		//	            /|				 //	              |				  //
		//	           / |				 //	              |				  //
		//	          /  |				 //	              |				  //
		//	         /   |				 //	              |				  //
		//	        z-   |				 //	              |				  //
		//	             y-				 //	              y-			  //
		//								 //								  //
		////////////////////////////////////////////////////////////////////
		//  DirectX 3D: x-axis == Mouse Delta: y-axis					  //
		//  DirectX 3D: y-axis == Mouse Delta: x-axis					  //
		////////////////////////////////////////////////////////////////////

		// Keep x-axis rotation from going all the way around
		if((float)m_Camera.GetXRotation() >= -1.5f && (float)m_Camera.GetXRotation() <= 1.0f){
			// Mouse
			m_Camera.RotateRel((float)m_Mouse.GetYDelta() / 200.0f, (float)m_Mouse.GetXDelta() / 200.0f, 0.0f);

			// Cannon Rotation
			D3DXMATRIX rot;
			D3DXVECTOR3 centreOfRotation(m_XPos, m_YPos, m_ZPos);

			// Rotate Cannon on X-axis based on Y-axis on mouse
			D3DXMatrixRotationX(&rot, ((float)m_Mouse.GetYDelta() / 200.0f));
			mobile[1]->rotate(&rot, &centreOfRotation);

			// Rotate Cannon on Y-axis based on x-axis on mouse
			D3DXMatrixRotationY(&rot, ((float)m_Mouse.GetXDelta() / 200.0f));
			mobile[1]->rotate(&rot, &centreOfRotation);

			// Joystick Force Feedback
			if(m_ForceFeedback)
				m_Camera.RotateRel(-((float)m_Joystick.GetXDelta() / 300000.0f), (float)m_Joystick.GetXDelta() / 300000.0f, 0.0f);
		}
		else if((float)m_Camera.GetXRotation() < -1.5f){
			m_Camera.SetXRotation(-1.5f);

			D3DXMATRIX rot;
			D3DXVECTOR3 centreOfRotation(m_XPos, m_YPos, m_ZPos);

			// Rotate Cannon on X-axis based on Y-axis on mouse
			D3DXMatrixRotationX(&rot, -((float)m_Mouse.GetYDelta() / 200.0f));
			mobile[1]->rotate(&rot, &centreOfRotation);
		}
		else if((float)m_Camera.GetXRotation() > 1.0f){
			m_Camera.SetXRotation(1.0f);

			D3DXMATRIX rot;
			D3DXVECTOR3 centreOfRotation(m_XPos, m_YPos, m_ZPos);

			// Rotate Cannon on X-axis based on Y-axis on mouse
			D3DXMatrixRotationX(&rot, -((float)m_Mouse.GetYDelta() / 200.0f));
			mobile[1]->rotate(&rot, &centreOfRotation);
		}
// END: Character Rotation (Camera View)

// START: Special Key Press
		// Fire Weapon: Left Mouse Button
		if(m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE){
			flag++;

			if(flag == 6){
				flag = 0;
				// Make a cannonbox
				int next = nLastingMobiles;
				if (nextEmpty(mobile, nMobiles, next)) {
					mobile[next] = mobile[1]->cut(1.0f, m_Graphics.GetDeviceCOM(), D3DXCOLOR(1.0f, 0.1f, 0.1f, 1));
					// Fire the 'cannonbox'
					if (mobile[next]) {
						mobile[next]->start(LIN_VELOCITY_0, ANG_VELOCITY_0);
						if (next == nMobiles)
							nMobiles++;
					}
				}
			}
		}

		// Rotate plank left
		if(m_Keyboard.GetKeyState(KEY_A) == TRUE){
			cp = true;
			dp = ANG_VELOCITY_0 * dt;
		}

		// Rotate plank right
		if(m_Keyboard.GetKeyState(KEY_S) == TRUE){
			cp = true;
			dp -= ANG_VELOCITY_0 * dt;
		}

		if(cp){
			D3DXMATRIX rot;
			D3DXMatrixRotationZ(&rot, dp/30);
			mobile[2]->rotate(&rot);
			D3DXVECTOR3 centreOfRotation(-140.0f, level->getHeight(-140.0f, -200.0f)+4, -200.0f);
			mobile[3]->rotate(&rot, &centreOfRotation);
		}

		// Reset falling block
		if(mobile[4]->GetYPosition() <= 5.0f)
			mobile[4]->move(0.0f, 600.0f, 0.0f);

		// Whether to change the particle offset ?
		if (m_Keyboard.GetKeyState(KEY_D) == TRUE) {
			// extend particle offset in particle box
			mobile[5]->scale(MOVE_FACTOR);
		}
		if (m_Keyboard.GetKeyState(KEY_F) == TRUE) {
			// retract particle offset in particle box
			mobile[5]->scale(1.0f/MOVE_FACTOR);
		}

		// Create a new particle at the particle offset
		if (m_Keyboard.GetKeyState(KEY_G) == TRUE) {
			// attach a particle at current particle offset
			int next = nLastingMobiles;
			if (nextEmpty(mobile, nMobiles, next)) {
				mobile[next] = mobile[5]->cut(1, m_Graphics.GetDeviceCOM(), D3DXCOLOR(1.0, 1.0, 1.0, 1));
				if (mobile[next] && next == nMobiles)
					nMobiles++;
			}
		}

		// Release the next particle in line naturally
		if (m_Keyboard.GetKeyState(KEY_H) == TRUE) {
			for (int i = nLastingMobiles; i < nMobiles; i++)
				if (mobile[i] && mobile[i]->detachable()) {
					// don't add any thrust or torque
					mobile[i]->start(0, 0);
					i = nMobiles;
				}
		}
// END: Special Key Press

// START: Physics Simulation
		// Keep rotate the red box
		mobile[0]->rotatex(boxSpeed);
		mobile[0]->rotatey(boxSpeed);
		mobile[0]->setOmega(D3DXVECTOR3(0.0f, 0.0f, -0.1f));

		// Update all mobile things using up to MAX_SUB_INCREMENTS time sub-increments of time increment
		// dt so as to process the collisions, if any, in the order of their occurence; in any event,
		// take no more than MAX_SUB_INCRMENTS to cover the full time increment dt
		D3DXVECTOR3 cn, n;
		int ci, cj, stepsLeft;
		bool collision, body, collide;
		bool contactM[MAX_MOBILES][MAX_MOBILES];
		D3DXVECTOR3 contactN[MAX_MOBILES][MAX_MOBILES];
		bool contactS[MAX_MOBILES][MAX_STATIONARIES];
		FLOAT lambda, lambdaij, /*lambdaji,*/ subDt;

		stepsLeft = MAX_SUB_INCREMENTS;
		while(dt > 0.0f && stepsLeft){
			// Determine the next collision, if any, and the size of the time
			// sub-increment (the next collision has the lowest value of lambdaij)
			ci = -1;
			cj = -1;
			lambda = 1.0001f;
			collision = false;
			for(int i = 0; i < nMobiles; i++){
				if(mobile[i]){
					for(int j = i + 1; j < nMobiles; j++){
						contactM[i][j] = false;
						if(mobile[j]){
							if(contactBetween(mobile[i], mobile[j], (float)dt, collide, lambdaij, &n)){
								if(!collide){
									contactM[i][j] = true;
									contactN[i][j] = n;
								}else if(lambdaij < lambda){
									lambda = lambdaij;
									ci = i;
									cj = j;
									cn = n;
									body = true;
									collision = true;
								}
							}
						}
					}
					if(i > 7){
						for(int j = 0; j < nStationaries; j++){
							if(!stationary[j]->permeable() && stationary[j]->isHitBy(mobile[i], lambdaij, (float)dt, &n) && lambdaij < lambda){
				    			lambda = lambdaij;
				    			ci = i;
				    			cj = j;
				    			cn = n;
				    			body = false;
				    			collision = true;
				    		}
						}
					}
				}
			}

			// The time sub-increment should not be below the size needed to ensure processing of the
			// entire increment for this, assume that the remaining sub-steps will be of equal size
			if(lambda * stepsLeft < 1.0f)
				lambda = 1.0f / stepsLeft;
			subDt  = lambda * dt;

			// Handle all contacting objects first
			for(int i = 0; i < nMobiles; i++){
				for(int j = i + 1; j < nMobiles; j++){
					if(contactM[i][j]){
						mobile[j]->contact(mobile[i], &contactN[i][j]);
					}
				}
				if(i > 7){
					for(int j = 0; j < nStationaries; j++){
						if(contactS[i][j]){
							//stationary[j]->contact(mobile[i]);
						}
					}
				}
			}

			// Handle the collision, if any, next
			if(collision){
				if(body){
					// Determine the impulse and apply the forces to the two colliding objects
					mobile[ci]->collision(mobile[cj], &cn, subDt);
				}else{
					// Determine the impulse and apply the force to the mobile object
					stationary[cj]->collision(mobile[ci], &cn, subDt);
				}
			}

			// Update all mobile things for time sub-increment subDt
			for(int i = 0; i < nMobiles; i++)
				if(mobile[i])
					mobile[i]->update(cannonSpeed);
			// Calculate remaining time step
			dt -= (DWORD)subDt;
			stepsLeft--;
		}

		// Remove any finished mobile things
		for(int i = nLastingMobiles; i < nMobiles; i++){
			if(mobile[i] && mobile[i]->finished()){
				delete mobile[i];
				mobile[i] = NULL;
			}
		}
// END: Physics Simulation

// START: Special Effects
		// Update Fireworks
		m_Exp[0]->update(0.005f);
		m_Exp[1]->update(0.006f);
		m_Exp[2]->update(0.007f);
		m_Exp[3]->update(0.008f);
		m_Exp[4]->update(0.009f);
		m_Exp[5]->update(0.010f);
		for(int i = 0; i < 6; i++){
			if(m_Exp[i]->isDead())
				m_Exp[i]->reset();
		}
// END: Special Effects

// START: Render Everything To Screen
		m_Graphics.Clear(0x00CCCCCC);
		if(m_Graphics.BeginScene() == TRUE) {
			m_Graphics.EnableZBuffer(TRUE);
			m_Graphics.EnableLighting(TRUE);

			// Terrain
			D3DXMatrixIdentity(&I);
			if(level)
				level->draw(&I, false);

			// Render Fireworks System
			for(int i = 0; i < 6; i++){
				m_Exp[i]->render();
			}

			// Fonts
			m_Font.Print("M - Menu", 0,0,0,0, 0xFFFF0000);
			m_Font.Print("ESC - Exit Game", 0,20,0,0, 0xFFFF0000);
			m_Font.Print("Move - Arrow Keys", 0,40,0,0, 0xFFFF0000);
			m_Font.Print("Look - Mouse or Joystick", 0,60,0,0, 0xFFFF0000);

			
			sprintf(Stats, "Mouse Button Left: Fire Red Particles");
			m_Font.Print(Stats, 0,100,0,0, 0xFFFF0000);

			sprintf(Stats, "A: Rotate Plank Left");
			m_Font.Print(Stats, 0,120,0,0, 0xFFFF0000);

			sprintf(Stats, "S: Rotate Plank Right");
			m_Font.Print(Stats, 0,140,0,0, 0xFFFF0000);

			sprintf(Stats, "D: Offset Particle Outwards");
			m_Font.Print(Stats, 0,160,0,0, 0xFFFF0000);

			sprintf(Stats, "F: Offset Particle Inwards");
			m_Font.Print(Stats, 0,180,0,0, 0xFFFF0000);

			sprintf(Stats, "G: Create White Particles");
			m_Font.Print(Stats, 0,200,0,0, 0xFFFF0000);

			sprintf(Stats, "H: Release White Particles");
			m_Font.Print(Stats, 0,220,0,0, 0xFFFF0000);

			// Fog Starts: Set the technique to use
			FogEffect->SetTechnique( FogTechHandle );
			UINT numPasses = 0;
    		FogEffect->Begin(&numPasses, 0);

			for(unsigned int i = 0; i < numPasses; i++){
				FogEffect->BeginPass(i);
			}

			// Terrain
			D3DXMatrixIdentity(&I);
			if(level)
				level->draw(&I, false);

			// Physics
			// draw the mobile things
			for(int i = 0; i < nMobiles; i++)
				if(mobile[i])
					mobile[i]->draw(m_Graphics.GetDeviceCOM());

			FogEffect->End();
			// Fog ends

			m_Graphics.EndScene();
		}
		m_Graphics.Display();
// END: Render Everything To Screen
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Move(): Lets you move the Windows position on the desktop, only usefull if in windowed mode.
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::Move(long XPos, long YPos){
	RECT ClientRect;

	GetClientRect(m_hWnd, &ClientRect);
	MoveWindow(m_hWnd, XPos, YPos, ClientRect.right, ClientRect.bottom, TRUE);

	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Resize(): Lets you Resize the Windows on the desktop, only usefull if in windowed mode.
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::Resize(long Width, long Height){
	RECT WndRect, ClientRect;
	long WndWidth, WndHeight;

	GetWindowRect(m_hWnd, &WndRect);
	GetClientRect(m_hWnd, &ClientRect);

	WndWidth  = (WndRect.right  - (ClientRect.right  - Width))  - WndRect.left;
	WndHeight = (WndRect.bottom - (ClientRect.bottom - Height)) - WndRect.top;

	MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);

	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// ShowMouse(): To show or not to show mosue pointer on the screen.
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::ShowMouse(BOOL Show){
	ShowCursor(Show);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// MenuFrame(): Select device Settings.
// ----------------------------------------------------------------------------------------------------
BOOL cApplication::MenuFrame(){
	Font			MenuFont;

	// Create a large font
	MenuFont.Create(&m_Graphics, "Times New Roman", 72);

	// Acquire input
	m_Mouse.Acquire(TRUE);

	// Get input
	m_Mouse.Read();
	m_Keyboard.Read();

	// Exit game
	if(m_Keyboard.GetKeyState(KEY_BACKSPACE) == TRUE || m_Keyboard.GetKeyState(KEY_ESC) == TRUE)
		return FALSE;

	// Exit Menu
	if(m_Mouse.GetButtonState(MOUSE_LBUTTON) == TRUE){
		if((float)m_Mouse.GetXPos() > 150 && (float)m_Mouse.GetXPos() < 450 && (float)m_Mouse.GetYPos() > 390 && (float)m_Mouse.GetYPos() < 420){
			m_Keyboard.SetKeyState(KEY_ESC, FALSE);
			return FALSE;
		}

		// Texture
		if((float)m_Mouse.GetXPos() > 50 && (float)m_Mouse.GetXPos() < 180 && (float)m_Mouse.GetYPos() > 140 && (float)m_Mouse.GetYPos() < 155)
			m_tex = 32;
		if((float)m_Mouse.GetXPos() > 50 && (float)m_Mouse.GetXPos() < 180 && (float)m_Mouse.GetYPos() > 160 && (float)m_Mouse.GetYPos() < 175)
			m_tex = 16;

		// Resolution
		if((float)m_Mouse.GetXPos() > 260 && (float)m_Mouse.GetXPos() < 350 && (float)m_Mouse.GetYPos() > 140 && (float)m_Mouse.GetYPos() < 155){
			m_res_w = 800;
			m_res_h = 600;
		}

		if((float)m_Mouse.GetXPos() > 260 && (float)m_Mouse.GetXPos() < 350 && (float)m_Mouse.GetYPos() > 160 && (float)m_Mouse.GetYPos() < 175){
			m_res_w = 1024;
			m_res_h = 768;
		}

		if((float)m_Mouse.GetXPos() > 260 && (float)m_Mouse.GetXPos() < 350 && (float)m_Mouse.GetYPos() > 180 && (float)m_Mouse.GetYPos() < 195){
			m_res_w = 1152;
			m_res_h = 864;
		}

		if((float)m_Mouse.GetXPos() > 260 && (float)m_Mouse.GetXPos() < 350 && (float)m_Mouse.GetYPos() > 200 && (float)m_Mouse.GetYPos() < 215){
			m_res_w = 1400;
			m_res_h = 1050;
		}

		if((float)m_Mouse.GetXPos() > 260 && (float)m_Mouse.GetXPos() < 350 && (float)m_Mouse.GetYPos() > 260 && (float)m_Mouse.GetYPos() < 275){
			m_res_w = 1024;
			m_res_h = 600;
		}

		if((float)m_Mouse.GetXPos() > 260 && (float)m_Mouse.GetXPos() < 350 && (float)m_Mouse.GetYPos() > 280 && (float)m_Mouse.GetYPos() < 295){
			m_res_w = 1280;
			m_res_h = 768;
		}

		if((float)m_Mouse.GetXPos() > 260 && (float)m_Mouse.GetXPos() < 350 && (float)m_Mouse.GetYPos() > 300 && (float)m_Mouse.GetYPos() < 315){
			m_res_w = 1680;
			m_res_h = 1050;
		}
	}

	// Render the frame's graphics
	m_Graphics.Clear();
	if(m_Graphics.BeginScene() == TRUE) {

		// Render backdrop (turning off Z-Buffering first)
		m_Graphics.EnableZBuffer(FALSE);

		// Draw the game's title
		MenuFont.Print("BattleOne",16,16,640,0,0xFFFFFFFF,DT_CENTER);

		// Draw Graphics related options
		m_Font.Print("Graphics Options", 0,100,640,0, 0xFFFFFFFF,DT_CENTER);
		m_Font.Print("Texture", 50,120,0,0, 0xFFFFFFFF);
		if(m_tex == 32)
			m_Font.Print("High   - 32bit", 60,140,0,0, 0xFFFF0000);
		else
			m_Font.Print("High   - 32bit", 60,140,0,0, 0xFFFFFFFF);

		if(m_tex == 16)
			m_Font.Print("Medium - 16bit", 60,160,0,0, 0xFFFF0000);
		else
			m_Font.Print("Medium - 16bit", 60,160,0,0, 0xFFFFFFFF);
			

		m_Font.Print("Resolution (Normal)", 250,120,0,0, 0xFFFFFFFF);
		if(m_res_w == 800 && m_res_h == 600)
			m_Font.Print("800 x 600", 260,140,0,0, 0xFFFF0000);
		else
			m_Font.Print("800 x 600", 260,140,0,0, 0xFFFFFFFF);

		if(m_res_w == 1024 && m_res_h == 768)
			m_Font.Print("1024 x 768", 260,160,0,0, 0xFFFF0000);
		else
			m_Font.Print("1024 x 768", 260,160,0,0, 0xFFFFFFFF);

		if(m_res_w == 1152 && m_res_h == 864)
			m_Font.Print("1152 x 864", 260,180,0,0, 0xFFFF0000);
		else
			m_Font.Print("1152 x 864", 260,180,0,0, 0xFFFFFFFF);

		if(m_res_w == 1400 && m_res_h == 1050)
			m_Font.Print("1400 x 1050", 260,200,0,0, 0xFFFF0000);
		else
			m_Font.Print("1400 x 1050", 260,200,0,0, 0xFFFFFFFF);


		m_Font.Print("Resolution (Wide Screen)", 250,240,0,0, 0xFFFFFFFF);
		if(m_res_w == 1024 && m_res_h == 600)
			m_Font.Print("1024 x 600", 260,260,0,0, 0xFFFF0000);
		else
			m_Font.Print("1024 x 600", 260,260,0,0, 0xFFFFFFFF);

		if(m_res_w == 1280 && m_res_h == 768)
			m_Font.Print("1280 x 768", 260,280,0,0, 0xFFFF0000);
		else
			m_Font.Print("1280 x 768", 260,280,0,0, 0xFFFFFFFF);

		if(m_res_w == 1680 && m_res_h == 1050)
			m_Font.Print("1680 x 1050", 260,300,0,0, 0xFFFF0000);
		else
			m_Font.Print("1680 x 1050", 260,300,0,0, 0xFFFFFFFF);

		m_Font.Print("Exit Menu (press ESC to Quit Game)"    , 0,400,640,0,0xFFFFFFFF,DT_CENTER);

		//// Display statistics, FONT
  //      m_Font.Print(Stats,0,0,400,100);

		m_Graphics.EndScene();
	}
	m_Graphics.Display();
	return TRUE;
}


// nextEmpty returns the next empty mobile
bool cApplication::nextEmpty(VolumeThing* mobile[], int nMobiles, int& next){
    // skip occupied mobiles
    while (next < nMobiles && mobile[next])
        next++;
    return next < MAX_MOBILES;
}