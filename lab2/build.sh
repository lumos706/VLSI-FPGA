g++ -std=c++14 -I E:\eigen-3.4.0 -o main main.cpp Arch.cpp Global.cpp Object.cpp Solution.cpp 
./main ./benchmark/small.txt ./output/small_placement.txt
./main ./benchmark/med1.txt ./output/med1_placement.txt
./main ./benchmark/med2.txt ./output/med2_placement.txt
./main ./benchmark/lg1.txt ./output/lg1_placement.txt
./main ./benchmark/lg2.txt ./output/lg2_placement.txt
./main ./benchmark/xl.txt ./output/xl_placement.txt
./main ./benchmark/huge.txt ./output/huge_placement.txt