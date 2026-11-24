/*
 * This source file is part of an OSTIS project.
 * Distributed under the MIT License.
 */

#include "KnowledgeAgents.hpp"

#include <algorithm>
#include <cctype>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

#include <sc-memory/sc_memory.hpp>

#include "keynodes/Keynodes.hpp"
#include "utils/NumberUtils.hpp"

namespace
{
std::string Normalize(std::string value)
{
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
  return value;
}

struct RelationValue
{
  ScAddr owner;
  ScAddr link;
  ScAddr connector;
  ScAddr relationArc;
};

std::vector<RelationValue> GetRelationValues(ScMemoryContext & context, ScAddr const & relation)
{
  std::vector<RelationValue> values;
  ScIterator5Ptr iterator = context.CreateIterator5(
      ScType::Node, ScType::ConstCommonArc, ScType::ConstNodeLink, ScType::ConstPermPosArc, relation);
  while (iterator->Next())
    values.push_back({iterator->Get(0), iterator->Get(2), iterator->Get(1), iterator->Get(3)});
  return values;
}

void AddRelationValues(ScMemoryContext & context, ScAddr const & conceptAddr, ScAddr const & relation,
                       ScStructure & result)
{
  ScIterator5Ptr iterator = context.CreateIterator5(
      conceptAddr, ScType::ConstCommonArc, ScType::ConstNodeLink, ScType::ConstPermPosArc, relation);
  while (iterator->Next())
  {
    result << iterator->Get(1) << iterator->Get(2) << iterator->Get(3);
  }
}

ScResult InvalidArguments(ScAction & action)
{
  return action.FinishWithError();
}
} // namespace

ScAddr ConceptSearchAgent::GetActionClass() const
{
  return Keynodes::action_for_concept_search;
}

ScResult ConceptSearchAgent::DoProgram(ScAction & action)
{
  auto const &[queryLink] = action.GetArguments<1>();
  if (!queryLink.IsValid())
    return InvalidArguments(action);

  std::string query;
  m_context.GetLinkContent(queryLink, query);
  if (query.empty())
    return InvalidArguments(action);

  std::string const normalizedQuery = Normalize(query);
  ScStructure result = m_context.GenerateStructure();
  ScAddr resultsSet = m_context.GenerateNode(ScType::ConstNode);
  result << resultsSet;

  struct SearchField
  {
    ScAddr relation;
    ScAddr reason;
  };
  SearchField const fields[] = {
      {Keynodes::nrel_main_idtf, Keynodes::concept_match_by_main_idtf},
      {Keynodes::nrel_idtf, Keynodes::concept_match_by_idtf},
      {Keynodes::nrel_definition, Keynodes::concept_match_by_definition}};

  for (auto const & field : fields)
  {
    for (auto const & value : GetRelationValues(m_context, field.relation))
    {
      std::string content;
      m_context.GetLinkContent(value.link, content);
      if (Normalize(content).find(normalizedQuery) == std::string::npos)
        continue;

      ScAddr membership = m_context.GenerateConnector(ScType::ConstPermPosArc, resultsSet, value.owner);
      ScAddr tuple = m_context.GenerateNode(ScType::ConstNodeTuple);
      ScAddr ownerArc = m_context.GenerateConnector(ScType::ConstCommonArc, value.owner, tuple);
      ScAddr reasonArc = m_context.GenerateConnector(ScType::ConstPermPosArc, field.reason, ownerArc);
      result << membership << tuple << ownerArc << reasonArc << value.connector << value.relationArc;
    }
  }

  action.SetResult(result);
  return action.FinishSuccessfully();
}

ScAddr DefinitionAnswerAgent::GetActionClass() const
{
  return Keynodes::action_for_definition_answer;
}

