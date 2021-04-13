#!/bin/bash

for f in $(ls tdms/2020122*.tdms)
do
    python l_tdms2root_ch1err.py $f
done
