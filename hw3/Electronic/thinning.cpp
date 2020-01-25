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

bool Thinning(int x_size, int y_size, unsigned char image[][256]) {
		
	// Expand the original image's border
	unsigned char big_image[300][300] = {0};
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++) {
			if(image[i][j] == 255)
				big_image[i+1][j+1] = 1;
			else
				big_image[i+1][j+1] = 0;
		}
		
	// Conditional array
	// bond = 4
	unsigned char Con[46][3][3] = {{{0, 1, 0}, {0, 1, 1}, {0, 0, 0}},	// Bond = 4
					  {{0, 1, 0}, {1, 1, 0}, {0, 0, 0}},
					  {{0, 0, 0}, {1, 1, 0}, {0, 1, 0}},
					  {{0, 0, 0}, {0, 1, 1}, {0, 1, 0}},
					  {{0, 0, 1}, {0, 1, 1}, {0, 0, 1}},
					  {{1, 1, 1}, {0, 1, 0}, {0, 0, 0}},
					  {{1, 0, 0}, {1, 1, 0}, {1, 0, 0}},
					  {{0, 0, 0}, {0, 1, 0}, {1, 1, 1}},
					  {{1, 1, 0}, {0, 1, 1}, {0, 0, 0}},    // Bond = 5
					  {{0, 1, 0}, {0, 1, 1}, {0, 0, 1}},
					  {{0, 1, 1}, {1, 1, 0}, {0, 0, 0}},
					  {{0, 0, 1}, {0, 1, 1}, {0, 1, 0}},
					  {{0, 1, 1}, {0, 1, 1}, {0, 0, 0}},
					  {{1, 1, 0}, {1, 1, 0}, {0, 0, 0}},
					  {{0, 0, 0}, {1, 1, 0}, {1, 1, 0}},
					  {{0, 0, 0}, {0, 1, 1}, {0, 1, 1}},
					  {{1, 1, 0}, {0, 1, 1}, {0, 0, 1}},	// Bond = 6
					  {{0, 1, 1}, {1, 1, 0}, {1, 0, 0}},
					  {{1, 1, 1}, {0, 1, 1}, {0, 0, 0}},
					  {{0, 1, 1}, {0, 1, 1}, {0, 0, 1}},
					  {{1, 1, 1}, {1, 1, 0}, {0, 0, 0}},
					  {{1, 1, 0}, {1, 1, 0}, {1, 0, 0}},
					  {{1, 0, 0}, {1, 1, 0}, {1, 1, 0}},
					  {{0, 0, 0}, {1, 1, 0}, {1, 1, 1}},
					  {{0, 0, 0}, {0, 1, 1}, {1, 1, 1}},
					  {{0, 0, 1}, {0, 1, 1}, {0, 1, 1}},
					  {{1, 1, 1}, {0, 1, 1}, {0, 0, 1}},	// Bond = 7
					  {{1, 1, 1}, {1, 1, 0}, {1, 0, 0}},
					  {{1, 0, 0}, {1, 1, 0}, {1, 1, 1}},
					  {{0, 0, 1}, {0, 1, 1}, {1, 1, 1}},
					  {{0, 1, 1}, {0, 1, 1}, {0, 1, 1}},	// Bond = 8
					  {{1, 1, 1}, {1, 1, 1}, {0, 0, 0}}, 
					  {{1, 1, 0}, {1, 1, 0}, {1, 1, 0}},
					  {{0, 0, 0}, {1, 1, 1}, {1, 1, 1}},
					  {{1, 1, 1}, {0, 1, 1}, {0, 1, 1}},	// Bond = 9
					  {{0, 1, 1}, {0, 1, 1}, {1, 1, 1}},
					  {{1, 1, 1}, {1, 1, 1}, {1, 0, 0}},
					  {{1, 1, 1}, {1, 1, 1}, {0, 0, 1}},
					  {{1, 1, 1}, {1, 1, 0}, {1, 1, 0}},
					  {{1, 1, 0}, {1, 1, 0}, {1, 1, 1}},
					  {{1, 0, 0}, {1, 1, 1}, {1, 1, 1}},
					  {{0, 0, 1}, {1, 1, 1}, {1, 1, 1}},
					  {{1, 1, 1}, {0, 1, 1}, {1, 1, 1}},	// Bond = 10
					  {{1, 1, 1}, {1, 1, 1}, {1, 0, 1}},
					  {{1, 1, 1}, {1, 1, 0}, {1, 1, 1}},
					  {{1, 0, 1}, {1, 1, 1}, {1, 1, 1}}}; 

	unsigned char candidate[256][256] = {0};
	unsigned char drop[256][256] = {0};
	bool erased = false;
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			
			if(image[i][j] == 0)
				continue;

			// Hit or miss check
			for(int k = 0; k < 46; k++) {
				
				bool hit = true;
				for(int r = 0; r < 3 && hit; r++) {
					for(int c = 0; c < 3 && hit; c++) {
						if((Con[k][r][c] != big_image[i+r][j+c])) {
							hit = false;
							break;
						}
						if(!hit)
							break;
					}
				}
				if(hit) {
					candidate[i][j] = 1;
					break;
				}
			}
		}
	}
	
	unsigned char M = 1;
	unsigned char A = 2;
	unsigned char B = 3;
	unsigned char C = 4;
	unsigned char D = 5;
	
	// Unconditional patterns
	unsigned char U[37][3][3] = {{{0, 0, M}, {0, M, 0}, {0, 0, 0}},	// Spur
					  {{M, 0, 0}, {0, M, 0}, {0, 0, 0}},
					  {{0, 0, 0}, {0, M, 0}, {0, M, 0}},	// Single 4-connection
					  {{0, 0, 0}, {0, M, M}, {0, 0, 0}},
					  {{0, 0, M}, {0, M, M}, {0, 0, 0}},	// L Cluster
					  {{0, M, M}, {0, M, 0}, {0, 0, 0}},
					  {{M, M, 0}, {0, M, 0}, {0, 0, 0}},
					  {{M, 0, 0}, {M, M, 0}, {0, 0, 0}},
					  {{0, 0, 0}, {M, M, 0}, {M, 0, 0}},    
					  {{0, 0, 0}, {0, M, 0}, {M, M, 0}},
					  {{0, 0, 0}, {0, M, 0}, {0, M, M}},
					  {{0, 0, 0}, {0, M, M}, {0, 0, M}},
					  {{0, M, M}, {M, M, 0}, {0, 0, 0}},	// 4-Connected
					  {{M, M, 0}, {0, M, M}, {0, 0, 0}},
					  {{0, M, 0}, {0, M, M}, {0, 0, M}},
					  {{0, 0, M}, {0, M, M}, {0, M, 0}},
					  {{0, A, M}, {0, M, B}, {M, 0, 0}},	// Spur corner cluster
					  {{M, B, 0}, {A, M, 0}, {0, 0, M}},
					  {{0, 0, M}, {A, M, 0}, {M, B, 0}},
					  {{M, 0, 0}, {0, M, B}, {0, A, M}},
					  {{M, M, D}, {M, M, D}, {D, D, D}},	// Corner cluster
					  {{D, M, 0}, {M, M, M}, {D, 0, 0}},	// Tee branch
					  {{0, M, D}, {M, M, M}, {0, 0, D}},
					  {{0, 0, D}, {M, M, M}, {0, M, D}},
					  {{D, 0, 0}, {M, M, M}, {D, M, 0}},
					  {{D, M, D}, {M, M, 0}, {0, M, 0}},
					  {{0, M, 0}, {M, M, 0}, {D, M, D}},
					  {{0, M, 0}, {0, M, M}, {D, M, D}},
					  {{D, M, D}, {0, M, M}, {0, M, 0}},	
					  {{M, D, M}, {D, M, D}, {A, B, C}},	// Vee branch
					  {{M, D, C}, {D, M, B}, {M, D, A}},
					  {{C, B, A}, {D, M, D}, {M, D, M}},
					  {{A, D, M}, {B, M, D}, {C, D, M}},    
					  {{D, M, 0}, {0, M, M}, {M, 0, D}},	// Diagonal branch
					  {{0, M, D}, {M, M, 0}, {D, 0, M}},
					  {{D, 0, M}, {M, M, 0}, {0, M, D}},
					  {{M, 0, D}, {0, M, M}, {D, M, 0}}};


	for(int i = 1; i < y_size-1; i++) {
		for(int j = 1; j < x_size-1; j++) {
			
			
			if(candidate[i][j] == 0)
				continue;
			
			bool hit = true;
			bool ABC = false;
			for(int k = 0; k < 37 && hit; k++) {
				bool a = false;
				bool b = false;
				bool c = false;
				
				for(int r = 0; r < 3&& hit; r++) {
					for(int c = 0; c < 3 && hit; c++) {
						if(U[k][r][c] == M) {
							if(candidate[i-1+r][j-1+c] != 1) {
								hit = false;
								break;
							}
						}

						else if(U[k][r][c] == A) {
							ABC = true;
							if(candidate[i-1+r][j-1+c] == 1)
								a = true;
						}

						else if(U[k][r][c] == B) {
							ABC = true;
							if(candidate[i-1+r][j-1+c] == 1)
								b = true;
						}

						else if(U[k][r][c] == C) {
							ABC = true;
							if(candidate[i-1+r][j-1+c] == 1)
								c = true;
						}

						else if(U[k][r][c] == 0) {
							if(candidate[i-1+r][j-1+c] != 0) {
								hit = false;
								break;
							}
						}
					}
				}

				if(ABC && ((!a && !b && !c) || (!a && !b))) {
					hit = false;
				}
				
				if(!hit) {
					// erase
					erased = true;
					drop[i][j] = 1;
				}
			}
		}	
	}

	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			if(drop[i][j] == 1) {
				image[i][j] = 0;
			}
		}
	}
	
	return erased;
	
}

int main(int argc, char** argv) {

	char read_filename[] = "raw/sample1.raw";
	char write_filename[] = "output/D1.raw";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image[y_size][x_size];
	
	Read_image(x_size, y_size, image, read_filename);
	while(Thinning(x_size, y_size, image)){}
  	Write_image(x_size, y_size, image, write_filename);
	return 0;

}
