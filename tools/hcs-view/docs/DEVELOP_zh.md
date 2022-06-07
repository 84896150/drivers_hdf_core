# HCS可视化工具开发说明

若当前工具功能不满足开发者需求，开发者需增强工具能力，则可基于已有源码进行工具二次开发，编译打包生成自定义的插件。

## 使用场景

开发者需增强工具能力，进行二次开发，编译打包生成自定义的插件。

## 工具开发

### 环境说明

系统：建议Ubuntu 20.04或者Windows 10

### 开发步骤

1. 在drivers_framework/tools/hcs-view/hcsVSCode这个目录下执行命令：

		npm i vsce

2. 在drivers_framework/tools/hcs-view/hcsVSCode这个目录下执行命令：

		npx vsce package

3. 每个选项都选择y，然后回车，最终会在当前目录下打包生成一个插件hcseditor-0.0.1.vsix。以Ubuntu环境为例，结果如下：

		harmony@Ubuntu-64:~/hcsVSCode$ npx vsce package
		WARNING  A 'repository' field is missing from the 'package.json' manifest file.
		Do you want to continue? [y/N] y
		WARNING  Using '*' activation is usually a bad idea as it impacts performance.
		More info: https://code.visualstudio.com/api/references/activation-events#Start-up
		Do you want to continue? [y/N] y
		WARNING  LICENSE.md, LICENSE.txt or LICENSE not found
		Do you want to continue? [y/N] y
		This extension consists of 2318 files, out of which 1223 are JavaScript files. For performance reasons, you should bundle your extension: https://aka.ms/vscode-bundle-extension . You should also exclude unnecessary files by adding them to your .vscodeignore: https://aka.ms/vscode-vscodeignore
		   
## 工具测试

  暂无

