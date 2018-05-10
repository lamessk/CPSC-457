#!/bin/bash
#############################
# Last Name:   Kharfan
# First Name:  Lamess
# Student ID:  10150607
# Course:      CPSC 457
# Tutorial:    T02
# Assignment:  2
# Question:    Q5
# File name: scan.sh
#############################
find -type f -name "*.$1" -printf "%p %s\n" | sort -n -k 2 -r | head -$2 | awk '{print $1 " " $2; total += $2} END { print "Total size: " total }' 