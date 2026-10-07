I am trying to build a really good game without game engines like Unreal Engine and Unity.

Currently I am using C , GCC Compiler , SDL2 , MSYS2(UCRT64).


Dependency Structure :

```text
main.c
   |
   +----------------+----------------+
   |                |                |
 game.c          input.c        renderer.c
   |                |                |
   +-------+        |        +-------+
   |       |        |        |       |
player.c target.c input.h renderer.h config.h
```
