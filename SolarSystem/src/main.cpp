/* This program demonstrates the planets in our solar system rotating
 * around the sun.
 */

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <windows.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <stdlib.h>

GLuint sunTex, starTex, mercuryTex, venusTex, earthTex, marsTex, jupiterTex,
		ringOfSaturnTex, saturnTex, uranusTex, neptuneTex;

float eyeX = 10, eyeY = 12, eyeZ = 13;
float centerX = 0, centerY = 0, centerZ = 0;
float upX = 0, upY = 7, upZ = 0;

void usage() {
	std::cout
			<< "\n\n\
		a,A: Parallel Orthographic Front View\n\
		d,D: Parallel Orthographic Side View\n\
		w,W: Parallel Orthographic Top View\n\
		s,S: Perspective Views\n";
	std::cout.flush();
}

struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	GLubyte *data;
};

// getint and getshort are helper functions to load larger data types
// in Big Endian CPUs such as those in older Mac (PowerPC) or Solaris
// (SPARC) workstations. They are needed because BMP files are
// designed for Little Endian CPUs like the Intel x86 series.
//
// Originally from the Xv bmp loader.

// Ensure that Little Endian ints are read into memory correctly on Big Endian platforms
static unsigned int getint(FILE *fp) {
	unsigned int c, c1, c2, c3;
	c = ((unsigned int) getc(fp));  // get 4 bytes
	c1 = ((unsigned int) getc(fp)) << 8;
	c2 = ((unsigned int) getc(fp)) << 16;
	c3 = ((unsigned int) getc(fp)) << 24;
	return c | c1 | c2 | c3;
}

// Ensure that Little Endian shorts are read into memory correctly on Big Endian platforms
static unsigned short getshort(FILE* fp) {
	unsigned short c, c1;
	//get 2 bytes
	c = ((unsigned short) getc(fp));
	c1 = ((unsigned short) getc(fp)) << 8;
	return c | c1;
}

// quick and dirty bitmap loader...for 24 bit bitmaps with 1 plane only.
bool ImageLoad(char *filename, Image *image) {
	FILE *file;
	unsigned long size;          // size of the image in bytes.
	size_t i, j, k, linediff;		// standard counter.
	unsigned short int planes;   // number of planes in image (must be 1)
	unsigned short int bpp;      // number of bits per pixel (must be 24)
	char temp;                   // temporary storage for bgr-rgb conversion.

	// make sure the file is there.
	if ((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return false;
	}

	// seek through the bmp header, up to the width/height:
	fseek(file, 18, SEEK_CUR);

	// read the width
	image->sizeX = getint(file);
	printf("Width of %s: %lu\n", filename, image->sizeX);

	// read the height
	image->sizeY = getint(file);
	printf("Height of %s: %lu\n", filename, image->sizeY);

	// calculate the size (assuming 24 bits or 3 bytes per pixel).
	// BMP lines are padded to the nearest double word boundary.
	// fortunat
	size = 4.0 * ceil(image->sizeX * 24.0 / 32.0) * image->sizeY;

	// read the planes
	planes = getshort(file);
	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return false;
	}

	// read the bpp
	bpp = getshort(file);
	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}

	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	// allocate space for the data.
	image->data = new GLubyte[size];
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return false;
	}

	// read the data
	i = fread(image->data, size, 1, file);
	if (i != 1) {
		printf("Error reading image data from %s.\n", filename);
		return false;
	}

	// reverse all of the colors (bgr -> rgb)
	// calculate distance to 4 byte boundary for each line
	// if this distance is not 0, then there will be a color reversal error
	//  unless we correct for the distance on each line.
	linediff = 4.0 * ceil(image->sizeX * 24.0 / 32.0) - image->sizeX * 3.0;
	k = 0;
	for (j = 0; j < image->sizeY; j++) {
		for (i = 0; i < image->sizeX; i++) {
			temp = image->data[k];
			image->data[k] = image->data[k + 2];
			image->data[k + 2] = temp;
			k += 3;
		}
		k += linediff;
	}
	return true;
}

