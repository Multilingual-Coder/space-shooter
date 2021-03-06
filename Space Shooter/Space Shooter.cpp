//author: @Multilingual-Coder 
//purpose: A tutorial for my youtube channel
//date: 9/28/2020
#include <Windows.h>
#include <stdlib.h>
#include <time.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp);


typedef struct _SPACESHIP {// a structure to organise information about our player
	int x = 0;
	int y = 500;
	int hp = 0;
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, TEXT("SpaceShip.bmp"), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
	HBITMAP mask = (HBITMAP)LoadImage(NULL, TEXT("mask.bmp"), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
}SPACESHIP;

typedef struct _ENEMY {// a structure to organise information about our enemy aliens
	int x;
	int y;
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, TEXT("alien.bmp"), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
	HBITMAP mask = (HBITMAP)LoadImage(NULL, TEXT("alien mask.bmp"), IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
}ENEMY;
typedef struct _BULLET { // a structure to organise information about our bullets
	bool fired = false;
	int x = 0;
	int y = 0;
	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, TEXT("Bullet.bmp"), IMAGE_BITMAP, 4, 4, LR_LOADFROMFILE);
	HBITMAP mask = (HBITMAP)LoadImage(NULL, TEXT("Bullet mask.bmp"), IMAGE_BITMAP, 4, 4, LR_LOADFROMFILE);
}BULLET;

SPACESHIP player; // create objects from our structures
ENEMY alien[10];
BULLET bullet[20];
int bullet_timer = 0;
int bullet_on = 0;

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR pCmdline, int nCmdShow) {
	WNDCLASS wc = { 0 }; // create a window class....
	wc.lpszClassName = TEXT("Main Window");//...and fill it up with properties
	wc.hInstance = hInst;
	wc.lpfnWndProc = WindowProc;
	wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);

	RegisterClass(&wc);

	HWND hWnd = CreateWindow(TEXT("Main Window"), TEXT("Hello GUI"), WS_SYSMENU | WS_VISIBLE, 0,
		0,
		600,
		600, HWND_DESKTOP, NULL, hInst, NULL); //create window from our "Main Window" window class
	if (hWnd == NULL) {
		return 0;
	}
	UINT_PTR timer = SetTimer(hWnd, 1, 1, NULL); // create timer so that we can use WM_TIMER message
	MSG msg = {};
	while (GetMessage(&msg, NULL, NULL, NULL)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wp, LPARAM lp) {
	switch (uMsg) {
	case WM_DESTROY: {
		PostQuitMessage(0);
		KillTimer(hWnd, 1);
		break;
	}
	case WM_CREATE: {
		srand(time(NULL));
		for (int i = 0; i < 10; i++) { // set all aliens to top of screen at somwhat random position
			alien[i].y = -(rand() % 300);
			alien[i].x = (rand() % 500);
		}
	}
	break;
	case WM_TIMER:
	{
		bullet_timer += 1;
		srand(time(NULL));
		for (int i = 0; i < 10; i++) {
			alien[i].y += 1;
			if (alien[i].y > 600) { // If alien touches bottom of screen go back up to the top at a somewhat random position
				alien[i].y = -(rand() % 300);
				alien[i].x = (rand() % 500);
				player.hp += 1;
			}
		}
		for (int i = 0; i < 20; i++) {
			if (bullet[i].fired == true) {
				bullet[i].y -= 10;
			}
			if (bullet[i].y < 0) {
				bullet[i].fired = false;
			}
		}
		if (player.hp >= 3) {
			PostQuitMessage(0);
			KillTimer(hWnd, 1);
		}
		if (GetAsyncKeyState(VK_LEFT)) {//arrow keys
			player.x -= 1;
		}
		if (GetAsyncKeyState(VK_RIGHT)) {//arrow keys
			player.x += 1;
		}
		if (GetAsyncKeyState(VK_SPACE)) {//space bar
			
			if (bullet_timer > 20) {
				bullet_on += 1;

				bullet[bullet_on].x = player.x + 16;
				bullet[bullet_on].y = player.y;
				bullet[bullet_on].fired = true;
			}
		}
		if (bullet_on > 20) {
			bullet_on = 0;
		}
		if (bullet_timer > 20) {
			bullet_timer = 0;
		}
		// This block checks for collisions
		for (int i = 0; i < 20; i++) {
			for (int j = 0; j < 10; j++) {
				if (bullet[i].x > alien[j].x && bullet[i].x < alien[j].x + 32) {
					if (bullet[i].y > alien[j].y && bullet[i].y < alien[j].y + 32) {
						alien[j].y = -(rand() % 100);
						alien[j].x = (rand() % 500);
						bullet[i].x = player.x + 16;
						bullet[i].y = player.y;
						bullet[i].fired = false;
					}
				}
			}
		}
		InvalidateRect(hWnd, NULL, TRUE); //makes window to redraw
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		RECT rc;
		GetClientRect(hWnd, &rc);
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC hdcMem = CreateCompatibleDC(hdc);
		HDC hdcBuffer = CreateCompatibleDC(hdc);// make a drawing space to draw everything to 
		HBITMAP hbmBuffer = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
		HBITMAP hbmOldBuffer = (HBITMAP)SelectObject(hdcBuffer, hbmBuffer); 
		FillRect(hdcBuffer, &ps.rcPaint, (HBRUSH)GetStockObject(BLACK_BRUSH));
		HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, player.mask);// put the player.mask image into hdcMem
		BitBlt(hdcBuffer, player.x, player.y, 32, 32, hdcMem, 0, 0, SRCAND);//draw hdcMem to hdcBuffer
		SelectObject(hdcMem, player.bitmap);//put the player.bitmap image into hdcMem
		BitBlt(hdcBuffer, player.x, player.y, 32, 32, hdcMem, 0, 0, SRCPAINT);//draw hdcMem to hdcBuffer
		for (int i = 0; i < 10; i++) {
			hbmOld = (HBITMAP)SelectObject(hdcMem, alien[i].mask);//put the alien[i].mask image into hdcMem
			BitBlt(hdcBuffer, alien[i].x, alien[i].y, 32, 32, hdcMem, 0, 0, SRCAND);//draw hdcMem to hdcBuffer
			SelectObject(hdcMem, alien[i].bitmap);//put the alien[i].bitmap image into hdcMem
			BitBlt(hdcBuffer, alien[i].x, alien[i].y, 32, 32, hdcMem, 0, 0, SRCPAINT);//draw hdcMem to hdcBuffer
		}
		for (int i = 0; i < 20; i++) {
			if (bullet[i].fired == true) {
				hbmOld = (HBITMAP)SelectObject(hdcMem, bullet[i].mask);//put the bullet[i].mask image into hdcMem
				BitBlt(hdcBuffer, bullet[i].x, bullet[i].y, 4, 4, hdcMem, 0, 0, SRCAND);//draw hdcMem to hdcBuffer
				SelectObject(hdcMem, bullet[i].bitmap);//put the bullet[i].bitmap image into hdcMem
				BitBlt(hdcBuffer, bullet[i].x, bullet[i].y, 4,4, hdcMem, 0, 0, SRCPAINT);//draw hdcMem to hdcBuffer
			}
		}
		BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdcBuffer, 0, 0, SRCCOPY);//draw hdcBuffer to our window
		SelectObject(hdcMem, hbmOld);
		DeleteDC(hdcMem); //free up system resouces and release hdc to our window
		ReleaseDC(hWnd, hdc);
		DeleteDC(hdcBuffer);
		DeleteObject(hbmBuffer);
	}
	break;
	default: {
		return DefWindowProc(hWnd, uMsg, wp, lp);
	}
		   break;
	}

}
