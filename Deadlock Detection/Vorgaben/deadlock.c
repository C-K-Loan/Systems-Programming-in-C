#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/syscall.h>

#include "resources.h"
#include "deadlock.h"
#include "print.h"

const char LABEL[] = "ABCD";

Matrix remove_Matrix_Zeile(Matrix a, int index, int a_size ){

  for(int i =index; i<a_size+2;i++){
    a.thread[i]=a.thread[i+1];
  }

    return a;

}

void update_Gesamtanforderung(unsigned t, unsigned r, unsigned a){
//addiere auf g_state G die anforderung
//verwende T als Zeile und R als Spalte der Wievielten Ressource. Das wird dann raufaddiert
  g_state.G.thread[t].resource[r]+=a;
  unsigned add=r+1;
  add++;
  } 

//   g_state.B.thread[t].resource[r]+=a;

//wenn was alloziert wurde, übergebe negatives a
//wenn was in die restanforderung gesteckt wird, positives a
void update_Restanforderung(unsigned t, unsigned r, unsigned a){
  g_state.R.thread[t].resource[r]+=a;
  unsigned aff=r+1;
  aff++;
  }


//etwas wird nur gefreet wenn ein Prozess beendet wurde
//ÜBergebe den Thread und subtrahiere jeden eintrag in frei von seinem BElegungsvektor
void update_Frei(unsigned t, unsigned r, unsigned a){
  g_state.f.resource[r]+=a;
  unsigned agg=r+t+1;
  agg++;
  }


void update_Belegung(unsigned t, unsigned r, unsigned a){
   g_state.B.thread[t].resource[r]+=a;
    unsigned att=r;
    att++;
  }   


//this funkction returns true if no element of vector a is negetiv, when i substract b from it  
bool Vektor_sub(Vector f, Vector e){
  Vector tmp;
  tmp = (Vector) {{0,0,0,0}};
  int a; 
  int b;
  int c;
  int d;

//wtf ist der syntax???
  //rest auch in ints speichern
  //void print_Vector(Vector *v, bool label);

tmp.resource [0]= f.resource[0]-e.resource[0];
tmp.resource [1]= f.resource[1]-e.resource[1];
tmp.resource [2]= f.resource[2]-e.resource[2];
tmp.resource [3]= f.resource[3]-e.resource[3];



//verleich  auf unsigned nicht möglich deswegen die Variablen ijn ints speichern
a=(int) tmp.resource[0];
b=(int) tmp.resource[1];
c=(int) tmp.resource[2];
d=(int) tmp.resource[3];

if(a<0 || b<0 || c<0 || d<0){
  return false ;
}else return true;
}

Vector vector_add(Vector f, Vector e){
  f.resource [0]= f.resource[0]+e.resource[0];
  f.resource [1]= f.resource[1]+e.resource[1];
  f.resource [2]= f.resource[2]+e.resource[2];
  f.resource [3]= f.resource[3]+e.resource[3];
return f;
}


void init_globals(){

  /* initialize resource state */
  //g state ist der speicher der dem sYSTEM zur verfügung steht 
  Matrix      tmp;
  tmp.thread[T1] = (Vector) {{1,1,1,2}};
  tmp.thread[T2] = (Vector) {{3,3,0,0}};
  tmp.thread[T3] = (Vector) {{3,0,0,0}};
  tmp.thread[T4] = (Vector) {{0,3,3,0}};
  g_state.G =   tmp;// die gesamtanvorderung ist eine Matrix
  g_state.v = (Vector) {{3,3,3,3}}; //vorhanden ist ein Vektor
  g_state.f = g_state.v;//frei ist ein Vektorx
  g_state.R = g_state.G;//restanforderung ist eine Matrix

  /* initialize mutexes/signals */
  for(unsigned r=FIRST_RESOURCE; r<NUM_RESOURCES; r++){
    if( pthread_cond_init (&(g_state.resource_released[r]), NULL) ){
      handle_error("cond_init");
    }
  }
  if( pthread_mutex_init(&(g_state.mutex), NULL) ){
    handle_error("mutex_init");
  }
  if( pthread_mutex_init(&g_cfd_mutex, NULL) ){
    handle_error("mutex_init");
  }   

  g_checkForDeadlocks = false;
  # if DETECTION
  g_checkForDeadlocks = true;
  # endif
}

