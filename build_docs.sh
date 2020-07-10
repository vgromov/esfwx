#!/bin/sh

# esfwx documentation generation
start="$(pwd)"

for dir in escore escomm esmath esscript 
do
  cd "$dir"
  doxygen Doxyfile
  cd "$start"
done

cd "$start/doc"
for docname in escore escomm esmath esscript 
do
  cd $docname
  pdflatex refman.tex 
  cp -u refman.pdf "../$docname.pdf"
  cd "$start/doc"
done

cd "$start"

