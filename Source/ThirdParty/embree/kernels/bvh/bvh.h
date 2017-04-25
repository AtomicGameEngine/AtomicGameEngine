// ======================================================================== //
// Copyright 2009-2017 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "../common/default.h"
#include "../common/alloc.h"
#include "../common/accel.h"
#include "../common/device.h"
#include "../common/scene.h"
#include "../geometry/primitive.h"
#include "../common/ray.h"

namespace embree
{
  /*! flags used to enable specific node types in intersectors */
  enum BVHNodeFlags
  {
    BVH_FLAG_ALIGNED_NODE = 0x00001,
    BVH_FLAG_ALIGNED_NODE_MB = 0x00010,
    BVH_FLAG_UNALIGNED_NODE = 0x00100,
    BVH_FLAG_UNALIGNED_NODE_MB = 0x01000,
    BVH_FLAG_TRANSFORM_NODE = 0x10000,
    BVH_FLAG_QUANTIZED_NODE = 0x100000,

    /* short versions */
    BVH_AN1 = BVH_FLAG_ALIGNED_NODE,
    BVH_AN2 = BVH_FLAG_ALIGNED_NODE_MB,
    BVH_UN1 = BVH_FLAG_UNALIGNED_NODE,
    BVH_UN2 = BVH_FLAG_UNALIGNED_NODE_MB,
    BVH_MB = BVH_FLAG_ALIGNED_NODE_MB | BVH_FLAG_UNALIGNED_NODE_MB,
    BVH_AN1_UN1 = BVH_FLAG_ALIGNED_NODE | BVH_FLAG_UNALIGNED_NODE,
    BVH_AN2_UN2 = BVH_FLAG_ALIGNED_NODE_MB | BVH_FLAG_UNALIGNED_NODE_MB,
    BVH_TN_AN1 = BVH_FLAG_TRANSFORM_NODE | BVH_FLAG_ALIGNED_NODE,
    BVH_TN_AN1_AN2 = BVH_FLAG_TRANSFORM_NODE | BVH_FLAG_ALIGNED_NODE | BVH_FLAG_ALIGNED_NODE_MB,
    BVH_QN1 = BVH_FLAG_QUANTIZED_NODE
  };

  /*! Multi BVH with N children. Each node stores the bounding box of
   * it's N children as well as N child references. */
  template<int N>
  class BVHN : public AccelData
  {
    ALIGNED_CLASS;
  public:

    /*! forward declaration of node type */
    struct NodeRef;
    struct BaseNode;
    struct AlignedNode;
    struct AlignedNodeMB;
    struct UnalignedNode;
    struct UnalignedNodeMB;
    struct TransformNode;
    struct QuantizedNode;

    /*! Number of bytes the nodes and primitives are minimally aligned to.*/
    static const size_t byteAlignment = 16;
    static const size_t byteNodeAlignment = 4*N;

    /*! highest address bit is used as barrier for some algorithms */
    static const size_t barrier_mask = (1LL << (8*sizeof(size_t)-1));

    /*! Masks the bits that store the number of items per leaf. */
    static const size_t align_mask = byteAlignment-1;
    static const size_t items_mask = byteAlignment-1;

    /*! different supported node types */
    static const size_t tyAlignedNode = 0;
    static const size_t tyAlignedNodeMB = 1;
    static const size_t tyUnalignedNode = 2;
    static const size_t tyUnalignedNodeMB = 3;
    static const size_t tyTransformNode = 4;
    static const size_t tyQuantizedNode = 5;
    static const size_t tyLeaf = 8;

    /*! Empty node */
    static const size_t emptyNode = tyLeaf;

    /*! Invalid node, used as marker in traversal */
    static const size_t invalidNode = (((size_t)-1) & (~items_mask)) | (tyLeaf+0);
    static const size_t popRay      = (((size_t)-1) & (~items_mask)) | (tyLeaf+1);

    /*! Maximal depth of the BVH. */
    static const size_t maxBuildDepth = 32;
    static const size_t maxBuildDepthLeaf = maxBuildDepth+8;
    static const size_t maxDepth = maxBuildDepth+maxBuildDepthLeaf;

    /*! Maximal number of primitive blocks in a leaf. */
    static const size_t maxLeafBlocks = items_mask-tyLeaf;

  private:

    /*! Shortcuts */
    typedef Vec3<vfloat<N>> Vec3vfN;
    typedef BBox<Vec3vfN> BBox3vfN;
    typedef AffineSpaceT<LinearSpace3<Vec3vfN>> AffineSpace3vfN;

  public:

    /*! Builder interface to create allocator */
    struct CreateAlloc
    {
    public:
      __forceinline CreateAlloc (BVHN* bvh) : bvh(bvh) {}
      __forceinline FastAllocator::ThreadLocal2* operator() () const { return bvh->alloc.threadLocal2();  }

    private:
      BVHN* bvh;
    };

    /*! Pointer that points to a node or a list of primitives */
    struct NodeRef
    {
      /*! Default constructor */
      __forceinline NodeRef () {}

      /*! Construction from integer */
      __forceinline NodeRef (size_t ptr) : ptr(ptr) {}

      /*! Cast to size_t */
      __forceinline operator size_t() const { return ptr; }

      /*! Prefetches the node this reference points to */
      __forceinline void prefetch(int types=0) const {
#if defined(__AVX512PF__) // MIC
          if (types != BVH_FLAG_QUANTIZED_NODE) {
            prefetchL2(((char*)ptr)+0*64);
            prefetchL2(((char*)ptr)+1*64);
            if ((N >= 8) || (types > BVH_FLAG_ALIGNED_NODE)) {
              prefetchL2(((char*)ptr)+2*64);
              prefetchL2(((char*)ptr)+3*64);
            }
            if ((N >= 8) && (types > BVH_FLAG_ALIGNED_NODE)) {
              prefetchL2(((char*)ptr)+4*64);
              prefetchL2(((char*)ptr)+5*64);
              prefetchL2(((char*)ptr)+6*64);
              prefetchL2(((char*)ptr)+7*64);
            }
          }
          else
          {
            /* todo: reduce if 32bit offsets are enabled */
            prefetchL2(((char*)ptr)+0*64);
            prefetchL2(((char*)ptr)+1*64);
            prefetchL2(((char*)ptr)+2*64);
          }
#else
          if (types != BVH_FLAG_QUANTIZED_NODE) {
            prefetchL1(((char*)ptr)+0*64);
            prefetchL1(((char*)ptr)+1*64);
            if ((N >= 8) || (types > BVH_FLAG_ALIGNED_NODE)) {
              prefetchL1(((char*)ptr)+2*64);
              prefetchL1(((char*)ptr)+3*64);
            }
            if ((N >= 8) && (types > BVH_FLAG_ALIGNED_NODE)) {
              prefetchL1(((char*)ptr)+4*64);
              prefetchL1(((char*)ptr)+5*64);
              prefetchL1(((char*)ptr)+6*64);
              prefetchL1(((char*)ptr)+7*64);
            }
          }
          else
          {
            /* todo: reduce if 32bit offsets are enabled */
            prefetchL1(((char*)ptr)+0*64);
            prefetchL1(((char*)ptr)+1*64);
            prefetchL1(((char*)ptr)+2*64);
          }
#endif
      }

