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

#include "bvh.h"
#include "bvh_statistics.h"

namespace embree
{
  template<int N>
  BVHN<N>::BVHN (const PrimitiveType& primTy, Scene* scene)
    : AccelData((N==4) ? AccelData::TY_BVH4 : (N==8) ? AccelData::TY_BVH8 : AccelData::TY_UNKNOWN),
      primTy(primTy), device(scene->device), scene(scene),
      root(emptyNode), msmblur(false), numTimeSteps(1), alloc(scene->device,scene->isStatic()), numPrimitives(0), numVertices(0) , primrefs(scene->device)
  {
  }

  template<int N>
  BVHN<N>::~BVHN ()
  {
    for (size_t i=0; i<objects.size(); i++) 
      delete objects[i];
  }

  template<int N>
  void BVHN<N>::clear()
  {
    set(BVHN::emptyNode,empty,0);
    alloc.clear();
  }

  template<int N>
  void BVHN<N>::set (NodeRef root, const LBBox3fa& bounds, size_t numPrimitives)
  {
    this->root = root;
    this->bounds = bounds;
    this->numPrimitives = numPrimitives;
  }

  template<int N>
  void BVHN<N>::printStatistics()
  {
    std::cout << BVHNStatistics<N>(this).str();
  }	

  template<int N>
  void BVHN<N>::clearBarrier(NodeRef& node)
  {
    if (node.isBarrier())
      node.clearBarrier();
    else if (!node.isLeaf()) {
      BaseNode* n = node.baseNode(BVH_FLAG_ALIGNED_NODE); // FIXME: flags should be stored in BVH
      for (size_t c=0; c<N; c++)
        clearBarrier(n->child(c));
    }
  }

  template<int N>
  void BVHN<N>::layoutLargeNodes(size_t num)
  {
    struct NodeArea 
    {
      __forceinline NodeArea() {}

      __forceinline NodeArea(NodeRef& node, const BBox3fa& bounds)
        : node(&node), A(node.isLeaf() ? float(neg_inf) : area(bounds)) {}

      __forceinline bool operator< (const NodeArea& other) const {
        return this->A < other.A;
      }

      NodeRef* node;
      float A;
    };
    std::vector<NodeArea> lst;
    lst.reserve(num);
    lst.push_back(NodeArea(root,empty));

    while (lst.size() < num)
    {
      std::pop_heap(lst.begin(), lst.end());
      NodeArea n = lst.back(); lst.pop_back();
      if (!n.node->isAlignedNode()) break;
      AlignedNode* node = n.node->alignedNode();
      for (size_t i=0; i<N; i++) {
        if (node->child(i) == BVHN::emptyNode) continue;
        lst.push_back(NodeArea(node->child(i),node->bounds(i)));
        std::push_heap(lst.begin(), lst.end());
      }
    }

    for (size_t i=0; i<lst.size(); i++)
      lst[i].node->setBarrier();
      
    root = layoutLargeNodesRecursion(root,*alloc.threadLocal());
  }
  
  template<int N>
  typename BVHN<N>::NodeRef BVHN<N>::layoutLargeNodesRecursion(NodeRef& node, FastAllocator::ThreadLocal& allocator)
  {
    if (node.isBarrier()) {
      node.clearBarrier();
      return node;
    }
    else if (node.isAlignedNode()) 
    {
      AlignedNode* oldnode = node.alignedNode();
      AlignedNode* newnode = (BVHN::AlignedNode*) allocator.malloc(sizeof(BVHN::AlignedNode),byteNodeAlignment);
      *newnode = *oldnode;
      for (size_t c=0; c<N; c++)
        newnode->child(c) = layoutLargeNodesRecursion(oldnode->child(c),allocator);
      return encodeNode(newnode);
    }
    else return node;
  }

  template<int N>
  double BVHN<N>::preBuild(const std::string& builderName)
  {
    if (builderName == "") 
      return inf;

    if (device->verbosity(1))
      std::cout << "building BVH" << N << "<" << primTy.name << "> using " << builderName << " ..." << std::flush;

    double t0 = 0.0;
    if (device->benchmark || device->verbosity(1)) t0 = getSeconds();
    return t0;
  }

  template<int N>
  void BVHN<N>::postBuild(double t0)
  {
    if (t0 == double(inf))
      return;
    
    double dt = 0.0;
    if (device->benchmark || device->verbosity(1)) 
      dt = getSeconds()-t0;

    /* print statistics */
    if (device->verbosity(1)) {
      const size_t usedBytes = alloc.getUsedBytes();
      std::cout << " [DONE]" << "  " << 1000.0f*dt << "ms, " << 1E-6*double(numPrimitives)/dt << " Mprim/s, " << 1E-9*double(usedBytes)/dt << " GB/s" << std::endl;
    }
    
    if (device->verbosity(2))
      printStatistics();

    if (device->verbosity(2))
      alloc.print_statistics(device->verbosity(3));

    /* benchmark mode */
    if (device->benchmark) {
      BVHNStatistics<N> stat(this);
      std::cout << "BENCHMARK_BUILD " << dt << " " << double(numPrimitives)/dt << " " << stat.sah() << " " << stat.bytesUsed() << std::endl;
    }
  }

#if defined(__AVX__)
  template class BVHN<8>;
#else
  template class BVHN<4>;
#endif
}