bool isSafe(unsigned t, unsigned r, unsigned a){
  Vector f_tmp =  g_state.f; // temporerer frei Vector 
  Matrix r_tmp =  g_state.R;
  Matrix b_tmp =  g_state.B;

  bool answer = UNDEFINED;

  char out[57];
  sprintf(out, "[%d] T%u: is \"allocate(%c, %u)\" safe?",
      gettid(), t+1, LABEL[r], a);

if ( g_state.f.resource[r]< a) answer= UNSAFE;
  else answer = SAFE; 


# if AVOIDANCE
 /* if(answer == UNSAFE){
  //int erledige_Anfrage;  
    printf("Okay wir haben ein Problem, wir sind Unsafe (Ein thread muss schlafen), aber so könnte man die ressourcen Verteilen ohne einen Deadlock zu haben : \n");
        if(Vektor_sub(f_tmp,r_tmp.thread[0]) ||Vektor_sub(f_tmp,b_tmp.thread[1])|| Vektor_sub(f_tmp,b_tmp.thread[2]) || Vektor_sub(f_tmp,b_tmp.thread[3])  ){
        printf("Eine der 4 Threads Anfragen ist erfüllbar\n");
        if(Vektor_sub(f_tmp,r_tmp.thread[0])==true) {
          printf("Der 1. ist erfüllbar, entferne ihn!\n");
          f_tmp=vector_add(f_tmp,b_tmp.thread[0]);
          r_tmp=remove_Matrix_Zeile(r_tmp,0,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,0,3);
        } else if (Vektor_sub(f_tmp,r_tmp.thread[1])== true) {
          printf("Der 2. ist erfüllbar, entferne ihn!\n");
          f_tmp=vector_add(f_tmp,b_tmp.thread[1]);
          r_tmp=remove_Matrix_Zeile(r_tmp,1,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,1,3);
          } else if (Vektor_sub(f_tmp,r_tmp.thread[2])==true){
          printf("Der 3. ist erfüllbar, entferne ihn!\n");  
         f_tmp=vector_add(f_tmp,b_tmp.thread[2]);
          r_tmp=remove_Matrix_Zeile(r_tmp,2,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,2,3);
          } else if (Vektor_sub(f_tmp,r_tmp.thread[3])==true){
          printf("Der 4. ist erfüllbar, entferne ihn!\n");        
         f_tmp=vector_add(f_tmp,b_tmp.thread[3]);
          r_tmp=remove_Matrix_Zeile(r_tmp,3,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,3,3);

          }

          //für 3 threads
      if(Vektor_sub(f_tmp,r_tmp.thread[0]) ||Vektor_sub(f_tmp,b_tmp.thread[1])|| Vektor_sub(f_tmp,b_tmp.thread[2])){
        printf("Eine der 3 Threads Anfragen ist erfüllbar\n");
         if(Vektor_sub(f_tmp,r_tmp.thread[0])==true) {
          printf("Der 1. ist erfüllbar, entferne ihn!\n");
          f_tmp=vector_add(f_tmp,b_tmp.thread[0]);
          r_tmp=remove_Matrix_Zeile(r_tmp,0,2); 
          b_tmp=remove_Matrix_Zeile(b_tmp,0,2);
        } else if (Vektor_sub(f_tmp,r_tmp.thread[1])== true) {
          printf("Der 2. ist erfüllbar, entferne ihn!\n");          
          f_tmp=vector_add(f_tmp,b_tmp.thread[1]);
          r_tmp=remove_Matrix_Zeile(r_tmp,1,2); 
          b_tmp=remove_Matrix_Zeile(b_tmp,1,2);
          } else if (Vektor_sub(f_tmp,r_tmp.thread[2])==true){
          printf("Der 3. ist erfüllbar, entferne ihn!\n");        
          f_tmp=vector_add(f_tmp,b_tmp.thread[2]);
           r_tmp=remove_Matrix_Zeile(r_tmp,2,2); 
          b_tmp=remove_Matrix_Zeile(b_tmp,2,2);
          }


          //für 2 threads 
      if(Vektor_sub(f_tmp,r_tmp.thread[0]) ||Vektor_sub(f_tmp,b_tmp.thread[1])){
        printf("Eine der 2 Threads Anfragen ist erfüllbar\n");       
        if(Vektor_sub(f_tmp,r_tmp.thread[0])==true) {
          printf("Der 1. ist erfüllbar, entferne ihn!\n");                
          f_tmp=vector_add(f_tmp,b_tmp.thread[0]);
          r_tmp=remove_Matrix_Zeile(r_tmp,0,1); 
          b_tmp=remove_Matrix_Zeile(b_tmp,0,1);
        } else if (Vektor_sub(f_tmp,r_tmp.thread[1])== true) {  
          printf("Der 2. ist erfüllbar, entferne ihn!\n");                      
          f_tmp=vector_add(f_tmp,b_tmp.thread[1]);
          r_tmp=remove_Matrix_Zeile(r_tmp,1,1); 
          b_tmp=remove_Matrix_Zeile(b_tmp,1,1);

        }
          //für 1 thread
      if (Vektor_sub(f_tmp,b_tmp.thread[0])){
        printf("In dieser Reihenfolge wird kein Deadlock entstehen!\n");
       answer = SAFE;} 
      
      }
      }    


    }



  }*/


# endif
  g_state.f  = f_tmp;
  g_state.R  = r_tmp;
  g_state.B  = b_tmp;


  char tmp[60];
  sprintf(tmp, "%s : %s\n", out, answer? "yes" : "no" );
  printc(tmp, t);
  
  #ifdef DEBUG
  print_State();
  #endif



  return answer;
}

