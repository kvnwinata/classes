#ifdef __APPLE__
# include <GLUT/glut.h>
/* Just in case we need these later
// References: 
// http://alumni.cs.ucsb.edu/~wombatty/tutorials/opengl_mac_osx.html
// # include <OpenGL/gl.h>
// # include <OpenGL/glu.h>
*/
#else
# include <GL/glut.h>
#endif

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include <vecmath.h>
using namespace std;

// Globals

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;

// You will need more global variables to implement color and position changes
int colorIndex = 0;
float light_x = 1.0f;
float light_y = 1.0f;
bool isRotating = false;
float angle = 0.;
bool vboLoaded = false;
GLuint vboId;
//GLfloat* vertices; // create vertex array
//GLfloat* normals; // create vertex array
float color_index = 0;
// These are convenience functions which allow us to call OpenGL
// methods on Vec3d objects
inline void glVertex(const Vector3f &a) 
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }

// glutTimerCallback increments the angle when it's rotating:
void timerCallback(int value){
    if (isRotating){
        angle++;
        glutPostRedisplay();
        glutTimerFunc(20, timerCallback , 0);
    }
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
    case 'c':
        // add code to change color here
        /* //discrete color transition
        colorIndex ++;
        colorIndex = colorIndex % 4;
        */
        // smooth color transition:
        color_index ++;
        if (color_index == 101){
            color_index = 0;
        }
        break;
    case 'r':
        isRotating = !isRotating;
        if (isRotating){
            glutTimerFunc(10, timerCallback , 0);
        }
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}


// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
        light_y += 0.5;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
        light_y -= 0.5;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
        light_x -= 0.5;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
        light_x += 0.5;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    // Set material properties of object

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };
    
	// Here we use the first color entry as the diffuse color
    // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffColors[colorIndex]);
    float red = 1.0 - color_index/100.0;
    float green;
    if (color_index <= 50){
        green = color_index/50.0;
    } else {
        green = 1.0 - (color_index-50)/50.0;
    }
    float blue = color_index/100.0;
    GLfloat color[4] = {red, green, blue, 1.0};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    
	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[] = {light_x, light_y, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);
    
    glRotatef(angle, 0.0f, 1.0f, 0.0f); // Rotate our object around the y axis
    
    for(unsigned int i=0; i < vecf.size(); i++) {
        glBegin(GL_TRIANGLES);
        glNormal3d(vecn[vecf[i][2]-1][0], vecn[vecf[i][2]-1][1], vecn[vecf[i][2]-1][2]);
        glVertex3d(vecv[vecf[i][0]-1][0], vecv[vecf[i][0]-1][1], vecv[vecf[i][0]-1][2]);
        glNormal3d(vecn[vecf[i][5]-1][0], vecn[vecf[i][5]-1][1], vecn[vecf[i][5]-1][2]);
        glVertex3d(vecv[vecf[i][3]-1][0], vecv[vecf[i][3]-1][1], vecv[vecf[i][3]-1][2]);
        glNormal3d(vecn[vecf[i][8]-1][0], vecn[vecf[i][8]-1][1], vecn[vecf[i][8]-1][2]);
        glVertex3d(vecv[vecf[i][6]-1][0], vecv[vecf[i][6]-1][1], vecv[vecf[i][6]-1][2]);
        glEnd();
    }
    
    /*/ use vbo
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId);
    
    // enable vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    
    // before draw, specify vertex and index arrays with their offsets
    glNormalPointer(GL_FLOAT, 0, (void*)sizeof(vertices));
    glVertexPointer(3, GL_FLOAT, 0, 0);
    
    glDrawArrays(GL_TRIANGLES, 0, vecf.size()*3);
    
    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_NORMAL_ARRAY);
    
    // it is good idea to release VBOs with ID 0 after use.
    // Once bound with 0, all pointers in gl*Pointer() behave as real
    // pointer, so, normal vertex array operations are re-activated
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
     */
    
    // Dump the image to the screen.
    glutSwapBuffers();


}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	// load the OBJ file here
    int MAX_BUFFER_SIZE = 100;
    char buffer[MAX_BUFFER_SIZE];
    //if ();
    while (cin.getline(buffer, MAX_BUFFER_SIZE) != 0){
        stringstream ss(buffer);
        string s;
        ss >> s;
        if (s == "v") {
            Vector3f v;
            ss >> v[0] >> v[1] >> v[2];
            vecv.push_back(v);
        } else if (s == "vn"){
            Vector3f vn;
            ss >> vn[0] >> vn[1] >> vn[2];
            vecn.push_back(vn);
        } else if (s == "f"){
            unsigned a,b,c,d,e,f,g,h,i;
            char x;
            ss >> a >> x >> b >> x >> c >>
                  d >> x >> e >> x >> f >>
                  g >> x >> h >> x >> i;
            vector<unsigned> face;
            face.push_back(a);
            face.push_back(b);
            face.push_back(c);
            face.push_back(d);
            face.push_back(e);
            face.push_back(f);
            face.push_back(g);
            face.push_back(h);
            face.push_back(i);
            vecf.push_back(face);
        } else {
            // ignore other type of lines
        }
    }
    
    /*
    vertices = new GLfloat[vecf.size()*9]; // create vertex array
    normals  = new GLfloat[vecf.size()*9]; // create vertex array
    
    cout << vecf.size();
    for(int i = 0; i < vecf.size(); i++) {
        std::cout << "i" << i;
        normals[9*i  ] = vecn[vecf[i][2]-1][0];
        normals[9*i+1] = vecn[vecf[i][2]-1][1];
        normals[9*i+2] = vecn[vecf[i][2]-1][2];
        cout << "1";
        vertices[9*i  ] = vecv[vecf[i][0]-1][0];
        vertices[9*i+1] = vecv[vecf[i][0]-1][1];
        vertices[9*i+2] = vecv[vecf[i][0]-1][2];
        cout << "2";

        normals[9*i+3] = vecn[vecf[i][5]-1][0];
        normals[9*i+4] = vecn[vecf[i][5]-1][1];
        normals[9*i+5] = vecn[vecf[i][5]-1][2];
        cout << "3";

        vertices[9*i+3] = vecv[vecf[i][3]-1][0];
        vertices[9*i+4] = vecv[vecf[i][3]-1][1];
        vertices[9*i+5] = vecv[vecf[i][3]-1][2];
        cout << "4";

        normals[9*i+6] = vecn[vecf[i][8]-1][0];
        normals[9*i+7] = vecn[vecf[i][8]-1][1];
        normals[9*i+8] = vecn[vecf[i][8]-1][2];
        cout << "5";

        vertices[9*i+6] = vecv[vecf[i][6]-1][0];
        vertices[9*i+7] = vecv[vecf[i][6]-1][1];
        vertices[9*i+8] = vecv[vecf[i][6]-1][2];
    }
    
    glGenBuffersARB(1, &vboId);
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, vboId);
    glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices)+sizeof(normals), 0, GL_STATIC_DRAW_ARB);
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(vertices), vertices);                             // copy vertices starting from 0 offest
    glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(vertices), sizeof(normals), normals);                // copy normals after vertices
    
    delete vertices;
    delete normals;
    std::cout << "loading done";
*/
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{
    loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
