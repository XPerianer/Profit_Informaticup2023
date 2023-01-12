#include <algorithm>
#include <iostream>
#include <variant>
#include <vector>

#include "connect.hpp"
#include "field_state.hpp"
#include "fields/distance_map.hpp"
#include "fields/field.hpp"
#include "fields/occupancy_map.hpp"
#include "geometry/rectangle.hpp"
#include "io/parsing.hpp"
#include "io/serialization.hpp"
#include "landscape.hpp"
#include "mdkp.hpp"
#include "placeable.hpp"
#include "rotation.hpp"
#include "utils.hpp"

// NOLINTBEGIN(google-build-using-namespace): In main, we allow using these
using namespace profit;
using namespace geometry;
// NOLINTEND(google-build-using-namespace)

// For now, we allow exception escape (causing std::terminate -> error shown)
int main() {  // NOLINT(bugprone-exception-escape)
  parsing::Input input = parsing::parse(std::cin);

  OccupancyMap occupancy_map = occupancies_from(input);
  auto deposits = input.deposits;
  ConnectedComponentsWrapper components_wrapper(static_cast<DepositId>(deposits.size()),
                                                occupancy_map.dimensions());
  for (size_t i = 0; i < deposits.size(); i++) {
    distances_from(deposits[i], occupancy_map, &components_wrapper, static_cast<DepositId>(i));
  }

  std::vector<ConnectedComponent> connected_components = components_wrapper.extract();
  std::vector<DistanceMap> distance_maps;
  distance_maps.reserve(deposits.size());
  FieldState state = {occupancy_map, {}, {}};

  for (size_t i = 0; i < deposits.size(); i++) {
    distance_maps.emplace_back(
        distances_from(deposits[i], occupancy_map, &components_wrapper, static_cast<DepositId>(i)));
  }

  DistanceMap merged = merge(distance_maps, Factory::DIMENSIONS);

  for (const auto &component : connected_components) {
    std::cerr << "Starting with first component\n";
    AvailableResources resources = available_resources(component, input);
    std::vector<ProductCount> fabrication_plan = pech(resources, input.products);
    std::cerr << " ----- Fabrication plan -----\n";
    for (auto product : fabrication_plan) {
      std::cerr << product << "\n";
    }
    std::cerr << " ----- Fabrication plan -----\n";
    // try to realize one by one
    // TODO: optimize by building product with higher scores first see #24
    // TODO: optimize by building the correct proportions see #25
    for (unsigned int i = 0; i < fabrication_plan.size(); i++) {
      auto product = input.products[i];
      auto count = fabrication_plan[i];
      std::cerr << "Starting with product " << i << ", count is " << count << "\n";
      if (count == 0) {
        continue;
      }
      std::cerr << "Trying to place factory\n";
      auto factory_id = place_factory(input.products[i].type, merged, &state);
      if (!factory_id) {
        continue;
      }
      std::cerr << "Placed factory\n";
      for (auto resource_type : RESOURCE_TYPES) {
        if (product.requirements[resource_type] == 0) {
          continue;
        }
        for (auto deposit_id : component) {
          auto deposit = input.deposits[deposit_id];
          if (deposit.type != resource_type) {
            continue;
          }
          auto pipeline_id = connect(deposit, *factory_id, &state);
          if (!pipeline_id) {
            continue;
          }
        }
      }
    }
  }

  std::vector<PlaceableObject> result = state.placed_objects();

#ifdef NDEBUG
  std::cout << serialization::serialize(result);
#else
  // Extented output for profit website
  serialization::Output output =
      serialization::Output{input.dimensions, input.turns,     input.time, input.products,
                            input.deposits,   input.obstacles, result};
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
