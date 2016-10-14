/* $ time ./matrix 1000 1
Number of threads forced to 1

real	0m18.380s
user	0m18.372s
sys	0m0.028s

$ time ./matrix 1000 2
Number of threads forced to 2

real	0m9.433s
user	0m18.592s
sys	0m0.032s

$ time ./matrix 1000 4
Number of threads forced to 4

real	0m10.271s
user	0m19.608s
sys	0m0.036s 

$ time ./matrix 1000 4
Number of threads forced to 4

real	0m19.889s
user	0m21.144s
sys	0m1.172s */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

typedef struct {
  int size; // dimension of matrix
  int *a, *b, *c; // a, b - original matrix, c - result matrix
} matrix_multiply_t;

/* Разделение: каждый тред "берет на себя" строку матрицу, соответствующую номеру треда, и последующие строки, соответсвующие номер треда + общее количество тредов */
typedef struct {
  int threadnum; // number of the thread
  int numofthread; //number of threads
  matrix_multiply_t* multiply;
} order_t;

int* generate_empty_matrix(int size) {
  int* matrix = (int*) malloc(sizeof(int) * size * size);

  return matrix;
}

int* generate_matrix(int size) {
  int* matrix = generate_empty_matrix(size);
  if(matrix == NULL) {
    return NULL;
  }

  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      *(matrix + i*size + j) = rand() % 10;
    }
  }

  return matrix;
}

/* We get matrix's element k(i,j) */
int multi_scalar(matrix_multiply_t *matrix, int i, int j) { 
  int size = matrix->size; 
  int result = 0;
  for(int c = 0; c < size; ++c) {
    result += *(matrix->a + i*size + c) * *(matrix->b + c*size + j);
  }
    
  return result;
}

void* mythread(void* arg) {
  order_t* order = (order_t*) arg;
  int size = order->multiply->size;
  
  for(int i = order->threadnum; i < size; i += order->numofthread) {
    for(int j = 0; j < size; ++j) {
      *(order->multiply->c + i*size + j) = multi_scalar(order->multiply, i, j);
    }
  }
  
  return NULL;
}

void print_matrix(int* matrix, int size) {
  printf("\n");
  for(int i = 0; i < size; ++i) {
    for(int j = 0; j < size; ++j) {
      printf(" %d ", *(matrix + i*size + j));
    }
    printf("\n");
  }
}

int main(int argc, char** argv) {
  /* Check for correct number of arguments */
  if(argc > 3 || argc < 2) {
    fprintf(stderr, "Usage: %s N [threadnum]\n", argv[0]);
    exit(EXIT_FAILURE); // Exit with error
  }

  int size = atoi(argv[1]);
  if(size < 1) {
    fprintf(stderr, "Incorrect matrix size\n");
    exit(EXIT_FAILURE);
  }

  /* Choose number of threads */
  int numofthread;
  if(argc == 3) {
    // Use provided number of threads manually
    numofthread = atoi(argv[2]);
    if(numofthread < 1) {
      fprintf(stderr, "Incorrect number of threads!\n");
      exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Number of threads we are using: %d\n", numofthread);
  } else {
    /* If user doesn't provide number of threads we will use number of 
    processors */
    numofthread = sysconf(_SC_NPROCESSORS_ONLN);
    if(numofthread < 1) {
      fprintf(stderr, "Unable to detect number of processors\n Use 2 threads");
      numofthread = 2;
    } else {
      fprintf(stderr, "Run in %d threads\n", numofthread);
    }
  }

  matrix_multiply_t multiply;
  multiply.size = size;
  multiply.a = generate_matrix(size);
  if(multiply.a == NULL) {
    perror("Unable to generate matrix a");
    exit(EXIT_FAILURE);
  }
  multiply.b = generate_matrix(size);
  if(multiply.b == NULL) {
    perror("Unable to generate matrix b");
    exit(EXIT_FAILURE);
  }  
  multiply.c = generate_empty_matrix(size);
  if(multiply.c == NULL) {
    perror("Unable to allocate memory for c");
    exit(EXIT_FAILURE);
  }

  printf("Data generation done!\n");

  pthread_t *tid = (pthread_t*) malloc(sizeof(pthread_t) * numofthread);
  if(tid == NULL) {
    perror("Can't allocate memory for thread structures");
    exit(EXIT_FAILURE);
  }
  
  for(int i = 0; i < numofthread; ++i) {
    order_t* order = (order_t*) malloc(sizeof(order_t));
    if(order == NULL) {
      perror("Can't allocate memory for order");
      exit(EXIT_FAILURE);
    }
    
    matrix_multiply_t* localmultiply = malloc(sizeof(matrix_multiply_t));

    localmultiply->a = generate_empty_matrix(size); 
    memcpy(localmultiply->a, multiply.a, sizeof(int) * size * size);

    localmultiply->b = generate_empty_matrix(size); 
    memcpy(localmultiply->b, multiply.b, sizeof(int) * size * size);

    localmultiply->size = size;
    localmultiply->c = multiply.c;
    order->multiply = localmultiply;
    order->threadnum = i;
    order->numofthread = numofthread;

    int startstatus = pthread_create(&tid[i], NULL, mythread, order);
    if(startstatus != 0) {
      perror("Creation of thread failed");
      exit(EXIT_FAILURE);
    }
  }

  for(int i = 0; i < numofthread; ++i) {
    pthread_join(tid[i], NULL);
  }

// print_matrix(multiply.a, size);
// print_matrix(multiply.b, size); 
// print_matrix(multiply.c, size); 
  
  exit(EXIT_SUCCESS);
}