      __forceinline void prefetchLLC(int types=0) const {
        embree::prefetchL2(((char*)ptr)+0*64);
        embree::prefetchL2(((char*)ptr)+1*64);
        if (types != BVH_FLAG_QUANTIZED_NODE) {
          if ((N >= 8) || (types > BVH_FLAG_ALIGNED_NODE)) {
            embree::prefetchL2(((char*)ptr)+2*64);
            embree::prefetchL2(((char*)ptr)+3*64);
          }
          if ((N >= 8) && (types > BVH_FLAG_ALIGNED_NODE)) {
            embree::prefetchL2(((char*)ptr)+4*64);
            embree::prefetchL2(((char*)ptr)+5*64);
            embree::prefetchL2(((char*)ptr)+6*64);
            embree::prefetchL2(((char*)ptr)+7*64);
          }
        }
      }

      __forceinline void prefetch_L1(int types=0) const {
        embree::prefetchL1(((char*)ptr)+0*64);
        embree::prefetchL1(((char*)ptr)+1*64);
        if (types != BVH_FLAG_QUANTIZED_NODE) {
          if ((N >= 8) || (types > BVH_FLAG_ALIGNED_NODE)) {
            embree::prefetchL1(((char*)ptr)+2*64);
            embree::prefetchL1(((char*)ptr)+3*64);
          }
          if ((N >= 8) && (types > BVH_FLAG_ALIGNED_NODE)) {
            embree::prefetchL1(((char*)ptr)+4*64);
            embree::prefetchL1(((char*)ptr)+5*64);
            embree::prefetchL1(((char*)ptr)+6*64);
            embree::prefetchL1(((char*)ptr)+7*64);
          }
        }
      }

      __forceinline void prefetch_L2(int types=0) const {
        embree::prefetchL2(((char*)ptr)+0*64);
        embree::prefetchL2(((char*)ptr)+1*64);
        if (types != BVH_FLAG_QUANTIZED_NODE) {
          if ((N >= 8) || (types > BVH_FLAG_ALIGNED_NODE)) {
            embree::prefetchL2(((char*)ptr)+2*64);
            embree::prefetchL2(((char*)ptr)+3*64);
          }
          if ((N >= 8) && (types > BVH_FLAG_ALIGNED_NODE)) {
            embree::prefetchL2(((char*)ptr)+4*64);
            embree::prefetchL2(((char*)ptr)+5*64);
            embree::prefetchL2(((char*)ptr)+6*64);
            embree::prefetchL2(((char*)ptr)+7*64);
          }
        }
      }


      __forceinline void prefetchW(int types=0) const {
        embree::prefetchEX(((char*)ptr)+0*64);
        embree::prefetchEX(((char*)ptr)+1*64);
        if ((N >= 8) || (types > BVH_FLAG_ALIGNED_NODE)) {
          embree::prefetchEX(((char*)ptr)+2*64);
          embree::prefetchEX(((char*)ptr)+3*64);
        }
        if ((N >= 8) && (types > BVH_FLAG_ALIGNED_NODE)) {
          embree::prefetchEX(((char*)ptr)+4*64);
          embree::prefetchEX(((char*)ptr)+5*64);
          embree::prefetchEX(((char*)ptr)+6*64);
          embree::prefetchEX(((char*)ptr)+7*64);
        }
      }

      /*! Sets the barrier bit. */
      __forceinline void setBarrier() { assert(!isBarrier()); ptr |= barrier_mask; }

      /*! Clears the barrier bit. */
      __forceinline void clearBarrier() { ptr &= ~barrier_mask; }

      /*! Checks if this is an barrier. A barrier tells the top level tree rotations how deep to enter the tree. */
      __forceinline bool isBarrier() const { return (ptr & barrier_mask) != 0; }

      /*! checks if this is a leaf */
      __forceinline size_t isLeaf() const { return ptr & tyLeaf; }

      /*! returns node type */
      __forceinline int type() const { return ptr & (size_t)align_mask; }

      /*! checks if this is a node */
      __forceinline int isAlignedNode() const { return (ptr & (size_t)align_mask) == tyAlignedNode; }

      /*! checks if this is a motion blur node */
      __forceinline int isAlignedNodeMB() const { return (ptr & (size_t)align_mask) == tyAlignedNodeMB; }

      /*! checks if this is a node with unaligned bounding boxes */
      __forceinline int isUnalignedNode() const { return (ptr & (size_t)align_mask) == tyUnalignedNode; }

      /*! checks if this is a motion blur node with unaligned bounding boxes */
      __forceinline int isUnalignedNodeMB() const { return (ptr & (size_t)align_mask) == tyUnalignedNodeMB; }

      /*! checks if this is a transformation node */
      __forceinline int isTransformNode() const { return (ptr & (size_t)align_mask) == tyTransformNode; }
      __forceinline int isTransformNode(int types) const { return (types == BVH_FLAG_TRANSFORM_NODE) || ((types & BVH_FLAG_TRANSFORM_NODE) && isTransformNode()); }

      /*! checks if this is a quantized node */
      __forceinline int isQuantizedNode() const { return (ptr & (size_t)align_mask) == tyQuantizedNode; }

      /*! returns base node pointer */
      __forceinline BaseNode* baseNode(int types)
      {
        assert(!isLeaf());
        if ((types & ~BVH_FLAG_TRANSFORM_NODE) == BVH_FLAG_ALIGNED_NODE) {
          assert((ptr & (size_t)align_mask) == 0);
          return (BaseNode*)ptr;
        }
        else
          return (BaseNode*)(ptr & ~(size_t)align_mask);
      }
      __forceinline const BaseNode* baseNode(int types) const
      {
        assert(!isLeaf());
        if ((types & ~BVH_FLAG_TRANSFORM_NODE) == BVH_FLAG_ALIGNED_NODE) {
          assert((ptr & (size_t)align_mask) == 0);
          return (const BaseNode*)ptr;
        }
        else
          return (const BaseNode*)(ptr & ~(size_t)align_mask);
      }

      /*! returns node pointer */
      __forceinline       AlignedNode* alignedNode()       { assert(isAlignedNode()); return (      AlignedNode*)ptr; }
      __forceinline const AlignedNode* alignedNode() const { assert(isAlignedNode()); return (const AlignedNode*)ptr; }

      /*! returns motion blur node pointer */
      __forceinline       AlignedNodeMB* alignedNodeMB()       { assert(isAlignedNodeMB()); return (      AlignedNodeMB*)(ptr & ~(size_t)align_mask); }
      __forceinline const AlignedNodeMB* alignedNodeMB() const { assert(isAlignedNodeMB()); return (const AlignedNodeMB*)(ptr & ~(size_t)align_mask); }

