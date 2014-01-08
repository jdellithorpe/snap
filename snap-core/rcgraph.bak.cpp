/*
 * rcgraph.bak.cpp
 *
 *  Created on: Nov 18, 2013
 *      Author: jdellit
 */

/////////////////////////////////////////////////
// Undirected Graph

bool TUNRCGraph::HasFlag(const TGraphFlag& Flag) const {
  return HasGraphFlag(TUNRCGraph::TNet, Flag);
}


bool TUNRCGraph::TNode::IsNbrNId(const uint64_t& NId) const {
  for(uint64_t i = 0; i < NIdV.size(); i++) {
    if(NIdV[i] == NId)
      return true;
  }
  return false;
}

TUNRCGraph::TNodeI::TNodeI(PUNRCGraph G) : Node(G), G(G), tableEnum(G->RamCloud, G->AdjTable_Id)  {

}

TUNRCGraph::TNodeI::TNodeI(const TNodeI& NodeI) : Node(NodeI.G), G(NodeI.G), tableEnum(NodeI.G->RamCloud, NodeI.G->AdjTable_Id) {

}

/*
TUNRCGraph::TNodeI::TNodeI(PUNRCGraph G, const TNode& Node) : Node(G, Node), G(G), tableEnum(G->RamCloud, G->AdjTable_Id) {

}
 */

TUNRCGraph::TNodeI& TUNRCGraph::TNodeI::operator++ (int) {
  uint32_t size = 0;
  const void* buffer = 0;

  tableEnum.next(&size, &buffer);
  RAMCloud::Object object(buffer, size);

  uint64_t* NIdPtr = (uint64_t*)object.getKey();
  uint64_t* NbrArr = (uint64_t*)object.getData();

  std::vector<uint64_t> NbrVec(NbrArr, NbrArr + (object.getDataLength()/sizeof(uint64_t)));
  Node = TUNRCGraph::TNode(G, *NIdPtr, NbrVec);

  return *this;
}

/// Add a node with a unique ID to the graph.
uint64_t TUNRCGraph::AddNode() {
  uint64_t NId = MxNId; MxNId++;
  std::vector<uint64_t> Nbrs(0);
  RamCloud.write(AdjTable_Id, (char*)&NId, sizeof(uint64_t), Nbrs.data(), Nbrs.size()*sizeof(uint64_t));
  return NId;
}

// Add a node of ID NId to the graph.
uint64_t TUNRCGraph::AddNode(const uint64_t NId) {
  IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
  MxNId = TMath::Mx(NId+1, MxNId);
  std::vector<uint64_t> Nbrs(0);
  RamCloud.write(AdjTable_Id, (char*)&NId, sizeof(uint64_t), Nbrs.data(), Nbrs.size()*sizeof(uint64_t));
  return NId;
}

// Add a node of ID NId to the graph and create edges to all nodes in vector Nbrs.
uint64_t TUNRCGraph::AddNode(const uint64_t& NId, const std::vector<uint64_t>& Nbrs, const bool& safetyChecks) {
  if(safetyChecks)
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
  MxNId = TMath::Mx(NId+1, MxNId);
  RamCloud.write(AdjTable_Id, (char*)&NId, sizeof(uint64_t), Nbrs.data(), Nbrs.size()*sizeof(uint64_t));
  NEdges += Nbrs.size();
  return NId;
}

uint64_t TUNRCGraph::AddNodes(const std::vector<uint64_t>& NodeIdVec, const std::vector<std::vector<uint64_t> >& NbrsVec) {
  RAMCloud::MultiWriteObject* requests[NodeIdVec.size()];
  RAMCloud::Tub<RAMCloud::MultiWriteObject> objects[NodeIdVec.size()];

  for(uint64_t i = 0; i < NodeIdVec.size(); i++) {
    objects[i].construct( AdjTable_Id,
                          (char*)&NodeIdVec[i],
                          sizeof(uint64_t),
                          (char*)NbrsVec[i].data(),
                          NbrsVec[i].size()*sizeof(uint64_t) );
    requests[i] = objects[i].get();
  }

  try {
    RamCloud.multiWrite(requests, NodeIdVec.size());
  } catch(RAMCloud::ClientException& e) {
    fprintf(stderr, "RAMCloud exception: %s\n", e.str().c_str());
    return -1;
  }

  return 0;
}

TUNRCGraph::TNode TUNRCGraph::GetNode(const uint64_t& NId) {
  RAMCloud::Buffer buffer;
  try {
    RamCloud.read(AdjTable_Id, (char*)&NId, sizeof(uint64_t), &buffer);
  } catch (RAMCloud::ClientException& e) {
    IAssertR(false, TStr::Fmt("NodeId %d does not exist", NId));
  }
  string adjlist(static_cast<const char*>(buffer.getRange(0, buffer.getTotalLength())), buffer.getTotalLength());
  uint64_t* neighborArray = (uint64_t*)adjlist.c_str();
  std::vector<uint64_t> neighborVec(neighborArray, neighborArray + (buffer.getTotalLength()/sizeof(uint64_t)));
  return TUNRCGraph::TNode(this, NId, neighborVec);
}

