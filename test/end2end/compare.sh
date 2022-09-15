#!/bin/bash

base_folder="resources"

red=`tput setaf 1`
green=`tput setaf 2`
reset=`tput sgr0`

current_folder=${3:-./}
passed=true

exec 3>${current_folder}/compared.dat
exec 2>&3
exec 1>&3


for file in ${current_folder}/${base_folder}/test*.dat; do
    echo -n -e "Testing ${file} ... \n\n"
    echo "MY:"
    { time $1 < $file; }
    echo -e "\nSTD:"
    { time $2 < $file; }
    echo "------"

    # Compare inputs
done

exit 0