      /*! returns unaligned node pointer */
      __forceinline       UnalignedNode* unalignedNode()       { assert(isUnalignedNode()); return (      UnalignedNode*)(ptr & ~(size_t)align_mask); }
      __forceinline const UnalignedNode* unalignedNode() const { assert(isUnalignedNode()); return (const UnalignedNode*)(ptr & ~(size_t)align_mask); }

      /*! returns unaligned motion blur node pointer */
      __forceinline       UnalignedNodeMB* unalignedNodeMB()       { assert(isUnalignedNodeMB()); return (      UnalignedNodeMB*)(ptr & ~(size_t)align_mask); }
      __forceinline const UnalignedNodeMB* unalignedNodeMB() const { assert(isUnalignedNodeMB()); return (const UnalignedNodeMB*)(ptr & ~(size_t)align_mask); }

      /*! returns transformation pointer */
      __forceinline       TransformNode* transformNode()       { assert(isTransformNode()); return (      TransformNode*)(ptr & ~(size_t)align_mask); }
      __forceinline const TransformNode* transformNode() const { assert(isTransformNode()); return (const TransformNode*)(ptr & ~(size_t)align_mask); }

      /*! returns quantized node pointer */
      __forceinline       QuantizedNode* quantizedNode()       { assert(isQuantizedNode()); return (      QuantizedNode*)(ptr & ~(size_t)align_mask); }
      __forceinline const QuantizedNode* quantizedNode() const { assert(isQuantizedNode()); return (const QuantizedNode*)(ptr & ~(size_t)align_mask); }

      /*! returns leaf pointer */
      __forceinline char* leaf(size_t& num) const {
        assert(isLeaf());
        num = (ptr & (size_t)items_mask)-tyLeaf;
        return (char*)(ptr & ~(size_t)align_mask);
      }

      /*! clear all bit flags */
      __forceinline void clearFlags() {
        ptr &= ~(size_t)align_mask;
      }

    private:
      size_t ptr;
    };

    /*! BVHN Base Node */
    struct BaseNode
    {
      /*! Clears the node. */
      __forceinline void clear() {
        for (size_t i=0; i<N; i++) children[i] = emptyNode;
      }

      /*! Returns reference to specified child */
      __forceinline       NodeRef& child(size_t i)       { assert(i<N); return children[i]; }
      __forceinline const NodeRef& child(size_t i) const { assert(i<N); return children[i]; }

      /*! verifies the node */
      __forceinline bool verify() const
      {
        for (size_t i=0; i<N; i++) {
          if (child(i) == BVHN::emptyNode) {
            for (; i<N; i++) {
              if (child(i) != BVHN::emptyNode)
                return false;
            }
            break;
          }
        }
        return true;
      }

      NodeRef children[N];    //!< Pointer to the N children (can be a node or leaf)
    };

    /*! BVHN AlignedNode */
    struct AlignedNode : public BaseNode
    {
      using BaseNode::children;

      struct Create
      {
        __forceinline NodeRef operator() (FastAllocator::ThreadLocal2* alloc, size_t numChildren = 0) const
        {
          AlignedNode* node = (AlignedNode*) alloc->alloc0->malloc(sizeof(AlignedNode),byteNodeAlignment); node->clear();
          return BVHN::encodeNode(node);
        }
      };

      struct Set
      {
        __forceinline void operator() (NodeRef node, size_t i, NodeRef child, const BBox3fa& bounds) const {
          node.alignedNode()->set(i,child);
          node.alignedNode()->set(i,bounds);
        }
      };

      struct Create2
      {
        template<typename BuildRecord>
        __forceinline NodeRef operator() (BuildRecord* children, const size_t num, FastAllocator::ThreadLocal2* alloc) const
        {
          AlignedNode* node = (AlignedNode*) alloc->alloc0->malloc(sizeof(AlignedNode), byteNodeAlignment); node->clear();
          for (size_t i=0; i<num; i++) node->set(i,children[i].bounds());
          return encodeNode(node);
        }
      };

      struct Set2
      {
        template<typename BuildRecord>
        __forceinline NodeRef operator() (const BuildRecord& precord, const BuildRecord* crecords, NodeRef ref, NodeRef* children, const size_t num) const
        {
          AlignedNode* node = ref.alignedNode();
          for (size_t i=0; i<num; i++) node->set(i,children[i]);
          return ref;
        }
      };

      struct Set3
      {
        Set3 (FastAllocator* allocator, PrimRef* prims)
        : allocator(allocator), prims(prims) {}

        template<typename BuildRecord>
        __forceinline NodeRef operator() (const BuildRecord& precord, const BuildRecord* crecords, NodeRef ref, NodeRef* children, const size_t num) const
        {
          AlignedNode* node = ref.alignedNode();
          for (size_t i=0; i<num; i++) node->set(i,children[i]);
#if 0
          const size_t threshold = 10000;
          if (precord.prims.size() > threshold) 
          {
            for (size_t i=0; i<num; i++) {
              if (crecords[i].prims.size() > threshold) continue;
              PrimRef* begin = &prims[crecords[i].prims.begin()];
              PrimRef* end   = &prims[crecords[i].prims.end()]; // FIXME: extended end for spatial split builder!!!!!
              size_t bytes = (size_t)end - (size_t)begin;
              allocator->addBlock(begin,bytes);
            }
          }
#endif
          return ref;
        }

        FastAllocator* const allocator;
        PrimRef* const prims;
      };

      /*! Clears the node. */
      __forceinline void clear() {
        lower_x = lower_y = lower_z = pos_inf;
        upper_x = upper_y = upper_z = neg_inf;
        BaseNode::clear();
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, const NodeRef& childID) {
        assert(i < N);
        children[i] = childID;
      }

      /*! Sets bounding box of child. */
      __forceinline void set(size_t i, const BBox3fa& bounds)
      {
        assert(i < N);
        lower_x[i] = bounds.lower.x; lower_y[i] = bounds.lower.y; lower_z[i] = bounds.lower.z;
        upper_x[i] = bounds.upper.x; upper_y[i] = bounds.upper.y; upper_z[i] = bounds.upper.z;
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, const BBox3fa& bounds, const NodeRef& childID) {
        set(i,bounds);
        children[i] = childID;
      }

      /*! Returns bounds of node. */
      __forceinline BBox3fa bounds() const {
        const Vec3fa lower(reduce_min(lower_x),reduce_min(lower_y),reduce_min(lower_z));
        const Vec3fa upper(reduce_max(upper_x),reduce_max(upper_y),reduce_max(upper_z));
        return BBox3fa(lower,upper);
      }

      /*! Returns bounds of specified child. */
      __forceinline BBox3fa bounds(size_t i) const
      {
        assert(i < N);
        const Vec3fa lower(lower_x[i],lower_y[i],lower_z[i]);
        const Vec3fa upper(upper_x[i],upper_y[i],upper_z[i]);
        return BBox3fa(lower,upper);
      }

      /*! Returns extent of bounds of specified child. */
      __forceinline Vec3fa extend(size_t i) const {
        return bounds(i).size();
      }

