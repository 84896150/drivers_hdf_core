# HDI-GEN
  - [Introduction](#introduction)
  - [Usage](#Usage)

## Introduction
HDI-GEN is a part of the HDF development suite and is used to convert the HDI interface description language into target source code (C/CPP and Java) that can be compiled and executed during compilation.


## Usage
**Show help**
```shell
$./hdi-gen --help
Compile a .idl file and generate C/C++ and Java codes.
Usage: idl [options] file
Options:
  --help            Display command line options
  --version         Display toolchain version information
  --hash            Display hash key of the idl file
  --dump-ast        Display the AST of the compiled file
  -c                Compile the .idl file
  --gen-c           Generate C codes
  --gen-cpp         Generate C++ codes
  --gen-java        Generate Java codes
  -d <directory>    Place generated codes into <directory>

```

**Show version**
```shell
$hdi-gen --version
```

**Generates hash-key of the .idl file**
```shell
$hdi-gen -c ./test/ISample.idl --hash
```

**Show ast dump of th idl file**
```shell
$hdi-gen -c ./test/ISample.idl --dump-ast
```

**Generates c code**
```shell
$hdi-gen -c ./test/ISample.idl --gen-c -d ./out
```

Node: It's supported to generates C/C++ and Java code now.