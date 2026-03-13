/******************************************************************************
 * Copyright 2000 by mental images GmbH & Co.KG, Fasanenstr. 81, D-10623 Berlin
 * All rights reserved.
 ******************************************************************************
 * Author:	thomas
 * Created:	03.02.2000
 * Purpose:	standard header file for all mental ray shaders
 *
 * Note: This file is generated automatically from the mental ray sources, do
 * not edit. Some definitions exist for internal reasons and are subject to
 * change between mental ray versions. See the mental ray user's manual for
 * a definitive description of the shader interface. Avoid using features not
 * documented in the manual because they may change in future versions.
 *****************************************************************************/

#include <stdio.h>
#include <math.h>

#ifndef SHADER_H
#define SHADER_H

#define RAY2			/* for shaders that share sources with 1.9 */

#ifdef __cplusplus
extern "C" {
#endif


/*------------ mi_raylib.h ------------*/

typedef enum {miFALSE=0, miTRUE=1}	miBoolean;
typedef unsigned char			miCBoolean;	/* if space is tight */
typedef int				miInteger;	/* for shader params */
typedef signed char			miSint1;
typedef unsigned char			miUint1;
typedef signed short			miSint2;
typedef unsigned short			miUint2;
typedef signed int			miSint4;
typedef unsigned int			miUint4;
typedef unsigned int			miUint;
typedef unsigned char			miUchar;
typedef unsigned short			miUshort;
typedef unsigned int			miUlong;
typedef signed char			miSchar;
typedef float				miScalar;
typedef double				miGeoScalar;
typedef struct {miScalar u, v;}		miVector2d;
typedef struct {miScalar x, y, z;}	miVector;
typedef struct {miGeoScalar u, v;}	miGeoVector2d;
typedef struct {miGeoScalar x, y, z;}	miGeoVector;
typedef struct {miScalar x, y, z, w;}	miQuaternion;
typedef miScalar			miMatrix[16];
typedef struct {float r, g, b, a;}	miColor;
typedef struct {miScalar min, max;}	miRange;
typedef struct {miGeoScalar min, max;}	miGeoRange;
typedef unsigned short			miIndex;
typedef unsigned int			miGeoIndex;
typedef miBoolean			(*miFunction_ptr)();
typedef union {void *p; double d;}	miPointer;	/* 8-byte aligned ptr*/
typedef miSint4				miContext;
#if defined(WIN_NT) && defined(BIT64)			/* int wide enough   */
typedef __int64				miIntptr;	/* for a pointer. Get*/
#else							/* this: 64-bit NT   */
typedef unsigned long			miIntptr;	/* will have 32-bit  */
#endif							/* longs! Losers...  */

#define miSCALAR_EPSILON		(1e-5)
#define miGEO_SCALAR_EPSILON		(1e-9)
#define miHUGE_SCALAR			(1e36)	/* don't require <limits.h> */
#define miHUGE_INT			(2147483647)
#define miNULL_INDEX			(0xffff) /* null value for miIndex */
#define miMAX_INDEX			(0xfffe) /* largest valid miIndex */
#define miNULL_GEOINDEX			(0xffffffff) /* null miGeoIndex */
#define miMAX_GEOINDEX			(0xfffffffe) /* largest miGeoIndex */

typedef enum {
	miM_NULL = 0,		/* not a legal module, wildcard value */
	miM_MSG,		/* message passing */
	miM_PAR,		/* message routing and event loops */
	miM_MEM,		/* memory manager */
	miM_DB,			/* database module */
	miM_PHEN,		/* phenomenon database */
	miM_TASK,		/* task queue manager */
	miM_SCENE,		/* common scene operations */
	miM_RC,			/* rendering core */
	miM_GAPMI,		/* geometry approximation (GAP) */
	miM_IMG,		/* image file read/write */
	miM_ECHO,		/* scene echo to .mi file */
	miM_MI,			/* mi file translator */
	miM_CAT,		/* Catia translator */
	miM_SOFT,		/* unused */
	miM_ALIAS,		/* unused */
	miM_WAVE,		/* unused */
	miM_GEOSI,		/* surface / surface intersection */
	miM_TEST,		/* not used in raylib, for test beds only */
	miM_LINK,		/* dynamical linking, function symbol table */
	miM_WIRE,		/* unused */
	miM_DISP,		/* realtime image displayer */
	miM_DSR,		/* unused */
	miM_JOB,		/* job manager (ray 3.0) */
	miM_MAIN,		/* all front-ends with main() */
	miM_GEOMI,		/* geometry service library (GEO) */
	miM_MATTER,		/* unused */
	miM_STUDIO,		/* unused */
	miM_INVENTOR,		/* SGI Inventor translator */
	miM_LIB,		/* library module, error messages */
	miM_API,		/* high-level app programming interface */
	miM_SPDL,		/* Softimage SPDL phenomenon parser */
	miM_PHCR,		/* shader editor GUI */
	miM_RCB,		/* RC boxing */
	miM_RCC,		/* RC contours */
	miM_RCI,		/* RC intersection */
	miM_RCGI,		/* RC global illumination and caustics */
	miM_RCS,		/* RC scheduler */
	miM_GAPS,		/* GAP scheduler */
	miM_SURF,		/* subdivision surfaces */
	miM_CAPI,		/* class api module */
	miM_GAPAD,		/* GAP adjacency detection and edge merging */
	miM_GAPCU,		/* GAP curves */
	miM_GAPDI,		/* GAP displacement mapping */
	miM_GAPIS,		/* GAP isoline segments */
	miM_GAPPO,		/* GAP polygons */
	miM_GAPFA,		/* GAP faces */
	miM_GAPTE,		/* GAP tessellation */
	miM_GEOCO,		/* GEO conditioning */
	miM_GEOEV,		/* GEO evaluation */
	miM_GEOMO,		/* GEO modelling */
	miM_GEOST,		/* GEO strips */
	miM_GEOTE,		/* GEO tessellation */
	miM_JPG,		/* JPEG compression/decompression */
	miNMODULES		/* # of modules (not a module itself) */
} miModule;
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2		1.57079632679489661923
#endif
#ifndef M_SQRT1_2
#define M_SQRT1_2	0.70710678118654752440
#endif
#define RAY2			/* for shaders that share sources with 1.9 */
#ifdef _WIN32
#define DLLIMPORT __declspec(dllexport)
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLIMPORT
#define DLLEXPORT
#endif

/*------------ mi_lib.h ------------*/

/*
 * vector operations. MI_SQRT works for ANSI C compilers. If it causes trouble,
 * #define MI_SQRT sqrt before including this file. IBM and HP mess up as usual.
 */

#ifndef MI_SQRT
#ifdef IRIX
#define MI_SQRT sqrtf
#else
#define MI_SQRT sqrt
#endif
#endif

#define mi_vector_neg(r)	((r)->x = -(r)->x,\
				 (r)->y = -(r)->y,\
				 (r)->z = -(r)->z)

#define mi_vector_add(r,a,b)	((r)->x = (a)->x + (b)->x,\
				 (r)->y = (a)->y + (b)->y,\
				 (r)->z = (a)->z + (b)->z)

#define mi_vector_sub(r,a,b)	((r)->x = (a)->x - (b)->x,\
				 (r)->y = (a)->y - (b)->y,\
				 (r)->z = (a)->z - (b)->z)

#define mi_vector_mul(r,f)	((r)->x *= f,\
				 (r)->y *= f,\
				 (r)->z *= f)

#define mi_vector_div(r,d)	do {if (d != 0.0f)\
						{register miScalar _i=1.0f/d;\
						 (r)->x *= _i;\
						 (r)->y *= _i;\
						 (r)->z *= _i; }} while (0)

#define mi_vector_div_d(r,d)	do {if (d != 0.0)\
						{register double _i = 1.0/d;\
						 (r)->x *= _i;\
						 (r)->y *= _i;\
						 (r)->z *= _i; }} while (0)

#define mi_vector_prod(r,a,b)	((r)->x = (a)->y * (b)->z - (a)->z * (b)->y,\
				 (r)->y = (a)->z * (b)->x - (a)->x * (b)->z,\
				 (r)->z = (a)->x * (b)->y - (a)->y * (b)->x)

#define mi_vector_dot(a,b)	((a)->x * (b)->x +\
				 (a)->y * (b)->y +\
				 (a)->z * (b)->z)

#define mi_vector_dot_d(a,b)	((double)(a)->x * (double)(b)->x +\
				 (double)(a)->y * (double)(b)->y +\
				 (double)(a)->z * (double)(b)->z)

#define mi_vector_norm(r)	(miScalar)MI_SQRT((r)->x * (r)->x +\
					(r)->y * (r)->y +\
					(r)->z * (r)->z)

#define mi_vector_norm_d(r)	sqrt((double)(r)->x * (double)(r)->x +\
				     (double)(r)->y * (double)(r)->y +\
				     (double)(r)->z * (double)(r)->z)

#define mi_vector_normalize(r)	do { register miScalar _n = mi_vector_norm(r);\
				     mi_vector_div(r, _n); } while (0)

#define mi_vector_normalize_d(r) do { register double _n= mi_vector_norm_d(r);\
				     mi_vector_div_d(r, _n); } while (0)

#define mi_vector_min(r,a,b)	((a)->x < (b)->x ? ((r)->x = (a)->x) \
						 : ((r)->x = (b)->x),\
				 (a)->y < (b)->y ? ((r)->y = (a)->y) \
						 : ((r)->y = (b)->y),\
				 (a)->z < (b)->z ? ((r)->z = (a)->z) \
						 : ((r)->z = (b)->z))

#define mi_vector_max(r,a,b)	((a)->x > (b)->x ? ((r)->x = (a)->x) \
						 : ((r)->x = (b)->x),\
				 (a)->y > (b)->y ? ((r)->y = (a)->y) \
						 : ((r)->y = (b)->y),\
				 (a)->z > (b)->z ? ((r)->z = (a)->z) \
						 : ((r)->z = (b)->z))

#define mi_vector_det(a,b,c)	((a)->x * ((b)->y * (c)->z - (c)->y * (b)->z)+\
				 (b)->x * ((c)->y * (a)->z - (a)->y * (c)->z)+\
				 (c)->x * ((a)->y * (b)->z - (b)->y * (a)->z))

#define mi_vector_dist(a,b)	MI_SQRT(((a)->x - (b)->x) * ((a)->x - (b)->x)+\
					((a)->y - (b)->y) * ((a)->y - (b)->y)+\
					((a)->z - (b)->z) * ((a)->z - (b)->z))


/*
 * matrix operations (libmatrix.c)
 */

#define	  mi_matrix_copy(t,s)		(void)memcpy(t, s, sizeof(miMatrix))
#define	  mi_matrix_null(m)		(void)memset(m, 0, sizeof(miMatrix))

DLLIMPORT void	    mi_matrix_ident	(miMatrix);
DLLIMPORT void	    mi_matrix_prod	(miMatrix,
					 miMatrix const,
					 miMatrix const);
DLLIMPORT miBoolean mi_matrix_isident	(miMatrix const);
DLLIMPORT miBoolean mi_matrix_isnull	(miMatrix const);
DLLIMPORT float	    mi_matrix_rot_det	(miMatrix const);
DLLIMPORT miBoolean mi_matrix_invert	(miMatrix, miMatrix const);
DLLIMPORT void	    mi_point_transform	(miVector * const,
					 miVector const * const,
					 miMatrix const);
DLLIMPORT void	    mi_vector_transform	(miVector * const,
					 miVector const * const,
					 miMatrix const);
DLLIMPORT void	    mi_vector_transform_T(miVector * const,
					 miVector const * const,
					 miMatrix const);
