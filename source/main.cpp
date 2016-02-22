#define _WIN32_WINDOWS 0x501
#define _WIN32_WINNT   0x501

#include <windows.h>
#include <commctrl.h>

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "graphics.h"
#include "mrcfile.h"
#include "mcubes.h"

#include "resource.h"

LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgUmbralProc   (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgColorProc    (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgVistaProc    (HWND, UINT, WPARAM, LPARAM);

char szClassName[ ] = "WindowsApp";

HWND hWndMain;
HWND hWndDraw;
HWND hWndRBar;
HWND hWndStat;
HWND hWndUpDw;
HINSTANCE hInst;

tObject  obj;
tVolumen vol;

int RButton = 0; /*1 si el boton derecho esta presionado*/
int LButton = 0; /*1 si el boton izquierdo esta presionado*/

int msx, msy;   /*Puntos donde se presiono algun boton del mouse*/

int color = RGB(128, 220, 255);
COLORREF colorCustom[16] = {0};

/**********************************************************************************/
/*	Funcion:			FreeAll                              					  */
/*	Descripcion:		Libera todos las estructuras en memoria					  */
/*	Parametros:			Ninguno                                                   */
/*	Regresa:			Ninguno                                                   */
/**********************************************************************************/

void FreeAll() {
	if(vol.data) {
		free(vol.data);
		vol.data = NULL;
	}

	if(obj.Vertice) {
		free(obj.Vertice);
		obj.Vertice = NULL;
	}

	if(obj.triInd) {
		free(obj.triInd);
		obj.triInd = NULL;
	}

	if(obj.fNormal) {
		free(obj.fNormal);
		obj.fNormal = NULL;
	}

	if(obj.vNormal) {
		free(obj.vNormal);
		obj.vNormal = NULL;
	}
}

/**********************************************************************************/
/*	Funcion:			FreeObj                              					  */
/*	Descripcion:		Libera la estructura que contiene la isosuperficie        */
/*	Parametros:			Ninguno                                                   */
/*	Regresa:			Ninguno                                                   */
/**********************************************************************************/

void FreeObj() {
	if(obj.Vertice) {
		free(obj.Vertice);
		obj.Vertice = NULL;
	}

	if(obj.triInd) {
		free(obj.triInd);
		obj.triInd = NULL;
	}

	if(obj.fNormal) {
		free(obj.fNormal);
		obj.fNormal = NULL;
	}

	if(obj.vNormal) {
		free(obj.vNormal);
		obj.vNormal = NULL;
	}
}

/**********************************************************************************/
/*	Funcion:			PrintStatus                                               */
/*	Descripcion:		Imprime informacion relevante de los procesos internos de */
/*                      la aplicacion, como el avance de la operacion seleccionada*/
/*                      por el usuario.                                           */
/*	Parametros:			Iguales a los de printf.                                  */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void PrintStatus(char *str, ...) {
	char buf[255] = {0};

	va_list vl;

	va_start(vl, str);

	vsprintf(buf, str, vl);

	va_end(vl);

	SendMessage(hWndStat, SB_SETTEXT, (WPARAM)3, (LPARAM)buf);
}

/**********************************************************************************/
/*	Funcion:			DialogoCargar                                             */
/*	Descripcion:		Muestra una cuadro de dialogo para que el usuario elija   */
/*                      el archivo a visualizar                                   */
/*	Parametros:			Ninguno                                                   */
/*	Regresa:			TRUE - operación exitosa, usuario presiono "Aceptar" y    */
/*                             además existía el archivo                          */
/*                      FALSE - operación no exitosa, usuario presiono "Cancelar" */
/**********************************************************************************/

BOOL DialogoCargar() {
	char filename[256] = {0};
	char buf[256] = {0};
	OPENFILENAME of;

	memset(&of, 0, sizeof(of));

	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner   = hWndMain;
	of.hInstance   = hInst;
	of.lpstrFilter = "MRC Files (*.mrc)\0*.mrc\0All files(*.*)\0*.*\0\0";
	of.lpstrFile   = filename;
	of.nMaxFile    = 256;
	of.Flags	   = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES;
	of.lpstrDefExt = "*.mrc\0";

	if(GetOpenFileName(&of)) {
		FreeAll();

		if(LoadMRC(filename, &vol)) {
			MessageBox(hWndMain, "Error loading file", "Error while opening", MB_ICONERROR);
	
			SendMessage(hWndStat, SB_SETTEXT, (WPARAM)0, (LPARAM)"");
			SendMessage(hWndStat, SB_SETTEXT, (WPARAM)1, (LPARAM)"");

			return FALSE;
		} else {
			sprintf_s(buf, "File: %s", filename + of.nFileOffset);
			SendMessage(hWndStat, SB_SETTEXT, (WPARAM)0, (LPARAM)buf);

			sprintf_s(buf, "Value Range: %d .. %d", (int)vol.min, (int)vol.max);
			SendMessage(hWndStat, SB_SETTEXT, (WPARAM)1, (LPARAM)buf);

			SendMessage( hWndUpDw, UDM_SETRANGE32, 0L, MAKELONG((int)vol.max, (int)vol.min));
			SendMessage( hWndUpDw, UDM_SETPOS32,   0L, MAKELONG( (int)(vol.max/2), 0));
		}
	}

	return TRUE;
}

/**********************************************************************************/
/*	Funcion:			DialogoColor                                              */
/*	Descripcion:		Muestra una cuadro de dialogo para que el usuario elija   */
/*                      el color para dibujar el objeto.                          */
/*	Parametros:			Ninguno                                                   */
/*	Regresa:			TRUE - operación exitosa                                  */
/*                      FALSE - operación no exitosa                              */
/**********************************************************************************/

BOOL DialogoColor() {
    CHOOSECOLOR cc = {sizeof(CHOOSECOLOR)};

    cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR;
    cc.hwndOwner = hWndMain;
    cc.rgbResult = color;
    cc.lpCustColors = colorCustom;

    if(ChooseColor(&cc)) {
        color = cc.rgbResult;
		return TRUE;
    }

	return FALSE;
}

/**********************************************************************************/
/*	Funcion:			LoadMainMenu                                              */
/*	Descripcion:		Carga el Menú principal de la aplicación con opciones de  */
/*                      "Abrir" y "Cerrar".                                       */
/*	Parametros:			Ninguno                                                   */
/*	Regresa:			Ninguno                                                   */
/**********************************************************************************/

void LoadMainMenu() {
	HMENU hm, hmp;

	hm  = CreateMenu();
	hmp = CreateMenu();

	AppendMenu(hmp, MF_STRING,    ID_MABRIR,  "&Open");
	AppendMenu(hmp, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hmp, MF_STRING,	  ID_MCERRAR, "&Exit");

	AppendMenu(hm, MF_STRING | MF_POPUP, (UINT_PTR)hmp, "&File");

	SetMenu(hWndMain, hm);
}

/**********************************************************************************/
/*	Funcion:			DlgUmbral                                                 */
/*	Descripcion:		Carga el dialogo que contendra la opcion de eleccion de   */
/*	                    umbral y el boton "Calcular" para dibujar la Isosuperficie*/
/*	                    según el umbral introducido por el usuario.               */
/*	Parametros:			hWndPrnt - Handle de la ventana padre                     */
/*	Regresa:			Handle del dialogo o NULL en caso de algun fallo.         */
/**********************************************************************************/

HWND DlgUmbral(HWND hWndPrnt) {
	HWND hWndTmp, hWndTmp1, hWndTmp2;
    WNDCLASSEX wincl;

    wincl.hInstance = hInst;
    wincl.lpszClassName = "DlgUmbral";
    wincl.lpfnWndProc = DlgUmbralProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_WINDOW;

	if (!RegisterClassEx (&wincl))
        return 0;

	hWndTmp = CreateWindowEx(0, "DlgUmbral", "", WS_CHILD | WS_VISIBLE,
							 0, 0, 0, 0, hWndPrnt, NULL, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "STATIC", "Thresh.: ", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
							 4, 4, 60, 24, hWndTmp, NULL, hInst, NULL);

	hWndTmp1 = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | ES_LEFT,
							 60, 1, 80, 24, hWndTmp, (HMENU)IDC_EDIT_UMBRAL, hInst, NULL);

	hWndTmp2 = CreateWindowEx (0L, UPDOWN_CLASS, "",
							   WS_CHILD | WS_BORDER | WS_VISIBLE | 
							   UDS_ARROWKEYS | UDS_ALIGNRIGHT | UDS_HOTTRACK | UDS_SETBUDDYINT | UDS_NOTHOUSANDS,
							   0, 0, 8, 8, hWndTmp, (HMENU)IDC_TRACK_UMBRAL, hInst, NULL );

	SendMessage( hWndTmp2, UDM_SETBUDDY, (LONG)hWndTmp1, 0L );

	SendMessage( hWndTmp2, UDM_SETRANGE, 0L, MAKELONG(255, 0));

	SendMessage( hWndTmp2, UDM_SETPOS, 0L, MAKELONG( 128, 0));

	hWndUpDw = hWndTmp2;

	hWndTmp1 = CreateWindowEx(0, "BUTTON", "Calculate", WS_CHILD | WS_VISIBLE | BS_CENTER  ,
							 144, 0, 80, 24, hWndTmp, (HMENU)IDC_BTN_DO, hInst, NULL);

	return hWndTmp;
}

/**********************************************************************************/
/*	Funcion:			DlgColor                                                  */
/*	Descripcion:		Carga el dialogo que contendra las opciones de color y    */
/*	                    modelo de iluminacion.                                    */
/*	Parametros:			hWndPrnt - Handle de la ventana padre                     */
/*	Regresa:			Handle del dialogo o NULL en caso de algun fallo.         */
/**********************************************************************************/

HWND DlgColor(HWND hWndPrnt) {
	HWND hWndTmp, hWndTmp1;
    WNDCLASSEX wincl;

    wincl.hInstance = hInst;
    wincl.lpszClassName = "DlgColor";
    wincl.lpfnWndProc = DlgColorProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_WINDOW;

	if (!RegisterClassEx (&wincl))
        return 0;

	hWndTmp = CreateWindowEx(0, "DlgColor", "", WS_CHILD | WS_VISIBLE,
							 0, 0, 0, 0, hWndPrnt, NULL, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "STATIC", "Colour: ", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
							 4, 4, 50, 24, hWndTmp, NULL, hInst, NULL);

	hWndTmp1 = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "BUTTON", "", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
							 50, 0, 30, 24, hWndTmp, (HMENU)IDC_BTN_COLOR, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "STATIC", "Lighting: ", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
							 84, 4, 80, 24, hWndTmp, NULL, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "BUTTON", "Gouraud", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
							 170, 0, 70, 24, hWndTmp, (HMENU)IDC_BTN_GOURAUD, hInst, NULL);

	SendMessage(hWndTmp1, BM_SETCHECK, (WPARAM)BST_CHECKED, NULL);

	hWndTmp1 = CreateWindowEx(0, "BUTTON", "Flat", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
							 242, 0, 70, 24, hWndTmp, (HMENU)IDC_BTN_PLANO, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "BUTTON", "Wire", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
							 314, 0, 70, 24, hWndTmp, (HMENU)IDC_BTN_MALLA, hInst, NULL);

	return hWndTmp;
}

