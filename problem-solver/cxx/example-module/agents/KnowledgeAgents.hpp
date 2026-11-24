/*
 * This source file is part of an OSTIS project.
 * Distributed under the MIT License.
 */

#pragma once

#include <sc-memory/sc_agent.hpp>

class ConceptSearchAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

class DefinitionAnswerAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

class ClassificationAnswerAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};

class RelationExplanationAgent : public ScActionInitiatedAgent
{
public:
  ScAddr GetActionClass() const override;
  ScResult DoProgram(ScAction & action) override;
};
