void *thread_work(void *thread_number){

  long t = (long)thread_number;
  char tmp[20];

  sprintf(tmp, "[%u] T%ld: started\n", gettid(), t+1);
  printc(tmp, t);
  switch(t) {
  // tmp.thread[T1] = (Vector) {{1,1,1,2}};
  // tmp.thread[T2] = (Vector) {{3,3,0,0}};
  // tmp.thread[T3] = (Vector) {{3,0,0,0}};
  // tmp.thread[T4] = (Vector) {{0,3,3,0}};


/*Die idee hinter dem veränderten Thread ist, das wir die 4 Anforderungsmatrizen sequentiell abarbeiten.
  Das heißt also, das sobald alle Threads erstellt sind,
    derjenige, der für T1 zuständig ist fängt sofort an
    derjenige, der für T2 zuständig ist, fängt nach 50000 Zeiteinheiten an
    derjenige, der für T3 zuständig ist, fängt nach 100000 Zeiteinheiten an
    derjenige, der für T4 zuständig ist, fängt nach 150000 Zeiteinheiten an

    Somit bekommt jeder Thread seine Ressouren und gibt diese zurück, bevor überhaupt ein anderer
    nach Ressourcen fragt

*/
    case T1:
      allocate_r(T1, A, 1);
      allocate_r(T1, B, 1);
      allocate_r(T1, C, 1);
      allocate_r(T1, D, 2);
      release_r(T1, A, 1);
      release_r(T1, B, 1);
      release_r(T1, C, 1);
      release_r(T1, D, 2);
      break;
    case T2:
      usleep(50000);
      allocate_r(T2, A, 3);
      allocate_r(T2, B, 3);
      allocate_r(T2, C, 0);
      allocate_r(T2, D, 0);
      release_r(T2, A, 3);
      release_r(T2, B, 3);
      release_r(T2, C, 0);
      release_r(T2, D, 0);
      break;
    case T3:
      usleep(100000);
      allocate_r(T3, A, 3);
      allocate_r(T3, B, 0);
      allocate_r(T3, C, 0);
      allocate_r(T3, D, 0);
      release_r(T3, A, 3);
      release_r(T3, B, 0);
      release_r(T3, C, 0);
      release_r(T3, D, 0);
      break;
    case T4:
      usleep(150000);
      allocate_r(T4, A, 0);
      allocate_r(T4, B, 3);
      allocate_r(T4, C, 3);
      allocate_r(T4, D, 0);
      release_r(T4, A, 0);
      release_r(T4, B, 3);
      release_r(T4, C, 3);
      release_r(T4, D, 0);
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