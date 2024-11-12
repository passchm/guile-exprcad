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
#include <BRepPrimAPI_MakeHalfSpace.hxx>

#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRepFilletAPI_MakeFillet2d.hxx>

#include <GC_MakeCircle.hxx>

#include <gp_Pln.hxx>

#include <BRepBndLib.hxx>

#include <STEPControl_Writer.hxx>

#include <StepData_Protocol.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>

#include <BRepMesh_IncrementalMesh.hxx>
#include <XCAFApp_Application.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <Message_ProgressRange.hxx>
#include <RWGltf_CafWriter.hxx>

#include <StlAPI_Writer.hxx>


#define EXPRCAD_DEFINE(FNAME, REQ, OPT, VAR, ARGLIST) \
SCM_SNARF_HERE(\
extern "C" SCM FNAME ARGLIST\
)


// TopoDS_Shape
SCM exprcad_type_shape;


extern "C"
void
exprcad_finalize_shape(SCM shape)
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    TopoDS_Shape *the_shape = static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    if (the_shape != nullptr) {
        scm_foreign_object_set_x(shape, 0, nullptr);

        delete the_shape;
        the_shape = nullptr;
    }

    scm_remember_upto_here_1(shape);
}


EXPRCAD_DEFINE(exprcad_box, 3, 0, 0, (SCM size_x, SCM size_y, SCM size_z))
{
    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            BRepPrimAPI_MakeBox(
                scm_to_double(size_x),
                scm_to_double(size_y),
                scm_to_double(size_z)
            )
        )
    );
}

EXPRCAD_DEFINE(exprcad_cone, 3, 0, 0, (SCM bottom_radius, SCM top_radius, SCM height))
{
    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            BRepPrimAPI_MakeCone(
                scm_to_double(bottom_radius),
                scm_to_double(top_radius),
                scm_to_double(height)
            )
        )
    );
}

EXPRCAD_DEFINE(exprcad_cylinder, 2, 0, 0, (SCM radius, SCM height))
{
    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            BRepPrimAPI_MakeCylinder(
                scm_to_double(radius),
                scm_to_double(height)
            )
        )
    );
}

EXPRCAD_DEFINE(exprcad_sphere, 1, 0, 0, (SCM radius))
{
    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            BRepPrimAPI_MakeSphere(
                scm_to_double(radius)
            )
        )
    );
}

EXPRCAD_DEFINE(exprcad_top_half_space, 0, 0, 0, ())
{
    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            BRepPrimAPI_MakeHalfSpace(
                BRepBuilderAPI_MakeFace(gp_Pln()),
                gp_Pnt(0, 0, 1)
            )
        )
    );
}

EXPRCAD_DEFINE(exprcad_common, 2, 0, 0, (SCM shape_a, SCM shape_b))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape_a);
    scm_assert_foreign_object_type(exprcad_type_shape, shape_b);
    const TopoDS_Shape &the_shape_a = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape_a, 0));
    const TopoDS_Shape &the_shape_b = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape_b, 0));

    TopoDS_Shape *result_shape = new TopoDS_Shape(BRepAlgoAPI_Common(the_shape_a, the_shape_b));

    scm_remember_upto_here_1(shape_a);
    scm_remember_upto_here_1(shape_b);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_cut, 2, 0, 0, (SCM shape_a, SCM shape_b))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape_a);
    scm_assert_foreign_object_type(exprcad_type_shape, shape_b);
    const TopoDS_Shape &the_shape_a = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape_a, 0));
    const TopoDS_Shape &the_shape_b = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape_b, 0));

    TopoDS_Shape *result_shape = new TopoDS_Shape(BRepAlgoAPI_Cut(the_shape_a, the_shape_b));

    scm_remember_upto_here_1(shape_a);
    scm_remember_upto_here_1(shape_b);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_fuse, 2, 0, 0, (SCM shape_a, SCM shape_b))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape_a);
    scm_assert_foreign_object_type(exprcad_type_shape, shape_b);
    const TopoDS_Shape &the_shape_a = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape_a, 0));
    const TopoDS_Shape &the_shape_b = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape_b, 0));

    TopoDS_Shape *result_shape = new TopoDS_Shape(BRepAlgoAPI_Fuse(the_shape_a, the_shape_b));

    scm_remember_upto_here_1(shape_a);
    scm_remember_upto_here_1(shape_b);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_translate, 4, 0, 0, (SCM x, SCM y, SCM z, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    gp_Trsf transformation;
    transformation.SetTranslation(gp_Vec(scm_to_double(x), scm_to_double(y), scm_to_double(z)));

    TopoDS_Shape *result_shape = new TopoDS_Shape(
        the_shape.Moved(TopLoc_Location(transformation))
    );

    scm_remember_upto_here_1(shape);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

SCM
exprcad_rotate_radians(const gp_Ax1 &axis, SCM &angle, SCM &shape)
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    gp_Trsf transformation;
    transformation.SetRotation(axis, scm_to_double(angle));

    TopoDS_Shape *result_shape = new TopoDS_Shape(
        the_shape.Moved(TopLoc_Location(transformation))
    );

    scm_remember_upto_here_1(shape);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_rotate_radians_x, 2, 0, 0, (SCM angle, SCM shape))
{
    return exprcad_rotate_radians(gp::OX(), angle, shape);
}

