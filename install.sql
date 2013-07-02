create or replace function gpu_info() 
returns text  
as '/Users/nalisnik/gpu_spatial.so', 'gpu_info' 
language C strict;


create or replace function gpu_Area(geometry) 
returns float8  
as '/Users/nalisnik/gpu_spatial.so', 'gpu_area' 
language C strict;


create or replace function gpu_Intersection(geometry, geometry) 
returns geometry
as '/Users/nalisnik/gpu_spatial.so', 'gpu_intersection' 
language C strict;



create or replace function gpu_Union(geometry, geometry) 
returns geometry
as '/Users/nalisnik/gpu_spatial.so', 'gpu_union' 
language C strict;
