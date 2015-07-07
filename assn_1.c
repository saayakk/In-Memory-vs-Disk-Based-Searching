#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

FILE* readFile(char *);
int getCount(FILE *);
int* allocateMemory(int *, int);
void printOutput(int*, int*, int);
void loadValues(int, int*, FILE*);
void linearSearch(int*, int*, int*, int, int);
void diskLinearSearch(int*, int*, FILE *, int);
void startBinarySearch(int*, int*, int*, int, int);
int binarySearchAlgorithm(int*, int, int, int);
void diskBinarySearch(int *, int *, FILE *, int, int);
void printTime(struct timeval, struct timeval);

int main(int arg_count, char *args[])
{
	if(arg_count != 4)
	{
		printf("Error: More Command Line Arguments expected!!!\n");
		return 1;
	}

	struct timeval stime;
	struct timeval etime;

	FILE *seekfile, *keyfile;
	seekfile = readFile(args[2]);
	keyfile = readFile(args[3]);

	int seekcount = getCount(seekfile);
	int keycount = getCount(keyfile);

	int *seek, *key, *hit;
	seek = allocateMemory(seek, seekcount);
	key = allocateMemory(key, keycount);
	hit = allocateMemory(hit, seekcount);

	loadValues(seekcount, seek, seekfile);

	if(strcmp(args[1],"--mem-lin") == 0)
	{
		gettimeofday(&stime, NULL);
		loadValues(keycount, key, keyfile);
		linearSearch(seek, key, hit, seekcount, keycount);
		gettimeofday(&etime, NULL);
		printOutput(hit, seek, seekcount);
		printTime(stime, etime);	
	}

	else if(strcmp(args[1],"--mem-bin") == 0)
	{
		gettimeofday(&stime, NULL);
		loadValues(keycount, key, keyfile);
		startBinarySearch(seek, key, hit, seekcount, keycount);
		gettimeofday(&etime, NULL);
		printOutput(hit, seek, seekcount);
		printTime(stime, etime);	
	}

	else if(strcmp(args[1],"--disk-lin") == 0)
	{
		gettimeofday(&stime, NULL);
		diskLinearSearch(seek, hit, keyfile, seekcount);
		gettimeofday(&etime, NULL);
		printOutput(hit, seek, seekcount);
		printTime(stime, etime);	
	}

	else if(strcmp(args[1],"--disk-bin") == 0)
	{
		gettimeofday(&stime, NULL);
		diskBinarySearch(seek, hit, keyfile, seekcount, keycount);
		gettimeofday(&etime, NULL);
		printOutput(hit, seek, seekcount);
		printTime(stime, etime);	
	}

	else 
	{
		printf("Wrong Search Mode!!!\n");
	}
	
	free(seek);
	free(key);
	free(hit);
	return 1;
}

FILE* readFile(char *filename)
{
	FILE *file = fopen(filename,"rb");

	if(file == 0)
	{
		printf("Error: File not found!!!");
		exit(0);
	}

	return file;
}

int getCount(FILE *file)
{
	int count = 0;
	
	if(file == 0)
	{
		printf("Error: Could not read the file!!!");
		exit(0);
	}
	
	fseek(file, 0, SEEK_END);
	count = ftell(file) / (sizeof(int));
	fseek(file, 0, SEEK_SET);
	
	return count;
}

int* allocateMemory(int *list, int count)
{
	list = (int *) malloc(count*sizeof(int));
	return list;
}

void printOutput(int *hit, int *seek, int seekcount)
{
	for(int i=0; i<seekcount; i++)
	{
		if(hit[i] == 1)
		{
			printf("%12d: Yes\n", seek[i]);
		}
		else if(hit[i] == 0)
		{
			printf("%12d: No\n", seek[i]);
		}
	}
}

void loadValues(int count, int *list, FILE *file)
{
	if(file != 0)
	{
		fread(list, sizeof(int), count, file);
	}
}

void linearSearch(int* seeklist, int* keylist, int* hitlist, int seek_count, int key_count)
{
	for(int i=0; i<seek_count; i++)
	{
		for(int j=0; j<key_count; j++)
		{
			if(seeklist[i] == keylist[j])
			{
				hitlist[i] = 1;
				break;
			}
		}
	}
}

void diskLinearSearch(int* seeklist, int* hitlist, FILE *keyfile, int seek_count)
{
	if(keyfile != 0)
	{
		int value;
		for(int i=0; i<seek_count; i++)
		{
			fread(&value, sizeof(int), 1, keyfile);
			while(!feof(keyfile))
			{
				if(seeklist[i] == value)
				{
					hitlist[i] = 1;
					break;
				}
				fread(&value, sizeof(int), 1, keyfile);
			}
			fseek(keyfile, 0, SEEK_SET);
		}
	}
	else
	{
		printf("File not found!!!\n");
	}
}

void startBinarySearch(int* seeklist, int* keylist, int* hitlist, int seek_count, int key_count)
{
	for(int i=0; i<seek_count; i++)
	{
		hitlist[i] = binarySearchAlgorithm(keylist, seeklist[i], (int)0, key_count);
	}
}

int binarySearchAlgorithm(int *keylist, int value, int min_index, int max_index)
{
	if(max_index < min_index)
	{
		return 0;
	}

	int mid_index = (min_index + max_index)/2;

	if(value == keylist[mid_index])
	{
		return 1;
	}

	if(value < keylist[mid_index])
	{
		return binarySearchAlgorithm(keylist, value, min_index, (mid_index-1));
	}

	if(value > keylist[mid_index])
	{
		return binarySearchAlgorithm(keylist, value, (mid_index+1), max_index);
	}
}

void diskBinarySearch(int *seeklist, int *hit, FILE *keyfile, int seekcount, int keycount)
{	
	int value = 0, mid = 0;

	for(int i=0; i<seekcount; i++)
	{
		int start = 0, end = keycount-1; 
		while(1)
		{
			if(end < start)
			{
				break;
			}

			mid = (start + end)/2;
			fseek(keyfile, (mid * sizeof(int)), SEEK_SET);
			fread(&value, sizeof(int), 1, keyfile);

			if(seeklist[i] == value)
			{
				hit[i] = 1;
				break;
			}

			if(seeklist[i] < value)
			{
				end = mid-1;
			}

			else if(seeklist[i] > value)
			{
				start = mid+1;
			}
		}
		fseek(keyfile, 0, SEEK_SET);
	}
}

void printTime(struct timeval stime, struct timeval etime)
{
	long time = (etime.tv_sec - stime.tv_sec)*1000000;
	time = time + (etime.tv_usec - stime.tv_usec);
	printf("Time: %ld.%06ld\n", (time/1000000), (time%1000000));
}
