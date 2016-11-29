#!/bin/sh

rm -rf /testdb/*
./bench
mkdir bench_log
mv current_get.txt bench_log/$(date +%Y%m%d%H%M)_get.txt
mv current_set.txt bench_log/$(date +%Y%m%d%H%M)_set.txt

mv current_sumthr.txt bench_log/$(date +%Y%m%d%H%M)_sumthr.txt
mv current_thr.txt bench_log/$(date +%Y%m%d%H%M)_thr.txt
