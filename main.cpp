#include <windows.h>		// Header File For Windows
#include <iostream>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <math.h>

int	mouse_x = 0, mouse_y = 0;
int screenWidth = 1200, screenHeight = 600;
bool LeftPressed = false;
bool keys[256];
float startingpoint;
bool jumping = false;
bool falling = true;

class Ground {
public:
	float minX;
	float maxX;
	float minY;
	float maxY;

	Ground() {

		minX = -screenWidth;
		maxX = screenWidth;
		minY = -screenHeight;
		maxY = -15;
	}

	void drawGround() {

		glColor3f(0, 0, 0);

		glBegin(GL_QUADS);
			glVertex2f(minX, maxY);
			glVertex2f(maxX, maxY);
			glVertex2f(maxX, minY);
			glVertex2f(minX, minY);
		glEnd();
	}
};

class Platform {
public:
	float platX;
	float platY;
	
	float minX;
	float maxX;
	float minY;
	float maxY;

	Platform(float x, float y) {

		platX = x; //MinorMax X taken by subtracting/adding 0.5 * width.
		platY = y; //MinorMax Y taken by subtracting/adding 0.5 * width.

		minX = platX - 7;
		maxX = platX + 7;
		minY = platY - 1.5;
		maxY = platY + 1.5;
	}

	//draws the platform to the screen
	void drawPlatform() {

		glColor3f(0, 0, 1);

		glBegin(GL_QUADS);
		glVertex2f(platX - 7, platY + 1.5);
		glVertex2f(platX + 7, platY + 1.5);
		glVertex2f(platX + 7, platY - 1.5);
		glVertex2f(platX - 7, platY - 1.5);
		glEnd();
	}

};

class Player {
public:
	float minX;
	float maxX;
	float minY;
	float maxY;
	float jumpheight;

	Player(float pminX, float pmaxX, float pminY, float pmaxY) {

		minX = pminX;
		maxX = pmaxX;
		minY = pminY;
		maxY = pmaxY;

		jumpheight = 17.5;

	}

	void drawPlayer(){

		glColor3f(1, 0, 0);
		glBegin(GL_QUADS);
			glVertex2f(minX, maxY);
			glVertex2f(maxX, maxY);
			glVertex2f(maxX, minY);
			glVertex2f(minX, minY);
		glEnd();
	}
};

const int pnumber = 6;
Platform* platforms[pnumber];
Ground* ground = new Ground();
Player* player = new Player(-55, -50, -15, -10);

//If next position of player is a collision return true.
bool collisionDetection(float xMinA, float xMaxA, float yMinA, float yMaxA, float xMinB, float xMaxB, float yMinB, float yMaxB){

	if (xMinA < xMaxB &&
		xMaxA > xMinB &&
		yMinA < yMaxB &&
		yMaxA > yMinB) {
		
		return true;
	}
	else {
		return false;
	}

}

bool leftCollision() {

	bool collision = false;
	for (int i = 0; i < pnumber; i++) {

		//platforms
		if (collisionDetection(player->minX - 0.25, player->maxX - 0.25, player->minY, player->maxY, platforms[i]->minX, platforms[i]->maxX, platforms[i]->minY, platforms[i]->maxY)) {
			
			collision = true;
			//std::cout << "collision with" << platforms[i] << std::endl;
		}

	}
	return collision;
}

bool rightCollision() {

	bool collision = false;
	for (int i = 0; i < pnumber; i++) {

		//platforms
		if (collisionDetection(player->minX + 0.25, player->maxX + 0.25, player->minY, player->maxY, platforms[i]->minX, platforms[i]->maxX, platforms[i]->minY, platforms[i]->maxY)) {

			collision = true;
			//std::cout << "collision with" << platforms[i] << std::endl;
		}

	}
	return collision;
}

bool upCollision() {

	bool collision = false;
	for (int i = 0; i < pnumber; i++) {

		//platforms
		if (collisionDetection(player->minX, player->maxX, player->minY + 0.25, player->maxY + 0.25, platforms[i]->minX, platforms[i]->maxX, platforms[i]->minY, platforms[i]->maxY)) {

			collision = true;
			//std::cout << "collision with" << platforms[i] << std::endl;
		}

	}
	return collision;
}

bool downCollision() {

	bool collision = false;
	for (int i = 0; i < pnumber; i++) {

		//platforms
		if (collisionDetection(player->minX, player->maxX, player->minY - 0.25, player->maxY - 0.25, platforms[i]->minX, platforms[i]->maxX, platforms[i]->minY, platforms[i]->maxY)){

			collision = true;
			//std::cout << "collision with" << platforms[i] << std::endl;
		}
		//ground
		if (collisionDetection(player->minX, player->maxX, player->minY - 0.25, player->maxY - 0.25, ground->minX, ground->maxX, ground->minY, ground->maxY)) {

			collision = true;
			//std::cout << "collision with" << ground << std::endl;
		}

	}
	return collision;
}

