/*
Author:		Marcelo Melo Linck
University: 	PUCRS (Porto Alegre) / MSU (Bozeman, MT)
Version:	1.3
Description:	This program processes the sound data from the nodes and localizes the shooter.
Update: 	10/08/2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define N_NODES 4

//instead of the defines, the node positions can be read from
//a text file previously written

struct _position_
{
	int posX;
	int posY;
	char node;
};

struct _data_
{
	int amp;
	int time;
};

char * gettime();

int main(int argc, char **argv) 
{ 
	struct _position_ nodeArray[N_NODES];
	struct _position_ aux_pos;

  	char str [30],
  		size [2];

  	FILE * pFile;

  	int i,
  		j, 
  		k = 0,
  		sizeX, 
  		sizeY,
  		scan = 0,
  		valid_positions=0;

	//////////////////////////////// Section 1 - Map Decoding, Seeking nodes position
  
  	pFile = fopen(argv[1], "r");
  	
  	fscanf (pFile, "%s", size);
  	sizeX = atoi(size);
  	fscanf (pFile, "%s", size);
  	sizeY = atoi(size); 
  	
  	for (j=0;j<sizeY;j++)
  	{
  			scan = fscanf(pFile, "%s", str);
  			if (scan <= 0) break;
		  	for(i=0;i<sizeX;i++)
		  	{
		  		if(str[i] != '#' && str[i] != 'x') 
		  		{
		  			nodeArray[k].posX = i;
		  			nodeArray[k].posY = j;
		  			nodeArray[k].node = str[i];
		  			valid_positions++;
		  			k++;
		  		}
		  		else if (str[i] == '#')
		  		{
		  			valid_positions++;	  			
		  		}
		  	}
  	}
  
	fclose (pFile);
  
//////////////////////////////// Section 2 - Calculates the resultant vector between each position and the nodes
  
 	const int const_valid_positions = valid_positions;
 	
 	float mat[const_valid_positions][N_NODES+2];
 	
 	int Xaux, 
 		Yaux, 
 		m = 0;

 	float res_vector, 
 		res_time;		//resultant vector, resultant time
 	

 	pFile = fopen(argv[1], "r");
 	
 	fscanf (pFile, "%s", size);
 	sizeX = atoi(size);
 	fscanf (pFile, "%s", size);
 	sizeY = atoi(size);
  
  	for (j=0;j<sizeY;j++){
	  	scan = fscanf(pFile, "%s", str);
	  	if (scan <= 0) break;
	  	for(i=0;i<sizeX;i++) { 
	  			if(str[i] != 'x') {
	  				for(k=0;k<N_NODES;k++) {
	  					Xaux = i - nodeArray[k].posX;			// Calculates distance between node and position
	  					Yaux = j - nodeArray[k].posY;
	  					res_vector = sqrt(pow(Xaux,2) + pow(Yaux,2));	// Calculates the resultant vector
	  					res_time = res_vector/0.34029;			// Time = Distance/velocity
	  					mat[m][k] = res_time;				// Stores resultant time in the matrix
	  				}
	  				mat[m][N_NODES] = i;
	  				mat[m][N_NODES+1] = j;
	  				m++;
	  			}
	  	}
  	}
  	
  	fclose (pFile);

//////////////////////////////// Section 3 - Data Analysis
	
	int aval = 0,
		tval = 0,
		rval = 0,
		amp = 0,
		time = 0,
		shooterX, 
		shooterY,
		nval = 0,
		avg_time;

	float input_time_array[N_NODES],
	 	time_array[N_NODES],
		avg_time_array = 0,
		error = 0,
		final_error = 10000,
		full_input_time_array[N_NODES];

	struct _data_ data_cycle[N_NODES];
	
	FILE * qFile = fopen(argv[2], "r");

	for(k=0;k<N_NODES;k++){
		data_cycle[k].amp = 0;
		data_cycle[k].time = 0;
	}
	
	do {								//Do for the whole input file

		avg_time = 0;
		k = 0;
		error = 0;
		final_error = 10000;

		for(i=0;i<4*N_NODES;i++){   // Each node will print 4 times per cycle

			rval = fscanf(qFile, "%d: %d - %d\n", &nval, &aval, &tval);	// Node: amplitude - time

			//printf("%d ~ %d: %d - %d\n", rval, nval, aval, tval);

			if(rval == 3){				// if it was read correctly
			
				if(nval == 2) aval = aval/2.33;	// little adjustment (calibration)  **VERIFY THIS**
				
				if(aval > data_cycle[nval].amp){		// if the read amplitude is higher than the previous one
					data_cycle[nval].amp = aval;
				}

				if(k < 3){	
					avg_time = avg_time + tval;
					k++;
				}
				else if(k >= 3){
					avg_time = avg_time + tval;
					avg_time = avg_time/4;
					data_cycle[nval].time = avg_time;
					avg_time = 0;
					k = 0;
				}
			}
			//else
			//{
				//printf("**Fatal Error: Data Error**\n");
				//exit(0);

			//}
		}

//////////////////////////////// Section 4 - Seeking for the node more next to shooter

		int min_time = 10000,
			node;

		for(i=0;i<N_NODES;i++){
			if(data_cycle[i].time < min_time)
				node = i;
				min_time = data_cycle[i].time;
		}
	
//////////////////////////////// Section 5 - Shooter Localization


		for(i=0;i<const_valid_positions;i++) {
			for(j=0;j<N_NODES;j++) {
				time_array[j] = mat[i][j] - data_cycle[j].time;
				avg_time_array = avg_time_array + time_array[j];
			}
			
			avg_time_array = avg_time_array/N_NODES;
			
			for(k=0;k<N_NODES;k++)
			{
				full_input_time_array[k] = data_cycle[k].time + avg_time_array;
			}
			
			for(k=0;k<N_NODES;k++)
			{
				error = error + pow(full_input_time_array[k]-mat[i][k], 2);
			}
					
			if(final_error > error)
			{
				shooterX = mat[i][N_NODES];
				shooterY = mat[i][N_NODES+1];
				final_error = error;
			}
			
			avg_time_array = 0;
			error = 0;
			
		}
		
//////////////////////////////// Section 6 - Printing Data

		char strtime[40];

		strcpy(strtime, gettime());

		if (argc > 3) 
		{
			//sleep(1);
			pFile = fopen(argv[3], "a"); 

			if (data_cycle[nval].amp > 20) 
				fprintf(pFile, "node = %d, amplitude = %d, time = %s, shooterX = %d, shooterY = %d\n", 
						node, data_cycle[node].amp, strtime, shooterX, shooterY);
		}
		
		if (data_cycle[nval].amp > 20)
			printf("node = %d, amplitude = %d, time = %s, shooterX = %d, shooterY = %d\n", 
						node, data_cycle[node].amp, strtime, shooterX, shooterY); 		

		node = 0;
		data_cycle[nval].amp = 0;
		time = 0;
		aux_pos.posX = 0;
		aux_pos.posY = 0;

		if (argc >3) {
			fclose(pFile);
			//fclose(qFile);
		}

	} while(1);
}

//////////////////////////////// Function - Get actual clock time

char * gettime()
{
	struct timeval time2; //timeval type
	struct tm *time1;     //tm type
	char string[30] = {"\0"}, time_uString[6];
	long int time_u;      //microseconds
	time_t t = time(NULL);//calibrating time
	
	time1 = localtime(&t); //getting hours, minutes and seconds of actual time
	gettimeofday(&time2, NULL); //getting microseconds since 1970
	
	time_u = (long int) time2.tv_usec; // converting timeval struct to integer
	
	strftime(string, sizeof(string), "%T", time1); //converting time1 to a single string
	
	snprintf(time_uString, sizeof(time_uString), "%ld", time_u/1000);
	
	strcat(string, ".");
	strcat(string, time_uString);
	
	return string;
}

