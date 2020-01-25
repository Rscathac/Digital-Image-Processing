#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<vector>
#include<algorithm>
#define GRAY_SCALE_LEVEL 256
#define _USE_MATH_DEFINES

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

void Write_image(int x_size, int y_size, char filename[], unsigned char accumulator[750][720]) {

	FILE *fp = fopen(filename, "wb");
	fwrite(accumulator, sizeof(unsigned char), 750*720, fp);
	fclose(fp);
	return;
}

void Hough_transform(int x_size, int y_size, unsigned char image[][256], unsigned char accumulator[750][720]) {
	
	float cos_table[720];
	float sin_table[720];
//	int accumulator[1024][180];

	for(int i = 0; i < 750; i++)
		for(int j = 0; j < 720; j++)
			accumulator[i][j] = 0;

	// Create cos & sin value table
	float angle = -90.0;
	for(int i = 0; i < 720; angle+=0.25, i++) {
		float theta = angle*M_PI/180.0;
		cos_table[i] = cos(theta);
		sin_table[i] = sin(theta);
	}

	// Count accumulator
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			if(image[i][j] == 0)
				continue;

			for(int a = 0; a < 720; a++) {
				float frho;
				frho = i*sin_table[a]+j*cos_table[a];
				
				int rho = round(frho);
				
				rho += 375;
				accumulator[rho][a]++;
			}
		}
	}


	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "output/E.raw";
	char write_filename[] = "output/H1.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	unsigned char accumulator[750][720];
	Read_image(x_size, y_size, image, read_filename);
	Hough_transform(x_size, y_size, image, accumulator);
	Write_image(750, 720, write_filename, accumulator);
	return 0;

}
