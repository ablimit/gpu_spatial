//
//
//	Postgis interface functions
//
//
//	To compile:
//		gcc -I/usr/local/pgsql/include/server -c gpu-api.c 
//
//	To Link:
//		Linux: 	gcc -shared -fPIC -o gpu_lib.so gpu-api.o gpu-info.o
//		Mac: 	gcc -shared -flat_namespace -undefined suppress -o gpu_lib.so gpu-info.o gpu-api.o -L /
//
//	Version 1 calling convention
//
#include <string.h>
#include <stdlib.h>
#include "postgres.h"
#include "fmgr.h"
#include "utils/geo_decls.h"
#include "liblwgeom.h"
#include "util.h"
#include "gpu-iface.h"


//
//	In poly.c
//
LWGEOM* Intersect(LWGEOM *poly1, LWGEOM *poly2);




#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif







//
//	Returns the number of GPU's detected and the name
//	and compute level of the FIRST device only.
//
//
PG_FUNCTION_INFO_V1(gpu_info);

Datum gpu_info(void)
{
	text 	*response;
	int32	responseSize;
	char 	*info = NULL;
	
	info = gi_device_info();
	responseSize = strlen(info) + VARHDRSZ;
	response = (text*)palloc0(responseSize);
	
	SET_VARSIZE(response, responseSize);
	memcpy(VARDATA(response), info, strlen(info));
	
	free(info);
	
	PG_RETURN_TEXT_P(response);
}






PG_FUNCTION_INFO_V1(gpu_area);
Datum gpu_area(PG_FUNCTION_ARGS)
{
	GSERIALIZED *geom = (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	LWGEOM *lwgeom = lwgeom_from_gserialized(geom);
	double area = 0.0;


	PG_RETURN_FLOAT8(area);
}






PG_FUNCTION_INFO_V1(gpu_intersection);
Datum gpu_intersection(PG_FUNCTION_ARGS)
{
	GSERIALIZED		*param1 = NULL, *param2 = NULL, *result = NULL;
	LWGEOM			*geom1, *geom2, *lwresult;

	param1 = (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	param2 = (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(1));
	
	geom1 = lwgeom_from_gserialized(param1);
	geom2 = lwgeom_from_gserialized(param2);

	lwresult = Intersect(geom1, geom2);
	if( lwresult ) {
		result = geometry_serialize(lwresult);
		lwgeom_free(lwresult);
	}
	
	if( result == NULL )
		PG_RETURN_NULL();
	else
		PG_RETURN_POINTER(result);
}






PG_FUNCTION_INFO_V1(gpu_union);
Datum gpu_union(PG_FUNCTION_ARGS)
{
	GSERIALIZED *geom1;
	GSERIALIZED *geom2;
	GSERIALIZED *result;
	LWGEOM *lwgeom1, *lwgeom2, *lwresult ;

	geom1 = (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	geom2 = (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(1));

	lwgeom1 = lwgeom_from_gserialized(geom1);
	lwgeom2 = lwgeom_from_gserialized(geom2);


	PG_RETURN_NULL();
}






// Test code -- PostGIS geometry interface trial
#if 0
PG_FUNCTION_INFO_V1(gpu_x_point);
Datum gpu_x_point(PG_FUNCTION_ARGS)
{
	GSERIALIZED		*geom;
	LWGEOM			*lwgeom;
	LWPOINT			*point = NULL;
	POINT2D			p;
		
	geom = (GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
	

	if( gserialized_get_type(geom) == POINTTYPE ) {
	
		lwgeom = lwgeom_from_gserialized(geom);
		point = lwgeom_as_lwpoint(lwgeom);
		if( lwgeom_is_empty(lwgeom) )
			PG_RETURN_NULL();
		else
			getPoint2d_p(point->point, 0, &p);	
		
		PG_FREE_IF_COPY(geom, 0);
	} else {
		PG_RETURN_NULL();
	}

	PG_RETURN_FLOAT8(p.x);	
}
#endif