// Load Bitmap for Stars and Convert To Textures
void LoadGLTexturesStars() {
	// Load Texture
	Image *imageStars;

	// allocate space for texture
	imageStars = new Image();
	if (imageStars == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\stars.bmp", imageStars)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &starTex);
	glBindTexture(GL_TEXTURE_2D, starTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageStars->sizeX,      // x size from image
			imageStars->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageStars->data        // image data itself
			);
}

void LoadGLTexturesMercury() {
	// Load Texture
	Image *imageMercury;

	// allocate space for texture
	imageMercury = new Image();
	if (imageMercury == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\mercury.bmp", imageMercury)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &mercuryTex);
	glBindTexture(GL_TEXTURE_2D, mercuryTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageMercury->sizeX,      // x size from image
			imageMercury->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageMercury->data        // image data itself
			);
}

void LoadGLTexturesVenus() {
	// Load Texture
	Image *imageVenus;

	// allocate space for texture
	imageVenus = new Image();
	if (imageVenus == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\venus.bmp", imageVenus)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &venusTex);
	glBindTexture(GL_TEXTURE_2D, venusTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageVenus->sizeX,      // x size from image
			imageVenus->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageVenus->data        // image data itself
			);
}

// Load Bitmap for Mars And Convert To Textures
void LoadGLTexturesMars() {
	// Load Texture
	Image *imageMars;

	// allocate space for texture
	imageMars = new Image();
	if (imageMars == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\mars.bmp", imageMars)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &marsTex);
	glBindTexture(GL_TEXTURE_2D, marsTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageMars->sizeX,      // x size from image
			imageMars->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageMars->data        // image data itself
			);
}

void LoadGLTexturesEarth() {
	// Load Texture
	Image *imageEarth;

	// allocate space for texture
	imageEarth = new Image();
	if (imageEarth == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\earth.bmp", imageEarth)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &earthTex);
	glBindTexture(GL_TEXTURE_2D, earthTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageEarth->sizeX,      // x size from image
			imageEarth->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageEarth->data        // image data itself
			);
}

void LoadGLTexturesJupiter() {
	// Load Texture
	Image *imageJupiter;

	// allocate space for texture
	imageJupiter = new Image();
	if (imageJupiter == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\jupiter.bmp", imageJupiter)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &jupiterTex);
	glBindTexture(GL_TEXTURE_2D, jupiterTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageJupiter->sizeX,      // x size from image
			imageJupiter->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageJupiter->data        // image data itself
			);
}

void LoadGLTexturesRingOfSaturn() {
	// Load Texture
	Image *imageRingOfSaturn;

	// allocate space for texture
	imageRingOfSaturn = new Image();
	if (imageRingOfSaturn == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\ringOfSaturn.bmp",
			imageRingOfSaturn)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &ringOfSaturnTex);
	glBindTexture(GL_TEXTURE_2D, ringOfSaturnTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageRingOfSaturn->sizeX,      // x size from image
			imageRingOfSaturn->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageRingOfSaturn->data        // image data itself
			);
}

void LoadGLTexturesSaturn() {
	// Load Texture
	Image *imageSaturn;

	// allocate space for texture
	imageSaturn = new Image();
	if (imageSaturn == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\saturn.bmp", imageSaturn)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &saturnTex);
	glBindTexture(GL_TEXTURE_2D, saturnTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageSaturn->sizeX,      // x size from image
			imageSaturn->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageSaturn->data        // image data itself
			);
}

void LoadGLTexturesUranus() {
	// Load Texture
	Image *imageUranus;

	// allocate space for texture
	imageUranus = new Image();
	if (imageUranus == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\uranus.bmp", imageUranus)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &uranusTex);
	glBindTexture(GL_TEXTURE_2D, uranusTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageUranus->sizeX,      // x size from image
			imageUranus->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageUranus->data        // image data itself
			);
}

void LoadGLTexturesNeptune() {
	// Load Texture
	Image *imageNeptune;

	// allocate space for texture
	imageNeptune = new Image();
	if (imageNeptune == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\neptune.bmp", imageNeptune)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current

	glGenTextures(2, &neptuneTex);
	glBindTexture(GL_TEXTURE_2D, neptuneTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC

	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageNeptune->sizeX,      // x size from image
			imageNeptune->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageNeptune->data        // image data itself
			);
}

// Load Bitmap for Sun And Convert To Textures
void LoadGLTexturesSun() {
	// Load Texture
	Image *imageSun;

	// allocate space for texture
	imageSun = new Image();
	if (imageSun == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}

	/*load picture from file
	 *You have to edit the path to lead to where your desired
	 *texture is located on your device
	 */
	if (!ImageLoad("C:\\Users\\allir\\Desktop\\sun.bmp", imageSun)) {
		exit(1);
	}

	// Create Texture Name and Bind it as current
	glGenTextures(1, &sunTex);
	glBindTexture(GL_TEXTURE_2D, sunTex);

	//Set Texture Parameters
	// scale linearly when image bigger than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// scale linearly when image smaller than texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// Load texture into OpenGL RC
	glTexImage2D(GL_TEXTURE_2D,     // 2D texture
			0,                  // level of detail 0 (normal)
			3,	                // 3 color components
			imageSun->sizeX,      // x size from image
			imageSun->sizeY,      // y size from image
			0,	                // border 0 (normal)
			GL_RGB,             // rgb color data order
			GL_UNSIGNED_BYTE,   // color component types
			imageSun->data        // image data itself
			);
}

// Sets initial parameters and assumes no defaults.
void InitGL(int Width, int Height) {
	LoadGLTexturesSun();
	LoadGLTexturesStars();
	LoadGLTexturesMercury();
	LoadGLTexturesVenus();
	LoadGLTexturesEarth();
	LoadGLTexturesMars();
	LoadGLTexturesJupiter();
	LoadGLTexturesRingOfSaturn();
	LoadGLTexturesSaturn();
	LoadGLTexturesUranus();
	LoadGLTexturesNeptune();
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
}

// In the GLUT library someone put the polar regions on the z-axis - yech!!
// We fixed it so that they are on the y-axis.  We do this by rotating -90
// degrees about the x-axis which brings (0,0,1) to (0,1,0).
void myWireSphere(GLfloat radius, int slices, int stacks) {
	glPushMatrix();
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutWireSphere(radius, slices, stacks);
}

static int yearForPlanet = 0, dayForPlanet = 0;

void createPlanet(GLuint texture, double distance, double radius, double year,
		double day) {
	glBindTexture(GL_TEXTURE_2D, texture);
	glRotatef((GLfloat) year, 0.0, 1.0, 0.0);
	glTranslatef(distance, 0.0, 0.0);
	glRotatef((GLfloat) day, 0.0, 1.0, 0.0);
	//draw planet with radius of radius
	myWireSphere(radius, 15, 15);
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	GLfloat light_ambient[] = { 10.2, 10.2, 10.2, 11.0 };
	GLfloat light_position[] = { 0.0, 0.0, 0.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//enable lighting parameters
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, starTex);
	//create a sphere and use it as a background to store the texture for the stars
	myWireSphere(20, 30, 30);
	GLUquadric *quadricStars = gluNewQuadric();
	gluQuadricDrawStyle(quadricStars, GLU_FILL);
	gluQuadricNormals(quadricStars, GLU_SMOOTH);
	gluQuadricTexture(quadricStars, GL_TRUE);
	gluSphere(quadricStars, 20.0, 20, 20);
	gluDeleteQuadric(quadricStars);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, sunTex);
//	//draw sun with radius of 1
	myWireSphere(1, 15, 15);
	GLUquadric *quadricSun = gluNewQuadric();
	gluQuadricDrawStyle(quadricSun, GLU_FILL);
	gluQuadricNormals(quadricSun, GLU_SMOOTH);
	gluQuadricTexture(quadricSun, GL_TRUE);
	gluSphere(quadricSun, 1.2, 20, 20);
	gluDeleteQuadric(quadricSun);
	glPopMatrix();

	glPushMatrix();
	createPlanet(mercuryTex, 2.0, 0.05, 4.14 * yearForPlanet, dayForPlanet);
	GLUquadric * quadricMercury = gluNewQuadric();
	gluQuadricDrawStyle(quadricMercury, GLU_FILL);
	gluQuadricNormals(quadricMercury, GLU_SMOOTH);
	gluQuadricTexture(quadricMercury, GL_TRUE);
	gluSphere(quadricMercury, .06, 20, 20);
	gluDeleteQuadric(quadricMercury);
	glPopMatrix();

	glPushMatrix();
	createPlanet(venusTex, 3.5, 0.17, 1.62 * yearForPlanet, dayForPlanet);
	GLUquadric * quadricVenus = gluNewQuadric();
	gluQuadricDrawStyle(quadricVenus, GLU_FILL);
	gluQuadricNormals(quadricVenus, GLU_SMOOTH);
	gluQuadricTexture(quadricVenus, GL_TRUE);
	gluSphere(quadricVenus, .18, 20, 20);
	gluDeleteQuadric(quadricVenus);
	glPopMatrix();

	glPushMatrix();
	createPlanet(earthTex, 5.0, 0.19, yearForPlanet, dayForPlanet);
	GLUquadric *quadricEarth = gluNewQuadric();
	gluQuadricDrawStyle(quadricEarth, GLU_FILL);
	gluQuadricNormals(quadricEarth, GLU_SMOOTH);
	gluQuadricTexture(quadricEarth, GL_TRUE);
	gluSphere(quadricEarth, 0.2, 20, 20);
	gluDeleteQuadric(quadricEarth);
	glPopMatrix();

	glPushMatrix();
	createPlanet(marsTex, 6.5, 0.06, 0.53 * yearForPlanet, dayForPlanet);
	GLUquadric *quadricMars = gluNewQuadric();
	gluQuadricDrawStyle(quadricMars, GLU_FILL);
	gluQuadricNormals(quadricMars, GLU_SMOOTH);
	gluQuadricTexture(quadricMars, GL_TRUE);
	gluSphere(quadricMars, .07, 20, 20);
	gluDeleteQuadric(quadricMars);
	glPopMatrix();

	glPushMatrix();
	createPlanet(jupiterTex, 9.0, 0.9, 0.08 * yearForPlanet, dayForPlanet);
	GLUquadric *quadricJupiter = gluNewQuadric();
	gluQuadricDrawStyle(quadricJupiter, GLU_FILL);
	gluQuadricNormals(quadricJupiter, GLU_SMOOTH);
	gluQuadricTexture(quadricJupiter, GL_TRUE);
	gluSphere(quadricJupiter, 1.0, 20, 20);
	gluDeleteQuadric(quadricJupiter);
	glPopMatrix();

	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glBindTexture(GL_TEXTURE_2D, ringOfSaturnTex);
	glRotatef((GLfloat) 0.03 * yearForPlanet, 0.0, 1.0, 0.0);
	glTranslatef(11.5, 0.0, 0.0);
	glRotatef((GLfloat) dayForPlanet, 0.0, 1.0, 0.0);
	glRotatef(90, 1, 0, 0);

	GLUquadric *quadricRingOfSaturn = gluNewQuadric();
	gluQuadricDrawStyle(quadricRingOfSaturn, GLU_FILL);
	gluQuadricNormals(quadricRingOfSaturn, GLU_SMOOTH);
	gluQuadricTexture(quadricRingOfSaturn, GL_TRUE);
	gluDisk(quadricRingOfSaturn, 1, 1.5, 100.0, 1.0);
	gluDeleteQuadric(quadricRingOfSaturn);

	glPopMatrix();

	glPushMatrix();
	createPlanet(saturnTex, 11.5, 0.7, 0.03 * yearForPlanet, dayForPlanet);
	GLUquadric *quadricSaturn = gluNewQuadric();
	gluQuadricDrawStyle(quadricSaturn, GLU_FILL);
	gluQuadricNormals(quadricSaturn, GLU_SMOOTH);
	gluQuadricTexture(quadricSaturn, GL_TRUE);
	gluSphere(quadricSaturn, 0.8, 20, 20);
	gluDeleteQuadric(quadricSaturn);
	glPopMatrix();

	glPushMatrix();
	createPlanet(uranusTex, 14.0, 0.5, 0.01189 * yearForPlanet, dayForPlanet);
	GLUquadric *quadricUranus = gluNewQuadric();
	gluQuadricDrawStyle(quadricUranus, GLU_FILL);
	gluQuadricNormals(quadricUranus, GLU_SMOOTH);
	gluQuadricTexture(quadricUranus, GL_TRUE);
	gluSphere(quadricUranus, 0.6, 20, 20);
	gluDeleteQuadric(quadricUranus);
	glPopMatrix();

	glPushMatrix();
	createPlanet(neptuneTex, 16.0, 0.5, 0.006 * yearForPlanet, dayForPlanet);
	GLUquadric *quadricNeptune = gluNewQuadric();
	gluQuadricDrawStyle(quadricNeptune, GLU_FILL);
	gluQuadricNormals(quadricNeptune, GLU_SMOOTH);
	gluQuadricTexture(quadricNeptune, GL_TRUE);
	gluSphere(quadricNeptune, 0.6, 20, 20);
	gluDeleteQuadric(quadricNeptune);
	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void KeyboardFunc(unsigned char key, int x, int y) {
	switch (key) {
		//parallel front view
	case 'a':
	case 'A':
		eyeX = 0, eyeY = 0, eyeZ = 20;
		centerX = 0, centerY = 0, centerZ = 0;
		upX = 0, upY = 1, upZ = 0;
		glutPostRedisplay();
		break;
		//top view
	case 'w':
	case 'W':
		eyeX = 0, eyeY = 20, eyeZ = 0; //was 6
		centerX = 0, centerY = 0, centerZ = 0;
		upX = 0, upY = 1, upZ = 1; //both 1
		glutPostRedisplay();
		break;
		//parallel side view
	case 'd':
	case 'D':
		eyeX = 20, eyeY = 0, eyeZ = 0;
		centerX = 0, centerY = 0, centerZ = 0;
		upX = 0, upY = 1, upZ = 0;
		glutPostRedisplay();
		break;
		//perspective
	case 's':
	case 'S':
		eyeX = 10, eyeY = 12, eyeZ = 13;
		centerX = 0, centerY = 0, centerZ = 0;
		upX = 0, upY = 1, upZ = 0;
		glutPostRedisplay();
		break;
	}
}

void timer(int v) {
	dayForPlanet = (dayForPlanet + 1) % 365;
	yearForPlanet = (yearForPlanet + 2) % 60190;
	glLoadIdentity();
	gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, timer, v);
}

void reshape(GLint w, GLint h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) w / (GLfloat) h, 1.0, 40.0);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
	usage();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("Solar System");

	glutDisplayFunc(display);
	glutKeyboardFunc(&KeyboardFunc);
	glutReshapeFunc(reshape);

	glutTimerFunc(100, timer, 0);

	glEnable(GL_DEPTH_TEST);
	InitGL(800, 600);
	glutMainLoop();
}
