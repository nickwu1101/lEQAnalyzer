#!/bin/bash

for f in $(ls tdms/202104*.tdms)
do
    python l_tdms2root.py $f
done
