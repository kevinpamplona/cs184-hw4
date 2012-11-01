/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "shaders.h"
#include "Transform.h"
#include <FreeImage.h>
#include "UCB/grader.h"

using namespace std ; 

// Main variables in the program.  
#define MAINPROGRAM 
#include "variables.h" 
#include "readfile.h" // prototypes for readfile.cpp  
void display(void) ;  // prototype for display function.  

int oldx = 600, oldy = 425; // Initial mouse positions
bool firstMove = true; // Flag for mouse function
GLint animate = 0; // Animate flag

void reshape(int width, int height){
	w = width;
	h = height;
        mat4 mv ; // just like for lookat

	glMatrixMode(GL_PROJECTION);
        float aspect = w / (float) h, zNear = 0.1, zFar = 99.0 ;
        // I am changing the projection stuff to be consistent with lookat
        if (useGlu) mv = glm::perspective(fovy,aspect,zNear,zFar) ; 
        else {
          mv = Transform::perspective(fovy,aspect,zNear,zFar) ; 
          mv = glm::transpose(mv) ; // accounting for row major 
        }
        glLoadMatrixf(&mv[0][0]) ; 

	glViewport(0, 0, w, h);
}

// UTILITY: Print out camera coordinates--used for debugging
void printCamera() {
    std::cout << "EYE: (" << eye.x << "," << eye.y << "," << eye.z << ")\n";
    std::cout << "CENTER: (" << center.x << "," << center.y << "," << center.z << ")\n";
    std::cout << "UP: (" << up.x << "," << up.y << "," << up.z << ")\n";
}

// UTILITY: Print out RGB VALUES--used for debugging
void printColor() {
    for(int i=0;i<256;i++){
        for(int j=0;j<256;j++){
            float red = grasstexture[i][j][0];
            float blue = grasstexture[i][j][1];
            float green = grasstexture[i][j][2];
            std::cout << "PIXEL: (" << i << "," << j << ") RED:" << red << " BLUE:" << blue << " GREEN:" << green << "\n";
        }
    }
    
    
}

