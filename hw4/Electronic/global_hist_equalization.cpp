#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define GRAY_SCALE_LEVEL 256

using namespace std;

void Read_image(int x_size, int y_size, unsigned char image[720][720], char filename[]) {

	FILE *fp = fopen(filename, "rb");
	
	if(fp == NULL) {
		puts("File not exist");
		return;
	}

	fread(image, sizeof(unsigned char), x_size*y_size, fp);
	fclose(fp);
	return;

}

void Write_image(int x_size, int y_size, unsigned char image[750][720], char filename[]) {

	FILE *fp = fopen(filename, "wb");
	fwrite(image, sizeof(unsigned char), x_size*y_size, fp);
	fclose(fp);
	return;

}


void Frequency(int x_size, int y_size, unsigned char image[750][720], int frequency_array[256]) {
	
	memset(frequency_array, 0, GRAY_SCALE_LEVEL*sizeof(int));
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++) {
			int idx = image[i][j];
			frequency_array[idx]++;
		}
	}
	return;
}


void Hist_equalization(int x_size, int y_size, unsigned char image[750][720], int frequency_array[256]) {

	// Calculate cumulative density values
	int new_gray_level[256] = {0};
	int cum = 0;
	int total_pixel = x_size*y_size;

	for(int i = 0; i < 256; i++) {
		cum += frequency_array[i];
		new_gray_level[i] = round((255 * float(cum)-frequency_array[0])/(total_pixel-frequency_array[0]));
		if(new_gray_level[i] >= 255) {
//			new_gray_level[i] = 0;
		}
	}
	
	for(int i = 0; i < y_size; i++) {
		for(int j = 0; j < x_size; j++)	{
			if(image[i][j] == 0)
				continue;
			image[i][j] = new_gray_level[image[i][j]];
		}
	}

	return;

}



int main(int argc, char** argv) {

	char read_filename[] = "output/H1.raw";
	char write_filename[] = "output/H2.raw";
	int x_size = 720;
	int y_size = 750;
	unsigned char image[750][720];
	int frequency_array[256];

	Read_image(x_size, y_size, image, read_filename);
	Frequency(x_size, y_size, image, frequency_array);
	Hist_equalization(x_size, y_size, image, frequency_array);
	Write_image(x_size, y_size, image, write_filename);

	return 0;

}
