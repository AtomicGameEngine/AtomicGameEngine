#pragma once

#include <Atomic/Graphics/IndexBuffer.h>
#include <Atomic/Graphics/VertexBuffer.h>
#include <Atomic/Graphics/Geometry.h>
#include <Atomic/Graphics/Model.h>

using namespace Atomic;

namespace ToolCore
{

class MPGeometry;

struct MPVertex
{
    Vector3 position_;
    Vector3 normal_;
    Vector4 tangent_;
    unsigned color_;
    Vector2 uv0_;
    Vector2 uv1_;

    void Clear()
    {
        position_ =  Vector3::ZERO;
        normal_ = Vector3::ZERO;
        tangent_ = Vector4::ZERO;
        uv0_ = Vector2::ZERO;
        uv1_ = Vector2::ZERO;
        color_ = 0;
    }

    bool operator==(const MPVertex& rhs)
    {
        return ( position_ == rhs.position_ &&
                 normal_ == rhs.normal_ &&
                 tangent_ == rhs.tangent_ &&
                 color_ == rhs.color_ &&
                 uv0_ == rhs.uv0_ &&
                 uv1_ == rhs.uv1_ );

    }
};

class MPGeometry : public RefCounted
{
    ATOMIC_REFCOUNTED(MPGeometry)

public:

    SharedPtr<Geometry> geometry_;
    PODVector<MPVertex> vertices_;
    PODVector<VertexElement> elements_;
    SharedArrayPtr<unsigned> indices_;
    unsigned numIndices_;

};

class MPLODLevel : public RefCounted
{
    ATOMIC_REFCOUNTED(MPLODLevel)

public:

    /// Get the total vertex and index counts of all LOD geometry
    void GetTotalCounts(unsigned& totalVertices, unsigned& totalIndices) const;

    /// Returns true if all LOD geometry contains element
    bool HasElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const;

    Vector<SharedPtr<MPGeometry>> mpGeometry_;

    unsigned level_;
};

/// Model packer/unpacker from/to packed representation (destructive!)
class ModelPacker : public Object
{
    ATOMIC_OBJECT(ModelPacker, Object)

public:

    ModelPacker(Context* context);
    virtual ~ModelPacker();

    bool Unpack(Model* model);

    bool Pack();

    const String& GetErrorText() const { return errorText_; }

    Vector<SharedPtr<MPLODLevel>> lodLevels_;

private:

    void SetError(const String& errorText) { errorText_ = errorText; }

    String errorText_;

    bool UnpackLODLevel(unsigned level);

    bool UnpackGeometry(MPLODLevel* level, Geometry* geometry);

    SharedPtr<Model> model_;

};

}
