#!/bin/bash

for f in $(ls tdms/2021041*.tdms)
do
    python l_tdms2root.py $f
done
