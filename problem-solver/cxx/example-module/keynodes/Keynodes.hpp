/*
 * This source file is part of an OSTIS project. For the latest info, see
 * http://ostis.net Distributed under the MIT License (See accompanying file
 * COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_addr.hpp>
#include <sc-memory/sc_keynodes.hpp>

class Keynodes : public ScKeynodes
{
public:
  static inline ScKeynode const action_search_subdividing{"action_search_subdividing", ScType::ConstNodeClass};
  static inline ScKeynode const nrel_subdividing{"nrel_subdividing", ScType::ConstNodeNonRole};
  static inline ScKeynode const action_search_isomorphic_structures{
      "action_search_isomorphic_structures",
      ScType::ConstNodeClass};
  static inline ScKeynode const nrel_search_result{"nrel_search_result", ScType::ConstNodeNonRole};
  static inline ScKeynode const empty_set{"empty_set", ScType::ConstNodeClass};
  static inline ScKeynode const action_search_minimum_path{"action_search_minimum_path", ScType::ConstNodeClass};
  static inline ScKeynode const action_example_inference{"action_example_inference", ScType::ConstNodeClass};

  static inline ScKeynode const action_for_concept_search{"action_for_concept_search", ScType::ConstNodeClass};
  static inline ScKeynode const action_for_definition_answer{"action_for_definition_answer", ScType::ConstNodeClass};
  static inline ScKeynode const action_for_classification_answer{"action_for_classification_answer", ScType::ConstNodeClass};
  static inline ScKeynode const action_for_relation_explanation{"action_for_relation_explanation", ScType::ConstNodeClass};

  static inline ScKeynode const nrel_main_idtf{"nrel_main_idtf", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_idtf{"nrel_idtf", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_definition{"nrel_definition", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_note{"nrel_note", ScType::ConstNodeNonRole};
  static inline ScKeynode const nrel_inclusion{"nrel_inclusion", ScType::ConstNodeNonRole};

  static inline ScKeynode const concept_match_by_main_idtf{"concept_match_by_main_idtf", ScType::ConstNodeClass};
  static inline ScKeynode const concept_match_by_idtf{"concept_match_by_idtf", ScType::ConstNodeClass};
  static inline ScKeynode const concept_match_by_definition{"concept_match_by_definition", ScType::ConstNodeClass};
};
