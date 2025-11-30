
#pragma once
#include <iostream> 

#include "Cell.h"


class Rule
{
public:
    virtual ~Rule() = default;

    // Retourne le futur état (vivant ou mort) selon la règle
    virtual bool computeNextState(const Cell& cell, int aliveNeighbors) const = 0;
};


class ConwayRule : public Rule
{
public:
    bool computeNextState(const Cell& cell, int aliveNeighbors) const override;
};