/**********************************************************************************/
/*	Funcion:			DlgVista                                                  */
/*	Descripcion:		Carga el dialogo que contendra las opciones de punto de   */
/*	                    visión de la camara.                                      */
/*	Parametros:			hWndPrnt - Handle de la ventana padre                     */
/*	Regresa:			Handle del dialogo o NULL en caso de algun fallo.         */
/**********************************************************************************/

HWND DlgVista(HWND hWndPrnt) {
	HWND hWndTmp, hWndTmp1;
    WNDCLASSEX wincl;

    wincl.hInstance = hInst;
    wincl.lpszClassName = "DlgVista";
    wincl.lpfnWndProc = DlgVistaProc;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_WINDOW;

	if (!RegisterClassEx (&wincl))
        return 0;

	hWndTmp = CreateWindowEx(0, "DlgVista", "", WS_CHILD | WS_VISIBLE,
							 0, 0, 0, 0, hWndPrnt, NULL, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "STATIC", "View: ", WS_CHILD | WS_VISIBLE | SS_SIMPLE,
							 4, 4, 50, 24, hWndTmp, NULL, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "BUTTON", "Up", WS_CHILD | WS_VISIBLE,
							 46, 0, 80, 24, hWndTmp, (HMENU)IDC_BTN_UP, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "BUTTON", "Front", WS_CHILD | WS_VISIBLE,
							 128, 0, 80, 24, hWndTmp, (HMENU)IDC_BTN_FRONT, hInst, NULL);

	hWndTmp1 = CreateWindowEx(0, "BUTTON", "Left", WS_CHILD | WS_VISIBLE,
							 210, 0, 80, 24, hWndTmp, (HMENU)IDC_BTN_LEFT, hInst, NULL);

	return hWndTmp;
}

