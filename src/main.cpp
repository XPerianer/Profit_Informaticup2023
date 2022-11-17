#include <iostream>
#include <vector>

#include "parsing.hpp"
#include "placeable.hpp"
#include "two_dimensional_vector.hpp"
#include "utils.hpp"

int main() {
  parsing::Input input = parsing::parse(std::cin);
  (void)input;
}


/*
* Überlegungen / Ideen:
* Zerlegung in Teilprobleme: Tricky, weil nach Zerlegung potenziell nicht mehr rechteckig
*   -> Vorschlag Richard: Input-Dimensionen beibehalten, und alle nicht-erreichbaren Felder einfach
*       als Obstacle. Dann haben wir quasi 2 Teilprobleme, jeweils mit großen Teilen nur obstacle, die
*       könnten einfacher zu lösen sein. Gesamtlösung sollte einfach Merge/Addition der Teillösungen sein
*
* Ein früher Schritt: Welche Produkte aus welchen Deposit
*   * -> als "Vorschlag", unklar ob realisierbar
*   * als Heuristik sinnvoll benutzbar: Max-Flow
*
*    Max Flow etwas Ticky: Wenn pairwise bestimmung notwendig: Haben potenziell 10k*10k = 100M Paare
*       * (für besser als 0 score: Feld braucht aber noch mind 1 Factory + 1 Mine)
*
* Für jetzt:
*   - Angenommen, wir haben einen Estimator, der uns gibt, welche Deposits zu welchem Produkt kombiniert werden sollten
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
  //    Überlegung: Kürzester Weg könnte kritisches Feld belagern, was uns später nicht erlaubt, weitere Pipelines zu bauen
  //    Evtl explizite Kreuzungen setzen?

  bool attempt_realize(const std::vector<Deposit>& deposits, Factory::TypeT factory_type) {
    return false;
  }

  enum FieldOccupancy {
    EMPTY,
    BLOCKED,
    CONVEYOR_CROSSING,
    INPUT,
    OUTPUT,
  };

  using OccupancyMap = TwoDimensionalVector<FieldOccupancy>;

  std::optional<std::vector<PlaceableObject>> connect(Vec2 egress_start_field, Vec2 ingress_target_field, const OccupancyMap& occupancies) {
    // Breitensuche, von Start, immer mit allen 4 (Conveyor3) + 4 (Conveyor4) + 4 (Combiner) Bewegungsmöglichkeiten
    return std::nullopt;

    // Invariante: Für Felder, die in der queue sind, gilt: Hier dürfen Inputs hin, für den gewünschten 
    std::queue<Vec2> reached_ingestion_fields;

    // TODO: Alle benachbarten Felder von egress_start_field in die Queue pushen, wenn nicht auch mit anderem Egress verbunden
    // TODO auch: Vielleicht bei Verbindung zu anderem Egress warnen (wir erwarten nicht, dass das häufig passiert?)
    // (Problem ist: +|.|+|)
    // reached_ingestion_fields.push(start);


    while(!reached_ingestion_fields.empty()) {
      Vec2 reached_field = reached_ingestion_fields.front();
      reached_ingestion_fields.pop();

      // TODO: Ausprobieren
      // 3-unit conveyor
      for(Rotation rot = 0; rot < 4; ++rot) {
        
      }
      // 4-unit conveyor
      // combinern
    }
  }
}
