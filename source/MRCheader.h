/****************************************************************************
** Copyright (C) 2004 Edgar Garduno Angeles.  All rights reserved.
**
** File to define the header of the Medical Council Research files as of
**Jan 2004.
@author Edgar Garduno Angeles.
@version 1.0
@see
****************************************************************************/
#ifndef _MRCHEADER_H
#define _MRCHEADER_H

typedef struct __MRCheader{
    int     nx;	       /* Number of Columns */
    int     ny;        /* Number of Rows */
    int     nz;        /* Number of Sections. */
    
    int     mode;      /* Types of pixel in image.  Values used by imod:
			*  0 = unsigned char, 
			*  1 = short, 
			*  2 = float,  
			*  3 = short * 2, (used for complex data)
			*  4 = float * 2, (used for complex data)
			*  6 = unsigned short, (used for complex data)
			* 16 = unsigned char * 3 (used for rgb data)
		        */
    
    int     nxstart;     /* Starting point of sub image.   */
    int     nystart;
    int     nzstart;
    
    int     mx;         /* Grid size in X, Y, and Z        */
    int     my;
    int     mz;
    
    float   xlen;       /* Cell size in Angstroms; pixel spacing = xlen/mx */
    float   ylen;
    float   zlen;
    
    float   alpha;      /* cell angles in degrees */
    float   beta;
    float   gamma;
    
    /* Ignored by imod. */
    
    int     mapc;       /* map coloumn 1=x,2=y,3=z.        */
    int     mapr;       /* map row     1=x,2=y,3=z.        */
    int     maps;       /* map section 1=x,2=y,3=z.        */
    
    /* These need to be set for proper scaling of non byte data. */
    
    float   amin;       /* Minimum pixel value. */
    float   amax;       /* Maximum pixel value. */
    float   amean;      /* Mean pixel value. */
    
    short   ispg;       /* image type  */
    short   nsymbt;     /* space group number  */
    int     next;       /* number of bytes in extended header  */
    short   creatid;    /* Creator ID  */
    
    unsigned char unused[30];
    
    short   nint;       /* Number of bytes per section in extended header  */
    short   nreal;      /* Flags for which types of data from SerialEM (shorts) 
			   1 = tilt angle * 100  (2 bytes)
			   2 = piece coordinate  (6 bytes)
			   4 = Stage position    (4 bytes)
			   8 = Magnification / 100 (2 bytes)
			   16 = Reserved (image size?) (4 bytes)
			   If the number of bytes implied by these flags does
			   not add up to the value in nint, then nint and nreal
			   are interpreted as ints and floats per section
			*/
    
    unsigned char unused2[28];
    
    /* Explanation of type of data. */
    
    short   idtype; /* ( 0 = mono, 1 = tilt, 2 = tilts, 3 = lina, 4 = lins) */
    short   lens;
    short   nd1;	/* for idtype = 1, nd1 = axis (1, 2, or 3)      */
    short   nd2;
    short   vd1;                       /* vd1 = 100. * tilt increment */
    short   vd2;                       /* vd2 = 100. * starting angle */
    
    /* Used to rotate model to match new rotated image. */
    
    float   tiltangles[6];  /* 0,1,2 = original:  3,4,5 = current  */
    
    union{
       struct{ /* OLD-STYLE MRC HEADER - IMOD 2.6.19 and below: */
          short   nwave;     /* # of wavelengths and values */
	  short   wave1;
	  short   wave2;
	  short   wave3;
	  short   wave4;
	  short   wave5;
          
	  float   zorg;   /* Origin of image.  Used to auto translate model */
	  float   xorg;   /* to match a new image that has been translated. */
	  float   yorg;
	 }_OBSOLETE;
       
       struct{ /* NEW-STYLE MRC image2000 HEADER - IMOD 2.6.20 and above: */
          float   xorg;    /* Origin of image.  Used to auto translate model */
	  float   yorg;    /* to match a new image that has been translated. */
	  float   zorg;
          
	  char    cmap[4];    /* Contains "MAP " */
	  char    stamp[4];   /* First byte has 17 for big- or 68 for
	                         little-endian */
          float   rms;
	}_RECENT;
    }_VERSION;
    
    int     nlabl;  	/* Number of labels with useful data. */
    char    label[10][80];    	/* 10 labels of 80 charactors. */
}MRCheader;