      /*! Returns bounds of all children (implemented later as specializations) */
      __forceinline void bounds(BBox<vfloat4>& bounds0, BBox<vfloat4>& bounds1, BBox<vfloat4>& bounds2, BBox<vfloat4>& bounds3) const {} // N = 4

      /*! swap two children of the node */
      __forceinline void swap(size_t i, size_t j)
      {
        assert(i<N && j<N);
        std::swap(children[i],children[j]);
        std::swap(lower_x[i],lower_x[j]);
        std::swap(lower_y[i],lower_y[j]);
        std::swap(lower_z[i],lower_z[j]);
        std::swap(upper_x[i],upper_x[j]);
        std::swap(upper_y[i],upper_y[j]);
        std::swap(upper_z[i],upper_z[j]);
      }

      /*! Returns reference to specified child */
      __forceinline       NodeRef& child(size_t i)       { assert(i<N); return children[i]; }
      __forceinline const NodeRef& child(size_t i) const { assert(i<N); return children[i]; }

      /*! output operator */
      friend std::ostream& operator<<(std::ostream& o, const AlignedNode& n)
      {
        o << "AlignedNode { " << std::endl;
        o << "  lower_x " << n.lower_x << std::endl;
        o << "  upper_x " << n.upper_x << std::endl;
        o << "  lower_y " << n.lower_y << std::endl;
        o << "  upper_y " << n.upper_y << std::endl;
        o << "  lower_z " << n.lower_z << std::endl;
        o << "  upper_z " << n.upper_z << std::endl;
        o << "  children = ";
        for (size_t i=0; i<N; i++) o << n.children[i] << " ";
        o << std::endl;
        o << "}" << std::endl;
        return o;
      }

    public:
      vfloat<N> lower_x;           //!< X dimension of lower bounds of all N children.
      vfloat<N> upper_x;           //!< X dimension of upper bounds of all N children.
      vfloat<N> lower_y;           //!< Y dimension of lower bounds of all N children.
      vfloat<N> upper_y;           //!< Y dimension of upper bounds of all N children.
      vfloat<N> lower_z;           //!< Z dimension of lower bounds of all N children.
      vfloat<N> upper_z;           //!< Z dimension of upper bounds of all N children.
    };

    /*! Motion Blur AlignedNode */
    struct AlignedNodeMB : public BaseNode
    {
      using BaseNode::children;

      struct Create
      {
        __forceinline NodeRef operator() (FastAllocator::ThreadLocal2* alloc) const
        {
          AlignedNodeMB* node = (AlignedNodeMB*) alloc->alloc0->malloc(sizeof(AlignedNodeMB),byteNodeAlignment); node->clear();
          return BVHN::encodeNode(node);
        }
      };

      struct Set
      {
        __forceinline void operator() (NodeRef node, size_t i, const std::tuple<NodeRef,LBBox3fa,BBox1f>& child) const {
          node.alignedNodeMB()->set(i,child);
        }
      };

      struct Create2
      {
        template<typename BuildRecord>
        __forceinline NodeRef operator() (BuildRecord* children, const size_t num, FastAllocator::ThreadLocal2* alloc) const
        {
          AlignedNodeMB* node = (AlignedNodeMB*) alloc->alloc0->malloc(sizeof(AlignedNodeMB),byteNodeAlignment); node->clear();
          return encodeNode(node);
        }
      };

      struct Set2
      {
        typedef std::pair<NodeRef,LBBox3fa> Ty;
        
        template<typename BuildRecord>
        __forceinline Ty operator() (const BuildRecord& precord, const BuildRecord* crecords, NodeRef ref, Ty* children, const size_t num) const
        {
          AlignedNodeMB* node = ref.alignedNodeMB();
          
          LBBox3fa bounds = empty;
          for (size_t i=0; i<num; i++) {
            node->set(i,children[i].first);
            node->set(i,children[i].second);
            bounds.extend(children[i].second);
          }
          return std::make_pair(ref,bounds);
        }
      };

      /*! Clears the node. */
      __forceinline void clear()  {
        lower_x = lower_y = lower_z = vfloat<N>(nan);
        upper_x = upper_y = upper_z = vfloat<N>(nan);
        lower_dx = lower_dy = lower_dz = vfloat<N>(nan); // initialize with NAN and update during refit
        upper_dx = upper_dy = upper_dz = vfloat<N>(nan);
        BaseNode::clear();
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, const BBox3fa& bounds, NodeRef childID) {
        lower_x[i] = bounds.lower.x; lower_y[i] = bounds.lower.y; lower_z[i] = bounds.lower.z;
        upper_x[i] = bounds.upper.x; upper_y[i] = bounds.upper.y; upper_z[i] = bounds.upper.z;
        children[i] = childID;
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, NodeRef childID) {
        children[i] = childID;
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, const BBox3fa& bounds) {
        lower_x[i] = bounds.lower.x; lower_y[i] = bounds.lower.y; lower_z[i] = bounds.lower.z;
        upper_x[i] = bounds.upper.x; upper_y[i] = bounds.upper.y; upper_z[i] = bounds.upper.z;
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, const BBox3fa& bounds0_i, const BBox3fa& bounds1_i)
      {
        /*! for empty bounds we have to avoid inf-inf=nan */
        const BBox3fa bounds0(min(bounds0_i.lower,Vec3fa(+FLT_MAX)),max(bounds0_i.upper,Vec3fa(-FLT_MAX)));
        const BBox3fa bounds1(min(bounds1_i.lower,Vec3fa(+FLT_MAX)),max(bounds1_i.upper,Vec3fa(-FLT_MAX)));

        lower_x[i] = bounds0.lower.x; lower_y[i] = bounds0.lower.y; lower_z[i] = bounds0.lower.z;
        upper_x[i] = bounds0.upper.x; upper_y[i] = bounds0.upper.y; upper_z[i] = bounds0.upper.z;

        /*! standard case */
        const Vec3fa dlower = bounds1.lower-bounds0.lower;
        const Vec3fa dupper = bounds1.upper-bounds0.upper;
        lower_dx[i] = dlower.x; lower_dy[i] = dlower.y; lower_dz[i] = dlower.z;
        upper_dx[i] = dupper.x; upper_dy[i] = dupper.y; upper_dz[i] = dupper.z;
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, const LBBox3fa& bounds) {
        set(i, bounds.bounds0, bounds.bounds1);
      }

      /*! Sets bounding box and ID of child. */
      __forceinline void set(size_t i, const std::tuple<NodeRef,LBBox3fa,BBox1f>& child) 
      {
        set(i,std::get<0>(child));
        set(i, std::get<1>(child).global(std::get<2>(child)));
      }

      /*! tests if the node has valid bounds */
      __forceinline bool hasBounds() const {
        return lower_dx.i[0] != cast_f2i(float(nan));
      }

      /*! Return bounding box for time 0 */
      __forceinline BBox3fa bounds0(size_t i) const {
        return BBox3fa(Vec3fa(lower_x[i],lower_y[i],lower_z[i]),
                       Vec3fa(upper_x[i],upper_y[i],upper_z[i]));
      }

