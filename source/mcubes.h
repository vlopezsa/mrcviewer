#ifndef MARCHINGCUBESH
#define MARCHINGCUBESH

/* Tablas obtenidas de http://astronomy.swin.edu.au/~pbourke/modelling/polygonise/ */
/* Autor: Paul Bourke*/

typedef struct {
   tVertex p[8];
   double val[8];
} GRIDCELL;

extern int edgeTable[256];

extern int triTable[256][16];

int MCCreateIsosurface(tVolumen *vol, tObject *obj, float thr);

#endif

