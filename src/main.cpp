#include <algorithm>
#include <iostream>
#include <variant>
#include <vector>

#include "landscape.hpp"
#include "parsing.hpp"
#include "placeable.hpp"
#include "serialization.hpp"
#include "two_dimensional_vector.hpp"
#include "utils.hpp"

int main() {
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

std::vector<Deposit> get_deposits(const parsing::Input &input) {
  std::vector<Deposit> deposits;
  for (auto const &object : input.objects) {
    std::visit(overloaded{[&](const Deposit &deposit) { deposits.push_back(deposit); },
                          [](const Obstacle &obstacle) {}},
               object);
  }
  return deposits;
}

// Idee: Immer erstmal nur eine Produktionspipeline (-> genau eine Fabrik) bauen
// Mehrere Fabriken dienen nur der Latenzoptimierung
// Latenzoptimierung machen wir im zweiten Schritt
// im ersten Schritt machen wir nur Scoreoptimierung (weil Score trumpft Latenz)
// -> Damit: Latenzoptimierung am Ende einfach, indem wir mehr Fabriken mit dieser Methode bauen
// Auch: Die Methode versucht immer direkt kürzeste Wege zu bauen
//    Überlegung: Kürzester Weg könnte kritisches Feld belagern, was uns später nicht erlaubt,
//    weitere Pipelines zu bauen Evtl explizite Kreuzungen setzen?

bool attempt_realize(const std::vector<Deposit> &deposits, Factory::TypeT factory_type) {
  // minen platzieren
  // fabrik platzieren
  // verbinden
  return false;
}

void select_deposits_and_products(const parsing::Input &input) {
  // Simplest algorithm: Try all products one by one
  for (auto const &product : input.products) {
    auto const &requirements = product.requirements;
    std::vector<Deposit> useful_deposits;
    for (auto const &deposit : get_deposits(input)) {
      if (requirements[deposit.subtype] != 0) {
        useful_deposits.push_back(deposit);
      }
    }
    // Now try if we can make this work
    attempt_realize(useful_deposits, product.subtype);
  }
}

enum class FieldOccupancy {
  EMPTY,
  BLOCKED,
  CONVEYOR_CROSSING,
  INPUT,
  OUTPUT,
};

using OccupancyMap = TwoDimensionalVector<FieldOccupancy>;

OccupancyMap createOccupancyMap(const parsing::Input &input) {
  OccupancyMap occupancyMap(input.dimensions);
  for (const auto &object : input.objects) {
    std::visit(overloaded{[&](const Deposit &deposit) {
                            const auto right_x = deposit.handle.x() + deposit.dimensions.width();
                            const auto bottom_y = deposit.handle.y() + deposit.dimensions.height();
                            for (CoordT y = deposit.handle.y(); y < bottom_y; y++) {
                              for (CoordT x = deposit.handle.x(); x < right_x; x++) {
                                if (y == deposit.handle.y() || y == bottom_y - 1 ||
                                    x == deposit.handle.x() || x == right_x - 1) {
                                  occupancyMap[{x, y}] = FieldOccupancy::OUTPUT;
                                } else {
                                  occupancyMap[{x, y}] = FieldOccupancy::BLOCKED;
                                }
                              }
                            }
                          },
                          [&](const Obstacle &obstacle) {
                            const auto right_x = obstacle.handle.x() + obstacle.dimensions.width();
                            const auto bottom_y =
                                obstacle.handle.y() + obstacle.dimensions.height();
                            for (CoordT y = obstacle.handle.y(); y < bottom_y; y++) {
                              for (CoordT x = obstacle.handle.x(); x < right_x; x++) {
                                occupancyMap[{x, y}] = FieldOccupancy::BLOCKED;
                              }
                            }
                          }},
               object);
  }
  return occupancyMap;
}

using ReachabilityMap = TwoDimensionalVector<int8_t>;

constexpr auto rotations = {Rotation::DOWN_TO_UP, Rotation::LEFT_TO_RIGHT, Rotation::UP_TO_DOWN,
                            Rotation::RIGHT_TO_LEFT};

ReachabilityMap create_reachability_for_deposit(const Deposit &deposit,
                                                const OccupancyMap &occupancies,
                                                const Vec2 dimensions) {
  ReachabilityMap map(dimensions);
  // Invariante: Für Felder, die in der queue sind, gilt: Hier dürfen Inputs hin, für den
  // gewünschten
  std::queue<Vec2> reached_ingestion_fields;

  const auto right_x = deposit.handle.x() + deposit.dimensions.width();
  const auto bottom_y = deposit.handle.y() + deposit.dimensions.height();
  for (CoordT y = deposit.handle.y(); y < bottom_y; y++) {
    // Check left side
    for (auto rotation : rotations) {
      // Check if mine can be placed
      auto mine = Mine::with_ingress({static_cast<CoordT>(deposit.handle.x() - 1), y},
                                     static_cast<Rotation>(rotation));
    }
  }
}

bool collides(const Mine &mine, const OccupancyMap &occupancies) {}

std::optional<std::vector<PlaceableObject>> connect(Vec2 egress_start_field,
                                                    Vec2 ingress_target_field,
                                                    const OccupancyMap &occupancies) {
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

    // TODO: Ausprobieren
    // 3-unit conveyor
    // for(Rotation rot = 0; rot < 4; ++rot) {

    //
    // 4-unit conveyor
    // combinern
  }
}
}  // namespace production_realization
