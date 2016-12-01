#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_OVERFLOW  -666
#define STACK_UNDERFLOW -235

typedef int /* your type */ data_t;

typedef struct {
  int size; // счетчик и вершина стека
 	data_t *data;
} stack_t;

stack_t stack = {};
static int maxsize = 0;

data_t top(stack_t* stack) { // возвращает элемент с вершины стека
  if(stack->size <= 0) {
    exit(STACK_UNDERFLOW);
  }
return stack->data[stack->size-1];
}

void stack_construct(stack_t* stack, int maxs) {
	maxsize = maxs;
	assert(stack); 
	stack->data = (data_t*) calloc(maxsize, sizeof(stack->data[0])); 
	if(!(stack->data)) {
    fprintf(stderr, "Ошибка при выделении памяти\n");
    exit(EXIT_FAILURE);
  }
	stack->size = 0;
return;
}
  
void stack_destruct(stack_t* stack) {
	free(stack->data); 
	stack->data = NULL;
	stack->size = -666;
  return;
}
/* Функция изменения размера */
void resize(stack_t *stack) {
  maxsize *= 2;
  stack->data = realloc(stack->data, maxsize * sizeof(data_t));
  if(stack->data == NULL) {
    exit(STACK_OVERFLOW);
  }
return;
}

/* Занесение элемента в стек. */
void push(stack_t* stack, data_t val) { // подаем стек и val(ue) - то, что кладется в стек
  if(stack->size >= maxsize) {
    resize(stack);
  }
	stack->data[stack->size++] = val;
return;
}

/* Получение верхнего элемента из стека. */
data_t pop(stack_t *stack) {
  if(stack->size == 0) {
    exit(STACK_UNDERFLOW);
  }
  stack->size--;
return stack->data[stack->size];
}

int is_empty(stack_t* stack) { // возвращает 1, если стек пуст, и 0 - в противном случае
  if(stack->size == 0) 
    return 1;
  if(stack->size != 0) 
    return 0;
}

// проверка
int graph[5][5] = 
{
  {0, 1, 1, 0, 0},
  {0, 0, 0, 1, 1},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0}
};

int main(void){
  int i;
  data_t dir;
  stack_t ps; 
  stack_construct(&ps, 100);
  push(&ps, 0);
  while((is_empty(&ps)) == 0) {
    dir = top(&ps);
    printf("%d\n", dir + 1);
    pop(&ps);
    for(i = 5; i >= 0; i--)
      if(graph[dir][i] == 1)
        push(&ps, i);
  }
 
  stack_destruct(&ps);
return 0;
}