      /*! Return bounding box for time 1 */
      __forceinline BBox3fa bounds1(size_t i) const {
        return BBox3fa(Vec3fa(lower_x[i]+lower_dx[i],lower_y[i]+lower_dy[i],lower_z[i]+lower_dz[i]),
                       Vec3fa(upper_x[i]+upper_dx[i],upper_y[i]+upper_dy[i],upper_z[i]+upper_dz[i]));
      }

      /*! Returns bounds of node. */
      __forceinline BBox3fa bounds() const {
        return BBox3fa(Vec3fa(reduce_min(min(lower_x,lower_x+lower_dx)),
                             reduce_min(min(lower_y,lower_y+lower_dy)),
                             reduce_min(min(lower_z,lower_z+lower_dz))),
                      Vec3fa(reduce_max(max(upper_x,upper_x+upper_dx)),
                             reduce_max(max(upper_y,upper_y+upper_dy)),
                             reduce_max(max(upper_z,upper_z+upper_dz))));
      }

      /*! Return bounding box of child i */
      __forceinline BBox3fa bounds(size_t i) const {
        return merge(bounds0(i),bounds1(i));
      }

      /*! Return linear bounding box of child i */
      __forceinline LBBox3fa lbounds(size_t i) const {
        return LBBox3fa(bounds0(i),bounds1(i));
      }

      /*! Return bounding box of child i at specified time */
      __forceinline BBox3fa bounds(size_t i, float time) const {
        return lerp(bounds0(i),bounds1(i),time);
      }

      /*! Returns the expected surface area when randomly sampling the time. */
      __forceinline float expectedHalfArea(size_t i) const {
        return lbounds(i).expectedHalfArea();
      }

      /*! Returns the expected surface area when randomly sampling the time. */
      __forceinline float expectedHalfArea(size_t i, const BBox1f& t0t1) const {
        return lbounds(i).expectedHalfArea(t0t1); 
      }

      /*! swap two children of the node */
      __forceinline void swap(size_t i, size_t j)
      {
        assert(i<N && j<N);
        std::swap(children[i],children[j]);

        std::swap(lower_x[i],lower_x[j]);
        std::swap(upper_x[i],upper_x[j]);
        std::swap(lower_y[i],lower_y[j]);
        std::swap(upper_y[i],upper_y[j]);
        std::swap(lower_z[i],lower_z[j]);
        std::swap(upper_z[i],upper_z[j]);

        std::swap(lower_dx[i],lower_dx[j]);
        std::swap(upper_dx[i],upper_dx[j]);
        std::swap(lower_dy[i],lower_dy[j]);
        std::swap(upper_dy[i],upper_dy[j]);
        std::swap(lower_dz[i],lower_dz[j]);
        std::swap(upper_dz[i],upper_dz[j]);
      }

      /*! Returns reference to specified child */
      __forceinline       NodeRef& child(size_t i)       { assert(i<N); return children[i]; }
      __forceinline const NodeRef& child(size_t i) const { assert(i<N); return children[i]; }

      /*! stream output operator */
      friend std::ostream& operator<<(std::ostream& cout, const AlignedNodeMB& n) 
      {
        cout << "AlignedNodeMB {" << std::endl;
        for (size_t i=0; i<N; i++) 
        {
          const BBox3fa b0 = n.bounds0(i);
          const BBox3fa b1 = n.bounds1(i);
          cout << "  child" << i << " { " << std::endl;
          cout << "    bounds0 = " << b0 << ", " << std::endl;
          cout << "    bounds1 = " << b1 << ", " << std::endl;
          cout << "  }";
        }
        cout << "}";
        return cout;
  }

    public:
      vfloat<N> lower_x;        //!< X dimension of lower bounds of all N children.
      vfloat<N> upper_x;        //!< X dimension of upper bounds of all N children.
      vfloat<N> lower_y;        //!< Y dimension of lower bounds of all N children.
      vfloat<N> upper_y;        //!< Y dimension of upper bounds of all N children.
      vfloat<N> lower_z;        //!< Z dimension of lower bounds of all N children.
      vfloat<N> upper_z;        //!< Z dimension of upper bounds of all N children.

      vfloat<N> lower_dx;        //!< X dimension of lower bounds of all N children.
      vfloat<N> upper_dx;        //!< X dimension of upper bounds of all N children.
      vfloat<N> lower_dy;        //!< Y dimension of lower bounds of all N children.
      vfloat<N> upper_dy;        //!< Y dimension of upper bounds of all N children.
      vfloat<N> lower_dz;        //!< Z dimension of lower bounds of all N children.
      vfloat<N> upper_dz;        //!< Z dimension of upper bounds of all N children.
    };

    /*! Node with unaligned bounds */
    struct UnalignedNode : public BaseNode
    {
      using BaseNode::children;
      
      struct Create
      {
        __forceinline NodeRef operator() (FastAllocator::ThreadLocal2* alloc) const
        {
          UnalignedNode* node = (UnalignedNode*) alloc->alloc0->malloc(sizeof(UnalignedNode),byteNodeAlignment); node->clear();
          return BVHN::encodeNode(node);
        }
      };

      struct Set
      {
        __forceinline void operator() (NodeRef node, size_t i, NodeRef child, const OBBox3fa& bounds) const {
          node.unalignedNode()->set(i,child);
          node.unalignedNode()->set(i,bounds);
        }
      };

      /*! Clears the node. */
      __forceinline void clear()
      {
        naabb.l.vx = Vec3fa(nan);
        naabb.l.vy = Vec3fa(nan);
        naabb.l.vz = Vec3fa(nan);
        naabb.p    = Vec3fa(nan);
        BaseNode::clear();
      }

      /*! Sets bounding box. */
      __forceinline void set(size_t i, const OBBox3fa& b)
      {
        assert(i < N);

        AffineSpace3fa space = b.space;
        space.p -= b.bounds.lower;
        space = AffineSpace3fa::scale(1.0f/max(Vec3fa(1E-19f),b.bounds.upper-b.bounds.lower))*space;

        naabb.l.vx.x[i] = space.l.vx.x;
        naabb.l.vx.y[i] = space.l.vx.y;
        naabb.l.vx.z[i] = space.l.vx.z;

        naabb.l.vy.x[i] = space.l.vy.x;
        naabb.l.vy.y[i] = space.l.vy.y;
        naabb.l.vy.z[i] = space.l.vy.z;

        naabb.l.vz.x[i] = space.l.vz.x;
        naabb.l.vz.y[i] = space.l.vz.y;
        naabb.l.vz.z[i] = space.l.vz.z;

        naabb.p.x[i] = space.p.x;
        naabb.p.y[i] = space.p.y;
        naabb.p.z[i] = space.p.z;
      }

      /*! Sets ID of child. */
      __forceinline void set(size_t i, const NodeRef& childID) {
        assert(i < N);
        children[i] = childID;
      }

