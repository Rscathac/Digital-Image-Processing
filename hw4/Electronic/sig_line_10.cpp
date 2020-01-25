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

void Write_RGB_image(int x_size, int y_size, unsigned char image[3][256][256], char filename[]) {

	FILE *fp = fopen(filename, "wb");
	fwrite(image, sizeof(unsigned char), x_size*y_size*3, fp);
	fclose(fp);
	return;
}

void Hough_transform(int x_size, int y_size, unsigned char image[][256], unsigned char accumulator[750][720], unsigned char RGB_image[3][256][256]) {
	
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

	// Pick the top 10/20 rho value
	int num = 10;
	int top_acc[10] = {0};
	int top_idx[2][10] = {0};
	int picked_acc[750][720] = {0};
	
	for(int n = 0; n < num; n++) {
		int max_acc = 0;
		for(int i = 0; i < 750; i++) {
			for(int j = 0; j < 720; j++) {
				
				if(picked_acc[i][j] == 1)
					continue;
				
				if(accumulator[i][j] > max_acc) {
					max_acc = accumulator[i][j];
					top_idx[0][n] = i;
					top_idx[1][n] = j;
					
					for(int r = -2; r < 2; r++) {
						for(int c = -2; c < 2; c++) {
							if((i+r) < 0 || (i+r) >= 750 || (j+c) < 0 || (j+c) >= 720) 
								continue;
							accumulator[i+r][j+c] = 0;
						}
					}
					
				}
			}	
		}

		picked_acc[top_idx[0][n]][top_idx[1][n]] = 1;
		top_acc[n] = max_acc;

	}
	
	// Output 
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			for(int a = 0; a < 720; a++) {
				float frho;
				frho = i*sin_table[a]+j*cos_table[a];
				
				int rho = round(frho);
				
				rho += 375;
				
				if(picked_acc[rho][a] == 1) {
					RGB_image[0][i][j] = 0;
					RGB_image[1][i][j] = 255;
					RGB_image[2][i][j] = 255;
				}
			}

		}
	}



	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "output/E.raw";
	char write_filename[] = "output/D1.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	unsigned char RGB_image[3][y_size][x_size];
	Read_image(x_size, y_size, image, read_filename);

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			for(int c = 0; c < 3; c++) {
				RGB_image[c][i][j] = image[i][j];
			}
		}
	}

	unsigned char accumulator[750][720];
	Hough_transform(x_size, y_size, image, accumulator, RGB_image);
	Write_RGB_image(x_size, y_size, RGB_image, write_filename);
	return 0;

}
