#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include "res.h"

static TCHAR szWindowClass[] = _T("DVD-Bouncing-Logo");

// La chaîne de cractère qui apparaîtra dans le titre de l'application
static TCHAR szTitle[] = _T("DVD-Bouncing-Logo");


DWORD WINAPI DVD_bounce(LPVOID lparam) ;
RECT rect;  // on verra qu'il sera utile dans la suite du code d'utiliser une structure RECT (pour définir un rectangle (ma fenêtre GUI) par les coordonnées de ses coins supérieur gauche et inférieur droit)

int w = GetSystemMetrics(SM_CXSCREEN) - 200; // Largeur X de ma fenêtre
int h = GetSystemMetrics(SM_CYSCREEN) - 200; // Hauteur Y de ma fenêtre
		
LRESULT CALLBACK WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) // fonction qu'on rappellera dans WinMain (il s'agit du background de la fenêtre) : 
{
	switch (Msg)
	{
	case WM_PAINT: {
		
		PAINTSTRUCT ps;
		
		HDC hdc = BeginPaint(hWnd, &ps); // contexte spécifié dans lequel la couleur sera appliquée
		
		
		HBRUSH noir = CreateSolidBrush(RGB(0, 0, 0)); // couleur noire
		SelectObject(hdc, noir); 
		
		PatBlt(hdc, 0, 0, w, h, PATCOPY); // application 


		EndPaint(hWnd, &ps);
		break;
	}

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0x00);
		break;

	default:
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}
	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// les principaux paramètres de notre fenêtre (flemme de décrire) 
     WNDCLASSEXA c;
	c.cbSize = sizeof(WNDCLASSEX);
	c.lpfnWndProc = WndProc;
	c.lpszClassName = szWindowClass;
	c.style = CS_HREDRAW | CS_VREDRAW;
	c.cbClsExtra = 0;
	c.cbWndExtra = 0;
	c.hInstance = GetModuleHandle(NULL);
	c.hIcon = LoadIcon(c.hInstance, IDI_APPLICATION);;
	c.hCursor = LoadCursor(NULL, IDC_ARROW);
	c.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	c.lpszMenuName = NULL;
	c.hIconSm = LoadIcon(c.hInstance, IDI_APPLICATION);

		if(!RegisterClassExA(&c))
	{
		//Erreur
		return -1;
	}

	HWND DVDWin = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW , szWindowClass, szTitle, WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, 0, 0, w, h, NULL, NULL, hInstance, NULL);
	// création d'une fenêtre noire, avec l'affichage du titre et du logo, impossible à redimensionner grâce aux attributs (WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU)
	
	ShowWindow(DVDWin, nCmdShow); // affichage de la fenêtre 
	UpdateWindow(DVDWin); // puis on la met à jour

    CreateThread(0, 0, &DVD_bounce, 0, 0, 0); // on appel notre fonction sous la forme d'un thread pour éviter de bloquer le programme (avec la boucle infinie) 

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0) > 0 && !false) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

DWORD WINAPI DVD_bounce(LPVOID lparam) 
{
	HBITMAP monDVD = (HBITMAP)LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDB_IMAGE), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS); // on charge notre image depuis un fichier ressource (le fameux logo DVD) 
    HWND DVDWin = FindWindowW(L"DVD-Bouncing-Logo", L"DVD-Bouncing-Logo"); // on récupère le handle de notre fenêtre (une fois sa création et son affichage)
    
	HDC hdc = GetDC(DVDWin); 
    HDC hdc_memoire = CreateCompatibleDC(hdc); // récupère le handle précédent (stock en mémoire) compatible 
    
    SelectObject(hdc_memoire, monDVD); // selection du contexte spécifié et de notre image 
    GetWindowRect(DVDWin, &rect); // récupère les dimensions du rectangle englobant de la fenêtre spécifiée
    
	int srcw = rect.right; // largeur source 
	int srch = rect.bottom; // hauteur source 
	
    int mouvementX = 5; int mouvementY = 5; // la vitesse à laquelle va se déplacer le logo
	
	
		while(true) {

			  int xposition = rect.left += mouvementX; 
	          int yposition = rect.top += mouvementY; 
	

			BitBlt(hdc, xposition, yposition, srcw, srch, hdc_memoire, 0, 0, SRCCOPY); // on dessine notre image (qui se déplacera en fonction des coordonnées XY

			if (yposition >= srch - 200) { // si la position Y dépasse la valeur correspondant à la hauteur de la fenêtre (définit plus haut) alors: 
				mouvementY = -mouvementY; // mouvement négatif donc direction changée
			}

			if (xposition >= srcw - 300) { // si la position X dépasse la valeur correspondant à la largeur de la fenêtre (définit plus haut) alors: 
				mouvementX = -mouvementX; // Idem

			}
			if (yposition <= 0) { //même chose mais position Y inférieure à 0
				mouvementY = -mouvementY;
			}

			if (xposition <= 0) { //même chose mais valeur X inférieure à 0 
				mouvementX = -mouvementX;
			}
			// créant ainsi un rebondissement sur les bords de la fenêtre infini
			Sleep(10); // pour avoir un mouvement stable 
}
}