      /*! Returns the extend of the bounds of the ith child */
      __forceinline Vec3fa extend(size_t i) const {
        assert(i<N);
        const Vec3fa vx(naabb.l.vx.x[i],naabb.l.vx.y[i],naabb.l.vx.z[i]);
        const Vec3fa vy(naabb.l.vy.x[i],naabb.l.vy.y[i],naabb.l.vy.z[i]);
        const Vec3fa vz(naabb.l.vz.x[i],naabb.l.vz.y[i],naabb.l.vz.z[i]);
        return rsqrt(vx*vx + vy*vy + vz*vz);
      }

      /*! Returns reference to specified child */
      __forceinline       NodeRef& child(size_t i)       { assert(i<N); return children[i]; }
      __forceinline const NodeRef& child(size_t i) const { assert(i<N); return children[i]; }

    public:
      AffineSpace3vfN naabb;   //!< non-axis aligned bounding boxes (bounds are [0,1] in specified space)
    };

    struct UnalignedNodeMB : public BaseNode
    {
      using BaseNode::children;

      struct Create
      {
        __forceinline NodeRef operator() (FastAllocator::ThreadLocal2* alloc) const
        {
          UnalignedNodeMB* node = (UnalignedNodeMB*) alloc->alloc0->malloc(sizeof(UnalignedNodeMB),byteNodeAlignment); node->clear();
          return encodeNode(node);
        }
      };

      struct Set
      {
        __forceinline void operator() (NodeRef node, size_t i, NodeRef child, const LinearSpace3fa& space, const LBBox3fa& lbounds, const BBox1f dt) const {
          node.unalignedNodeMB()->set(i,child);
          node.unalignedNodeMB()->set(i,space,lbounds.global(dt));
        }
      };

      /*! Clears the node. */
      __forceinline void clear()
      {
        space0 = one;
        //b0.lower = b0.upper = Vec3fa(nan);
        b1.lower = b1.upper = Vec3fa(nan);
        BaseNode::clear();
      }

      /*! Sets space and bounding boxes. */
      __forceinline void set(size_t i, const AffineSpace3fa& space, const LBBox3fa& lbounds) {
        set(i,space,lbounds.bounds0,lbounds.bounds1);
      }

      /*! Sets space and bounding boxes. */
      __forceinline void set(size_t i, const AffineSpace3fa& s0, const BBox3fa& a, const BBox3fa& c)
      {
        assert(i < N);

        AffineSpace3fa space = s0;
        space.p -= a.lower;
        Vec3fa scale = 1.0f/max(Vec3fa(1E-19f),a.upper-a.lower);
        space = AffineSpace3fa::scale(scale)*space;
        BBox3fa a1((a.lower-a.lower)*scale,(a.upper-a.lower)*scale);
        BBox3fa c1((c.lower-a.lower)*scale,(c.upper-a.lower)*scale);

        space0.l.vx.x[i] = space.l.vx.x; space0.l.vx.y[i] = space.l.vx.y; space0.l.vx.z[i] = space.l.vx.z;
        space0.l.vy.x[i] = space.l.vy.x; space0.l.vy.y[i] = space.l.vy.y; space0.l.vy.z[i] = space.l.vy.z;
        space0.l.vz.x[i] = space.l.vz.x; space0.l.vz.y[i] = space.l.vz.y; space0.l.vz.z[i] = space.l.vz.z;
        space0.p   .x[i] = space.p   .x; space0.p   .y[i] = space.p   .y; space0.p   .z[i] = space.p   .z;

        /*b0.lower.x[i] = a1.lower.x; b0.lower.y[i] = a1.lower.y; b0.lower.z[i] = a1.lower.z;
          b0.upper.x[i] = a1.upper.x; b0.upper.y[i] = a1.upper.y; b0.upper.z[i] = a1.upper.z;*/

        b1.lower.x[i] = c1.lower.x; b1.lower.y[i] = c1.lower.y; b1.lower.z[i] = c1.lower.z;
        b1.upper.x[i] = c1.upper.x; b1.upper.y[i] = c1.upper.y; b1.upper.z[i] = c1.upper.z;
      }

      /*! Sets ID of child. */
      __forceinline void set(size_t i, const NodeRef& childID) {
        assert(i < N);
        children[i] = childID;
      }

      /*! Returns the extend of the bounds of the ith child */
      __forceinline Vec3fa extend0(size_t i) const {
        assert(i < N);
        const Vec3fa vx(space0.l.vx.x[i],space0.l.vx.y[i],space0.l.vx.z[i]);
        const Vec3fa vy(space0.l.vy.x[i],space0.l.vy.y[i],space0.l.vy.z[i]);
        const Vec3fa vz(space0.l.vz.x[i],space0.l.vz.y[i],space0.l.vz.z[i]);
        return rsqrt(vx*vx + vy*vy + vz*vz);
      }

    public:
      AffineSpace3vfN space0;
      //BBox3vfN b0; // these are the unit bounds
      BBox3vfN b1;
    };

    struct TransformNode
    {
      __forceinline TransformNode () {}

      __forceinline TransformNode(const AffineSpace3fa& local2world, const BBox3fa& localBounds, NodeRef child, unsigned mask, unsigned int instID, unsigned int xfmID, unsigned int type)
        : local2world(local2world), world2local(rcp(local2world)), localBounds(localBounds), identity(local2world == AffineSpace3fa(one)), child(child), mask(mask), instID(instID), xfmID(xfmID), type(type) {}

      AffineSpace3fa local2world; //!< transforms from local space to world space
      AffineSpace3fa world2local; //!< transforms from world space to local space
      BBox3fa localBounds;
      bool identity;
      NodeRef child;
      unsigned mask;
      unsigned int instID;
      unsigned int xfmID;
      unsigned int type;
    };

    /*! BVHN Quantized Node */
    struct __aligned(16) QuantizedNode : public BaseNode
    {
      using BaseNode::children;

      static const unsigned char MIN_QUAN_8BIT = 0;
      static const unsigned char MAX_QUAN_8BIT = 255;

      struct Create2
      {
        template<typename BuildRecord>
        __forceinline NodeRef operator() (BuildRecord* children, const size_t n, FastAllocator::ThreadLocal2* alloc) const
        {
          __aligned(64) AlignedNode node;
          node.clear();
          for (size_t i=0; i<n; i++) {
            node.set(i,children[i].bounds());
          }
          QuantizedNode *qnode = (QuantizedNode*) alloc->alloc0->malloc(sizeof(QuantizedNode), byteAlignment);
          qnode->init(node);
          
          return (size_t)qnode | tyQuantizedNode;
        }
      };

      struct Set2
      {
        template<typename BuildRecord>
        __forceinline NodeRef operator() (const BuildRecord& precord, const BuildRecord* crecords, NodeRef ref, NodeRef* children, const size_t num) const
        {
          QuantizedNode* node = ref.quantizedNode();
          for (size_t i=0; i<num; i++) node->set(i,children[i]);
          return ref;
        }
      };

