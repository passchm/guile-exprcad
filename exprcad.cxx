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
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <GC_MakeCircle.hxx>

#include <gp_Pln.hxx>

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

EXPRCAD_DEFINE(exprcad_cone, 3, 0, 0, (SCM bottom_radius, SCM top_radius, SCM height))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepPrimAPI_MakeCone(
                scm_to_double(bottom_radius),
                scm_to_double(top_radius),
                scm_to_double(height)
            )
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_cylinder, 2, 0, 0, (SCM radius, SCM height))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepPrimAPI_MakeCylinder(
                scm_to_double(radius),
                scm_to_double(height)
            )
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_sphere, 1, 0, 0, (SCM radius))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepPrimAPI_MakeSphere(
                scm_to_double(radius)
            )
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_common, 2, 0, 0, (SCM a, SCM b))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepAlgoAPI_Common(
                *static_cast<const TopoDS_Shape *>(scm_to_pointer(a)),
                *static_cast<const TopoDS_Shape *>(scm_to_pointer(b))
            )
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_cut, 2, 0, 0, (SCM a, SCM b))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepAlgoAPI_Cut(
                *static_cast<const TopoDS_Shape *>(scm_to_pointer(a)),
                *static_cast<const TopoDS_Shape *>(scm_to_pointer(b))
            )
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_fuse, 2, 0, 0, (SCM a, SCM b))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepAlgoAPI_Fuse(
                *static_cast<const TopoDS_Shape *>(scm_to_pointer(a)),
                *static_cast<const TopoDS_Shape *>(scm_to_pointer(b))
            )
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_translate, 4, 0, 0, (SCM x, SCM y, SCM z, SCM shape))
{
    const TopoDS_Shape &the_shape = *static_cast<const TopoDS_Shape *>(scm_to_pointer(shape));

    gp_Trsf transformation;
    transformation.SetTranslation(gp_Vec(scm_to_double(x), scm_to_double(y), scm_to_double(z)));

    return scm_from_pointer(
        new TopoDS_Shape(
            the_shape.Moved(TopLoc_Location(transformation))
        ),
        exprcad_free_shape
    );
}

SCM
exprcad_rotate(const gp_Ax1 &axis, SCM &angle, SCM &shape)
{
    const TopoDS_Shape &the_shape = *static_cast<const TopoDS_Shape *>(scm_to_pointer(shape));

    gp_Trsf transformation;
    transformation.SetRotation(axis, scm_to_double(angle));

    return scm_from_pointer(
        new TopoDS_Shape(
            the_shape.Moved(TopLoc_Location(transformation))
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_rotate_x, 2, 0, 0, (SCM angle, SCM shape))
{
    return exprcad_rotate(gp::OX(), angle, shape);
}

EXPRCAD_DEFINE(exprcad_rotate_y, 2, 0, 0, (SCM angle, SCM shape))
{
    return exprcad_rotate(gp::OY(), angle, shape);
}

EXPRCAD_DEFINE(exprcad_rotate_z, 2, 0, 0, (SCM angle, SCM shape))
{
    return exprcad_rotate(gp::OZ(), angle, shape);
}

EXPRCAD_DEFINE(exprcad_scale_uniformly, 2, 0, 0, (SCM factor, SCM shape))
{
    const TopoDS_Shape &the_shape = *static_cast<const TopoDS_Shape *>(scm_to_pointer(shape));

    gp_Trsf transformation;
    transformation.SetScaleFactor(scm_to_double(factor));

    BRepBuilderAPI_Transform builder(the_shape, transformation, true);

    return scm_from_pointer(
        new TopoDS_Shape(
            builder.Shape()
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_rectangle, 2, 0, 0, (SCM size_x, SCM size_y))
{
    const double s_x = scm_to_double(size_x);
    const double s_y = scm_to_double(size_y);

    const gp_Pln plane;
    const BRepBuilderAPI_MakeFace face_builder(plane, -0.5 * s_x, 0.5 * s_x, -0.5 * s_y, 0.5 * s_y);

    return scm_from_pointer(
        new TopoDS_Shape(
            face_builder.Face()
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_disc, 1, 0, 0, (SCM radius))
{
    GC_MakeCircle circle_builder(gp::OZ(), scm_to_double(radius));

    BRepBuilderAPI_MakeEdge edge_builder(circle_builder.Value());

    BRepBuilderAPI_MakeWire wire_builder(edge_builder.Edge());

    const BRepBuilderAPI_MakeFace face_builder(wire_builder.Wire());

    return scm_from_pointer(
        new TopoDS_Shape(
            face_builder.Face()
        ),
        exprcad_free_shape
    );
}

EXPRCAD_DEFINE(exprcad_extrude, 2, 0, 0, (SCM size_z, SCM shape))
{
    return scm_from_pointer(
        new TopoDS_Shape(
            BRepPrimAPI_MakePrism(
                *static_cast<const TopoDS_Shape *>(scm_to_pointer(shape)),
                gp_Vec(0, 0, scm_to_double(size_z)),
                true
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
