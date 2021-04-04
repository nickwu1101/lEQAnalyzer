#!/bin/bash

for f in $(ls tdms/20210402*.tdms)
do
    python l_tdms2root.py $f
done
