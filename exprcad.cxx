/*
Copyright 2023 Pascal Schmid

This file is part of guile-exprcad.

guile-exprcad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

guile-exprcad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with guile-exprcad.  If not, see <https://www.gnu.org/licenses/>.
*/


extern "C"
{
#include <libguile.h>
}


#include <BRepPrimAPI_MakeBox.hxx>

#include <STEPControl_Writer.hxx>


#define EXPRCAD_DEFINE(FNAME, REQ, OPT, VAR, ARGLIST) \
SCM_SNARF_HERE(\
extern "C" SCM FNAME ARGLIST\
)


extern "C"
void
exprcad_free_shape(void *shape)
{
    delete static_cast<TopoDS_Shape *>(shape);
}

EXPRCAD_DEFINE(exprcad_box, 3, 0, 0, (SCM size_x, SCM size_y, SCM size_z))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepPrimAPI_MakeBox(
                scm_to_double(size_x),
                scm_to_double(size_y),
                scm_to_double(size_z)
            )
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_export_step, 2, 0, 0, (SCM shape, SCM filename))
{
    char *the_filename = scm_to_locale_string(filename);

    STEPControl_Writer writer;
    writer.Transfer(
        *static_cast<const TopoDS_Shape *>(scm_to_pointer(shape)),
        STEPControl_AsIs
    );
    writer.Write(the_filename);

    free(the_filename);

    return SCM_BOOL_T;
}

extern "C"
void
exprcad_init()
{
#ifndef SCM_MAGIC_SNARFER
#include "exprcad_snarf_init.hxx"
#endif
}
