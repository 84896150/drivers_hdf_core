# HDI-GEN
  - [简介](#简介)
  - [使用说明](#使用说明)

## 简介
HDI-GEN工具属于HDF开发套件的一部分，用于在编译阶段将HDI接口描述语言转换为可以编制执行的目标源码(C/CPP、Java).

## 使用说明
查看帮助
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

查看版本
```shell
$hdi-gen --version
```

生成.idl文件hash值
```shell
$hdi-gen -c ./test/ISample.idl --hash
```

查看.idl文件dump
```shell
$hdi-gen -c ./test/ISample.idl --dump-ast
```

生成C代码
```shell
$hdi-gen -c ./test/ISample.idl --gen-c -d ./out
```
注：现支持C、C++、Java代码生成