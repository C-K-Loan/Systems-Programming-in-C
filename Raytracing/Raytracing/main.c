#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "bitmap.h"
#include "color.h"
#include "raytrace.h"
#include "util.h"

int raytracer_simple(const char* filename){

	printf("%s      :  ", filename);//name der datei geprintet
	unsigned long start, end;		

	// init time measurement
	start = current_time_millis();

	// init raytracer
	vec_t bounds[4];		
	scene_t *scene = create_scene();
	calculate_casting_bounds(scene->cam, bounds);

	// Allocate buffer for picture data
	// macht für für jeden pixel den wird verweden werden speicher frei 
	pix_t *img = (pix_t*) calloc(HEIGHT * WIDTH, sizeof(pix_t)); 
	if (img){

		// calculate the data for the image (do the actual raytrace)
		// Erster Parameter ist Pointer auf freien Speicherblock mit HEIGHT*WIDTH  viel Speicher. Also für jeden Pixel
		//Zweiter Paramtetereter ist ein vec_T array der size of 4 hat
		//Dritter Parameter ist eine szene 
		//Vierter Paramteter x offset
		//Fünfter Paramteter y offset
		//Sechster Parameter die breite des zu berechnenden bereiches
		//Siebter Parameter die höhe des zu ebrecchnenden bereiches
		//berechnet von x offset bis width 
		//berechnet von y offset bis Height
		raytrace(img, bounds, scene, 0, 0, WIDTH, HEIGHT);

		delete_scene(scene);

		// open file
		FILE *file = fopen(filename, "wb");
		if (file) {
			// write the header
			//1. Parameter, wohin geschrieben wird
			// 2 und 3 Parameter höhe und breite des bitmap
			write_bitmap_header(file, WIDTH, HEIGHT);

			// write image to file on disk
			//fwrite schreibt Daten in den Pointer File hinein (4. Parameter)
			//Die Informationen die gespeichert werden sollen, befinden sich im 1. Parameter, also img
			//2. Parameter gibt an wie Groß ein DatenElement ist, in bytes. Also 3 Bytes pro element
			// 3 . Paramteter, also WIDTH*HEIGHT gibt an wieviele DatenElemente gespeichert werden
			fwrite(img, 3, WIDTH * HEIGHT, file);

			// free buffer
			//mache den allozierten speicher wieder frei von jedem pixel
			free(img);
			
			// close file
			fclose(file);
			
			// print the measured time
			end = current_time_millis();
			printf("Render time: %.3fs\n", (double) (end - start) / 1000);
			
			return EXIT_SUCCESS;
		}
	}
	return EXIT_FAILURE;
}



  //With the parameters frame_x, frame_y, frame_width and frame_height it's
  //possible to just calculate a smaller part of the whole scene.

//tested for 1,2,4,5,7
//not working 3,6,8,9,10,11,12
int raytracer_loop(const char* filename, int processcount){

	printf("%s (%i)    :  ", filename, processcount);
	unsigned long start, end;

	// init time measurement
	start = current_time_millis();

	// init raytracer
	vec_t bounds[4];
	scene_t *scene = create_scene();	
	calculate_casting_bounds(scene->cam, bounds);



	int y_offset= 0;
	int i = processcount;
	int n = 0;
	FILE *file = fopen(filename, "wb");
	write_bitmap_header(file, WIDTH, HEIGHT);
	pix_t * speicherBlocke[processcount];//array mir allen speicherblöcken
	int x= HEIGHT/processcount;
	printf("%d X IST \n",x );

	//alloziere speicher für  jede Spalte 
	while(i!= 0){
		//calloc sollte WIDTH*X sein, grad steht hier was falsches for funky glitch fun
	speicherBlocke[n]=(pix_t*)calloc(WIDTH*x,sizeof(pix_t));
	i--;
	n++;
	}

	n=0;
	i = processcount;
	
	while(i != 0 ){
		printf("Durchlauf NR. %d\n",n );
		printf("%d IST OFFSET Y\n",y_offset );

		raytrace(speicherBlocke[n], bounds, scene, 0, y_offset,WIDTH, x);

		n++;
		//y_offset= (HEIGHT*n)/processcount ; 
		y_offset=y_offset+x;

		i --;

	} 	 
	i=processcount;
	n= 0;

	//writing  segments
	while(i!=0){
		//height/count  *widht 
	fwrite(speicherBlocke[n], 3,(x*WIDTH), file);
	n++;	
	i--;
	}

	i=processcount;
	n= 0;

	//freeing allocated  mem for each segment 
	while(i!=0){
	free(speicherBlocke[n]);
	n++;
	i--;	
	}
	fclose(file);


	end = current_time_millis();
	printf("Render time: %.3fs\n", (double) (end - start) / 1000);
	
	//return EXIT_SUCCESS;
	return EXIT_FAILURE;

}