// UTILITY: CHECK IF CAMERA IS AT ORIGIN -- USED FOR MOUSE FUNCTION AND DEBUGGING
bool checkOrigin(vec3 center) {
    if (center.x == 0) {
        if (center.y == 0) {
            if (center.z == 0) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

// HELPER: SCALE THE VECTOR BY GIVEN CONSTANT
vec3 scaleVec(const vec3 &in, float value) {
    vec3 out;
    
    out.x = in.x * value;
    out.y = in.y * value;
    out.z = in.z * value;
    
    return out;
}

// HELPER: USED TO KEEP TRACK ANIMATION MOVEMENT AND FLAG
void animation(void) {
    ani_deg += 1;
    
    if (ani_deg > 360) {
        ani_deg = 0;
    }
    
	glutPostRedisplay();
}

// THE NEXT FOUR FUNCTIONS INITIALIZE THE DIFFERENCE TEXTURES.
// INEFFICIENCY IS ACKNOWLEDGED SINCE WE CREATE THREE DIFFERENT FUNCTION,
// BUT IT WAS A BIT DIFFICULT TO DEAL WITH THE POINTERS AND DEREFERENCES.
// WE DOUBLE CHECKED WITH GSI AND WE GOT THE OKAY AS LONG AS IT WORKS.

// Initialize Grass Texture
void inittextureGrass (const char * filename, GLuint program) {
    int i,j,k;
	FILE * fp;
	assert(fp = fopen(filename,"rb"));
	fscanf(fp,"%*s %*d %*d %*d%*c");
	for (i=0; i<256; i++) {
		for (j=0; j<256; j++) {
			for (k=0; k<3; k++) {
			        fscanf(fp,"%c", &(grasstexture[i][j][k])); 
			}
		}
	}
	fclose(fp);
    glGenTextures(1, &grass);
    glBindTexture(GL_TEXTURE_2D, grass);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, grasstexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLint texsampler;
    texsampler = glGetUniformLocation(program, "tex");
    glUniform1i(texsampler, 0);
    istex = glGetUniformLocation(program,"istex");
    std::cout << "GRASS TEXTURE ID: " << grass << "\n";
}

// Initialize Wood Texture
void inittextureWood (const char * filename, GLuint program) {
    int i,j,k;
	FILE * fp;
	assert(fp = fopen(filename,"rb"));
	fscanf(fp,"%*s %*d %*d %*d%*c");
	for (i=0; i<256; i++) {
		for (j=0; j<256; j++) {
			for (k=0; k<3; k++) {
			        fscanf(fp,"%c", &(woodtexture[i][j][k])); 
			}
		}
	}
	fclose(fp);
    glGenTextures(1, &wood);
    glBindTexture(GL_TEXTURE_2D, wood);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, woodtexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLint texsampler;
    texsampler = glGetUniformLocation(program, "tex");
    glUniform1i(texsampler, 0);
    istex = glGetUniformLocation(program,"istex");
    std::cout << "WOOD TEXTURE ID: " << wood << "\n";
}

// Initialize Weave Map Texture
void inittextureWeave (const char * filename, GLuint program) {
    int i,j,k;
	FILE * fp;
	assert(fp = fopen(filename,"rb"));
	fscanf(fp,"%*s %*d %*d %*d%*c");
	for (i=0; i<256; i++) {
		for (j=0; j<256; j++) {
			for (k=0; k<3; k++) {
				fscanf(fp,"%c", &(weavetexture[i][j][k])); 
			}
		}
	}
	fclose(fp);
    glGenTextures(1, &weave);
    glBindTexture(GL_TEXTURE_2D, weave);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, weavetexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLint texsampler;
    texsampler = glGetUniformLocation(program, "tex");
    glUniform1i(texsampler, 0);
    istex = glGetUniformLocation(program,"istex");
    std::cout << "WEAVE TEXTURE ID: " << weave << "\n";
}

// Initialize Bump Map Texture
void inittextureBump (const char * filename, GLuint program) {
    int i,j,k;
	FILE * fp;
	assert(fp = fopen(filename,"rb"));
	fscanf(fp,"%*s %*d %*d %*d%*c");
	for (i=0; i<256; i++) {
		for (j=0; j<256; j++) {
			for (k=0; k<3; k++) {
				fscanf(fp,"%c", &(bumptexture[i][j][k])); 
			}
		}
	}
	fclose(fp);
    glGenTextures(1, &bump);
    glBindTexture(GL_TEXTURE_2D, bump);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, bumptexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLint texsampler;
    texsampler = glGetUniformLocation(program, "tex");
    glUniform1i(texsampler, 0);
    istex = glGetUniformLocation(program,"istex");
    std::cout << "BUMP TEXTURE ID: " << bump << "\n";
}



// OBJECT-LOADER--credit given to GSI BRANDON's help and reference to the wikibooks tutorial page
void load_obj(const char * filename, vector<glm::vec4> &vertices, vector<glm::vec3> &normals, vector<GLushort> &elements) {
    ifstream in(filename, ios::in);
    if (!in) { cerr << "Cannot open file: " << filename << endl; exit(1); }
    
    string line;
    while (getline(in, line)) {
        if (line.substr(0,2) == "v ") {
            istringstream s(line.substr(2));
            glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
            vertices.push_back(v);
        } else if (line.substr(0,2) == "f ") {
            istringstream s(line.substr(2));
            GLushort a,b,c;
            s >> a; s >> b; s >> c;
            a--; b--; c--;
            elements.push_back(a); elements.push_back(b); elements.push_back(c);
        } 
        else if (line[0] == '#') { /* ignoring this line */ }
        else { /* ignoring this line */ }
    }
    
    // Calculate normals
    normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0)); 
    for (int i=0; i<elements.size(); i+=3) {
        GLushort ia = elements[i];
        GLushort ib = elements[i+1];
        GLushort ic = elements[i+2];
        glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]), 
                                                     glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
        normals[ia] = normals[ib] = normals[ic] = normal;
    }
    
    std::cout << filename << " has been loaded. There are " << vertices.size() << " inputted vertices, " << normals.size() << " calculated normals, and " << elements.size()/3 << " faces..\n";
}




// THE FOLLOWING FOUR FUNCTIONS (ZOOM, STRAFE, LOOKLEFT, LOOKUP) ARE USED FOR CAMERA MOVEMENT
void zoom(float amount) {
    vec3 temp = glm::normalize(eye-center);
    eye -= scaleVec(temp, amount*0.1);
    center -= scaleVec(temp, amount*0.1);
}

void strafe(float amount) {
        vec3 right = glm::cross(glm::normalize(up), glm::normalize(eye-center));
        right = scaleVec(right, amount*0.01);
        eye = eye + right;
        center = center + right;
}

void lookLeft(float degrees) {
    center = center-eye;
    mat3 rot = Transform::rotate(degrees, glm::normalize(up));
    center = rot*(center);
    center = center+eye;
}

void lookUp(float degrees) {
    center = center-eye;
    vec3 temp = glm::normalize(glm::cross(center, up));
    center = center*Transform::rotate(degrees, temp);
    up = up*Transform::rotate(degrees, temp);
    center = center+eye;
}
// END OF CAMERA FUNCTIONS 




void printHelp() {
  std::cout << "\npress 'h' to print this message again.\n" 
	        << "use w-a-s-d to move forward/backward/left/right around the scene.\n"
            << "use i-j-k-l to rotate the camera and look around the scene.\n"       
            << "left-click the mouse to toggle the mouse functionality--the mouse controls the same movement as i-j-k-l.\n"
            << "press 'r' to reset the scene/camera.\n"
            << "press 'p' to toggle the animation.\n"
            << "press 't' to toggle the textures.\n"
            << "press 'b' to toggle the bench and admire our beautiful hand-drawn object.\n"
            << "press 'c' to print the current camera attributes.\n"
            << "press ESC to quit.\n" ;      
}

