
#include <ThirdParty/thekla/thekla_atlas.h>

#include <Atomic/IO/Log.h>

#include "ModelPacker.h"
#include "MeshLightmapUVGen.h"


namespace  ToolCore
{


MeshLightmapUVGen::MeshLightmapUVGen(Context* context, Model* model, const String& modelName, const Settings& settings) : Object(context),
    model_(model),
    modelName_(modelName),
    modelPacker_(new ModelPacker(context)),
    settings_(settings),
    tOutputMesh_(0),
    tInputMesh_(0)
{


}

MeshLightmapUVGen::~MeshLightmapUVGen()
{

}

inline void MeshLightmapUVGen::EmitVertex(PODVector<MPVertex>& vertices, unsigned& index, const MPVertex& vertex)
{
    index = 0;

    for (unsigned i = 0; i < vertices.Size(); i++)
    {
        if (vertices[i] == vertex)
        {
            index = i;
            return;
        }
    }

    index = vertices.Size();
    vertices.Push(vertex);
}

void MeshLightmapUVGen::WriteLightmapUVCoords()
{
    String modelName = modelName_;

    if (!modelName.Length())
        modelName = "AnonymousModel";

    Thekla::atlas_write_debug_textures(tOutputMesh_, tInputMesh_, ToString("/Users/jenge/Desktop/%s_lmWorldSpaceTexture.png", modelName.CString()).CString() ,
                                                                  ToString("/Users/jenge/Desktop/%s_lmNormalTexture.png", modelName.CString()).CString() );
    Vector<PODVector<MPVertex>> geoVerts;
    Vector<PODVector<unsigned>> geoIndices;

    geoVerts.Resize(curLOD_->mpGeometry_.Size());
    geoIndices.Resize(curLOD_->mpGeometry_.Size());

    float uscale = 1.f / tOutputMesh_->atlas_width;
    float vscale = 1.f / tOutputMesh_->atlas_height;

    for (unsigned i = 0; i < tOutputMesh_->index_count/3; i++)
    {
        unsigned v0 = (unsigned) tOutputMesh_->index_array[i * 3];
        unsigned v1 = (unsigned) tOutputMesh_->index_array[i * 3 + 1];
        unsigned v2 = (unsigned) tOutputMesh_->index_array[i * 3 + 2];

        Thekla::Atlas_Output_Vertex& tv0 = tOutputMesh_->vertex_array[v0];
        Thekla::Atlas_Output_Vertex& tv1 = tOutputMesh_->vertex_array[v1];
        Thekla::Atlas_Output_Vertex& tv2 = tOutputMesh_->vertex_array[v2];

        LMVertex& lv0 = lmVertices_[tv0.xref];
        LMVertex& lv1 = lmVertices_[tv1.xref];
        LMVertex& lv2 = lmVertices_[tv2.xref];

        unsigned geometryIdx = lv0.geometryIdx_;

        // check for mixed geometry in triangle
        if (geometryIdx != lv1.geometryIdx_ || geometryIdx != lv2.geometryIdx_)
        {
            assert(0);
        }

        MPGeometry* mpGeo = curLOD_->mpGeometry_[geometryIdx];

        PODVector<MPVertex>& verts = geoVerts[geometryIdx];
        PODVector<unsigned>& indices = geoIndices[geometryIdx];

        unsigned index;

        MPVertex mpv = mpGeo->vertices_[lv0.originalVertex_];
        mpv.uv1_ = Vector2(tv0.uv[0] * uscale,  tv0.uv[1] * vscale);
        EmitVertex(verts, index, mpv);
        indices.Push(index);

        mpv = mpGeo->vertices_[lv1.originalVertex_];
        mpv.uv1_ = Vector2(tv1.uv[0] * uscale,  tv1.uv[1] * vscale);
        EmitVertex(verts, index, mpv);
        indices.Push(index);

        mpv = mpGeo->vertices_[lv2.originalVertex_];
        mpv.uv1_ = Vector2(tv2.uv[0] * uscale,  tv2.uv[1] * vscale);
        EmitVertex(verts, index, mpv);
        indices.Push(index);

    }

    for (unsigned i = 0; i < curLOD_->mpGeometry_.Size(); i++)
    {
        MPGeometry* mpGeo = curLOD_->mpGeometry_[i];

        mpGeo->vertices_ = geoVerts[i];
        mpGeo->indices_ = new unsigned[geoIndices[i].Size()];
        memcpy(&mpGeo->indices_[0], &geoIndices[i][0], sizeof(unsigned) * geoIndices[i].Size());
        mpGeo->numIndices_ = geoIndices[i].Size();

        // Check whether we need to add UV1 semantic

        PODVector<VertexElement> nElements;

        unsigned texCoordCount = 0;
        for (unsigned j = 0; j < mpGeo->elements_.Size(); j++)
        {
            VertexElement element = mpGeo->elements_[j];

            if (element.type_ == TYPE_VECTOR2 && element.semantic_ == SEM_TEXCOORD)
                texCoordCount++;
        }

        if (texCoordCount == 0)
        {
            // We don't have a valid UV set in UV0
            mpGeo->elements_.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD, 0));
            mpGeo->elements_.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD, 1));
        }
        else if (texCoordCount == 1)
        {
            bool added = false;
            for (unsigned j = 0; j < mpGeo->elements_.Size(); j++)
            {
                VertexElement element = mpGeo->elements_[j];

                nElements.Push(element);

                if ( (element.type_ == TYPE_VECTOR2 && element.semantic_ == SEM_TEXCOORD) || (!added && j == (mpGeo->elements_.Size() - 1) ) )
                {
                    added = true;
                    VertexElement element(TYPE_VECTOR2, SEM_TEXCOORD, 1);
                    nElements.Push(element);
                }

            }

            mpGeo->elements_ = nElements;
        }

    }

}

