#ifndef HWS_ONTOLOGY_H
#define HWS_ONTOLOGY_H

#include <array>
#include <string>

#include "hello_worlds/BasicTypes/Shape.h"
#include "ontologenius/OntologiesManipulator.h"

namespace hws {

  namespace ontology {

    std::array<double, 4> getEntityColor(onto::OntologyManipulator* onto, const std::string& indiv_name, const std::array<double, 4>& default_value = {0.8, 0.8, 0.8, 0.});

    Shape_t getEntityShape(onto::OntologyManipulator* onto, const std::string& indiv_name);

    double getEntityMass(onto::OntologyManipulator* onto, const std::string& indiv_name);

    void addColor(onto::OntologyManipulator* onto, const std::string& color_name, const std::string& rgb_value = "");

    void addColorToEntity(onto::OntologyManipulator* onto, const std::string& indiv_name, const std::string& color_name);

  } // namespace ontology

} // namespace hws

#endif // HWS_ONTOLOGY_H