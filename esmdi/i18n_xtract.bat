xgettext -C -T --no-location --no-wrap -d esmdi -k_ -k_i -o locale/esmdi.SRC.pot *.cpp *.h *.hxx *.def
msgcat -s --no-wrap -o locale/esmdi.pot locale/esmdi.SRC.pot
