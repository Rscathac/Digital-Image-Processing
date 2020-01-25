#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<queue>
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

void Canny_edge_detection(int x_size, int y_size, unsigned char image[][256]) {
		
	// Expand the original image's border
	const int window = 5;
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
	
	// Step1 : Noise reduction
	// Gaussian filter (5x5)
	float filter[5][5] = {{2, 4, 5, 4, 2}, {4, 9, 12, 9, 4}, {5, 12, 15, 12, 5}, {4, 9, 12, 9 , 4}, {2, 4, 5, 4, 2}};
	for(int i = 0; i < window; i++) 
		for(int j = 0; j < window; j++)
			filter[i][j] /= 159;

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {

			float sum = 0.0;
			for(int m = 0; m < window; m++) {
				for(int n = 0; n < window; n++) {
					sum += (big_image[i+m][j+n] * filter[m][n]);
				}
			}
			image[i][j] = round(sum);
		}
	}
	
	// Step2 : Compute gradient magnitude and orientation
	// 1st order : Prewitt Mask
	int K = 1;
	float G[256][256];
	float theta[256][256];

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			float GR = float((big_image[i+expand-1][j+expand+1]+K*big_image[i+expand][j+expand+1]+big_image[i+expand+1][j+expand+1]) - (big_image[i+expand-1][j+expand-1]+K*big_image[i+expand][j+expand-1]+big_image[i+expand+1][j+expand-1])) / (K+2);
			float GC = float((big_image[i+expand-1][j+expand-1]+K*big_image[i+expand-1][j+expand]+big_image[i+expand-1][j+expand+1]) - (big_image[i+expand+1][j+expand-1]+K*big_image[i+expand+1][j+expand]+big_image[i+expand+1][j+expand+1])) / (K+2);
			G[i][j] = pow((GR*GR + GC*GC), 0.5);
			theta[i][j] = atan(GC/GR)*180/M_PI;

		}
	}

	// Step3 : Non-maximal suppression
	float GN[256][256];
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			// Calculate the approximate direction of the normal line (Eight quadrant)
			int x_dir, y_dir;
			
			if(theta[i][j] >= -22.5 && theta[i][j] <= 22.5) {
				x_dir = 1;
				y_dir = 0;
			}
	
			else if(theta[i][j] > 22.5 && theta[i][j] <= 67.5) {
				x_dir = 1;
				y_dir = -1;
			}

			else if(theta[i][j] > -22.5 && theta[i][j] <= -67.5){
				x_dir = 1;
				y_dir = 1;
			}

			else if((theta[i][j] > 67.5 && theta[i][j] <= 90) || (theta[i][j] < -67.5 && theta[i][j] >= -90))  {
				x_dir = 0;
				y_dir = 1;
			}
			
			// Generate GN
			if(G[i][j] > G[i+y_dir][j+x_dir] && G[i][j] > G[i-y_dir][j-x_dir]) 
				GN[i][j] = G[i][j];
			else
				GN[i][j] = 0;			
		}
	}

	// Step4 : Hysteretic thresholding
	// edge(2), candidate(1), non-edge(0)
	float TH = 70;
	float TL = 30;
	int candidate[256][256];
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			if(GN[i][j] >= TH) 
				candidate[i][j] = 2;
			else {
				if(GN[i][j] >= TL)
					candidate[i][j] = 1; 
				else
					candidate[i][j] = 0;
			}	
		}
	}

	// Step5 : Connected component labeling
	int label[256][256] = {0};
	queue<pair<int, int> > bfs;
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			if(candidate[i][j] == 2) {
				bfs.push(make_pair(i, j));
				label[i][j] = 1;
			}	
		}
	}
	while(!bfs.empty()) {
		pair<int, int> node = bfs.front();
		for(int i = -1; i <= 1; i++) {
			for(int j = -1; j <= 1; j++) {
				if(i==0&j==0)
					continue;
				if(node.first+i > 255 || node.first+i < 0 || node.second+j > 255 || node.second < 0)
					continue;
				if(candidate[node.first+i][node.second+j] == 1 && label[node.first+i][node.second+j] == 0) {
					label[node.first+i][node.second+j] = 1;
					bfs.push(make_pair(node.first+i, node.second+j));
				}
			}
		}
		bfs.pop();
	}

	for(int i = 0; i < y_size; i++)
		for(int j = 0; j < x_size; j++)
			image[i][j] = label[i][j] * 255;

	return;
}

int main(int argc, char** argv) {

	char read_filename1[] = "raw/sample1.raw";
	char write_filename1[] = "output/sample1_E3.raw";
	char read_filename2[] = "raw/sample2.raw";
	char write_filename2[] = "output/sample2_E3.raw";
	char read_filename3[] = "raw/sample3.raw";
	char write_filename3[] = "output/sample3_E3.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image1[y_size][x_size];
	unsigned char image2[y_size][x_size];
	unsigned char image3[y_size][x_size];
	
	Read_image(x_size, y_size, image1, read_filename1);
	Canny_edge_detection(x_size, y_size, image1);
  	Write_image(x_size, y_size, image1, write_filename1);
	Read_image(x_size, y_size, image2, read_filename2);
	Canny_edge_detection(x_size, y_size, image2);
  	Write_image(x_size, y_size, image2, write_filename2);
	Read_image(x_size, y_size, image3, read_filename3);
	Canny_edge_detection(x_size, y_size, image3);
  	Write_image(x_size, y_size, image3, write_filename3);

	return 0;

}
