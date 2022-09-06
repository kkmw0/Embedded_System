#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"

#include <sys/time.h>

struct timeval tstart, tfinish;
double tsecs;

double Gaussian_kernel[11][11] =
{
{1.05757e-06,	7.81441153305360e-06,	3.70224770827489e-05,	0.000112464355116679,	0.000219050652866017,	0.000273561160085806,	0.000219050652866017,	0.000112464355116679,	3.70224770827489e-05,	7.81441153305360e-06,	1.05756559815326e-06},
{7.81441153305360e-06,	5.77411251978637e-05,	0.000273561160085806,	0.000831005429087199,	0.00161857756253439,	0.00202135875836257,	0.00161857756253439,	0.000831005429087199,	0.000273561160085806,	5.77411251978637e-05,	7.81441153305360e-06},
{3.70224770827489e-05,	0.000273561160085806,	0.00129605559384320,	0.00393706926284679,	0.00766836382523672,	0.00957662749024030,	0.00766836382523672,	0.00393706926284679,	0.00129605559384320,	0.000273561160085806,	3.70224770827489e-05},
{0.000112464355116679,	0.000831005429087199,	0.00393706926284679,	0.0119597604100370,	0.0232944324734871,	0.0290912256485504,	0.0232944324734871,	0.0119597604100370,	0.00393706926284679,	0.000831005429087199,	0.000112464355116679},
{0.000219050652866017,	0.00161857756253439,	0.00766836382523672,	0.0232944324734871,	0.0453713590956603,	0.0566619704916846,	0.0453713590956603,	0.0232944324734871,	0.00766836382523672,	0.00161857756253439,	0.000219050652866017},
{0.000273561160085806,	0.00202135875836257,	0.00957662749024030,	0.0290912256485504,	0.0566619704916846,	0.0707622377639470,	0.0566619704916846,	0.0290912256485504,	0.00957662749024030,	0.00202135875836257,	0.000273561160085806},
{0.000219050652866017,	0.00161857756253439,	0.00766836382523672,	0.0232944324734871,	0.0453713590956603,	0.0566619704916846,	0.0453713590956603,	0.0232944324734871,	0.00766836382523672,	0.00161857756253439,	0.000219050652866017},
{0.000112464355116679,	0.000831005429087199,	0.00393706926284679,	0.0119597604100370,	0.0232944324734871,	0.0290912256485504,	0.0232944324734871,	0.0119597604100370,	0.00393706926284679,	0.000831005429087199,	0.000112464355116679},
{3.70224770827489e-05,	0.000273561160085806,	0.00129605559384320,	0.00393706926284679,	0.00766836382523672,	0.00957662749024030,	0.00766836382523672,	0.00393706926284679,	0.00129605559384320,	0.000273561160085806,	3.70224770827489e-05},
{7.81441153305360e-06,	5.77411251978637e-05,	0.000273561160085806,	0.000831005429087199,	0.00161857756253439,	0.00202135875836257,	0.00161857756253439,	0.000831005429087199,	0.000273561160085806,	5.77411251978637e-05,	7.81441153305360e-06},
{1.05756559815326e-06,	7.81441153305360e-06,	3.70224770827489e-05,	0.000112464355116679,	0.000219050652866017,	0.000273561160085806,	0.000219050652866017,	0.000112464355116679,	3.70224770827489e-05,	7.81441153305360e-06,	1.05756559815326e-06}
};