bool isDeadlocked(unsigned t){
  printf("in Dedlock function\n");
  Vector f_tmp =  g_state.f; // temporerer frei Vector 
  Matrix r_tmp =  g_state.R;
  Matrix b_tmp =  g_state.B;

int i = 0;
  bool answer = UNDEFINED;
  char out[57];
  sprintf(out, "[%d] T%u: Deadlock detected?", gettid(), t+1);
//Matrix remove_Matrix_Zeile(Matrix a, int index, int a_size ){

# if DETECTION
// TODO:      implement Deadxlock detection
 // while(1==1){
    //für 4 threads

    if(Vektor_sub(f_tmp,r_tmp.thread[0]) ||Vektor_sub(f_tmp,b_tmp.thread[1])|| Vektor_sub(f_tmp,b_tmp.thread[2]) || Vektor_sub(f_tmp,b_tmp.thread[3])  ){
        printf("Eine der 4 Threads Anfragen ist erfüllbar\n");
        if(Vektor_sub(f_tmp,r_tmp.thread[0])==true) {
          printf("Der 1. ist erfüllbar, entferne ihn!\n");
          f_tmp=vector_add(f_tmp,b_tmp.thread[0]);
          r_tmp=remove_Matrix_Zeile(r_tmp,0,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,0,3);
        } else if (Vektor_sub(f_tmp,r_tmp.thread[1])== true) {
          printf("Der 2. ist erfüllbar, entferne ihn!\n");
          f_tmp=vector_add(f_tmp,b_tmp.thread[1]);
          r_tmp=remove_Matrix_Zeile(r_tmp,1,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,1,3);
          } else if (Vektor_sub(f_tmp,r_tmp.thread[2])==true){
          printf("Der 3. ist erfüllbar, entferne ihn!\n");  
         f_tmp=vector_add(f_tmp,b_tmp.thread[2]);
          r_tmp=remove_Matrix_Zeile(r_tmp,2,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,2,3);
          } else if (Vektor_sub(f_tmp,r_tmp.thread[3])==true){
          printf("Der 4. ist erfüllbar, entferne ihn!\n");        
         f_tmp=vector_add(f_tmp,b_tmp.thread[3]);
          r_tmp=remove_Matrix_Zeile(r_tmp,3,3); 
          b_tmp=remove_Matrix_Zeile(b_tmp,3,3);

          }

          //für 3 threads
      if(Vektor_sub(f_tmp,r_tmp.thread[0]) ||Vektor_sub(f_tmp,b_tmp.thread[1])|| Vektor_sub(f_tmp,b_tmp.thread[2])){
        printf("Eine der 3 Threads Anfragen ist erfüllbar\n");
         if(Vektor_sub(f_tmp,r_tmp.thread[0])==true) {
          printf("Der 1. ist erfüllbar, entferne ihn!\n");
          f_tmp=vector_add(f_tmp,b_tmp.thread[0]);
          r_tmp=remove_Matrix_Zeile(r_tmp,0,2); 
          b_tmp=remove_Matrix_Zeile(b_tmp,0,2);
        } else if (Vektor_sub(f_tmp,r_tmp.thread[1])== true) {
          printf("Der 2. ist erfüllbar, entferne ihn!\n");          
          f_tmp=vector_add(f_tmp,b_tmp.thread[1]);
          r_tmp=remove_Matrix_Zeile(r_tmp,1,2); 
          b_tmp=remove_Matrix_Zeile(b_tmp,1,2);
          } else if (Vektor_sub(f_tmp,r_tmp.thread[2])==true){
          printf("Der 3. ist erfüllbar, entferne ihn!\n");        
          f_tmp=vector_add(f_tmp,b_tmp.thread[2]);
           r_tmp=remove_Matrix_Zeile(r_tmp,2,2); 
          b_tmp=remove_Matrix_Zeile(b_tmp,2,2);
          }


          //für 2 threads 
      if(Vektor_sub(f_tmp,r_tmp.thread[0]) ||Vektor_sub(f_tmp,b_tmp.thread[1])){
        printf("Eine der 2 Threads Anfragen ist erfüllbar\n");       
        if(Vektor_sub(f_tmp,r_tmp.thread[0])==true) {
          printf("Der 1. ist erfüllbar, entferne ihn!\n");                
          f_tmp=vector_add(f_tmp,b_tmp.thread[0]);
          r_tmp=remove_Matrix_Zeile(r_tmp,0,1); 
          b_tmp=remove_Matrix_Zeile(b_tmp,0,1);
        } else if (Vektor_sub(f_tmp,r_tmp.thread[1])== true) {  
          printf("Der 2. ist erfüllbar, entferne ihn!\n");                      
          f_tmp=vector_add(f_tmp,b_tmp.thread[1]);
          r_tmp=remove_Matrix_Zeile(r_tmp,1,1); 
          b_tmp=remove_Matrix_Zeile(b_tmp,1,1);

        }
          //für 1 thread
      if (Vektor_sub(f_tmp,b_tmp.thread[0])){
        i = 10;
        printf("Answer wird jetzt Save \n");
       answer = SAFE;} 
      
      }
      }    


    }

    if(i !=10){
      printf("Answer was UNDEFINED, so its UNSAFE now \n");
     answer = UNSAFE;}

 // } 
# endif
  
  char tmp[60];
  sprintf(tmp, "%s : %s\n", out, answer? "no" : "yes" );
  printc(tmp, t);
  
  #ifdef DEBUG
  print_State();
  #endif
printf("YO ich bin fertig, answer ist irgendwas\n");

  return answer? false : true;
}

