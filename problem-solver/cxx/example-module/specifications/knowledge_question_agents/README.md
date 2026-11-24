# Knowledge question agents

This directory contains the SC classes used by the four C++ agents registered in
`problem-solver/cxx/example-module/exampleModule.cpp`.

## Action arguments

Arguments are read positionally with `ScAction::GetArguments<N>()`:

| Action class | Argument 1 | Argument 2 |
| --- | --- | --- |
| `action_for_concept_search` | SC-link with the search text | — |
| `action_for_definition_answer` | concept node | — |
| `action_for_classification_answer` | parent concept node | — |
| `action_for_relation_explanation` | start concept node | end concept node |

## Result structures

- Concept search returns a result structure containing a set of matched concepts.
  Each match is annotated with one of `concept_match_by_main_idtf`,
  `concept_match_by_idtf`, or `concept_match_by_definition`. A search with no
  matches still finishes successfully and returns an empty set.
- Definition answer returns the requested concept and existing SC elements for
  its definition, notes, and incoming `nrel_inclusion` relation (parent class).
- Classification answer returns a set of directly included concepts and the
  existing inclusion relation elements used to find them.
- Relation explanation performs breadth-first search over directed common arcs,
  considering both outgoing and incoming directions. The result contains the
  path nodes, original connectors, their numeric order, and explicit start/end
  annotations. If no path is present in the knowledge base, the action finishes
  with an error; this means that the available data is insufficient to explain
  the relation, not that the concepts are known to be unrelated.

The implementation reuses existing knowledge-base connectors in its results;
it does not copy definition or inclusion relations into new semantic facts.
