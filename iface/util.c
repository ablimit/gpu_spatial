//
//
//	util
//
//		Utility functions.
//
//
//	
#include "postgres.h"
#include "fmgr.h"
#include "utils/geo_decls.h"
#include "liblwgeom.h"

//	
//	 liblwgeom allocator callback - install the defaults (malloc/free/stdout/stderr)
//		
//		Needed by any code using the lwgeom library.
//
//
void lwgeom_init_allocators()
{
	lwgeom_install_default_allocators();
}





//
//	Utility method to call the serialization and then set the
// 	PgSQL varsize header appropriately with the serialized size.
//
//
GSERIALIZED* geometry_serialize(LWGEOM *lwgeom)
{
	static int is_geodetic = 0;
	size_t ret_size = 0;
	GSERIALIZED *g = NULL;

	g = gserialized_from_lwgeom(lwgeom, is_geodetic, &ret_size);

	SET_VARSIZE(g, ret_size);
	return g;
}

