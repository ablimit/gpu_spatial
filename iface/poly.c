//
//
//	poly	
//
//		Polygon functions for the postgis interface.
//	
#include "postgres.h"
#include "liblwgeom.h"
#include "gpu-iface.h"



// How to construct a polygon and access input polugons
//	LWPOLY		*lwpoly = NULL;
//	POINTARRAY	*ptArray = NULL, *poly1Pta;
//	POINTARRAY 	**ppa = NULL;
//	LWMPOLY		*mPoly = NULL;
//	LWGEOM		*lwresult = NULL;

//	LWPOLY		*temp = (LWPOLY*)poly1;
//	POINTARRAY *tempPoly = temp->rings[0];
	
	// Construct geometry to return
//	ptArray = ptarray_construct_empty(0, 0, 4);
//	if( ptArray ) {
//		POINT4D p; 
//		getPoint4d_p(tempPoly, 2, &p);
		
//		ptarray_append_point(ptArray, &p, LW_TRUE);

//		p.x = 10.0;
//		p.y = 20.0;
//		p.z = 0.0;
//		p.m =  0.0;
//		ptarray_append_point(ptArray, &p, LW_TRUE);

//		p.x = 20.0;
//		p.y = 20.0;
//		p.z = 0.0;
//		p.m =  0.0;
//		ptarray_append_point(ptArray, &p, LW_TRUE);

//		p.x = 20.0;
//		p.y = 10.0;
//		p.z = 0.0;
//		p.m =  0.0;
//		ptarray_append_point(ptArray, &p, LW_TRUE);
	
//		ppa = (POINTARRAY**)palloc(sizeof(POINTARRAY**));
//		ppa[0] = ptArray;

//		lwpoly = lwpoly_construct(SRID_UNKNOWN, NULL, 1, ppa);
	
//		mPoly = lwmpoly_construct_empty(SRID_UNKNOWN, 0, 0);
//		mPoly = lwmpoly_add_lwpoly(mPoly, lwpoly);
//		lwresult = lwmpoly_as_lwgeom(mPoly);
//	}

//	return lwresult;


int ConvertToVertex(POINTARRAY *pa, VERTEX **va, int *size)
{
	int 	result = 1, idx;
	VERTEX	*ptr;
	
	*size = pa->npoints;
	*va = (VERTEX*)palloc(*size * sizeof(VERTEX));
	
	if( va ) {
		ptr = *va;
		POINT4D p;
	
		memset(*va, 0, *size * sizeof(VERTEX));

		for(idx = 0; idx < *size; idx++) {
			getPoint4d_p(pa, idx, &p);
			ptr[idx].x = p.x;
			ptr[idx].y = p.y;
		}
	} else {
		result = 0;
	}
	
	return result;
}





LWGEOM* Intersect(LWGEOM *poly1, LWGEOM *poly2)
{
	LWPOLY		*lwpoly = NULL;
	POINTARRAY	*ptArray = NULL, *poly1Pta;
	POINTARRAY 	**ppa = NULL;
	LWMPOLY		*mPoly = NULL;
	LWGEOM		*lwresult = NULL;

	LWPOLY		*in1 = (LWPOLY*)poly1, *in2 = (LWPOLY*)poly2;
	POINTARRAY 	*pa1 = in1->rings[0], *pa2 = in2->rings[0];
	
	VERTEX		*inPoly1 = NULL, *inPoly2 = NULL; 
	int			poly1Size, poly2Size;
	
	ConvertToVertex(pa1, &inPoly1, &poly1Size);
	ConvertToVertex(pa2, &inPoly2, &poly2Size);
		
	
	// Construct geometry to return
	ptArray = ptarray_construct_empty(0, 0, 4);
	if( ptArray ) {
		POINT4D p;
		getPoint4d_p(pa1, 2, &p);
		
		ptarray_append_point(ptArray, &p, LW_TRUE);

		p.x = 10.0;
		p.y = 20.0;
		p.z = 0.0;
		p.m =  0.0;
		ptarray_append_point(ptArray, &p, LW_TRUE);

		p.x = 20.0;
		p.y = 20.0;
		p.z = 0.0;
		p.m =  0.0;
		ptarray_append_point(ptArray, &p, LW_TRUE);

		p.x = 20.0;
		p.y = 10.0;
		p.z = 0.0;
		p.m =  0.0;
		ptarray_append_point(ptArray, &p, LW_TRUE);
	
		ppa = (POINTARRAY**)palloc(sizeof(POINTARRAY**));
		ppa[0] = ptArray;

		lwpoly = lwpoly_construct(SRID_UNKNOWN, NULL, 1, ppa);
	
		mPoly = lwmpoly_construct_empty(SRID_UNKNOWN, 0, 0);
		mPoly = lwmpoly_add_lwpoly(mPoly, lwpoly);
		lwresult = lwmpoly_as_lwgeom(mPoly);
	}

	return lwresult;
}