DLLIMPORT void	    mi_matrix_rotate	(miMatrix, const miScalar,
					 const miScalar, const miScalar);
DLLIMPORT void	    mi_matrix_rotate_axis(miMatrix, miVector const * const,
					 miScalar const);

DLLIMPORT miScalar  mi_spline		(miScalar, const int, miScalar *const);
DLLIMPORT miScalar  mi_random		(void);
DLLIMPORT miScalar  mi_erandom		(unsigned short seed[3]);
DLLIMPORT void	    mi_srandom		(long newseed);
DLLIMPORT miScalar  mi_noise_1d		(const miScalar);
DLLIMPORT miScalar  mi_unoise_1d	(const miScalar);
DLLIMPORT miScalar  mi_noise_2d		(const miScalar, const miScalar);
DLLIMPORT miScalar  mi_unoise_2d	(const miScalar, const miScalar);
DLLIMPORT miScalar  mi_noise_3d		(miVector * const);
DLLIMPORT miScalar  mi_unoise_3d	(miVector * const);
DLLIMPORT miScalar  mi_noise_1d_grad	(const miScalar, miScalar * const);
DLLIMPORT miScalar  mi_unoise_1d_grad	(const miScalar, miScalar * const);
DLLIMPORT miScalar  mi_noise_2d_grad	(const miScalar, const miScalar,
					 miScalar * const, miScalar * const);
DLLIMPORT miScalar  mi_unoise_2d_grad	(const miScalar, const miScalar,
					 miScalar * const, miScalar * const);
DLLIMPORT miScalar  mi_noise_3d_grad	(miVector * const, miVector * const);
DLLIMPORT miScalar  mi_unoise_3d_grad	(miVector * const, miVector * const);


/*
 * error handler: severity levels for mi_set_verbosity (miERR_*), and
 * sub-levels to turn on individual message types per module (miMSG_*).
 */

DLLIMPORT void	mi_fatal		(const char * const, ...);
DLLIMPORT void	mi_error		(const char * const, ...);
DLLIMPORT void	mi_warning		(const char * const, ...);
DLLIMPORT void	mi_info			(const char * const, ...);
DLLIMPORT void	mi_progress		(const char * const, ...);
DLLIMPORT void	mi_debug		(const char * const, ...);
DLLIMPORT void	mi_vdebug		(const char * const, ...);
DLLIMPORT void	mi_nfatal		(const int, const char * const, ...);
DLLIMPORT void	mi_nerror		(const int, const char * const, ...);
DLLIMPORT void	mi_nwarning		(const int, const char * const, ...);
DLLIMPORT char	*mi_strerror		(void);

DLLEXPORT char *mi_string_substitute(	/* apply dos/unix etc path substns */
	char	*new_name,		/* place to copy mangled path */
	char	*name,			/* path to be mangled */
	long	max_new_name_len);	/* size of new_name[] */
typedef enum {
	miQ_NONE			= 0,
	miQ_VERSION			= 1,
	miQ_DATE			= 2,
	miQ_INST_FUNCTION		= 3,
	miQ_INST_GLOBAL_TO_LOCAL	= 4,
	miQ_INST_LOCAL_TO_GLOBAL	= 5,
	miQ_INST_IDENTITY		= 6,
	miQ_INST_ITEM			= 7,
	miQ_INST_PARENT			= 8,
	miQ_INST_HIDE			= 9,
	miQ_INST_VISIBLE		= 90,
	miQ_INST_SHADOW			= 91,
	miQ_INST_TRACE			= 92,
	miQ_INST_CAUSTIC		= 93,
	miQ_INST_GLOBILLUM		= 111,
	miQ_INST_DECL			= 10,
	miQ_INST_PARAM_SIZE		= 11,
	miQ_INST_PARAM			= 12,
	miQ_INST_MATERIAL		= 83,
	miQ_INST_LABEL			= 101,
	miQ_INST_DATA			= 102,
	miQ_GROUP_MERGE_GROUP		= 13,
	miQ_GROUP_NKIDS			= 14,
	miQ_GROUP_KID			= 15,
	miQ_GROUP_LABEL			= 103,
	miQ_GROUP_DATA			= 104,
	miQ_OBJ_TYPE			= 16,
	miQ_OBJ_VISIBLE			= 17,
	miQ_OBJ_TRACE			= 18,
	miQ_OBJ_SHADOW			= 19,
	miQ_OBJ_VIEW_DEPENDENT		= 20,
	miQ_OBJ_CAUSTIC			= 21,
	miQ_OBJ_GLOBILLUM		= 112,
	miQ_OBJ_LABEL			= 22,
	miQ_OBJ_DATA			= 100,
	miQ_LIGHT_TYPE			= 23,
	miQ_LIGHT_AREA			= 24,
	miQ_LIGHT_EXPONENT		= 25,
	miQ_LIGHT_CAUSTIC_PHOTONS	= 26,
	miQ_LIGHT_GLOBAL_PHOTONS	= 27,
	miQ_LIGHT_ENERGY		= 28,
	miQ_LIGHT_SHADER		= 29,
	miQ_LIGHT_EMITTER		= 30,
	miQ_LIGHT_ORIGIN		= 31,
	miQ_LIGHT_DIRECTION		= 32,
	miQ_LIGHT_AREA_R_EDGE_U		= 33,
	miQ_LIGHT_AREA_R_EDGE_V		= 34,
	miQ_LIGHT_AREA_D_NORMAL		= 35,
	miQ_LIGHT_AREA_D_RADIUS		= 36,
	miQ_LIGHT_AREA_S_RADIUS		= 37,
	miQ_LIGHT_AREA_C_RADIUS		= 96,
	miQ_LIGHT_AREA_C_AXIS		= 97,
	miQ_LIGHT_AREA_SAMPLES_U	= 38,
	miQ_LIGHT_AREA_SAMPLES_V	= 39,
	miQ_LIGHT_SPREAD		= 40,
	miQ_LIGHT_USE_SHADOWMAP		= 41,
	miQ_LIGHT_LABEL			= 98,
	miQ_LIGHT_DATA			= 99,
	miQ_MTL_OPAQUE			= 42,
	miQ_MTL_SHADER			= 43,
	miQ_MTL_DISPLACE		= 44,
	miQ_MTL_SHADOW			= 45,
	miQ_MTL_VOLUME			= 46,
	miQ_MTL_ENVIRONMENT		= 47,
	miQ_MTL_CONTOUR			= 48,
	miQ_MTL_PHOTON			= 49,
	miQ_MTL_PHOTONVOL		= 50,
	miQ_FUNC_USERPTR		= 51,
	miQ_FUNC_LOCK			= 52,
	miQ_FUNC_TYPE			= 53,
	miQ_FUNC_DECL			= 54,
	miQ_FUNC_NEXT			= 55,
	miQ_FUNC_INDIRECT		= 56,
	miQ_FUNC_PARAM_SIZE		= 57,
	miQ_FUNC_RESULT_SIZE		= 58,
	miQ_FUNC_PARAM			= 59,
	miQ_DECL_LOCK			= 60,
	miQ_DECL_TYPE			= 61,
	miQ_DECL_RESULT_SIZE		= 62,
	miQ_DECL_NAME			= 63,
	miQ_DECL_PARAM			= 64,
	miQ_DECL_VERSION		= 89,
	miQ_IMAGE_WIDTH			= 65,
	miQ_IMAGE_HEIGHT		= 66,
	miQ_IMAGE_BITS			= 67,
	miQ_IMAGE_COMP			= 68,
	miQ_IMAGE_FILTER		= 69,
	miQ_IMAGE_DESATURATE		= 85,
	miQ_IMAGE_DITHER		= 86,
	miQ_IMAGE_NOPREMULT		= 87,
	miQ_IMAGE_GAMMA			= 88,
	miQ_IMAGE_COLORCLIP		= 115,
	miQ_NUM_GLOBAL_LIGHTS		= 70,
	miQ_GLOBAL_LIGHTS		= 71,
	miQ_TRANS_INTERNAL_TO_WORLD	= 72,
	miQ_TRANS_INTERNAL_TO_CAMERA	= 73,
	miQ_TRANS_INTERNAL_TO_OBJECT	= 74,
	miQ_TRANS_WORLD_TO_INTERNAL	= 75,
	miQ_TRANS_CAMERA_TO_INTERNAL	= 76,
	miQ_TRANS_OBJECT_TO_INTERNAL	= 77,
	miQ_TRANS_WORLD_TO_CAMERA	= 78,
	miQ_TRANS_WORLD_TO_OBJECT	= 79,
	miQ_TRANS_CAMERA_TO_WORLD	= 80,
	miQ_TRANS_OBJECT_TO_WORLD	= 81,
	miQ_NUM_TEXTURES		= 82,
	miQ_GEO_LABEL			= 84,
	miQ_GEO_DATA			= 114,
	miQ_PRI_BBOX_MIN		= 94,
	miQ_PRI_BBOX_MAX		= 95,
	miQ_DATA_PARAM			= 105,
	miQ_DATA_DECL			= 106,
	miQ_DATA_NEXT			= 107,
	miQ_DATA_LABEL			= 108,
	miQ_DATA_PARAM_SIZE		= 109,
	miQ_DATA_NEEDSWAP		= 110,
	miQ_TILE_PIXELS			= 116,
	miQ_TILE_SAMPLES		= 117,
	miNQ				= 118
} miQ_type;

struct miState;
DLLIMPORT miBoolean mi_query(const miQ_type, struct miState * const,
				miUint, void * const, ...);

/*------------ mi_mem.h ------------*/

typedef struct miMemblk {
	char	*head;		/* first page */
	char	**free;		/* first free block */
	int	size;		/* aligned size of each block in bytes */
	union {
		void	*l;
		char	pad[8];
	}lock;
} miMemblk;
#define mi_mem_blkcreate(s)	mi_mem_int_blkcreate	(__FILE__, __LINE__, s)
#define mi_mem_strdup(s)	mi_mem_int_strdup	(__FILE__, __LINE__, s)
#define mi_mem_release(m)	mi_mem_int_release	(__FILE__, __LINE__, m)
#define mi_mem_allocate(s)	mi_mem_int_allocate	(__FILE__, __LINE__, s)
#define mi_mem_test_allocate(s)	mi_mem_int_test_allocate(__FILE__, __LINE__, s)
#define mi_mem_reallocate(m,s)	mi_mem_int_reallocate	(__FILE__,__LINE__,m,s)
#define mi_mem_test_reallocate(m,s) mi_mem_int_test_reallocate\
							(__FILE__,__LINE__,m,s)
DLLIMPORT void *mi_mem_int_allocate(
				  char * const,		/* __FILE__ */
				  const int,		/* __LINE__ */
				  const int);		/* # bytes to alloc */
DLLIMPORT void *mi_mem_int_test_allocate(
				  char * const,		/* __FILE__ */
				  const int,		/* __LINE__ */
				  const int);		/* # bytes to alloc */
DLLIMPORT void *mi_mem_int_reallocate(
				  char * const,		/* __FILE__ */
				  const int,		/* __LINE__ */
				  void * const,		/* block to resize */
				  const int);		/* new size of block */
DLLIMPORT void *mi_mem_int_test_reallocate(
				  char * const,		/* __FILE__ */
				  const int,		/* __LINE__ */
				  void * const,		/* block to resize */
				  const int);		/* new size of block */
DLLIMPORT void	mi_mem_int_release(
				  char * const,		/* __FILE__ */
				  const int,		/* __LINE__ */
				  void * const);	/* block to release */
