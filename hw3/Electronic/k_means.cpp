#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<vector>
#include<algorithm>
#include<limits>
#define GRAY_SCALE_LEVEL 256

double M[9][600][600];
double M2[9][600][600];
double T[9][600][600];

using namespace std;

void Read_image(int x_size, int y_size, unsigned char image[][512], char filename[]) {

	FILE *fp = fopen(filename, "rb");	
	if(fp == NULL) {
		puts("File not exist");
		return;
	}
	fread(image, sizeof(unsigned char), x_size*y_size, fp);
	fclose(fp);
	return;
}

void Write_image(int x_size, int y_size, unsigned char image[][512], char filename[]) {

	FILE *fp = fopen(filename, "wb");
	fwrite(image, sizeof(unsigned char), x_size*y_size, fp);
	fclose(fp);
	return;
}

void Laws_method(int x_size, int y_size, unsigned char image[][512]) {
    
    // Mirco-structure impulse response arrays
    float H[9][3][3] =
    {{{1.0/36, 2.0/36, 1.0/36}, {2.0/36, 4.0/36, 2.0/36}, {1.0/36, 2.0/36, 1.0/36}},
        {{1.0/12, 0/12, -1.0/12}, {2.0/12, 0/12, -2.0/12}, {1.0/12, 0/12, -1.0/12}},
        {{-1.0/12, 2.0/12, -1.0/12}, {-2.0/12, 4.0/12, -2.0/12}, {-1.0/12, 2.0/12, -1.0/12}},
        {{-1.0/12, -2.0/12, -1.0/12}, {0/12, 0/12, 0/12}, {1.0/12, 2.0/12, 1.0/12}},
        {{1.0/4, 0/4, -1.0/4}, {0/4, 0/4, 0/4}, {-1.0/4, 0/4, 1.0/4}},
        {{-1.0/4 ,2.0/4, -1.0/4}, {0/4, 0/4, 0/4}, {1.0/4, -2.0/4, 1.0/4}},
        {{-1.0/12, -2.0/12, -1.0/12}, {2.0/12, 4.0/12, 2.0/12}, {-1.0/12, -2.0/12, -1.0/12}},
        {{-1.0/4, 0/4, 1.0/4}, {2.0/4, 0/4, -2.0/4}, {-1.0/4, 0/4, 1.0/4}},
        {{1.0/4, -2.0/4, 1.0/4}, {-2.0/4, 4.0/4, -2.0/4}, {1.0/4, -2.0/4, 1.0/4}}};
    
    
    const int window = 13;
    const int expand = 13/2;
    
    // Convolution
    for(int k = 0; k < 9; k++) {
        for(int i = 0; i < 512; i++) {
            for(int j = 0; j < 512; j++) {
                
                for(int r = 0; r < 3; r++) {
                    for(int c = 0; c < 3; c++) {
                        int r_idx = (i-1+r) < 0 ? 0: i-1+r;
                        int c_idx = (j-1+c) < 0 ? 0: j-1+c;
						r_idx = (i-1+r) > 511 ? 511: i-1+r;
						c_idx = (j-1+c) > 511 ? 511: j-1+c;
                        //						printf("%d %d\n", r_idx, c_idx);
                        M[k][i+expand][j+expand] += H[k][r][c]*image[r_idx][c_idx];
                    }
                }
            }
        }
    }
    
    // Expansion
    // <--->
    for(int k = 0; k < 9 ; k++) {
        for(int i = 0; i < y_size; i++) {
            for(int j = 0; j < expand; j++) {
                // <--
                M[k][i+expand][j] = M[k][i+expand][j+expand];
                // -->
                M[k][i+expand][expand+x_size+j] = M[k][i+expand][x_size+j];
            }
        }
    }
    
    // ↑ ↓
    for(int k = 0; k < 9; k++) {
        for(int i = 0; i < expand; i++) {
            for(int j = 0; j < x_size+2*expand; j++) {
                // ↑
                M[k][i][j] = M[k][i+expand][j];
                // ↓
                M[k][y_size+expand+i][j] = M[k][y_size+i][j];
            }
        }
    }
        
    // Engergy Computation
    for(int k = 0; k < 9; k++) {
        for(int i = 0; i < y_size; i++) {
            for(int j = 0; j < x_size; j++) {
                
                for(int r = 0; r < window; r++) {
                    for(int c = 0; c < window; c++) {
                        T[k][i][j] += M[k][i+r][j+c]*M[k][i+r][j+c];
                    }
                }
                
            }
        }
    }

	return;
}

void K_means(int x_size, int y_size, unsigned char image[][512]) {
	
	// Initialize four centroids
	int ini_size = 25;
	double centroid[9][4] = {0.0};
	for(int k = 0; k < 9; k++ ) {
		for(int i = -ini_size; i <= ini_size; i++) {
			for(int j = -ini_size; j <= ini_size; j++) {
				centroid[k][0] += T[k][i+128][j+100]/2500;//(ini_size*ini_size);
				centroid[k][1] += T[k][i+100][j+384]/2500;//(ini_size*ini_size);
				centroid[k][2] += T[k][i+300][j+160]/2500;//(ini_size*ini_size);
				centroid[k][3] += T[k][i+384][j+384]/2500;//(ini_size*ini_size);
			}
		}
	}	
	
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 9; j++) {
//			printf("%lf ", centroid[j][i]);

		}
	}

	unsigned char label[512][512];
	double dis[4];
	double min_dis = 0.0;
	int cluster = 0;
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			for(int c = 0; c < 4; c++) {
				dis[c] = 0,0;				
				for(int k = 0; k < 9; k++) {
					dis[c] += (T[k][i][j] - centroid[k][c]) * (T[k][i][j] - centroid[k][c]);
				}			
				if(c == 0 || dis[c] < min_dis) {
					cluster = c;
					min_dis = dis[c];
				}
			}
			label[i][j] = cluster;
		}
	}

	for(int i = 0; i < y_size; i++) {
		for(int j =0; j < x_size; j++) {
			image[i][j] = label[i][j] * 70;
		}
	}



	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "raw/sample2.raw";
	char write_filename[] = "output/E.raw";
	const int x_size = 512;
	const int y_size = 512;
	unsigned char image[y_size][x_size];
	
	Read_image(x_size, y_size, image, read_filename);
	Laws_method(x_size, y_size, image);
	K_means(x_size, y_size, image);
	Write_image(x_size, y_size, image, write_filename);
	return 0;

}
