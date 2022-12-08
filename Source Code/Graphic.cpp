///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Graphic.cpp
//	Start Date: 02/09/2005
//	End Date:   
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
#include ".\WinMain.h"


// ----------------------------------------------------------------------------------------------------
// Graphic(): Graphic Constructor
// ----------------------------------------------------------------------------------------------------
Graphic::Graphic(){
	g_hWnd = NULL;

	g_D3D = NULL;
	g_D3DDevice = NULL;
	g_Sprite = NULL;
	g_AmbientRed = g_AmbientGreen = g_AmbientBlue = 255;

	g_Width  = 0;
	g_Height = 0;
	g_BPP    = 0;

	g_Windowed = TRUE;
	g_ZBuffer  = FALSE;
	g_HAL      = FALSE;
}


// ----------------------------------------------------------------------------------------------------
// Shutdown(): Release the COM objects (Related to Graphic only)
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::Shutdown(){
	ReleaseCOM(g_Sprite);
	ReleaseCOM(g_D3DDevice);
	ReleaseCOM(g_D3D);
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Init(): Create LPDIRECT3D9
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::Init(){
	Shutdown();
	if((g_D3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::Init()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetMode(): Setup all COM objects related to this class
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::SetMode(HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width, long Height, char BPP){
	D3DPRESENT_PARAMETERS d3dpp;
	D3DFORMAT             Format, AltFormat;
	RECT                  WndRect, ClientRect;
	long                  WndWidth, WndHeight;
	float                 Aspect;

	// Error checking
	if((g_hWnd = hWnd) == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	if(g_D3D == NULL){
		MessageBox(GethWnd(), "Error 2", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Get the current display format
	if(FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &g_d3ddm))){
		MessageBox(GethWnd(), "Error 3", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Configure width
	if(!Width){
		// Default to screen width if fullscreen
		if(Windowed == FALSE){
			g_Width = g_d3ddm.Width;
		}else{
			// Otherwise grab from client size
			GetClientRect(g_hWnd, &ClientRect);
			g_Width = ClientRect.right;
		}
	}else{
		g_Width = Width;
	}

	// Configure height
	if(!Height){
		// Default to screen height if fullscreen
		if(Windowed == FALSE){
			g_Height = g_d3ddm.Height;
		}else{
			// Otherwise grab from client size
			GetClientRect(g_hWnd, &ClientRect);
			g_Height = ClientRect.bottom;
		}
	}else{
		g_Height = Height;
	}

	// Configure BPP
	if(!(g_BPP = BPP) || Windowed == TRUE)
		if(!(g_BPP = GetFormatBPP(g_d3ddm.Format))){
			MessageBox(GethWnd(), "Error 4", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}

	// Resize client window if using windowed mode
	if(Windowed == TRUE){
		GetWindowRect(g_hWnd, &WndRect);
		GetClientRect(g_hWnd, &ClientRect);
		WndWidth  = (WndRect.right  - (ClientRect.right  - g_Width))  - WndRect.left;
		WndHeight = (WndRect.bottom - (ClientRect.bottom - g_Height)) - WndRect.top;
		MoveWindow(g_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);
	}

	// Clear presentation structure
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
	  
	// Default to no hardware acceleration detected
	g_HAL = FALSE;

	// Setup Windowed or fullscreen usage
	if((g_Windowed = Windowed) == TRUE){
		d3dpp.Windowed         = TRUE;
		d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = g_d3ddm.Format;

		// See if card supports HAL
		if(CheckFormat(g_d3ddm.Format, TRUE, TRUE) == TRUE)
			g_HAL = TRUE;
		else
		// Return error if not emulated
		if(CheckFormat(g_d3ddm.Format, TRUE, FALSE) == FALSE){
			MessageBox(GethWnd(), "Error 5", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}
	}else{
		d3dpp.Windowed   = FALSE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	  
		d3dpp.BackBufferWidth  = g_Width;
		d3dpp.BackBufferHeight = g_Height;
		d3dpp.FullScreen_RefreshRateInHz      = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // or D3DPRESENT_INTERVAL_DEFAULT or D3DPRESENT_INTERVAL_IMMEDIATE

		// Figure display format to use
		if(g_BPP == 32){
			Format    = D3DFMT_X8R8G8B8;
			AltFormat = D3DFMT_X8R8G8B8;
		}
		if(g_BPP == 24){
			Format    = D3DFMT_R8G8B8;
			AltFormat = D3DFMT_R8G8B8;
		}
		if(g_BPP == 16){
			Format    = D3DFMT_R5G6B5;
			AltFormat = D3DFMT_X1R5G5B5;
		}
		if(g_BPP == 8){
			Format    = D3DFMT_P8;
			AltFormat = D3DFMT_P8;
		}

		// Check for HAL device
		if(CheckFormat(Format, FALSE, TRUE) == TRUE)
			g_HAL = TRUE;
		else{
            // Check for HAL device in alternate format
			if(CheckFormat(AltFormat, FALSE, TRUE) == TRUE){
				g_HAL = TRUE;
				Format = AltFormat;
			}else{
				// Check for Emulation device
				if(CheckFormat(Format, FALSE, FALSE) == FALSE){
					// Check for Emulation device in alternate format
					if(CheckFormat(AltFormat, FALSE, FALSE) == FALSE){
						MessageBox(GethWnd(), "Error 6", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
						return FALSE;
					}
					else
						Format = AltFormat;
				}
			}
		}
		d3dpp.BackBufferFormat = Format;
	}

	// Setup Zbuffer format - 16 bit
	if((g_ZBuffer = UseZBuffer) == TRUE){
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}else
		d3dpp.EnableAutoDepthStencil = FALSE;

	// Create the Direct3D Device object
	if(FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, (g_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice))){
        // Try to create Direct3D without ZBuffer support 
		// if selected and first call failed.
		if(g_ZBuffer == TRUE){
            g_ZBuffer = FALSE;
            d3dpp.EnableAutoDepthStencil = FALSE;

			if(FAILED(g_D3D->CreateDevice(D3DADAPTER_DEFAULT, (g_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice))){
				MessageBox(GethWnd(), "Error 7", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}else{
			MessageBox(GethWnd(), "Error 8", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
            return FALSE;
		}
	}

    // Set default rendering states
	EnableLighting(FALSE);
	EnableZBuffer(g_ZBuffer);
	EnableAlphaBlending(FALSE);
	EnableAlphaTesting(FALSE);

	// Enable texture rendering stages and filter types
	g_D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_D3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	g_D3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

	g_D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	// Set default ambient color to white
	SetAmbientLight(255,255,255);

	// Calculate the aspect ratio based on window size
	Aspect = (float)g_Height / (float)g_Width;
	SetPerspective(D3DX_PI/4, Aspect, 1.0f, 10000.0f);

	// Create a sprite interface
	if(FAILED(D3DXCreateSprite(g_D3DDevice, &g_Sprite))){
		MessageBox(GethWnd(), "Error 9", "Graphic::SetMode()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetPerspective(): Setup a left-handed perspective projection matrix based on a field of view
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::SetPerspective(float FOV, float Aspect, float Near, float Far){
	D3DXMATRIX matProjection;

	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::SetPerspective()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, Aspect, Near, Far);
	if(FAILED(g_D3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection))){
		MessageBox(GethWnd(), "Error 2", "Graphic::SetPerspective()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetCamera(): Setup camera view
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::SetCamera(Camera *Camera){
	if(g_D3DDevice == NULL || Camera == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::SetCamera()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->SetTransform(D3DTS_VIEW, Camera->GetMatrix()))){
		MessageBox(GethWnd(), "Error 2", "Graphic::SetCamera()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetLight(): Setup light
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::SetLight(long Num, Light *Light){
	if(Light == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::SetLight()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 2", "Graphic::SetLight()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->SetLight(Num, Light->GetLight()))){
		MessageBox(GethWnd(), "Error 3", "Graphic::SetLight()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetAmbientLight(): Setup the colour of the light
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::SetAmbientLight(char Red, char Green, char Blue){
	D3DCOLOR Color;

	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::SetAmbientLight()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	Color = D3DCOLOR_XRGB((g_AmbientRed = Red), (g_AmbientGreen = Green), (g_AmbientBlue = Blue));
	if(FAILED(g_D3DDevice->SetRenderState(D3DRS_AMBIENT, Color))){
		MessageBox(GethWnd(), "Error 2", "Graphic::SetAmbientLight()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// EnableLight(): Enables or disables a set of lighting parameters within a device
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::EnableLight(long Num, BOOL Enable){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::EnableLight()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->LightEnable(Num, Enable))){
		MessageBox(GethWnd(), "Error 2", "Graphic::EnableLight()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// EnableLighting(): Sets a single device render-state parameter (Lighting)
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::EnableLighting(BOOL Enable){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::EnableLighting()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->SetRenderState(D3DRS_LIGHTING, Enable))){
		MessageBox(GethWnd(), "Error 2", "Graphic::EnableLighting()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// EnableZBuffer(): Sets a single device render-state parameter (ZBuffer)
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::EnableZBuffer(BOOL Enable){
	if(g_D3DDevice == NULL || g_ZBuffer == FALSE){
		MessageBox(GethWnd(), "Error 1", "Graphic::EnableZBuffer()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->SetRenderState(D3DRS_ZENABLE, (Enable == TRUE) ? D3DZB_TRUE : D3DZB_FALSE))){
		MessageBox(GethWnd(), "Error 2", "Graphic::EnableZBuffer()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// EnableAlphaBlending(): Sets a single device render-state parameter (Alpha Blending)
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::EnableAlphaBlending(BOOL Enable, DWORD Src, DWORD Dest){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::EnableAlphaBlending()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Enable or disable
	if(FAILED(g_D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, Enable))){
		MessageBox(GethWnd(), "Error 2", "Graphic::EnableAlphaBlending()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Set blend type
	if(Enable == TRUE){
		g_D3DDevice->SetRenderState(D3DRS_SRCBLEND,  Src);
		g_D3DDevice->SetRenderState(D3DRS_DESTBLEND, Dest);
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// EnableAlphaTesting(): Sets a single device render-state parameter (Test Alpha Blending)
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::EnableAlphaTesting(BOOL Enable){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::EnableAlphaTesting()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, Enable))){
		MessageBox(GethWnd(), "Error 2", "Graphic::EnableAlphaTesting()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Set test type
	if(Enable == TRUE) {
		g_D3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
		g_D3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// GetFormatBPP(): Defines the various types of surface formats
// ----------------------------------------------------------------------------------------------------
char Graphic::GetFormatBPP(D3DFORMAT Format){
	switch(Format){
		// 32 bit modes
		case D3DFMT_A8R8G8B8:
		case D3DFMT_X8R8G8B8:
			return 32;
		break;

		// 24 bit modes
		case D3DFMT_R8G8B8:
			return 24;
		break;

		// 16 bit modes
		case D3DFMT_R5G6B5:
		case D3DFMT_X1R5G5B5:
		case D3DFMT_A1R5G5B5:
		case D3DFMT_A4R4G4B4:
			return 16;
		break;

		// 8 bit modes
		case D3DFMT_A8P8:
		case D3DFMT_P8:
			return 8;
		break;

		default:
			return 0;
	}
}


// ----------------------------------------------------------------------------------------------------
// CheckFormat(): Check whether a hardware accelerated device type can be used on this adapter
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL){
	if(FAILED(g_D3D->CheckDeviceType(D3DADAPTER_DEFAULT, (HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, Format, Format, Windowed))){
		MessageBox(GethWnd(), "Error 1", "Graphic::CheckFormat()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Clear(): The clear target is both the surface and the z-buffer
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::Clear(long Color, float ZBuffer){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::Clear()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Only clear screen if no zbuffer
	if(g_ZBuffer == FALSE)
		return ClearDisplay(Color);

	// Clear display and zbuffer
	if(FAILED(g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, Color, ZBuffer, 0))){
		MessageBox(GethWnd(), "Error 2", "Graphic::Clear()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE; 
}


// ----------------------------------------------------------------------------------------------------
// ClearDisplay(): The clear target is the surface
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::ClearDisplay(long Color){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::ClearDisplay()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, Color, 1.0f, 0))){
		MessageBox(GethWnd(), "Error 2", "Graphic::ClearDisplay()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE; 
}


// ----------------------------------------------------------------------------------------------------
// ClearZBuffer(): The clear target is the z-buffer
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::ClearZBuffer(float ZBuffer){
	if(g_D3DDevice == NULL || g_ZBuffer == FALSE){
		MessageBox(GethWnd(), "Error 1", "Graphic::ClearZBuffer()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, ZBuffer, 0))){
		MessageBox(GethWnd(), "Error 2", "Graphic::ClearZBuffer()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// BeginScene(): Begins a scene
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::BeginScene(){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::BeginScene()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->BeginScene())){
		MessageBox(GethWnd(), "Error 2", "Graphic::BeginScene()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// EndScene(): End a scene
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::EndScene(){
	short i;

	// Error checking
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::EndScene()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Release all textures
	for(i=0;i<8;i++)
		g_D3DDevice->SetTexture(i, NULL);

	// End the scene
	if(FAILED(g_D3DDevice->EndScene())){
		MessageBox(GethWnd(), "Error 2", "Graphic::EndScene()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Display(): Presents the contents
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::Display(){
	if(g_D3DDevice == NULL){
		MessageBox(GethWnd(), "Error 1", "Graphic::Display()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(FAILED(g_D3DDevice->Present(NULL, NULL, NULL, NULL))){
		MessageBox(GethWnd(), "Error 2", "Graphic::Display()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// SetBPP(): Set BPP
// ----------------------------------------------------------------------------------------------------
BOOL Graphic::SetBPP(char format){
	g_BPP = format;

	if(g_BPP == 32)
		g_d3ddm.Format = D3DFMT_X8R8G8B8;
	else if(g_BPP == 16)
		g_d3ddm.Format = D3DFMT_R5G6B5;
	else if(g_BPP == 8)
		g_d3ddm.Format = D3DFMT_P8;
	return TRUE;
}