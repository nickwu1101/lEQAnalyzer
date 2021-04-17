#!/bin/bash

for f in $(ls tdms/20210404*.tdms) $(ls tdms/20210405*.tdms) $(ls tdms/20210406*.tdms) $(ls tdms/20210407*.tdms) $(ls tdms/20210408*.tdms) $(ls tdms/20210409*.tdms)
do
    python l_tdms2root.py $f
done