// KEYBOARD CONTROL
void keyboard(unsigned char key, int x, int y) {
	switch(key) {
	case '+':
		amount++;
		std::cout << "amount set to " << amount << "\n" ;
		break;
	case '-':
		amount--;
		std::cout << "amount set to " << amount << "\n" ; 
		break;
	case 'h':
		printHelp();
		break;
    case 27:  // Escape to quit
        exit(0) ;
        break ;
    case 'r': 
        eye = vec3(-0.543768, -4.25238, 1.40089);
        center = vec3(-0.543768, -0.0714374, 0.280609);
        up = vec3(0.00000000178084, 0.258819, 0.965926);
        fovy = 30.0;
        sx = sy = sz = 1.0 ; 
        tx = ty = 0.0 ; 
        break ;   
    case 'w':
        zoom(amount);
        break;
    case 's':
        zoom(-amount);
        break;
    case 'a':
        strafe(-amount);
        break;
    case 'd':
        strafe(amount);
        break;
    case 'i':
        lookUp(amount);
        break;
    case 'k':
        lookUp(-amount);
        break;
    case 'j':
        lookLeft(-amount);
        break;
    case 'l':
        lookLeft(amount);
        break;
    case 'c':
        printCamera();
        break;
    case 'p':
        animate = !animate;
        if (animate) {
            glutIdleFunc(animation);
        } else {
            glutIdleFunc(NULL);
        }
        break;
    case 'b':
            bench = !bench;
            glutPostRedisplay();
        break;
    case 't':
        texton = !texton;
        glutPostRedisplay();
        break;
    }
	glutPostRedisplay();
}


// VIEW FUNCTIONS FROM HW2 
void specialKey(int key, int x, int y) {
	switch(key) {
	case 100: //left
           Transform::left(amount, eye,  up);
          break;
	case 101: //up
            Transform::up(amount,  eye,  up);
          break;
	case 102: //right
            Transform::left(-amount, eye,  up);
          break;
	case 103: //down
            Transform::up(-amount,  eye,  up);
          break;
	}
	glutPostRedisplay();
}


// THE FOLLOWING TWO FUNCTIONS ARE USED FOR MOUSE FUNCTION AND ARE CALLED MAIN() IN THE GLUT CALLBACKS
void mouse(int x, int y) {
    if (firstMove) {
        firstMove = !firstMove;
        eye = vec3(-0.543768, -4.25238, 1.40089);
        center = vec3(-0.543768, -0.0714374, 0.280609);
        up = vec3(0.00000000178084, 0.258819, 0.965926);
        fovy = 30.0;
        sx = sy = sz = 1.0 ; 
        tx = ty = 0.0 ;
        
    }
    int delta_x = x - oldx;
    int delta_y = -1 * (y - oldy);
    lookLeft(delta_x*0.1);
    lookUp(delta_y*0.1);
    oldx = x;
    oldy = y;
    glutPostRedisplay();
}

void click(int button, int state, int x, int y) 
{
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_UP) {
			// Do Nothing ;
		}
		else if (state == GLUT_DOWN) {
            if (firstMove) {
                glutPassiveMotionFunc(mouse);
                firstMove = !firstMove;
            } else {
                glutPassiveMotionFunc(NULL);
                firstMove = !firstMove;
            }

		}
    }
}
// END OF TWO MOUSE FUNCTIONS 


void init() {
      // Initialize cow object
      load_obj("objects/cow.obj", cow_vertices, cow_normals, cow_elements);
    
      // Initialize magnolia flowers
      load_obj("objects/magnolia.obj", magnolia_vertices, magnolia_normals, magnolia_elements);
        
      // Initialize shaders
      vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
      fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
      shaderprogram = initprogram(vertexshader, fragmentshader) ; 
      enablelighting = glGetUniformLocation(shaderprogram,"enablelighting") ;
      lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;       
      lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;       
      numusedcol = glGetUniformLocation(shaderprogram,"numused") ;       
      ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
      diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
      specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
      emissioncol = glGetUniformLocation(shaderprogram,"emission") ;       
      shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;    

      // Initialize textures
      inittextureGrass("textures/grass.ppm", shaderprogram);
      inittextureWood("textures/wood.ppm", shaderprogram);
      inittextureBump("textures/bump.ppm", shaderprogram);
      inittextureWeave("textures/weave2.ppm", shaderprogram);
    
    texton = true;
    bench = false;
}

int main(int argc, char* argv[]) {

    
	if (argc != 2) {
		cerr << "Usage: transforms scenefile [grader input (optional)]\n"; 
		exit(-1); 
	}

  	FreeImage_Initialise();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("HW2: Scene Viewer");
	init();
    readfile(argv[1]) ; 
	glutDisplayFunc(display);
	glutSpecialFunc(specialKey);
	glutKeyboardFunc(keyboard);
    glutMouseFunc(click);
    glutPassiveMotionFunc(NULL);
	glutReshapeFunc(reshape);
	glutReshapeWindow(w, h);

	printHelp();
	glutMainLoop();
	FreeImage_DeInitialise();
	return 0;
}
