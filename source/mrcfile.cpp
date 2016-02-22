#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "mrcfile.h"
#include "MRCheader.h"

#pragma warning (disable:4996)

/**********************************************************************************/
/*	Funcion:			readAsUchar                                               */
/*	Descripcion:		Lee el archivo MRC con datos de 1 byte de longitud        */
/*	Parametros:	        fmt - Handle del archivo a leer                           */
/*	                    vol - Estructura en donde escribir los datos leidos.      */
/*	Regresa:			Cero en lectura correcta y diferente de cero si hubo error*/
/**********************************************************************************/

int readAsUchar(FILE *fmt, tVolumen *vol) {
	int i, r, size, step;
	u8 *data;

	size = vol->z * vol->rs;

	data = (u8 *)malloc(size);
	memset(data, 0, size);

	step = size / 100;
	if(step<=0)
		step = 1;

	for(i=0; i<size && !feof(fmt); ) {
		r = (int)fread(data+i, 1, size-i, fmt);
		if(!r) {
			printf("Hubo un error mientrás se leía el archivo: %d %d %d\n", ferror(fmt), i, size);
			return -1;
		}
		i+=r;

		if(i%step==0) {
			PrintStatus("Abriendo: %0.2f%%", 5.0f + (45.0f*i / (float)size));
		}
	}

	vol->data = (float *)malloc(vol->z * vol->rs * sizeof(float));
	memset(vol->data, 0, vol->z * vol->rs * sizeof(float));

	vol->min = vol->max = (float)data[0];

	for(i=0; i<size; i++) {
		vol->data[i] = (float)data[i];

		if(vol->data[i] < vol->min)
			vol->min = vol->data[i];
		
		if(vol->data[i] > vol->max)
			vol->max = vol->data[i];

		if(i%step==0) {
			PrintStatus("Abriendo: %0.2f%%", 50.0f + (50.0f*i / (float)size));
		}
	}

	free(data);

	return 0;
}

/**********************************************************************************/
/*	Funcion:			readAsShort                                               */
/*	Descripcion:		Lee el archivo MRC con datos de 2 bytes de longitud       */
/*	Parametros:	        fmt - Handle del archivo a leer                           */
/*	                    vol - Estructura en donde escribir los datos leidos.      */
/*	Regresa:			Cero en lectura correcta y diferente de cero si hubo error*/
/**********************************************************************************/

int readAsShort1(FILE *fmt, tVolumen *vol) {
	int i, size;
	short t;

	size = vol->z * vol->rs;

	vol->data = (float *)malloc(size * sizeof(float));
	memset(vol->data, 0, size * sizeof(float));

	size = 98 * vol->rs;

	for(i=0; i<size && !feof(fmt); i++) {
		fread(&t, 1, sizeof(short), fmt);
		vol->data[i] = (float)t;
	}

	return 0;
}

int readAsShort(FILE *fmt, tVolumen *vol) {
	int i, r, size, step;
	u8 *data;
	
	size = vol->z * vol->rs * sizeof(short);

	data = (u8 *)malloc(size);
	memset(data, 0, size);
	
	step = size / 100;
	if(step<=0)
		step = 1;

		
	for(i=0; i<size && !feof(fmt); ) {
		r = (int)fread(data+i, 1, size-i, fmt);
		if(!r) {
			printf("Hubo un error mientrás se leía el archivo: %d %d %d\n", ferror(fmt), i, size);
			return -1;
		}
		i+=r;

		if(i%step==0) {
			PrintStatus("Abriendo: %0.2f%%", 5.0f + (45.0f*i / (float)size));
		}
	}

	vol->data = (float *)malloc(vol->z * vol->rs * sizeof(float));
	memset(vol->data, 0, vol->z * vol->rs * sizeof(float));

	size = vol->z * vol->rs;

	vol->min = vol->max = (float)(*(short *)(&data[0]));

	step = size / 100;
	if(step<=0)
		step = 1;

	for(i=0, r=0; i<size; i++, r+=2) {
		vol->data[i] = (float)(*(short *)(&data[r]));

		if(vol->data[i] < vol->min)
			vol->min = vol->data[i];

		if(vol->data[i] > vol->max)
			vol->max = vol->data[i];

		if(i%step==0) {
			PrintStatus("Abriendo: %0.2f%%", 50.0f + (50.0f*i / (float)size));
		}
	}

	free(data);

	return 0;
}

/**********************************************************************************/
/*	Funcion:			LoadMRC                                                   */
/*	Descripcion:		Lee un archivo MRC                                        */
/*	Parametros:	        file - Cadena con el nombre del archivo a leer.           */
/*	                    vol  - Estructura en donde escribir los datos leidos.     */
/*	Regresa:			Cero en lectura correcta y diferente de cero si hubo error*/
/**********************************************************************************/

int LoadMRC(char *file, tVolumen *vol) {
	FILE *fmt;
	
	MRCheader mrc;

	if(vol==NULL) {
		printf("Llamada ilegal a LoadMRC\n");
		return -1;
	}

	fmt = fopen(file, "rb");
	if(!fmt) {
		printf("Archivo no encontrado: %s\n", file);
		return -1;
	}

	PrintStatus("Abriendo: %0.2f%%", 0.0f);

	memset(&mrc, 0, sizeof(MRCheader));

	fread(&mrc, 1, sizeof(MRCheader), fmt);

	PrintStatus("Abriendo: %0.2f%%", 5.0f);

	vol->x = mrc.nx;
	vol->y = mrc.ny;
	vol->z = mrc.nz;

	vol->rs = vol->x * vol->y;

	fseek(fmt, 1024, SEEK_SET);

	switch(mrc.mode) {
		case 0: /*  0 = unsigned char */ 
			if(readAsUchar(fmt, vol)) {
				fclose(fmt);
				return -1;
			}
			break;

		case 1: /*  1 = short */
			if(readAsShort(fmt, vol)) {
				fclose(fmt);
				return -1;
			}
			break;

		case 2: /*  2 = float */ 
		case 3: /*  3 = short * 2, (used for complex data) */ 
		case 4: /*  4 = float * 2, (used for complex data) */ 
		case 6: /*  6 = unsigned short, (used for complex data) */ 
		case 16: /* 16 = unsigned char * 3 (used for rgb data) */ 
			printf("Modo %d no soportado\n", mrc.mode);
			fclose(fmt);
			return -1;
			break;
		default:
			printf("Modo %d desconocido\n", mrc.mode);
			fclose(fmt);
			return -1;
	}


	fclose(fmt);

	PrintStatus("Abriendo: %0.2f%% (Modo %d - %d %d %d)", 100.0f, mrc.mode, vol->x, vol->y, vol->z);

	return 0;
}
