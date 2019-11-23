#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h> 
#include <sys/time.h>

__global__ void AddArraysAtDevice(int *a, int *b, int count){
	int t_id = blockIdx.x * blockDim.x + threadIdx.x;
	if(t_id < count){
		a[t_id] += b[t_id];
	}
}

int main(){
	int count = 1000;
	int *h_a = (int*)malloc(sizeof(int) * 1000);
	int *h_b = (int*)malloc(sizeof(int) * 1000);

	for(int i=0; i<count; i++){
		h_a[i] = i;
		h_b[i] = count-i;
	}

	int *d_a, *d_b;

	if(cudaMalloc(&d_a, sizeof(int)*count) != cudaSuccess){
		printf("Problem with memory allocation\n");
		return 0;
	}
	if(cudaMalloc(&d_b, sizeof(int)*count) != cudaSuccess){
		printf("Problem with memory allocation\n");
		return 0;
	}

	if(cudaMemcpy(d_a, h_a, sizeof(int)*count, cudaMemcpyHostToDevice) != cudaSuccess){
		printf("Problem with copying from host to device\n");
		return 0;
	}

	if(cudaMemcpy(d_b, h_b, sizeof(int)*count, cudaMemcpyHostToDevice) != cudaSuccess){
		printf("Problem with copying from host to device\n");
		return 0;
	}

	AddArraysAtDevice<<<count / 256 + 1, 256>>>(d_a, d_b, count);

	if(cudaMemcpy(h_a, d_a, sizeof(int)*count, cudaMemcpyDeviceToHost) != cudaSuccess){
		printf("Problem with copying from device to host\n");
		return 0;
	}

	printf("Numbers added on GPU!!\n");

	for(int i=0; i<count; i++){
		printf("Num %d: %d\n", i+1, h_a[i]);
	}

	free(h_a);
	free(h_b);
	cudaFree(d_a);
	cudaFree(d_b);
	
	return 0;
}