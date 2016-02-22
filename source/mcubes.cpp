#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"
#include "graphics.h"
#include "mcubes.h"

float Umbral = 3.0f;
float Ancho = 1.0f;

/**********************************************************************************/
/*	Funcion:			VertexInterp.                                             */
/*	Descripcion:        Apartir de dos puntos de una celda, encuentra el punto    */
/*	                    interpolado segun el valor del umbral (isonivel) y los    */
/*	                    valores del puntos del grid.                              */
/*	Parametros:	        isolevel - Isonivel o valor del umbral.                   */
/*	           	        p1 y p2  - Coordenadas del grid.                          */
/*	           	        valp1 y valp2 - Valores de los puntos del grid.           */
/*	Regresa:			Punto interpolado.                                        */
/**********************************************************************************/

tVertex VertexInterp(double isolevel, tVertex p1, tVertex p2, double valp1, double valp2) {
   double mu;
   tVertex p;

   if (fabs(isolevel-valp1) < 0.00001)
      return(p1);
   if (fabs(isolevel-valp2) < 0.00001)
      return(p2);
   if (fabs(valp1-valp2) < 0.00001)
      return(p1);
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = (float)(p1.x + mu * (p2.x - p1.x));
   p.y = (float)(p1.y + mu * (p2.y - p1.y));
   p.z = (float)(p1.z + mu * (p2.z - p1.z));

   return(p);
}

/**********************************************************************************/
/*	Funcion:			MCMoveGrid. .                                             */
/*	Descripcion:        Recorre un cubo del grid y obtiene su coordenadas asi     */
/*	                    como los valores que hay en cada punto.                   */
/*	Parametros:	        vol      - Volumen que se esta reconstruyendo.            */
/*	           	        x, y, z  - Coordenadas logicas del grid.                  */
/*	           	        fx, fy, fz- Ancho, alto y profundidad del cubo.           */
/*	Regresa:			Ninguno.                                                  */
/**********************************************************************************/
void MCMoveGrid(tVolumen *vol, GRIDCELL *g, int x, int y, int z,
									   float fx, float fy, float fz) {
	float fx1=fx+Ancho, fy1=fy+Ancho, fz1=fz+Ancho;
	int x1=x+1,y1=y+1,z1=z+1;
	int offset;

	g->p[0].x=fx;
	g->p[0].y=fy;
	g->p[0].z=fz;
	offset = z * vol->rs + y * vol->x + x;
	g->val[0]=vol->data[offset];

	g->p[1].x=fx1;
	g->p[1].y=fy;
	g->p[1].z=fz;
	offset = z * vol->rs + y * vol->x + x1;
	g->val[1]=vol->data[offset];

	g->p[2].x=fx1;
	g->p[2].y=fy;
	g->p[2].z=fz1;
	offset = z1 * vol->rs + y * vol->x + x1;
	g->val[2]=vol->data[offset];

	g->p[3].x=fx;
	g->p[3].y=fy;
	g->p[3].z=fz1;
	offset = z1 * vol->rs + y * vol->x + x;
	g->val[3]=vol->data[offset];

	g->p[4].x=fx;
	g->p[4].y=fy1;
	g->p[4].z=fz;
	offset = z * vol->rs + y1 * vol->x + x;
	g->val[4]=vol->data[offset];

	g->p[5].x=fx1;
	g->p[5].y=fy1;
	g->p[5].z=fz;
	offset = z * vol->rs + y1 * vol->x + x1;
	g->val[5]=vol->data[offset];

	g->p[6].x=fx1;
	g->p[6].y=fy1;
	g->p[6].z=fz1;
	offset = z1 * vol->rs + y1 * vol->x + x1;
	g->val[6]=vol->data[offset];

	g->p[7].x=fx;
	g->p[7].y=fy1;
	g->p[7].z=fz1;
	offset = z1 * vol->rs + y1 * vol->x + x;
	g->val[7]=vol->data[offset];
}

/**********************************************************************************/
/*	Funcion:			MCCheckGrid.                                              */
/*	Descripcion:        Verifica cuantos puntos estan dentro de o fuera del       */
/*	                    objeto definido por la isosuperficie y crea los           */
/*	                    triangulos necesarios.                                    */
/*	Parametros:	        g  - Cubo actual del grid.                                */
/*	           	        lT - lista de triangulos a la cual agregar los            */
/*	           	        triangulos creados.                                       */
/*	Regresa:			Cero si no se genero ningun triangulo y 1 si se generaron */
/**********************************************************************************/

