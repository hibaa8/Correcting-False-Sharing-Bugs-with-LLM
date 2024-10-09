#include <vector>
#include <iostream>
#include <atomic>
#include <cassert>
#include <pthread.h>
#include <array>
#include <cstdlib> // for atoi

unsigned NUM_THREADS = 8;
unsigned REFCOUNT_BUMPS = 1 << 8;
unsigned FS_WRITES = 1 << 20;

typedef struct {
  int i;
  char pad[60];
#ifdef FIX_FS
  char pad[60];
#endif
} PaddedInt;

pthread_mutex_t ref_count_mutex;
unsigned long long ref_count;

typedef struct {
  int  *sp;
  PaddedInt ints[8] __attribute__ ((aligned (64)));
  std::array<std::array<int *, 1 << 8>, 8> sptrs;
} Globals;

Globals *G;

// Custom barrier implementation for macOS
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int num_threads;
} pthread_barrier_t;

int pthread_barrier_init(pthread_barrier_t *barrier, void *attr, unsigned count) {
    (void)attr; // unused
    if (count == 0) {
        return -1;
    }
    barrier->num_threads = count;
    barrier->count = 0;
    pthread_mutex_init(&barrier->mutex, nullptr);
    pthread_cond_init(&barrier->cond, nullptr);
    return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier) {
    pthread_mutex_lock(&barrier->mutex);
    barrier->count++;
    if (barrier->count >= barrier->num_threads) {
        barrier->count = 0;
        pthread_cond_broadcast(&barrier->cond);
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    } else {
        pthread_cond_wait(&barrier->cond, &barrier->mutex);
        pthread_mutex_unlock(&barrier->mutex);
        return 0;
    }
}

pthread_barrier_t b;

void *workerThread(void *tidptr) {
  unsigned tid = *(unsigned *)tidptr;

  pthread_barrier_wait(&b);

  for (unsigned i = 0; i < REFCOUNT_BUMPS; i++) {
#ifdef LOCKED
    pthread_mutex_lock(&ref_count_mutex);
#endif
    G->sptrs[tid][i] = G->sp; // increments G->sp's refcount via relaxed atomic
    ref_count += 1;
#ifdef LOCKED
    pthread_mutex_unlock(&ref_count_mutex);
#endif
    for (unsigned j = 0; j < FS_WRITES; j++) {
      G->ints[tid].i++;
    }
  }

  return nullptr;
}

int main(int argc, char *argv[]) {
  // If arguments are provided, use them.
  if (argc > 1) NUM_THREADS = std::atoi(argv[1]);
  if (argc > 2) REFCOUNT_BUMPS = std::atoi(argv[2]);
  if (argc > 3) FS_WRITES = std::atoi(argv[3]);

  G = (Globals *) malloc(sizeof(Globals));
  pthread_mutex_init(&ref_count_mutex, NULL);
  ref_count = 0;

  pthread_barrier_init(&b, NULL, NUM_THREADS);

  std::cout << "ints[0]:" << std::hex << (((long)&G->ints[0]) >> 6) << 
    " ints[1]:" << (((long)&G->ints[1]) >> 6) << 
    " ints[7]:" << (((long)&G->ints[7]) >> 6) << std::endl;

  G->sp = new int(42);

  std::vector<pthread_t> threads(NUM_THREADS);
  std::vector<unsigned> args(NUM_THREADS);

  for (unsigned i = 0; i < NUM_THREADS; i++) {
    args[i] = i;
    pthread_create(&threads[i], nullptr, workerThread, &args[i]);
  }

  for (auto &t : threads) {
    pthread_join(t, nullptr);
  }

  free(G);
  return 0;
}
