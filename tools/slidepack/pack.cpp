//	
//	pack.cpp
//
//		Utility to create BLOBS from the markup polygons in the PAIS 
//		database.
//
//			
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include "libpq-fe.h"
#include "cmdline.h"


using namespace std;



struct PolyIndex {
	char	paisUid[65];
	char	tilename[65];
	int		markupId;
	int		index;
	int		vertCnt;
};




//
//	Connect to the specified database as the specified user
//
PGconn* Connect(string username, string password,
				string hostname, string database)
{
	PGconn *conn = NULL;
	string connectStr;
	
	connectStr += "user=";
	connectStr += username;
	if( !password.empty() ) {
		connectStr += " password=";
		connectStr += password;
	}
	connectStr += " host=";
	connectStr += hostname;
	connectStr += " dbname=";
	connectStr += database;

	conn = PQconnectdb(connectStr.c_str());
	
	if( PQstatus(conn) != CONNECTION_OK ) {
		cerr << "Connection failed" << endl;
		PQfinish(conn);
		conn = NULL;
	}
	return conn;
}



//	
//	Extract the vertices from polygon and insert it into the 'verts'
//	and 'index' arrays.
//
//
void ParsePolygon(string polygon, float *xVerts, float *yVerts,
				  PolyIndex& poly, int& vertBase)
{
	int		pos, end, vertCnt = 0;
	string	num; 

	// Mark start of polygon
	poly.index = vertBase;
	
	// Skip to end of POLYGON((
	pos = polygon.find("((");
	pos++;
	
	// Extract each coordinate pair
	while( pos != string::npos ) {
		end = polygon.find(" ", pos + 1);
		num = polygon.substr(pos + 1, end - pos);
		xVerts[vertBase + vertCnt] = atof(num.c_str());
		
 		pos = polygon.find(",", pos + 1);
 		if( pos == string::npos ) {
 			int last = polygon.find(")", pos + 1);
			num = polygon.substr(end + 1, last - end - 1);
 		} else {
			num = polygon.substr(end + 1, pos - end - 1);
		}
		yVerts[vertBase + vertCnt] = atof(num.c_str());
		vertCnt++;
	}
	vertBase += vertCnt;
	poly.vertCnt = vertCnt;
}



//
// 	Get all the polygons, tilenames and markup ids for the specified
// 	pais uid. 'vertices' holds the vertices of all polygons, 'index'
//	describes the polygon and its start position in 'vertices'
//
//
bool GetRecords(PGconn *conn, string uid, PolyIndex	*&index, 
				float *&xVerts, float *&yVerts, 
				int& vertCnt, int& polyCnt)
{
	bool	result = true;
	int		nFields, nRecs;
	
	// Start the transaction
	PGresult *dbRes = PQexec(conn, "BEGIN");
	if( PQresultStatus(dbRes) != PGRES_COMMAND_OK ) {
		cerr << "BEGIN command failed" << endl;
		result = false;
	}
	PQclear(dbRes);
	
	// Create a cursor to retrieve 
	if( result ) {
		dbRes = PQexec(conn, "DECLARE polyRec CURSOR FOR select id, ST_AsText(polygon) from test_polygons");
		if( PQresultStatus(dbRes) != PGRES_COMMAND_OK ) {
			cerr << "DECLARE CURSOR failed" << endl;
			result = false;	
		}
	}
	PQclear(dbRes);
	
	if( result ) {
		dbRes = PQexec(conn, "FETCH ALL in polyRec");
		if( PQresultStatus(dbRes) != PGRES_TUPLES_OK ) {
			cerr << "FETCH ALL failed" << endl;
			result = false;
		}
	}

	if( result ) {
		string 	value;
		int		polyIdx = 0, vertIdx = 0, vertSize;
		
		nFields = PQnfields(dbRes);
		nRecs = PQntuples(dbRes);
		
		index = (PolyIndex*)malloc(nRecs * sizeof(*index));
		vertSize = nRecs * 20;
		xVerts = (float*)malloc(vertSize * sizeof(float));
		yVerts = (float*)malloc(vertSize * sizeof(float));
		
		if( index && xVerts && yVerts ) {
			for(int i = 0; i < nRecs; i++) {
	
				value = PQgetvalue(dbRes, i, 1);
				ParsePolygon(value, xVerts, yVerts, index[polyIdx++], vertIdx);
					
				if( vertIdx > 0.9f * vertSize ) {
					float *temp;
						
					vertSize *= 0.5f;
					temp = (float*)realloc(xVerts, vertSize * sizeof(float));
					if( temp )
						xVerts = temp;
					else {
						cerr << "Unable to adjust vert buffer" << endl;
						result = false;
					}
						
					temp = (float*)realloc(yVerts, vertSize * sizeof(float));
					if( temp )
						yVerts = temp;
					else {
						cerr << "Unable to adjust vert buffer" << endl;
						result = false;
					}
				}
			}
			vertCnt = vertIdx;
			polyCnt = polyIdx;
		} else {
			cerr << "Unable to allocate polygon arrays" << endl;
			result = false;
		}
		dbRes = PQexec(conn, "CLOSE polyRec");
		PQclear(dbRes);
	
		dbRes = PQexec(conn, "END");
		PQclear(dbRes);
	}
	return result;
}




//
//	Saves to a binary file... Will add the ability to store as a
//	table in the database.
//
//
bool SaveBlob(string paisUid, PolyIndex *pIdx, float* x, float *y, 
			  int vertCnt, int polyCnt)
{
	bool	result = true;
	
	paisUid += ".ply";
	
	cout << "Saving to: " << paisUid << endl;
	cout << polyCnt << " polygons and " << vertCnt << " vertices" << endl;
	
	ofstream outFile(paisUid.c_str(), ios::out | ios::binary);
	
	if( outFile.is_open() ) {
	
		outFile.write((char*)&polyCnt, sizeof(polyCnt));
		outFile.write((char*)&vertCnt, sizeof(vertCnt));
	
		outFile.write((char*)pIdx, polyCnt * sizeof(*pIdx));
		outFile.write((char*)x, vertCnt * sizeof(*x));
		outFile.write((char*)y, vertCnt * sizeof(*y));
		
		outFile.close();
	} else {
		cerr << "Unable to create output file" << endl;
		result = false;
	}
	return result;
}





int main(int argc, char *argv[])
{
	gengetopt_args_info		args;
	PGconn					*conn; 
	float					*x = NULL, *y = NULL;
	PolyIndex				*pIdx = NULL;
	int						vertCnt, polyCnt;
	
	if( cmdline_parser(argc, argv, &args) != 0 ) {
		exit(-1);
	}
	string 	hostname = args.server_arg, 
			username = args.username_arg,  
			password = args.password_given ? args.password_arg : "", 
			database = args.database_arg,
			paisUid = args.pais_uid_given ? args.pais_uid_arg : "poly";
	
	
	conn = Connect(username, password, hostname, database);
	if( conn ) {
		cout << "---------- Connected ----------" << endl;
		GetRecords(conn, paisUid, pIdx, x, y, vertCnt, polyCnt);
		SaveBlob(paisUid, pIdx, x, y, vertCnt, polyCnt);
		cout << "---------- Done ----------" << endl;
		PQfinish(conn);
	}
	return 0;
}