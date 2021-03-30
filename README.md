# Teditor
## A simple text/code editor

<image src="teditor.gif">

A simple text and code editor made in ncurses with UTF-8 support,
mouse events (including scroll and clicks), autotab, a permanent
stratch buffer (just run `ted`, without any argument), open,
save as, help page(s), C/C++, Python and Bash/Zsh syntax highlighting.
All this with only less than 1500 lines of C code
(counting with cloc).

## Installing

### Arch Linux

Go to your programs directory and do:

```sh
git clone https://github.com/ArthurBacci64/Teditor.git
cd Teditor
git checkout stable
makepkg -si
```

Installed.

### Manual installation

Go to your programs directory and do: 

```sh
git clone https://github.com/ArthurBacci64/Teditor.git
cd Teditor
git checkout stable
sudo make PREFIX=/usr install
```

Installed.

If you want to install locally, do `make PREFIX=~/.local install` instead of `sudo make install`.

## Uninstalling

Just do:

```sh
rm ~/.config/ted -rf
sudo rm $(which ted) -rf
```

If you installed locally, you does not need to use `sudo`.

## Help

Press `Ctrl-G`, type `manual`, press `ENTER` and press `ENTER` again.

## Contributing

Please open a Pull Request if you have a solution or a new feature.

Please open a Issue if you find some bug or if you have a suggestion.

Pages in the Wiki also are appreciated

Thanks for reading.
