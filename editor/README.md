# Simple browser-based editor

This directory contains a simple browser-based editor based on
[Monaco Editor](https://microsoft.github.io/monaco-editor/)
and [model-viewer](https://modelviewer.dev/).

If `guile-exprcad` is properly installed, the server can be launched by running the following command in the current directory:
```sh
guile -s ./server.scm
```
Then, visit `http://localhost:8080/` in a browser.

It was tested in [Firefox 128](https://packages.debian.org/bookworm/firefox-esr)
and [Chromium 130](https://packages.debian.org/bookworm/chromium).

## License

Copyright 2024 Pascal Schmid

This file is part of guile-exprcad.

guile-exprcad is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

guile-exprcad is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with guile-exprcad. If not, see <https://www.gnu.org/licenses/>.
