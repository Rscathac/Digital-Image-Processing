#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
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


void Edge_crispen(int x_size, int y_size, unsigned char image[][256]) {
	
	// Set up filter mask
	const int window = 3;
	const int b = 2;
	float filter[window][window] = {{1, b, 1}, {b, b*b, b}, {1, b, 1}};
	for(int i = 0; i < window; i++)
		for(int j = 0; j < window; j++) 
			filter[i][j] /= pow((b+2) ,2);
	
	// Expand the original image's border
	const int new_x_size = x_size+2*(window/2);
	const int new_y_size = y_size+2*(window/2);
	int expand = window/2;
	unsigned char big_image[new_y_size][new_x_size];
	unsigned char new_image[256][256];
	
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++)
			big_image[i+expand][j+expand] = image[i][j];
	
	// Expand <- ->
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < expand; j++) {
			big_image[i][j] = big_image[i][expand];
			big_image[i][x_size+expand+j] = big_image[i][x_size+expand-1];
		}
	}
	
	for(int i = 0; i < expand; i++) {
		for(int j = 0; j < new_x_size; j++) {
			big_image[i][j] = big_image[expand][j];
			big_image[y_size+expand+i][j] = big_image[y_size+expand-1][j];
		}
	}

	// Low-pass filtering
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			float sum = 0.0;
			for(int m = 0; m < window; m++) {
				for(int n = 0; n < window; n++) {
					sum += (big_image[i+m][j+n] * filter[m][n]);
				}
			}
			new_image[i][j] = round(sum);
		}
	}
	
	float G[256][256];
	float c = 0.7;
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			G[i][j] = c/(2*c-1)*float(image[i][j]) - (1-c)/(2*c-1)*float(new_image[i][j]);
			if(G[i][j] < 0)
				G[i][j] = 0;
		}
	}

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			image[i][j] = round(G[i][j]);
		}
	}

	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "raw/sample4.raw";
	char write_filename[] = "output/C.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	Read_image(x_size, y_size, image, read_filename);
	Edge_crispen(x_size, y_size, image);
	Write_image(x_size, y_size, image, write_filename);
	
	return 0;

}
