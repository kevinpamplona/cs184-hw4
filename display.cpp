/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"

using namespace std ; 
#include "variables.h"
#include "readfile.h"

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.  
// Some of you may want to use the more modern routines in readfile.cpp 
// that can also be used.  

void transformvec (const GLfloat input[4], GLfloat output[4]) {
  GLfloat modelview[16] ; // in column major order
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ; 
  
  for (int i = 0 ; i < 4 ; i++) {
    output[i] = 0 ; 
    for (int j = 0 ; j < 4 ; j++) 
      output[i] += modelview[4*j+i] * input[j] ; 
  }
}


void drawTriangle(vec4 a, vec4 b, vec4 c, vec3 norm_a, vec3 norm_b, vec3 norm_c, vec3 acol, vec3 bcol, vec3 ccol) {
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glColor3f(acol.x, acol.y, acol.z); glNormal3f(norm_a.x, norm_a.y, norm_c.z); glVertex3f(a.x, a.y, a.z); 
    glColor3f(bcol.x, bcol.y, bcol.z); glNormal3f(norm_b.x, norm_b.y, norm_c.z); glVertex3f(b.x, b.y, b.z); 
    glColor3f(ccol.x, ccol.y, ccol.z); glNormal3f(norm_c.x, norm_c.y, norm_c.z); glVertex3f(c.x, c.y, c.z); 
    glEnd();
    glPopMatrix();
}

void draw_obj(vector<glm::vec4> vertices, vector<GLushort> faces, vector<glm::vec3> normals, vec3 acol, vec3 bcol, vec3 ccol) {
    for (int i=0; i<faces.size(); i+=3) {
        GLushort a = faces[i];
        GLushort b = faces[i+1];
        GLushort c = faces[i+2];
        
        vec4 vert_a = vertices[a];
        vec4 vert_b = vertices[b];
        vec4 vert_c = vertices[c];
        
        vec3 norm_a = normals[a];
        vec3 norm_b = normals[b];
        vec3 norm_c = normals[c];
        
        drawTriangle(vert_a, vert_b, vert_c, norm_a, norm_b, norm_c, acol, bcol, ccol);
    }
    glFlush();
}


