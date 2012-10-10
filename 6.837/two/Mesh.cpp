#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.
    std::fstream file;
	file.open(filename, std::ios::in);
        
    while (!file.eof()){
        string s;
        file >> s;
        if (s == "v") {
            float x, y, z;
            file >> x >> y >> z;
            bindVertices.push_back(Vector3f(x,y,z));
        } else if (s == "f"){
            Tuple3u face;
            file >> face[0] >> face[1] >> face[2];
            faces.push_back(face);
        } else {
            // ignore other types of lines
        }
    }
    file.close();
	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;
}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".
    for(unsigned int i=0; i < faces.size(); i++) {
        glBegin(GL_TRIANGLES);
        Vector3f v1, v2, v3, normal;
        v1 = currentVertices[faces[i][0]-1];
        v2 = currentVertices[faces[i][1]-1];
        v3 = currentVertices[faces[i][2]-1];
        normal = (Vector3f::cross(v2-v1, v3-v1)).normalized();
        glNormal3d(normal[0], normal[1], normal[2]);
        glVertex3d(v1[0], v1[1], v1[2]);
        glNormal3d(normal[0], normal[1], normal[2]);
        glVertex3d(v2[0], v2[1], v2[2]);
        glNormal3d(normal[0], normal[1], normal[2]);
        glVertex3d(v3[0], v3[1], v3[2]);
        glEnd();
    }
}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments
    
    std::fstream file;
	file.open(filename, std::ios::in);
    float weight;
    while (!file.eof()){
        std::vector<float> weights;
        weights.push_back(0); // for the root;
        for (unsigned i = 0; i < 17; i++){
            file >> weight;
            weights.push_back(weight);
        }
        attachments.push_back(weights);
    }
    file.close();
}