#endif //_MRCHEADER_H

/*
ARRAY:		A storage location for data transfer. NOTE ARRAY is either
		REAL*4 or COMPLEX*8 !!!!!
		
ATBUTE:		File attribute specification. One of the following character
		strings must be given:  'NEW'  'OLD'  'RO'  or  'SCRATCH'

DMIN/DMAX/	The minimum, maximum, & mean density values for this image.
DMEAN:

EXTRA(25)	An array (up to 25 long) or a storage location for holding
		the values for the EXTRA info slot in the header.

IFORM:		-1 Old 20th century style map, 0 new 2000 format with
		machine stamp in header.

ISTART:		A number between 1-29 to select the first element of the
		EXTRA info to transfer.

ISTREAM/	A number between 1 and 12 used to select which file
JSTREAM:	to use for READ/WRITE operations. A maximum of 5
		files can be active at any one time.

IXYZMIN(3)/	Returned lower & upper pixel limits that correspond to user
IXYZMAX(3):	selected input range. (pixels have 0,0 at bottom left)

MODE:		Defines data structure on disk:
		0 = INTEGER*1	 IMAGE note:values between 127 & 255
                                       stored as -128 to -1 but returned
                                       as their original values.
		1 = INTEGER*2    IMAGE
		2 = REAL*4       IMAGE
		3 = INTEGER*2    FOURIER TRANSFORM
		4 = REAL*4       FOURIER TRANSFORM

MX/MY:		Dimensions of ARRAY. If transferred data size is smaller
		than MX,MY the remainder of the space will be set to 0.
		Note: MX correspons to the number of REAL numbers in the
		fast dimension of ARRAY. If MODE = 3 or 4 (Fourier
		Transform) MX MUST be multiplied by 2!!!

MXYZ(3):	Specifies the number of columns, rows, & sections in the
		entire "unit cell"  (can be identical to NXYZ).

LABELS(20,N)	N = 1 - 10. This is a way to initialize more than
		one text label when creating a new header. LABELS(20)
		is also permitted. Each label is 80 characters long (A4).

NAME:		A character string specifying either the FILENAME or
		a LOGICAL name to associate with a given stream.

NEXTRA:		A number bewteen 1-25 to select the number of EXTRA
		value to transfer.

NL:		The number of LABELS being initialized (see LABELS, above).

NTFLAG:		-1 no titles added to list.
		 0 to use TITLE as sole title on file (overwrites old title).
		 1 to add TITLE to end of list. 
		 2 to add at top, pushing any other labels down.

NX1/NX2:	Beginning & ending COLUMN numbers for data transfer.
		On reading these numbers are relative to NXTZST(1).
		For Fourier Transforms, these are taken as Complex Indicies!!!

NXYZ(3):	Specifies the number of columns, rows, & sections in the
		current file (ie. fastest -> slowest changing).

NXYZST(3):	Specifies the starting column, row and section number.
		Note, the NXYZST is for your use ONLY. It is stored in the
		file header for interfacing with the X-ray programs, but all
		calls within the image package, are always taken relative to
		the start of each line, or section. The start is ALWAYS
		denoted by 0.

NY:		Line number for positioning pointer. This value
		is relative to NXYST(2)

NY1/NY2:	Beginning & ending ROW    numbers for data transfer.
		On reading these numbers are relative to NXTZST(2).

NZ:		Section number for positioning pointer. This value
		is relative to NXYST(3)

TITLE(20)	80 character title. Written as dictated by NTFLAG.

XORIGIN/	The X,Y,Z image origin (generally used for relating
YORIGIN		phase origins). ZORIGIN set to zero for 2D images.
ZORIGIN:

*LINE		LINE is a statement number to branch to on an
		END-OF-FILE error. Example:   *99
*/
/*************************************************************************
*									*
*		    Some useful documentation for:			*
*									*
*	   IMAGE Analysis and file format Programs & Routines		*
*									*
*									*
*	   Last Update:		22-AUG-2000	         		*
*									*
*									*
************************************************************************
*                                                                       *
*                Map/Image Header Format				*
*									*
*        Length = 1024 bytes, organized as 56 LONG words followed	*
*	by space for 10 80 byte text labels.				*
*									*
*       1       NX      number of columns (fastest changing in map)     *
*       2       NY      number of rows                                  *
*       3       NZ      number of sections (slowest changing in map)    *
*       4       MODE    data type :                                     *
*                       0       image : signed 8-bit bytes range -128   *
*                                       to 127                          *
*                       1       image : 16-bit halfwords                *
*                       2       image : 32-bit reals                    *
*                       3       transform : complex 16-bit integers     *
*                       4       transform : complex 32-bit reals        *
*       5       NXSTART number of first column in map (Default = 0)	*
*       6       NYSTART number of first row in map       "              *
*       7       NZSTART number of first section in map   "              *
*       8       MX      number of intervals along X                     *
*       9       MY      number of intervals along Y                     *
*       10      MZ      number of intervals along Z                     *
*       11-13   CELLA   cell dimensions in angstroms                    *
*       14-16   CELLB   cell angles in degrees                          *
*       17      MAPC    axis corresp to cols (1,2,3 for X,Y,Z)          *
*       18      MAPR    axis corresp to rows (1,2,3 for X,Y,Z)          *
*       19      MAPS    axis corresp to sections (1,2,3 for X,Y,Z)      *
*       20      DMIN    minimum density value                           *
*       21      DMAX    maximum density value                           *
*       22      DMEAN   mean density value                              *
*       23      ISPG    space group number 0 or 1 (default=0)           *
*       24      NSYMBT  number of bytes used for symmetry data (0 or 80)*
*       25-49   EXTRA   extra space used for anything  - 0 by default   *
*       50-52   ORIGIN  origin in X,Y,Z used for transforms             *
*       53      MAP     character string 'MAP ' to identify file type   *
*       54      MACHST  machine stamp                                   *
*       55      RMS     rms deviation of map from mean density          *
*       56      NLABL   number of labels being used                     *
*       57-256  LABEL(20,10) 10 80-character text labels                *
*                                                                       *
*	Symmetry records follow - if any - stored as text as in 	*
*	International Tables, operators separated by * and grouped into *
*	'lines' of 80 characters (ie. symmetry operators do not cross 	*
*	the ends of the 80-character 'lines' and the 'lines' do not 	*
*	terminate in a *).						*
*                                                                       *
*       Data records follow.						*
*                                                                       *
*                                                                       *
************************************************************************

+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


		Higher-Level IMAGE File subroutines
		===================================

	These routine are designed to facilitate the rapid setup and
manipulation of image-format files. Either Sequential access or
Direct access I/O operations may be performed at any time.These routines
will now also cope with map-format files.
	In general, the X-Y origin is taken as 0,0 being in the
lower-left corner of the image AND the first data point in the file
(normally corresponding to array element 1,1).

			Top of Image		 1,NY	  Array    NX,NY
		^ !				   ^ !
		! !				   ! !
		! !				   ! !
		y !				   y !
		  !_____________   x-->              !_______________  x-->
		0,0				    1,1             NX,1




	The convention for FOURIER TRANSFORMS is that the transform of
an image of dimensions NX x NY yields a transform of NX/2+1 x NY
COMPLEX values. The origin of Fourier space is located at coordinates
0,NY/2 (disk coordinates, which would normally be 1,NY/2+1 in the FORTRAN
array). In reciprocal lattice units:
   X goes from 0 --> 0.5  and Y goes from -0.5 ---> (.5 - deltaY)
*/
