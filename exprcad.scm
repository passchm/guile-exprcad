;; Copyright 2023 Pascal Schmid
;;
;; This file is part of guile-exprcad.
;;
;; guile-exprcad is free software: you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.
;;
;; guile-exprcad is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with guile-exprcad.  If not, see <https://www.gnu.org/licenses/>.

(define-module (exprcad)
               #:use-module ((srfi srfi-1) #:select (fold)))

(load-extension "libguile-exprcad" "exprcad_init")

(export box)

(export cone)

(export cylinder)

(export sphere)

(export top-half-space)

(export common)

(begin
  (define* (intersection shape #:rest shapes)
           (fold common shape shapes))
  (export intersection))

(export cut)

(begin
  (define* (difference shape #:rest shapes)
           (fold (lambda (x y) (cut y x)) shape shapes))
  (export difference))

(export fuse)

(begin
  (define* (union shape #:rest shapes)
           (fold fuse shape shapes))
  (export union))

(export translate)

(export rotate-radians-x)

(export rotate-radians-y)

(export rotate-radians-z)

; /usr/include/math.h M_PI
(define-public pi 3.14159265358979323846)

(define-public (degrees->radians degrees)
               (* (/ pi 180) degrees))

(export scale-uniformly)

(export axis-mirror)

(export plane-mirror)

(export rectangle)

(export rounded-rectangle)

(export disc)

(export extrude)

(export bounding-box)

(export export-step)

(define-public (export-step-file filename shape)
               (call-with-output-file filename
                                      (lambda (port) (export-step port shape))))

(export export-glb-file)

(export export-ascii-stl-file)

(export import-step-file)

(export import-stl-file)

(export count-faces)

(export count-edges)

(export count-vertices)

(export fillet-2d-vertices-radii)

(export fillet-3d-edges-radii)

(export filter-planar-faces)

(export filter-aligned-faces)

(export filter-edges-of-face)

(export filter-parallel-linear-edges)