/**********************************************************************************/
/*	Funcion:			LoadControls                                              */
/*	Descripcion:		Carga la barra de herramientas que contendra los dialogos */
/*	                    DlgUmbral, DlgColor y DlgVista, que permitiran la         */
/*	                    interaccion del usuario con la aplicacion.                */
/*	Parametros:			Ninguno                                                   */
/*	Regresa:			Ninguno                                                   */
/**********************************************************************************/


void LoadControls() {
	REBARINFO	  rbi;
	REBARBANDINFO rbBand;
	HWND		  hWndTmp;

	hWndRBar = CreateWindowEx(WS_EX_CLIENTEDGE, REBARCLASSNAME, NULL,
                           WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
						   RBS_VARHEIGHT | CCS_NODIVIDER | RBS_BANDBORDERS ,
                           0,0,0,0, hWndMain, NULL, hInst, NULL);

	rbi.cbSize = sizeof(REBARINFO);
	rbi.fMask  = 0;
	rbi.himl   = (HIMAGELIST)NULL;
	
	SendMessage(hWndRBar, RB_SETBARINFO, 0, (LPARAM)&rbi);


	memset(&rbBand, 0, sizeof(rbBand));

	hWndTmp = DlgUmbral(hWndRBar);

	rbBand.cbSize = sizeof(REBARBANDINFO);
	rbBand.fMask  = RBBIM_COLORS | RBBIM_STYLE | RBBIM_CHILD  | RBBIM_CHILDSIZE | RBBIM_SIZE;
	rbBand.fStyle = RBBS_CHILDEDGE | RBBS_NOGRIPPER;
	rbBand.hwndChild  = hWndTmp;
	rbBand.cxMinChild = 0;
	rbBand.cyMinChild = 24;
	rbBand.cx         = 230;
	rbBand.clrBack	  = GetSysColor(COLOR_MENUBAR);
   
	SendMessage(hWndRBar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

	rbBand.hwndChild  = DlgColor(hWndRBar);
	rbBand.cx         = 390;
   
	SendMessage(hWndRBar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

	rbBand.hwndChild  = DlgVista(hWndRBar);
	rbBand.cx         = 140;
   
	SendMessage(hWndRBar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);


}

/**********************************************************************************/
/*	Funcion:			LoadStatusBar                                             */
/*	Descripcion:		Carga la barra de estado donde se mostrara informacion    */
/*	                    de las operaciones y eventos de la aplicacion.            */
/*	Parametros:			Ninguno                                                   */
/*	Regresa:			Ninguno                                                   */
/**********************************************************************************/


void LoadStatusBar() {
    int statwidths[] = {150, 400, 650, -1};

    hWndStat = CreateWindow(STATUSCLASSNAME, NULL,
						    SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,
							0, 0, 0, 0, hWndMain, (HMENU)IDC_MAIN_STATUS, hInst, NULL);

	SendMessage(hWndStat, SB_SETPARTS, (WPARAM)4, (LPARAM)statwidths);
}

/**********************************************************************************/
/*	Funcion:			WinMain                                                   */
/*	Descripcion:		Funcion principal de toda aplicacion para Windows.        */
/*	Parametros:			Los definidos en el Platform SDK de la WinAPI.            */
/*	Regresa:			Lo definidos en el Platfrom SDK de la WinAPI.             */
/**********************************************************************************/

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)

{    
    MSG messages;
    WNDCLASSEX wincl;
	INITCOMMONCONTROLSEX icc;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx (&wincl))
        return 0;

	hInst = hThisInstance;

	icc.dwSize = sizeof(icc);
	icc.dwICC  = ICC_STANDARD_CLASSES | ICC_PROGRESS_CLASS | ICC_COOL_CLASSES | ICC_WIN95_CLASSES;

	InitCommonControlsEx(&icc);

    hWndMain = CreateWindowEx (0, szClassName, "Viewer",
           WS_OVERLAPPEDWINDOW ,
           CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
           HWND_DESKTOP, NULL, hThisInstance, NULL);

	LoadMainMenu();

	LoadControls();

	hWndDraw = CreateWindow("STATIC", "",
							WS_VISIBLE | WS_CHILD | SS_BLACKFRAME,
							2, 30, 780, 542, hWndMain, NULL, hThisInstance, NULL);

	LoadStatusBar();

	ShowWindow (hWndMain, SW_SHOWMAXIMIZED);

	InitGraphics(hWndDraw);

	UpdateWindow(hWndMain);

	SetLightColor(color);

	memset(&obj, 0, sizeof(tObject));
	memset(&vol, 0, sizeof(tVolumen));

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

	EndGraphics();

	FreeAll();

    return (int)messages.wParam;
}

