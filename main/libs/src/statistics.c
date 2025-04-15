#include "statistics.h"
#include "esp_log.h"

static const char *TAG = "STATISTICS";

void swap(double* xp, double* yp) 
{ 
    int temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

void selectionSortAscending(double srcarr[], double destarr[], int n) 
{ 
    int i, j, min_idx; 
  
    // One by one move boundary of 
    // unsorted subarray 
    for (i = 0; i < n; i++) { 
        // Find the minimum element in 
        // unsorted array 
        min_idx = i; 
        for (j = i + 1; j < n; j++) 
            if (srcarr[j] < srcarr[min_idx]) 
                min_idx = j; 
  
        // Swap the found minimum element 
        // with the first element 
        swap(&srcarr[min_idx], &srcarr[i]);
        destarr[i] = srcarr[i];
    }
}

double quartile_I(double arr[], int size)
{
    int qr1 = (size + 1 ) / 4;
    ESP_LOGV(TAG, "quartile_I %lf\n", arr[qr1]);
    return arr[qr1];
}

double quartile_III(double arr[], int size)
{
    int qr3 = 3*(size + 1 ) / 4;
    ESP_LOGV(TAG, "quartile_III %lf\n", arr[qr3]);
    return arr[qr3];
}

typedef struct{
    int flag;
    double value;
}mean_t;

double trimmed_mean(double arr[], int size)
{
    double qr1, qr3, iqr;
    double meanarr[size];
    selectionSortAscending(arr, meanarr, size);
    qr1 = quartile_I(meanarr, size);
    qr3 = quartile_III(meanarr, size);
    iqr = qr3 - qr1;
    mean_t mean[size];
    for (int i = 0; i < size; i++)
    {
        if(meanarr[i] > (qr3+T_FACTOR*iqr) || meanarr[i] < (qr1 - T_FACTOR*iqr))
        {
            mean[i].flag= 0;
            mean[i].value= 0;
        }else{
            mean[i].flag= 1;
            mean[i].value= meanarr[i];
        }
    }
    int numberofelements = 0;
    double sumofelements = 0;
    for (int i = 0; i < size; i++)
    {
        /* code */
        numberofelements += mean[i].flag;
        sumofelements +=mean[i].value;
    }
    ESP_LOGI(TAG, "numberofelements %d\n", numberofelements);
    ESP_LOGI(TAG, "sumofelements %lf\n", sumofelements);
    if (numberofelements != 0)
    {
        return sumofelements / numberofelements; 
    } else {
        return 0;
    }
    
       
}

double mean_distance(double arr[], int size)
{
    int numberofelements = quantityofmostFrequent(arr, size);
    double reliablearray[numberofelements];
    mostFrequent(arr,reliablearray, size);
    double sumofarray = 0;
    for (int i = 0; i < numberofelements; i++)
    {
        sumofarray += reliablearray[i];
    }
    return sumofarray / numberofelements;
}

int quantityofmostFrequent(double srcarr[], int size) 
{ 
    int numberofelements = 0, count = 0;
    int threshold = size * THRESHOL_VALUE; 
    for (int i = 0; i < size; i++) { 
        for (int j = 0; j < size; j++) { 
            if (srcarr[i] <= srcarr[j]*1.1 && srcarr[i] >= srcarr[j]*0.9){
                count++;
            }
        }
        if(count > threshold){
            count = 0;            
            numberofelements++;
        }else{
            count = 0;
        }  
    }
    if (numberofelements == 0)
    {
        return 1; 
    }
    
    return numberofelements; 
}

void mostFrequent(double srcarr[], double destarr[], int size) 
{ 
    int numberofelements = 0, count = 0; 
    int threshold = size * THRESHOL_VALUE; 
    for (int i = 0; i < size; i++) { 
        for (int j = 0; j < size; j++) { 
            if (srcarr[i] <= srcarr[j]*1.1 && srcarr[i] >= srcarr[j]*0.9){
                count++;
            }
        }
        if(count > threshold){
            count = 0;
            destarr[numberofelements] = srcarr[i];
            numberofelements++;
        }else{
            count = 0;
        }
    }
} 