ScResult DefinitionAnswerAgent::DoProgram(ScAction & action)
{
  auto const &[conceptAddr] = action.GetArguments<1>();
  if (!conceptAddr.IsValid())
    return InvalidArguments(action);

  ScStructure result = m_context.GenerateStructure();
  result << conceptAddr;
  AddRelationValues(m_context, conceptAddr, Keynodes::nrel_definition, result);
  AddRelationValues(m_context, conceptAddr, Keynodes::nrel_note, result);
  AddRelationValues(m_context, conceptAddr, Keynodes::nrel_inclusion, result);

  action.SetResult(result);
  return action.FinishSuccessfully();
}

ScAddr ClassificationAnswerAgent::GetActionClass() const
{
  return Keynodes::action_for_classification_answer;
}

ScResult ClassificationAnswerAgent::DoProgram(ScAction & action)
{
  auto const &[conceptAddr] = action.GetArguments<1>();
  if (!conceptAddr.IsValid())
    return InvalidArguments(action);

  ScStructure result = m_context.GenerateStructure();
  ScAddr kindsSet = m_context.GenerateNode(ScType::ConstNode);
  result << kindsSet;

  ScIterator5Ptr iterator = m_context.CreateIterator5(
      conceptAddr, ScType::ConstCommonArc, ScType::Node, ScType::ConstPermPosArc, Keynodes::nrel_inclusion);
  while (iterator->Next())
  {
    ScAddr kind = iterator->Get(2);
    ScAddr membership = m_context.GenerateConnector(ScType::ConstPermPosArc, kindsSet, kind);
    result << membership << kind << iterator->Get(1) << iterator->Get(3);
  }

  action.SetResult(result);
  return action.FinishSuccessfully();
}

ScAddr RelationExplanationAgent::GetActionClass() const
{
  return Keynodes::action_for_relation_explanation;
}

ScResult RelationExplanationAgent::DoProgram(ScAction & action)
{
  auto const &[start, finish] = action.GetArguments<2>();
  if (!start.IsValid() || !finish.IsValid())
    return InvalidArguments(action);

  std::queue<ScAddr> pending;
  std::unordered_map<ScAddr, ScAddr, ScAddrHashFunc> previousNode;
  std::unordered_map<ScAddr, ScAddr, ScAddrHashFunc> previousConnector;
  pending.push(start);
  previousNode.emplace(start, ScAddr());

  while (!pending.empty() && previousNode.find(finish) == previousNode.end())
  {
    ScAddr current = pending.front();
    pending.pop();

    ScIterator3Ptr outgoing = m_context.CreateIterator3(current, ScType::ConstCommonArc, ScType::Node);
    while (outgoing->Next())
    {
      ScAddr next = outgoing->Get(2);
      if (previousNode.emplace(next, current).second)
      {
        previousConnector.emplace(next, outgoing->Get(1));
        pending.push(next);
      }
    }

    ScIterator3Ptr incoming = m_context.CreateIterator3(ScType::Node, ScType::ConstCommonArc, current);
    while (incoming->Next())
    {
      ScAddr next = incoming->Get(0);
      if (previousNode.emplace(next, current).second)
      {
        previousConnector.emplace(next, incoming->Get(1));
        pending.push(next);
      }
    }
  }

  if (previousNode.find(finish) == previousNode.end())
    return action.FinishWithError();

  std::vector<ScAddr> nodes;
  std::vector<ScAddr> connectors;
  for (ScAddr current = finish; current.IsValid(); current = previousNode[current])
  {
    nodes.push_back(current);
    if (current != start)
      connectors.push_back(previousConnector[current]);
  }
  std::reverse(nodes.begin(), nodes.end());
  std::reverse(connectors.begin(), connectors.end());

  ScStructure result = m_context.GenerateStructure();
  ScStructure path = m_context.GenerateStructure();
  result << start << finish << path;

  for (size_t i = 0; i < nodes.size(); ++i)
  {
    path << nodes[i];
    if (i < connectors.size())
    {
      ScAddr order = utils::NumberUtils::ResolveNumber(m_context, static_cast<unsigned>(i + 1));
      path << connectors[i] << order;
    }
  }

  action.SetResult(result);
  return action.FinishSuccessfully();
}
