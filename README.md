### Install gcc
```
sudo apt update
sudo apt install gcc
sudo apt install g++
sudo apt install libcurl4-openssl-dev
```

### Compile and run:
```
gcc -O2 main.c -o main -lcurl # or `gcc -g main.c -o main -lcurl`
chmod +x main
./main
```
