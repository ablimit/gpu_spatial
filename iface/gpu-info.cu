//
//
//	gpu-info
//
//		Returns information on the installed GPU device.
//
//
//	
#include <cstdlib>
#include <cstring>
#include <cstdio>


#define  VER	"0.1"


char* device_info(void)
{
	cudaDeviceProp		prop;
	int					count;
	cudaError_t			result;
	char				*buffer;
	
	buffer = (char*)malloc(1024);
	if( buffer ) {
	
		result = cudaGetDeviceCount(&count);
		if( result == cudaSuccess ) {

			if( count > 0 ) {
				result = cudaGetDeviceProperties(&prop, 0);
				if( result == cudaSuccess )
					sprintf(buffer, "%s (%d.%d), count: %d, lib ver: %s", 
							prop.name, prop.major, prop.minor, count, VER);
			} else {
				sprintf(buffer, "No gpu devices detected");
			}
		}
	}
	return buffer;
}





//
//
//	 C wrapper for the Postgres plugin
//
//
extern "C" {

char* gi_device_info(void) 
{
	return device_info();
}

}
