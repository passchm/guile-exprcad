;; Copyright 2024 Pascal Schmid
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

(use-modules ((web server) #:select (run-server))
             ((web request) #:select (request-uri request-method))
             ((web response) #:select (build-response))
             ((web uri) #:select (uri-path uri->string split-and-decode-uri-path))
             ((ice-9 textual-ports) #:select (get-string-all put-string))
             ((ice-9 binary-ports) #:select (get-bytevector-all))
             ((rnrs bytevectors) #:select (utf8->string))
             ((ice-9 eval-string) #:select (eval-string))
             ((ice-9 match) #:select (match)))

(define (request-path-components request)
  (split-and-decode-uri-path (uri-path (request-uri request))))

(define (not-found request)
  (values (build-response #:code 404)
          (string-append "Resource not found: "
                         (uri->string (request-uri request)))))

(define initial-code "(use-modules ((exprcad) #:prefix exprcad:))\n\n(exprcad:export-glb-file \"model.glb\" (exprcad:sphere 1.0))\n")

(unless (file-exists? "model.scm")
  (call-with-output-file "model.scm"
                         (lambda (port) (put-string port initial-code))))

(define (eval-handler request body)
  (match (cons (request-method request) (request-path-components request))
         (('GET)
          (values '((content-type . (application/xhtml+xml (charset . "utf-8"))))
                  (call-with-input-file "index.xhtml" get-string-all)))
         (('GET "model.glb")
          (values '((content-type . (model/gltf-binary)))
                  (call-with-input-file "model.glb" get-bytevector-all)))
         (('GET "model.scm")
          (values '((content-type . (text/plain)))
                  (call-with-input-file "model.scm" get-bytevector-all)))
         (('PUT "model.scm")
          (call-with-output-file "model.scm"
                                 (lambda (port) (put-string port (utf8->string body))))
          (values (build-response #:code 204) ""))
         (('POST "run")
          (values '((content-type . (text/plain)))
                  ; (call-with-output-string (lambda (port) (display (eval-string (utf8->string body)) port)))))
                  (let ((model-code (call-with-input-file "model.scm" get-string-all)))
                    (call-with-output-string (lambda (port) (display (eval-string model-code) port))))))
         (_ (not-found request))))

(run-server eval-handler)
