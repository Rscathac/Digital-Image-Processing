#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<vector>
#include<algorithm>
#define GRAY_SCALE_LEVEL 256

using namespace std;

void Read_image(int x_size, int y_size, unsigned char image[][256], char filename[]) {

	FILE *fp = fopen(filename, "rb");	
	if(fp == NULL) {
		puts("File not exist");
		return;
	}
	fread(image, sizeof(unsigned char), x_size*y_size, fp);
	fclose(fp);
	return;
}

void Write_image(int x_size, int y_size, unsigned char image[][256], char filename[]) {

	FILE *fp = fopen(filename, "wb");
	fwrite(image, sizeof(unsigned char), x_size*y_size, fp);
	fclose(fp);
	return;
}

void Boundary_extraction(int x_size, int y_size, unsigned char image[][256]) {
		
	// Expand the original image's border
	const int window = 3;
	int expand = window/2;

	// Create a new canvas 
	unsigned char canvas[300][300];
	for(int i = 0; i < 300; i++)
		for(int j = 0; j < 300; j++)
			canvas[i][j] = 255;
	
	// Erosion
	// Sternberg definition
	//int H[3][3] = {{1, 1, 1}, {1, 0, 0,}, {1, 1, 1}};
	int H[3][3] = {{0, 0, 1,}, {0, 1, 0}, {1, 0, 0}};


	for(int i = 0; i < window; i++) {
		for(int j = 0; j < window; j++) {
			if(H[i][j] == 0)
				continue;
			for(int m = 0; m < y_size; m++) {
				for(int n = 0; n < x_size; n++) {
					canvas[m+i][n+j] &= image[m][n]; 
				}
			}

		}
	}

	// G(j, k) = F(j, k) - (F(j,k)@(H(j,k)))
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			image[i][j] = image[i][j] - canvas[i+expand][j+expand];
		}
	}

	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "raw/sample1.raw";
	char write_filename[] = "output/B.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	
	Read_image(x_size, y_size, image, read_filename);
	Boundary_extraction(x_size, y_size, image);
  	Write_image(x_size, y_size, image, write_filename);
	return 0;

}