DLLIMPORT void	mi_mem_check	 (void);
DLLIMPORT int	mi_mem_getsize	 (void);
DLLIMPORT void	mi_mem_summary	 (void);
DLLIMPORT void	mi_mem_dump	 (const miModule);	/* for which module/0*/

DLLIMPORT char *mi_mem_int_strdup(
				  char * const,		/* __FILE__ */
				  const int,		/* __LINE__ */
				  const char * const);	/* text to duplicate */
DLLIMPORT miMemblk *mi_mem_int_blkcreate(
				  char * const,		/* __FILE__ */
				  const int,		/* __LINE__ */
				  int);			/* fixed block size */
DLLIMPORT void	mi_mem_blkdelete(
				  miMemblk * const);	/* arena to destroy */
DLLIMPORT void *mi_mem_blkallocate(
				  miMemblk * const);	/* arena to alloc frm*/
DLLIMPORT void *mi_mem_blkfallocate(
				  miMemblk * const);	/* arena to alloc frm*/
DLLIMPORT void	mi_mem_blkrelease(
				  miMemblk * const,	/* arena block is in */
				  void * const);	/* block to release */
DLLIMPORT void	mi_mem_blkclear(
				  miMemblk * const);	/* arena to clear */
DLLIMPORT void	mi_mem_blkenumerate(
				  miMemblk * const,	/* arena to enumerate*/
				  void (*)(void *));	/* callback for slots*/

/*------------ mi_db.h ------------*/

typedef miUint miTag;

#define miNULLTAG 0
DLLIMPORT void  *mi_db_access(
			const miTag);	/* tag to be accessed */
DLLIMPORT int    mi_db_type(
			const miTag);	/* tag to be accessed */
DLLIMPORT int    mi_db_size(
			const miTag);	/* tag to be accessed */
DLLIMPORT void  *mi_db_access_type(
			int * const,	/* returned type, really miScene_type*/
			const miTag);	/* tag to be accessed */
DLLIMPORT void   mi_db_unpin(
			const miTag);	/* tag to be unpinned */
DLLIMPORT void  *mi_db_create(
			miTag * const,	/* returned new tag */
			const int,	/* miSCENE_* type to store */
			const int);	/* size of item in bytes */
DLLIMPORT void  *mi_db_realloc(
			const miTag,	/* tag to be resized */
			const int);		/* new size of the db item */
DLLIMPORT void   mi_db_delete(
			const miTag);	/* tag to be deleted */
DLLIMPORT miTag  mi_db_copy(
			const miTag);	/* source tag to duplicate */
DLLIMPORT void   mi_db_flush(
			const miTag);	/* tag to be flushed */
DLLIMPORT void   mi_db_deferredflush(
			const miTag);	/* tag to be flushed */
DLLIMPORT int    mi_db_vpu_of_tag(
			const miTag);	/* tag to extract VPU from */
DLLIMPORT void   mi_db_dump(
			const miModule);/* dump whose entries, 0=all */

/*------------ mi_img.h ------------*/

/*
 * supported internal scanline types. These types are used when a file is
 * opened and created; they specify the internal data structure but not the
 * type of the file to be created. If you change this list, also change
 * compatible_type[] in imgcall.c. The use as array indexes is the reason
 * why every enumerator is assigned explicitly, to show that the numbers
 * are important. No real type may exceed 31!
 * miIMG_TYPE_ANY is a wildcard that disables conversion. The USER types
 * are assigned real types in "frame buffer" statements. Defining them here
 * allows storing them in the out_type and out_interp bitmaps in miFunction.
 * No type should have a code >31 because output statements use type bitmaps.
 */

typedef enum {
	miIMG_TYPE_RGBA		= 0,	/* 4*8	bits, color with alpha */
	miIMG_TYPE_RGBA_16	= 1,	/* 4*16 bits, color with alpha */
	miIMG_TYPE_RGB		= 2,	/* 4*8	bits, color */
	miIMG_TYPE_RGB_16	= 3,	/* 4*16 bits, color */
	miIMG_TYPE_A		= 4,	/* 1*8	bits, alpha */
	miIMG_TYPE_A_16		= 5,	/* 1*16 bits, alpha */
	miIMG_TYPE_S		= 6,	/* 1*8	bits, intensity */
	miIMG_TYPE_S_16		= 7,	/* 1*16 bits, intensity */
	miIMG_TYPE_VTA		= 8,	/* 2*16 bits, bumpmap from alpha */
	miIMG_TYPE_VTS		= 9,	/* 2*16 bits, bumpmap from intensity */
	miIMG_TYPE_Z		= 10,	/* 1*32 bits, Z coordinates (float) */
	miIMG_TYPE_N		= 11,	/* 3*32 bits, normal vectors (float) */
	miIMG_TYPE_M		= 14,	/* 3*32 bits, motion vectors (float) */
	miIMG_TYPE_TAG		= 12,	/* 1*32 bits, material tags (u int) */
	miIMG_TYPE_BIT		= 15,	/* 1*1	bit,  must-render flag map */
	miIMG_TYPE_RGBA_FP	= 13,	/* 4*32 bits, color with alpha, float*/
	miIMG_NTYPES		= 16,
					/* remainder is out-of-band types */
	miIMG_TYPE_CONT		= 23,	/* API/PHEN: contours */
	miIMG_TYPE_USER_FIRST	= 24,	/* API/PHEN: user-defined types 0..7 */
	miIMG_TYPE_USER_LAST	= 31,	/* (used by frame buffer defs 0..7) */
	miIMG_TYPE_ANY		= 99	/* img_open: don't convert */
} miImg_type;


/*
 * supported file formats. These are used for one purpose only: when creating
 * an image file, they specify the output format. When adding or inserting
 * an entry, add external declarations and entries to all arrays defined as
 * "something[miIMG_NFORMATS]" at the beginning of imgdispatch.c. Make sure
 * the order is correct!
 * Generally, common formats should precede uncommon, and formats difficult
 * to recognize by their headers should come last (qnt, zpic). QNT_PAL must
 * be QNT_NTSC-1, and there may be only two QNT formats (see mi_img_open).
 */

typedef enum {
	miIMG_FORMAT_RLA	= 0,	/* Wavefront RLA */
	miIMG_FORMAT_RLB	= 1,	/* Wavefront RLB */
	miIMG_FORMAT_PIC	= 2,	/* Softimage pic, compressed */
	miIMG_FORMAT_ALIAS	= 3,	/* Alias */
	miIMG_FORMAT_RGB	= 4,	/* SGI RGB, compressed */
	miIMG_FORMAT_CT		= 5,	/* mi Color Texture */
	miIMG_FORMAT_CT_16	= 6,	/* mi Color Texture, 16 bits */
	miIMG_FORMAT_ST		= 7,	/* mi Scalar Texture */
	miIMG_FORMAT_ST_16	= 8,	/* mi Scalar Texture, 16 bits */
	miIMG_FORMAT_VT		= 9,	/* mi vector texture, from alpha */
	miIMG_FORMAT_WT		= 10,	/* mi vector texture, from intensity */
	miIMG_FORMAT_ZT		= 11,	/* mi Z channel, from intersect depth*/
	miIMG_FORMAT_NT		= 12,	/* mi normal vectors */
	miIMG_FORMAT_MT		= 28,	/* mi motion vectors */
	miIMG_FORMAT_TT		= 13,	/* mi tag channel, from material tags*/
	miIMG_FORMAT_BIT	= 29,	/* mi one-bit render-enable flags */
	miIMG_FORMAT_TIFF	= 14,	/* compressed	4-component 8bit TIFF */
	miIMG_FORMAT_TIFF_U	= 15,	/* uncompressed 4-component 8bit TIFF */
	miIMG_FORMAT_TIFF_3	= 16,	/* compressed	3-component 8bit TIFF */
	miIMG_FORMAT_TIFF_U3	= 17,	/* uncompressed 3-component 8bit TIFF */
	miIMG_FORMAT_TIFF_16	= 30,	/* compressed 4-component 16bit TIFF */
	miIMG_FORMAT_TIFF_16_U	= 31,	/* uncompress 4-component 16bit TIFF */
	miIMG_FORMAT_TIFF_16_3	= 32,	/* compressed 3-component 16bit TIFF */
	miIMG_FORMAT_TIFF_16_U3	= 33,	/* uncompress 3-component 16bit TIFF */
	miIMG_FORMAT_CATIA	= 18,	/* Dassault Systemes Catia format */
	miIMG_FORMAT_PPM	= 19,	/* Jef Poskanzer's portable pixmaps */
	miIMG_FORMAT_CT_FP	= 20,	/* mi Color Texture, floating-point */
	miIMG_FORMAT_MAP	= 21,	/* mmap-ed raw texture file */
	miIMG_FORMAT_TARGA	= 22,	/* compressed RGBA Targa */
	miIMG_FORMAT_BMP	= 23,	/* Microsoft BMP 32-bit uncompressed */
	miIMG_FORMAT_QNT_PAL	= 24,	/* Quantel/Abekas, 576x720 */
	miIMG_FORMAT_QNT_NTSC	= 25,	/* Quantel/Abekas, 486x720 */
	miIMG_FORMAT_ZPIC	= 26,	/* Softimage Zpic, uncompr Z coords */
	miIMG_FORMAT_NULL	= 27,	/* dummy file, write nothing at all */
	miIMG_FORMAT_JPEG	= 34,	/* baseline sequential JPEG */
	miIMG_FORMAT_CUSTOM_0	= 35,	/* user-defined */
	miIMG_FORMAT_CUSTOM_1	= 36,	/* user-defined */
	miIMG_FORMAT_CUSTOM_2	= 37,	/* user-defined */
	miIMG_FORMAT_CUSTOM_3	= 38,	/* user-defined */
	miIMG_FORMAT_CUSTOM_4	= 39,	/* user-defined */
	miIMG_FORMAT_CUSTOM_5	= 40,	/* user-defined */
	miIMG_FORMAT_CUSTOM_6	= 41,	/* user-defined */
	miIMG_FORMAT_CUSTOM_7	= 42,	/* user-defined */
	miIMG_FORMAT_CUSTOM_8	= 43,	/* user-defined */
	miIMG_FORMAT_CUSTOM_9	= 44,	/* user-defined */
	miIMG_FORMAT_CUSTOM_10	= 45,	/* user-defined */
	miIMG_FORMAT_CUSTOM_11	= 46,	/* user-defined */
	miIMG_FORMAT_CUSTOM_12	= 47,	/* user-defined */
	miIMG_FORMAT_CUSTOM_13	= 48,	/* user-defined */
	miIMG_FORMAT_CUSTOM_14	= 49,	/* user-defined */
	miIMG_FORMAT_CUSTOM_15	= 50,	/* user-defined */
	miIMG_FORMAT_CUSTOM_LAST= 50,	/* last user-defined */
	miIMG_NFORMATS		= 51,
	miIMG_FORMAT_PS		= 52	/* out-of-band type for PostScript */
					/* and mi format. Handled as a */
					/* special case, and implies contour */
					/* output. Never passed to IMG. */
} miImg_format;



/*
 * error codes. If you add one, also add a message to errmsg[] in libimg.c.
 */

typedef enum {
	miIMG_ERR_NONE		= 0,	/* no error */
	miIMG_ERR_READ		= 1,	/* can't read, end of file */
	miIMG_ERR_WRITE		= 2,	/* can't write, probably disk full */
	miIMG_ERR_DECODE	= 3,	/* file format error */
	miIMG_ERR_ENCODE	= 4,	/* internal error */
	miIMG_ERR_TYPE		= 5,	/* unrecognized file type */
	miIMG_ERR_SUBTYPE	= 6,	/* file version not supported */
	miIMG_ERR_CREATE	= 7,	/* failed to create file */
	miIMG_ERR_OPEN		= 8,	/* failed to open file */
	miIMG_ERR_CLOSE		= 9,	/* failed to close file */
	miIMG_ERR_ARGS		= 10,	/* incorrect call argument */
	miIMG_ERR_NOTOPEN	= 11,	/* file not open */
	miIMG_ERR_NOTCLOSED	= 12,	/* file still open, can't re-open */
	miIMG_ERR_SIZE		= 13,	/* wrong file size */
	miIMG_NERRS
} miImg_error;


