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

void First_order_edge_detection(int x_size, int y_size, unsigned char image[][256]) {
		
	// Expand the original image's border
	const int window = 3;
	const int new_x_size = x_size+2*(window/2);
	const int new_y_size = y_size+2*(window/2);
	int expand = window/2;
	unsigned char big_image[300][300];
	
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
	
	// Edge detection
	// Prewitt Mask
	// int K = 1;
	// Sobel Mask
	int K = 2;
	int threshold = 70;
	float GR[256][256];
	float GC[256][256];

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			float GR = float((big_image[i+expand-1][j+expand+1]+K*big_image[i+expand][j+expand+1]+big_image[i+expand+1][j+expand+1]) - (big_image[i+expand-1][j+expand-1]+K*big_image[i+expand][j+expand-1]+big_image[i+expand+1][j+expand-1])) / (K+2);
			float GC = float((big_image[i+expand-1][j+expand-1]+K*big_image[i+expand-1][j+expand]+big_image[i+expand-1][j+expand+1]) - (big_image[i+expand+1][j+expand-1]+K*big_image[i+expand+1][j+expand]+big_image[i+expand+1][j+expand+1])) / (K+2);
			float G = pow((GR*GR + GC*GC), 0.5);
			if(G >= threshold)
				image[i][j] = 255;
			else 
				image[i][j] = 0;

		}
	}

	return;
}

int main(int argc, char** argv) {

	char read_filename1[] = "raw/sample1.raw";
	char write_filename1[] = "output/sample1_E1.raw";
	char read_filename2[] = "raw/sample2.raw";
	char write_filename2[] = "output/sample2_E1.raw";
	char read_filename3[] = "raw/sample3.raw";
	char write_filename3[] = "output/sample3_E1.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image1[y_size][x_size];
	unsigned char image2[y_size][x_size];
	unsigned char image3[y_size][x_size];

	Read_image(x_size, y_size, image1, read_filename1);
	First_order_edge_detection(x_size, y_size, image1);
  	Write_image(x_size, y_size, image1, write_filename1);

	Read_image(x_size, y_size, image2, read_filename2);
	First_order_edge_detection(x_size, y_size, image2);
  	Write_image(x_size, y_size, image2, write_filename2);

	Read_image(x_size, y_size, image3, read_filename3);
	First_order_edge_detection(x_size, y_size, image3);
  	Write_image(x_size, y_size, image3, write_filename3);
	return 0;

}
