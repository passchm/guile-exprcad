# syntax=docker/dockerfile:1.4
FROM debian:bookworm

# Copyright 2023 Pascal Schmid
#
# This file is part of guile-exprcad.
#
# guile-exprcad is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# guile-exprcad is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with guile-exprcad.  If not, see <https://www.gnu.org/licenses/>.

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    guile-3.0-dev \
    libocct-foundation-dev \
    libocct-modeling-data-dev \
    libocct-modeling-algorithms-dev \
    libocct-visualization-dev \
    libocct-ocaf-dev \
    libocct-data-exchange-dev \
    && rm -rf /var/lib/apt/lists/*

COPY ./CMakeLists.txt ./snarf.cmake ./exprcad.cxx ./exprcad.scm /project/

RUN --network=none useradd --system --user-group builder \
    && mkdir /project/build/ \
    && chown -R builder:builder /project/build/

WORKDIR /project/build/

USER builder

ARG EXPRCAD_VERSION=0.1.0

RUN --network=none cmake --install-prefix /usr ..

RUN --network=none cmake --build .

RUN --network=none DESTDIR="${PWD}/package" cmake --install .

COPY --chown=builder:builder <<EOF ./package/DEBIAN/control
Package: guile-exprcad
Version: ${EXPRCAD_VERSION}
Architecture: amd64
Priority: optional
Homepage: https://github.com/passchm/guile-exprcad
Maintainer: Pascal Schmid <175302205+passchm@users.noreply.github.com>
Depends: guile-3.0,
    libocct-data-exchange-7.6,
    libocct-ocaf-7.6,
    libocct-visualization-7.6,
    libocct-modeling-algorithms-7.6,
    libocct-modeling-data-7.6,
    libocct-foundation-7.6
Description: CAD library for Guile
EOF

RUN --network=none dpkg-deb --root-owner-group --build ./package/ ./guile-exprcad_${EXPRCAD_VERSION}_amd64.deb

USER root

FROM scratch

COPY --from=0 --chown=root /project/build/libguile-exprcad.so /project/build/*.deb /build/
