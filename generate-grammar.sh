#!/usr/bin/env bash

mkdir src/generated >> /dev/null 2>&1 || echo ""
cd src/generated || exit
yacc -v -d ../../res/Grammar.y -o Parser.tab.cpp
cd .. || exit
cd .. || exit