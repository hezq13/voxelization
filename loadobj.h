#ifndef LOADOBJ_H
#define LOADOBJ_H

#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<map>

using namespace std;

//vertex of triangle 
typedef struct TriangleDot{
    double x;
    double y;
    double z;
}TriangleDot;

//face of triangle
typedef struct TriangleFace{
    int dot[3];
}TriangleFace;

//key of map,the position of voxel
typedef struct st{
    int x,y,z;
    st():x(0),y(0),z(0){}
    st(int a,int b,int c):x(a),y(b),z(c){}
    bool operator < (const st &a)const{
	return (x<a.x || ((x==a.x)&&(y<a.y)) || ((x==a.x) && (y==a.y) && (z<a.z)));
    }
}st;

//the struct of mesh
typedef struct TriangleMesh{
    vector<TriangleDot> dots;
    vector<TriangleFace> faces;
}TriangleMesh;


//load data from obj file
void loadObj(const string path,TriangleMesh & mesh){
    ifstream file(path.c_str());
    if(!file.is_open()){
	cout<<"Error opening file!\n"<<endl;
        return;
    }
    //double x,y,z;
    char buf[512];
    while(!file.getline(buf,512).eof()){
	if(buf[0] == 'v' && buf[1] == ' '){
            TriangleDot dot;
	    if(sscanf(buf,"v %lf %lf %lf",&dot.x,&dot.y,&dot.z)==3){
		mesh.dots.push_back(dot);
	    }
	}
	else if(buf[0] == 'f' && buf[1] == ' '){
	    TriangleFace face;
     	    if(sscanf(buf,"f %d %d %d",&face.dot[0],&face.dot[1],&face.dot[2])==3){
		mesh.faces.push_back(face);
	    }
	}
    } 
}

//ax + by + cz + d = 0;
void getPanel(TriangleDot d1,TriangleDot d2,TriangleDot d3,double &a,double &b,double &c,double &d){
    a = ((d2.y - d1.y)*(d3.z - d1.z) - (d2.z - d1.z)*(d3.y - d1.y));
    b = ((d2.z - d1.z)*(d3.x - d1.x) - (d2.x - d1.x)*(d3.z - d1.z));
    c = ((d2.x - d1.x)*(d3.y - d1.y) - (d2.y - d1.y)*(d3.x - d1.x));
    d = ( 0 - (a*d1.x + b*d1.y + c*d1.z)); 

}

// if d3 and dd on the same side of d1 and d2
bool sameSide(TriangleDot d1,TriangleDot d2,TriangleDot d3,TriangleDot dd){
    TriangleDot d1_2,d1_3,d1_d,dp1,dp2;

    d1_2.x = d2.x - d1.x;
    d1_2.y = d2.y - d1.y;
    d1_2.z = d2.z - d1.z;

    d1_3.x = d3.x - d1.x;
    d1_3.y = d3.y - d1.y;
    d1_3.z = d3.z - d1.z;

    d1_d.x = dd.x - d1.x;
    d1_d.x = dd.x - d1.x;
    d1_d.y = dd.y - d1.y;
    
    dp1.x = d1_2.y * d1_3.z - d1_2.z * d1_3.y;
    dp1.y = d1_2.z * d1_3.x - d1_2.x * d1_3.z;
    dp1.z = d1_2.x * d1_3.y - d1_2.y * d1_3.x;

    dp2.x = d1_2.y * d1_d.z - d1_2.z * d1_d.y;
    dp2.y = d1_2.z * d1_d.x - d1_2.x * d1_d.z;
    dp2.z = d1_2.x * d1_d.y - d1_2.y * d1_d.x;

    return ((dp1.x*dp2.x + dp1.y*dp2.y + dp1.z*dp2.z)>=0);
    
}

//if dd in triangle
bool pointInTriangle(TriangleDot d1,TriangleDot d2,TriangleDot d3,TriangleDot dd){
    return (sameSide(d1,d2,d3,dd) && sameSide(d2,d3,d1,dd) && sameSide(d3,d1,d2,dd));
}

