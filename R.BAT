@echo off
timeit pc r
if EXIST trp.mix goto link
echo Can't link.. No object.
goto done
:link
timeit pcl [5K,40K,] trp trpfunc trpform trpplot trppop trpwndo vpeek vpoke trpvers trpfile trpprint;qc
copy trp.exe \trp
:done
rem beep
echo All done