void jump() {

	if (jumping == true) { //if already jumping, 

		if (player->minY >= startingpoint + player->jumpheight || upCollision() == true) { //if there is something in the way or has reached the top of the jump

			jumping = false;
		}
		else {					//move player up
			player->maxY += 0.5f;
			player->minY += 0.5f;

		}
		
	}
	
}

void gravity() {

	if (jumping == false && downCollision() == false) { //if not jumping and not colliding with a platform, pull player down.
		falling = true;
		player->maxY -= 0.5f;
		player->minY -= 0.5f;
	}
	if (downCollision() == true) {
		startingpoint = player->minY;
	}

}

//OPENGL FUNCTION PROTOTYPES
void display();				//draws everything to the screen
void reshape(int width, int height);//called when the window is resized
void init();				//called in winmain when the program starts.
void processKeys();			//called in winmain to process keyboard controls

/*************    START OF OPENGL FUNCTIONS   ****************/
void display()									
{
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	//draw platform, ground and player after translation;
	for (int i = 0; i < pnumber; i++) {
		platforms[i]->drawPlatform();
	}
	ground->drawGround();
	player->drawPlayer();
	glFlush();
}

void reshape(int width, int height)		// Resize the OpenGL window
{
	screenWidth=width; screenHeight = height;           // to ensure the mouse coordinates match 
														// we will use these values to set the coordinate system
	glViewport(0,0,width,height);						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluOrtho2D(width*-0.05, width*0.05, height*-0.05, height*0.05);	// set the coordinate system for the window
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void init()
{
	glClearColor(1,1,1,1);
	
	//Create platforms.
	Platform *platform1 = new Platform(-5, -5);
	Platform *platform2 = new Platform(20, -5);
	Platform* platform3 = new Platform(20, 10);
	Platform* platform4 = new Platform(45, -5);
	Platform* platform5 = new Platform(45, 10);
	Platform* platform6 = new Platform(45, 25);

	//add to platforms;
	platforms[0] = platform1;
	platforms[1] = platform2;
	platforms[2] = platform3;
	platforms[3] = platform4;
	platforms[4] = platform5;
	platforms[5] = platform6;

}

void movement()
{
	if(keys[VK_LEFT]) 
	{
		if(leftCollision() == false) //only allow left movement if next movement doesnt cause collision
		{
			//std::cout << "moving left" << std::endl;
			player->minX -= 0.25f;
			player->maxX -= 0.25f;
		}
	}
	if(keys[VK_RIGHT] )
	{
		
		if(rightCollision() == false) //only allow right movement if next movement doesnt cause collision
		{
			//std::cout << "moving right" << std::endl;
			player->minX += 0.25f;
			player->maxX += 0.25f;
		}
				
	}
	if(keys[VK_UP])
	{
		if (falling != true) { //only allow jumps if not already falling
			jumping = true;
		}
		falling = false;
	}

}

/**************** END OPENGL FUNCTIONS *************************/

//WIN32 functions
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void KillGLWindow();									// releases and destroys the window
bool CreateGLWindow(char* title, int width, int height); //creates the window
int WINAPI WinMain(	HINSTANCE, HINSTANCE, LPSTR, int);  // Win32 main function

//win32 global variabless
HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

/******************* WIN32 FUNCTIONS ***************************/
int WINAPI WinMain(	HINSTANCE	hInstance, HINSTANCE	hPrevInstance,	LPSTR lpCmdLine, int nCmdShow)			
{
	MSG		msg;									// Windows Message Structure
	bool	done=false;								// Bool Variable To Exit Loop

	AllocConsole();
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);

	// Create Our OpenGL Window
	if (!CreateGLWindow("My Game",screenWidth,screenHeight))
	{
		return 0;									// Quit If Window Was Not Created
	}

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=true;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			if(keys[VK_ESCAPE])
				done = true;

			movement();
			jump();
			gravity();
			display();
			SwapBuffers(hDC);				// Swap Buffers (Double Buffering)
		}
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (int)(msg.wParam);						// Exit The Program
}

//WIN32 Processes function - useful for responding to user inputs or other events.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM	lParam)			
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
		break;

		case WM_SIZE:								// Resize The OpenGL Window
		{
			reshape(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
		break;

		case WM_LBUTTONDOWN:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = screenHeight - HIWORD(lParam);
				LeftPressed = true;
			}
		break;

		case WM_LBUTTONUP:
			{
	            LeftPressed = false;
			}
		break;

		case WM_MOUSEMOVE:
			{
	            mouse_x = LOWORD(lParam);          
				mouse_y = screenHeight  - HIWORD(lParam);
			}
		break;
		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = true;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}
		break;
		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = false;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}
		break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

void KillGLWindow()								// Properly Kill The Window
{
	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

bool CreateGLWindow(char* title, int width, int height)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;											// Return FALSE
	}
	
	dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
	dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	
	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								400, 250,							// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	reshape(width, height);					// Set Up Our Perspective GL Screen

	init();
	
	return true;									// Success
}

