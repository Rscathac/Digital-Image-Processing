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

void Decrease_darkness(int x_size, int y_size, unsigned char image[][256], int intensity) {
	
	double image_double[256][256];
	F(x_size, y_size, image, image_double);
	
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++) 
			image_double[i][j] /= intensity;
	F_inverse(x_size, y_size, image, image_double);

	return;

}

void Frequency(int x_size, int y_size, unsigned char image[][256], int frequency_array[256]) {
	
	memset(frequency_array, 0, GRAY_SCALE_LEVEL*sizeof(int));
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			int idx = image[i][j];
			frequency_array[idx]++;
		}
	}
	return;
}

void Global_hist_equalization(int x_size, int y_size, unsigned char image[][256], int frequency_array[256]) {

	// Calculate cumulative density values
	int new_gray_level[256] = {0};
	int cum = 0;
	int total_pixel = x_size*y_size;

	for(int i = 0; i < 256; i++) {
		cum += frequency_array[i];
		new_gray_level[i] = round(255 * float(cum)/total_pixel);
	}
	
	for(int i = 0; i < y_size; i++) 
		for(int j = 0; j < x_size; j++)	
			image[i][j] = new_gray_level[image[i][j]];

	return;

}
void Local_hist_equalization(const int x_size, const int y_size, unsigned char image[][256], int frequency_array[256]) {

	// Calculate cumulative density values
	int window = 247;
	int expand = (window/2);
	int new_x_size = x_size + 2*expand;
	int new_y_size = y_size + 2*expand;
	unsigned char big_image[new_y_size][new_x_size];

	// Initialize the expand image array
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

	for(int n = 0; n < y_size; n++) {
		for(int m = 0; m < x_size; m++) {
			
			int frequency_seg[256] = {0};
			int new_gray_level[256] = {0};
			for(int i = 0; i < window; i++) {
				for(int j = 0; j < window; j++) {
					frequency_seg[big_image[n+i][m+j]]++;
				}
			}
			int cum = 0;
			for(int i = 0; i < 256; i++) {
				cum += frequency_seg[i];
				new_gray_level[i] = round(255 * float(cum)/(window*window));
			}				
			image[n][m] = new_gray_level[big_image[n+expand][m+expand]];
		}
	}

	return;

}

void Write_CSV(int frequency_array[256], char csv_filename[]) {
	
	FILE* fp = fopen(csv_filename, "w");
	int len = 256;
	for(int i = 0; i < len; i++) {
		if(i == 0) 
			fprintf(fp, "%d", frequency_array[i]);
		else
			fprintf(fp, "\n%d", frequency_array[i]);
	}

	return;
}

int main(int argc, char** argv) {

	char read_filename[] = "sample2.raw";
	char write_filename1[] = "output/D.raw";
	char write_filename2[] = "output/E.raw";
	char global_filename1[] = "output/HD.raw";
	char global_filename2[] = "output/HE.raw";
	char local_filename1[] = "output/LD.raw";
	char local_filename2[] = "output/LE.raw";
	char csv_filename1[] = "csv/D.csv";
	char csv_filename2[] = "csv/E.csv";
	char csv_filename_hd[] = "csv/HD.csv";
	char csv_filename_he[] = "csv/HE.csv";
	char csv_filename_ld[] = "csv/lD.csv";
	char csv_filename_le[] = "csv/lE.csv";
	const int x_size = 256;
	const int y_size = 256;
	unsigned char image1[y_size][x_size];
	unsigned char image2[y_size][x_size];
	int frequency_array1[GRAY_SCALE_LEVEL];
	int frequency_array2[GRAY_SCALE_LEVEL];

	// (a)(b) Decrease brightness
	Read_image(x_size, y_size, image1, read_filename);
    Read_image(x_size, y_size, image2, read_filename);
	Decrease_darkness(x_size, y_size, image1, 2);
	Decrease_darkness(x_size, y_size, image2, 3);
	Write_image(x_size, y_size, image1, write_filename1);
	Write_image(x_size, y_size, image2, write_filename2);

	// (c) Write frequency array into CSV file and plot histogram(on excel)
	Frequency(x_size, y_size, image1, frequency_array1);
	Frequency(x_size, y_size, image2, frequency_array2);
	Write_CSV(frequency_array1, csv_filename1);
	Write_CSV(frequency_array2, csv_filename2);

	char unsigned image1_backup[y_size][x_size];
	char unsigned image2_backup[y_size][x_size];
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			image1_backup[i][j] = image1[i][j];
			image2_backup[i][j] = image2[i][j];
		}
	}

	// (f) Global histogram equalization
	Global_hist_equalization(x_size, y_size, image1, frequency_array1);
	Global_hist_equalization(x_size, y_size, image2, frequency_array2);
	Frequency(x_size, y_size, image1, frequency_array1);
	Frequency(x_size, y_size, image2, frequency_array2);
	Write_CSV(frequency_array1, csv_filename_hd);
	Write_CSV(frequency_array2, csv_filename_he);
	Write_image(x_size, y_size, image1, global_filename1);
	Write_image(x_size, y_size, image2, global_filename2);


	// (e) Local histogram equalization
	Local_hist_equalization(x_size, y_size, image1_backup, frequency_array1);
	Local_hist_equalization(x_size, y_size, image2_backup, frequency_array2);
	Frequency(x_size, y_size, image1_backup, frequency_array1);
	Frequency(x_size, y_size, image2_backup, frequency_array2);
	Write_CSV(frequency_array1, csv_filename_ld);
	Write_CSV(frequency_array2, csv_filename_le);
	Write_image(x_size, y_size, image1_backup, local_filename1);
	Write_image(x_size, y_size, image2_backup, local_filename2);

	return 0;

}
