/* CS580 Homework 6 */
/* Lujia Wang */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#define PI (float) 3.14159265358979323846
#define COLOR_BOUNDARY 4095

/* Helper Method - degree to radians*/
double DegToRad(float degree) {
	return (double)(degree * PI / 180.0);
}

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
	/* HW 3.1
	// Create rotate matrix : rotate along x axis
	// Pass back the matrix using mat value
	*/

	float cosine = (float)cos(DegToRad(degree));
	float sine = (float)sin(DegToRad(degree));

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0;
		}
	}

	mat[0][0] = 1.0;
	mat[1][1] = cosine;
	mat[1][2] = -1 * sine;
	mat[2][1] = sine;
	mat[2][2] = cosine;
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
	/* HW 3.2
	// Create rotate matrix : rotate along y axis
	// Pass back the matrix using mat value
	*/

	float cosine = (float)cos(DegToRad(degree));
	float sine = (float)sin(DegToRad(degree));

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0;
		}
	}

	mat[0][0] = cosine;
	mat[0][2] = sine;
	mat[1][1] = 1.0;
	mat[2][0] = -1 * sine;
	mat[2][2] = cosine;
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
	/* HW 3.3
	// Create rotate matrix : rotate along z axis
	// Pass back the matrix using mat value
	*/

	float cosine = (float)cos(DegToRad(degree));
	float sine = (float)sin(DegToRad(degree));

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0;
		}
	}

	mat[0][0] = cosine;
	mat[0][1] = -1 * sine;
	mat[1][0] = sine;
	mat[1][1] = cosine;
	mat[2][2] = 1.0;
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
	/* HW 3.4
	// Create translation matrix
	// Pass back the matrix using mat value
	*/

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0;
		}
	}

	mat[0][0] = 1.0;
	mat[0][3] = translate[0];
	mat[1][1] = 1.0;
	mat[1][3] = translate[1];
	mat[2][2] = 1.0;
	mat[2][3] = translate[2];
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}

int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
	/* HW 3.5
	// Create scaling matrix
	// Pass back the matrix using mat value
	*/

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mat[i][j] = 0;
		}
	}

	mat[0][0] = scale[0];
	mat[1][1] = scale[1];
	mat[2][2] = scale[2];
	mat[3][3] = 1.0;

	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
	/* HW1.1 create a framebuffer for MS Windows display:
	 -- set display resolution
	 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
	 -- allocate memory for pixel buffer
	 */

	framebuffer = (char*)malloc(3 * sizeof(char) * xRes * yRes);
	pixelbuffer = new GzPixel[xRes * yRes];

	xres = (unsigned short)xRes;
	yres = (unsigned short)yRes;


	/* HW 3.6
	- setup Xsp and anything only done once
	- init default camera
	*/

	//=> Xsp

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Xsp[i][j] = 0;
		}
	}

	Xsp[0][0] = (float)xres / 2.0;
	Xsp[0][3] = (float)xres / 2.0;
	Xsp[1][1] = -1 * (float)yres / 2.0;
	Xsp[1][3] = (float)yres / 2.0;
	Xsp[2][2] = MAXINT;
	Xsp[3][3] = 1.0;


	//=> default camera

	m_camera.FOV = DEFAULT_FOV;
	m_camera.position[0] = DEFAULT_IM_X;
	m_camera.position[1] = DEFAULT_IM_Y;
	m_camera.position[2] = DEFAULT_IM_Z;
	for (int i = 0; i < 3; i++) {
		m_camera.lookat[i] = 0;
		m_camera.worldup[i] = 0;
	}
	m_camera.worldup[1] = 1.0;

}

GzRender::~GzRender()
{
	/* HW1.2 clean up, free buffer memory */

	delete[] framebuffer;
	delete[] pixelbuffer;

}

