#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define NUMBER_THREADS 8
#define BUFFER_SIZE 67108864

int threads_characters[NUMBER_THREADS][128];
char file_buffer[BUFFER_SIZE];

typedef struct{
	int thread_number;
	int partition_size;
}Thread_info;

void *counter(void *thread_info_array){
	Thread_info* thread_info = (Thread_info*) thread_info_array;

	int location = thread_info->thread_number * thread_info->partition_size;
	char current = file_buffer[location];

	for (int i=0; i < thread_info->partition_size && location < BUFFER_SIZE; i++){
		current = file_buffer[location++];
		threads_characters[thread_info->thread_number][current]++;
	}


	pthread_exit(NULL);
}

int main(int argc, char* argv[]){


		pthread_t threads[NUMBER_THREADS];

		Thread_info thread_info_array[NUMBER_THREADS];

		int sourceFileDescriptor;

		if ((sourceFileDescriptor = open (argv[1], O_RDONLY)) == -1){
			fprintf(stderr, "Source File cannot be opened!");
			return -1;
		}

		ssize_t sourceLength;

		if ((sourceLength = read(sourceFileDescriptor, file_buffer, BUFFER_SIZE)) == -1){
			fprintf(stderr, "Error reading file!");
			return -1;
		}

		close(sourceFileDescriptor);

		int partition = (double) sourceLength / (double) NUMBER_THREADS;
		printf("number of threads = %i\n", NUMBER_THREADS);
		printf("partition size = %i \n", partition);

		for (int i = 0; i < NUMBER_THREADS; i++){
			thread_info_array[i].thread_number = i;
			thread_info_array[i].partition_size = partition;

			if (pthread_create(&threads[i], NULL, &counter, &thread_info_array[i]) == -1){
				fprintf(stderr, "Error creating thread!");
				return -1;
			}
		}

		for (int i=0; i<NUMBER_THREADS; i++){
			pthread_join(threads[i], NULL);
		}

		int sum;
		for (int j =0; j<128; j++){
			sum = 0;

			for (int k =0; k< NUMBER_THREADS; k++){
				sum += threads_characters[k][j];
			}

			printf("%i occurences of ", sum);

			if (j<=32 || j==127) printf("0x%02x\n", j);
			else printf("%c\n", j);
		}

		return 0;


}


