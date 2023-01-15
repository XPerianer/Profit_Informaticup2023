#include <pthread.h>

#include <algorithm>
#include <ctime>
#include <iostream>
#include <variant>
#include <vector>

#include "io/parsing.hpp"
#include "io/serialization.hpp"
#include "solver.hpp"
#include "worker.hpp"

// NOLINTBEGIN(google-build-using-namespace): In main, we allow using these
using namespace profit;
using namespace geometry;
// NOLINTEND(google-build-using-namespace)

// For now, we allow exception escape (causing std::terminate -> error shown)
int main() {  // NOLINT(bugprone-exception-escape)
  parsing::Input input = parsing::parse(std::cin);

  Solution best_solution;
  Synchronization sync;

  pthread_mutex_lock(&sync.worker_thread_condition_mutex);
  Worker worker(&best_solution, &sync, input);
  worker.start();
  int return_code = 0;
  timespec time{};
  clock_gettime(CLOCK_REALTIME, &time);
  const int time_to_stop_threads_and_write_best_solution = 2;
  time.tv_sec += input.time - time_to_stop_threads_and_write_best_solution;
  while (sync.worker_threads_count != 0 && return_code == 0) {
    DEBUG(sync.worker_threads_count << " count\n");
    return_code = pthread_cond_timedwait(&sync.worker_thread_condition,
                                         &sync.worker_thread_condition_mutex, &time);
  }

  pthread_mutex_lock(&sync.best_solution_mutex);

#ifdef NDEBUG
  std::cout << serialization::serialize(best_solution.parts);
#else
  // Extented output for profit website
  serialization::Output output =
      serialization::Output{input.dimensions, input.turns,     input.time,         input.products,
                            input.deposits,   input.obstacles, best_solution.parts};
  std::cout << serialization::serialize_detailed(output);
#endif
  pthread_mutex_unlock(&sync.best_solution_mutex);

  return 0;
}