//get voxels of obj file
void getVoxels(double size,TriangleMesh &mesh,map<st,int> &voxel){
    double xmin,xmax,ymin,ymax,zmin,zmax;
    xmin = xmax = mesh.dots[0].x;
    ymin = ymax = mesh.dots[0].y;
    zmin = zmax = mesh.dots[0].z;
    for(unsigned int i = 1; i < mesh.dots.size(); i++){
	
	//x min and max
	if(mesh.dots[i].x < xmin)
	    xmin = mesh.dots[i].x;
	else if(mesh.dots[i].x > xmax)
	    xmax = mesh.dots[i].x;

	//y min and max
	if(mesh.dots[i].y < ymin)
	    ymin = mesh.dots[i].y;
	else if(mesh.dots[i].y > ymax)
	    ymax = mesh.dots[i].y;
	
	//z min and max
	if(mesh.dots[i].z < zmin)
	    zmin = mesh.dots[i].z;
	else if(mesh.dots[i].z > zmax)
	    zmax = mesh.dots[i].z;
    }
    double temp;
    temp = (xmax - xmin)/size;
    temp = (temp-(int)temp)*size;
    if(temp > 0){
	xmin -= (size - temp)/2;
	xmax += (size - temp)/2;
    }

    temp = (ymax - ymin)/size;
    temp = (temp-(int)temp)*size;
    if(temp > 0){
	ymin -= (size - temp)/2;
	ymax += (size - temp)/2;
    }

    temp = (zmax - zmin)/size;
    temp = (temp-(int)temp)*size;
    if(temp > 0){
	zmin -= (size - temp)/2;
	zmax += (size - temp)/2;
    }

    for(unsigned int i = 0; i < mesh.faces.size(); i++){
	TriangleDot d[3];
	for(unsigned int j = 0; j < 3; j++){
	    d[j] = mesh.dots[mesh.faces[i].dot[j]-1];
	}

	double a,b,c,e;
        getPanel(d[0],d[1],d[2],a,b,c,e);
	
	double x_min,x_max,y_min,y_max,z_min,z_max;
	
	x_min = x_max = d[0].x;
	y_min = y_max = d[0].y;
	z_min = z_max = d[0].z;

	for(unsigned int j = 1; j < 3; j++){
	    if(d[j].x<x_min)
		x_min = d[j].x;
	    else if(d[j].x>x_max)
		x_max = d[j].x;

	    if(d[j].y<y_min)
		y_min = d[j].y;
	    else if(d[j].y>y_max)
		y_max = d[j].y;

	    if(d[j].z<z_min)
		z_min = d[j].z;
	    else if(d[j].z>z_max)
		z_max = d[j].z;
        }
 
//	int x_l = (int)((x_min-xmin)/size);
//	int x_h = (int)((x_max-xmin)/size);
//
//	int y_l = (int)((y_min-ymin)/size);
//	int y_h = (int)((y_max-ymin)/size);
//
//	int z_l = (int)((z_min-zmin)/size);
//	int z_h = (int)((z_max-zmin)/size);

	if(a == 0.0){
	    if(b == 0.0){
		for(double ii=x_min; ii<=x_max;){
		    for(double jj=y_min; jj<=y_max;){
			TriangleDot dd;
			dd.x = ii;
			dd.y = jj;
			dd.z = -e/c;
			if(pointInTriangle(d[0],d[1],d[2],dd))
			    voxel.insert(pair<st,int>(st((int)((ii-xmin)/size),(int)((jj-ymin)/size),(int)((dd.z-zmin)/size)),1));
			if(y_max - jj > 0 && y_max -jj < size)
			    jj = y_max;
			else 
			    jj += size;
		    }
		    if(x_max - ii > 0 && x_max -ii < size)
			ii = x_max;
		    else 
			ii += size;
		}	
	    }
	    else{
		if(c == 0.0){

		    for(double ii=x_min; ii<=x_max; ){
		        for(double jj=z_min; jj<=z_max; ){
			    TriangleDot dd;
			    dd.x = ii;
			    dd.z = jj;
			    dd.y = -e/b;
			    if(pointInTriangle(d[0],d[1],d[2],dd))
			        voxel.insert(pair<st,int>(st((int)((ii-xmin)/size),(int)((dd.y-ymin)/size),(int)((jj-zmin)/size)),1));
			    if(z_max - jj > 0 && z_max -jj < size)
			        jj = z_max;
			    else 
			        jj += size;
			}
			if(x_max - ii > 0 && x_max -ii < size)
			    ii = x_max;
			else 
			    ii += size;
		    }
		}
		else{
		    for(double ii = x_min; ii<=x_max; ){
			for(double jj = y_min; jj<=y_max; ){
			    TriangleDot dd;
			    dd.x = ii;
			    dd.y = jj;
			    dd.z = (-e - b * dd.y)/c;
			    if(pointInTriangle(d[0],d[1],d[2],dd))
				voxel.insert(pair<st,int>(st((int)((ii-xmin)/size),(int)((jj-ymin)/size),(int)((dd.z-zmin)/size)),1));
			    if(y_max - jj > 0 && y_max -jj < size)
			        jj = y_max;
			    else 
			        jj += size;
			}
			if(x_max - ii > 0 && x_max -ii < size)
			    ii = x_max;
			else 
			    ii += size;
		    }		
		}
	
	    }
	}
	else{
	    if(b == 0.0){
		if(c == 0.0){
		    for(double ii=y_min; ii<=y_max; ){
		    	for(double jj=z_min; jj<=z_max; ){
			    TriangleDot dd;
			    dd.y = ii;
			    dd.z = jj;
			    dd.x = -e/a;
			    if(pointInTriangle(d[0],d[1],d[2],dd))
			        voxel.insert(pair<st,int>(st((int)((dd.x-xmin)/size),(int)((ii-ymin)/size),(int)((jj-zmin)/size)),1));

			    if(z_max - jj > 0 && z_max -jj < size)
			        jj = z_max;
			    else 
			        jj += size;
		   	 }
			 if(y_max - ii > 0 && y_max -ii < size)
			     ii = y_max;
			 else 
			     ii += size;
		    }	
		}
		else{
		    for(double ii = x_min; ii<=x_max; ){
			for(double jj = y_min; jj<=y_max; ){
			    TriangleDot dd;
			    dd.x = ii;
			    dd.y = jj;
			    dd.z = (-e - a * dd.x)/c;
			    if(pointInTriangle(d[0],d[1],d[2],dd))
				voxel.insert(pair<st,int>(st((int)((ii-xmin)/size),(int)((jj-ymin)/size),(int)((dd.z-zmin)/size)),1));
		 	    if(y_max - jj > 0 && y_max -jj < size)
			        jj = y_max;
			    else 
			        jj += size;
			}
			if(x_max - ii > 0 && x_max -ii < size)
			    ii = x_max;
			else 
			    ii += size;
		    }		
		}
	    }
	    else{
		if(c == 0.0){
		    for(double ii = x_min; ii<=x_max; ){
			for(double jj = z_min; jj<=z_max; ){
			    TriangleDot dd;
			    dd.x = ii;
			    dd.z = jj;
			    dd.y = (-e - a * dd.x)/b;
			    if(pointInTriangle(d[0],d[1],d[2],dd))
				voxel.insert(pair<st,int>(st((int)((ii-xmin)/size),(int)((dd.y-ymin)/size),(int)((jj-zmin)/size)),1));
		 	    if(z_max - jj > 0 && z_max -jj < size)
			        jj = z_max;
			    else 
			        jj += size;
			}
			if(x_max - ii > 0 && x_max -ii < size)
			    ii = x_max;
			else 
			    ii += size;
		    }	
		}
		else{
		    for(double ii=x_min; ii<=x_max; ){
		    	for(double jj=y_min; jj<=y_max; ){
			    TriangleDot dd;
			    dd.x = ii;
			    dd.y = jj;
			    dd.z = (-e - a*dd.x - b*dd.y)/c;
			    
			    if(pointInTriangle(d[0],d[1],d[2],dd))
			       voxel.insert(pair<st,int>(st((int)((ii-xmin)/size),(int)((jj-ymin)/size),(int)((dd.z-zmin)/size)),1));
			    if(y_max - jj > 0 && y_max -jj < size)
			        jj = y_max;
			    else 
			        jj += size;
		        }
			if(x_max - ii > 0 && x_max -ii < size)
			    ii = x_max;
			else 
			    ii += size;
		    }	
	
		}
	    }
	} 

    }
	
}
#endif // LOADOBJ_H
