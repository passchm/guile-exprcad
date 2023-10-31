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

COPY ./CMakeLists.txt ./exprcad.cxx /project/

RUN --network=none useradd --system --user-group builder \
    && mkdir /project/build/ \
    && chown -R builder:builder /project/build/

WORKDIR /project/build/

USER builder

RUN --network=none cmake ..

RUN --network=none cmake --build .

USER root

FROM scratch

COPY --from=0 --chown=root /project/build/ /build/
