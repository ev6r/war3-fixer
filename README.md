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
