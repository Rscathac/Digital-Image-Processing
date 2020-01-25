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


void Swirled_disk(int x_size, int y_size, unsigned char image[][256], unsigned char output[][256]) {
	
	// Switch to polar coordinate
	int radius = x_size / 2;
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			float dis = pow((i-radius)*(i-radius)+(j-radius)*(j-radius), 0.5);

			// Remove off-disk point 
			if(dis >= radius) {
				output[i][j] = 255;
				continue;
			}

			// Swirl the point according to the distance to the center point
			float k = 70;
			float theta = atan2(j-radius, i-radius);
			float new_theta = theta + 2-(dis/k);
			int new_i = cos(new_theta) * dis + radius;
			int new_j = sin(new_theta) * dis + radius;

			if(new_i > 255)
				new_i = 255;
			if(new_i < 0)
				new_i = 0;
			if(new_j > 255)
				new_j = 255;
			if(new_j < 0)
				new_j = 0;
			
			// backward tracing
			output[i][j] = image[new_i][new_j];

		}
	}

	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "output/C.raw";
	char write_filename[] = "output/D.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	unsigned char output[y_size][x_size];
	Read_image(x_size, y_size, image, read_filename);
	Swirled_disk(x_size, y_size, image, output);
	Write_image(x_size, y_size, output, write_filename);
	
	return 0;

}
