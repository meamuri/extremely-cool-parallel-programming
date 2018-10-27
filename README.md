# Parallel programming course

Before build and launch application:
```
$ chmod +x run
```

## Build and run app

execute `./run` or next commands:

```
mpicc -o main main.cpp
mpirun -np 2 ./main
```