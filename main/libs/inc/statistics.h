#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#define T_FACTOR            1.5
#define THRESHOL_VALUE      0.3

void selectionSortAscending(double srcarr[], double destarr[], int n);
double trimmed_mean(double arr[], int size);
double mean_distance(double arr[], int size);
int quantityofmostFrequent(double srcarr[], int size) ;
void mostFrequent(double srcarr[], double destarr[], int size) ;

#endif 