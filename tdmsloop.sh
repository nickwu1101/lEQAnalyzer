#!/bin/bash

for f in $(ls tdms/202106*.tdms)
do
    python l_tdms2root.py $f
done