/**********************************************************************************/
/*	Funcion:			WindowProcedure                                           */
/*	Descripcion:		Funcion que procesa los mensajes de la aplicacion.        */
/*	Parametros:			Los definidos en el Platform SDK de la WinAPI.            */
/*	Regresa:			Lo definidos en el Platfrom SDK de la WinAPI.             */
/**********************************************************************************/


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	tVertex T = {0, 0, 0};
	short zDelta;
	RECT r, r1, r2;

    switch (message)
    {
		case WM_CTLCOLORSTATIC:
			if((HWND)lParam == hWndDraw) {
				if(obj.Vertice!=NULL && obj.triInd!=NULL)
					RenderObject(&obj);
				else
					RenderObject(NULL);
			} break;
		
		case WM_SIZE:
			MoveWindow(hWndRBar, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
			GetClientRect(hWndMain, &r);
			GetClientRect(hWndRBar, &r1);
			GetClientRect(hWndStat, &r2);

			ResizeGraphics(r.right - 2 , r.bottom - r1.bottom - r2.bottom - 4);

			MoveWindow(hWndDraw, 1, r1.bottom + 4, r.right - 2, r.bottom - r1.bottom - r2.bottom - 4, FALSE);

			SendMessage(hWndStat, WM_SIZE, 0, 0);

			InvalidateRect(hWndMain,  NULL, FALSE);
			break;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case ID_MABRIR:
					DialogoCargar();
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
				case ID_MCERRAR:
					PostQuitMessage(0);
					break;
			} break;

		case WM_LBUTTONDOWN:
			LButton = 1;
			msx = (int)LOWORD(lParam);
			msy = (int)HIWORD(lParam);
			break;

		case WM_RBUTTONDOWN:
			RButton = 1;
			msx = (int)LOWORD(lParam);
			msy = (int)HIWORD(lParam);
			break;

		case WM_LBUTTONUP:
			LButton=0;
			break;

		case WM_RBUTTONUP:
			RButton=0;
			break;

		case WM_MOUSEMOVE:
			if(LButton) {
				T.y = (float)((int)LOWORD(lParam)-msx)/-3.6f;
				T.x = (float)((int)HIWORD(lParam)-msy)/-3.6f;
	
				AdjustRotation(T);

				msx = (int)LOWORD(lParam);
				msy = (int)HIWORD(lParam);

				InvalidateRect(hWndMain,  NULL, FALSE);
			} break;

		case WM_MOUSEWHEEL:
			zDelta = (short)HIWORD(wParam);
			if(zDelta > 0)
				AdjustZoom(0.3f);
			else
				AdjustZoom(-0.3f);
			InvalidateRect(hWndMain,  NULL, FALSE);
			break;

        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

/**********************************************************************************/
/*	Funcion:			DlgUmbralProc                                             */
/*	Descripcion:		Funcion que procesa los mensajes de DlgUmbral.            */
/*	Parametros:			Los definidos en el Platform SDK de la WinAPI.            */
/*	Regresa:			Lo definidos en el Platfrom SDK de la WinAPI.             */
/**********************************************************************************/

LRESULT CALLBACK DlgUmbralProc   (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int k;
	char buf[256]={0};
	switch(message) {
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_BTN_DO:
					k  = GetDlgItemInt(hwnd, IDC_EDIT_UMBRAL, NULL, NULL);
					FreeObj();
					MCCreateIsosurface(&vol, &obj, (float)k);
					sprintf_s(buf, "Num. de Triangles: %d", obj.nTrian);
					SendMessage(hWndStat, SB_SETTEXT, (WPARAM)2, (LPARAM)buf);
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
			} break;
		case WM_DESTROY:
            PostQuitMessage (0);
            break;
		default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

/**********************************************************************************/
/*	Funcion:			DlgColorProc                                              */
/*	Descripcion:		Funcion que procesa los mensajes de DlgColor.             */
/*	Parametros:			Los definidos en el Platform SDK de la WinAPI.            */
/*	Regresa:			Lo definidos en el Platfrom SDK de la WinAPI.             */
/**********************************************************************************/

LRESULT CALLBACK DlgColorProc   (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static HBRUSH pincel;
	switch(message) {
		case WM_CREATE:
			pincel = CreateSolidBrush(color);
			return TRUE;

		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_BTN_COLOR:
					DialogoColor();
					DeleteObject(pincel);
					pincel = CreateSolidBrush(color);
					SetLightColor(color);
					InvalidateRect(hWndMain,  NULL, FALSE);
					InvalidateRect(hwnd,  NULL, TRUE);
					break;
				case IDC_BTN_GOURAUD:
					AdjustLightModel(EST_SUAVE);
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
				case IDC_BTN_PLANO:
					AdjustLightModel(EST_PLANO);
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
				case IDC_BTN_MALLA:
					AdjustLightModel(EST_MALLA);
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
			} break;
		case WM_CTLCOLORBTN:
			SetBkColor((HDC)wParam, color);
			return (LRESULT)pincel;
		case WM_DESTROY:
			DeleteObject(pincel);
            PostQuitMessage (0);
            break;
		default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

/**********************************************************************************/
/*	Funcion:			DlgVistaProc                                              */
/*	Descripcion:		Funcion que procesa los mensajes de DlgVista.             */
/*	Parametros:			Los definidos en el Platform SDK de la WinAPI.            */
/*	Regresa:			Lo definidos en el Platfrom SDK de la WinAPI.             */
/**********************************************************************************/

LRESULT CALLBACK DlgVistaProc   (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case IDC_BTN_UP:
					SetQuickView(0);
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
				case IDC_BTN_FRONT:
					SetQuickView(1);
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
				case IDC_BTN_LEFT:
					SetQuickView(2);
					InvalidateRect(hWndMain,  NULL, FALSE);
					break;
			} break;
		case WM_DESTROY:
            PostQuitMessage (0);
            break;
		default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}