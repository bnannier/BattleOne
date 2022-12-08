///////////////////////////////////////////////////////////////////////////////////////
//	File Name:  Input.cpp
//	Start Date: 22/10/2005
//	End Date:   15/10/2005
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
// Shutdown(): Release the COM objects (i_DI)
// ----------------------------------------------------------------------------------------------------
BOOL Input::Shutdown(){
	// Release the COM objects
	ReleaseCOM(i_DI);

	// Clear parent Window handle
	i_hWnd = NULL;

	// Return a success
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Init(): Create a IDirectInput8
// ----------------------------------------------------------------------------------------------------
BOOL Input::Init(HWND hWnd, HINSTANCE hInst){
	// Free a prior Init
	Shutdown();

	// Record parent Window handle
	i_hWnd = hWnd;

	// Create a DirectInput interface
	if(FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&i_DI, NULL))){
		MessageBox(GethWnd(), "Error 1", "Input::Init()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Return a success
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// InputDevice(): InputDevice Constructor
// ----------------------------------------------------------------------------------------------------
InputDevice::InputDevice(){
	i_Input			= NULL;						// Clear parent Input object pointer
	i_Type			= NONE;						// Setup device to none
	i_Windowed		= TRUE;						// Set windowed usage to TRUE
	i_DIDevice		= NULL;						// Clear the DirectInput interface pointer
	i_MouseState    = (DIMOUSESTATE*)&i_State;	// Point the mouse structure to the state buffer
	m_JoystickState = (DIJOYSTATE*)&i_State;	// Point the joystick structures to the state buffer
	Clear();									// Clear the device variables
}


// ----------------------------------------------------------------------------------------------------
// Create(): Creates and initializes an instance of a device based on a given globally unique identifier (GUID)
// ----------------------------------------------------------------------------------------------------
BOOL InputDevice::Create(Input *Input, short Type, BOOL Windowed){
	DIDATAFORMAT *DataFormat;
	DIPROPRANGE   DIprg;
	DIPROPDWORD   DIpdw;

	GUID     guidEffect;

	// Free a prior device
	Free();

	// Check for a valid parent Input class
	if((i_Input = Input) == NULL){
		MessageBox(Input->GethWnd(), "Error 1", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Create the device and remember device data format
	switch(Type){
		case KEYBOARD:
			if(FAILED(i_Input->GetDirectInputCOM()->CreateDevice(GUID_SysKeyboard, &i_DIDevice, NULL))){
				MessageBox(Input->GethWnd(), "Error 2", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
			DataFormat = (DIDATAFORMAT*)&c_dfDIKeyboard;
		break;
		case MOUSE:
			if(FAILED(i_Input->GetDirectInputCOM()->CreateDevice(GUID_SysMouse, &i_DIDevice, NULL))){
				MessageBox(Input->GethWnd(), "Error 3", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
			DataFormat = (DIDATAFORMAT*)&c_dfDIMouse;
		break;
		case JOYSTICK:
			if(FAILED(i_Input->GetDirectInputCOM()->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticks, this, DIEDFL_FORCEFEEDBACK | DIEDFL_ATTACHEDONLY))){
				MessageBox(Input->GethWnd(), "Error 4", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
			if(i_DIDevice == NULL){
				MessageBox(Input->GethWnd(), "You DON'T have a Force Feedback capable Joystick!", "Force Feedback", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
			DataFormat = (DIDATAFORMAT*)&c_dfDIJoystick;
		break;
		default: return FALSE;
	}

	// Set the windowed usage
	i_Windowed = Windowed;

	if(Type == KEYBOARD || Type == MOUSE){
		// Set the data format of keyboard
		if(FAILED(i_DIDevice->SetDataFormat(DataFormat))){
            MessageBox(Input->GethWnd(), "Error 6", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}

		// Set the cooperative level - Foreground & Nonexclusive
		if(FAILED(i_DIDevice->SetCooperativeLevel(i_Input->GethWnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))){
			MessageBox(Input->GethWnd(), "Error 7", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}
	}
	// Set the special properties if it's a joystick
	else if(Type == JOYSTICK){
		if(FAILED(i_DIDevice->SetCooperativeLevel(i_Input->GethWnd(), DISCL_EXCLUSIVE | DISCL_FOREGROUND))){
			MessageBox(Input->GethWnd(), "Error 8", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}

		if(FAILED(i_DIDevice->SetDataFormat(DataFormat))){
			MessageBox(Input->GethWnd(), "Error 9", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}

		// Set the special properties of the joystick - range
		DIprg.diph.dwSize       = sizeof(DIPROPRANGE);
		DIprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		DIprg.diph.dwHow        = DIPH_BYOFFSET;
		DIprg.lMin              = -1024;
		DIprg.lMax              = +1024;

		// Set X range
		DIprg.diph.dwObj = DIJOFS_X;
		if(FAILED(i_DIDevice->SetProperty(DIPROP_RANGE, &DIprg.diph))){
			MessageBox(Input->GethWnd(), "Error 10", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}

		// Set Y range
		DIprg.diph.dwObj = DIJOFS_Y;
		if(FAILED(i_DIDevice->SetProperty(DIPROP_RANGE, &DIprg.diph))){
			MessageBox(Input->GethWnd(), "Error 11", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}


		// Set the special properties of the joystick - deadzone 12%
		DIpdw.diph.dwSize       = sizeof(DIPROPDWORD);
		DIpdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
		DIpdw.diph.dwObj        = 0;
		DIpdw.diph.dwHow        = DIPH_DEVICE;
		DIpdw.dwData            = DIPROPAUTOCENTER_OFF;
	    

		if(FAILED(i_DIDevice->SetProperty(DIPROP_AUTOCENTER, &DIpdw.diph))){
			MessageBox(Input->GethWnd(), "Error 12", "InputDevice::Create()", MB_OK | MB_ICONSTOP);		
			return FALSE;
		}


		if(FAILED(i_DIDevice->EnumEffects((LPDIENUMEFFECTSCALLBACK) DIEnumEffectsProc, &guidEffect, DIEFT_PERIODIC))){
			MessageBox(Input->GethWnd(), "Error 13", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}


		// Arrays can be initialized when they, along with the structures, are declared.
		DWORD      dwAxes[2] = {DIJOFS_X, DIJOFS_Y};
		LONG       lDirection[2] = {0, 0};

		DIPERIODIC diPeriodic;      // type-specific parameters
		DIENVELOPE diEnvelope;      // envelope
		DIEFFECT   diEffect;        // general parameters
		 

		// Next, initialize the type-specific parameters. The values in this example create a
		// full-force periodic effect with a period of one-twentieth of a second.
		diPeriodic.dwMagnitude = DI_FFNOMINALMAX; 
		diPeriodic.lOffset = 0; 
		diPeriodic.dwPhase = 0; 
		diPeriodic.dwPeriod = (DWORD)(0.05 * DI_SECONDS); 
		 

		// To produce an effect of the chain-saw motor trying to start, briefly coughing into life, and then
		// slowly dying, you set an envelope with an attack time of one-half second and a fade time of one second.
		diEnvelope.dwSize = sizeof(DIENVELOPE);
		diEnvelope.dwAttackLevel = 0; 
		diEnvelope.dwAttackTime = (DWORD)(0.5 * DI_SECONDS); 
		diEnvelope.dwFadeLevel = 0; 
		diEnvelope.dwFadeTime = (DWORD)(1.0 * DI_SECONDS); 
		 

		// Next, set up the basic effect parameters. These include flags to determine how the directions and
		// device objects (buttons and axes) are identified, the sample period and gain for the effect, and
		// pointers to the other data that you have just prepared. You also associate the effect with the
		// joystick's fire button so that it is automatically played whenever that button is pressed.
		diEffect.dwSize = sizeof(DIEFFECT); 
		diEffect.dwFlags = DIEFF_POLAR | DIEFF_OBJECTOFFSETS; 
		diEffect.dwDuration = INFINITE;

		diEffect.dwSamplePeriod = 0;               // = default 
		diEffect.dwGain = DI_FFNOMINALMAX;         // no scaling
		diEffect.dwTriggerButton = DIJOFS_BUTTON0;
		diEffect.dwTriggerRepeatInterval = 0;      
		diEffect.cAxes = 2; 
		diEffect.rgdwAxes = dwAxes; 
		diEffect.rglDirection = &lDirection[0]; 
		diEffect.lpEnvelope = &diEnvelope; 
		diEffect.cbTypeSpecificParams = sizeof(diPeriodic);
		diEffect.lpvTypeSpecificParams = &diPeriodic;  
		 

		// Finally, you can create the effect.
		if(FAILED(i_DIDevice->CreateEffect(
							guidEffect,     // GUID from enumeration
							&diEffect,      // where the data is
							&g_lpdiEffect,  // where to put interface pointer
							NULL))){         // no aggregation
			MessageBox(Input->GethWnd(), "Error 14", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}
	}

	// Acquire the device for use
	if(FAILED(i_DIDevice->Acquire())){
		MessageBox(Input->GethWnd(), "Error 15", "InputDevice::Create()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Set the device type
	i_Type = Type;

	// Clear the device information
	Clear();

	// Return a success
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Free(): Releases access to the device
// ----------------------------------------------------------------------------------------------------
BOOL InputDevice::Free(){
	// Unacquire and release the object
	if(i_DIDevice != NULL) {
		i_DIDevice->Unacquire();
		ReleaseCOM(i_DIDevice);
	}

	// Set to no device installed
	i_Type = NONE;

	// Clear the data  
	Clear();

	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Clear(): Clear all vales in this class
// ----------------------------------------------------------------------------------------------------
BOOL InputDevice::Clear(){
	short i;

	ZeroMemory(&i_State, 256);
	  
	for(i=0;i<256;i++)
		i_Locks[i]  = FALSE;

	i_XPos = 0;
	i_YPos = 0;
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Read(): Retrieves immediate data from the device
// ----------------------------------------------------------------------------------------------------
BOOL InputDevice::Read(){
	HRESULT hr;
	long BufferSizes[3] = { 256, sizeof(DIMOUSESTATE), sizeof(DIJOYSTATE) };
	short i;

	// Make sure to have a valid IDirectInputDevice8 object
	if(i_DIDevice == NULL){
		MessageBox(i_Input->GethWnd(), "Error 1", "InputDevice::Read()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Make sure device type if in range
	if(i_Type < 1 || i_Type > 3){
		MessageBox(i_Input->GethWnd(), "Error 2", "InputDevice::Read()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Loop polling and reading until succeeded or unknown error
	// Also take care of lost-focus problems
	while(1){
		// Poll
		i_DIDevice->Poll();

		// Read in state
		if(SUCCEEDED(hr = i_DIDevice->GetDeviceState(BufferSizes[i_Type-1], (LPVOID)&i_State)))
			break;

		// Return on an unknown error  
		if(hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED){
			MessageBox(i_Input->GethWnd(), "Error 3", "InputDevice::Read()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}

		// Reacquire and try again
		if(FAILED(i_DIDevice->Acquire())){
			MessageBox(i_Input->GethWnd(), "Error 4", "InputDevice::Read()", MB_OK | MB_ICONSTOP);
			return FALSE;
		}
	}

	// Since only the mouse coordinates are relative, you'll
	// have to deal with them now
	if(i_Type == MOUSE) {
		// If windowed usage, ask windows for coordinates
		if(i_Windowed == TRUE) {
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(i_Input->GethWnd(), &pt);
			i_XPos = pt.x;
			i_YPos = pt.y;
		}else{
			i_XPos += i_MouseState->lX;
			i_YPos += i_MouseState->lY;
		}
	}

	// Released keys and button need to be unlocked
	switch(i_Type) {
		case KEYBOARD:
			for(i=0;i<256;i++) {
				if(!(i_State[i] & 0x80))
				i_Locks[i] = FALSE;
			}
		break;
		case MOUSE:
			for(i=0;i<4;i++) {
				if(!(i_MouseState->rgbButtons[i]))
				i_Locks[i] = FALSE;
			}
		break;
		case JOYSTICK:
			for(i=0;i<32;i++) {
				if(!(m_JoystickState->rgbButtons[i]))
				i_Locks[i] = FALSE;
			}
       break;
	}
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// Acquire(): Obtains access to the input device
// ----------------------------------------------------------------------------------------------------
BOOL InputDevice::Acquire(BOOL Active){
	if(i_DIDevice == NULL){
		MessageBox(i_Input->GethWnd(), "Error 1", "InputDevice::Read()", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	if(Active == TRUE)
		i_DIDevice->Acquire();
	else
		i_DIDevice->Unacquire();
	return TRUE;
}


// ----------------------------------------------------------------------------------------------------
// GetKeyState(): Check if key is pressed
// ----------------------------------------------------------------------------------------------------
BOOL InputDevice::GetKeyState(char Num){
	// Check if key/button is pressed
	if(i_State[Num] & 0x80 && i_Locks[Num] == FALSE)
		return TRUE;
	return FALSE;
}


// ----------------------------------------------------------------------------------------------------
// GetXDelta(): Position of mouse on the x-axis
// ----------------------------------------------------------------------------------------------------
long InputDevice::GetXDelta(){
  switch(i_Type) {
    case MOUSE:
		return i_MouseState->lX;
	case JOYSTICK:
        return m_JoystickState->lX - i_XPos;
    default: return 0;
  }
}


// ----------------------------------------------------------------------------------------------------
// GetYDelta(): Position of mouse on the y-axis
// ----------------------------------------------------------------------------------------------------
long InputDevice::GetYDelta(){
  switch(i_Type) {
    case MOUSE:
		return i_MouseState->lY;
	case JOYSTICK:
		return m_JoystickState->lY - i_YPos;
    default: return 0;
  }
}


// ----------------------------------------------------------------------------------------------------
// GetButtonState(): Button pressed on the mouse
// ----------------------------------------------------------------------------------------------------
BOOL InputDevice::GetButtonState(char Num){
	char State = 0;

	if(i_Type == MOUSE)
		State = i_MouseState->rgbButtons[Num];
	if(i_Type == JOYSTICK)
		State = m_JoystickState->rgbButtons[Num];

	// Check if key/button is pressed
	if(State & 0x80 && i_Locks[Num] == FALSE)
		return TRUE;
	return FALSE;
}


// ----------------------------------------------------------------------------------------------------
// GetXPos(): Get possition on the x-axis
// ----------------------------------------------------------------------------------------------------
long InputDevice::GetXPos(){
	// Update coordinates if a joystick
	if(i_Type == JOYSTICK)
		i_XPos = m_JoystickState->lX;
	return i_XPos;
}


// ----------------------------------------------------------------------------------------------------
// GetYPos(): Get position on the y-axis
// ----------------------------------------------------------------------------------------------------
long InputDevice::GetYPos(){
	// Update coordinates if a joystick
	if(i_Type == JOYSTICK)
		i_YPos = m_JoystickState->lY;
	return i_YPos;
}


// ----------------------------------------------------------------------------------------------------
// EnumJoysticks(): Create joystick interface
// ----------------------------------------------------------------------------------------------------
BOOL FAR PASCAL InputDevice::EnumJoysticks(LPCDIDEVICEINSTANCE pdInst, LPVOID pvRef){
	InputDevice *c_input;

	// Stop enumeration if no parent InputDevice pointer
	if((c_input = (InputDevice*)pvRef) == NULL)
		return DIENUM_STOP;

	// Try to create a joystick interface
	if(FAILED(c_input->i_Input->GetDirectInputCOM()->CreateDevice(pdInst->guidInstance, &c_input->i_DIDevice, NULL))){
		MessageBox(c_input->i_Input->GethWnd(), "CreateDevice NOT created", "InputDevice::EnumJoysticks()", MB_OK | MB_ICONSTOP);
		return DIENUM_CONTINUE;
	}

	// All done - stop enumeration
	return DIENUM_STOP;
}


// ----------------------------------------------------------------------------------------------------
// DIEnumEffectsProc(): Create joystick interface
// ----------------------------------------------------------------------------------------------------
BOOL FAR PASCAL InputDevice::DIEnumEffectsProc(LPCDIEFFECTINFO pei, LPVOID pv){
    *((GUID *)pv) = pei->guid;

    return DIENUM_STOP;  // one is enough
}