int GzRender::GzDefault()
{
	/* HW1.3 set pixel buffer to some default values - start a new frame */

	for (int index = 0; index < xres * yres; index++) {
		pixelbuffer[index] = { 2000, 2000, 3500, 1, MAXINT }; //r,g,b,a,z - background color
	}

	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
	/* HW 3.7
	- setup for start of each frame - init frame buffer color,alpha,z
	- compute Xiw and projection xform Xpi from camera definition
	- init Ximage - put Xsp at base of stack, push on Xpi and Xiw
	- now stack contains Xsw and app can push model Xforms when needed
	*/

	//=> Xpi

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m_camera.Xpi[i][j] = 0;
		}
	}

	m_camera.Xpi[0][0] = 1.0;
	m_camera.Xpi[1][1] = 1.0;
	m_camera.Xpi[2][2] = (float)tan(DegToRad(m_camera.FOV) / 2.0);
	m_camera.Xpi[3][2] = (float)tan(DegToRad(m_camera.FOV) / 2.0);
	m_camera.Xpi[3][3] = 1.0;


	//=> Xiw
	GzCoord cl;
	for (int i = 0; i < 3; i++) {
		cl[i] = m_camera.lookat[i] - m_camera.position[i];
	}

	GzCoord capX, capY, capZ;

	//Z
	for (int i = 0; i < 3; i++) {
		capZ[i] = cl[i] / (float)sqrt((double)(cl[0] * cl[0] + cl[1] * cl[1] + cl[2] * cl[2]));
	}

	GzCoord upPrime;
	float upDotZ = m_camera.worldup[0] * capZ[0] + m_camera.worldup[1] * capZ[1] + m_camera.worldup[2] * capZ[2];
	for (int i = 0; i < 3; i++) {
		upPrime[i] = m_camera.worldup[i] - upDotZ * capZ[i];
	}

	//Y
	for (int i = 0; i < 3; i++) {
		capY[i] = upPrime[i] / (float)sqrt((double)(upPrime[0] * upPrime[0] + upPrime[1] * upPrime[1] + upPrime[2] * upPrime[2]));
	}

	//X=YxZ
	capX[0] = capY[1] * capZ[2] - capY[2] * capZ[1];
	capX[1] = capY[2] * capZ[0] - capY[0] * capZ[2];
	capX[2] = capY[0] * capZ[1] - capY[1] * capZ[0];

	//Xiw matrix

	m_camera.Xiw[0][0] = capX[0];
	m_camera.Xiw[0][1] = capX[1];
	m_camera.Xiw[0][2] = capX[2];
	m_camera.Xiw[0][3] = -1 * (capX[0] * m_camera.position[0] + capX[1] * m_camera.position[1] + capX[2] * m_camera.position[2]);

	m_camera.Xiw[1][0] = capY[0];
	m_camera.Xiw[1][1] = capY[1];
	m_camera.Xiw[1][2] = capY[2];
	m_camera.Xiw[1][3] = -1 * (capY[0] * m_camera.position[0] + capY[1] * m_camera.position[1] + capY[2] * m_camera.position[2]);

	m_camera.Xiw[2][0] = capZ[0];
	m_camera.Xiw[2][1] = capZ[1];
	m_camera.Xiw[2][2] = capZ[2];
	m_camera.Xiw[2][3] = -1 * (capZ[0] * m_camera.position[0] + capZ[1] * m_camera.position[1] + capZ[2] * m_camera.position[2]);

	m_camera.Xiw[3][0] = 0;
	m_camera.Xiw[3][1] = 0;
	m_camera.Xiw[3][2] = 0;
	m_camera.Xiw[3][3] = 1.0;


	//=> push matrices

	matlevel = -1;
	matlevelNorm = -1;

	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw);


	//inital lights number
	numlights = 0;


	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
	/* HW 3.8
	/*- overwrite renderer camera structure with new camera definition
	*/

	for (int i = 0; i < 3; i++) {
		m_camera.position[i] = camera.position[i];
		m_camera.lookat[i] = camera.lookat[i];
		m_camera.worldup[i] = camera.worldup[i];
	}
	m_camera.FOV = camera.FOV;

	return GZ_SUCCESS;
}


/*Helper method for push matrix to a general stack*/
int PushMatrix(GzMatrix *Xstack, short &level, GzMatrix matrix) {

	if (level >= MATLEVELS) //stack overflow
		return GZ_FAILURE;

	if (level == -1) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				Xstack[0][i][j] = matrix[i][j];
			}
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				float sum = 0;
				for (int k = 0; k < 4; k++) {
					sum += Xstack[level][i][k] * matrix[k][j];
				}
				Xstack[level + 1][i][j] = sum;
			}
		}
	}
	level++;

	return GZ_SUCCESS;
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
	/* HW 3.9
	- push a matrix onto the Ximage stack
	- check for stack overflow
	*/

	PushMatrix(Ximage, matlevel, matrix);

	if (matlevel >= MATLEVELS) //stack overflow
		return GZ_FAILURE;


	/* HW 4
	- push Xnorm stack in image space (Xiw)
	*/

	if (matlevelNorm == 2) { //Xiw
		GzMatrix Xiw_norm;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				Xiw_norm[i][j] = m_camera.Xiw[i][j];
			}
		}
		//"put zeros in 3 upper elements of right column prior to any matrix push"
		Xiw_norm[0][3] = 0;
		Xiw_norm[1][3] = 0;
		Xiw_norm[2][3] = 0;

		PushMatrix(Xnorm, matlevelNorm, Xiw_norm);
	}
	else if (matlevelNorm < 2
		|| (matrix[0][1] == 0 && matrix[0][2] == 0
			&& matrix[1][0] == 0 && matrix[1][2] == 0
			&& matrix[2][0] == 0 && matrix[2][1] == 0)) { //don't allow transition and scaling

		//identity matrix
		GzMatrix identity;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j)
					identity[i][j] = 1.0;
				else
					identity[i][j] = 0.0;
			}
		}

		PushMatrix(Xnorm, matlevelNorm, identity);
	}
	else {
		PushMatrix(Xnorm, matlevelNorm, matrix);
	}

	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
	/* HW 3.10
	- pop a matrix off the Ximage stack
	- check for stack underflow
	*/

	if (matlevel < 0) //stack underflow
		return GZ_FAILURE;

	matlevel--;

	return GZ_SUCCESS;
}


