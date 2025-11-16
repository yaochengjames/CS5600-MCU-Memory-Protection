#!/bin/bash

echo "================================="
echo "Running all experiments..."
echo "================================="

cd baseline_and_static

# Baseline
echo "[1/2] Running Baseline..."
make CONFIG=baseline clean > /dev/null 2>&1
make CONFIG=baseline > /dev/null 2>&1
timeout 120 make CONFIG=baseline run > ../results/baseline_latest.log 2>&1
echo "Baseline complete"

# Static  
echo "[2/2] Running Static MPU..."
make CONFIG=static clean > /dev/null 2>&1
make CONFIG=static > /dev/null 2>&1
timeout 120 make CONFIG=static run > ../results/static_latest.log 2>&1
echo "Static complete"