/*
 * file descriptor. The file name is stored to be able to print meaningful
 * error messages. It is an array, not a strdup'ed pointer, because it is
 * not clear who would de-allocate the name otherwise. If the file name is
 * longer than the array, it is truncated. The real file name is stored
 * too, but it can't be relied upon to exist. Gamma is not used and may go
 * away in future versions.
 * Other modules may only use the public fields. The others are reserved
 * by raylib (and in fact not documented in the external manual). The fp
 * field is actually a FILE*, but it's void* here because otherwise every
 * file including img.h would have to include stdio.h as well. It's always
 * cast to FILE* in img*.c.
 */

typedef struct {
	int		c[4];		/* indexed by miIMG_* */
} miImg_line;				/* component scanlines follow here */

typedef struct miImg_file {
	int		null;		/* bomb out if someone forgets & */
					/* --------------- public --------- */
	int		width, height;	/* width and height in pixels */
	int		bits;		/* requested bits per component, 8/16*/
	int		comp;		/* requested components/pixel, 1..4 */
	miBoolean	filter;		/* caller wants filtered lookups */
	miBoolean	topdown;	/* read file top-down */
	int		filesize;	/* if reading, size of file in bytes */
	float		gamma;		/* gamma correction value (read only)*/
	float		aspect;		/* aspect/xres*yres (for .pic output)*/
	float		parms[8];	/* additional undefined parameters, */
					/* parm[0] is compression quality */
	miImg_type	type;		/* requested miIMG_TYPE_* */
	miImg_format	format;		/* if reading, file format */
	miImg_error	error;		/* one of miIMG_ERR_*, or 0 */
	int		os_error;	/* copy of Unix's errno, or 0 */
					/* --------------- private --------- */
	int		magic;		/* magic number while file is open */
	int		lineno;		/* current line# to be read/written */
	miBoolean	writing;	/* if nz, file is open for writing */
	miCBoolean	swap_map;	/* swap if memory-mapping */
	miCBoolean	ftype;		/* actual file data type, stored as   */
					/* type + miIMG_NTYPES if current type*/
					/* differs from best_type of format   */
	miCBoolean	spare[2];	/* not used */
	miPointer	fp;		/* open file descriptor, 0 if closed */
	miPointer	real_name;	/* complete path (for unlink/local) */
	miPointer	convline;	/* converted line if type != format */
	miPointer	data;		/* more format-dependent file info */
	char		name[64];	/* file name (for error messages) */
} miImg_file;


/*
 * generic scanline and image struct. They are based on index values that
 * define the beginning of component scanlines relative to the beginning of
 * the struct (including header and all). Both miImg_line and miImg_image
 * are actually allocated larger than their size; the component scanlines
 * directly follow the typedef'ed header. One mi_mem_release suffices for
 * the whole thing including data.
 * <comp> in the access macro stands for one of the miIMG_* constants below.
 * Do not use miIMG_LINEACCESS before using miIMG_ACCESS to load local texs!
 */

#define miIMG_R		0		/* red */
#define miIMG_G		1		/* green */
#define miIMG_B		2		/* blue */
#define miIMG_A		3		/* alpha */
#define miIMG_S		miIMG_A		/* scalar */
#define miIMG_U		0		/* u vector */
#define miIMG_V		1		/* v vector */
#define miIMG_Z		0		/* Z coordinate */
#define miIMG_T		0		/* material tag */
#define miIMG_NX	0		/* normal vector X component */
#define miIMG_NY	1		/* normal vector Y component */
#define miIMG_NZ	2		/* normal vector Z component */

#define miIMG_ACCESS_RAW(_i, _y,_c)\
	((miUchar *)(_i) + (_i)->c[4*(_y)+(_c)])

#define miIMG_ACCESS(_i,_y,_c) (				\
	(_i)->local ?(						\
		((_i)->image.p = (_i)->image.p ?		\
			(_i)->image.p				\
		:						\
			(void*)mi_img_validate_local_image(_i)),\
		miIMG_ACCESS_RAW(				\
			(miImg_image*)((_i)->image.p), _y, _c)	\
	) :							\
		miIMG_ACCESS_RAW(_i,_y,_c)			\
)

#define miIMG_LINEACCESS(_l,_c)	((miUchar *)(_l)+(_l)->c[(_c)])

#define miIMG_DIRSIZE	20		/* image filters have max 20 levels */

typedef struct miImg_image {
	miScalar	filter;		/* filtered lookups if > 0 */
	int		dirsize;	/* valid # of filter levels */
	int		dir[miIMG_DIRSIZE]; /* offs from this to other imgs */
	int		width, height;	/* width and height in pixels */
	int		bits;		/* requested bits per comp, 8/16/32 */
	int		comp;		/* requested components/pixel, 1..4 */
	miBoolean	local;		/* local texture, use image/mmap/path*/
	int		type;		/* requested miIMG_TYPE_* */
	miTag		real_name;	/* (local) file name to open */
	int		null[1];	/* unused */
	int		c[4];		/* local==miFALSE: */
					/*	indexed by miIMG_*; 4*height */
					/*	component indices follow, */
					/*	then component scanlines */
	miPointer	image;		/* local==miTRUE: */
	miImg_file	ifp;		/*	describes open file */
	miBoolean	mmapped;	/*	mmapped, not mi_mem_allocated*/
} miImg_image;

DLLIMPORT miBoolean    mi_img_create(
				miImg_file *const,	/* file descriptor */
				char * const,		/* new file name */
				const miImg_type,	/* data type, RGB etc*/
				const miImg_format,	/* format, RLA etc */
				const int,		/* image width */
				const int);		/* image height */
DLLIMPORT miBoolean    mi_img_open(
				miImg_file *const,	/* file descriptor */
				char * const,		/* file name */
				const miImg_type);	/* desired data type */
DLLIMPORT miBoolean    mi_img_close(
				miImg_file *const);	/* file descriptor */
DLLIMPORT miBoolean    mi_img_image_read(
				miImg_file *const,	/* file descriptor */
				miImg_image*const);	/* frame buffer */
DLLIMPORT miBoolean    mi_img_image_write(
				miImg_file *const,	/* file descriptor */
				miImg_image*const);	/* frame buffer */
DLLIMPORT miImg_image *mi_img_mmap_address(
				miImg_file *const);	/* file descriptor */
DLLIMPORT miImg_image *mi_img_image_dbcreate(
				miImg_file *const,	/* file descriptor */
				miTag * const);		/* returned tag in DB*/
DLLIMPORT miImg_image *mi_img_local_image_dbcreate(
				miImg_file *const,	/* file struct */
				miTag * const);		/* returned tag in DB*/
DLLIMPORT miImg_image *mi_img_validate_local_image(
				miImg_image *const);	/* image to validate */
DLLIMPORT miUchar     *mi_img_access(
				miImg_image	*,	/* image to access */
				const int,		/* requested scanline*/
				const int);		/* req component 0..3*/
DLLIMPORT void	       mi_img_invalidate_local_image(
				miImg_image *const);	/* image to invalid. */
DLLIMPORT miImg_image *mi_img_image_alloc(
				miImg_file *const);	/* file descriptor */
DLLIMPORT miImg_type   mi_img_type_identify(
				char * const);		/* type, "rgb" etc */
DLLIMPORT miImg_format mi_img_format_identify(
				char * const);		/* format, "rla" etc */
DLLIMPORT miImg_type   mi_img_best_type(
				const miImg_format);	/* format to test */
DLLIMPORT char	      *mi_img_type_name(
				const miImg_type);	/* data type to name */
DLLIMPORT char	      *mi_img_format_name(
				const miImg_format);	/* format to name */
typedef enum {						/* mode selector */
	miIMG_MODE_DESATURATE,	/* miBoolean */		/* desaturation */
	miIMG_MODE_DITHER,	/* miBoolean */		/* dithering */
	miIMG_MODE_NOPREMULT,	/* miBoolean */		/* premultiplication */
	miIMG_MODE_COLORCLIP,	/* int */		/* color clip mode */
	miIMG_MODE_GAMMA	/* double */		/* gamma correction */
} miImg_mode_sel;

typedef enum {						/* colorclip modes */
	miIMG_COLORCLIP_RGB,				/* rgb priority */
	miIMG_COLORCLIP_ALPHA,				/* alpha priority */
	miIMG_COLORCLIP_RAW				/* ignore premult */
} miImg_colorclip;

typedef union {						/* mode value  */
	int		i;
	miBoolean	b;
	double		d;
} miImg_mode_val;
DLLIMPORT void	       mi_img_getmode(
				miImg_mode_sel	const ,	/* mode id */
				miImg_mode_val	*const);/* mode value */
DLLIMPORT void	       mi_img_clip_color(
				miImg_image *const,	/* image to clip for */
				miColor *const);	/* color to clip */