int main(){

	BITMAPINFOHEADER bitmapInfoHeader;

	unsigned char* imageData = NULL;
	unsigned char* imageData_test = NULL;
	unsigned char* Orign_gray = NULL;
	unsigned char* Test_gray = NULL;
	int width, height;

	/* image content allocation */
	unsigned char redOriTemp, greenOriTemp, blueOriTemp;
	unsigned char redSampleTemp, greenSampleTemp, blueSampleTemp;

	/* fidelity calculation allocation */
	double stdOri, stdSample, stdOriSample, k1, k2, c1, c2, mssim, sum_ssim;
	double mu1_mu2, mu1_sq, mu2_sq;

	/* Use Register*/
	register double mu1, mu2, sigma1_sq, sigma2_sq, sigma12;
	double ttmmpp = 0;

	int lwHor, lwVer;

	/* counter allocation */
	long x, y;
	int xx, yy;

	unsigned char* filename;
	unsigned char* test;

	//// For debugging
	////for (int i = 0; i<=0; i++){
	for (int i = 0; i <= 29; i++){
		switch(i){
		//IO1
		case 0:
			filename = "testTID/I01.bmp";
			test = "testTID/i01_16_1.bmp";
			break;
		case 1:
			filename = "testTID/I01.bmp";
			test = "testTID/i01_16_2.bmp";
			break;
		case 2:
			filename = "testTID/I01.bmp";
			test = "testTID/i01_16_3.bmp";
			break;
		case 3:
			filename = "testTID/I01.bmp";
			test = "testTID/i01_16_4.bmp";
			break;
		case 4:
			filename = "testTID/I01.bmp";
			test = "testTID/i01_16_5.bmp";
			break;
		//IO2
		case 5:
			filename = "testTID/I02.bmp";
			test = "testTID/i02_16_1.bmp";
			break;
		case 6:
			filename = "testTID/I02.bmp";
			test = "testTID/i02_16_2.bmp";
			break;
		case 7:
			filename = "testTID/I02.bmp";
			test = "testTID/i02_16_3.bmp";
			break;
		case 8:
			filename = "testTID/I02.bmp";
			test = "testTID/i02_16_4.bmp";
			break;
		case 9:
			filename = "testTID/I02.bmp";
			test = "testTID/i02_16_5.bmp";
			break;
		//I03
		case 10:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_11_1.bmp";
			break;
		case 11:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_11_2.bmp";
			break;
		case 12:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_11_3.bmp";
			break;
		case 13:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_11_4.bmp";
			break;
		case 14:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_11_5.bmp";
			break;
		case 15:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_16_1.bmp";
			break;
		case 16:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_16_2.bmp";
			break;
		case 17:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_16_3.bmp";
			break;
		case 18:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_16_4.bmp";
			break;
		case 19:
			filename = "testTID/I03.bmp";
			test = "testTID/i03_16_5.bmp";
			break;
		//IO4
		case 20:
			filename = "testTID/I04.bmp";
			test = "testTID/i04_16_1.bmp";
			break;
		case 21:
			filename = "testTID/I04.bmp";
			test = "testTID/i04_16_2.bmp";
			break;
		case 22:
			filename = "testTID/I04.bmp";
			test = "testTID/i04_16_3.bmp";
			break;
		case 23:
			filename = "testTID/I04.bmp";
			test = "testTID/i04_16_4.bmp";
			break;
		case 24:
			filename = "testTID/I04.bmp";
			test = "testTID/i04_16_5.bmp";
			break;
		//IO5
		case 25:
			filename = "testTID/I05.bmp";
			test = "testTID/i05_16_1.bmp";
			break;
		case 26:
			filename = "testTID/I05.bmp";
			test = "testTID/i05_16_2.bmp";
			break;
		case 27:
			filename = "testTID/I05.bmp";
			test = "testTID/i05_16_3.bmp";
			break;
		case 28:
			filename = "testTID/I05.bmp";
			test = "testTID/i05_16_4.bmp";
			break;
		case 29:
			filename = "testTID/I05.bmp";
			test = "testTID/i05_16_5.bmp";
			break;
		}

		printf("%s\n", filename);
		printf("%s\n", test);
		imageData = (unsigned char*)malloc(589824);
		imageData_test = (unsigned char*)malloc(589824);

		// read images
		LoadBitmapFile(filename, &width, &height, imageData);
		LoadBitmapFile(test, &width, &height, imageData_test);

		Orign_gray = (unsigned char*) calloc(width*height, sizeof(unsigned char));
		Test_gray = (unsigned char*) calloc(width*height, sizeof(unsigned char));

		// calculate SSIM
		k1 = 0.01;
		k2 = 0.03;
		c1 = pow(k1 * 255,2);
		c2 = pow(k2 * 255,2);

		// compute local window SSIM (8 x 8)
		lwHor = (int)(width -5);
		lwVer = (int)(height -5);

		gettimeofday(&tstart, NULL);

		// rgb2gray
		/* Compares with Zero */
		for(y = height - 1; y >= 0; y--){
			/* Use Register & Use local variables */
			/* Reduction in Strength */
			register int y_width = y << 9;
			/* Compares with Zero */
			for(x = width - 1; x >= 0; x-=2){
					  /* Common Subexpression Elimination */
					  /* Reduction in Strength */
					  /* Loop Unrolling */
					  /* Use register & Use local variables */
					  register int tmp = y_width + x;
					  register int temp = (tmp << 1) + tmp;

					  register int tmp2 = y_width + x - 1;
					  register int temp2 = (tmp2 << 1) + tmp2;

					  redOriTemp = imageData[temp];
					  greenOriTemp = imageData[temp + 1];
					  blueOriTemp = imageData[temp + 2];
					  Orign_gray[tmp] = 0.299 * redOriTemp + 0.587 * greenOriTemp + 0.114 * blueOriTemp + 0.5;

					  redOriTemp = imageData[temp2];
					  greenOriTemp = imageData[temp2 + 1];
					  blueOriTemp = imageData[temp2 + 2];
					  Orign_gray[tmp2] = 0.299 * redOriTemp + 0.587 * greenOriTemp + 0.114 * blueOriTemp + 0.5;

					  redSampleTemp = imageData_test[temp];
					  greenSampleTemp = imageData_test[temp + 1];
					  blueSampleTemp = imageData_test[temp + 2];
					  Test_gray[tmp] = 0.299 * redSampleTemp + 0.587 * greenSampleTemp + 0.114 * blueSampleTemp + 0.5;

					  redSampleTemp = imageData_test[temp2];
					  greenSampleTemp = imageData_test[temp2 + 1];
					  blueSampleTemp = imageData_test[temp2 + 2];
					  Test_gray[tmp2] = 0.299 * redSampleTemp + 0.587 * greenSampleTemp + 0.114 * blueSampleTemp + 0.5;

			}
		}

		sum_ssim = 0;
		// MSSIM

		for(y = 5; y < lwVer; y++){
			/* Use Register & Use local variable */
			register int y_tmp = y - 5;
			for(x = 5; x < lwHor; x++){
				/* Use Register & Use local variable */
				register int x_tmp = x - 5;
				mu1 = 0;
			  	mu2 = 0;
				sigma1_sq = 0;
			  	sigma2_sq = 0;
			  	sigma12 = 0;
				 /* Compares with Zero */
				 for(yy = 10; yy >= 0; yy--) {
					  /* Use Register & Use local variable */
					  /* Reduction in Strength */
					  register int yy_tmp = (y_tmp + yy) << 9;
					  /* Compares with Zero */
					  for(xx = 10; xx >= 0; xx--) {
						  /* Common Subexpression Elimination */
						  /* Use Register & Use local variables */
						  register int tmp = yy_tmp + (x_tmp + xx);
						  register double Orign_gray_tmp = Orign_gray[tmp];
						  register double Test_gray_tmp = Test_gray[tmp];
						  register double Gaussian_kernel_tmp = Gaussian_kernel[xx][yy];

						  register double Orign_mul_Gauss = Orign_gray_tmp * Gaussian_kernel_tmp;
						  register double Test_mul_Gauss = Test_gray_tmp * Gaussian_kernel_tmp;

						  mu1 += Orign_mul_Gauss;
						  mu2 += Test_mul_Gauss;

						  sigma1_sq += Orign_gray_tmp * Orign_mul_Gauss;
						  sigma2_sq += Test_gray_tmp * Test_mul_Gauss;
						  sigma12 += Orign_gray_tmp * Test_mul_Gauss;

					  }
				  }

				mu1_sq = mu1 * mu1;
				mu2_sq = mu2 * mu2;
				mu1_mu2 = mu1 * mu2;

				sigma1_sq -= mu1_sq;
				sigma2_sq -= mu2_sq;
				sigma12 -= mu1_mu2;

				//printf("mu1_sq: %f \n",mu1_sq);
				//printf("mu2_sq: %f \n",mu2_sq);
				//printf("mu1_mu2: %f \n",mu1_mu2);

				//printf("sigma1_sq: %f \n",sigma1_sq);
				//printf("sigma2_sq: %f \n",sigma2_sq);
				//printf("sigma12: %f \n",sigma12);

				sum_ssim += ((mu1_mu2 * 2 + c1) * (sigma12 * 2 + c2)) / ((mu1_sq + mu2_sq + c1) * (sigma1_sq + sigma2_sq + c2));

			}
		}

	mssim = sum_ssim / ((width-10)*(height-10));
	gettimeofday(&tfinish, NULL);
	tsecs = (tfinish.tv_sec - tstart.tv_sec) + 1E-6 * (tfinish.tv_usec - tstart.tv_usec);
	printf("main : elapsed time - %f sec\n",tsecs);
	//printf("sum_ssim = %5.5f\n", sum_ssim);
	//printf("lwHor*lwVer = %5.5f\n", lwHor*lwVer);
	printf("MSSIM = %5.5f\n", mssim);
	printf("\n");

	ttmmpp += tsecs;

	free(imageData);
	free(imageData_test);

	free(Orign_gray);
	free(Test_gray);


	}
	printf("\n\nTotal Time : %f\n", ttmmpp);
}