      /*! Clears the node. */
      __forceinline void clear() {
        for (size_t i=0; i<N; i++) lower_x[i] = lower_y[i] = lower_z[i] = MAX_QUAN_8BIT;
        for (size_t i=0; i<N; i++) upper_x[i] = upper_y[i] = upper_z[i] = MIN_QUAN_8BIT;
        BaseNode::clear();
      }

      __forceinline void set(size_t i, NodeRef childID) {
        children[i] = childID;
      }
      
      /*! Returns bounds of specified child. */
      __forceinline BBox3fa bounds(size_t i) const
      {
        assert(i < N);
        const Vec3fa lower(madd(scale.x,(float)lower_x[i],start.x),
                           madd(scale.y,(float)lower_y[i],start.y),
                           madd(scale.z,(float)lower_z[i],start.z));
        const Vec3fa upper(madd(scale.x,(float)upper_x[i],start.x),
                           madd(scale.y,(float)upper_y[i],start.y),
                           madd(scale.z,(float)upper_z[i],start.z));
        return BBox3fa(lower,upper);
      }

      /*! Returns extent of bounds of specified child. */
      __forceinline Vec3fa extend(size_t i) const {
        return bounds(i).size();
      }

      static __forceinline void init_dim(const vfloat<N> &lower,
                                         const vfloat<N> &upper,
                                         unsigned char lower_quant[N],
                                         unsigned char upper_quant[N],
                                         float &start,
                                         float &scale)
      {
        const vbool<N> m_valid = lower != vfloat<N>(pos_inf);
        const float minF = reduce_min(lower);
        const float maxF = reduce_max(upper);
        float diff = maxF - minF; //todo: add extracted difference here
        float scale_diff = diff / 255.0f;

        /* accomodate floating point accuracy issues in 'diff' */
        size_t iterations = 0;
        while(minF + scale_diff * 255.0f < maxF)
        {
          diff = nextafter(diff, FLT_MAX);
          scale_diff = diff / 255.0f;
          iterations++;
        }
        const float inv_diff   = 255.0f / diff;

        vfloat<N> floor_lower = floor(  (lower - vfloat<N>(minF)) * vfloat<N>(inv_diff) );
        vfloat<N> ceil_upper  = ceil (  (upper - vfloat<N>(minF)) * vfloat<N>(inv_diff) );
        vint<N> i_floor_lower( floor_lower );
        vint<N> i_ceil_upper ( ceil_upper  );

        /* lower/upper correction */
        vbool<N> m_lower_correction = ((madd(vfloat<N>(i_floor_lower),scale_diff,minF)) > lower) & m_valid;
        vbool<N> m_upper_correction = ((madd(vfloat<N>(i_ceil_upper),scale_diff,minF)) < upper) & m_valid;
        i_floor_lower  = select(m_lower_correction,i_floor_lower-1,i_floor_lower);
        i_ceil_upper   = select(m_upper_correction,i_ceil_upper +1,i_ceil_upper);

        /* disable invalid lanes */
        i_floor_lower = select(m_valid,i_floor_lower,255);
        i_ceil_upper  = select(m_valid,i_ceil_upper ,0);

        /* store as uchar to memory */
        vint<N>::store_uchar(lower_quant,i_floor_lower);
        vint<N>::store_uchar(upper_quant,i_ceil_upper);
        start = minF;
        scale = scale_diff;

#if 0
        vfloat<N> extract_lower( vint<N>::load(lower_quant) );
        vfloat<N> extract_upper( vint<N>::load(upper_quant) );
        vfloat<N> final_extract_lower = madd(extract_lower,scale_diff,minF);
        vfloat<N> final_extract_upper = madd(extract_upper,scale_diff,minF);
        assert( (movemask(final_extract_lower <= lower ) & movemask(m_valid)) == movemask(m_valid));
        assert( (movemask(final_extract_upper >= upper ) & movemask(m_valid)) == movemask(m_valid));
#endif
      }

      __forceinline void init(AlignedNode& node)
      {
        for (size_t i=0;i<N;i++) children[i] = emptyNode;
        init_dim(node.lower_x,node.upper_x,lower_x,upper_x,start.x,scale.x);
        init_dim(node.lower_y,node.upper_y,lower_y,upper_y,start.y,scale.y);
        init_dim(node.lower_z,node.upper_z,lower_z,upper_z,start.z,scale.z);
      }

      __forceinline vfloat<N> dequantizeLowerX() const { return madd(vfloat<N>(vint<N>::load(lower_x)),scale.x,vfloat<N>(start.x)); }

      __forceinline vfloat<N> dequantizeUpperX() const { return madd(vfloat<N>(vint<N>::load(upper_x)),scale.x,vfloat<N>(start.x)); }

      __forceinline vfloat<N> dequantizeLowerY() const { return madd(vfloat<N>(vint<N>::load(lower_y)),scale.y,vfloat<N>(start.y)); }

      __forceinline vfloat<N> dequantizeUpperY() const { return madd(vfloat<N>(vint<N>::load(upper_y)),scale.y,vfloat<N>(start.y)); }

      __forceinline vfloat<N> dequantizeLowerZ() const { return madd(vfloat<N>(vint<N>::load(lower_z)),scale.z,vfloat<N>(start.z)); }

      __forceinline vfloat<N> dequantizeUpperZ() const { return madd(vfloat<N>(vint<N>::load(upper_z)),scale.z,vfloat<N>(start.z)); }

      template <int M>
      __forceinline vfloat<M> dequantize(const size_t offset) const { return vfloat<M>(vint<M>::loadu(all_planes+offset)); }


      union {
        struct {
          unsigned char lower_x[N]; //!< 8bit discretized X dimension of lower bounds of all N children
          unsigned char upper_x[N]; //!< 8bit discretized X dimension of upper bounds of all N children
          unsigned char lower_y[N]; //!< 8bit discretized Y dimension of lower bounds of all N children
          unsigned char upper_y[N]; //!< 8bit discretized Y dimension of upper bounds of all N children
          unsigned char lower_z[N]; //!< 8bit discretized Z dimension of lower bounds of all N children
          unsigned char upper_z[N]; //!< 8bit discretized Z dimension of upper bounds of all N children
        };
        unsigned char all_planes[6*N];
      };

      Vec3f start;
      Vec3f scale;
    };


    /*! swap the children of two nodes */
    __forceinline static void swap(AlignedNode* a, size_t i, AlignedNode* b, size_t j)
    {
      assert(i<N && j<N);
      std::swap(a->children[i],b->children[j]);
      std::swap(a->lower_x[i],b->lower_x[j]);
      std::swap(a->lower_y[i],b->lower_y[j]);
      std::swap(a->lower_z[i],b->lower_z[j]);
      std::swap(a->upper_x[i],b->upper_x[j]);
      std::swap(a->upper_y[i],b->upper_y[j]);
      std::swap(a->upper_z[i],b->upper_z[j]);
    }

