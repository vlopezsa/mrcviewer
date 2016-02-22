#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>

#include "common.h"
#include "graphics.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

extern HWND hWndDraw;

HDC hDC;
HGLRC hRC;

int videoFlags;
int dEstilo = EST_SUAVE;

GLfloat glcolor[4] = {0};

tVertex Posicion;   //Donde esta la camara
tVertex Vista;	  //Hacia donde ve la camara
tVertex vArriba;	  //Apunta hacia arriba

tVertex Rot;

/**********************************************************************************/
/*	Funcion:			DoMagnitud                                                */
/*	Descripcion:		Calcula la magnitud de un vector.                         */
/*	Parametros:	        v - Vector al cual se le quiere obtener la magnitud.      */
/*	Regresa:			Magnitud del vector.                                      */
/**********************************************************************************/

float DoMagnitude(tVertex v) {
	return (float)sqrt(((v.x)*(v.x))+((v.y)*(v.y))+((v.z)*(v.z)));
}

/**********************************************************************************/
/*	Funcion:			Normalize                                                 */
/*	Descripcion:		Normaliza un vector                                       */
/*	Parametros:	        v - Vector que se quiere normalizar.                      */
/*	Regresa:			Nada. El vector que se paso como parametro se actualiza.  */
/**********************************************************************************/

void Normalize(tVertex *v) {
	float lon = DoMagnitude(*v);

	(*v).x /= lon;
	(*v).y /= lon;
	(*v).z /= lon;
}

/**********************************************************************************/
/*	Funcion:			DoSimpleNormal                                            */
/*	Descripcion:		A partir de tres puntos que describen un triangulo, se    */
/*	                    se calcula el vector normal a este.                       */
/*	Parametros:	        p1, p2, p3 - Puntos que describen el triangulo.           */
/*	                    vn         - Vector donde guardar el vector normal.       */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void DoSimpleNormal(tVertex p1, tVertex p2, tVertex p3, tVertex *vn) {
	float lon;
	tVertex v1,v2;
 
	v1.x=p2.x-p1.x;
	v1.y=p2.y-p1.y;
	v1.z=p2.z-p1.z;

	v2.x=p3.x-p1.x;
	v2.y=p3.y-p1.y;
	v2.z=p3.z-p1.z;

	vn->x=(v1.y*v2.z)-(v1.z*v2.y);
	vn->y=(v1.z*v2.x)-(v1.x*v2.z);
	vn->z=(v1.x*v2.y)-(v1.y*v2.x);
  
	lon=(float)sqrt(((vn->x)*(vn->x))+((vn->y)*(vn->y))+((vn->z)*(vn->z)));

	vn->x=(vn->x)/lon;
	vn->y=(vn->y)/lon;
	vn->z=(vn->z)/lon;	
}

/**********************************************************************************/
/*	Funcion:			ResizeWindow                                              */
/*	Descripcion:        Cambia el tamaño del area de dibujo.                      */
/*	Parametros:	        width, height  - ancho y alto respectivamente de la nueva */
/*	                                     area de dibujo.                          */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void ResizeWindow(int width, int height) {
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 

	gluPerspective(55.0f,(GLfloat)width/(GLfloat)height, 0.5f, 10000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/**********************************************************************************/
/*	Funcion:			ResizeGraphics                                            */
/*	Descripcion:        Llamada por la aplicacion principal para informar que el  */
/*	                    la ventana ha cambiado de tamaño y es necesario cambiar el*/
/*	                    tamaño del area de dibujo.                                */
/*	Parametros:	        width, height  - ancho y alto respectivamente de la nueva */
/*	                                     area de dibujo.                          */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

int ResizeGraphics(int width, int height) {
	ResizeWindow(width, height);

	return 0;
}

/**********************************************************************************/
/*	Funcion:			InitGL                                                    */
/*	Descripcion:        Inicializa los parametros por omision de OpenGL en la     */
/*	                    aplicacion.                                               */
/*	Parametros:	        Ninguno.                                                  */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void InitGL() {
	//GLfloat LightAmbient[]= {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat blanco[]= {0.5f,0.9f,1.0f,1.0f};
	//GLfloat LightPosition[]={220.0f,-200.0f,220.0f,1.0f};

	glEnable (GL_LIGHTING);
	
	// Buffer de profundidad 
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST); //Habilida la prueba de profundidad
	
	glDepthFunc(GL_LEQUAL); // Tipo de prueba

	glEnable(GL_NORMALIZE);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); //Mejora la perspectiva

	glClearColor(0.0f, 0.0f, 0.0f, 0); //Pone el color del fondo a gris	

	//glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, blanco);
	//glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
	glEnable (GL_LIGHT0);
}

