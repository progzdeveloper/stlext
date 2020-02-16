#!/bin/bash


lcov --directory tmp --capture --output-file compact.info

mkdir ./coverage-report

genhtml compact.info --output-directory ./coverage-report


