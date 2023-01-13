#pragma once
#include <pthread.h>
#include <unistd.h>

#include <concepts>
#include <utility>

#include "placeable.hpp"
#include "product.hpp"
#include "solver.hpp"

namespace profit {

struct Synchronization {
  Synchronization() {
    pthread_cond_init(&worker_thread_condition, NULL);
    pthread_mutex_init(&worker_thread_condition_mutex, NULL);
    pthread_mutex_init(&best_solution_mutex, NULL);
    pthread_mutex_init(&worker_threads_count_mutex, NULL);
  }
  int worker_threads_count = 0;
  pthread_cond_t worker_thread_condition;
  pthread_mutex_t worker_thread_condition_mutex;
  pthread_mutex_t best_solution_mutex;
  pthread_mutex_t worker_threads_count_mutex;

  Synchronization(const Synchronization& other) = delete;
  Synchronization& operator=(const Synchronization& other) = delete;
  Synchronization(Synchronization&& other) = delete;

  ~Synchronization() {
    pthread_cond_destroy(&worker_thread_condition);
    pthread_mutex_destroy(&worker_thread_condition_mutex);
    pthread_mutex_destroy(&best_solution_mutex);
    pthread_mutex_destroy(&worker_thread_condition_mutex);
  }
};

struct Worker {
  Worker(Solution& solution, Synchronization& sync, const parsing::Input& input)
      : solution_(solution), sync_(sync), input_(input) {}

  inline void run() {
    simple_greedy_solver(input_, [&](Solution solution) { update_solution(solution); });
    worker_thread_add(-1);
  }

  inline void worker_thread_add(int number) {
    pthread_mutex_lock(&sync_.worker_threads_count_mutex);
    sync_.worker_threads_count += number;
    pthread_mutex_unlock(&sync_.worker_threads_count_mutex);
    pthread_cond_signal(&sync_.worker_thread_condition);
    std::cerr << "cond_signal\n";
  }

  inline void update_solution(Solution solution) {
    std::cerr << "Update of solution\n";
    pthread_mutex_lock(&sync_.best_solution_mutex);
    if (solution.score > solution_.score) {
      solution_ = std::move(solution);
    }
    pthread_mutex_unlock(&sync_.best_solution_mutex);
  }

  inline int start() {
    worker_thread_add(1);
    return pthread_create(
        &thread_id_, nullptr,
        [](void* self) -> void* {
          static_cast<Worker*>(self)->run();
          return nullptr;
        },
        this);
  }

  inline void stop() const { pthread_cancel(thread_id_); }

 private:
  Solution& solution_;
  Synchronization& sync_;
  const parsing::Input input_;
  pthread_t thread_id_;
};

}  // namespace profit