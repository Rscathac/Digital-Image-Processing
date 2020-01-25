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

void Second_order_edge_detection(int x_size, int y_size, unsigned char image[][256]) {
		
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
	for(int i = 0; i < new_y_size; i++) {
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
	// Laplacian generation
//	float H[3][3] = {{0, -1, 0}, {-1, 4, -1}, {0, -1, 0}};     	         // four neighbor
//	float d = 4;
//	float H[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};   // eight neighbor (non-separable)
	float H[3][3] = {{-2, 1, -2}, {1, 4, 1}, {-2, 1, -2}};       // eight neighbor (separable)
    float d = 8;


	// Filter
	float T = 70;
	int flag = 1;
	float G_prime[300][300] = {0};
	float zero[256][256] = {0};
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			for(int m = 0; m < window; m++) {
				for(int n = 0; n < window; n++) {
					G_prime[i+expand][j+expand] += (H[m][n] * big_image[i+m][j+n])/d;

				}
			}
			if((G_prime[i+expand][j+expand] > 0 && G_prime[i+expand][j+expand] < T) ||(G_prime[i+expand][j+expand] < 0 && G_prime[i+expand][j+expand] > -T))
				zero[i][j] = 1;
		}
	}
		

	// Zero-crossing
	float threshold = 150;
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			if(zero[i][j] == 0) {
				image[i][j] = 0;
				continue;
			}
			int cross = 0;
			if(G_prime[i+expand-1][j+expand-1] * G_prime[i+expand+1][j+expand+1] < -threshold)
				cross++;
			if(G_prime[i+expand-1][j+expand] * G_prime[i+expand+1][j+expand] < -threshold)
				cross++;
			if(G_prime[i+expand-1][j+expand+1] * G_prime[i+expand+1][j+expand-1] < -threshold)
				cross++;
			if(G_prime[i+expand][j+expand+1] * G_prime[i+expand][j+expand-1] < -threshold)
				cross++;
			
			if(cross > 0)
				image[i][j] = 255;
			else
				image[i][j] = 0;
			
		}
	}

	return;
}

int main(int argc, char** argv) {

	char read_filename1[] = "raw/sample1.raw";
	char write_filename1[] = "output/sample1_E2.raw";
	char read_filename2[] = "raw/sample2.raw";
	char write_filename2[] = "output/sample2_E2.raw";
	char read_filename3[] = "raw/sample3.raw";
	char write_filename3[] = "output/sample3_E2.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image1[y_size][x_size];
	unsigned char image2[y_size][x_size];
	unsigned char image3[y_size][x_size];
	
	Read_image(x_size, y_size, image1, read_filename1);
	Second_order_edge_detection(x_size, y_size, image1);
  	Write_image(x_size, y_size, image1, write_filename1);
	Read_image(x_size, y_size, image2, read_filename2);
	Second_order_edge_detection(x_size, y_size, image2);
  	Write_image(x_size, y_size, image2, write_filename2);
	Read_image(x_size, y_size, image3, read_filename3);
	Second_order_edge_detection(x_size, y_size, image3);
  	Write_image(x_size, y_size, image3, write_filename3);
	return 0;

}
