# Pac-Man
## Instalation of dependencies
**SDL Library**
```bash
sudo apt install libsdl2-dev
```
**SDL_Image library** (for image handleing support)
```bash
sudo apt install libsdl2-image-dev
```
**SDL_TTF library** (for font & text handleing support)
```bash
sudo apt install libsdl2-ttf-dev
```

## Compilation & Running
After you have successfully installed SDL dependencies and have sorted those files into hierarchy below you can now compile using command:
(You have to be in folder in which is Makefile file)
```bash
make
```
And then run using this command:
```bash
./main
```

## File hierarchy
+ [Pacman]
    + README.md
    + Makefile
    + [userData]
        + highestScore.txt
    + [src]
        + ghosts.c
        + main.c
        + map.c
        + pacman.c
        + sdl.c
        + ui.c
        + [images]
            + [eyes]
                + d.bmp
                + l.bmp
                + r.bmp
                + u.bmp
            + [ghosths]
                + b-0.bmp
                + b-1.bmp
                + f-0.bmp
                + f-1.bmp
                + p-0.bmp
                + p-1.bmp
                + r-0.bmp
                + r-1.bmp
                + y-0.bmp
                + y-1.bmp
            + [map]
                + big-0.bmp
                + pill.bmp
            + [pacman]
                + [death]
                    + death0.bmp
                    + death1.bmp
                    + death2.bmp
                    + death3.bmp
                    + death4.bmp
                    + death5.bmp
                    + death6.bmp
                    + death7.bmp
                    + death8.bmp
                    + death9.bmp
                    + death10.bmp
                + pacman0.bmp
                + pacman1.bmp
                + pacman2.bmp
            + [ui]
                + [menu]
                    + exit.bmp
                    + map1.bmp
                    + map2.bmp
                    + MenuBackGround.bmp
                    + Start.bmp
                + bcg-GameOver.bmp
                + clearBcg.bmp
        + [headers]
            + ghosts.h
            + map.h
            + pacman.h
            + sdl.h
            + ui.h
        + [fonts]
            + pacman.ttf
        + [maps]
            + map1.txt
            + map2.txt
            + testMap.txt (no use just for testing purposes)

[My github](https://github.com/HankeMat)
