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

#include "bvh_statistics.h"

namespace embree
{
  template<int N>
  BVHNStatistics<N>::BVHNStatistics (BVH* bvh) : bvh(bvh)
  {
    double A = max(0.0f,bvh->getLinearBounds().expectedHalfArea());
    if (bvh->msmblur) 
    {
      NodeRef* roots = (NodeRef*)(size_t)bvh->root;
      for (size_t i=0; i<bvh->numTimeSteps-1; i++) {
        const BBox1f t0t1(float(i+0)/float(bvh->numTimeSteps-1),
                        float(i+1)/float(bvh->numTimeSteps-1));
        stat = stat + statistics(roots[i],A,t0t1);
      }
    }
    else {
      stat = statistics(bvh->root,A,BBox1f(0.0f,1.0f));
    }
  }
  
  template<int N>
  std::string BVHNStatistics<N>::str()
  {
    std::ostringstream stream;
    stream.setf(std::ios::fixed, std::ios::floatfield);
    stream << "  primitives = " << bvh->numPrimitives << ", vertices = " << bvh->numVertices << ", depth = " << stat.depth << std::endl;
    size_t totalBytes = stat.bytes(bvh);
    double totalSAH = stat.sah(bvh);
    stream << "  total            : sah = "  << std::setw(7) << std::setprecision(3) << totalSAH << " (100.00%), ";
    stream << "#bytes = " << std::setw(7) << std::setprecision(2) << totalBytes/1E6 << " MB (100.00%), ";
    stream << "#nodes = " << std::setw(7) << stat.size() << " (" << std::setw(6) << std::setprecision(2) << 100.0*stat.fillRate(bvh) << "% filled), ";
    stream << "#bytes/prim = " << std::setw(6) << std::setprecision(2) << double(totalBytes)/double(bvh->numPrimitives) << std::endl;
    if (stat.statAlignedNodes.numNodes    ) stream << "  alignedNodes     : "  << stat.statAlignedNodes.toString(bvh,totalSAH,totalBytes) << std::endl;
    if (stat.statUnalignedNodes.numNodes  ) stream << "  unalignedNodes   : "  << stat.statUnalignedNodes.toString(bvh,totalSAH,totalBytes) << std::endl;
    if (stat.statAlignedNodesMB.numNodes  ) stream << "  alignedNodesMB   : "  << stat.statAlignedNodesMB.toString(bvh,totalSAH,totalBytes) << std::endl;
    if (stat.statUnalignedNodesMB.numNodes) stream << "  unalignedNodesMB : "  << stat.statUnalignedNodesMB.toString(bvh,totalSAH,totalBytes) << std::endl;
    if (stat.statTransformNodes.numNodes  ) stream << "  transformNodes   : "  << stat.statTransformNodes.toString(bvh,totalSAH,totalBytes) << std::endl;
    if (stat.statQuantizedNodes.numNodes  ) stream << "  quantizedNodes   : "  << stat.statQuantizedNodes.toString(bvh,totalSAH,totalBytes) << std::endl;
    if (true)                               stream << "  leaves           : "  << stat.statLeaf.toString(bvh,totalSAH,totalBytes) << std::endl;
    if (true)                               stream << "    histogram      : "  << stat.statLeaf.histToString() << std::endl;
    return stream.str();
  }
  
  template<int N>
  typename BVHNStatistics<N>::Statistics BVHNStatistics<N>::statistics(NodeRef node, const double A, const BBox1f t0t1)
  {
    Statistics s;
    assert(t0t1.size() > 0.0f);
    double dt = max(0.0f,t0t1.size());
    if (node.isAlignedNode())
    {
      AlignedNode* n = node.alignedNode();
      for (size_t i=0; i<N; i++) {
        if (n->child(i) == BVH::emptyNode) continue;
        s.statAlignedNodes.numChildren++;
        const double Ai = max(0.0f,halfArea(n->extend(i)));
        s = s + statistics(n->child(i),Ai,t0t1); 
      }
      s.statAlignedNodes.numNodes++;
      s.statAlignedNodes.nodeSAH += dt*A;
      s.depth++;
    }
    else if (node.isUnalignedNode())
    {
      UnalignedNode* n = node.unalignedNode();
      for (size_t i=0; i<N; i++) {
        if (n->child(i) == BVH::emptyNode) continue;
        s.statUnalignedNodes.numChildren++;
        const double Ai = max(0.0f,halfArea(n->extend(i)));
        s = s + statistics(n->child(i),Ai,t0t1); 
      }
      s.statUnalignedNodes.numNodes++;
      s.statUnalignedNodes.nodeSAH += dt*A;
      s.depth++;
    }
    else if (node.isAlignedNodeMB())
    {
      AlignedNodeMB* n = node.alignedNodeMB();
      for (size_t i=0; i<N; i++) {
        if (n->child(i) == BVH::emptyNode) continue;
        s.statAlignedNodesMB.numChildren++;
        const double Ai = max(0.0f,n->expectedHalfArea(i,t0t1));
        s = s + statistics(n->child(i),Ai,t0t1);
      }
      s.statAlignedNodesMB.numNodes++;
      s.statAlignedNodesMB.nodeSAH += dt*A;
      s.depth++;
    }
    else if (node.isUnalignedNodeMB())
    {
      UnalignedNodeMB* n = node.unalignedNodeMB();
      for (size_t i=0; i<N; i++) {
        if (n->child(i) == BVH::emptyNode) continue;
        s.statUnalignedNodesMB.numChildren++;
        const double Ai = max(0.0f,halfArea(n->extend0(i)));
        s = s + statistics(n->child(i),Ai,t0t1); 
      }
      s.statUnalignedNodesMB.numNodes++;
      s.statUnalignedNodesMB.nodeSAH += dt*A;
      s.depth++;
    }
    else if (node.isTransformNode())
    {
      s.statTransformNodes.numNodes++;
      s.statTransformNodes.nodeSAH += dt*A;
      s.depth++;
    }
    else if (node.isQuantizedNode())
    {
      QuantizedNode* n = node.quantizedNode();
      for (size_t i=0; i<N; i++) {
        if (n->child(i) == BVH::emptyNode) continue;
        s.statQuantizedNodes.numChildren++;
        const double Ai = max(0.0f,halfArea(n->extend(i)));
        s = s + statistics(n->child(i),Ai,t0t1); 
      }
      s.statQuantizedNodes.numNodes++;
      s.statQuantizedNodes.nodeSAH += dt*A;
      s.depth++;
    }
    else if (node.isLeaf())
    {
      size_t num; const char* tri = node.leaf(num);
      if (num)
      {
        for (size_t i=0; i<num; i++) {
          s.statLeaf.numPrims += bvh->primTy.size(tri+i*bvh->primTy.bytes);
        }
        s.statLeaf.numLeaves++;
        s.statLeaf.numPrimBlocks += num;
        s.statLeaf.leafSAH += dt*A*num;
        if (num-1 < Statistics::LeafStat::NHIST) {
          s.statLeaf.numPrimBlocksHistogram[num-1]++;
        }
      }
    }
    else {
      throw std::runtime_error("not supported node type in bvh_statistics");
    }
    return s;
  } 

#if defined(__AVX__)
  template class BVHNStatistics<8>;
#else
  template class BVHNStatistics<4>;
#endif
}
