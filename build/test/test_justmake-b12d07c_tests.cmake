add_test( ParseDeps /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake ParseDeps  )
set_tests_properties( ParseDeps PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( ComparaisonTemps /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake ComparaisonTemps  )
set_tests_properties( ComparaisonTemps PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( CommandBuilder /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake CommandBuilder  )
set_tests_properties( CommandBuilder PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( CommandExecOneOk /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake CommandExecOneOk  )
set_tests_properties( CommandExecOneOk PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( CommandExecOneErreurCommande /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake CommandExecOneErreurCommande  )
set_tests_properties( CommandExecOneErreurCommande PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( CommandExecOneErreurExec /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake CommandExecOneErreurExec  )
set_tests_properties( CommandExecOneErreurExec PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( CommandExecAllOk /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake CommandExecAllOk  )
set_tests_properties( CommandExecAllOk PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( CommandExecAllKeepGoing /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake CommandExecAllKeepGoing  )
set_tests_properties( CommandExecAllKeepGoing PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
add_test( ListerRepertoire /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/bin/test_justmake ListerRepertoire  )
set_tests_properties( ListerRepertoire PROPERTIES WORKING_DIRECTORY /home/marius/Bureau/session4/INF3173/TP/tp1/inf3173-243-tp1-v2-etudiant/build/test SKIP_RETURN_CODE 4)
set( test_justmake_TESTS ParseDeps ComparaisonTemps CommandBuilder CommandExecOneOk CommandExecOneErreurCommande CommandExecOneErreurExec CommandExecAllOk CommandExecAllKeepGoing ListerRepertoire)
