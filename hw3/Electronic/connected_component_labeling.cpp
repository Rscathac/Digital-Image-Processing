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

void Write_RGB_image(int x_size, int y_size, unsigned char image[3][256][256], char filename[]) {

	FILE *fp = fopen(filename, "wb");
	fwrite(image, sizeof(unsigned char), x_size*y_size*3, fp);
	fclose(fp);
	return;
}

void Connected_component_labeling(int x_size, int y_size, unsigned char image[][256], unsigned char rgb_image[3][256][256]) {
		
	// Expand the original image's border
	const int window = 3;
	
	// Connected component labeling
	int labeled[256][256];
	int label = 1;
	bool find_new = true;

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			labeled[i][j] = 0;
		}
	}

	
	for(int r = 0; r < y_size; r++) {
		for(int c = 0; c < x_size; c++) {
			if(image[r][c] == 0 || labeled[r][c] != 0)
				continue;

			labeled[r][c] = label;
			find_new = true;
			unsigned char dilation[300][300];
			for(int i = 0; i < 300; i++)
				for(int j = 0; j < 300; j++)
					dilation[i][j] = 0;
			dilation[r][c] = 255;

			while(find_new) {
				unsigned char new_dilation[300][300];
				for(int i = 0; i < 300; i++)
					for(int j = 0; j < 300; j++)
						new_dilation[i][j] = dilation[i][j];
				
				find_new = false;
				// Gi(j, k) = (Gi-1(j, k)@H(j, k)) & F(j, k)
				// eight connectivity 
				for(int i = -1; i <= 1; i++) {
					for(int j = -1; j <= 1; j++) {
					
						// Dilation
						
						for(int m = 0; m < y_size; m++) {
							for(int n = 0; n < x_size; n++) {
								if( (m+i) < 0 || (n+j) < 0 )
									continue;
								dilation[m+i][n+j] |= new_dilation[m][n];
							}
						}
					}
				}
				// intersection with F(j, k)
				for(int m = 0; m < y_size; m++) {
					for(int n = 0; n < x_size; n++) {
						dilation[m][n] &= image[m][n]; 
						if(dilation[m][n] != 0) {
							if(labeled[m][n] == 0) {
								labeled[m][n] = label;
								find_new = true;
							}									
						}
					}
				}
			}
			label++;
		}
	}
	
	// G(j, k) = F(j, k) - (F(j,k)@(H(j,k)))
//	printf("%d\n", label);
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			image[i][j] = labeled[i][j] * 30;
			if(labeled[i][j] == 1) {		// Red
				rgb_image[0][i][j] = 255;
				rgb_image[1][i][j] = 0;
				rgb_image[2][i][j] = 0;
			}

			if(labeled[i][j] == 2) {		// Orange
				rgb_image[0][i][j] = 255;
				rgb_image[1][i][j] = 165;
				rgb_image[2][i][j] = 0;
			}

			if(labeled[i][j] == 3) {		// Yellow
				rgb_image[0][i][j] = 255;
				rgb_image[1][i][j] = 255;
				rgb_image[2][i][j] = 0;
			}

			if(labeled[i][j] == 4) {		// Green
				rgb_image[0][i][j] = 0;
				rgb_image[1][i][j] = 255;
				rgb_image[2][i][j] = 0;
			}

			if(labeled[i][j] == 5) {		// Blue
				rgb_image[0][i][j] = 0;
				rgb_image[1][i][j] = 127;
				rgb_image[2][i][j] = 255;
			}
			
			if(labeled[i][j] == 6) {		// Indigo
				rgb_image[0][i][j] = 0;
				rgb_image[1][i][j] = 0;
				rgb_image[2][i][j] = 255;
			}

			if(labeled[i][j] == 7) {		// Violet
				rgb_image[0][i][j] = 139;
				rgb_image[1][i][j] = 0;
				rgb_image[2][i][j] = 255;
			}

			if(labeled[i][j] == 8) {		// White
				rgb_image[0][i][j] = 255;
				rgb_image[1][i][j] = 255;
				rgb_image[2][i][j] = 255;
			}



//			printf("%d ", labeled[i][j]);
		}
	}

	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "raw/sample1.raw";
	char write_filename[] = "output/C.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	unsigned char rgb_image[3][y_size][x_size];
	
	Read_image(x_size, y_size, image, read_filename);
	Connected_component_labeling(x_size, y_size, image, rgb_image);
//  Write_image(x_size, y_size, image, write_filename);
	Write_RGB_image(x_size, y_size, rgb_image, write_filename);
	return 0;

}
