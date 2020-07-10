xgettext -C -T --no-location --no-wrap -d escore -k_ -k_i -o locale/escore.SRC.pot *.cpp *.h *.hxx *.def
msgcat -s --no-wrap -o locale/escore.pot locale/escore.SRC.pot locale/lang_names.pot locale/lang_names_rare.pot