int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
	/* HW1.4 write pixel values into the buffer */

	//color clamping	
	if (r < 0)
		r = 0;
	else if (r > COLOR_BOUNDARY)
		r = COLOR_BOUNDARY;
	if (g < 0)
		g = 0;
	else if (g > COLOR_BOUNDARY)
		g = COLOR_BOUNDARY;
	if (b < 0)
		b = 0;
	else if (b > COLOR_BOUNDARY)
		b = COLOR_BOUNDARY;

	if (i >= 0 && i <= (xres - 1) && j >= 0 && j <= (yres - 1) && z < pixelbuffer[ARRAY(i, j)].z) {
		pixelbuffer[ARRAY(i, j)] = { r, g, b, a, z };
	}

	return GZ_SUCCESS;
}

int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
	/* HW1.5 retrieve a pixel information from the pixel buffer */

	GzPixel pix = pixelbuffer[ARRAY(i, j)];
	*r = pix.red;
	*g = pix.green;
	*b = pix.blue;
	*a = pix.alpha;
	*z = pix.z;

	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
	/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */

	fprintf(outfile, "P6 %d %d 255\r", xres, yres);

	for (int index = 0; index < xres * yres; index++) {

		GzPixel pix = pixelbuffer[index];
		char r, g, b;

		r = (char)(pix.red >> 4);
		g = (char)(pix.green >> 4);
		b = (char)(pix.blue >> 4);

		fputc(r, outfile);
		fputc(g, outfile);
		fputc(b, outfile);
	}

	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
	/* HW1.7 write pixels to framebuffer:
		- put the pixels into the frame buffer
		- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red
		- NOT red, green, and blue !!!
	*/

	for (int index = 0; index < xres * yres; index++) {

		GzPixel pix = pixelbuffer[index];
		char r, g, b;

		r = (char)(pix.red >> 4);
		g = (char)(pix.green >> 4);
		b = (char)(pix.blue >> 4);

		framebuffer[3 * index] = b;
		framebuffer[3 * index + 1] = g;
		framebuffer[3 * index + 2] = r;
	}

	return GZ_SUCCESS;
}


