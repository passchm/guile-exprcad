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

(export exprcad-box)

(export exprcad-cone)

(export exprcad-cylinder)

(export exprcad-sphere)

(export exprcad-common)

(begin
  (define* (exprcad-intersection shape #:rest shapes)
           (fold exprcad-common shape shapes))
  (export exprcad-intersection))

(export exprcad-cut)

(begin
  (define* (exprcad-difference shape #:rest shapes)
           (fold (lambda (x y) (exprcad-cut y x)) shape shapes))
  (export exprcad-difference))

(export exprcad-fuse)

(begin
  (define* (exprcad-union shape #:rest shapes)
           (fold exprcad-fuse shape shapes))
  (export exprcad-union))

(export exprcad-translate)

(export exprcad-rotate-x)

(export exprcad-rotate-y)

(export exprcad-rotate-z)

(export exprcad-scale-uniformly)

(export exprcad-rectangle)

(export exprcad-rounded-rectangle)

(export exprcad-disc)

(export exprcad-extrude)

(export exprcad-export-step)