bool MeshLightmapUVGen::Generate()
{
    if (model_.Null())
        return false;    

    if (!modelPacker_->Unpack(model_))
    {
        return false;
    }

    for (unsigned i = 0; i < modelPacker_->lodLevels_.Size(); i++)
    {

        curLOD_ = modelPacker_->lodLevels_[i];

        // combine all LOD vertices/indices

        unsigned totalVertices = 0;
        unsigned totalIndices = 0;
        for (unsigned j = 0; j < curLOD_->mpGeometry_.Size(); j++)
        {
            MPGeometry* geo = curLOD_->mpGeometry_[j];
            totalVertices += geo->vertices_.Size();
            totalIndices += geo->numIndices_;
        }

        // Setup thekla input mesh
        tInputMesh_ = new Thekla::Atlas_Input_Mesh();

        // Allocate vertex arrays
        lmVertices_ = new LMVertex[totalVertices];

        tInputMesh_->vertex_count = totalVertices;
        tInputMesh_->vertex_array = new Thekla::Atlas_Input_Vertex[tInputMesh_->vertex_count];

        tInputMesh_->face_count = totalIndices / 3;
        tInputMesh_->face_array = new Thekla::Atlas_Input_Face[tInputMesh_->face_count];

        unsigned vCount = 0;
        unsigned fCount = 0;

        for (unsigned j = 0; j < curLOD_->mpGeometry_.Size(); j++)
        {
            MPGeometry* geo = curLOD_->mpGeometry_[j];

            unsigned vertexStart = vCount;

            for (unsigned k = 0; k < geo->vertices_.Size(); k++, vCount++)
            {
                const MPVertex& mpv = geo->vertices_[k];

                LMVertex& lmv = lmVertices_[vCount];
                Thekla::Atlas_Input_Vertex& tv = tInputMesh_->vertex_array[vCount];

                lmv.geometry_ = geo;
                lmv.geometryIdx_ = j;
                lmv.originalVertex_ = k;

                tv.position[0] = mpv.position_.x_;
                tv.position[1] = mpv.position_.y_;
                tv.position[2] = mpv.position_.z_;

                tv.normal[0] = mpv.normal_.x_;
                tv.normal[1] = mpv.normal_.y_;
                tv.normal[2] = mpv.normal_.z_;

                tv.uv[0] = mpv.uv0_.x_;
                tv.uv[1] = mpv.uv0_.y_;

                // this appears unused in thekla atlas?
                tv.first_colocal = vCount;

            }

            for (unsigned k = 0; k < geo->numIndices_/3; k++, fCount++)
            {
                Thekla::Atlas_Input_Face& tface = tInputMesh_->face_array[fCount];

                tface.vertex_index[0] = (int) (geo->indices_[k * 3] + vertexStart);
                tface.vertex_index[1] = (int) (geo->indices_[k * 3 + 1] + vertexStart);
                tface.vertex_index[2] = (int) (geo->indices_[k * 3 + 2] + vertexStart);

                if (tface.vertex_index[0] > totalVertices || tface.vertex_index[1] > totalVertices || tface.vertex_index[2] > totalVertices)
                {
                    ATOMIC_LOGERROR("Vertex overflow");
                    return false;
                }

                tface.material_index = j;

            }

        }

        Thekla::Atlas_Options atlasOptions;
        atlas_set_default_options(&atlasOptions);
        atlasOptions.packer_options.witness.texel_area = 32;
        Thekla::Atlas_Error error = Thekla::Atlas_Error_Success;

        tOutputMesh_ = atlas_generate(tInputMesh_, &atlasOptions, &error);

        if (tOutputMesh_)
        {
            WriteLightmapUVCoords();
        }

        delete [] tInputMesh_->vertex_array;
        delete [] tInputMesh_->face_array;
        delete tInputMesh_;
        tInputMesh_ = 0;

        atlas_free(tOutputMesh_);
        tOutputMesh_ = 0;

    }

    // update model
    modelPacker_->Pack();

    return true;

}

}