// Add an edge between SrcNId and DstNId to the graph.
int TUNRCGraph::AddEdge(const uint64_t& SrcNId, const uint64_t& DstNId) {
  TNode SrcNode(this);
  {
    RAMCloud::Buffer buffer;
    try {
      RamCloud.read(AdjTable_Id, (char*)&SrcNId, sizeof(uint64_t), &buffer);
    } catch (RAMCloud::ClientException& e) {
      IAssertR(false, TStr::Fmt("NodeId %d does not exist", SrcNId));
    }
    string adjlist(static_cast<const char*>(buffer.getRange(0, buffer.getTotalLength())), buffer.getTotalLength());
    uint64_t* neighborArray = (uint64_t*)adjlist.c_str();
    std::vector<uint64_t> neighborVec(neighborArray, neighborArray + (buffer.getTotalLength()/sizeof(uint64_t)));
    SrcNode = TNode(this, SrcNId, neighborVec);
  }


  if(SrcNId != DstNId) {
    // Not a loop
    TNode DstNode(this);
    {
      RAMCloud::Buffer buffer;
      try {
        RamCloud.read(AdjTable_Id, (char*)&DstNId, sizeof(uint64_t), &buffer);
      } catch (RAMCloud::ClientException& e) {
        IAssertR(false, TStr::Fmt("NodeId %d does not exist", DstNId));
      }
      string adjlist(static_cast<const char*>(buffer.getRange(0, buffer.getTotalLength())), buffer.getTotalLength());
      uint64_t* neighborArray = (uint64_t*)adjlist.c_str();
      std::vector<uint64_t> neighborVec(neighborArray, neighborArray + (buffer.getTotalLength()/sizeof(uint64_t)));
      DstNode = TNode(this, DstNId, neighborVec);
    }

    // Check whether or not the edge already exists
    if(SrcNode.IsNbrNId(DstNId))
      return -2;


    DstNode.NIdV.push_back(SrcNId);
    RamCloud.write(AdjTable_Id, (char*)&DstNode.Id, sizeof(uint64_t), DstNode.NIdV.data(), DstNode.NIdV.size()*sizeof(uint64_t));
  }

  SrcNode.NIdV.push_back(DstNId);
  RamCloud.write(AdjTable_Id, (char*)&SrcNode.Id, sizeof(uint64_t), SrcNode.NIdV.data(), SrcNode.NIdV.size()*sizeof(uint64_t));

  NEdges++;
  return -1;
}

/*
// Add a node of ID NId to the graph and create edges to all nodes in the vector NIdVId in the vector pool Pool).
int TUNRCGraph::AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId) {
  int NewNId;
  if (NId == -1) {
    NewNId = MxNId;  MxNId++;
  } else {
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));
    NewNId = NId;
    MxNId = TMath::Mx(NewNId+1, MxNId());
  }
  TNode& Node = NodeH.AddDat(NewNId);
  Node.Id = NewNId;
  Node.NIdV.GenExt(Pool.GetValVPt(NIdVId), Pool.GetVLen(NIdVId));
  Node.NIdV.Sort();
  NEdges += Node.GetDeg();
  return NewNId;
}

// Delete node of ID NId from the graph.
void TUNRCGraph::DelNode(const int& NId) {
  { AssertR(IsNode(NId), TStr::Fmt("NodeId %d does not exist", NId));
  TNode& Node = GetNode(NId);
  NEdges -= Node.GetDeg();
  for (int e = 0; e < Node.GetDeg(); e++) {
    const int nbr = Node.GetNbrNId(e);
    if (nbr == NId) { continue; }
    TNode& N = GetNode(nbr);
    const int n = N.NIdV.SearchBin(NId);
    IAssert(n != -1); // if NId points to N, then N also should point back
    if (n!= -1) { N.NIdV.Del(n); }
  } }
  NodeH.DelKey(NId);
}

int TUNRCGraph::GetEdges() const {
  //int Edges = 0;
  //for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
  //  Edges += NodeH[N].GetDeg();
  //}
  //return Edges/2;
  return NEdges;
}

// Delete an edge between node IDs SrcNId and DstNId from the graph.
void TUNRCGraph::DelEdge(const int& SrcNId, const int& DstNId) {
  IAssertR(IsNode(SrcNId) && IsNode(DstNId), TStr::Fmt("%d or %d not a node.", SrcNId, DstNId).CStr());
  { TNode& N = GetNode(SrcNId);
  const int n = N.NIdV.SearchBin(DstNId);
  if (n!= -1) { N.NIdV.Del(n);  NEdges--; } }
  if (SrcNId != DstNId) { // not a self edge
    TNode& N = GetNode(DstNId);
    const int n = N.NIdV.SearchBin(SrcNId);
    if (n!= -1) { N.NIdV.Del(n); }
  }
}

// Test whether an edge between node IDs SrcNId and DstNId exists the graph.
bool TUNRCGraph::IsEdge(const int& SrcNId, const int& DstNId) const {
  if (! IsNode(SrcNId) || ! IsNode(DstNId)) return false;
  return GetNode(SrcNId).IsNbrNId(DstNId);
}
 */

