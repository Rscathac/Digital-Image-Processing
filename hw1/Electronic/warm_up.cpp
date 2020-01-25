#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<math.h>

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

void Horizontally_flip(int x_size, int y_size, unsigned char image[][256]) {

	int x_mid = x_size/2;

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_mid; j++) {
			unsigned char tmp = image[i][j];
			image[i][j] = image[i][x_size-1-j];
			image[i][x_size-1-j] = tmp;
		}
	}
	
	return;
}
void F(int x_size, int y_size, unsigned char image[][256], double image_double[][256]) {
	
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++) {
			image_double[i][j] = image[i][j];
			image_double[i][j] /= 255;
		}
	return;
}

void F_inverse(int x_size, int y_size, unsigned char image[][256], double image_double[][256]) {
	
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++) 
			image[i][j] = round(image_double[i][j]*255);	
		
	return;
}

void Power_law(int x_size, int y_size, unsigned char image[][256]) {
	
	double image_double[256][256];
	F(x_size, y_size, image, image_double);
	
	double gamma = 3;
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++) 
			image_double[i][j] = pow(image_double[i][j], gamma);

	F_inverse(x_size, y_size, image, image_double);

	return;

}

int main(int argc, char** argv) {

	char read_filename[] = "sample1.raw";
	char flipped_filename[] = "output/B.raw";
	char write_filename[] = "output/B_enhanced.raw";
	int x_size = 256;
	int y_size = 256;
	unsigned char image[256][256];

	Read_image(x_size, y_size, image, read_filename);
	Horizontally_flip(x_size, y_size, image);
	Write_image(x_size, y_size, image, flipped_filename);
	Power_law(x_size, y_size, image);
	Write_image(x_size, y_size, image, write_filename);

	return 0;

}
