# My Set
Rethinking std::set. My goal is to create AVL tree that would be more efficient in the tasks of finding kth smallest elemet in a set and finding the number of elements smaller then the given one.

## How to compile
```
cmake -S . -B build -DNOGTEST=FALSE -DCOMPARE=FALSE
make -C build -j12 install DESTDIR=/whatever/you/want
```
There I set NOGTEST to FALSE to enable unit-tests and COMPARE to FALSE to disable comparation with std::set. 
 
## How to run
 ```
cd build
ctest
 ```
## Results
If you want to see the results of comparison with std::set in the tasks of finding kth smallest elemet in a set and finding the number of elements smaller then the given one you can check this [file](results/compared.dat) or enable comparation and test it again.