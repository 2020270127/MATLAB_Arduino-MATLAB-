#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define _FOLD_ "(Trace route)"
#define TraceFN "AES.traces"
#define AlignedTraceFN "AligendAES.traces"

double cov(float* x, float* y, int size)
{
   double Sxy = 0, Sx = 0, Sy = 0;
   int i;
   for (i = 0; i < size; i++)
   {
      Sxy += x[i] * y[i]; //E(XY)
      Sx += x[i];
      Sy += y[i];
   }
   return (Sxy - Sx * Sy / (double)size) / (double)size;
}

double corr(float* x, float* y, int size)
{
   double Sxy = 0, Sx = 0, Sy = 0, Sxx = 0, Syy = 0;
   int i;
   for (i = 0; i < size; i++)
   {
      Sxy += x[i] * y[i]; //E(XY)
      Sx += x[i];
      Sy += y[i];
      Sxx += x[i] * x[i];
      Syy += y[i] * y[i];
   }
   return ((double)size * Sxy - Sx * Sy) / sqrt(((double)size * Sxx - Sx * Sx) * ((double)size * Syy - Sy * Sy));
}

void subalign(float* data1, float* data2, int windowsize, int stepsize, int threshold, int TraceLength)
{
   //data 배열에 저장되어 있는 전력파형을 기준으로 data1 배열에 저장되어 있는 전력파형을 정렬

   int m, j, size, maxcovpos, k;
   float *x, *y;
   double covval, maxcov;


   for (m = 0; m < (TraceLength-windowsize); m += stepsize)
   {
      for (j = -threshold; j < threshold; j++)
      {
         maxcovpos = 0;
         maxcov = 0;

         if (j < 0)
         {
            x = data1 + m;
            y = data2 + m - j;
            size = windowsize + j;
         }
         else
         {
            x = data1 + m + j;
            y = data2 + m;
            size = windowsize - j;
         }
         covval=cov(x, y, size);
         if (covval > maxcov)
         {
            maxcovpos = j;
            maxcov = covval;
         }
         if (maxcovpos < 0)
         {
            for (k = m; k > (TraceLength + maxcovpos); k++)
            {
               data2[k] = data2[k - maxcovpos];
            }
         }
         else
         {
            for (k = (TraceLength - maxcovpos - 1); k >= m; k--)
            {
               data2[k + maxcovpos] = data2[k];
            }
         }
      }
   }

}

void Alignment()//파형에 따라서 조정
{
   int windowsize = 500; //부분부분 정령을 맞추고 싶은 파형의 길이
   int stepsize = 450; //한 부분의 정렬을 맞춘 후에 몇 포인트씩 이동후 다시 정렬을 맞출 것인지 결정
   int threshold = 100;//좌우로 얼마나 흔들면서 cov값을 계산해서 최대값과 이동할 포인트수를 계산
   char buf[256];
   float* data, *data1;
   int err, TraceLength, TraceNum, i;
   FILE* rfp, *wfp;

   sprintf_s(buf, 256 * sizeof(char), "%s%s", _FOLD_, TraceFN);
   if ((err = fopen_s(&rfp, buf, 'rb')))
   {
      printf("File Open Error");
   }

   sprintf_s(buf, 256 * sizeof(char), "%s%s", _FOLD_, AlignedTraceFN);
   if ((err = fopen_s(&wfp, buf, 'wb')))
   {
      printf("File Open Error");
   }

   fread(&TraceLength, sizeof(int), 1, rfp);
   fwrite(&TraceLength, sizeof(int), 1, wfp);
   fread(&TraceNum, sizeof(int), 1, rfp);
   fwrite(&TraceNum, sizeof(int), 1, wfp);

   data = (float*)calloc(TraceLength, sizeof(float));
   data1 = (float*)calloc(TraceLength, sizeof(float));

   fread(data, sizeof(float), TraceLength, rfp);
   fwrite(data, sizeof(float), TraceLength, wfp);

   for (i = 1; i < TraceNum; i++)
   {
      fread(data1, sizeof(float), TraceLength, rfp);
      subalign(data, data1, windowsize, stepsize, threshold, TraceLength);
      fwrite(data1, sizeof(float), TraceLength, wfp);
   }
   fclose(rfp);
   fclose(wfp);
   free(data);
   free(data1);
}

int main()
{
   Alignment();
   return 0;
}
