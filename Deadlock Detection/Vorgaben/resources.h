#ifndef INC_RESOURCES_H
#define INC_RESOURCES_H

#include <pthread.h>

/* set of resources */
enum {FIRST_RESOURCE, A=FIRST_RESOURCE, B, C, D, NUM_RESOURCES}; 
/* set of threads */
enum {FIRST_THREAD, T1=FIRST_THREAD, T2, T3, T4, NUM_THREADS};
/* states for deadlock algorithms */
enum {UNSAFE, SAFE, UNDEFINED};

/* row vector */
//eine Zeile in der Matrix?
//Zeilenvektor
typedef struct {
  unsigned resource[NUM_RESOURCES];
} Vector;


// Ein Array von Matrix Zeilen. 1 Element ist erste Zeile 
typedef struct {
  Vector thread[NUM_THREADS];
} Matrix;

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t resource_released[NUM_RESOURCES];
  Matrix G; /* Gesamtanforderung */
  Matrix B; /* Belegt - Allocation */
  Matrix R; /* Restanforderung - Need */
  Vector f; /* frei - Available */
  Vector v; /* insgesamt vorhanden */
} State;

/* globally accessible variables, mutexes and signals */
extern const char LABEL[]; /* used to label resources */
State g_state; /* used for keeping track of resource state */

#endif /* INC_RESOURCES_H */