EXPRCAD_DEFINE(exprcad_rotate_radians_y, 2, 0, 0, (SCM angle, SCM shape))
{
    return exprcad_rotate_radians(gp::OY(), angle, shape);
}

EXPRCAD_DEFINE(exprcad_rotate_radians_z, 2, 0, 0, (SCM angle, SCM shape))
{
    return exprcad_rotate_radians(gp::OZ(), angle, shape);
}

EXPRCAD_DEFINE(exprcad_scale_uniformly, 2, 0, 0, (SCM factor, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    gp_Trsf transformation;
    transformation.SetScaleFactor(scm_to_double(factor));

    BRepBuilderAPI_Transform builder(the_shape, transformation, true);

    TopoDS_Shape *result_shape = new TopoDS_Shape(
        builder.Shape()
    );

    scm_remember_upto_here_1(shape);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_axis_mirror, 4, 0, 0, (SCM a_x, SCM a_y, SCM a_z, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    const gp_Dir axis_vector(scm_to_double(a_x), scm_to_double(a_y), scm_to_double(a_z));

    gp_Trsf transformation;
    transformation.SetMirror(gp_Ax1(gp::Origin(), axis_vector));

    BRepBuilderAPI_Transform builder(the_shape, transformation, true);

    TopoDS_Shape *result_shape = new TopoDS_Shape(
        builder.Shape()
    );

    scm_remember_upto_here_1(shape);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_plane_mirror, 4, 0, 0, (SCM n_x, SCM n_y, SCM n_z, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    const gp_Dir normal_vector(scm_to_double(n_x), scm_to_double(n_y), scm_to_double(n_z));

    gp_Trsf transformation;
    transformation.SetMirror(gp_Ax2(gp::Origin(), normal_vector));

    BRepBuilderAPI_Transform builder(the_shape, transformation, true);

    TopoDS_Shape *result_shape = new TopoDS_Shape(
        builder.Shape()
    );

    scm_remember_upto_here_1(shape);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_rectangle, 2, 0, 0, (SCM size_x, SCM size_y))
{
    const double s_x = scm_to_double(size_x);
    const double s_y = scm_to_double(size_y);

    const gp_Pln plane;
    const BRepBuilderAPI_MakeFace face_builder(plane, -0.5 * s_x, 0.5 * s_x, -0.5 * s_y, 0.5 * s_y);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            face_builder.Face()
        )
    );
}

EXPRCAD_DEFINE(exprcad_rounded_rectangle, 3, 0, 0, (SCM size_x, SCM size_y, SCM radius))
{
    const double s_x = scm_to_double(size_x);
    const double s_y = scm_to_double(size_y);

    const gp_Pln plane;
    const BRepBuilderAPI_MakeFace face_builder(plane, -0.5 * s_x, 0.5 * s_x, -0.5 * s_y, 0.5 * s_y);

    BRepFilletAPI_MakeFillet2d fillet_builder(face_builder.Face());

    const double r = scm_to_double(radius);

    TopTools_IndexedMapOfShape vertices_map;
    TopExp::MapShapes(face_builder.Face(), TopAbs_VERTEX, vertices_map);
    for (size_t vertex_index = 0; vertex_index < vertices_map.Extent(); ++vertex_index) {
        const TopoDS_Vertex &vertex = TopoDS::Vertex(vertices_map(vertex_index + 1));
        fillet_builder.AddFillet(vertex, r);
    }

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            fillet_builder.Shape()
        )
    );
}

EXPRCAD_DEFINE(exprcad_disc, 1, 0, 0, (SCM radius))
{
    GC_MakeCircle circle_builder(gp::OZ(), scm_to_double(radius));

    BRepBuilderAPI_MakeEdge edge_builder(circle_builder.Value());

    BRepBuilderAPI_MakeWire wire_builder(edge_builder.Edge());

    const BRepBuilderAPI_MakeFace face_builder(wire_builder.Wire());

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        new TopoDS_Shape(
            face_builder.Face()
        )
    );
}

