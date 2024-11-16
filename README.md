# guile-exprcad

`guile-exprcad` is a computer-aided design (CAD) library for [Guile](https://www.gnu.org/software/guile/).

*This project is still very much work in progress!*

The library allows building CAD models in a functional programming style.
It uses an immutable data structure to represent geometric shapes.

Most of the heavy lifting is done by [Open CASCADE Technology](https://dev.opencascade.org/).

## Usage

Place the following code in a file called `model.scm`:
```scheme
(use-modules ((exprcad) #:prefix exprcad:))

(define model
  (exprcad:box 1 2 3))

(exprcad:export-step-file "model.step" model)
```
Then, run the script:
```sh
guile -s ./model.scm
```
This generates the chosen shape and exports it to a STEP file called `model.step`.

Currently, *the code is the documentation* as the library is still under heavy development.

## Install

On *Debian GNU/Linux 12 (bookworm)*, download or build the `.deb` file, then install it:
```sh
apt install ./guile-exprcad_*_amd64.deb
```

## Build

Install the [`docker.io`](https://packages.debian.org/bookworm/docker.io) package and run the following command:
```sh
sudo DOCKER_BUILDKIT=1 docker build --output type=tar,dest=build.tar .
```

Then, extract the generated `build.tar` file into the `build` directory to obtain the package file:
```sh
tar --extract --file ./build.tar --one-top-level=build --strip-components=1
```

## Inspiration

`guile-exprcad` is inspired by these great projects:

- [libfive](https://github.com/libfive/libfive)
- [CadQuery](https://github.com/CadQuery/cadquery)
- [OpenSCAD](https://openscad.org/)
- [Cascade Studio](https://github.com/zalo/CascadeStudio/)
- [OCCT Draw Harness](https://draw.sview.ru/)

## License

Copyright 2023 Pascal Schmid

This file is part of guile-exprcad.

guile-exprcad is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

guile-exprcad is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with guile-exprcad. If not, see <https://www.gnu.org/licenses/>.
