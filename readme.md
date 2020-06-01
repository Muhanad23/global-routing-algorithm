# Improved Global Routing Using A-Star Algorithm

| Name                         | Sec | B.N |              Email               |
| ---------------------------- | :-: | :-: | :------------------------------: |
| Muhanad Atef Mustafa Mekkawy |  2  | 26  |     muhanad.atef23@gmail.com     |
| Abdulrahman Khalid Hassan    |  1  | 31  | abdulrahman.elshafei98@gmail.com |
| Mahmoud Mohamed Mahmoud Ali  |  2  | 22  |        Mmmacmp@gmail.com         |
| Hossam ahmed mahmoud ibrahem |  1  | 15  |   hossamahmed201515@gmail.com    |

First, we wrote A\* code algorithm then we added the algorithm with different heuristic function into
[cu-gr](https://github.com/cuhk-eda/cu-gr) code in MazeRoute.cpp and MazeRoute.h files which you will find in **Code/paper_code/src/single_net** folder

- Note that the first code doesn't have rerouting option while the second one has.

---

## 1) A\* algorithm Code

### Dependencies

- Any c++ compiler (e.g: g++)

### How to compile

```
$ cd Code/a_star/
$ make
```

### How to Run

#### For test case 1

```
$ ./run.out input2.txt output2.txt
```

#### For test case 2

```
$ ./run.out input1.txt output1.txt
```

---

## 2) Paper Code

- First cut paper_code folder and put it in the outer folder (make sure paper_code folder is not in an inner folder that could cause a problem while building).
- You will find test cases in **Code/paper_code/toys/** folder.
- The output guide will be in **Code/paper_code/run/** folder.
- I there is a problem you can clone the repo [cu-gr](https://github.com/cuhk-eda/cu-gr) then copy toys folder, MazeRoute.cpp and MazeRoute.h and put them in their right directory.

### Dependencies

- [GCC](https://gcc.gnu.org)
- [CMake](https://cmake.org)
- [Boost](https://www.boost.org)
- [Python](https://www.python.org)
- [Rsyn](https://github.com/RsynTeam/rsyn-x)

### How to Build

```
$ cd  Code/paper_code/
$ ./build.sh
```

### How to Run

#### For test case 1

```
$ ./test1_run.sh
```

#### For test case 2

```
$ ./test2_run.sh
```

#### For test case 3

```
$ ./test3_run.sh
```

#### For test case 4

```
$ ./test4_run.sh
```

#### For test case 5

```
$ ./test5_run.sh
```

#### For test case 6

```
$ ./test6_run.sh
```

#### For test case 7

```
$ ./test7_run.sh
```

#### For test case 8

```
$ ./test8_run.sh
```

---

### Another Heuristic Function

You can chose your own heuristic function by changing the heuristic lambda function in **Code/paper_code/src/single_net/MazeRoute.cpp**

---

- If any problem while building the code please, don't hesitate to contact us

---

® [cu-gr](https://github.com/cuhk-eda/cu-gr)
