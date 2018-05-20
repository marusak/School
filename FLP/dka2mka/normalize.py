#!/usr/bin/env python3
import sys
import itertools


def geuss_maping(states, refStates, rules, refRules, initState, endStates):
    if sorted(rules) == sorted(refRules):
        return (sorted(rules), sorted(states), initState, sorted(endStates))
    more = list(set(states) - set(refStates))
    less = list(set(refStates) - set(states))
    for g in itertools.product(more, less):
        nr, ns, ni, ne = convert_rules(rules, states, initState, endStates, g[0], g[1])
        if sorted(nr) == sorted(refRules):
            return (sorted(nr), sorted(ns), ni, sorted(ne))
        nr, ns, ni, ne = geuss_maping(ns, refStates, nr, refRules, ni, ne)
        if sorted(nr) == sorted(refRules):
            return (sorted(nr), sorted(ns), ni, sorted(ne))
    return (rules, states, initState, endStates)


def convert_rules(rules, states, initState, endStates, more, less):
    newRules = []
    for rule in rules:
        x = rule[0]
        y = rule[2]
        if rule[0] == more:
            x = less
        if rule[2] == more:
            y = less
        newRules.append([x, rule[1], y])
    rules = newRules
    states = [c if c != more else less for c in states]
    if initState == more:
        initState = less
    endStates = [c if c != more else less for c in endStates]
    return (newRules, states, initState, endStates)


with open(sys.argv[1], 'r') as fl:
    states = fl.readline().strip().split(",")
    initState = fl.readline().strip()
    endStates = fl.readline().strip().split(",")
    rules = []
    for line in fl:
        rules.append(line.strip().split(","))

with open(sys.argv[2], 'r') as fl:
    refStates = fl.readline().strip().split(",")
    refInitState = fl.readline().strip()
    refEndStates = fl.readline().strip().split(",")
    refRules = []
    for line in fl:
        refRules.append(line.strip().split(","))

#########################################################################
#                    HERE NORMALIZE YOUR OUTPUT                         #
#########################################################################
rules, states, initState, endStates = geuss_maping(states, refStates, rules, refRules,
                                                   initState, endStates)
#########################################################################
with open(sys.argv[1], 'w') as fl:
    fl.write(",".join(states) + "\n")
    fl.write(initState + "\n")
    fl.write(",".join(endStates) + "\n")
    for rule in rules:
        fl.write(",".join(rule) + "\n")
