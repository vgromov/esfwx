rem esfwx documentation generation

set start="%cd%"

for %%d in (escore escomm esmath esscript) do (
cd %%d
doxygen Doxyfile
cd %start%
)

cd %start%\doc
for %%d in (escore escomm esmath esscript) do (
cd %%d
make.bat
copy /Y refman.pdf ..\%%d.pdf
cd %start%\doc
)

cd %start%