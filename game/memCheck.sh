#!/bin/sh
valgrind --leak-check=full -v ./a.out