int MCCheckGrid(GRIDCELL *g, listTriangle *lT) {
	int i;
	int cubeindex;
	tVertex vertlist[12];

	cubeindex = 0;

	if (g->val[0] > Umbral) cubeindex |= 1;
	if (g->val[1] > Umbral) cubeindex |= 2;
	if (g->val[2] > Umbral) cubeindex |= 4;
	if (g->val[3] > Umbral) cubeindex |= 8;
	if (g->val[4] > Umbral) cubeindex |= 16;
	if (g->val[5] > Umbral) cubeindex |= 32;
	if (g->val[6] > Umbral) cubeindex |= 64;
	if (g->val[7] > Umbral) cubeindex |= 128;

	if (edgeTable[cubeindex] == 0)
		return 0;
      
   if (edgeTable[cubeindex] & 1)
      vertlist[0] =
         VertexInterp(Umbral,g->p[0],g->p[1],g->val[0],g->val[1]);
   if (edgeTable[cubeindex] & 2)
      vertlist[1] =
         VertexInterp(Umbral,g->p[1],g->p[2],g->val[1],g->val[2]);
   if (edgeTable[cubeindex] & 4)
      vertlist[2] =
         VertexInterp(Umbral,g->p[2],g->p[3],g->val[2],g->val[3]);
   if (edgeTable[cubeindex] & 8)
      vertlist[3] =
         VertexInterp(Umbral,g->p[3],g->p[0],g->val[3],g->val[0]);
   if (edgeTable[cubeindex] & 16)
      vertlist[4] =
         VertexInterp(Umbral,g->p[4],g->p[5],g->val[4],g->val[5]);
   if (edgeTable[cubeindex] & 32)
      vertlist[5] =
         VertexInterp(Umbral,g->p[5],g->p[6],g->val[5],g->val[6]);
   if (edgeTable[cubeindex] & 64)
      vertlist[6] =
         VertexInterp(Umbral,g->p[6],g->p[7],g->val[6],g->val[7]);
   if (edgeTable[cubeindex] & 128)
      vertlist[7] =
         VertexInterp(Umbral,g->p[7],g->p[4],g->val[7],g->val[4]);
   if (edgeTable[cubeindex] & 256)
      vertlist[8] =
         VertexInterp(Umbral,g->p[0],g->p[4],g->val[0],g->val[4]);
   if (edgeTable[cubeindex] & 512)
      vertlist[9] =
         VertexInterp(Umbral,g->p[1],g->p[5],g->val[1],g->val[5]);
   if (edgeTable[cubeindex] & 1024)
      vertlist[10] =
         VertexInterp(Umbral,g->p[2],g->p[6],g->val[2],g->val[6]);
   if (edgeTable[cubeindex] & 2048)
      vertlist[11] =
         VertexInterp(Umbral,g->p[3],g->p[7],g->val[3],g->val[7]);

	for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
		if(lT->size+1 >= lT->sizet) {
			lT->sizet += lT->sizet/2;
        	lT->t = (tTriangle *)realloc(lT->t, lT->sizet*sizeof(tTriangle));
		}

		lT->t[lT->size].v[0] = vertlist[triTable[cubeindex][i  ]];
		lT->t[lT->size].v[1] = vertlist[triTable[cubeindex][i+1]];
		lT->t[lT->size].v[2] = vertlist[triTable[cubeindex][i+2]];

		lT->size = lT->size + 1;
   }

	return 1;
}

/**********************************************************************************/
/*	Funcion:			MCCreateTriangles                                         */
/*	Descripcion:        Recorrer todo el grid y genera los triangulos que         */
/*	                    formaran la isosuperficie.                                */
/*	Parametros:	        vol - Volumen a reconstruir.                              */
/*	           	        lT  - Lista de triangulos a crear.                        */
/*	Regresa:			Cero si se realizo con exito.                             */
/**********************************************************************************/

int MCCreateTriangles(tVolumen *vol, listTriangle *lT) {
	float x,y,z, per, fs;
	int i, j, k, s, size, step;
	GRIDCELL G;
	
	size = vol->z * vol->rs;

	step = size / 100;
	if(step<=0)
		step = 1;

	fs  = (80.0f*step / (float)size);
	per = 0.0f;
	s=0;
	
	lT->sizet = size * 3;

	lT->t = (tTriangle *)malloc((lT->sizet)*sizeof(tTriangle));
	
	if(lT->t==NULL) {
		lT->sizet = size;
		lT->t = (tTriangle *)malloc((lT->sizet)*sizeof(tTriangle));
	}

	for(i=0, y=(float)(-vol->y/2); i<vol->y-1; i++, y+=Ancho)
		for(j=0, x=(float)(-vol->x/2); j<vol->x-1; j++, x+=Ancho)
			for(k=0, z=(float)(-vol->z/2); k<vol->z-1; k++,z+=Ancho) {
			//for(k=0, z=(float)(-vol->z/2); k<95; k++,z+=Ancho) {
				MCMoveGrid(vol, &G, j, i, k, x, y, z);
				MCCheckGrid(&G, lT);

				if(k>97 && i>100 && j>80)
					k=k;

				s++;
				if(s>=step) {
					s=0;
					per+=fs;
					PrintStatus("Extracting Isosurface: %0.2f%%", per);
				}				
			}

	return 0;
}