    /*! compacts a node (moves empty children to the end) */
    __forceinline static void compact(AlignedNode* a)
    {
      /* find right most filled node */
      ssize_t j=N;
      for (j=j-1; j>=0; j--)
        if (a->child(j) != emptyNode)
          break;

      /* replace empty nodes with filled nodes */
      for (ssize_t i=0; i<j; i++) {
        if (a->child(i) == emptyNode) {
          a->swap(i,j);
          for (j=j-1; j>i; j--)
            if (a->child(j) != emptyNode)
              break;
        }
      }
    }

    /*! compacts a node (moves empty children to the end) */
    __forceinline static void compact(AlignedNodeMB* a)
    {
      /* find right most filled node */
      ssize_t j=N;
      for (j=j-1; j>=0; j--)
        if (a->child(j) != emptyNode)
          break;

      /* replace empty nodes with filled nodes */
      for (ssize_t i=0; i<j; i++) {
        if (a->child(i) == emptyNode) {
          a->swap(i,j);
          for (j=j-1; j>i; j--)
            if (a->child(j) != emptyNode)
              break;
        }
      }
    }

  public:

    /*! BVHN default constructor. */
    BVHN (const PrimitiveType& primTy, Scene* scene);

    /*! BVHN destruction */
    ~BVHN ();

    /*! clears the acceleration structure */
    void clear();

    /*! sets BVH members after build */
    void set (NodeRef root, const LBBox3fa& bounds, size_t numPrimitives);

    /*! prints statistics about the BVH */
    void printStatistics();

    /*! Clears the barrier bits of a subtree. */
    void clearBarrier(NodeRef& node);

    /*! lays out num large nodes of the BVH */
    void layoutLargeNodes(size_t num);
    NodeRef layoutLargeNodesRecursion(NodeRef& node, FastAllocator::ThreadLocal& allocator);

    /*! calculates the amount of bytes allocated */
    size_t bytesAllocated() {
      return alloc.getAllocatedBytes();
    }

    /*! called by all builders before build starts */
    double preBuild(const std::string& builderName);

    /*! called by all builders after build ended */
    void postBuild(double t0);

    /*! allocator class */
    struct Allocator {
      BVHN* bvh;
      Allocator (BVHN* bvh) : bvh(bvh) {}
      __forceinline void* operator() (size_t bytes) const { 
        return bvh->alloc.threadLocal()->malloc(bytes); 
      }
    };

    /*! shrink allocated memory */
    void shrink() {
      alloc.shrink();
    }

    /*! post build cleanup */
    void cleanup() {
      alloc.cleanup();
    }


    /*! return the true root */
    __forceinline NodeRef getRoot(const RayPrecalculations& pre) const {
      return root;
    }

    __forceinline NodeRef getRoot(const RayPrecalculationsMB& pre) const {
      NodeRef* roots = (NodeRef*)(size_t)root;
      return roots[pre.itime()];
    }

    template<int K>
      __forceinline NodeRef getRoot(const RayKPrecalculations<K>& pre, size_t k) const {
      return root;
    }

    template<int K>
      __forceinline NodeRef getRoot(const RayKPrecalculationsMB<K>& pre, size_t k) const {
      NodeRef* roots = (NodeRef*)(size_t)root;
      return roots[pre.itime(k)];
    }

  public:

    /*! Encodes a node */
    static __forceinline NodeRef encodeNode(AlignedNode* node) {
      assert(!((size_t)node & align_mask));
      return NodeRef((size_t) node);
    }

    static __forceinline unsigned int encodeQuantizedNode(size_t base, size_t node) {
      assert(!((size_t)node & align_mask));
      ssize_t node_offset = (ssize_t)node-(ssize_t)base;
      assert(node_offset != 0);
      assert((int64_t)node_offset >= -int64_t(0x80000000) && (int64_t)node_offset <= (int64_t)0x7fffffff);
      return (unsigned int)node_offset | tyQuantizedNode;
    }

    static __forceinline int encodeQuantizedLeaf(size_t base, size_t node) {
      ssize_t leaf_offset = (ssize_t)node-(ssize_t)base;
      assert((int64_t)leaf_offset >= -int64_t(0x80000000) && (int64_t)leaf_offset <= (int64_t)0x7fffffff);
      return (int)leaf_offset;
    }

    static __forceinline NodeRef encodeNode(AlignedNodeMB* node) {
      assert(!((size_t)node & align_mask));
      return NodeRef((size_t) node | tyAlignedNodeMB);
    }

    /*! Encodes an unaligned node */
    static __forceinline NodeRef encodeNode(UnalignedNode* node) {
      return NodeRef((size_t) node | tyUnalignedNode);
    }

    /*! Encodes an unaligned motion blur node */
    static __forceinline NodeRef encodeNode(UnalignedNodeMB* node) {
      return NodeRef((size_t) node | tyUnalignedNodeMB);
    }

    /*! Encodes a transformation node */
    static __forceinline NodeRef encodeNode(TransformNode* node) {
      return NodeRef((size_t) node | tyTransformNode);
    }

    /*! Encodes a leaf */
    static __forceinline NodeRef encodeLeaf(void* tri, size_t num) {
      assert(!((size_t)tri & align_mask));
      assert(num <= maxLeafBlocks);
      return NodeRef((size_t)tri | (tyLeaf+min(num,(size_t)maxLeafBlocks)));
    }

    /*! Encodes a leaf */
    static __forceinline NodeRef encodeTypedLeaf(void* ptr, size_t ty) {
      assert(!((size_t)ptr & align_mask));
      return NodeRef((size_t)ptr | (tyLeaf+ty));
    }

    /*! bvh type information */
  public:
    const PrimitiveType& primTy;       //!< primitive type stored in the BVH

    /*! bvh data */
  public:
    Device* device;                    //!< device pointer
    Scene* scene;                      //!< scene pointer
    NodeRef root;                      //!< root node
    bool msmblur;                      //!< when true root points to array of roots for MSMBlur mode
    unsigned numTimeSteps;             //!< number of time steps
    FastAllocator alloc;               //!< allocator used to allocate nodes

    /*! statistics data */
  public:
    size_t numPrimitives;              //!< number of primitives the BVH is build over
    size_t numVertices;                //!< number of vertices the BVH references

    /*! data arrays for special builders */
  public:
    std::vector<BVHN*> objects;
    avector<char,aligned_allocator<char,32>> subdiv_patches;
    mvector<PrimRef> primrefs;
  };

  template<>
  __forceinline void BVHN<4>::AlignedNode::bounds(BBox<vfloat4>& bounds0, BBox<vfloat4>& bounds1, BBox<vfloat4>& bounds2, BBox<vfloat4>& bounds3) const {
    transpose(lower_x,lower_y,lower_z,vfloat4(zero),bounds0.lower,bounds1.lower,bounds2.lower,bounds3.lower);
    transpose(upper_x,upper_y,upper_z,vfloat4(zero),bounds0.upper,bounds1.upper,bounds2.upper,bounds3.upper);
  }

  typedef BVHN<4> BVH4;
  typedef BVHN<8> BVH8;
}
