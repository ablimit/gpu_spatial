
#include "postgres.h"
#include "lwgeom_functions_analytic.h"

__global__ void contains(PG_FUNCTION_ARGS)
{

  GSERALIZED *geom1, *geom2;
  GEOSGeometry *g, *h;
  GBOX b1, b2;
  int type1, type2;
  LWGEOM *lwg;
  LWPOINT *lwp;
  bool result;

  geom1= (GSERIALIZED *) PG_DETOAST_DATUM(PG_GETARG_DATUM(0));
  geom2= (GSERIALIZED *) PG_DETOAST_DATUM(PG_GETARG_DATUM(1));

  lwp = lwgeom_as_point(lwgeom_from_gseralized(geom2));

  result = point_in_polygon((LWPOLY*) lwg, lwp);

  PG_RETURN_BOOL(result);

}
