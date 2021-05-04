#!/bin/bash

for f in $(ls tdms/202104*.tdms) $(ls tdms/202105*.tdms)
do
    python l_tdms2root.py $f
done
