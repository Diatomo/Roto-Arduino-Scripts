


#include <math.h>
#include <Arduino.h>
#include "stats.h"

Stats::Stats(){

}

int Stats::average(int arr[], int size){
		int avg = 0;
		for (int i=0; i<size; i++){
				avg += arr[i];
		}
		avg = (avg/size);
		return avg;
}

int Stats::stdDev(int arr[], int size){
		int avg = Stats::average(arr,size);
		int dev = 0;
		for(int i=0; i<size; i++){
				dev += (arr[i] - avg) * (arr[i] - avg);
		}
		dev = sqrt(avg/(size-1));
		return dev;
}
