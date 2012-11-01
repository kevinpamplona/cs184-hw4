# version 120 

/* This is the fragment shader for reading in a scene description, including 
   lighting.  Uniform lights are specified from the main program, and used in 
   the shader.  As well as the material parameters of the object.  */

// Mine is an old machine.  For version 130 or higher, do 
// in vec4 color ;  
// in vec4 mynormal ; 
// in vec4 myvertex ;
// That is certainly more modern

varying vec4 color ;
varying vec3 mynormal ; 
varying vec4 myvertex ; 

const int numLights = 10 ; 
uniform bool enablelighting ; // are we lighting at all (global).
uniform vec4 lightposn[numLights] ; // positions of lights 
uniform vec4 lightcolor[numLights] ; // colors of lights
uniform int numused ;               // number of lights used

// Now, set the material parameters.  These could be varying and/or bound to 
// a buffer.  But for now, I'll just make them uniform.  
// I use ambient, diffuse, specular, shininess as in OpenGL.  
// But, the ambient is just additive and doesn't multiply the lights.  

uniform vec4 ambient ; 
uniform vec4 diffuse ; 
uniform vec4 specular ; 
uniform vec4 emission ; 
uniform float shininess ; 



uniform sampler2D tex; 
uniform int istex;

uniform int makeshine;
uniform int makedull;

vec4 ComputeLight (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {
    
        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong ; 
    
        return retval ;            
}

vec4 ComputeLightShine (const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {
    
        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert + phong*1000; 
    
        return retval ;            
}

vec4 ComputeLightDull(const in vec3 direction, const in vec4 lightcolor, const in vec3 normal, const in vec3 halfvec, const in vec4 mydiffuse, const in vec4 myspecular, const in float myshininess) {
    
        float nDotL = dot(normal, direction)  ;         
        vec4 lambert = mydiffuse * lightcolor * max (nDotL, 0.0) ;  

        float nDotH = dot(normal, halfvec) ; 
        vec4 phong = myspecular * lightcolor * pow (max(nDotH, 0.0), myshininess) ; 

        vec4 retval = lambert; 
    
        return retval ;            
}

void main (void) 
{
  if (makeshine == 1) {
    vec4 sum = vec4(0,0,0,0);
    vec4 finalcolor ; 
    
    const vec3 eyepos = vec3(0,0,0);
    vec4 _mypos = gl_ModelViewMatrix * myvertex;
    vec3 mypos = _mypos.xyz / _mypos.w;
    vec3 eyedirn = normalize(eyepos - mypos);
    
    vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz ;
    vec3 normal = normalize(_normal) ;
    
    for (int i=0; i<numused; i++) {
      //directional
      if (lightposn[i].w==0) {
	vec3 direction0 = normalize(lightposn[i].xyz);
	vec3 half0 = normalize(direction0 + eyedirn);
	sum = sum + ComputeLightShine(direction0, lightcolor[i], normal, half0, diffuse, specular, shininess);  
	//point
      } else { 
	vec3 position = lightposn[i].xyz / lightposn[i].w;
	vec3 direction1 = normalize(position - mypos);
	vec3 half1 = normalize(direction1 + eyedirn);
	sum = sum + ComputeLightShine(direction1, lightcolor[i], normal, half1, diffuse, specular, shininess);
      } // END ELSE
    } // END FOR
    gl_FragColor = ambient + emission + sum; 

  } else  if (enablelighting) {
    vec4 sum = vec4(0,0,0,0);
    vec4 finalcolor ; 
    
    const vec3 eyepos = vec3(0,0,0);
    vec4 _mypos = gl_ModelViewMatrix * myvertex;
    vec3 mypos = _mypos.xyz / _mypos.w;
    vec3 eyedirn = normalize(eyepos - mypos);
    
    vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz ;
    vec3 normal = normalize(_normal) ;
    
    for (int i=0; i<numused; i++) {
      //directional
      if (lightposn[i].w==0) {
	vec3 direction0 = normalize(lightposn[i].xyz);
	vec3 half0 = normalize(direction0 + eyedirn);
	sum = sum + ComputeLight(direction0, lightcolor[i], normal, half0, diffuse, specular, shininess);  
	//point
      } else { 
	vec3 position = lightposn[i].xyz / lightposn[i].w;
	vec3 direction1 = normalize(position - mypos);
	vec3 half1 = normalize(direction1 + eyedirn);
	sum = sum + ComputeLight(direction1, lightcolor[i], normal, half1, diffuse, specular, shininess);
      } // END ELSE
    } // END FOR
    gl_FragColor = ambient + emission + sum; 
  } else if (istex > 0) {
    if (istex==1) gl_FragColor = texture2D(tex, gl_TexCoord[0].st);
    if (istex==2) {
    vec4 sum = vec4(0,0,0,0);
    vec4 finalcolor ; 
    
    const vec3 eyepos = vec3(0,0,0);
    vec4 _mypos = gl_ModelViewMatrix * myvertex;
    vec3 mypos = _mypos.xyz / _mypos.w;
    vec3 eyedirn = normalize(eyepos - mypos);
    
    vec3 _normal = (gl_ModelViewMatrixInverseTranspose*vec4(mynormal,0.0)).xyz ;
    vec3 normal = normalize(texture2D(tex, gl_TexCoord[0].st).rgb * 2.0 - 1.0);

    for (int i=0; i<numused; i++) {
      //directional
      if (lightposn[i].w==0) {
	vec3 direction0 = normalize(lightposn[i].xyz);
	vec3 half0 = normalize(direction0 + eyedirn);
	sum = sum + ComputeLight(direction0, lightcolor[i], normal, half0, diffuse, specular, shininess);  
	//point
      } else { 
	vec3 position = lightposn[i].xyz / lightposn[i].w;
	vec3 direction1 = normalize(position - mypos);
	vec3 half1 = normalize(direction1 + eyedirn);
	sum = sum + ComputeLight(direction1, lightcolor[i], normal, half1, diffuse, specular, shininess);
      } // END ELSE
    } // END FOR
    gl_FragColor = ambient + emission + sum;     
    }
  } else {
    gl_FragColor = color;
  }
}