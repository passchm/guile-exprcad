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