/**********************************************************************************/
/*	Funcion:			MCFindVertex                                              */
/*	Descripcion:        Busque vertices en el objeto para que no repetir vertices.*/
/*	Parametros:	        V   - Vertice a agregar en el objeto.                     */
/*	           	        obj - objeto que se esta reconstruyendo                   */
/*	Regresa:			Indice de la posicion en donde se inserto el vertice.     */
/**********************************************************************************/

int MCFindVertex(tVertex V, tObject *obj) {
	int i, s;

	s = (obj->nVert>24)? obj->nVert-24: 0;

	for(i=s; i<obj->nVert; i++)
		if(obj->Vertice[i].x==V.x && obj->Vertice[i].y==V.y && obj->Vertice[i].z==V.z)
			return i;

	//i = obj->nVert;

	obj->nVert++;

	if(obj->nVert >= obj->nVertt) {
		obj->nVertt += obj->nVertt / 2;
		obj->Vertice = (tVertex *)realloc(obj->Vertice, obj->nVertt * sizeof(tVertex));
	}

	//obj->Vertice = (tVertex *)realloc(obj->Vertice, obj->nVert * sizeof(tVertex));

	obj->Vertice[i] = V;	

	return i;
}

/**********************************************************************************/
/*	Funcion:			MCCreateIsosurface                                        */
/*	Descripcion:        Crea la isosuperficie a un umbral dado.                   */
/*	Parametros:	        vol - volumen a reconstruir.                              */
/*	           	        obj - objeto donde se reconstruir el volumen              */
/*	           	        thr - umbral                                              */
/*	Regresa:			Cero si se realizo con exito.                             */
/**********************************************************************************/

int MCCreateIsosurface(tVolumen *vol, tObject *obj, float thr) {
	int i, step;
	listTriangle lT = {0};
	tVertex *tvb;

	lT.size = 0;
	lT.t = NULL;

	Umbral = thr;

	PrintStatus("Extracting Isosurface: %0.2f%%", 0.0f);

	MCCreateTriangles(vol, &lT);

	PrintStatus("Extracting Isosurface: %0.2f%%", 80.0f);

	if(obj->triInd)  free(obj->triInd);
	if(obj->Vertice) free(obj->Vertice);
	if(obj->fNormal) free(obj->fNormal);
	if(obj->vNormal) free(obj->vNormal);

	obj->cNormal = 0;
	obj->nVert   = 0;
	obj->nTrian  = lT.size;
	obj->nVertt  = lT.size*3;

	obj->triInd  = (tITriang *)  malloc(obj->nTrian * sizeof(tITriang));
	
	obj->Vertice = (tVertex *)malloc(obj->nVertt * sizeof(tVertex));

	if(obj->Vertice==NULL) {
		obj->nVertt  = lT.size;
		obj->Vertice = (tVertex *)malloc(obj->nVertt * sizeof(tVertex));
	}

	step = lT.size / 100;
	if(step<=0)
		step = 1;

	for(i=0; i<lT.size; i++) {
		obj->triInd[i].vertInd[0] = MCFindVertex(lT.t[i].v[0], obj);
		obj->triInd[i].vertInd[1] = MCFindVertex(lT.t[i].v[1], obj);
		obj->triInd[i].vertInd[2] = MCFindVertex(lT.t[i].v[2], obj);

		if(i%step==0) {
			PrintStatus("Extracting Isosurface: %0.2f%%", 80.0f + (15.0f*i / (float)lT.size));
		}
	}

	tvb = obj->Vertice;
	obj->Vertice = (tVertex *)malloc(obj->nVert * sizeof(tVertex));
	memcpy(obj->Vertice, tvb, obj->nVert * sizeof(tVertex));
	free(tvb);

	PrintStatus("Extracting Isosurface: %0.2f%%", 95.0f);

	if(lT.t) {
		free(lT.t);
		lT.t = NULL;
	}

	DoNormals(obj);

	PrintStatus("Extracting Isosurface: %0.2f%%", 100.0f);

	return 0;
}