int raytracer_parallel(const char* filename, int processcount){

	printf("%s (%i):  ", filename, processcount);
	unsigned long start, end;
	
	// init time measurement
	start = current_time_millis();

	// init raytracer
	vec_t bounds[4];
	scene_t *scene = create_scene();
	calculate_casting_bounds(scene->cam, bounds);

	int y_offset= 0;
	int i = processcount;
	int n = 0;
	FILE *file1 = fopen(filename, "wb");
	fclose(file1);
	pix_t * speicherBlocke[processcount];//array mir allen speicherblöcken
	int x= HEIGHT/processcount;
	printf("%d X IST \n",x );

	//alloziere speicher für  jede Spalte 
	while(i!= 0){
		speicherBlocke[n]=(pix_t*)calloc(WIDTH*x,sizeof(pix_t));
	i--;
	n++;
	}
	

	n=0;
	i = processcount;
	int Pid;
	int y;

	//Mache forks
	for(y=1;y<processcount;y++) {
	Pid=fork();
	if(Pid==0){
		break;}	

	}
	FILE *file = fopen(filename, "rb+");
	write_bitmap_header(file, WIDTH, HEIGHT);


	//berechne bild
	y_offset=x*(y-1);
	printf("Hallo, ich bin Nr %d, mein Offset ist %d\n",y,y_offset );	
	raytrace(speicherBlocke[y-1], bounds, scene, 0, y_offset,WIDTH, x);

	n=0;
		
	//write to IMG
	//fwrite(speicherBlocke[y-1], 3,x*WIDTH, file);

	printf("YO ICH bin nr %d und schreibe jetzt\n",y);	

	wait(NULL);	
			//fseek(file,(3*WIDTH*h*i)+2,0);

	fseek (file,3*(WIDTH*x*(y-1))+3,1);
	fwrite(speicherBlocke[y-1], 3,(x*WIDTH), file);
	
	fclose(file);		

	end = current_time_millis();
	printf("Render time: %.3fs\n", (double) (end - start) / 1000);
	
	//return EXIT_SUCCESS;
	return EXIT_FAILURE;
}

int main(int argc, char** argv) {

	if (argc != 2){
		printf("Usage: raytracer PROCESSCOUNT\n");
		return EXIT_FAILURE;
	}
												//zweiter parameter um zu beschreiben wo speichern
	unsigned int processcount = strtol(argv[1], NULL, 10);//string to long  mit basis 10


	 raytracer_parallel("image-parallel.bmp", processcount);

//	raytracer_loop("image-loop.bmp", processcount);
	// 	if (raytracer_simple("image-simple.bmp") != EXIT_SUCCESS){
	// 		printf("Error or not implemented.\n\n");
	// 	}
	
	// if (raytrearacer_loop("image-loop.bmp", processcount) != EXIT_SUCCESS){
	// 	printf("Error or not implemented.\n\n");
	// }

	// if (raytracer_parallel("image-parallel.bmp", processcount) != EXIT_SUCCESS){
	// 	printf("Error or not implemented.\n\n");
	// }


	return 0;
}