bool TUNRCGraph::IsNode(const uint64_t& NId) {
  RAMCloud::Buffer buffer;
  try {
    RamCloud.read(AdjTable_Id, (char*)&NId, sizeof(uint64_t), &buffer);
  } catch (RAMCloud::ClientException& e) {
    return false;
  }
  return true;
}

/*
// Return an iterator referring to edge (SrcNId, DstNId) in the graph.
TUNRCGraph::TEdgeI TUNRCGraph::GetEI(const int& SrcNId, const int& DstNId) const {
  const int MnNId = TMath::Mn(SrcNId, DstNId);
  const int MxNId = TMath::Mx(SrcNId, DstNId);
  const TNodeI SrcNI = GetNI(MnNId);
  const int NodeN = SrcNI.NodeHI.GetDat().NIdV.SearchBin(MxNId);
  IAssert(NodeN != -1);
  return TEdgeI(SrcNI, EndNI(), NodeN);
}


// Get a vector IDs of all nodes in the graph.
void TUNRCGraph::GetNIdV(TIntV& NIdV) const {
  NIdV.Gen(GetNodes(), 0);
  for (int N=NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    NIdV.Add(NodeH.GetKey(N)); }
}

// Defragment the graph.
void TUNRCGraph::Defrag(const bool& OnlyNodeLinks) {
  for (int n = NodeH.FFirstKeyId(); NodeH.FNextKeyId(n); ) {
    NodeH[n].NIdV.Pack();
  }
  if (! OnlyNodeLinks && ! NodeH.IsKeyIdEqKeyN()) {
    NodeH.Defrag();
  }
}

// Check the graph data structure for internal consistency.
bool TUNRCGraph::IsOk(const bool& ThrowExcept) const {
  bool RetVal = true;
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    if (! Node.NIdV.IsSorted()) {
      const TStr Msg = TStr::Fmt("Neighbor list of node %d is not sorted.", Node.GetId());
      if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
      RetVal=false;
    }
    int prevNId = -1;
    for (int e = 0; e < Node.GetDeg(); e++) {
      if (! IsNode(Node.GetNbrNId(e))) {
        const TStr Msg = TStr::Fmt("Edge %d --> %d: node %d does not exist.",
          Node.GetId(), Node.GetNbrNId(e), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      if (e > 0 && prevNId == Node.GetNbrNId(e)) {
        const TStr Msg = TStr::Fmt("Node %d has duplicate edge %d --> %d.",
          Node.GetId(), Node.GetId(), Node.GetNbrNId(e));
        if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
        RetVal=false;
      }
      prevNId = Node.GetNbrNId(e);
    }
  }
  int EdgeCnt = 0;
  for (TEdgeI EI = BegEI(); EI < EndEI(); EI++) { EdgeCnt++; }
  if (EdgeCnt != GetEdges()) {
    const TStr Msg = TStr::Fmt("Number of edges counter is corrupted: GetEdges():%d, EdgeCount:%d.", GetEdges(), EdgeCnt);
    if (ThrowExcept) { EAssertR(false, Msg); } else { ErrNotify(Msg.CStr()); }
    RetVal=false;
  }
  return RetVal;
}

// Print the graph in a human readable form to an output stream OutF.
void TUNRCGraph::Dump(FILE *OutF) const {
  const int NodePlaces = (int) ceil(log10((double) GetNodes()));
  fprintf(OutF, "-------------------------------------------------\nUndirected Node Graph: nodes: %d, edges: %d\n", GetNodes(), GetEdges());
  for (int N = NodeH.FFirstKeyId(); NodeH.FNextKeyId(N); ) {
    const TNode& Node = NodeH[N];
    fprintf(OutF, "  %*d [%d] ", NodePlaces, Node.GetId(), Node.GetDeg());
    for (int edge = 0; edge < Node.GetDeg(); edge++) {
      fprintf(OutF, " %*d", NodePlaces, Node.GetNbrNId(edge)); }
    fprintf(OutF, "\n");
  }
  fprintf(OutF, "\n");
}

// Return a small graph on 5 nodes and 5 edges.
PUNRCGraph TUNRCGraph::GetSmallGraph() {
  PUNRCGraph Graph = TUNRCGraph::New();
  for (int i = 0; i < 5; i++) { Graph->AddNode(i); }
  Graph->AddEdge(0,1);  Graph->AddEdge(0,2);
  Graph->AddEdge(0,3);  Graph->AddEdge(0,4);
  Graph->AddEdge(1,2);
  return Graph;
}
 */


