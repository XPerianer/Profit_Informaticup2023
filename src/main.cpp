#include <algorithm>
#include <iostream>
#include <variant>
#include <vector>

#include "fields/field.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/rectangle.hpp"
#include "io/parsing.hpp"
#include "io/serialization.hpp"
#include "landscape.hpp"
#include "placeable.hpp"
#include "rotation.hpp"
#include "utils.hpp"

// NOLINTBEGIN(google-build-using-namespace): In main, we allow using these
using namespace profit;
using namespace geometry;
// NOLINTEND(google-build-using-namespace)

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

using DistanceT = int16_t;
constexpr DistanceT NOT_REACHABLE = -1;

using DistanceMap = Field<DistanceT, NOT_REACHABLE, NOT_REACHABLE>;

/* Returns an approximation */
inline DistanceMap distances_from(const Deposit& deposit, const OccupancyMap& occupancies) {
  DistanceMap distances(occupancies.dimensions());

  // Invariante: Für Felder, die in der queue sind, gilt: Dieses Feld haben wir erreicht, in
  // (Feld-Wert) Schritten, und hier dürfen Inputs für Conveyors/Combiners hin
  std::queue<Vec2> reached_ingress_fields;

  auto cell_reachable = [&](const Vec2 cell, DistanceT distance) {
    if (distances.at(cell) == NOT_REACHABLE) {
      reached_ingress_fields.emplace(cell);
      distances.set(cell, distance);
    }
  };

  auto place = [&](auto placeable, DistanceT distance) -> void {
    if (!collides<>(placeable, occupancies)) {
      for (const Vec2 downstream_ingress_cell : placeable.downstream_ingress_cells()) {
        if (occupancies.at(downstream_ingress_cell) != CellOccupancy::EMPTY) {
          continue;
        }
        if (any_neighbor_is(occupancies, downstream_ingress_cell, CellOccupancy::EGRESS)) {
          continue;
        }

        cell_reachable(downstream_ingress_cell, distance);
      }
    }
  };

  for (Vec2 possible_ingress_location : outer_connected_border_cells(as_rectangle(deposit))) {
    for (auto rotation : ROTATIONS) {
      auto mine = Mine::with_ingress(possible_ingress_location, rotation);
      place(mine, 1);
    }
  }

  // TODO
  // Conveyor * 2 längen * 4 rotationen
  // Combiner *  2 positionierungen (links, rechts die äußersten eingänge) * 4 rotationen

  // Für eine Cell in downstream_ingress_cell:
  // Für Conveyor 3
  // Gehe alle Rotationen durch
  // Schaue, ob collided
  // Wenn nicht collided, schreibe in distance map und füge neighbor_ingresses in die queue
  while (!reached_ingress_fields.empty()) {
    Vec2 reached_ingress = reached_ingress_fields.front();
    reached_ingress_fields.pop();

    for (auto rotation : ROTATIONS) {
      // 3 conveyor
      auto conveyor_3 = Conveyor3::with_ingress(reached_ingress, rotation);
      place(conveyor_3, static_cast<DistanceT>(distances.at(reached_ingress) + 1));

      // 4 conveyor
      auto conveyor_4 = Conveyor4::with_ingress(reached_ingress, rotation);
      place(conveyor_4, static_cast<DistanceT>(distances.at(reached_ingress) + 1));

      // Combiner
      auto possible_combiners = {Combiner::with_left_ingress(reached_ingress, rotation),
                                 Combiner::with_right_ingress(reached_ingress, rotation)};
      for (auto possible_combiner : possible_combiners) {
        if (std::ranges::any_of(possible_combiner.ingresses(), [&](const Vec2 ingress) {
              return any_neighbor_is(occupancies, ingress, CellOccupancy::EGRESS);
            })) {
          continue;
        }
        place(possible_combiner, static_cast<DistanceT>(distances.at(reached_ingress) + 1));
      }
    };
    // check if fields of other ingresses are free
  }

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

using namespace production_realization;

// For now, we allow exception escape (causing std::terminate -> error shown)
int main() {  // NOLINT(bugprone-exception-escape)
  parsing::Input input = parsing::parse(std::cin);

  // TODO: lots of calculations with input
  std::vector<PlaceableObject> result = {};
  OccupancyMap occupancy_map = create_from(input);

  for (const auto& product : input.products) {
    const auto& requirements = product.requirements;
    const std::vector<Deposit> input_deposits = get_deposits(input);

    std::vector<Deposit> useful_deposits;
    std::copy_if(input_deposits.begin(), input_deposits.end(), std::back_inserter(useful_deposits),
                 [&](const Deposit& deposit) { return requirements[deposit.type] != 0; });

    // Now try if we can make this work
    for (auto useful_deposit : useful_deposits) {
      DistanceMap distances = distances_from(useful_deposit, occupancy_map);
      for (Coordinate y = 0; y < input.dimensions.y(); ++y) {
        for (Coordinate x = 0; x < input.dimensions.x(); ++x) {
          int current = static_cast<int>(distances.at({x, y}));
          if (current > 0 && current < 10) {
            std::cout << "0" << current;
          } else if (current < 0) {
            std::cout << "xx";
          } else {
            std::cout << current;
          }
        }
        std::cout << std::endl;
      }
      std::cout << std::endl;
      std::cout << std::endl;
    }
  }

  /*for (Coordinate y = 0; y < input.dimensions.y(); ++y) {
    for (Coordinate x = 0; x < input.dimensions.x(); ++x) {
      std::cout << static_cast<int>(occupancy_map.at({x, y}));
    }
    std::cout << std::endl;
  }*/

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
