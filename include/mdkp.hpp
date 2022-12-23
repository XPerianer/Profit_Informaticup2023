#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <ratio>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "fields/distance_map.hpp"
#include "fields/occupancy_map.hpp"
#include "io/parsing.hpp"
#include "landscape.hpp"
#include "placeable.hpp"
#include "product.hpp"
#include "resource_type.hpp"
#include "subtype.hpp"

namespace profit {

using ObjectId = int16_t;
using DepositId = int16_t;
using ConnectedComponent = std::vector<DepositId>;

// Type used for storing how many products we want to fabricate. Since products have integer
// requirements on their resources, we can only produce as many products as we have resources
// stored. We assume that product requirements are non-negative.
using ProductCount = StoredResourceCount;

using AvailableResources = std::array<StoredResourceCount, SUBTYPE_COUNT>;

[[nodiscard]] inline std::vector<ProductCount> pech(AvailableResources resources,
                                                    const std::vector<Product>& products) {
  std::vector<ProductCount> result(products.size());

  while (true) {
    ProductScore max_product_score = 0;
    const Product* best_product = nullptr;

    for (const Product& product : products) {
      ProductScore max_score_for_current_product = std::numeric_limits<ProductScore>::max();

      for (ResourceType resource : RESOURCE_TYPES) {
        if (product.requirements[resource] == 0) {
          continue;
        }
        // TODO(Richard): Nochmal wegen Static Casts überlegen -- können wir ResourceRequirements
        // generalisieren?
        auto producable = static_cast<ProductCount>(resources[static_cast<size_t>(resource)] /
                                                    product.requirements[resource]);
        ProductScore limitation_by_current_resource = producable * product.points;
        max_score_for_current_product =
            std::min(max_score_for_current_product, limitation_by_current_resource);
      }

      if (max_score_for_current_product > max_product_score) {
        best_product = &product;
        max_product_score = max_score_for_current_product;
      }
    }

    if (max_product_score == 0) {
      return result;
    }

    size_t best_product_index = best_product - products.data();

    auto maximum_producable_product_count =
        static_cast<ProductCount>(max_product_score / best_product->points);

    constexpr auto ALPHA_INVERSE = 20;
    ProductCount product_count = std::max(1, maximum_producable_product_count / ALPHA_INVERSE);

    result[best_product_index] += product_count;

    for (ResourceType resource : RESOURCE_TYPES) {
      resources[static_cast<size_t>(resource)] -=
          static_cast<StoredResourceCount>(best_product->requirements[resource] * product_count);
    }
  }
}

inline void drive(const parsing::Input& input, const std::vector<ConnectedComponent>& components) {
  std::vector<StoredResourceCount> remaining_resources_by_deposit(input.deposits.size());
  std::ranges::transform(input.deposits, remaining_resources_by_deposit.begin(),
                         initial_resource_count);

  std::vector<std::array<StoredResourceCount, SUBTYPE_COUNT>> remaining_resources_by_component(
      components.size());

  for (const ConnectedComponent& component : components) {
    size_t component_id = &component - components.data();

    for (const DepositId& deposit_id : component) {
      const Deposit deposit = input.deposits[deposit_id];
      remaining_resources_by_component[component_id][static_cast<size_t>(deposit.type)] +=
          initial_resource_count(deposit);
    }
  }

  // TODO: Bitmagic?
  std::vector<Product> products_that_can_be_tried = input.products;

  while (true) {
    // Eine Sache: Evtl haben Produkte keine überlagernden Ressourcen
    //  -> Dann werden Dinge einfacher, beachten

    for (const ConnectedComponent& component : components) {
      size_t component_id = &component - components.data();

      std::vector<ProductCount> to_realize =
          pech(remaining_resources_by_component[component_id], products_that_can_be_tried);

      if (std::accumulate(to_realize.begin(), to_realize.end(), 0) == 0) {
        break;  // TODO(DomJ): Nochmal ganz stark nachdenken.
      }

      // Vereinfachung: Alle Ressourcen in einen Topf schmeißen. Hieraus entscheiden, welches
      // Produkt wir als nächstes produzieren wollen, und wie oft Dann versuchen zu realisieren.
      // Das klappt potenziell nicht, dann überspringen, weiter machen mit nächstem Produkt

      // Problem: Sobald Pipeline gebaut draint sie die gesamten Ressources des Deposits
      // -> Daher der Algorithmus: Immer Produkt nehmen, das bei komplett-Verwendung als nächstes
      // den Score maximiert
      //
      // Damit sind Folgeiterationen nicht mehr "Gegeben diese verfügbaren Ressourcen, was sollte
      // ich bauen?" sondern "Wem kann ich was wegnehmen, um den Score zu verbessern".
      //

      // realize(/* Todo */);
    }

    // ausgeben, was als nächstes realisiert werden soll
    // je nachdem was ähm
    // ob das funktioniert muss man halt verschieden darauf reagieren.
  }
  // irgendwas
}

}  // namespace profit