void lock_state(unsigned t){
  printd("about to lock state");
  pthread_mutex_lock(&(g_state.mutex));
  printd("state locked");
}

void unlock_state(unsigned t){
  printd("state unlocked");
  pthread_mutex_unlock(&(g_state.mutex));
}


//t ist der Thread der Speicher haben will
//R ist welche ressource gefordert wrid 
//A ist wieviel von der ressource gefordet wird 
void allocate_r(unsigned t, unsigned r, unsigned a){

  char tmp[50]; 
  struct timespec ts;

  lock_state(t);//spert den Thread T

  //iwas mit zeit 
  clock_gettime(CLOCK_REALTIME, &ts);
  ts.tv_sec += 2;
  int alreadyWaited = 0;

  /* wait if request wasn't granted */
  while( (isSafe(t, r, a) == UNSAFE) ){
    sprintf(tmp, "[%d] T%u: waiting to allocate(%c, %u)\n",
        gettid(), t+1, LABEL[r], a);
    printc(tmp, t);
    
    /* first wait is a timed wait */
    if( !alreadyWaited )
      alreadyWaited = pthread_cond_timedwait(&(g_state.resource_released[r]),
        &(g_state.mutex), &ts);
    else
      pthread_cond_wait(&(g_state.resource_released[r]),
        &(g_state.mutex));
  }
  


//t ist der Thread der Speicher haben will
//R ist welche ressource gefordert wrid 
//A ist wieviel von der ressource gefordet wird 
  /* TODO: update resource state */
  update_Gesamtanforderung(t,r,a);
  update_Frei(t,r,-a);
  update_Belegung(t,r,a);
  update_Restanforderung(t,r,-a);

  printd("%u unit(s) of resource %c allocated", a, LABEL[r]);
  
  sprintf(tmp, "[%d] T%u: allocate(%c, %u)\n", gettid(), t+1,
      LABEL[r], a);
  printc(tmp, t);
  
  #ifdef DEBUG
  print_State();
  #endif

  unlock_state(t);
}

void release_r(unsigned t, unsigned r, unsigned a){

  char tmp[30];
  printd("[%d] T%u: about to release(%c, %u)\n",
      gettid(), t+1, LABEL[r], a);

  lock_state(t);

  update_Belegung(t,r,-a);
  update_Frei(t,r,a);



  printd("%u unit(s) of resource %c released", a, LABEL[r]);

  pthread_cond_signal(&(g_state.resource_released[r]));

  sprintf(tmp, "[%d] T%u: release(%c, %u)\n", getpid(), t+1,
      LABEL[r], a);
  printc(tmp, t);
  
  #ifdef DEBUG
  print_State();
  #endif

  unlock_state(t);
}