void display() {
	glClearColor(0.427, 0.815, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        
        // I'm including the basic matrix setup for model view to 
        // give some sense of how this works.  

	glMatrixMode(GL_MODELVIEW);
	mat4 mv ; 

        if (useGlu) mv = glm::lookAt(eye,center,up) ; 
	else {
          mv = Transform::lookAt(eye,center,up) ; 
          mv = glm::transpose(mv) ; // accounting for row major
        }
        glLoadMatrixf(&mv[0][0]) ; 

        // Set Light and Material properties for the teapot
        // Lights are transformed by current modelview matrix. 
        // The shader can't do this globally. 
        // So we need to do so manually.  
        if (numused) {
          glUniform1i(enablelighting,true) ;

          // YOUR CODE FOR HW 2 HERE.  
          // You need to pass the lights to the shader. 
          // Remember that lights are transformed by modelview first.
			
			for (int i=0; i<numused; i++) {
				GLfloat temp[4];
				GLfloat out[4];
				
				temp[0] = lightposn[4*i];
				temp[1] = lightposn[4*i+1]; 
				temp[2] = lightposn[4*i+2];
				temp[3] = lightposn[4*i+3];
				
				transformvec(temp,out);
				
				lightransf[4*i] = out[0];
				lightransf[4*i+1] = out[1];
				lightransf[4*i+2] = out[2];
				lightransf[4*i+3] = out[3];
			}
			glUniform4fv(lightpos,numLights,lightransf);
			glUniform4fv(lightcol,numLights,lightcolor);
        }
        else glUniform1i(enablelighting,false) ; 
		glUniform1i(numusedcol, numused);
	
        // Transformations for objects, involving translation and scaling 
        mat4 sc(1.0) , tr(1.0), transf(1.0) ; 
        sc = Transform::scale(sx,sy,sz) ; 
        tr = Transform::translate(tx,ty,0.0) ; 

        // YOUR CODE FOR HW 2 HERE.  
        // You need to use scale, translate and modelview to 
        // set up the net transformation matrix for the objects.  
        // Account for GLM issues etc.
	
		transf = mv*glm::transpose(tr)*glm::transpose(sc);
		glLoadMatrixf(&transf[0][0]) ;     
    
        for (int i = 0 ; i < numobjects ; i++) {
          object * obj = &(objects[i]) ; 

          {
          // YOUR CODE FOR HW 2 HERE. 
          // Set up the object transformations 
          // And pass in the appropriate material properties

			  //do i need to transpose transform?
			  transf = mv*glm::transpose(tr)*glm::transpose(sc)*glm::transpose(obj->transform);
			  glLoadMatrixf(&transf[0][0]);
			  
			  glUniform4fv(ambientcol,1,obj->ambient);
			  glUniform4fv(diffusecol,1,obj->diffuse);
			  glUniform4fv(specularcol,1,obj->specular);
			  glUniform4fv(emissioncol,1,obj->emission);
			  glUniform1f(shininesscol,obj->shininess);
          }
		  //glMatrixMode(GL_MODELVIEW);
			
          // Actually draw the object
          // We provide the actual glut drawing functions for you.  
          if (obj -> type == cube) {
            glutSolidCube(obj->size) ; 
          }
          else if (obj -> type == sphere) {
            const int tessel = 20 ; 
            glutSolidSphere(obj->size, tessel, tessel) ; 
          }
          else if (obj -> type == teapot) {
            glutSolidTeapot(obj->size) ; 
          }

        }
    
    // CAMPING TENT
    sc = Transform::scale(0.45, 0.45, 0.45);
    tr = Transform::translate(.5,2.4,0.01);
    mat3 _rt = mat3(1.0);
    mat4 rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    glUniform1i(enablelighting,true);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wood);
    glUniform1i(istex,1);
    if (texton) glUniform1i(enablelighting,false);
    glBegin(GL_QUADS);                            
    glTexCoord2d(1,1); glColor3f(0.0f,0.0f,1.0f); glVertex3f(0.5f,0.8333f, 0.0f);
    glTexCoord2d(1,0); glColor3f(0.0f,1.0f,0.0f); glVertex3f(0.5f, 0.8333f, 1.333f);
    glTexCoord2d(0,0); glColor3f(1.0f,0.0f,0.0f); glVertex3f(0.0f, 0.0f, 1.333f);
    glTexCoord2d(0,1); glColor3f(0.0f,1.0f,1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glUniform1i(enablelighting,true);
    glEnd();
    
    glPushMatrix();
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wood);
    glBegin(GL_QUADS);                           
    glColor3f(1.0f, 0.0f, 0.0f);
    glTexCoord2d(1,1); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2d(1,0); glVertex3f(1.0f, 0.0f, 1.333f);
    glTexCoord2d(0,0); glVertex3f(0.5f, 0.8333f, 1.333f);
    glTexCoord2d(0,1); glVertex3f(0.5f,0.8333f, 0.0f);
    glDisable(GL_TEXTURE_2D);
    glEnd();
    glFlush();
    glPopMatrix();
    
    glPushMatrix();
    glBegin(GL_QUADS);                           
    glColor3f(1.0f,1.0f,1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f(0.0f, 0.0f, 1.333f);
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f(1.0f, 0.0f, 1.333f);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex3f(1.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wood);
    glBegin(GL_TRIANGLES);
    glTexCoord2d(0,0); glColor3f(1.0f,1.0f,1.0f); glVertex3f(1.0f, 0.0f, 0.0f);
    glTexCoord2d(0,1); glColor3f(0.1f,0.6f,0.0f); glVertex3f(0.5f,0.8333f, 0.0f);
    glTexCoord2d(1,1); glColor3f(1.0f,1.0f,0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
    glEnd();
    glPopMatrix();
    
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glActiveTexture(GL_TEXTURE2);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wood);
    glBegin(GL_TRIANGLES);
    glTexCoord2d(0,0);
    glColor3f(1.0f,1.0f,1.0f);
    glVertex3f(1.0f, 0.0f, 1.333f);
    glTexCoord2d(0,1);
    glColor3f(0.3f,0.8f,0.0f);
    glVertex3f(0.0f, 0.0f, 1.333f);
    glTexCoord2d(1,1);
    glColor3f(1.0f,1.0f,0.5f);
    glVertex3f(0.5f, 0.8333f, 1.333f);
    glEnd();
    glPopMatrix();
    glUniform1i(enablelighting,false);
    glUniform1i(istex,-1);
    //CAMPING TENT
    
    // COW OBJECT
    glUniform1i(enablelighting,true);
    //tr = Transform::translate(-1.5,5.5,0.2);
    tr = Transform::translate(-1.65,2.5,0.18);
    sc = Transform::scale(0.05,0.05,0.05); 
    _rt = mat3(1.0);
    mat3 _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    mat4 ani = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    glUniform1i(makedull,1); 
    draw_obj(cow_vertices, cow_elements, cow_normals, vec3(1.0,0.0,0.0), vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0));
    glUniform1i(makedull,0); 
    glUniform1i(enablelighting,false);
    // END OF COW OBJECT
    
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,0.1,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,0.1,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    mat3 _rt2 = mat3(1.0);
    mat3 _ani2 = mat3(1.0);
    mat4 rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    mat4 ani2 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,0.1,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    mat3 _rt3 = mat3(1.0);
    mat3 _ani3 = mat3(1.0);
    mat4 rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    mat4 ani3 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS
    
    // Floor object 
    if (texton) glUniform1i(istex,1);
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-2,0,0);
    sc = Transform::scale(3,3,3);
    transf = mv*glm::transpose(tr)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, grass);
    glBegin(GL_QUADS);
    glNormal3f(0.0, 0.0, 1.0);
    glTexCoord2d(1, 1); glColor3f(0.0f,1.0f,0.0f); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2d(1, 0); glColor3f(0.0f,1.0f,0.0f); glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2d(0, 0); glColor3f(0.0f,1.0f,0.0f); glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2d(0, 1); glColor3f(0.0f,1.0f,0.0f); glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glUniform1i(enablelighting,true);
    glUniform1i(istex,0);
    // END OF FLOOR OBJECT
    
    // Start Bench
    sc = Transform::scale(0.08, 0.08, 0.08);
    tr = Transform::translate(-0.65,2,0.25);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    rt2 = Transform::homogen(Transform::rotate(90,vec3(0.0,0.0,1.0)));    
        transf = mv*glm::transpose(tr)*glm::transpose(rt2)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    
    glUniform1i(enablelighting,bench);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, wood);
    glUniform1i(istex,1);
    
    double topWid = 1, topHeight = 5, topThick = 0.5, seatWid = 1, seatHeight = 5, seatThick = 0.5;
    double seatSlatWid = 4, seatSlatHeight = 0.25, seatSlatThick = 0.75;

    //drawing the three table slats
    glPushMatrix(); // middle slat
    glColor3f(0.5f, 0.5f, 1.0f);
    glTranslated(0, 0, 0);
    glScaled(topWid, topThick, topHeight);
    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);
    glBindTexture(GL_TEXTURE_2D, wood);
    glutSolidCube(1.0);    
    glDisable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glDisable(GL_TEXTURE_GEN_T);        
    glPopMatrix();
    
    glPushMatrix(); // left slat
    glTranslated(1.1, 0, 0);
    glScaled(topWid, topThick, topHeight);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix(); // right slat
    glTranslated(-1.1, 0, 0);
    glScaled(topWid, topThick, topHeight);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //drawing the seats
    glPushMatrix(); // right seat
    glTranslated(-2.0, -1.5, 0);
    glScaled(seatWid, seatThick, seatHeight);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix(); // left seat
    glTranslated(2.0, -1.5, 0);
    glScaled(seatWid, seatThick, seatHeight);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //slats under seats
    glPushMatrix();
    glColor3f(1.0f, 0.0f ,1.0f);
    glTranslated(0.0, -1.5-seatThick/2-seatSlatThick/2, -1.75+seatSlatHeight/2);
    glScaled(seatSlatWid, seatSlatThick, seatSlatHeight);
    glutSolidCube(1.0);
    glPopMatrix();
    
    glPushMatrix();
    glColor3f(1.0f, 0.0f ,1.0f);
    glTranslated(0.0, -1.5-seatThick/2-seatSlatThick/2, 1.75-seatSlatHeight/2);
    glScaled(seatSlatWid, seatSlatThick, seatSlatHeight);
    glutSolidCube(1.0);
    glPopMatrix();
    
    //drawing the supporting beams
    glPushMatrix();
    glBegin(GL_QUADS);
    
    //FIRST LEFT SLAT
    glColor3f(1.0f,1.0f,0.0f);    // Color Yellow, first left slat outer
    glVertex3f( 0.0f, -0.25f,-2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.5f, -0.25f,-2.0f);    // Top Left Of The Quad (Back)
    glVertex3f( 2.0f,-3.0f,-2.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.5f,-3.0f,-2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(1.0f,1.0f,0.0f);    // Color Yellow, first left slat inner
    glVertex3f( 0.0f,-0.25f,-1.75f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.5f,-0.25f,-1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( 2.0f, -3.0f,-1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.5f, -3.0f,-1.75f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);    // Color Blue, first left slat near seat
    glVertex3f( 0.5f, -0.25f, -2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.5f,-0.25f,-1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( 2.0f, -3.0f,-1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 2.0f, -3.0f, -2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);      // Color Blue, first left slat away from seat
    glVertex3f( 0.0f, -0.25f, -2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.0f,-0.25f,-1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( 1.5f, -3.0f,-1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.5f, -3.0f, -2.0f);    // Bottom Right Of The Quad (Back)
    
    //FIRST RIGHT SLAT
    glColor3f(1.0f,1.0f,0.0f);    // Color Yellow, first right slat outer
    glVertex3f( 0.0f, -0.25f,-2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( -0.5f, -0.25f,-2.0f);    // Top Left Of The Quad (Back)
    glVertex3f( -2.0f,-3.0f,-2.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -1.5f,-3.0f,-2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(1.0f,1.0f,0.0f);    // Color Yellow, first right slat inner
    glVertex3f( 0.0f,-0.25f,-1.75f);    // Top Right Of The Quad (Back)
    glVertex3f( -0.5f,-0.25f,-1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( -2.0f, -3.0f,-1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -1.5f, -3.0f,-1.75f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);    // Color Blue, first right slat near seat
    glVertex3f( -0.5f, -0.25f, -2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( -0.5f,-0.25f,-1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( -2.0f, -3.0f,-1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -2.0f, -3.0f, -2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);      // Color Blue, first right slat away from seat
    glVertex3f( 0.0f, -0.25f, -2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.0f,-0.25f,-1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( -1.5f, -3.0f,-1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -1.5f, -3.0f, -2.0f);    // Bottom Right Of The Quad (Back)
    
    //SECOND LEFT SLAT
    glColor3f(1.0f,0.5f,0.0f);        // Color Orange, second vertical slat outer
    glVertex3f( 0.0f, -0.25f,2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.5f, -0.25f,2.0f);    // Top Left Of The Quad (Back)
    glVertex3f( 2.0f,-3.0f,2.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.5f,-3.0f,2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(1.0f,0.5f,0.0f);    // Color Orange, first left slat inner
    glVertex3f( 0.0f,-0.25f,1.75f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.5f,-0.25f,1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( 2.0f, -3.0f,1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.5f, -3.0f,1.75f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);    // Color Blue, first left slat near seat
    glVertex3f( 0.5f, -0.25f, 2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.5f,-0.25f,1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( 2.0f, -3.0f,1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 2.0f, -3.0f, 2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);      // Color Blue, first left slat away from seat
    glVertex3f( 0.0f, -0.25f, 2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.0f,-0.25f,1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( 1.5f, -3.0f,1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( 1.5f, -3.0f, 2.0f);    // Bottom Right Of The Quad (Back)
    
    //SECOND RIGHT SLAT
    glColor3f(1.0f,0.5f,0.0f);        // Color Orange, second vertical slat outer
    glVertex3f( 0.0f, -0.25f,2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( -0.5f, -0.25f,2.0f);    // Top Left Of The Quad (Back)
    glVertex3f( -2.0f,-3.0f,2.0f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -1.5f,-3.0f,2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(1.0f,0.5f,0.0f);    // Color Orange, first left slat inner
    glVertex3f( 0.0f,-0.25f,1.75f);    // Top Right Of The Quad (Back)
    glVertex3f( -0.5f,-0.25f,1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( -2.0f, -3.0f,1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -1.5f, -3.0f,1.75f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);    // Color Blue, first left slat near seat
    glVertex3f( -0.5f, -0.25f, 2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( -0.5f,-0.25f,1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( -2.0f, -3.0f,1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -2.0f, -3.0f, 2.0f);    // Bottom Right Of The Quad (Back)
    
    glColor3f(0.0f,0.0f,1.0f);      // Color Blue, first left slat away from seat
    glVertex3f( 0.0f, -0.25f, 2.0f);    // Top Right Of The Quad (Back)
    glVertex3f( 0.0f,-0.25f,1.75f);    // Top Left Of The Quad (Back)
    glVertex3f( -1.5f, -3.0f,1.75f);    // Bottom Left Of The Quad (Back)
    glVertex3f( -1.5f, -3.0f, 2.0f);    // Bottom Right Of The Quad (Back)
    
    //close up the legs
    //FIRST LEFT SLAT BOTTOM
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.5f, -3.0f,-1.75f);    // Top Right Of The Quad (Top) 
    glVertex3f(2.0f, -3.0f,-1.75f);    // Top Left Of The Quad (Top) 
    glVertex3f(2.0f,-3.0f,-2.0f);    // Bottom Left Of The Quad (Top) 
    glVertex3f(1.5f,-3.0f,-2.0f);    // Bottom Right Of The Quad (Top) 
    
    //FIRST RIGHT SLAT BOTTOM
    
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.5f, -3.0f,-1.75f);    // Top Right Of The Quad (Top) 
    glVertex3f(-2.0f, -3.0f,-1.75f);    // Top Left Of The Quad (Top) 
    glVertex3f(-2.0f,-3.0f,-2.0f);    // Bottom Left Of The Quad (Top) 
    glVertex3f(-1.5f,-3.0f,-2.0f);    // Bottom Right Of The Quad (Top) 
    
    //SECOND LEFT SLAT BOTTOM
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.5f, -3.0f,1.75f);    // Top Right Of The Quad (Top) 
    glVertex3f(2.0f, -3.0f,1.75f);    // Top Left Of The Quad (Top) 
    glVertex3f(2.0f,-3.0f,2.0f);    // Bottom Left Of The Quad (Top) 
    glVertex3f(1.5f,-3.0f,2.0f);    // Bottom Right Of The Quad (Top) 
    
    //SECOND RIGHT SLAT BOTTOM
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-1.5f, -3.0f,1.75f);    // Top Right Of The Quad (Top) 
    glVertex3f(-2.0f, -3.0f,1.75f);    // Top Left Of The Quad (Top) 
    glVertex3f(-2.0f,-3.0f,2.0f);    // Bottom Left Of The Quad (Top) 
    glVertex3f(-1.5f,-3.0f,2.0f);    // Bottom Right Of The Quad (Top) 
    
    glEnd();
    //glFlush();
    glPopMatrix();
    glUniform1i(enablelighting, true);
    glUniform1i(istex,-1);
    //End Bench
    
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,0.1,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,0.1,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt2 = mat3(1.0);
    _ani2 = mat3(1.0);
    rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani2 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,0.1,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt3 = mat3(1.0);
    _ani3 = mat3(1.0);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani3 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS

    
    // START OF PAIR
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,0.5,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,0.5,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt2 = mat3(1.0);
    _ani2 = mat3(1.0);
    rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani2 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,0.5,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt3 = mat3(1.0);
    _ani3 = mat3(1.0);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani3 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,0.5,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,0.5,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt2 = mat3(1.0);
    _ani2 = mat3(1.0);
    rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani2 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,0.5,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt3 = mat3(1.0);
    _ani3 = mat3(1.0);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani3 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS
    // END OF PAIR

    // START OF PAIR
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,1.0,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,1.0,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt2 = mat3(1.0);
    _ani2 = mat3(1.0);
    rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani2 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,1.0,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt3 = mat3(1.0);
    _ani3 = mat3(1.0);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani3 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,1.0,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,1.0,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt2 = mat3(1.0);
    _ani2 = mat3(1.0);
    rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani2 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,1.0,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt3 = mat3(1.0);
    _ani3 = mat3(1.0);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani3 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS
    // END OF PAIR
    
    // START OF PAIR 
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,1.5,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,1.5,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt2 = mat3(1.0);
    _ani2 = mat3(1.0);
    rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani2 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,1.5,0.08);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt3 = mat3(1.0);
    _ani3 = mat3(1.0);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani3 = Transform::homogen(Transform::rotate(-ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS
    // FLOWERS
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.5,1.5,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt = mat3(1.0);
    _ani = mat3(1.0);
    rt = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani)*glm::transpose(rt)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.929, 0.507, 0.929), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.7,1.5,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt2 = mat3(1.0);
    _ani2 = mat3(1.0);
    rt2 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani2 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani2)*glm::transpose(rt2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.578, 0.0, 0.824), vec3(1.0, 0.078, 0.574), vec3(1.0, 0.078, 0.574));
    glUniform1i(enablelighting,true);
    
    glUniform1i(enablelighting,false);
    tr = Transform::translate(-1.9,1.5,0.05);
    sc = Transform::scale(0.001,0.001,0.001); 
    _rt3 = mat3(1.0);
    _ani3 = mat3(1.0);
    rt3 = Transform::homogen(Transform::rotate(90,vec3(1.0,0.0,0.0)));
    ani3 = Transform::homogen(Transform::rotate(ani_deg,vec3(0.0,0.0,1.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(ani3)*glm::transpose(rt3)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    draw_obj(magnolia_vertices, magnolia_elements, magnolia_normals, vec3(0.679, 0.187, 0.375), vec3(0.574, 0.437, 0.855), vec3(0.679, 0.187, 0.375));
    glUniform1i(enablelighting,true);
    // END OF FLOWERS
    // END OF PAIR

    // START PICNIC BASKET
    glUniform1i(enablelighting,true);
    tr = Transform::translate(0.0, 1.0, 0.0);
    sc = Transform::scale(0.13,0.13,0.13);
    mat4 sc2 = Transform::scale(2.0,1.0,1.0);
    mat4 tmp = Transform::homogen(Transform::rotate(-90, vec3(1.0,0.0,0.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(tmp)*glm::transpose(sc2)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, weave);
    if (texton) glUniform1i(istex,1);
    glUniform1i(enablelighting,false);
    glBegin(GL_QUADS);
    glTexCoord2d(1, 1); glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2d(1, 0); glVertex3f(1.0,0.0,0.0);
    glTexCoord2d(0, 0); glVertex3f(1.0,-1.0,0.0);
    glTexCoord2d(0, 1); glVertex3f(0.0,-1.0,0.0);
    
    glTexCoord2d(1, 1); glVertex3f(1.0,0.0,0.0);
    glTexCoord2d(1, 0); glVertex3f(1.0,0.0,-1.0);
    glTexCoord2d(0, 0); glVertex3f(1.0,-1.0,-1.0);
    glTexCoord2d(0, 1); glVertex3f(1.0,-1.0,0.0);
    glDisable(GL_TEXTURE_2D);

    glTexCoord2d(1, 1); glVertex3f(1.0,0.0,-1.0);
    glTexCoord2d(1, 0); glVertex3f(0.0,0.0,-1.0);
    glTexCoord2d(0, 0); glVertex3f(0.0,-1.0,-1.0);
    glTexCoord2d(0, 1); glVertex3f(1.0,-1.0,-1.0);
    glDisable(GL_TEXTURE_2D);
    
    glTexCoord2d(1, 1); glVertex3f(0.0,0.0,-1.0);
    glTexCoord2d(1, 0); glVertex3f(0.0,0.0,0.0);
    glTexCoord2d(0, 0); glVertex3f(0.0,-1.0,0.0);
    glTexCoord2d(0, 1); glVertex3f(0.0,-1.0,-1.0);
    glDisable(GL_TEXTURE_2D);
    glEnd();
    glUniform1i(istex,-1);
    glUniform1i(enablelighting,true);
    // END PICNIC BASKET

    // PICNIC BLANKET
    tr = Transform::translate(0.4, 0.7, 0.01);
    sc = Transform::scale(0.65,0.65,0.65);
    mat4 turn = Transform::homogen(Transform::rotate(90,vec3(0.0,1.0,0.0)));
    transf = mv*glm::transpose(tr)*glm::transpose(turn)*glm::transpose(sc);
    glLoadMatrixf(&transf[0][0]) ; 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, bump);
    if (texton) glUniform1i(istex,2);
    glUniform1i(enablelighting,false);
    glBegin(GL_QUADS);
    glTexCoord2d(1, 1); glVertex3f(0.0,0.0,0.0);
    glTexCoord2d(1, 0); glVertex3f(0.0,1.0,0.0);
    glTexCoord2d(0, 0); glVertex3f(0.0,1.0,-1.0);
    glTexCoord2d(0, 1); glVertex3f(0.0,0.0,-1.0);
    glEnd();
    // END PICNIC BLANKET

    glutSwapBuffers();
}