/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList)
{
	/* HW 2.1
	-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	-- In later homeworks set shaders, interpolaters, texture maps, and lights
	*/

	/* HW 4
	- GzPutAttribute() must accept the following tokens/values:

	- GZ_RGB_COLOR					GzColor		default flat-shade color
	- GZ_INTERPOLATE				int			shader interpolation mode
	- GZ_DIRECTIONAL_LIGHT			GzLight
	- GZ_AMBIENT_LIGHT            	GzLight		(ignore direction)
	- GZ_AMBIENT_COEFFICIENT		GzColor		Ka reflectance
	- GZ_DIFFUSE_COEFFICIENT		GzColor		Kd reflectance
	- GZ_SPECULAR_COEFFICIENT       GzColor		Ks coef's
	- GZ_DISTRIBUTION_COEFFICIENT   float		spec power
	*/

	/* HW 5
	- GZ_TEXTURE_MAP				GzTexture	texture function
	*/

	/* HW 6
	- GZ_AASHIFTX					X offset
	- GZ_AASHIFTY					Y offset
	*/

	for (int i = 0; i < numAttributes; i++) {

		if (nameList[i] == GZ_RGB_COLOR) {
			float *color = (float *)valueList[i];

			flatcolor[RED] = color[RED];
			flatcolor[GREEN] = color[GREEN];
			flatcolor[BLUE] = color[BLUE];
		}
		//shading mode - Shader
		else if (nameList[i] == GZ_INTERPOLATE) {
			int *mode = (int *)valueList[i];
			interp_mode = *mode;
		}
		//lighting - Lights
		else if (nameList[i] == GZ_DIRECTIONAL_LIGHT) {
			GzLight *light = (GzLight *)valueList[i];

			lights[numlights].direction[X] = light->direction[X];
			lights[numlights].direction[Y] = light->direction[Y];
			lights[numlights].direction[Z] = light->direction[Z];

			lights[numlights].color[RED] = light->color[RED];
			lights[numlights].color[GREEN] = light->color[GREEN];
			lights[numlights].color[BLUE] = light->color[BLUE];

			numlights++;
		}
		else if (nameList[i] == GZ_AMBIENT_LIGHT) {
			GzLight *light = (GzLight *)valueList[i];

			ambientlight.direction[X] = light->direction[X];
			ambientlight.direction[Y] = light->direction[Y];
			ambientlight.direction[Z] = light->direction[Z];

			ambientlight.color[RED] = light->color[RED];
			ambientlight.color[GREEN] = light->color[GREEN];
			ambientlight.color[BLUE] = light->color[BLUE];
		}
		//material - Shader
		else if (nameList[i] == GZ_AMBIENT_COEFFICIENT) {
			float *color = (float *)valueList[i];

			Ka[RED] = color[RED];
			Ka[GREEN] = color[GREEN];
			Ka[BLUE] = color[BLUE];
		}
		else if (nameList[i] == GZ_DIFFUSE_COEFFICIENT) {
			float *color = (float *)valueList[i];

			Kd[RED] = color[RED];
			Kd[GREEN] = color[GREEN];
			Kd[BLUE] = color[BLUE];
		}
		else if (nameList[i] == GZ_SPECULAR_COEFFICIENT) {
			float *color = (float *)valueList[i];

			Ks[RED] = color[RED];
			Ks[GREEN] = color[GREEN];
			Ks[BLUE] = color[BLUE];
		}
		else if (nameList[i] == GZ_DISTRIBUTION_COEFFICIENT) {
			float *coef = (float *)valueList[i];
			spec = *coef;
		}
		//texture function
		else if (nameList[i] == GZ_TEXTURE_MAP) {
			GzTexture func = (GzTexture)valueList[i];
			tex_fun = func;
		}
		//offsets
		else if (nameList[i] == GZ_AASHIFTX) {
			float *xoffset = (float *)valueList[i];
			Xoffset = *xoffset;
		}
		else if (nameList[i] == GZ_AASHIFTY) {
			float *yoffset = (float *)valueList[i];
			Yoffset = *yoffset;
		}
		else {
			return GZ_SUCCESS;
		}
	}


	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
		  GZ_NULL_TOKEN:		do nothing - no values
		  GZ_POSITION:			3 vert positions in model space
	-- Return error code
	*/
	/* HW 4
		  GZ_NORMAL:			3 normals
	*/
	/* HW 5
		  GZ_TEXTURE_INDEX		uv lists
	*/
	/*
	-- Xform positions of verts using matrix on top of stack
	-- Clip - just discard any triangle with any vert(s) behind view plane
			- optional: test for triangles with all three verts off-screen (trivial frustum cull)
	-- invoke triangle rasterizer
	*/

	GzCoord *position, *normal;
	GzTextureIndex *uv;

	for (int i = 0; i < numParts; i++) {
		if (nameList[i] == GZ_NULL_TOKEN)
			return GZ_SUCCESS;
		else if (nameList[i] == GZ_POSITION)
			position = (GzCoord *)valueList[i];
		else if (nameList[i] == GZ_NORMAL)
			normal = (GzCoord *)valueList[i];
		else if (nameList[i] == GZ_TEXTURE_INDEX)
			uv = (GzTextureIndex *)valueList[i];
		else
			return GZ_FAILURE;
	}


	/* HW 3
	-- 3D->4D
	*/

	//=> homogeneous coordinates: [x y z w]T = [x/w y/w z/w]T

	//Convert 3D to 4D
	float v4D[3][4]; //3 triangles: [x y z 1]
	float v4Dnorm[3][4];

	for (int t = 0; t < 3; t++) { //t for triangles
		for (int i = 0; i < 3; i++) {
			v4D[t][i] = position[t][i];
			v4Dnorm[t][i] = normal[t][i];
		}
		v4D[t][3] = 1.0; //w=1
		v4Dnorm[t][3] = 1.0;
	}

	//apply stack to every vertex
	float result[3][4]; //3 triangles: [x y z w]T
	float resultNorm[3][4];

	//Xstack dot v4D = result
	for (int t = 0; t < 3; t++) {
		for (int i = 0; i < 4; i++) {
			float sum = 0, sumNorm = 0;
			for (int j = 0; j < 4; j++) {
				sum += Ximage[matlevel][i][j] * v4D[t][j];
				sumNorm += Xnorm[matlevelNorm][i][j] * v4Dnorm[t][j];
			}
			result[t][i] = sum;
			resultNorm[t][i] = sumNorm;
		}
	}

	//convert back: [x y z w]T = [x/w y/w z/w]T
	for (int t = 0; t < 3; t++) {
		for (int i = 0; i < 3; i++) {
			position[t][i] = result[t][i] / result[t][3];
			normal[t][i] = resultNorm[t][i] / resultNorm[t][3];
		}
	}


	/* HW 6  - apply offsets (after transformation, before rasterization)*/

	for (int t = 0; t < 3; t++) {
		position[t][X] -= Xoffset;
		position[t][Y] -= Yoffset;
	}


	/* HW 2.2
	-- Invoke the rastrizer/scanline framework
	*/

	/*LEE*/

	//=> sort y-coord from low to high order

	if (position[0][Y] > position[1][Y]) {
		for (int i = 0; i < 3; i++) {
			float temp = position[0][i];
			position[0][i] = position[1][i];
			position[1][i] = temp;

			float tempn = normal[0][i];
			normal[0][i] = normal[1][i];
			normal[1][i] = tempn;
		}
		for (int i = 0; i < 2; i++) {
			float tempuv = uv[0][i];
			uv[0][i] = uv[1][i];
			uv[1][i] = tempuv;
		}
	}

	if (position[0][Y] > position[2][Y]) {
		for (int i = 0; i < 3; i++) {
			float temp = position[0][i];
			position[0][i] = position[2][i];
			position[2][i] = temp;

			float tempn = normal[0][i];
			normal[0][i] = normal[2][i];
			normal[2][i] = tempn;
		}
		for (int i = 0; i < 2; i++) {
			float tempuv = uv[0][i];
			uv[0][i] = uv[2][i];
			uv[2][i] = tempuv;
		}
	}

	if (position[1][Y] > position[2][Y]) {
		for (int i = 0; i < 3; i++) {
			float temp = position[1][i];
			position[1][i] = position[2][i];
			position[2][i] = temp;

			float tempn = normal[1][i];
			normal[1][i] = normal[2][i];
			normal[2][i] = tempn;
		}
		for (int i = 0; i < 2; i++) {
			float tempuv = uv[1][i];
			uv[1][i] = uv[2][i];
			uv[2][i] = tempuv;
		}
	}

	//=> find L/R

	//use middle vertex

	float middleX = position[1][X];
	float middleY = position[1][Y];

	//long edge info - Ax+By+C=0

	float dx = position[0][X] - position[2][X];
	float dy = position[0][Y] - position[2][Y];
	float A = dy;
	float B = -1 * dx;
	float C = dx * position[2][Y] - dy * position[2][X];

	//compare Xp with Xmiddle

	float xp = (-1 * B * middleY - C) / A;
	// Xp<Xmiddle: middle is the right - 1->2->3
	// Xp>Xmiddle: long edge is the right - 1->3->2
	if (xp > middleX) {
		for (int i = 0; i < 3; i++) {
			float temp = position[1][i];
			position[1][i] = position[2][i];
			position[2][i] = temp;

			float tempn = normal[1][i];
			normal[1][i] = normal[2][i];
			normal[2][i] = tempn;
		}
		for (int i = 0; i < 2; i++) {
			float tempuv = uv[1][i];
			uv[1][i] = uv[2][i];
			uv[2][i] = tempuv;
		}
	}

	//special case: same Y value

	if (position[0][Y] == position[1][Y]) {
		if (position[0][X] > position[1][X]) {
			for (int i = 0; i < 3; i++) {
				float temp = position[0][i];
				position[0][i] = position[1][i];
				position[1][i] = temp;

				float tempn = normal[0][i];
				normal[0][i] = normal[1][i];
				normal[1][i] = tempn;
			}
			for (int i = 0; i < 2; i++) {
				float tempuv = uv[0][i];
				uv[0][i] = uv[1][i];
				uv[1][i] = tempuv;
			}
		}
	}
	else if (position[1][Y] == position[2][Y]) {
		if (position[1][X] < position[2][X]) {
			for (int i = 0; i < 3; i++) {
				float temp = position[1][i];
				position[1][i] = position[2][i];
				position[2][i] = temp;

				float tempn = normal[1][i];
				normal[1][i] = normal[2][i];
				normal[2][i] = tempn;
			}
			for (int i = 0; i < 2; i++) {
				float tempuv = uv[1][i];
				uv[1][i] = uv[2][i];
				uv[2][i] = tempuv;
			}
		}
	}


	//=> interpolate Z

	//edge1: 1-2
	float X1 = position[1][X] - position[0][X];
	float Y1 = position[1][Y] - position[0][Y];
	float Z1 = position[1][Z] - position[0][Z];
	//edge2: 1-3
	float X2 = position[2][X] - position[0][X];
	float Y2 = position[2][Y] - position[0][Y];
	float Z2 = position[2][Z] - position[0][Z];
	//3D equation: Ax+By+Cz+D=0 (plane A,B,C face normal = (edge1 x edge2))
	float planeA = (Y1 * Z2) - (Z1 * Y2);
	float planeB = (X2 * Z1) - (X1 * Z2);
	float planeC = (X1 * Y2) - (X2 * Y1);
	float planeD = -1 * (position[0][X] * planeA + position[0][Y] * planeB + position[0][Z] * planeC);


	/*  HW 4
	--  Interpolate RGB vertex colors - for Gourand
	--  Interpolate Normals and compute color at each pixel - for Phong
	--  Clamp R dot E
	--  Compare signs of N dot L and N dot E
	*/

	//=> interpolate RGB colors with lights for Gourand

	//inital colors
	GzColor total[3], specG[3], diffG[3], ambiG[3];
	GzColor specGSum[3], diffGSum[3];

	for (int t = 0; t < 3; t++) { //t for triangle with three vertices
		for (int c = 0; c < 3; c++) { //c for colors
			total[t][c] = 0;
			specG[t][c] = 0;
			diffG[t][c] = 0;
			ambiG[t][c] = 0;

			specGSum[t][c] = 0;
			diffGSum[t][c] = 0;
		}

		for (int l = 0; l < numlights; l++) {

			GzCoord E, R;

			E[X] = 0;
			E[Y] = 0;
			E[Z] = -1.0;

			float NdotL = normal[t][X] * lights[l].direction[X] + normal[t][Y] * lights[l].direction[Y] + normal[t][Z] * lights[l].direction[Z];
			float NdotE = normal[t][X] * E[X] + normal[t][Y] * E[Y] + normal[t][Z] * E[Z];

			//compare N dot L and N dot E

			if (NdotL < 0 && NdotE < 0) { //flip normal and compute backside

				for (int k = 0; k < 3; k++) {
					normal[t][k] *= -1.0;
				}

				NdotL = normal[t][X] * lights[l].direction[X] + normal[t][Y] * lights[l].direction[Y] + normal[t][Z] * lights[l].direction[Z];
				NdotE = normal[t][X] * E[X] + normal[t][Y] * E[Y] + normal[t][Z] * E[Z];
			}

			if (NdotL > 0 && NdotE > 0) { //compute lighting model

				//R: reflection = 2 (L dot N) N - L;
				for (int k = 0; k < 3; k++) {
					R[k] = 2.0 * NdotL * normal[t][k] - lights[l].direction[k];
				}
				//normalize R
				float Rlen = (float)sqrt((double)(R[X] * R[X] + R[Y] * R[Y] + R[Z] * R[Z]));
				for (int k = 0; k < 3; k++) {
					R[k] /= Rlen;
				}

				//compare and clamp R dot E
				float RdotE = R[X] * E[X] + R[Y] * E[Y] + R[Z] * E[Z];
				if (RdotE < 0)
					RdotE = 0;
				else if (RdotE > 1)
					RdotE = 1.0;

				//specular and diffuse colors
				for (int c = 0; c < 3; c++) { //c for RGB colors
					specGSum[t][c] += lights[l].color[c] * (float)pow((double)RdotE, (double)spec);
					diffGSum[t][c] += lights[l].color[c] * NdotL;
				}
			}
		}

		for (int c = 0; c < 3; c++) {
			specG[t][c] = Ks[c] * specGSum[t][c];
			diffG[t][c] = Kd[c] * diffGSum[t][c];
			ambiG[t][c] = Ka[c] * ambientlight.color[c];

			/*  HW 5  Set KT=Kd=Ks=Ka for Gourand texture after z-buffer test*/
			if (tex_fun != 0)
				total[t][c] = specGSum[t][c] + diffGSum[t][c] + ambientlight.color[c];
			else
				total[t][c] = specG[t][c] + diffG[t][c] + ambiG[t][c];
		}
	}

	//X1, Y1, X2, Y2 no change
	//R
	float r_Z1 = total[1][RED] - total[0][RED];
	float r_Z2 = total[2][RED] - total[0][RED];
	float r_planeA = (Y1 * r_Z2) - (r_Z1 * Y2);
	float r_planeB = (X2 * r_Z1) - (X1 * r_Z2);
	float r_planeC = (X1 * Y2) - (X2 * Y1);
	float r_planeD = -1 * (position[0][X] * r_planeA + position[0][Y] * r_planeB + total[0][RED] * r_planeC);

	//G
	float g_Z1 = total[1][GREEN] - total[0][GREEN];
	float g_Z2 = total[2][GREEN] - total[0][GREEN];
	float g_planeA = (Y1 * g_Z2) - (g_Z1 * Y2);
	float g_planeB = (X2 * g_Z1) - (X1 * g_Z2);
	float g_planeC = (X1 * Y2) - (X2 * Y1);
	float g_planeD = -1 * (position[0][X] * g_planeA + position[0][Y] * g_planeB + total[0][GREEN] * g_planeC);

	//B
	float b_Z1 = total[1][BLUE] - total[0][BLUE];
	float b_Z2 = total[2][BLUE] - total[0][BLUE];
	float b_planeA = (Y1 * b_Z2) - (b_Z1 * Y2);
	float b_planeB = (X2 * b_Z1) - (X1 * b_Z2);
	float b_planeC = (X1 * Y2) - (X2 * Y1);
	float b_planeD = -1 * (position[0][X] * b_planeA + position[0][Y] * b_planeB + total[0][BLUE] * b_planeC);


	//=> interpolate normals for Phong, X1, Y1, X2, Y2 no changes

	//normal X
	float nX_Z1 = normal[1][X] - normal[0][X];
	float nX_Z2 = normal[2][X] - normal[0][X];
	float nX_planeA = (Y1 * nX_Z2) - (nX_Z1 * Y2);
	float nX_planeB = (X2 * nX_Z1) - (X1 * nX_Z2);
	float nX_planeC = (X1 * Y2) - (X2 * Y1);
	float nX_planeD = -1 * (position[0][X] * nX_planeA + position[0][Y] * nX_planeB + normal[0][X] * nX_planeC);

	//normal Y
	float nY_Z1 = normal[1][Y] - normal[0][Y];
	float nY_Z2 = normal[2][Y] - normal[0][Y];
	float nY_planeA = (Y1 * nY_Z2) - (nY_Z1 * Y2);
	float nY_planeB = (X2 * nY_Z1) - (X1 * nY_Z2);
	float nY_planeC = (X1 * Y2) - (X2 * Y1);
	float nY_planeD = -1 * (position[0][X] * nY_planeA + position[0][Y] * nY_planeB + normal[0][Y] * nY_planeC);

	//normal Z
	float nZ_Z1 = normal[1][Z] - normal[0][Z];
	float nZ_Z2 = normal[2][Z] - normal[0][Z];
	float nZ_planeA = (Y1 * nZ_Z2) - (nZ_Z1 * Y2);
	float nZ_planeB = (X2 * nZ_Z1) - (X1 * nZ_Z2);
	float nZ_planeC = (X1 * Y2) - (X2 * Y1);
	float nZ_planeD = -1 * (position[0][X] * nZ_planeA + position[0][Y] * nZ_planeB + normal[0][Z] * nZ_planeC);


	/*  HW 5
	--  Texture u,v coordinate interpolation
	*/

	//1. transform u,v to perspective space U,V

	float VzPrime;
	GzTextureIndex Ps[3];
	for (int t = 0; t < 3; t++) {
		//(9)
		VzPrime = position[t][Z] / (MAXINT - position[t][Z]);
		//(10)
		Ps[t][U] = uv[t][U] / (VzPrime + 1.0);
		Ps[t][V] = uv[t][V] / (VzPrime + 1.0);
	}

	//2-1. interpolate transformed U,V

	float U_Z1 = Ps[1][U] - Ps[0][U];
	float U_Z2 = Ps[2][U] - Ps[0][U];
	float U_planeA = (Y1 * U_Z2) - (U_Z1 * Y2);
	float U_planeB = (X2 * U_Z1) - (X1 * U_Z2);
	float U_planeC = (X1 * Y2) - (X2 * Y1);
	float U_planeD = -1 * (position[0][X] * U_planeA + position[0][Y] * U_planeB + Ps[0][U] * U_planeC);

	float V_Z1 = Ps[1][V] - Ps[0][V];
	float V_Z2 = Ps[2][V] - Ps[0][V];
	float V_planeA = (Y1 * V_Z2) - (V_Z1 * Y2);
	float V_planeB = (X2 * V_Z1) - (X1 * V_Z2);
	float V_planeC = (X1 * Y2) - (X2 * Y1);
	float V_planeD = -1 * (position[0][X] * V_planeA + position[0][Y] * V_planeB + Ps[0][V] * V_planeC);

	//=> LEE for Rasterization

	//three lines: Ax+By+C=0

	float dx12 = position[1][X] - position[0][X];
	float dy12 = position[1][Y] - position[0][Y];
	float A12 = dy12;
	float B12 = -1 * dx12;
	float C12 = dx12 * position[0][Y] - dy12 * position[0][X];

	float dx23 = position[2][X] - position[1][X];
	float dy23 = position[2][Y] - position[1][Y];
	float A23 = dy23;
	float B23 = -1 * dx23;
	float C23 = dx23 * position[1][Y] - dy23 * position[1][X];

	float dx31 = position[0][X] - position[2][X];
	float dy31 = position[0][Y] - position[2][Y];
	float A31 = dy31;
	float B31 = -1 * dx31;
	float C31 = dx31 * position[2][Y] - dy31 * position[2][X];

	//bounding box

	float minX = min(min(position[0][X], position[1][X]), position[2][X]);
	float maxX = max(max(position[0][X], position[1][X]), position[2][X]);

	float minY = min(min(position[0][Y], position[1][Y]), position[2][Y]);
	float maxY = max(max(position[0][Y], position[1][Y]), position[2][Y]);

	if (minX < 0) {
		minX = 0;
	}
	if (minY < 0) {
		minY = 0;
	}
	if (maxX > xres) {
		maxX = xres;
	}
	if (maxY > yres) {
		maxY = yres;
	}



	for (int i = floor(minX); i < ceil(maxX); i++) {
		for (int j = floor(minY); j < ceil(maxY); j++) {

			//LEE results = Ax+By+C

			float LEE12 = A12 * i + B12 * j + C12;
			float LEE23 = A23 * i + B23 * j + C23;
			float LEE31 = A31 * i + B31 * j + C31;

			if ((LEE12 > 0 && LEE23 > 0 && LEE31 > 0 && planeC != 0) //consistent signs
				|| (LEE12 < 0 && LEE23 < 0 && LEE31 < 0 && planeC != 0) //consistent signs
				|| LEE31 == 0) { //falls on line, assign to left edge only

				//z-buffer checking

				float Zpix = -1 * (planeA * i + planeB * j + planeD) / planeC;
				float Zfb = pixelbuffer[ARRAY(i, j)].z;

				if (Zpix < Zfb && Zpix >= 0) { //only overwrite or do visible ones
					GzIntensity rIn, gIn, bIn;

					/* HW 5 */
					//2-2. interpolate transformed U,V

					GzTextureIndex uvInterp;
					uvInterp[U] = -1 * (U_planeA * i + U_planeB * j + U_planeD) / U_planeC;
					uvInterp[V] = -1 * (V_planeA * i + V_planeB * j + V_planeD) / V_planeC;

					//3. transform U,V to affine space u,v

					//(9)
					float VzPrimeAffine = Zpix / (MAXINT - Zpix);
					//(11)
					GzTextureIndex P;
					P[U] = uvInterp[U] * (VzPrimeAffine + 1.0);
					P[V] = uvInterp[V] * (VzPrimeAffine + 1.0);

					//4. texture color
					GzColor textureColor;
					if (tex_fun != 0)
						tex_fun(P[U], P[V], textureColor);

					//flat shading
					if (interp_mode == GZ_FLAT) {
						rIn = ctoi(flatcolor[RED]);
						gIn = ctoi(flatcolor[GREEN]);
						bIn = ctoi(flatcolor[BLUE]);
					}
					//Gourand
					else if (interp_mode == GZ_COLOR) {
						GzColor colorG;
						colorG[RED] = -1 * (r_planeA * i + r_planeB * j + r_planeD) / r_planeC;
						colorG[GREEN] = -1 * (g_planeA * i + g_planeB * j + g_planeD) / g_planeC;
						colorG[BLUE] = -1 * (b_planeA * i + b_planeB * j + b_planeD) / b_planeC;

						if (tex_fun != 0) {
							for (int c = 0; c < 3; c++) {
								colorG[c] *= textureColor[c];
							}
						}

						rIn = ctoi(colorG[RED]);
						gIn = ctoi(colorG[GREEN]);
						bIn = ctoi(colorG[BLUE]);
					}
					//Phong
					if (interp_mode == GZ_NORMALS) { //similar with interpolating colors but with a interpolated normals
						//inital colors
						GzColor specPh, diffPh, ambiPh;
						GzColor specPhSum, diffPhSum; //summations

						for (int c = 0; c < 3; c++) {
							specPh[c] = 0;
							diffPh[c] = 0;
							ambiPh[c] = 0;

							specPhSum[c] = 0;
							diffPhSum[c] = 0;
						}

						GzCoord normals;

						normals[X] = -1 * (nX_planeA * i + nX_planeB * j + nX_planeD) / nX_planeC;
						normals[Y] = -1 * (nY_planeA * i + nY_planeB * j + nY_planeD) / nY_planeC;
						normals[Z] = -1 * (nZ_planeA * i + nZ_planeB * j + nZ_planeD) / nZ_planeC;

						for (int l = 0; l < numlights; l++) {//l for lights

							GzCoord E, R;

							E[X] = 0;
							E[Y] = 0;
							E[Z] = -1.0;

							float NdotL = normals[X] * lights[l].direction[X] + normals[Y] * lights[l].direction[Y] + normals[Z] * lights[l].direction[Z];
							float NdotE = normals[X] * E[X] + normals[Y] * E[Y] + normals[Z] * E[Z];

							//compare N dot L and N dot E

							if (NdotL < 0 && NdotE < 0) { //flip normal and compute backside

								for (int k = 0; k < 3; k++) {
									normals[k] *= -1.0;
								}

								NdotL = normals[X] * lights[l].direction[X] + normals[Y] * lights[l].direction[Y] + normals[Z] * lights[l].direction[Z];
								NdotE = normals[X] * E[X] + normals[Y] * E[Y] + normals[Z] * E[Z];
							}

							if (NdotL > 0 && NdotE > 0) { //compute lighting model

								//R: reflection = 2 (L dot N) N - L;
								for (int k = 0; k < 3; k++) {
									R[k] = 2.0 * NdotL * normals[k] - lights[l].direction[k];
								}
								//normalize R
								float Rlen = (float)sqrt((double)(R[X] * R[X] + R[Y] * R[Y] + R[Z] * R[Z]));
								for (int k = 0; k < 3; k++) {
									R[k] /= Rlen;
								}

								//compare and clamp R dot E
								float RdotE = R[X] * E[X] + R[Y] * E[Y] + R[Z] * E[Z];
								if (RdotE < 0)
									RdotE = 0;
								else if (RdotE > 1)
									RdotE = 1.0;

								//specular and diffuse colors summation part
								for (int c = 0; c < 3; c++) { //c for RGB colors
									specPhSum[c] += lights[l].color[c] * (float)pow((double)RdotE, (double)spec);
									diffPhSum[c] += lights[l].color[c] * NdotL;
								}
							}
						}

						for (int c = 0; c < 3; c++) {
							if (tex_fun != 0) {
								Kd[c] = textureColor[c];
								Ka[c] = textureColor[c];
							}
							specPh[c] = Ks[c] * specPhSum[c];
							diffPh[c] = Kd[c] * diffPhSum[c];
							ambiPh[c] = Ka[c] * ambientlight.color[c];
						}

						rIn = ctoi(specPh[RED] + diffPh[RED] + ambiPh[RED]);
						gIn = ctoi(specPh[GREEN] + diffPh[GREEN] + ambiPh[GREEN]);
						bIn = ctoi(specPh[BLUE] + diffPh[BLUE] + ambiPh[BLUE]);
					}

					GzPut(i, j, rIn, gIn, bIn, 1, Zpix);

				}
			}
		}
	}


	return GZ_SUCCESS;
}