DLLIMPORT void	       mi_img_put_color(
				miImg_image *const,	/* image to store in */
				miColor * const,	/* color to store */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_put_scalar(
				miImg_image *const,	/* image to store in */
				const float,		/* scalar to store */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_put_vector(
				miImg_image *const,	/* image to store in */
				miVector * const,	/* vector to store */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_put_depth(
				miImg_image *const,	/* image to store in */
				const float,		/* depth, 0=eye, >=0 */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_put_normal(
				miImg_image *const,	/* image to store in */
				miVector * const,	/* normal to store */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_put_label(
				miImg_image *const,	/* image to store in */
				const miUint,		/* label value */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_get_color(
				miImg_image *const,	/* image to store in */
				miColor * const,	/* color to read */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_get_scalar(
				miImg_image *const,	/* image to store in */
				float * const,		/* scalar to read */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_get_vector(
				miImg_image *const,	/* image to store in */
				miVector * const,	/* vector to read */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_get_depth(
				miImg_image *const,	/* image to store in */
				float * const,		/* depth, 0=eye, >=0 */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_get_normal(
				miImg_image *const,	/* image to store in */
				miVector * const,	/* normal to read */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_get_label(
				miImg_image *const,	/* image to store in */
				miUint * const,		/* returned label */
				const int,		/* X coordinate */
				const int);		/* Y coordinate */
DLLIMPORT void	       mi_img_invalidate_local_images(
				void);			/* all local images */

/*------------ mi_msg.h ------------*/

typedef union {
	struct mi_lock *milockptr;
	int	padding[2];
} miLock;

DLLIMPORT miBoolean mi_init_lock(miLock *const);/* create and initialize lock*/
DLLIMPORT void mi_delete_lock(miLock * const);	/* destroy and free lock */
DLLIMPORT void mi_lock(miLock);			/* acquire lock, blocking */
DLLIMPORT void mi_unlock(miLock);		/* release a lock */

/*------------ mi_par.h ------------*/

typedef miUint		miVpu;
						/* (host, thread) -> VPU */
#define miVPU(h,t)	((((h)&0xffffu)<<16)|((t)&0xffffu))
#define miHOST(vpu)	(((vpu)>>16)&0xffffu)	/* VPU -> host */
#define miTHREAD(vpu)	((vpu)&0xffffu)		/* VPU -> thread */
DLLIMPORT miVpu mi_par_localvpu(void);

DLLIMPORT int mi_par_nthreads(void);

DLLIMPORT void mi_par_abort(		/* quit frame while it is running */
	const int	flag);		/* 0=clear, m=set bit, ~m=clear bit */
DLLIMPORT int mi_par_aborted(void);

/*------------ mi_scene.h ------------*/

/*
 * Every DB element managed by SCENE has a unique type and is known by SCENE.
 * mi_db_create should not be used by modules other than SCENE because that
 * makes it hard to properly swap bytes between sane parts of the
 * network and Windows NT.
 * When adding types, change the arrays in scncall1.c!
 */

typedef enum {
	miSCENE_NONE = 0,	/* unknown. Danger, Will Robinson. */
	miSCENE_FUNCTION_DECL,	/*  1:miFunction_decl */
	miSCENE_FUNCTION,	/*  2:miFunction */
	miSCENE_MATERIAL,	/*  3:miMaterial */
	miSCENE_LIGHT,		/*  4:miLight */
	miSCENE_CAMERA,		/*  5:miCamera */
	miSCENE_BASIS_LIST,	/*  6:miBasis_list */
	miSCENE_OBJECT,		/*  7:miObject */
	miSCENE_BOX,		/*  8:miBox */
	miSCENE_LINEBOX,	/*  9:miLinebox */
	miSCENE_INSTANCE,	/* 10:miInstance */
	miSCENE_GROUP,		/* 11:miGroup */
	miSCENE_OPTIONS,	/* 12:miRc_options */
	miSCENE_IMAGE,		/* 13:image data type */
	miSCENE_POLYGON,	/* 14:miPolygon [] */
	miSCENE_INDEX,		/* 15:miIndex [], used as indices */
	miSCENE_GEOINDEX,	/* 16:miGeoIndex [], used as indices */
	miSCENE_VERTEX,		/* 17:miIndex [], used as vertices */
	miSCENE_GEOVERTEX,	/* 18:miGeoIndex [], used as vertices */
	miSCENE_VECTOR,		/* 19:miVector [] */
	miSCENE_GEOVECTOR,	/* 20:miGeoVector [] */
	miSCENE_FACE,		/* 21:miFace [] */
	miSCENE_SURFACE,	/* 22:miSurface [] */
	miSCENE_CURVE,		/* 23:miCurve [] */
	miSCENE_CURVPNT,	/* 24:miCurve_point [] */
	miSCENE_SCALAR,		/* 25:miScalar [] */
	miSCENE_GEOSCALAR,	/* 26:miGeoScalar [] */
	miSCENE_ALGEBRAIC,	/* 27:miAlgebraic [] */
	miSCENE_TAG,		/* 28:miTag [] */
	miSCENE_STRING,		/* 29:char [] */
	miSCENE_SPACECURVE,	/* 30:miSpacecurve [] */
	miSCENE_BOOK,		/* 31:miBook */
	miSCENE_PAGE,		/* 32:miPage */
	miSCENE_SUBDIVSURF,	/* 33:miSurf_subsurf */
	miSCENE_USERDATA,	/* 34:miUserdata */
	miSCENE_NTYPES
} miScene_types;

/*------------ mi_scene_db.h ------------*/

enum miApprox_method {
	miAPPROX_PARAMETRIC,
	miAPPROX_REGULAR,
	miAPPROX_SPATIAL,
	miAPPROX_CURVATURE,
	miAPPROX_LDA,
	miAPPROX_ADJACENCY,		/* only for curves */
	miAPPROX_ALGEBRAIC,		/* only for surfaces */
	miAPPROX_DEFTRIM,		/* only for miFace def_trim_approx */
	miAPPROX_NMETHODS
};
enum miApprox_style {
	miAPPROX_STYLE_NONE = 0,	/* must be 0 for miOptions default */
	miAPPROX_STYLE_GRID,		/* (0 means don't override) */
	miAPPROX_STYLE_TREE,
	miAPPROX_STYLE_DELAUNAY,
	miAPPROX_NSTYLES
};

#define miCNST_UPARAM	0		/* regular/parametric only */
#define miCNST_VPARAM	1
#define miCNST_LENGTH	0		/* curvature/spatial only */
#define miCNST_DISTANCE	1
#define miCNST_ANGLE	2

typedef struct miApprox	{
	miGeoScalar	     cnst[3];	/* indexed with miUPARAM...miANGLE  */
	miCBoolean	     spare0[2];	/* not used			    */
	miCBoolean	     any;	/* stop if any criterion is met	    */
	miCBoolean	     view_dep;	/* view dependent ?		    */
	enum miApprox_method method;
	enum miApprox_style  style;
	miUshort	     subdiv[2];	/* recursive subdivision depth	    */
	miGeoIndex	     max;	/* maximum number of triangles	    */
	miScalar	     grading;	/* min angle bound for graded meshing*/
} miApprox;

#define miAPPROX_MAX_SUBDIV 7
#define miAPPROX_DEFAULT(A) do {		\
	(A).style	  = miAPPROX_STYLE_TREE;\
	(A).method	  = miAPPROX_PARAMETRIC;\
	(A).spare0[0]	  = miFALSE;		\
	(A).spare0[1]	  = miFALSE;		\
	(A).any		  = miFALSE;		\
	(A).view_dep	  = miFALSE;		\
	(A).cnst[0]	  = 0.0;		\
	(A).cnst[1]	  = 0.0;		\
	(A).cnst[2]	  = 0.0;		\
	(A).subdiv[miMIN] = 0;			\
	(A).subdiv[miMAX] = 5;			\
	(A).max		  = miHUGE_INT;		\
	(A).grading	  = 0.0; } while (0)
#define miRC_MAX_IMAGES		16		/* max. number of frame bufs */
#define miRC_IMAGE_RGBA		0		/* standard RGBA color fb */
#define miRC_IMAGE_Z		1		/* standard depth fb */
#define miRC_IMAGE_N		2		/* standard normal-vector fb */
#define miRC_IMAGE_M		3		/* standard motion-vector fb */
#define miRC_IMAGE_TAG		4		/* standard object label fb */
#define miRC_IMAGE_PPREV	7		/* reserved for pixel preview*/
#define miRC_IMAGE_USER		8		/* first user-defined fb */
#define miRC_CLIP_MIN		0.001		/* min near clipping plane */

#define miRc_options		miOptions	/* backwards compatibility */

/* bit definitions for miOptions.diagnostic_mode */
#define miSCENE_DIAG_SAMPLES	1		/* turn on sample view mode */
#define miSCENE_DIAG_PHOTON	6		/* photon 2 bits  */
#define miSCENE_DIAG_PHOTON_D	2		/* photon density  */
#define miSCENE_DIAG_PHOTON_I	4		/* photon irradiance  */
#define miSCENE_DIAG_GRID	24		/* grid 2 bits */
#define miSCENE_DIAG_GRID_O	8		/* object_space_grid*/
#define miSCENE_DIAG_GRID_W	16		/* world_space_grid*/
#define miSCENE_DIAG_GRID_C	24		/* camera_space_grid*/


typedef struct miOptions {
	miBoolean	trace;			/* 2nd generation ray trace? */
	int		scanline;		/* 0=off, 1=on, 'o'=OpenGL */
	miBoolean	motion;			/* motion blur? : miFALSE */
	miBoolean	shadow_sort;		/* Obsolete! Do not use */
	miBoolean	preview_mode;		/* for future use */
	int		reflection_depth;	/* refl. trace depth : 1 */
	int		refraction_depth;	/* refr. trace depth : 1 */
	int		trace_depth;		/* refl. + refr. depth : 1 */
	int		min_samples;		/* min. sampling level : -2 */
	int		max_samples;		/* max. sampling level : 0 */
	miColor		contrast;		/* sampling contrast: .1 */
	miColor		time_contrast;		/* temporal sampling contrast*/
	miTag		contour_contrast;	/* NULLTAG */
	miTag		contour_store;		/* NULLTAG */
	miBoolean	caustic;		/* enable caustics: miFALSE */
	miBoolean	globillum;		/* enable global il.: miFALSE*/
	int		caustic_accuracy;	/* no. caus. photons in est. */
	float		caustic_radius;		/* maxdist for caus. photons */
	int		globillum_accuracy;	/* no. glob. photons in est. */
	float		globillum_radius;	/* maxdist for glob. photons */
	float		caustic_filter_const;	/* filter const. for caus. */
	float		filter_size_x;		/* filter size in x : 1.0 */
	float		filter_size_y;		/* filter size in y : 1.0 */
	float		jitter;			/* sample jittering : 0.0 */
	float		shutter;		/* shutter speed for motion:0*/
	int		subdivision;		/* ray class. adjustment : 0 */
	int		subdivision_2d;		/* eye + shadow adjustment:0 */
	int		subdivision_memory;	/* ray class. memory (mb): 6 */
	int		space_max_size;		/* space subdiv. leaf size: 4*/
	int		space_max_depth;	/* space subdiv. depth : 25*/
	float		grid_size;		/* typical # voxels per tri */
	int		no_images;		/* # images : 1 */
	miImg_type	image_types[miRC_MAX_IMAGES]; /* image types */
						/* miIMG_TYPE_RGBA, ...ANY...*/
	miBoolean	write_image[miRC_MAX_IMAGES]; /* write image?*/
						/* miTRUE ... */
	miBoolean	interp_image[miRC_MAX_IMAGES]; /* interpolate image? */
						/* miTRUE, miFALSE ... */
	miTag		shader_tree_image;	/* for previewing */
	miTag		pixel_sample_image;	/* for previewing */
	char		use_shadow_maps;	/* use shadow maps ? 0 */
	miCBoolean	rendering_shadow_maps;	/* Shadow map mode? 0 */
	char		recompute_shadow_maps;	/* 'n'o,'y'es (could be Bool)*/
	char		shadow;			/* shadow casting? : 1 */
						/* 0 off, 1 on, 'l' sort, */
						/* 's' segments */
	char		caustic_filter;		/* filter-type */
						/* 'b' box, 'c' cone, */
						/* 'g' gaussian */
	miCBoolean	spare3;
	char		filter;			/* sample filter type : 'b' */
						/* 'b' box, 't' triangle, */
						/* 'g' gauss */
	char		acceleration;		/* acceleration: 'b' */
						/* 'c' ray classification, */
						/* 'b' bsp, 'g' grid */
	char		face;			/* primitive facing: 'a' */
						/* 'f' front, 'b' back, */
						/* 'a' all */
	char		field;			/* field rendering? 0 */
						/* 0 off, 'e' even, 'o' odd */
	char		smethod;		/* sampling algorithm: 'q' */
	char		render_space;		/* coordinate space 'o' */
						/* 'c' camera space, */
						/* 'o' object space, */
						/* 'm' mixed space */
	miCBoolean	pixel_preview;		/* pixel selective sampling */
	miCBoolean	task_preview;		/* task selective sampling */
	miCBoolean	visible_lights;		/* any visible area lights? */
	miCBoolean	shadow_map_motion;	/* motion blurred shadowmaps?*/
	int		task_size;		/* image task size : 0 */
	miBoolean	strips;			/* GAP returns triangles in
						   strips */
	miTag		photonmap_file;		/* photon map file name */
	miBoolean	photonmap_rebuild;	/* photon map rebuild ? */
	int		photon_reflection_depth;/* photon refl. depth */
	int		photon_refraction_depth;/* photon refl. depth */
	int		photon_trace_depth;	/* total photon trace depth */
	int		space_max_mem;		/* maximum bsp memory (mb):0 */
	miPointer	image[miRC_MAX_IMAGES];	/* pointers to frame buffers */
						/* (for output shaders only) */
	miCBoolean	no_lens;		/* disable lens shaders */
	miCBoolean	no_volume;		/* disable volume shaders */
	miCBoolean	no_geometry;		/* disable geometry shaders */
	miCBoolean	no_displace;		/* disable displace shaders */
	miUint1		no_output;		/* disable output shaders */
	miCBoolean	no_merge;		/* disable surface merging */
	miUint1		caustic_flag;		/* def. caustic flag for objs*/
	miUint1		diagnostic_mode;	/* miSCENE_DIAG_* flags */
	int		photonvol_accuracy;	/* no. vol. photons in est. */
	float		photonvol_radius;	/* maxdist for vol. photons */
	miUint1		globillum_flag;		/* def. globil. flag for objs*/
	miCBoolean	samplelock;		/* steady samples?: on */
	miCBoolean	spare1[8];		/* future disable flags */
	miCBoolean	first_cut;		/* first cookie cut */
	miCBoolean	last_cut;		/* last cookie cut */
	int		cut_windows;		/* cookie cutter cuts: 1 */
	miScalar	cut_expand;		/* tessellate outside frustrum
						   expanded by n/100%, 0=def */
	miTag		userdata;		/* optional user data blocks */
	miApprox	approx;			/* approximation overrides if*/
	miApprox	approx_displace;	/* style != APPROX_STYLE_NONE*/
	miBoolean	finalgather;		/* fg. for globillum: miFALSE*/
	int		finalgather_rays;	/* no. rays in final gather */
	float		finalgather_maxradius;	/* maxdist for finalgather */
	float		finalgather_minradius;	/* mindist for finalgather */
	float		diag_photon_density;	/* density for diag */
	float		diag_grid_size;		/* gridsize for diag */
	int		spare2[14];		/* not used */
} miOptions;
typedef struct miCamera	{
	miBoolean	orthographic;		/* orthographic	rendering? */
	float		focal;			/* focal length	*/
	float		aperture;		/* aperture */
	float		aspect;			/* aspect ratio	of the image */
	miRange		clip;			/* min/max clipping */
	int		x_resolution;		/* x resolution	*/
	int		y_resolution;		/* y resolution	*/
	struct {int xl,	yl, xh,	yh;}
			window;			/* corners of subwindow	*/
	miTag		volume;			/* opt.	volume shader */
	miTag		environment;		/* opt.	environment shader */
	miTag		lens;			/* opt.	lens shader */
	miTag		output;			/* opt. output shaders/files */
	int		frame;			/* current frame number	*/
	float		frame_time;		/* ... as time in seconds */
	int		frame_field;		/* 0=frame, 1/2=field number */
	float		x_offset;		/* x image offset in pixels */
	float		y_offset;		/* y image offset in pixels */
	miTag		userdata;		/* optional user data blocks */
	int		spare[15];		/* not used */
} miCamera;
enum miLight_type {
	miLIGHT_ORIGIN,
	miLIGHT_DIRECTION,
	miLIGHT_SPOT
};

enum miLight_area {
	miLIGHT_NONE = 0,
	miLIGHT_RECTANGLE,
	miLIGHT_DISC,
	miLIGHT_SPHERE,
	miLIGHT_CYLINDER
};
enum miFunction_type {
	miFUNCTION_C,				/* regular C/C++ shader */
	miFUNCTION_PHEN,			/* phenomenon interface */
	miFUNCTION_C_REQ,			/* C/C++ shader with reqmnts */
	miFUNCTION_OUTFILE,			/* write an image to a file */
	miFUNCTION_DATA				/* user data decl, no miFunc */
};

typedef struct miFunction {
	miPointer	user;			/* for shader (init data etc)*/
	miPointer	cached_address;		/* for RC only */
	miPointer	cached_result;		/* array[thread#] of results */
	miPointer	cached_ghost_ptr;	/* if ghost_offs, tag ptrs */
	miPointer	gcache_timestamp;	/* if ghost_offs, eval time */
	miPointer	lightlist;		/* array of lights for shader*/
	miLock		lock;			/* local shader	lock */
	enum miFunction_type type;		/* C/C++, phenomenon, or file*/
	miUint		out_typemap;		/* if output, IMG_TYPE bitmap*/
	miUint		out_interpmap;		/* if output, interpolate bm */
	miTag		function_decl;		/* declaration if C or PHEN */
	miTag		next_function;		/* next	function call in list*/
	miTag		parameter_indirect;	/* get params from this func */
	miTag		interfacephen;		/* parent phen, if any */
	miBoolean	spare2;			/* not used */
	int		parameter_size;		/* size	of parameter block */
	int		result_size;		/* size of result struct */
	int		ghost_offs;		/* offset to tag ghost, or 0 */
	int		spare1[2];		/* not used */
	miCBoolean	has_init_shader;	/* init	shader exists */
	miCBoolean	func_init;		/* was function	initialized? */
	miCBoolean	inst_init_req;		/* init	every instance too? */
	miCBoolean	inst_init;		/* was instance	initialized? */
	miCBoolean	no_expl_params;		/* candidate for indirect par*/
	miCBoolean	spare[4];		/* not used */
	miCBoolean	simple;			/* PHEN: can use fast call */
	miUchar		ret_type;		/* PHEN:return type of shader*/
	miUchar		label;			/* sequential # for bitmasks */
	char		parameters[8];		/* parameter block, then */
						/* miTag ghost for phenomena */
} miFunction;
typedef struct miUserdata {
	miTag		data_decl;		/* parameter declaration */
	miTag		next_data;		/* next	data block in list */
	miUint		label;			/* data label */
	int		parameter_size;		/* size	of parameter block */
	short		one;			/* ==1, for byte order check */
	short		spare1;			/* not used */
	int		spare2;			/* not used */
	char		parameters[8];		/* parameter block */
} miUserdata;

/*------------ mi_shader.h ------------*/

/*
 * Enumeration of all types of rays to be traced
 * DO NOT renumber the flags, or precompiled shaders may stop working!
 *
 * volume shaders can find out
 * from state->type whether this ray came
 * from an object or from a light source
 */

typedef enum miRay_type {
	miRAY_EYE,		/* eye ray */
	miRAY_TRANSPARENT,	/* transparency ray */
	miRAY_REFLECT,		/* reflection ray */
	miRAY_REFRACT,		/* refraction ray */
	miRAY_LIGHT,		/* light ray */
	miRAY_SHADOW,		/* shadow ray */
	miRAY_ENVIRONMENT,	/* ray only into environment and volume */
	miRAY_NONE,		/* other ray */
	miPHOTON_ABSORB,		/* photon is absorbed (RIP) */
	miPHOTON_LIGHT,			/* photon emitted from a light source*/
	miPHOTON_REFLECT_SPECULAR,	/* specular reflection of a photon */
	miPHOTON_REFLECT_GLOSSY,	/* glossy reflection of a photon */
	miPHOTON_REFLECT_DIFFUSE,	/* diffuse reflection of a photon */
	miPHOTON_TRANSMIT_SPECULAR,	/* specular transmission of a photon */
	miPHOTON_TRANSMIT_GLOSSY,	/* glossy transmission of a photon */
	miPHOTON_TRANSMIT_DIFFUSE,	/* diffuse transmission of a photon */
	miRAY_DISPLACE,			/* displacement during tesselation */
	miRAY_OUTPUT,			/* output shader */
	miPHOTON_SCATTER_VOLUME,	/* volume scattering of a photon */
	miPHOTON_TRANSPARENT,		/* transparency photon */
	miRAY_FINALGATHER,		/* final gather ray */
	miRAY_NO_TYPES
} miRay_type;

/*
 * Determine if a ray given by miRay_type is an eye or first generation
 * transparency ray (primary), or a reflection or refraction (secondary) ray.
 */

#define miRAY_PRIMARY(r)	((r) == miRAY_EYE)
#define miRAY_SECONDARY(r)	((r) >	miRAY_EYE &&\
				 (r) <	miPHOTON_ABSORB ||\
				 (r) == miRAY_FINALGATHER)

/*
 * Determine if ray is a photon
 */

#define miRAY_PHOTON(r)		(((r) >= miPHOTON_ABSORB &&\
				 (r) <= miPHOTON_TRANSMIT_DIFFUSE) ||\
				 ((r) >= miPHOTON_SCATTER_VOLUME &&\
				  (r) <= miPHOTON_TRANSPARENT))

#define MAX_TEX		64	/* max. number of texture spaces */
#define NO_DERIVS	5	/* number of surface derivatives */

/*
 * Types of shaders
 */

typedef enum {
	miSHADER_LENS,		/* lens shader */
	miSHADER_MATERIAL,	/* material shader */
	miSHADER_LIGHT,		/* light shader */
	miSHADER_SHADOW,	/* shadow shader */
	miSHADER_ENVIRONMENT,	/* environment shader */
	miSHADER_VOLUME,	/* volume shader */
	miSHADER_TEXTURE,	/* texture shader */
	miSHADER_PHOTON,	/* forward shader */
	miSHADER_GEOMETRY,	/* geometry shader */
	miSHADER_DISPLACE,	/* displacement shader */
	miSHADER_PHOTON_EMITTER,/* photon emitter */
	miSHADER_OUTPUT,	/* output shader */
	miSHADER_OTHER,		/* shader not known by RC */
	miSHADER_NO_TYPES
} miShader_type;

/*
 * State structure
 */

#define MI_STATE_VERSION 2

typedef struct miState {
					/* global */
	int		version;		/* version of state struct,
						 * and shader interface
						 * currently 2 */
	miTag		camera_inst;		/* camera instance */
	struct miCamera	*camera;		/* camera */
	struct miOptions *options;		/* options */
	miLock		global_lock;		/* global lock for shaders */
	miUint		qmc_instance;		/* instance of low discrepacny
						 * vector associated to current
						 * ray tree */
#ifdef BIT64
	miUint		pad;
#endif
	struct miTs	*ts;			/* thread local storage (ts) */
						/* (internal) */
	float		raster_x;		/* x in raster space */
	float		raster_y;		/* y in raster space */
					/* ray */
	struct miState	*parent;		/* state of parent ray
						 * first eye ray/lens shader:
						 *   NULL
						 * subsequent eye rays/lens
						 * shaders: previous one
						 * reflection, refraction,
						 * dissolve rays: parent ray
						 * light rays: parent ray
						 * shadow rays: light ray */
	miRay_type	type;			/* type of the ray */
	miUchar		qmc_component;		/* next component of current
						 * instance of low discrepancy
						 * vector to be used */
	miCBoolean	scanline;		/* intersect ray by scanline?*/
	miCBoolean	inv_normal;		/* normals are inverted because
						 * ray hit the back side */
	void		*active;		/* active DepthSort entry
						 * for scanline
						 * internal to RC */
	void		*cache;			/* cache holding the beam
						 * of the ray
						 * for light and shadow rays
						 * it's a shadow cache
						 * internal to RC */
	int		reflection_level;	/* reflection depth of the ray
						 * SHOULD BE SHORT
						 */
	int		refraction_level;	/* refraction depth of the ray
						 * SHOULD BE SHORT
						 */
	miVector	org;			/* ray origin
						 * for light, shadow rays the
						 * light position */
	miVector	dir;			/* ray direction
						 * for light, shadow ray
						 * it points
						 * towards the intersection */
	double		dist;			/* length of the ray */
	float		time;			/* time of the ray */
	miScalar	ior;			/* ior of outgoing ray */
	miScalar	ior_in;			/* ior of incoming ray */
	miTag		material;		/* material of the primitive.
						 * Set to miNULL_TAG if no hit.
						 */
	miTag		volume;			/* volume shader to be applied
						 */
	miTag		environment;		/* environment shader to be
						 * applied if no hit */
					/* intersection point */
	miTag		refraction_volume;	/* volume shader to apply
						 * to refraction ray */
	miUint		label;			/* label of hit object */
	miTag		instance;		/* hit object instance */
	miTag		light_instance;		/* light instance */

	void		*pri;			/* box_info of hit box, shaders
						 * can check if	 pri== NULL
						 */
	int		pri_idx;		/* index of hit primitive
						 * in box */
	float		bary[4];		/* barycentric coordinates
						 * of hit primitive
						 * triangles: 0 - 2,
						 * imp. patches: 0 - 3 */
	miVector	point;			/* point of intersection */
	miVector	normal;			/* interpolated normal
						 * pointing to the side of the
						 * ray */
	miVector	normal_geom;		/* geometric normal of pri.
						 * pointing to the side of the
						 * ray */
	miScalar	dot_nd;			/* dot of normal, dir
						 * for light rays,
						 * it's the dot_nd of the
						 * parent (intersection) normal
						 * and the light ray dir */
	double		shadow_tol;		/* the minimum distance to a
						 * plane perpendicular to the
						 * shading normal for which the
						 * triangle is completely on
						 * the negative side.  */
	miVector	*tex_list;		/* list of texture coordinates
						 */
	miVector	*bump_x_list;		/* list of bump map basis x
						 * (perturbation) vectors */
	miVector	*bump_y_list;		/* list of bump map basis y
						 * (perturbation) vectors */
	miVector	motion;			/* motion vector or (0,0,0) */
	miVector	*derivs;		/* list of surface derivative
						 * vectors containing dx/du,
						 * dx/dv, d^2x/du^2, d^2x/dv^2,
						 * d^2x/dudv or (0,0,0) */
					/* light shader, shadow shader */
						/* instance is the
						 * light instance, see above */
					/* texture shader */
	miVector	tex;			/* texture coordinates for
						 * lookup */
					/* shader */
	struct miFunction *shader;		/* current shader */
					/* other */
	struct miState	*child;			/* child state */
	short		thread;			/* current thread number */
					/* user */
	void		*user;			/* user data */
	int		user_size;		/* size of user data */
} miState;

/*
 * a photonmap photon
 * this structure will very likely change in the future!
 */

typedef struct miPhoton {
	float		pos[3];	  /* miVector */
	unsigned short	flag;
	unsigned char	theta, phi;
	float		flux[3];   /* miColor */
} miPhoton;
/*
 * state structure passed to old-style output shaders. It is used for
 * output shaders whose declaration does not specify a version number.
 * Do not use this. Output shaders with nonzero version numbers get
 * a miState, not a miOutstate, and work with versioning and init/exit.
 */

#define MI_OUTSTATE_VERSION 2

typedef struct {			    /* state for output shaders */
	int		version;		/* version of miOutstate */
	miTag		camera_inst;		/* camera instance */
	struct miCamera	*camera;		/* camera */
	struct miRc_options *options;		/* options */
	int		xres, yres;		/* image resolution */
	miImg_image	*frame_rgba;		/* RGBA color frame buffer */
	miImg_image	*frame_z;		/* depth channel frame buffer*/
	miImg_image	*frame_n;		/* normal vector frame buffer*/
	miImg_image	*frame_label;		/* label channel frame buffer*/
	miMatrix	camera_to_world;	/* world transformation */
	miMatrix	world_to_camera;	/* inverse world transform */
	miImg_image	*frame_m;		/* motion vector frame buffer*/
} miOutstate;


/*
 * shader type
 */

typedef miBoolean (*miShader)(
		miColor		*result,	/* result */
		miState		*state,		/* state */
		void		*args);		/* shader args */

typedef miBoolean (*miShaderx)(
		miColor		*result,	/* result */
		miState		*state,		/* state */
		void		*args,		/* shader args */
		void		*arg);		/* arg from parent shader */

typedef miBoolean (*miShader_init)(
		miState		*state,		/* state */
		void		*args,		/* shader args or NULL */
		miBoolean	*init_req);	/* instance inits too? */

typedef miBoolean (*miShader_exit)(
		miState		*state,		/* state */
		void		*args);		/* shader args or NULL */

typedef int (*miShader_version)(void);

typedef void (*miContour_Store_Function)(
    void      *info_void,
    int	      *info_size,
    miState   *state,
    miColor   *color
);

typedef miBoolean (*miContour_Contrast_Function)(
    void     *info1,
    void     *info2,
    int	     level,
    miState  *state,
    void     *paras
);

struct miContour_endpoint;   /* typedef'd later */

typedef miBoolean (*miContour_Shader)(
    struct miContour_endpoint *epoint,
    void      *info_near,
    void      *info_far,
    miState   *state,
    void      *paras
);



/*
 * global variables
 */

/*
 * functions
 */

#define mi_eval( s, p ) (!(s)->shader->ghost_offs ? (void*)(p) : \
  (!*((miTag*)((char*)(p)+(s)->shader->ghost_offs)) ? (void*)(p) : \
  mi_phen_eval(s,(void*)(p))))

#define mi_eval_boolean(p)	((miBoolean *)mi_eval(state, (void *)(p)))
#define mi_eval_integer(p)	((miInteger *)mi_eval(state, (void *)(p)))
#define mi_eval_scalar(p)	((miScalar  *)mi_eval(state, (void *)(p)))
#define mi_eval_vector(p)	((miVector  *)mi_eval(state, (void *)(p)))
#define mi_eval_transform(p)	((miScalar  *)mi_eval(state, (void *)(p)))
#define mi_eval_color(p)	((miColor   *)mi_eval(state, (void *)(p)))
#define mi_eval_tag(p)		((miTag	    *)mi_eval(state, (void *)(p)))

						/* get address of function */
DLLIMPORT miBoolean mi_call_shader(
		miColor		*result,	/* result */
		miShader_type	type,		/* shader type */
		miState		*state,		/* state */
		miTag		shader);	/* shader function instance */
DLLIMPORT miBoolean mi_call_shader_x(
		miColor		*result,	/* result */
		miShader_type	type,		/* shader type */
		miState		*state,		/* state */
		miTag		shader,		/* shader function instance */
		void		*arg);		/* pass to shader */
DLLIMPORT void mi_flush_cache(
		miState *state );		/* the famous state */
DLLIMPORT miBoolean mi_call_material(		/* for smart volume shaders */
		miColor		*result,	/* material shader result */
		miState		*state);	/* state with state->material*/
DLLIMPORT miBoolean mi_call_photon_material(	/* for photon volume shaders */
		miColor		*energy,	/* energy (surprise!) */
		miState		*state);	/* state with state->material*/
DLLIMPORT void *mi_phen_eval(			/* evaluate a shader param */
		miState *state,			/* state */
		void *parm );			/* parameter */
DLLIMPORT miBoolean mi_lookup_color_texture(
		miColor	* const	  color,	/* returned color */
		miState	* const	  state,
		const miTag	  tex,		/* texture to look up */
		miVector * const  coord);	/* texture coordinate */
DLLIMPORT miBoolean mi_lookup_scalar_texture(
		miScalar * const  scalar,	/* returned scalar */
		miState	* const	  state,
		const miTag	  tex,		/* texture to look up */
		miVector * const  coord);	/* texture coordinate */
DLLIMPORT miBoolean mi_lookup_vector_texture(
		miVector * const  vector,	/* returned vector */
		miState	* const	  state,
		const miTag	  tex,		/* texture to look up */
		miVector * const  coord);	/* texture coordinate */
/*
 * texture filtering
 */

typedef struct {
	miScalar	eccmax;			/* max. eccentricity */
	miScalar	max_minor;		/* max. minor axis length */
	miScalar	circle_radius;		/* scale blurring */
	miBoolean	bilinear;		/* bilin. subpixel interp. */
	miScalar	spare[10];		/* not used */
} miTexfilter;

DLLIMPORT miBoolean mi_lookup_filter_color_texture(
		miColor * const color,		/* returned color */
		miState * const state,
		const miTag	tex,		/* texture to look up */
		const miTexfilter *paras,	/* filter options */
		miMatrix	ST);		/* pixel->texture transform */

DLLIMPORT miBoolean mi_texture_filter_project(
		miVector	p[3],		/* pixel space points */
		miVector	t[3],		/* texture space points */
		miState *const	state,
		miScalar	disc_r,		/* projection disc radius */
		miUint		space);		/* texture space index */

DLLIMPORT miBoolean mi_texture_filter_transform(
		miMatrix	ST,		/* screen->texture transform */
		miVector	p[3],		/* pixel space points */
		miVector	t[3]);		/* texture space points */

/* info functions */
DLLIMPORT void	  mi_texture_info(
		const miTag	tag,		/* texture to inquire about */
		int * const	xresp,		/* store width here */
		int * const	yresp,		/* store height here */
		void ** const	paraspp);	/* store ptr to tex paras */
DLLIMPORT void	  mi_light_info(
		const miTag	tag,		/* light to inquire about */
		miVector *const	orgp,		/* store origin here */
		miVector *const	dirp,		/* store direction here */
		void ** const	paraspp);	/* store ptr to light paras 0*/
DLLIMPORT int	    mi_global_lights_info(
		miTag		**tag);		/* array of light leaf insts */
DLLIMPORT void	**mi_shader_info(
		miState	* const	state);		/* state with shader */
DLLIMPORT int	  mi_instance_info(
		const miTag	inst_tag,	/* instance tag */
		void ** const	paraspp,	/* store ptr to inst paras */
		void ** const	spare1,		/* not currently used */
		void ** const	spare2,		/* not currently used */
		void ** const	spare3);	/* not currently used */

DLLIMPORT miBoolean mi_inclusive_lightlist(
		int		*n_lights,	/* number of lights */
		miTag		**lights,	/* lightlist (array of tags) */
		miState		*state);	/* state */
DLLIMPORT miBoolean mi_exclusive_lightlist(
		int		*n_lights,	/* number of lights */
		miTag		**lights,	/* lightlist (array of tags) */
		miState		*state);	/* state */

DLLIMPORT miBoolean mi_tri_vectors(
		miState		*state,
		int		which,		/* point, normal, motion, tex*/
		int		ntex,		/* if which==t, which texture*/
		miVector	**a,		/* returned vectors */
		miVector	**b,
		miVector	**c);

/* ray tracing functions */
DLLIMPORT miBoolean mi_trace_eye(
		miColor		*result,	/* result */
		miState		*state,		/* state */
		miVector	*origin,	/* ray origin */
		miVector	*direction);	/* ray direction */
DLLIMPORT miBoolean mi_trace_transparent(
		miColor		*result,	/* result */
		miState		*state);	/* state */
DLLIMPORT miBoolean mi_trace_reflection(
		miColor		*result,	/* result */
		miState		*state,		/* state */
		miVector	*direction);	/* ray direction */
DLLIMPORT miBoolean mi_trace_refraction(
		miColor		*result,	/* result */
		miState		*state,		/* state */
		miVector	*direction);	/* ray direction */
DLLIMPORT miBoolean mi_trace_environment(
		miColor		*result,	/* result */
		miState		*state,		/* state */
		miVector	*direction);	/* direction */
DLLIMPORT miBoolean mi_trace_probe(
		miState		*state,		/* state */
		miVector const	*direction,	/* direction */
		miVector const	*origin);	/* origin */
DLLIMPORT miBoolean mi_sample_light(
		miColor * const	result,		/* result */
		miVector *const	dir,		/* opt. result direction */
		float	 *const	dot_nl,		/* opt. result dot of N, L */
		miState * const	state,		/* state */
		const miTag	light_inst,	/* light instance */
		int * const	samples);	/* # samples */
DLLIMPORT miBoolean mi_trace_light(
		miColor		*result,	/* result */
		miVector	*dir,		/* opt. result direction */
		float		*dot_nl,	/* opt. result dot of N, L */
		miState		*state,		/* state */
		miTag		light_inst);	/* light instance */
DLLIMPORT miBoolean mi_trace_shadow(
		miColor * const	result,		/* result */
		miState * const	state);		/* state */
DLLIMPORT miBoolean mi_trace_shadow_seg(
		miColor * const	result,		/* result */
		miState * const	state);		/* state */

/* compute reflection / transmission directions */
DLLIMPORT void mi_reflection_dir(
		miVector	*dir,		/* in direction */
		miState		*state);	/* state */
DLLIMPORT miBoolean mi_refraction_dir(
		miVector	*dir,		/* in direction */
		miState		*state,		/* state */
		miScalar	ior_in,		/* in index of refraction */
		miScalar	ior_out);	/* out index of refraction */
DLLIMPORT void mi_reflection_dir_specular(	/* same as mi_reflection_dir */
		miVector	*dir,		/* in direction */
		miState		*state);	/* state */
DLLIMPORT void mi_reflection_dir_glossy(
		miVector	*dir,		/* out direction */
		miState		*state,		/* state */
		miScalar	shiny);		/* shiny coefficient */
DLLIMPORT void mi_reflection_dir_anisglossy(
		miVector	*dir,		/* out direction */
		miState		*state,		/* state */
		miVector	*u,		/* u direction */
		miVector	*v,		/* v direction */
		miScalar	shiny_u,	/* shiny coefficient in u dir*/
		miScalar	shiny_v);	/* shiny coefficient in v dir*/
DLLIMPORT void mi_reflection_dir_diffuse(
		miVector	*dir,		/* out direction */
		miState		*state);	/* state */
DLLIMPORT miBoolean mi_transmission_dir_specular( /*same as mi_refraction_dir*/
		miVector	*dir,		/* in direction */
		miState		*state,		/* state */
		miScalar	ior_in,		/* in index of refraction */
		miScalar	ior_out);	/* out index of refraction */
DLLIMPORT miBoolean mi_transmission_dir_glossy(
		miVector	*dir,		/* out direction */
		miState		*state,		/* state */
		miScalar	ior_in,		/* in index of refraction */
		miScalar	ior_out,	/* out index of refraction */
		miScalar	shiny);		/* shiny coefficient */
DLLIMPORT miBoolean mi_transmission_dir_anisglossy(
		miVector	*dir,		/* out direction */
		miState		*state,		/* state */
		miScalar	ior_in,		/* in index of refraction */
		miScalar	ior_out,	/* out index of refraction */
		miVector	*u,		/* u direction */
		miVector	*v,		/* v direction */
		miScalar	shiny_u,	/* shiny coefficient in u dir*/
		miScalar	shiny_v);	/* shiny coefficient in v dir*/
DLLIMPORT void mi_transmission_dir_diffuse(
		miVector	*dir,		/* out direction */
		miState		*state);	/* state */
DLLIMPORT void mi_scattering_dir_diffuse(
		miVector	*dir,		/* result: out direction */
		miState		*state);
DLLIMPORT void mi_scattering_dir_directional(
		miVector	*dir,		/* result: out direction */
		miState		*state,
		miScalar	directionality);/* -1: back ... 1: forward */
DLLIMPORT miScalar mi_scattering_pathlength(
		miState	 *state,
		miScalar k);

/* choose scatter type */
DLLIMPORT miRay_type mi_choose_scatter_type(
		miState		*state,
		miScalar	transp,		/* refl = 1-transp */
		miColor		*diffuse,
		miColor		*glossy,
		miColor		*specular);
DLLIMPORT miRay_type mi_choose_simple_scatter_type(
		miState		*state,
		miColor		*rdiffuse,
		miColor		*rspecular,
		miColor		*tdiffuse,
		miColor		*tspecular);
DLLIMPORT int mi_choose_lobe(
		miState		*state,
		miScalar	r);		/* probability of first lobe */

/* evalution of reflection models */
DLLIMPORT miScalar mi_phong_specular(
		miScalar	spec_exp,	/* specular exponent */
		miState		*state,		/* state */
		miVector	*dir);		/* direction */
DLLIMPORT miScalar mi_blong_specular(
		miScalar	spec_exp,	/* specular exponent */
		miState		*state,		/* state */
		miVector	*dir);		/* direction */
DLLIMPORT miScalar mi_blinn_specular(
		miVector	*di,		/* incident direction */
		miVector	*dr,		/* direction of reflection */
		miVector	*n,		/* surface normal */
		miScalar	roughness,	/* average microfacet slope */
		miScalar	ior);		/* index of refraction */
DLLIMPORT miBoolean mi_cooktorr_specular(
		miColor		*result,	/* resulting color */
		miVector	*di,		/* incident direction */
		miVector	*dr,		/* direction of reflection */
		miVector	*n,		/* surface normal */
		miScalar	roughness,	/* average microfacet slope */
		miColor		*ior);	   /* relative ior for 3 wavelengths */
DLLIMPORT void mi_fresnel_specular(
		miScalar	*ns,
		miScalar	*ks,
		miScalar	spec_exp,	/* specular exponent */
		miState		*state,		/* state */
		miVector	*dir,		/* direction */
		miScalar	ior_in,		/* in index of refraction */
		miScalar	ior_out);	/* out index of refraction */
DLLIMPORT miScalar mi_ward_glossy(
		miVector	*di,		/* incident direction */
		miVector	*dr,		/* direction of reflection */
		miVector	*n,		/* normal */
		miScalar	shiny);		/* glossyness */
DLLIMPORT miScalar mi_ward_anisglossy(
		miVector	*di,		/* incident direction */
		miVector	*dr,		/* direction of reflection */
		miVector	*n,		/* normal */
		miVector	*u,		/* u direction */
		miVector	*v,		/* v direction */
		miScalar	shiny_u,	/* glossyness in u direction */
		miScalar	shiny_v);	/* glossyness in u direction */
DLLIMPORT miScalar mi_fresnel(
		miScalar	n1,
		miScalar	n2,
		miScalar	t1,
		miScalar	t2);
DLLIMPORT miScalar mi_fresnel_reflection(
		miState		*state,
		miScalar	ior_in,
		miScalar	ior_out);
DLLIMPORT miScalar mi_schlick_scatter(
		miVector  *di,			/* incident direction */
		miVector  *dr,			/* direction of reflection */
		miScalar  directionality);	/* -1:back, 0:isot., 1:forw. */

/* photon tracing */
DLLIMPORT miBoolean mi_photon_reflection_specular(
		miColor		*energy,	/* energy carried by photon */
		miState		*state,
		miVector	*dir);
DLLIMPORT miBoolean mi_photon_reflection_glossy(
		miColor		*energy,	/* energy carried by photon */
		miState		*state,
		miVector	*dir);
DLLIMPORT miBoolean mi_photon_reflection_diffuse(
		miColor		*energy,	/* energy carried by photon */
		miState		*state,
		miVector	*dir);
DLLIMPORT miBoolean mi_photon_transmission_specular(
		miColor		*energy,	/* energy carried by photon */
		miState		*state,
		miVector	*dir);
DLLIMPORT miBoolean mi_photon_transmission_glossy(
		miColor		*energy,	/* energy carried by photon */
		miState		*state,
		miVector	*dir);
DLLIMPORT miBoolean mi_photon_transmission_diffuse(
		miColor		*energy,	/* energy carried by photon */
		miState		*state,
		miVector	*dir);
DLLIMPORT miBoolean mi_photon_transparent(
		miColor		*energy,	/* photon flux */
		miState		*parent);
DLLIMPORT miBoolean mi_photon_light(
		miColor		*energy,
		miState		*state );
DLLIMPORT miBoolean mi_photon_volume_scattering(
		miColor		*energy,	/* energy carried by photon */
		miState		*state,
		miVector	*dir);

/* photon map interface */
DLLIMPORT miBoolean mi_store_photon(
		miColor		*energy,	/* energy carried by photon */
		miState		*state);
DLLIMPORT miBoolean mi_compute_irradiance(
		miColor		*result,
		miState		*state);
DLLIMPORT miBoolean mi_store_volume_photon(
		miColor		*energy,	/* energy carried by photon */
		miState		*state);
DLLIMPORT miBoolean mi_compute_volume_irradiance(
		miColor		*result,
		miState		*state);
DLLIMPORT int mi_get_photons(
		const miState	*state,		/* current state */
		const char	type,		/* which photon map? c,g,v? */
		const int	reqno,		/* requested photons */
		miPhoton	***photons,	/* the result photon array */
		float		**dist2 );	/* squared distances */
DLLIMPORT void mi_photon_direction(
		const miPhoton	*p,
		miVector	*dir );

/*
 * user frame buffer access
 */

DLLIMPORT miBoolean mi_fb_put(
		miState		*state,		/* current shader state */
		int		fb,		/* frame buffer number, 0..7 */
		void		*data);		/* sample data to store */

DLLIMPORT miBoolean mi_fb_get(
		miState		*state,		/* current shader state */
		int		fb,		/* frame buffer number, 0..7 */
		void		*data);		/* sample data to store */

/*
 * contour output shader interface
 */

typedef struct miContour_endpoint {
    miVector  point;  /* x, y in screen coordinates, z in camera coordinates */
    miColor   color;
    float     width;
    miVector  motion;
    miVector  normal;
    miTag     material;
    int	      label;
} miContour_endpoint;

DLLIMPORT miBoolean mi_get_contour_line(
		miContour_endpoint *p1,
		miContour_endpoint *p2);

DLLIMPORT void mi_add_contour_lines(
		miContour_endpoint p1[],   /* list of first endpoints */
		miContour_endpoint p2[],   /* list of second endpoints */
		int n);			   /* number of contour lines */



/*
 * parallel safe random number generator
 */

DLLIMPORT double mi_par_random( miState *state );

/*
 * more functions, defined by LIB. They are here because they need miState.
 * All take the state, a result vector pointer, and an input vector pointer.
 */

DLLIMPORT void mi_point_to_world(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_to_camera(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_to_object(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_to_raster(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_to_light(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_from_world(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_from_camera(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_from_object(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_point_from_light(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_to_world(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_to_camera(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_to_object(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_to_light(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_from_world(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_from_camera(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_from_object(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_vector_from_light(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_to_world(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_to_camera(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_to_object(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_to_light(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_from_world(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_from_camera(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_from_object(
		miState *const, miVector *const, miVector *const);
DLLIMPORT void mi_normal_from_light(
		miState *const, miVector *const, miVector *const);

/*
 * geometry shaders
 */

DLLIMPORT miBoolean
mi_geoshader_add_result(
	miTag			*result,	/* shader result */
	const miTag		item);		/* scene item to add */

typedef struct {				/* intersection description */
	double			dist;		/* distance along ray */
	miVector		point;		/* point in render space */
	miVector		normal_geom;	/* geometric normal */
	float			bary[4];	/* barycentric coord. 0..2 */
	void			*pri;		/* internal primitive */
	int			pri_idx;	/* primitive index */
	miTag			instance;	/* hit instance */
} miIntersect;


/*------------ mi_phen.h ------------*/

DLLIMPORT miTag		mi_phen_clone(struct miState *, miTag);
DLLIMPORT void		mi_phen_unclone(miTag);

/*------------ mi_smethod.h ------------*/

DLLEXPORT miBoolean mi_sample(
	double *sample, 
	int *instance,
	struct miState *state, 
	const miUshort dimension, 
	miUint *n);

#ifdef __cplusplus
}
#endif
#endif
