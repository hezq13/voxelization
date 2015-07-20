#include"loadobj.h"
#include<sstream>
using namespace std;

int main(int argc,char *argv[]){
    
    double size = 0.01;
    if(argc == 2){
	stringstream ss(argv[1]);
	ss >> size;
    }
    string path="/home/hzq/surf_sphere.obj";
    TriangleMesh mesh;
    map<st,int> voxel;
    loadObj(path,mesh);
    getVoxels(size,mesh,voxel);
    map<st,int>::iterator it;
    for(it = voxel.begin(); it!=voxel.end();it++){
	cout<<it->first.x<<" "<<it->first.y<<" "<<it->first.z<<endl;
	
    }
    return 0;
}

