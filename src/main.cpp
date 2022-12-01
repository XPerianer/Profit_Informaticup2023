#include <algorithm>
#include <iostream>
#include <variant>
#include <vector>

#include "field.hpp"
#include "geometry/rectangle.hpp"
#include "landscape.hpp"
#include "parsing.hpp"
#include "placeable.hpp"
#include "rotation.hpp"
#include "serialization.hpp"
#include "utils.hpp"

using namespace profit;
using namespace geometry;

// For now, we allow exception escape (causing std::terminate -> error shown)
int main() {  // NOLINT(bugprone-exception-escape)
  parsing::Input input = parsing::parse(std::cin);

  // TODO: lots of calculations with input
  std::vector<PlaceableObject> result = {};

#ifdef NDEBUG
  std::cout << serialization::serialize(result);
#else
  // Extented output for profit website
  serialization::Output output = serialization::Output{input.dimensions, input.turns,   input.time,
                                                       input.products,   input.objects, result};
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

#include <queue>

namespace production_realization {

// Idee: Immer erstmal nur eine Produktionspipeline (-> genau eine Fabrik) bauen
// Mehrere Fabriken dienen nur der Latenzoptimierung
// Latenzoptimierung machen wir im zweiten Schritt
// im ersten Schritt machen wir nur Scoreoptimierung (weil Score trumpft Latenz)
// -> Damit: Latenzoptimierung am Ende einfach, indem wir mehr Fabriken mit dieser Methode bauen
// Auch: Die Methode versucht immer direkt kürzeste Wege zu bauen
//    Überlegung: Kürzester Weg könnte kritisches Feld belagern, was uns später nicht erlaubt,
//    weitere Pipelines zu bauen Evtl explizite Kreuzungen setzen?

inline bool attempt_realize(const std::vector<Deposit>& /*deposits*/,
                            FactoryType /*factory_type*/) {
  // minen platzieren
  // fabrik platzieren
  // verbinden
  return false;
}

inline void select_deposits_and_products(const parsing::Input& input) {
  // Simplest algorithm: Try all products one by one
  for (const auto& product : input.products) {
    const auto& requirements = product.requirements;
    const std::vector<Deposit> input_deposits = get_deposits(input);

    std::vector<Deposit> useful_deposits;
    std::copy_if(input_deposits.begin(), input_deposits.end(), std::back_inserter(useful_deposits),
                 [&](const Deposit& deposit) { return requirements[deposit.type] != 0; });

    // Now try if we can make this work
    attempt_realize(useful_deposits, product.type);
  }
}

enum class CellOccupancy {
  EMPTY,
  BLOCKED,
  CONVEYOR_CROSSING,
  INPUT,
  OUTPUT,
};

using OccupancyMap = Field<CellOccupancy, CellOccupancy::BLOCKED, CellOccupancy::EMPTY>;

inline OccupancyMap createOccupancyMap(const parsing::Input& input) {
  OccupancyMap occupancyMap(input.dimensions);
  for (const auto& object : input.objects) {
    std::visit(utils::overloaded{[&](const Deposit& deposit) {
                                   const auto rect = as_rectangle(deposit);
                                   for (Vec2 coordinate : rect) {
                                     if (is_on_border(rect, coordinate)) {
                                       occupancyMap.set(coordinate, CellOccupancy::OUTPUT);
                                     } else {
                                       occupancyMap.set(coordinate, CellOccupancy::BLOCKED);
                                     }
                                   }
                                 },
                                 [&](const Obstacle& obstacle) {
                                   const auto rect = as_rectangle(obstacle);
                                   for (Vec2 coordinate : rect) {
                                     occupancyMap.set(coordinate, CellOccupancy::BLOCKED);
                                   }
                                 }},
               object);
  }
  return occupancyMap;
}

inline bool collides(const Mine& /*mine*/, const OccupancyMap& /*occupancies*/) {
  // TODO
  return false;
}

using DistanceT = int16_t;
constexpr DistanceT NOT_REACHABLE = -1;

using DistanceMap = Field<DistanceT, NOT_REACHABLE, NOT_REACHABLE>;

inline DistanceMap distances_from(const Deposit& deposit, const OccupancyMap& occupancies) {
  DistanceMap distances(occupancies.dimensions());

  // Invariante: Für Felder, die in der queue sind, gilt: Dieses Feld haben wir erreicht, in
  // (Feld-Wert) Schritten, und hier dürfen Inputs für Conveyors/Combiners hin
  std::queue<Vec2> reached_ingestion_fields;

  const auto deposit_rect = as_rectangle(deposit);

  for (Vec2 egress : left_border(deposit_rect)) {
    // Check left side
    for (auto rotation : rotations) {
      // Check if mine can be placed
      auto mine = Mine::with_ingress(egress - Vec2{1, 0}, static_cast<Rotation>(rotation));
      if (collides(mine, occupancies)) {
        continue;
      }
    }
  }

  // TODO

  return distances;
}

inline std::optional<std::vector<PlaceableObject>> connect(Vec2 /*egress_start_field*/,
                                                           Vec2 /*ingress_target_field*/,
                                                           const OccupancyMap& /*occupancies*/) {
  // Breitensuche, von Start, immer mit allen 4 (Conveyor3) + 4 (Conveyor4) + 4 (Combiner)
  // Bewegungsmöglichkeiten
  return std::nullopt;

  // Invariante: Für Felder, die in der queue sind, gilt: Hier dürfen Inputs hin, für den
  // gewünschten
  std::queue<Vec2> reached_ingestion_fields;

  // TODO: Alle benachbarten Felder von egress_start_field in die Queue pushen, wenn nicht auch mit
  // anderem Egress verbunden
  // TODO auch: Vielleicht bei Verbindung zu anderem Egress warnen (wir erwarten nicht, dass das
  // häufig passiert?) (Problem ist: +|.|+|) reached_ingestion_fields.push(start);

  while (!reached_ingestion_fields.empty()) {
    Vec2 reached_field = reached_ingestion_fields.front();
    reached_ingestion_fields.pop();

    (void)reached_field;

    // TODO: Ausprobieren
    // 3-unit conveyor
    // for(Rotation rot = 0; rot < 4; ++rot) {

    //
    // 4-unit conveyor
    // combinern
  }
}
}  // namespace production_realization
