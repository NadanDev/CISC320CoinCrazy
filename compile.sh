#!/bin/sh
declare -a arrc=()
declare -a prm=()
while IFS= read -r -d $'\0' f; do
    arrc+=("$f")
done < <(find . -name "*.cpp" -print0)
prm=$(IFS=' ' ; echo "${arrc[*]}")
cmdg=""
cmdg+="g++ "
cmdg+="${prm}"
cmdg+=" -o "
cmdg+="${1}"
set -x
eval "${cmdg}"
set -x