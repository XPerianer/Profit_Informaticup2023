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
  Worker worker(best_solution, sync, input);
  worker.start();
  int return_code = 0;
  timespec time{};
  clock_gettime(CLOCK_REALTIME, &time);
  const int time_to_stop_threads_and_write_best_solution = 2;
  time.tv_sec += 10 - time_to_stop_threads_and_write_best_solution;
  while (sync.worker_threads_count != 0 && return_code == 0) {
    std::cerr << sync.worker_threads_count << " count\n";
    return_code = pthread_cond_timedwait(&sync.worker_thread_condition,
                                         &sync.worker_thread_condition_mutex, &time);
  }

#ifdef NDEBUG
  std::cout << serialization::serialize(best_solution.parts);
#else
  // Extented output for profit website
  serialization::Output output =
      serialization::Output{input.dimensions, input.turns,     input.time,         input.products,
                            input.deposits,   input.obstacles, best_solution.parts};
  std::cout << serialization::serialize_detailed(output);
#endif

  return 0;
}
/*
 * Überlegungen / Ideen:
 * Zerlegung in Teilprobleme: Tricky, weil nach Zerlegung potenziell nicht mehr rechteckig
 *   -> Vorschlag Richard: Input-Dimensionen beibehalten, und alle nicht-erreichbaren Felder einfach
 *       als Obstacle. Dann haben wir quasi 2 Teilprobleme, jeweils mit großen Teilen nur obstacle,
 * die könnten einfacher zu lösen sein. Gesamtlösung sollte einfach Merge/Addition der Teillösungen
 * sein
 *
 * Ein früher Schritt: Welche Produkte aus welchen Deposit
 *   * -> als "Vorschlag", unklar ob realisierbar
 *   * als Heuristik sinnvoll benutzbar: Max-Flow
 *
 *    Max Flow etwas Ticky: Wenn pairwise bestimmung notwendig: Haben potenziell 10k*10k = 100M
 * Paare
 *       * (für besser als 0 score: Feld braucht aber noch mind 1 Factory + 1 Mine)
 *
 * Für jetzt:
 *   - Angenommen, wir haben einen Estimator, der uns gibt, welche Deposits zu welchem Produkt
 * kombiniert werden sollten
 *   -> Brauchen Kompontente, die versucht, das zu realisieren
 *       -> gegeben N deposits, Designe Pipelines und Factory, um die Produktion zu realisieren
 */

namespace production_realization {

// Idee: Immer erstmal nur eine Produktionspipeline (-> genau eine Fabrik) bauen
// Mehrere Fabriken dienen nur der Latenzoptimierung
// Latenzoptimierung machen wir im zweiten Schritt
// im ersten Schritt machen wir nur Scoreoptimierung (weil Score trumpft Latenz)
// -> Damit: Latenzoptimierung am Ende einfach, indem wir mehr Fabriken mit dieser Methode bauen
// Auch: Die Methode versucht immer direkt kürzeste Wege zu bauen
//    Überlegung: Kürzester Weg könnte kritisches Feld belagern, was uns später nicht erlaubt,
//    weitere Pipelines zu bauen Evtl explizite Kreuzungen setzen?

}  // namespace production_realization