/**********************************************************************************/
/*	Funcion:			SetLightColor                                             */
/*	Descripcion:        Cambia el color de la luz difusa del objeto.              */
/*	Parametros:	        color - Nuevo color para la luz difusa.                   */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void SetLightColor(int color) {
	glcolor[0] = ((GLfloat)((unsigned int)color&0xFF)) / 255.0f;
	glcolor[1] = ((GLfloat)(((unsigned int)color>>8)&0xFF)) / 255.0f;
	glcolor[2] = ((GLfloat)(((unsigned int)color>>16)&0xFF)) / 255.0f;
	glcolor[3] = 1.0f;

	glLightfv(GL_LIGHT0, GL_DIFFUSE, glcolor);
}

/**********************************************************************************/
/*	Funcion:			InitGraphics                                              */
/*	Descripcion:        Inicializa la parte grafica de la aplicacion.             */
/*	Parametros:	        hWnd  - Handle del area sobre la que se quiere dibujar.   */
/*	Regresa:			Cero en operacion exitosa y distinto de cero si no.       */
/**********************************************************************************/

int  InitGraphics(HWND hWnd) {
	GLuint PixelFormat; //Contiene el formato de pixel

	static PIXELFORMATDESCRIPTOR pfd= //Contiene la informacion del formato del pixel
	{
		sizeof(PIXELFORMATDESCRIPTOR), //Tamaño de este descriptor
			1,
			PFD_DRAW_TO_WINDOW | //Formato que soporte Windows
			PFD_SUPPORT_OPENGL | //Formato que soporte OpenGL
			PFD_DOUBLEBUFFER, //Soporta doble buffering
			PFD_TYPE_RGBA, //Peticion de un formato RGBA
			16, // Selecciona el color de la profundidad
			0, 0, 0, 0, 0, 0, //Bits de Color Ignorado
			0, // No Alpha Buffer
			0, // Shift Bit Ignorado
			0, // Buffer de no acumulacion
			0, 0, 0, 0, //Bits de acumulacion ignorados
			16, //16bit Z Buffer (Buffer de profundidad)
			0, //No Stencil Buffer
			0, //No Auxiliary Buffer
			PFD_MAIN_PLANE, //Capa principal de dibujo
			0, //Reservado
			0,0,0 //Mascaras de capa ignoradas
	};

	if (!(hDC=GetDC(hWnd))) { //Se obtuvo un contexto de dispositivo?
		MessageBox(NULL,"No se pudo crear un contexto GL","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return -1;
	}

	if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) {//Windows encontro un formato de pixel similar?
		MessageBox(NULL,"No se encontro un formato de pixel adecuado","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return -1;
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd)) {//Se pudo iniciar el formato de pixel?
		MessageBox(NULL,"No se pudo iniciar un formato de pixel","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return -1;
	}

	if(!(hRC=wglCreateContext(hDC))) {//Conseguimos el contexto para renderear?
		MessageBox(NULL,"No se pudo crear un contexto para el rendereo GL","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return -1;
	}

	if(!wglMakeCurrent(hDC,hRC)) {//Intento de activar el contexto de rendering
		MessageBox(NULL,"No se pudo activar el contexto de rendereo GL","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return -1;
	}

	RECT r;

	GetWindowRect(hWnd, &r);

	InitGL();

	ResizeGraphics(r.right, r.bottom);

	Rot.x = 0;
	Rot.y = 0;
	Rot.z = 0;

	Vista.x = 0;
	Vista.y = 0.5;
	Vista.z = 0;

	Posicion.x =  0;
	Posicion.y =  0.5;
	Posicion.z =  76;

	vArriba.x = 0;
	vArriba.y = 1;
	vArriba.z = 0;

	return 0;
}

/**********************************************************************************/
/*	Funcion:			EndGraphics                                               */
/*	Descripcion:        Libera los recursos reservador para la parte grafica de   */
/*	                    la aplicacion.                                            */
/*	Parametros:	        Ninguno.                                                  */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void EndGraphics() {
	if (hRC) {
		if(!wglMakeCurrent(NULL,NULL)) // ¿Se pudo liberar los contextos DC y RC?
			MessageBox(NULL,"No se pudo liberar DC y RC", "ERROR",MB_OK | MB_ICONINFORMATION);

		if(!wglDeleteContext(hRC)) // ¿Se borro RC?
			MessageBox(NULL, "Fallo la liberacion del Contexto de Rendereo","ERROR",MB_OK | MB_ICONINFORMATION);

		hRC=NULL;
	}

	//if (hDC && !ReleaseDC(hWndDraw,hDC)) { // ¿Se libero DC?
	//	MessageBox(NULL,"No se libero el contexto del dispositivo","ERROR",MB_OK | MB_ICONINFORMATION);
	//	hDC=NULL;
	//}
}

/**********************************************************************************/
/*	Funcion:			Rotate.                                                   */
/*	Descripcion:        Rota un vector a partir de un punto pivote, usando un     */
/*	                    vector de rotacion.                                       */
/*	Parametros:	        Rt    - Vector de rotacion.                               */
/*	                    Piv   - Punto pivote.                                     */
/*	                    Prt   - Punto a rotar.                                    */
/*	Regresa:			Ninguno. Prt se actualiza con el vector rotado.           */
/**********************************************************************************/

void Rotate(tVertex Rt, tVertex Piv, tVertex *Prt) {
	tVertex vNewView;
	tVertex vView;	
	float angle = 0.05f;	
	float cosTheta = (float)cos(angle);
	float sinTheta = (float)sin(angle);

	vView.x = Prt->x - Piv.x;
	vView.y = Prt->y - Piv.y;
	vView.z = Prt->z - Piv.z;

	vNewView.x  = (cosTheta + (1 - cosTheta) * Rt.x * Rt.x)			* vView.x;
	vNewView.x += ((1 - cosTheta) * Rt.x * Rt.y - Rt.z * sinTheta)	* vView.y;
	vNewView.x += ((1 - cosTheta) * Rt.x * Rt.z + Rt.y * sinTheta)	* vView.z;

	vNewView.y  = ((1 - cosTheta) * Rt.x * Rt.y + Rt.z * sinTheta)	* vView.x;
	vNewView.y += (cosTheta + (1 - cosTheta) * Rt.y * Rt.y)			* vView.y;
	vNewView.y += ((1 - cosTheta) * Rt.y * Rt.z - Rt.x * sinTheta)	* vView.z;
	
	vNewView.z  = ((1 - cosTheta) * Rt.x * Rt.z - Rt.y * sinTheta)	* vView.x;
	vNewView.z += ((1 - cosTheta) * Rt.y * Rt.z + Rt.x * sinTheta)	* vView.y;
	vNewView.z += (cosTheta + (1 - cosTheta) * Rt.z * Rt.z)			* vView.z;

	Prt->x = Piv.x + vNewView.x;
	Prt->y = Piv.y + vNewView.y;
	Prt->z = Piv.z + vNewView.z;
}

/**********************************************************************************/
/*	Funcion:			AdjustRotation.                                           */
/*	Descripcion:        Rota el punto de vision segun el vector pasado como       */
/*	                    parametro.                                                */
/*	Parametros:	        R.x, R.y, Rotacion en X e Y segun el movimiento del mouse */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void AdjustRotation(tVertex R) {
	if(figual(R.x, 0.0f) && figual(R.y, 0.0f) && figual(R.z, 0.0f)) return;

	Normalize(&R);

	Rotate(R, Vista, &Posicion);
}

/**********************************************************************************/
/*	Funcion:			AdjustZoom.                                               */
/*	Descripcion:        Ajusta el Zoom de la camara.                              */
/*	Parametros:	        z  - Factor de Zoom.                                      */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void AdjustZoom(float z) {
	tVertex Vector;

	Vector.x = Vista.x - Posicion.x;
	Vector.y = Vista.y - Posicion.y;
	Vector.z = Vista.z - Posicion.z;

	Posicion.x += Vector.x * z;
	Posicion.y += Vector.y * z;
	Posicion.z += Vector.z * z;

	/*Vista.x += Vector.x * z;
	Vista.y += Vector.y * z;
	Vista.z += Vector.z * z;*/
}

/**********************************************************************************/
/*	Funcion:			SetQuickView.                                             */
/*	Descripcion:        Ajusta la posicion de la camara a una estandar.           */
/*	Parametros:	        view - 0     Desde Arriba                                 */
/*	                           1     Desde El Frente                              */
/*	                           2     Desde La Izquierda                           */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void SetQuickView(int view) {
	float m = DoMagnitude(Posicion);

	Vista.x = 0;
	Vista.y = 0;
	Vista.z = 0;

	if(view==0) {
		Posicion.x = 0;	 Posicion.y = m; Posicion.z = 0;
		vArriba.x  = 0;  vArriba.y  = 0; vArriba.z  =-1;
	} else if(view==1) {
		Posicion.x = 0;	 Posicion.y = 0; Posicion.z =  m;
		vArriba.x  = 0;  vArriba.y  = 1; vArriba.z  =  0;
	} else if(view==2) {
		Posicion.x = -m; Posicion.y = 0; Posicion.z = 0;
		vArriba.x  =  0; vArriba.y  = 1; vArriba.z  = 0;
	}
}

/**********************************************************************************/
/*	Funcion:			AdjustLight   .                                           */
/*	Descripcion:        Selecciona el modelo de iluminacion.                      */
/*	Parametros:	        Est - EST_SUAVE     Gouraud                               */
/*	                          EST_PLANO     Facetado                              */
/*	                          EST_MALLA     Muestra la malla del modelo           */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void AdjustLightModel(int Est) {
	dEstilo = Est;

	if(dEstilo!=EST_MALLA) {
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glEnable(GL_LIGHT0);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	}
}

/**********************************************************************************/
/*	Funcion:			DoNormals.                                                */
/*	Descripcion:        Calcula las normales de todos los triangulos del objeto.  */
/*	Parametros:	        O - Objeto al cual se le quieren calcular las normales.   */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void DoNormals(tObject *O) {
	tVertex *vTemp;
	int *nvect, i;

	O->fNormal = (tVertex *)malloc(O->nTrian * sizeof(tVertex));
	O->vNormal = (tVertex *)malloc(O->nVert  * sizeof(tVertex));

	for(i=0; i<O->nTrian; i++) {
		DoSimpleNormal(O->Vertice[O->triInd[i].vertInd[0]],
					  O->Vertice[O->triInd[i].vertInd[1]],
					  O->Vertice[O->triInd[i].vertInd[2]],
					  &O->fNormal[i]);
	}

	vTemp = (tVertex *)malloc(O->nVert  * sizeof(tVertex));
	nvect = (int *)malloc(O->nVert  * sizeof(int));

	memset(vTemp, 0, O->nVert * sizeof(tVertex));
	memset(nvect, 0, O->nVert * sizeof(int));

	for(i=0; i<O->nTrian; i++) {
		vTemp[O->triInd[i].vertInd[0]].x += O->fNormal[i].x;
		vTemp[O->triInd[i].vertInd[0]].y += O->fNormal[i].y;
		vTemp[O->triInd[i].vertInd[0]].z += O->fNormal[i].z;
		nvect[O->triInd[i].vertInd[0]]++;

		vTemp[O->triInd[i].vertInd[1]].x += O->fNormal[i].x;
		vTemp[O->triInd[i].vertInd[1]].y += O->fNormal[i].y;
		vTemp[O->triInd[i].vertInd[1]].z += O->fNormal[i].z;
		nvect[O->triInd[i].vertInd[1]]++;

		vTemp[O->triInd[i].vertInd[2]].x += O->fNormal[i].x;
		vTemp[O->triInd[i].vertInd[2]].y += O->fNormal[i].y;
		vTemp[O->triInd[i].vertInd[2]].z += O->fNormal[i].z;
		nvect[O->triInd[i].vertInd[2]]++;
	}

	for(i=0; i<O->nVert; i++) {
		O->vNormal[i].x = vTemp[i].x / (float)nvect[i];
		O->vNormal[i].y = vTemp[i].y / (float)nvect[i];
		O->vNormal[i].z = vTemp[i].z / (float)nvect[i];

		Normalize(&O->vNormal[i]);
	}

	free(vTemp);
	free(nvect);
}

/**********************************************************************************/
/*	Funcion:			RenderObject.                                             */
/*	Descripcion:        Dibuja el objecto en pantalla.                            */
/*	Parametros:	        O - Objeto a dibujar.                                     */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/

void RenderObject(tObject *O) {
	int i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Se borra el buffer de pantalla y profundidad
	glLoadIdentity(); // Se resetea la matriz Current Modelview

	gluLookAt(Posicion.x, Posicion.y, Posicion.z,
		      Vista.x, Vista.y, Vista.z,
			  vArriba.x, vArriba.y, vArriba.z);

	if(O == NULL) {
		SwapBuffers(hDC);	
		return;
	}

	glBegin(GL_TRIANGLES);

	if(dEstilo != EST_SUAVE) {
		for(i=0; i<O->nTrian; i++) {
	
			glNormal3f(O->fNormal[i].x,
					   O->fNormal[i].y,
					   O->fNormal[i].z);

			//glColor3f(glcolor[0], glcolor[1], glcolor[2]);
			glVertex3f(O->Vertice[O->triInd[i].vertInd[0]].x,
					   O->Vertice[O->triInd[i].vertInd[0]].y,
					   O->Vertice[O->triInd[i].vertInd[0]].z);
			glVertex3f(O->Vertice[O->triInd[i].vertInd[1]].x,
					   O->Vertice[O->triInd[i].vertInd[1]].y,
					   O->Vertice[O->triInd[i].vertInd[1]].z);
			glVertex3f(O->Vertice[O->triInd[i].vertInd[2]].x,
					   O->Vertice[O->triInd[i].vertInd[2]].y,
					   O->Vertice[O->triInd[i].vertInd[2]].z);
		}
	} else {
		for(i=0; i<O->nTrian; i++) {
			glNormal3f(O->vNormal[O->triInd[i].vertInd[0]].x,
					   O->vNormal[O->triInd[i].vertInd[0]].y,
					   O->vNormal[O->triInd[i].vertInd[0]].z);
			glVertex3f(O->Vertice[O->triInd[i].vertInd[0]].x,
					   O->Vertice[O->triInd[i].vertInd[0]].y,
					   O->Vertice[O->triInd[i].vertInd[0]].z);

			glNormal3f(O->vNormal[O->triInd[i].vertInd[1]].x,
					   O->vNormal[O->triInd[i].vertInd[1]].y,
					   O->vNormal[O->triInd[i].vertInd[1]].z);
			glVertex3f(O->Vertice[O->triInd[i].vertInd[1]].x,
					   O->Vertice[O->triInd[i].vertInd[1]].y,
					   O->Vertice[O->triInd[i].vertInd[1]].z);

			glNormal3f(O->vNormal[O->triInd[i].vertInd[2]].x,
					   O->vNormal[O->triInd[i].vertInd[2]].y,
					   O->vNormal[O->triInd[i].vertInd[2]].z);
			glVertex3f(O->Vertice[O->triInd[i].vertInd[2]].x,
					   O->Vertice[O->triInd[i].vertInd[2]].y,
					   O->Vertice[O->triInd[i].vertInd[2]].z);		
		}
	}

	glEnd();

	SwapBuffers(hDC);	
}


