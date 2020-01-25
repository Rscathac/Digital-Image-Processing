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

void PSNR(int x_size, int y_size, unsigned char original[][256], unsigned char processed[][256], int window) {
	
	// Calculate MSE
	double MSE = 0;
	double psnr_value = 0;
	
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++) 
			MSE += (processed[i][j]-original[i][j])*(processed[i][j]-original[i][j]);
	MSE /= (y_size*x_size);
	psnr_value = 10*log10(255*255/MSE);
	puts("Applying median filtering");
	printf("Window Size: %d, PSNR: %f\n",window, psnr_value);
	return;
}	

void Median_filtering(int x_size, int y_size, unsigned char image[][256], unsigned char new_image[][256], int window) {
	
//	const int window = 3;
	
	// Expand the original image's border
	const int new_x_size = x_size+2*(window/2);
	const int new_y_size = y_size+2*(window/2);
	int expand = window/2;
//	unsigned char big_image[new_y_size][new_x_size];
	unsigned char big_image[500][500];
	
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
	
	// Median filtering
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			vector<int> pixels;
			for(int m = 0; m < window; m++) {
				for(int n = 0; n < window; n++) {
					pixels.push_back(big_image[i+m][j+n]);
				}
			}
			sort(pixels.begin(), pixels.end());
			new_image[i][j] = pixels[window*window/2+1];
			pixels.clear();
		}
	}

	return;
}

int main(int argc, char** argv) {

	char original_filename[] = "sample3.raw";
	char read_filename[] = "sample5.raw";
	char write_filename[] = "output/N2.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	unsigned char new_image[y_size][x_size];
	unsigned char original_image[y_size][x_size];
	
	Read_image(x_size, y_size, original_image, original_filename);
	Read_image(x_size, y_size, image, read_filename);
	Median_filtering(x_size, y_size, image, new_image, 3);
  	Write_image(x_size, y_size, new_image, write_filename);
	PSNR(x_size, y_size ,original_image, new_image, 3);
	return 0;

}
