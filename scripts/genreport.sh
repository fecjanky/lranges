#!/bin/sh
cd $(dirname $0)/..
rm -rf report
mkdir report
python -m gcovr -r . --html --html-details -o report/coverage_report.html --filter="include/.*" --html-title "coverage report" --exclude-unreachable-branches