int edgeTable[256]={
	0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
	0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
	0x190, 0x099, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
	0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
	0x230, 0x339, 0x033, 0x13a, 0x636, 0x73f, 0x435, 0x53c,
	0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
	0x3a0, 0x2a9, 0x1a3, 0x0aa, 0x7a6, 0x6af, 0x5a5, 0x4ac,
	0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
	0x460, 0x569, 0x663, 0x76a, 0x066, 0x16f, 0x265, 0x36c,
	0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
	0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0x0ff, 0x3f5, 0x2fc,
	0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
	0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x055, 0x15c,
	0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
	0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0x0cc,
	0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
	0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
	0x0cc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
	0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
	0x15c, 0x055, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
	0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
	0x2fc, 0x3f5, 0x0ff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
	0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
	0x36c, 0x265, 0x16f, 0x066, 0x76a, 0x663, 0x569, 0x460,
	0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
	0x4ac, 0x5a5, 0x6af, 0x7a6, 0x0aa, 0x1a3, 0x2a9, 0x3a0,
	0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
	0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x033, 0x339, 0x230,
	0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
	0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x099, 0x190,
	0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
	0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
};

int triTable[256][16] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  1,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  8,  3,  9,  8,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  3,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  2, 10,  0,  2,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  8,  3,  2, 10,  8, 10,  9,  8, -1, -1, -1, -1, -1, -1, -1},
	{ 3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 11,  2,  8, 11,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  9,  0,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1, 11,  2,  1,  9, 11,  9,  8, 11, -1, -1, -1, -1, -1, -1, -1},
	{ 3, 10,  1, 11, 10,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 10,  1,  0,  8, 10,  8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  9,  0,  3, 11,  9, 11, 10,  9, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  3,  0,  7,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  1,  9,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  1,  9,  4,  7,  1,  7,  3,  1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2, 10,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  4,  7,  3,  0,  4,  1,  2, 10, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  2, 10,  9,  0,  2,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1},
	{ 2, 10,  9,  2,  9,  7,  2,  7,  3,  7,  9,  4, -1, -1, -1, -1},
	{ 8,  4,  7,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11,  4,  7, 11,  2,  4,  2,  0,  4, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  0,  1,  8,  4,  7,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  7, 11,  9,  4, 11,  9, 11,  2,  9,  2,  1, -1, -1, -1, -1},
	{ 3, 10,  1,  3, 11, 10,  7,  8,  4, -1, -1, -1, -1, -1, -1, -1},
	{ 1, 11, 10,  1,  4, 11,  1,  0,  4,  7, 11,  4, -1, -1, -1, -1},
	{ 4,  7,  8,  9,  0, 11,  9, 11, 10, 11,  0,  3, -1, -1, -1, -1},
	{ 4,  7, 11,  4, 11,  9,  9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  5,  4,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  5,  4,  1,  5 , 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  5,  4,  8,  3 , 5,  3,  1,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2, 10,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  0,  8,  1,  2, 10,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 5,  2, 10,  5,  4,  2,  4,  0,  2, -1, -1, -1, -1, -1, -1, -1},
	{ 2, 10,  5,  3,  2,  5,  3,  5,  4,  3,  4,  8, -1, -1, -1, -1},
	{ 9,  5,  4,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0, 11,  2,  0,  8, 11,  4,  9,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  5,  4,  0,  1,  5,  2,  3, 11, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  1,  5,  2,  5,  8,  2,  8, 11,  4,  8,  5, -1, -1, -1, -1},
	{10,  3, 11, 10,  1,  3,  9,  5,  4, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  9,  5,  0,  8,  1,  8, 10 , 1,  8, 11, 10, -1, -1, -1, -1},
	{ 5,  4,  0,  5,  0, 11,  5, 11, 10, 11,  0,  3, -1, -1, -1, -1},
	{ 5,  4,  8,  5,  8, 10, 10,  8, 11, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  7,  8,  5,  7,  9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  3,  0,  9,  5,  3,  5,  7,  3, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  7,  8,  0,  1,  7,  1,  5,  7, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  7,  8,  9,  5,  7, 10,  1,  2, -1, -1, -1, -1, -1, -1, -1},
	{ 10, 1,  2,  9,  5,  0,  5,  3,  0,  5,  7,  3, -1, -1, -1, -1},
	{ 8,  0,  2,  8,  2,  5,  8,  5,  7, 10,  5,  2, -1, -1, -1, -1},
	{ 2, 10,  5,  2,  5,  3,  3,  5,  7, -1, -1, -1, -1, -1, -1, -1},
	{ 7,  9,  5,  7,  8,  9,  3, 11,  2, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  5,  7,  9,  7,  2,  9,  2,  0,  2,  7, 11, -1, -1, -1, -1},
	{ 2,  3, 11,  0,  1,  8,  1,  7,  8,  1,  5,  7, -1, -1, -1, -1},
	{11,  2,  1, 11,  1,  7,  7,  1,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  5,  8,  8,  5,  7, 10,  1,  3, 10,  3, 11, -1, -1, -1, -1},
	{ 5,  7,  0,  5,  0,  9,  7, 11,  0,  1,  0, 10, 11, 10,  0, -1},
	{11, 10,  0, 11,  0,  3, 10,  5,  0,  8,  0,  7,  5,  7,  0, -1},
	{11, 10,  5,  7, 11,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  3,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  0,  1,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  8,  3,  1,  9,  8,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  6,  5,  2,  6,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  6,  5,  1,  2,  6,  3,  0,  8, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  6,  5,  9,  0,  6,  0,  2,  6, -1, -1, -1, -1, -1, -1, -1},
	{ 5,  9,  8,  5,  8,  2,  5,  2,  6,  3,  2,  8, -1, -1, -1, -1},
	{ 2,  3, 11, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11,  0,  8, 11,  2,  0, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  1,  9,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1, -1, -1, -1},
	{ 5, 10,  6,  1,  9,  2,  9, 11,  2,  9,  8, 11, -1, -1, -1, -1},
	{ 6,  3, 11,  6,  5,  3,  5,  1,  3, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8, 11,  0, 11,  5,  0,  5,  1,  5, 11,  6, -1, -1, -1, -1},
	{ 3, 11,  6,  0,  3,  6,  0,  6,  5,  0,  5,  9, -1, -1, -1, -1},
	{ 6,  5,  9,  6,  9, 11, 11,  9,  8, -1, -1, -1, -1, -1, -1, -1},
	{ 5, 10,  6,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  3,  0,  4,  7,  3,  6,  5, 10, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  9,  0,  5, 10,  6,  8,  4,  7, -1, -1, -1, -1, -1, -1, -1},
	{10,  6,  5,  1,  9,  7,  1,  7,  3,  7,  9,  4, -1, -1, -1, -1},
	{ 6,  1,  2,  6,  5,  1,  4,  7,  8, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2,  5,  5,  2,  6,  3,  0,  4,  3,  4,  7, -1, -1, -1, -1},
	{ 8,  4,  7,  9,  0,  5,  0,  6,  5,  0,  2,  6, -1, -1, -1, -1},
	{ 7,  3,  9,  7,  9,  4,  3,  2,  9,  5,  9,  6,  2,  6,  9, -1},
	{ 3, 11,  2,  7,  8,  4, 10,  6,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 5, 10,  6,  4,  7,  2,  4,  2,  0,  2,  7, 11, -1, -1, -1, -1},
	{ 0,  1,  9,  4,  7,  8,  2,  3, 11,  5, 10,  6, -1, -1, -1, -1},
	{ 9,  2,  1,  9, 11,  2,  9,  4, 11,  7, 11,  4,  5, 10,  6, -1},
	{ 8,  4,  7,  3, 11,  5,  3,  5,  1,  5, 11,  6, -1, -1, -1, -1},
	{ 5,  1, 11,  5, 11,  6,  1,  0, 11,  7, 11,  4,  0,  4, 11, -1},
	{ 0,  5,  9,  0,  6,  5,  0,  3,  6, 11,  6,  3,  8,  4,  7, -1},
	{ 6,  5,  9,  6,  9, 11,  4,  7,  9,  7, 11,  9, -1, -1, -1, -1},
	{10,  4,  9,  6,  4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4, 10,  6,  4,  9, 10,  0,  8,  3, -1, -1, -1, -1, -1, -1, -1},
	{10,  0,  1, 10,  6,  0,  6,  4,  0, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  3,  1,  8,  1,  6,  8,  6,  4,  6,  1, 10, -1, -1, -1, -1},
	{ 1,  4,  9,  1,  2,  4,  2,  6,  4, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  0,  8,  1,  2,  9,  2,  4,  9,  2,  6,  4, -1, -1, -1, -1},
	{ 0,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  3,  2,  8,  2,  4,  4,  2,  6, -1, -1, -1, -1, -1, -1, -1},
	{10,  4,  9, 10,  6,  4, 11,  2,  3, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  2 , 2,  8, 11,  4,  9, 10,  4, 10,  6, -1, -1, -1, -1},
	{ 3, 11,  2 , 0,  1,  6,  0,  6,  4,  6,  1, 10, -1, -1, -1, -1},
	{ 6,  4,  1,  6,  1, 10,  4,  8,  1,  2,  1, 11,  8, 11,  1, -1},
	{ 9,  6,  4,  9,  3,  6,  9,  1,  3, 11,  6,  3, -1, -1, -1, -1},
	{ 8, 11,  1,  8,  1,  0, 11,  6,  1,  9,  1,  4,  6,  4,  1, -1},
	{ 3, 11,  6,  3,  6,  0,  0,  6,  4, -1, -1, -1, -1, -1, -1, -1},
	{ 6,  4,  8, 11,  6,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 7, 10,  6,  7,  8, 10,  8,  9, 10, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  7,  3,  0, 10,  7,  0,  9, 10,  6,  7, 10, -1, -1, -1, -1},
	{10,  6,  7,  1, 10,  7,  1,  7,  8,  1,  8,  0, -1, -1, -1, -1},
	{10,  6,  7, 10,  7,  1,  1,  7,  3, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2,  6,  1,  6,  8,  1,  8,  9,  8,  6,  7, -1, -1, -1, -1},
	{ 2,  6,  9,  2,  9,  1,  6,  7,  9,  0,  9,  3,  7,  3,  9, -1},
	{ 7,  8,  0,  7,  0,  6,  6,  0,  2, -1, -1, -1, -1, -1, -1, -1},
	{ 7,  3,  2,  6,  7,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  3, 11, 10,  6,  8, 10,  8,  9,  8,  6,  7, -1, -1, -1, -1},
	{ 2,  0,  7,  2,  7, 11,  0,  9,  7,  6,  7, 10,  9, 10,  7, -1},
	{ 1,  8,  0,  1,  7,  8,  1, 10,  7,  6,  7, 10,  2,  3, 11, -1},
	{11,  2,  1, 11,  1,  7, 10,  6,  1,  6,  7,  1, -1, -1, -1, -1},
	{ 8,  9,  6,  8,  6,  7,  9,  1,  6, 11,  6,  3,  1,  3,  6, -1},
	{ 0,  9,  1, 11,  6,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 7,  8,  0,  7,  0,  6,  3, 11,  0, 11,  6,  0, -1, -1, -1, -1},
	{ 7, 11,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  0,  8, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  1,  9, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  1,  9,  8,  3,  1, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1},
	{10,  1,  2,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2, 10,  3,  0,  8,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  9,  0,  2, 10,  9,  6, 11,  7, -1, -1, -1, -1, -1, -1, -1},
	{ 6, 11,  7,  2, 10,  3, 10,  8,  3, 10,  9,  8, -1, -1, -1, -1},
	{ 7,  2,  3,  6,  2,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 7,  0,  8,  7,  6,  0,  6,  2,  0, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  7,  6,  2,  3,  7,  0,  1,  9, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  6,  2,  1,  8,  6,  1,  9,  8,  8,  7,  6, -1, -1, -1, -1},
	{10,  7,  6, 10,  1,  7,  1,  3,  7, -1, -1, -1, -1, -1, -1, -1},
	{10,  7,  6,  1,  7, 10,  1,  8,  7,  1,  0,  8, -1, -1, -1, -1},
	{ 0,  3,  7,  0,  7, 10,  0, 10,  9,  6, 10,  7, -1, -1, -1, -1},
	{ 7,  6, 10,  7, 10,  8,  8, 10,  9, -1, -1, -1, -1, -1, -1, -1},
	{ 6,  8,  4, 11,  8,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  6, 11,  3,  0,  6,  0,  4,  6, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  6, 11,  8,  4,  6,  9,  0,  1, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  4,  6,  9,  6,  3,  9,  3,  1, 11,  3,  6, -1, -1, -1, -1},
	{ 6,  8,  4,  6, 11,  8,  2, 10,  1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2, 10,  3,  0, 11,  0,  6, 11,  0,  4,  6, -1, -1, -1, -1},
	{ 4, 11,  8,  4 , 6, 11,  0,  2,  9,  2, 10,  9, -1, -1, -1, -1},
	{10,  9,  3, 10 , 3,  2,  9,  4,  3, 11,  3,  6,  4,  6,  3, -1},
	{ 8,  2,  3,  8 , 4,  2,  4,  6,  2, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  4,  2,  4 , 6,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  9,  0,  2 , 3,  4,  2,  4,  6,  4,  3,  8, -1, -1, -1, -1},
	{ 1,  9,  4,  1,  4,  2,  2,  4,  6, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  1,  3,  8,  6,  1,  8,  4,  6,  6, 10,  1, -1, -1, -1, -1},
	{10,  1,  0, 10,  0,  6,  6,  0,  4, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  6,  3,  4,  3,  8,  6, 10,  3,  0,  3,  9, 10,  9,  3, -1},
	{10,  9,  4,  6, 10,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  9,  5,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  3,  4 , 9,  5, 11,  7,  6, -1, -1, -1, -1, -1, -1, -1},
	{ 5,  0,  1,  5 , 4,  0,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11,  7,  6,  8,  3,  4,  3,  5,  4,  3,  1,  5, -1, -1, -1, -1},
	{ 9,  5,  4, 10,  1,  2,  7,  6, 11, -1, -1, -1, -1, -1, -1, -1},
	{ 6, 11,  7,  1,  2, 10,  0,  8,  3,  4,  9,  5, -1, -1, -1, -1},
	{ 7,  6, 11,  5,  4, 10,  4,  2, 10,  4,  0,  2, -1, -1, -1, -1},
	{ 3,  4,  8,  3,  5,  4,  3,  2,  5, 10,  5,  2, 11,  7,  6, -1},
	{ 7,  2,  3,  7,  6,  2,  5,  4,  9, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  5,  4,  0,  8,  6,  0,  6,  2,  6,  8,  7, -1, -1, -1, -1},
	{ 3,  6,  2,  3,  7,  6,  1,  5,  0,  5,  4,  0, -1, -1, -1, -1},
	{ 6,  2,  8,  6,  8,  7,  2,  1,  8,  4,  8,  5,  1,  5,  8, -1},
	{ 9,  5,  4, 10,  1,  6,  1,  7,  6,  1,  3,  7, -1, -1, -1, -1},
	{ 1,  6, 10,  1,  7,  6,  1,  0,  7,  8,  7,  0,  9,  5,  4, -1},
	{ 4,  0, 10,  4, 10,  5,  0,  3, 10,  6, 10,  7,  3,  7, 10, -1},
	{ 7,  6, 10,  7, 10,  8,  5,  4, 10,  4,  8, 10, -1, -1, -1, -1},
	{ 6,  9,  5,  6, 11,  9, 11,  8,  9, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  6, 11,  0,  6,  3,  0,  5,  6,  0,  9,  5, -1, -1, -1, -1},
	{ 0, 11,  8,  0,  5, 11,  0,  1,  5,  5,  6, 11, -1, -1, -1, -1},
	{ 6, 11,  3,  6,  3,  5,  5,  3,  1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2, 10,  9,  5, 11,  9, 11,  8, 11,  5,  6, -1, -1, -1, -1},
	{ 0, 11,  3,  0,  6, 11,  0,  9,  6,  5,  6,  9,  1,  2, 10, -1},
	{11,  8,  5, 11,  5,  6,  8,  0,  5, 10,  5,  2,  0,  2,  5, -1},
	{ 6, 11,  3,  6,  3,  5,  2, 10,  3, 10,  5,  3, -1, -1, -1, -1},
	{ 5,  8,  9,  5,  2,  8,  5,  6,  2,  3,  8,  2, -1, -1, -1, -1},
	{ 9,  5,  6,  9,  6,  0,  0,  6,  2, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  5,  8,  1 , 8,  0,  5,  6,  8,  3,  8,  2,  6,  2,  8, -1},
	{ 1,  5,  6,  2 , 1,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  3,  6,  1,  6, 10,  3,  8,  6,  5,  6,  9,  8,  9,  6, -1},
	{10,  1,  0, 10,  0,  6,  9,  5,  0,  5,  6,  0, -1, -1, -1, -1},
	{ 0,  3,  8,  5,  6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10,  5,  6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11,  5, 10,  7,  5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11,  5, 10, 11,  7,  5,  8,  3,  0, -1, -1, -1, -1, -1, -1, -1},
	{ 5, 11,  7,  5, 10, 11,  1,  9,  0, -1, -1, -1, -1, -1, -1, -1},
	{10,  7,  5, 10, 11,  7,  9,  8,  1,  8,  3,  1, -1, -1, -1, -1},
	{11,  1,  2, 11,  7,  1,  7,  5,  1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  3,  1,  2,  7,  1,  7,  5,  7,  2, 11, -1, -1, -1, -1},
	{ 9,  7,  5,  9,  2,  7,  9,  0,  2,  2, 11,  7, -1, -1, -1, -1},
	{ 7,  5,  2,  7,  2, 11,  5,  9,  2,  3,  2,  8,  9,  8,  2, -1},
	{ 2,  5, 10,  2,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  2,  0,  8,  5,  2,  8,  7,  5, 10,  2,  5, -1, -1, -1, -1},
	{ 9,  0,  1,  5, 10,  3,  5,  3,  7,  3, 10,  2, -1, -1, -1, -1},
	{ 9,  8,  2,  9,  2,  1,  8,  7,  2, 10,  2,  5,  7,  5,  2, -1},
	{ 1,  3,  5,  3,  7,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  7,  0,  7,  1,  1,  7,  5, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  0,  3,  9,  3,  5,  5,  3,  7, -1, -1, -1, -1, -1, -1, -1},
	{ 9,  8,  7,  5,  9,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 5,  8,  4,  5, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1},
	{ 5,  0,  4,  5, 11,  0,  5, 10, 11, 11,  3,  0, -1, -1, -1, -1},
	{ 0,  1,  9,  8,  4, 10,  8, 10, 11, 10,  4,  5, -1, -1, -1, -1},
	{10, 11,  4, 10,  4,  5, 11,  3,  4,  9,  4,  1,  3,  1,  4, -1},
	{ 2,  5,  1,  2,  8,  5,  2, 11,  8,  4,  5,  8, -1, -1, -1, -1},
	{ 0,  4, 11,  0, 11,  3,  4,  5, 11,  2, 11,  1,  5,  1, 11, -1},
	{ 0,  2,  5,  0,  5,  9,  2, 11,  5,  4,  5,  8, 11,  8,  5, -1},
	{ 9,  4,  5,  2, 11,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  5, 10,  3,  5,  2,  3,  4,  5,  3,  8,  4, -1, -1, -1, -1},
	{ 5, 10,  2,  5,  2,  4,  4,  2,  0, -1, -1, -1, -1, -1, -1, -1},
	{ 3, 10,  2,  3,  5, 10,  3,  8,  5,  4,  5,  8,  0,  1,  9, -1},
	{ 5, 10,  2,  5,  2,  4,  1,  9,  2,  9,  4,  2, -1, -1, -1, -1},
	{ 8,  4,  5,  8,  5,  3,  3,  5,  1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  4,  5,  1,  0,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 8,  4,  5,  8,  5,  3,  9,  0,  5,  0,  3,  5, -1, -1, -1, -1},
	{ 9,  4,  5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4, 11,  7,  4,  9, 11,  9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  8,  3,  4,  9,  7,  9, 11,  7,  9, 10, 11, -1, -1, -1, -1},
	{ 1, 10, 11,  1, 11,  4,  1,  4,  0,  7,  4, 11, -1, -1, -1, -1},
	{ 3,  1,  4,  3,  4,  8,  1, 10,  4,  7,  4, 11, 10, 11,  4, -1},
	{ 4, 11,  7,  9, 11,  4,  9,  2, 11,  9,  1,  2, -1, -1, -1, -1},
	{ 9,  7,  4,  9, 11,  7,  9,  1, 11,  2, 11,  1,  0,  8,  3, -1},
	{11,  7,  4, 11,  4,  2,  2,  4,  0, -1, -1, -1, -1, -1, -1, -1},
	{11,  7,  4, 11,  4,  2,  8,  3,  4,  3,  2,  4, -1, -1, -1, -1},
	{ 2,  9, 10,  2,  7,  9,  2,  3,  7,  7,  4,  9, -1, -1, -1, -1},
	{ 9, 10,  7,  9,  7,  4, 10,  2,  7,  8,  7,  0,  2,  0,  7, -1},
	{ 3,  7, 10,  3, 10,  2,  7,  4, 10,  1, 10,  0,  4,  0, 10, -1},
	{ 1, 10,  2,  8,  7,  4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  9,  1,  4,  1,  7,  7,  1,  3, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  9,  1,  4,  1,  7,  0,  8,  1,  8,  7,  1, -1, -1, -1, -1},
	{ 4,  0,  3,  7,  4,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 4,  8,  7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 9, 10,  8, 10, 11,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  0,  9,  3,  9, 11, 11,  9, 10, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  1, 10,  0, 10,  8,  8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  1, 10, 11,  3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  2, 11,  1, 11,  9,  9, 11,  8, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  0,  9,  3,  9, 11,  1,  2,  9,  2, 11,  9, -1, -1, -1, -1},
	{ 0,  2, 11,  8,  0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 3,  2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  3,  8,  2,  8, 10, 10,  8,  9, -1, -1, -1, -1, -1, -1, -1},
	{ 9, 10,  2,  0,  9,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 2,  3,  8,  2,  8, 10,  0,  1 , 8,  1, 10,  8, -1, -1, -1, -1},
	{ 1, 10,  2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 1,  3,  8,  9,  1,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  9,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{ 0,  3,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};
