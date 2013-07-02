#if !defined(_GPU_IFACE_H_)
#define _GPU_IFACE_H_

typedef struct vertex {
	float	x;
	float	y;
	float	alpha;
	int		next;
	bool	internal;
	int		linkTag;
}VERTEX;



char *gi_device_info(void);



#endif