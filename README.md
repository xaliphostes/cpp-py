# Initiation au C++ et au binding python

## Compilation

### 1) C++
1. Go to the `cpp` folder
2. Create a `build` directory
3. Go inside and type
```sh
cmake ..
make
```

To test the C++ code, go to the bin directory (`../bin`), and type
```sh
./app
```

### 2) Python binding
Go to the `py` folder and type
```sh
yarn install
yarn build
```

To test the binding, go to the bin directory (`../bin`), and type
```sh
python3 test.py
```

<br><br><br>

## To look at the dependencies (macos)
In the bin directory, type
```sh
otool -L app
```
or
```sh
otool -L pyalgo.so
```