EXPRCAD_DEFINE(exprcad_extrude, 2, 0, 0, (SCM size_z, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    TopoDS_Shape *result_shape = new TopoDS_Shape(
        BRepPrimAPI_MakePrism(
            the_shape,
            gp_Vec(0, 0, scm_to_double(size_z)),
            true
        )
    );

    scm_remember_upto_here_1(shape);

    return scm_make_foreign_object_1(
        exprcad_type_shape,
        result_shape
    );
}

EXPRCAD_DEFINE(exprcad_bounding_box, 1, 0, 0, (SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    Bnd_Box box;
    BRepBndLib::AddOptimal(the_shape, box);
    double x_min, y_min, z_min, x_max, y_max, z_max;
    box.Get(x_min, y_min, z_min, x_max, y_max, z_max);

    scm_remember_upto_here_1(shape);

    return scm_list_n(
        scm_from_double(x_min),
        scm_from_double(y_min),
        scm_from_double(z_min),
        scm_from_double(x_max),
        scm_from_double(y_max),
        scm_from_double(z_max),
        SCM_UNDEFINED
    );
}

EXPRCAD_DEFINE(exprcad_export_step, 2, 0, 0, (SCM port, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    const TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    // https://dev.opencascade.org/doc/overview/html/occt_user_guides__step.html

    // Write STEP data to a stream instead of a file
    // https://tracker.dev.opencascade.org/view.php?id=0032350

    std::ostringstream stream;

    STEPControl_Writer writer;

    const IFSelect_ReturnStatus status = writer.Transfer(
        the_shape,
        STEPControl_AsIs
    );
    if (status != IFSelect_RetDone) {
        return SCM_BOOL_F;
    }

    Handle(StepData_StepModel) model = writer.Model();
    if (model.IsNull()) {
        return SCM_BOOL_F;
    }

    Handle(StepData_Protocol) protocol = Handle(StepData_Protocol)::DownCast(model->Protocol());
    if (protocol.IsNull()) {
        return SCM_BOOL_F;
    }

    StepData_StepWriter data_writer(model);
    data_writer.SendModel(protocol);
    if (!data_writer.Print(stream)) {
        return SCM_BOOL_F;
    }

    const std::string &stream_data = stream.str();
    scm_c_write(port, stream_data.data(), stream_data.size());

    scm_remember_upto_here_1(shape);

    return SCM_BOOL_T;
}

EXPRCAD_DEFINE(exprcad_export_glb_file, 2, 0, 0, (SCM filename, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    char *the_filename = scm_to_locale_string(filename);

    Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();

    Handle(TDocStd_Document) doc = new TDocStd_Document("XmlOcaf");
    app->InitDocument(doc);

    Handle(XCAFDoc_ShapeTool) shape_tool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
    shape_tool->AddShape(the_shape);

    BRepMesh_IncrementalMesh mesh(the_shape, 1e-3);

    SCM result = SCM_BOOL_T;

    TColStd_IndexedDataMapOfStringString metadata;
    RWGltf_CafWriter writer(the_filename, true);
    writer.ChangeCoordinateSystemConverter().SetInputCoordinateSystem(RWMesh_CoordinateSystem_Zup);
    Message_ProgressRange progress_range;
    if (!writer.Perform(doc, metadata, progress_range)) {
        result = SCM_BOOL_F;
    }

    free(the_filename);

    scm_remember_upto_here_1(shape);

    return result;
}

EXPRCAD_DEFINE(exprcad_export_ascii_stl_file, 2, 0, 0, (SCM filename, SCM shape))
{
    scm_assert_foreign_object_type(exprcad_type_shape, shape);
    TopoDS_Shape &the_shape = *static_cast<TopoDS_Shape *>(scm_foreign_object_ref(shape, 0));

    char *the_filename = scm_to_locale_string(filename);

    BRepMesh_IncrementalMesh mesh(the_shape, 1e-3);

    StlAPI_Writer writer;
    writer.Write(the_shape, the_filename);

    free(the_filename);

    scm_remember_upto_here_1(shape);

    return SCM_BOOL_T;
}


void
exprcad_init_type_shape()
{
    SCM name = scm_from_utf8_symbol("shape");
    SCM slots = scm_list_1(scm_from_utf8_symbol("pointer"));
    exprcad_type_shape = scm_make_foreign_object_type(name, slots, exprcad_finalize_shape);
}

extern "C"
void
exprcad_init()
{
    exprcad_init_type_shape();

#ifndef SCM_MAGIC_SNARFER
#include "exprcad_snarf_init.hxx"
#endif
}
