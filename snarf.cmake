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

file(READ "${SNARF_INPUT_FILE}" EXPRCAD_CODE)
string(REGEX REPLACE ";" "\\\\;" EXPRCAD_LINES "${EXPRCAD_CODE}")
string(REGEX REPLACE "\n" ";" EXPRCAD_LINES "${EXPRCAD_LINES}")
foreach(EXPRCAD_LINE IN LISTS EXPRCAD_LINES)
    if(EXPRCAD_LINE MATCHES "^EXPRCAD_DEFINE\\(")
        string(REPLACE "EXPRCAD_DEFINE(" "" EXPRCAD_LINE_REM "${EXPRCAD_LINE}")
        string(REGEX REPLACE ";" "\\\\;" EXPRCAD_LINE_REM "${EXPRCAD_LINE_REM}")
        string(REGEX REPLACE "," ";" EXPRCAD_LINE_REM "${EXPRCAD_LINE_REM}")
        list(TRANSFORM EXPRCAD_LINE_REM STRIP)
        list(POP_FRONT EXPRCAD_LINE_REM EXPRCAD_DEF_FNAME EXPRCAD_DEF_REQ EXPRCAD_DEF_OPT EXPRCAD_DEF_VAR)
        string(REPLACE "_" "-" EXPRCAD_DEF_PRIMNAME "${EXPRCAD_DEF_FNAME}")
        string(APPEND EXPRCAD_GSUBR "scm_c_define_gsubr(\"${EXPRCAD_DEF_PRIMNAME}\", ")
        string(APPEND EXPRCAD_GSUBR "${EXPRCAD_DEF_REQ}, ${EXPRCAD_DEF_OPT}, ${EXPRCAD_DEF_VAR}, ")
        string(APPEND EXPRCAD_GSUBR "reinterpret_cast<void *>(${EXPRCAD_DEF_FNAME}));\n")
    endif()
endforeach()
file(WRITE "${SNARF_OUTPUT_FILE}" "${EXPRCAD_GSUBR}")