void *thread_work(void *thread_number){

  long t = (long)thread_number;
  char tmp[20];

  sprintf(tmp, "[%u] T%ld: started\n", gettid(), t+1);
  printc(tmp, t);
  switch(t) {
    case T1:
      usleep(10000);
      allocate_r(t, A, 1);
      allocate_r(t, D, 2);
      usleep(10000);
      release_r(t, A, 1);
      allocate_r(t, B, 1);
      usleep(10000);
      allocate_r(t, C, 1);
      usleep(50000);
      release_r(t, B, 1);
      usleep(10000);
      release_r(t, C, 1);
      release_r(t, D, 2);
      break;
    case T2:
      usleep(10000);
      allocate_r(t, A, 2);
      allocate_r(t, B, 3);
      usleep(10000);
      allocate_r(t, A, 1);
      usleep(50000);
      release_r(t, A, 3);
      usleep(10000);
      release_r(t, B, 3);          
      break;
    case T3:
      usleep(10000);
      allocate_r(t, A, 3);
      usleep(50000);
      release_r(t, A, 3);
      break;
    case T4:
      usleep(10000);
      allocate_r(t, B, 2);
      allocate_r(t, C, 3);
      usleep(10000);
      allocate_r(t, B, 1);
      usleep(50000);
      release_r(t, B, 3);
      usleep(10000);
      release_r(t, C, 3);
      break;
    case NUM_THREADS:
      /* DL-WatchDog */
      /* poll resource state to check for deadlocks */
      pthread_mutex_lock(&g_cfd_mutex);
      while( g_checkForDeadlocks ){
        pthread_mutex_unlock(&g_cfd_mutex);
        usleep(1000000);
        if( isDeadlocked(t) ){
          char tmp[35];
          sprintf(tmp, "[%d] T%ld: Deadlock detected!\n",
              gettid(), t+1);
          printc(tmp, t);
          pthread_exit((void*)EXIT_FAILURE);
        }
        pthread_mutex_lock(&g_cfd_mutex);
      }
      pthread_mutex_unlock(&g_cfd_mutex);
      break;
    default:
      printf("unexpected!");
      exit(EXIT_FAILURE);
      break;
  }

  pthread_exit(EXIT_SUCCESS);
}

int main(){

  init_globals();

  printf("Total resources are v:\n");
  print_Vector(&(g_state.v), true);
  printf("\n");
  printf("Currently available resources are f:\n");
  print_Vector(&(g_state.f), true);
  printf("\n");
  printf("Maximum needed resources are G:\n");
  print_Matrix(&(g_state.G), true);
  printf("\n");
  printf("Currently allocated resources are B:\n");
  print_Matrix(&(g_state.B), true);
  printf("\n");
  printf("Still needed resources are R:\n");
  print_Matrix(&(g_state.R), true);
  printf("\n");

  fflush(stdout);
  setbuf(stdout, NULL);

  /* spawn threads */
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_t thread[NUM_THREADS+1] = { 0 };
  for(long t=FIRST_THREAD; t<=NUM_THREADS; t++){
    if( pthread_create(&thread[t], &attr, thread_work, (void *)t) ){
      handle_error("create");
    }
  }
  pthread_attr_destroy(&attr);

  /* wait for threads */ 
  void *status;
  unsigned finished = 0;
  for(unsigned t=FIRST_THREAD; t<=NUM_THREADS; t++){
    if( pthread_join(thread[t], &status) ){
      handle_error("join");
    }
    if( status ){
      printf("\n[%d] T%u exited abnormally. Status: %ld\n",
          gettid(), t+1, (long)status);
    } else {
      printf("\n[%d] T%u exited normally.\n", gettid(), t+1);
    }    
    if( t<NUM_THREADS ) finished++;
    /* tell DL-WatchDog to quit*/
    if(finished==NUM_THREADS){
      pthread_mutex_lock(&g_cfd_mutex);
      g_checkForDeadlocks = false;
      pthread_mutex_unlock(&g_cfd_mutex);
    }  
  }

  /* Clean-up */
  if( pthread_mutex_destroy(&g_cfd_mutex) ){
    handle_error("mutex_destroy");
  } 
  if( pthread_mutex_destroy(&(g_state.mutex)) ){
      handle_error("mutex_destroy");
  }
  for(unsigned r=FIRST_RESOURCE; r<NUM_RESOURCES; r++){
    if( pthread_cond_destroy(&(g_state.resource_released[r])) ){
        handle_error("cond_destroy");
    }
  }

  printf("Main thread exited normally.\nFinal resource state:\n");

  print_State();

  exit(EXIT_SUCCESS);
}
  