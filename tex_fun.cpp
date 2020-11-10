/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

#include	<complex>

GzColor	*image = NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
	unsigned char		pixel[3];
	unsigned char     dummy;
	char  		foo[8];
	int   		i, j;
	FILE			*fd;

	if (reset) {          /* open and load texture file */
		fd = fopen("texture", "rb");
		if (fd == NULL) {
			fprintf(stderr, "texture file not found\n");
			exit(-1);
		}
		fscanf(fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
		image = (GzColor*)malloc(sizeof(GzColor)*(xs + 1)*(ys + 1));
		if (image == NULL) {
			fprintf(stderr, "malloc for texture image failed\n");
			exit(-1);
		}

		for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
			fread(pixel, sizeof(pixel), 1, fd);
			image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
			image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
			image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
		}

		reset = 0;          /* init is done */
		fclose(fd);
	}

	/* bounds-test u,v to make sure nothing will overflow image array bounds */

	if (u > 1.0)
		u = 1.0;
	else if (u < 0.0)
		u = 0.0;

	if (v > 1.0)
		v = 1.0;
	else if (v < 0.0)
		v = 0.0;

	/* determine texture cell corner values and perform bilinear interpolation */
	/* set color to interpolated GzColor value and return */

	float s, t;
	GzColor A, B, C, D;

	//U,V -> u,v
	u *= (xs - 1.0);
	v *= (ys - 1.0);

	s = u - floor(u);
	t = v - floor(v);

	for (i = 0; i < 3; i++) {
		A[i] = image[(int)(floor(u) + floor(v) * xs)][i];
		B[i] = image[(int)(ceil(u) + floor(v) * xs)][i];
		C[i] = image[(int)(ceil(u) + ceil(v) * xs)][i];
		D[i] = image[(int)(floor(u) + ceil(v) * xs)][i];
	}
	
	//bilinear interpolation
	for (i = 0; i < 3; i++) {
		color[i] = s * t * C[i] + (1.0 - s) * t * D[i] + s * (1.0 - t) * B[i] + (1.0 - s) * (1.0 - t) * A[i];
	}


	return GZ_SUCCESS;
}

float LUT(int value, int index) {
	float color;

	if (index == RED) {
		switch (value) {
		case 0: color = 1.0; //red
			break;
		case 1: color = 0.0; //green
			break;
		case 2: color = 1.0; //yellow
			break;
		case 3: color = 0.0; //blue
			break; 
		case 4: color = 1.0; //magenta
			break;
		case 5: color = 0.0; //cyan
			break;
		case 6: color = 1.0; //orange
			break;
		case 7: color = 0.0; //turquoise
			break;
		case 8: color = 0.0; //navy
			break;
		case 9: color = 0.47; //grey
			break;
		case 10: color = 0.8; //pink
			break;
		default: color = 1.0;
			break;
		}
	}
	else if (index == GREEN) {
		switch (value) {
		case 0: color = 0.0;
			break;
		case 1: color = 1.0;
			break;
		case 2: color = 1.0;
			break;
		case 3: color = 0.0;
			break;
		case 4: color = 0.0;
			break;
		case 5: color = 1.0;
			break;
		case 6: color = 0.65;
			break;
		case 7: color = 0.53;
			break;
		case 8: color = 0.0;
			break;
		case 9: color = 0.53;
			break;
		case 10: color = 0.38;
			break;
		default: color = 1.0;
			break;
		}
	}
	else if (index == BLUE) {
		switch (value) {
		case 0: color = 0.0;
			break;
		case 1: color = 0.0;
			break;
		case 2: color = 0.0;
			break;
		case 3: color = 1.0;
			break;
		case 4: color = 1.0;
			break;
		case 5: color = 1.0;
			break;
		case 6: color = 0.0;
			break;
		case 7: color = 0.55;
			break;
		case 8: color = 0.5;
			break;
		case 9: color = 0.6;
			break;
		case 10: color = 0.56;
			break;
		default: color = 1.0;
			break;
		}
	}


	return color;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	/* bounds-test u,v to make sure nothing will overflow image array bounds */

	if (u > 1.0)
		u = 1.0;
	else if (u < 0.0)
		u = 0.0;

	if (v > 1.0)
		v = 1.0;
	else if (v < 0.0)
		v = 0.0;
		
	/*
	//Simple Checkboard

	int uInt = (int)floor(u * 6);
	int vInt = (int)floor(v * 6);

	if (uInt % 2 == vInt % 2) {
		for (int c = 0; c < 3; c++)
			color[c] = 0.0;
	}
	else {
		for (int c = 0; c < 3; c++)
			color[c] = 1.0;
	}
	*/

	
	// Fractal Texture

	//The Julia Set
	//std::complex<float> C(-0.12375, 0.56805);
	//std::complex<float> Xfractal(u, v);
	
	//The Mandlebrot Set
	std::complex<float> Xfractal(0, 0);
	std::complex<float> C(u, v);
	
	float bound = pow(10,11);

	int N = 500;
	int i;

	for (i = 0; i < N; i++) {
		Xfractal = Xfractal * Xfractal + C;
		if (abs(Xfractal) >= bound)
			break;
	}
	
	float lengthX = (float)abs(Xfractal);
	float clr;

	if (lengthX >= 0 && lengthX <= 2) {				

		lengthX *= 5; //make intervals from 0.2 to 1, easy to calculate
		float Sceil = ceil(lengthX);
		float Sfloor = floor(lengthX);

		float a = Sceil - lengthX;
		float b = lengthX - Sfloor;
	
		for (int c = 0; c < 3; c++)
			color[c] = a * LUT(Sfloor, c) + b * LUT(Sceil, c);
	}
	else {			
		for (int c = 0; c < 3; c++)
			color[c] = 0.0;
	}


	return GZ_SUCCESS;
}


/* Free texture memory */
int GzFreeTexture()
{
	if (image != NULL)
		free(image);
	return GZ_SUCCESS;
}

