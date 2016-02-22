#ifndef GRAPHICSH
#define GRAPHICSH

#define EST_PLANO	0
#define EST_SUAVE	1
#define EST_MALLA	2

#define epsilon 0.0002f
#define figual(a,b) ((fabs(a-b)<=epsilon)? 1: 0)

typedef struct {
	float x, y, z;
}tVertex;

typedef struct {
	tVertex v[3]; //Vertices del triángulo
	tVertex n; //Normal del triángulo
}tTriangle;

typedef struct {
	int size, sizet;
	tVertex iniv;
	tTriangle *t;
}listTriangle;

typedef struct {
	int vertInd[3];	
}tITriang;

typedef struct {
	int			cNormal;
	int			nTrian;
	int			nVert, nVertt;
	tVertex		*Vertice;
	tITriang 	*triInd;
	tVertex		*fNormal;
	tVertex		*vNormal;
}tObject;

int  InitGraphics(HWND hWnd);
void EndGraphics();

void SetLightColor(int color);

void RenderObject(tObject *obj);

void DoNormals(tObject *O);

void AdjustRotation(tVertex R);

void AdjustZoom(float z);

void AdjustLightModel(int Est);

void SetQuickView(int view);

int ResizeGraphics(int width, int height);

#endif

