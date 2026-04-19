# war3-fixer
修复Windows11下运行魔兽3打RPG地图时出现的中文叠字问题

## 编译说明

使用mingw在命令行下编译，使用如下命令：

```
gcc -Wall -Wextra -O2 -c main.c -o main.o
gcc -Wall -Wextra -O2 -c config.c -o config.o
gcc -Wall -Wextra -O2 -c fixer.c -o fixer.o
gcc -Wall -Wextra -O2 -c winapi.c -o winapi.o
gcc -o war3-fixer.exe main.o config.o fixer.o winapi.o
```

## 使用说明

将`war3-fixer.exe`放到魔兽争霸3的运行目录下（包含War3.exe的目录），双击运行`war3-fixer.exe`即可自动启动窗口模式的魔兽游戏。
