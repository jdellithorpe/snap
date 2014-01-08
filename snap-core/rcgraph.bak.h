//#//////////////////////////////////////////////
/// Undirected graphs

class TUNRCGraph;

/// Pointer to an undirected graph (TUNGraph)
typedef TPt<TUNRCGraph> PUNRCGraph;

//#//////////////////////////////////////////////
/// Undirected graph. ##Undirected_graph
class TUNRCGraph {
public:
  typedef TUNRCGraph TNet;
  typedef TPt<TUNRCGraph> PNet;
public:
  class TNode {
  private:
    uint64_t Id;
    std::vector<uint64_t> NIdV;
    PUNRCGraph G;
  public:
    TNode(PUNRCGraph G) : Id(), NIdV(), G(G) { }
    TNode(PUNRCGraph G, const uint64_t& NId) : Id(NId), NIdV(), G(G) { }
    TNode(PUNRCGraph G, const uint64_t& NId, std::vector<uint64_t>& Nbrs) : Id(NId), NIdV(Nbrs), G(G) { }
    TNode(PUNRCGraph G, const TNode& Node) : Id(Node.Id), NIdV(Node.NIdV), G(G) { }
    /*TNode(TSIn& SIn) : Id(SIn), NIdV(SIn) { }*/
    /*void Save(TSOut& SOut) const { Id.Save(SOut); NIdV.Save(SOut); }*/
    int GetId() const { return Id; }
    int GetDeg() const { return NIdV.size(); }
    int GetInDeg() const { return GetDeg(); }
    int GetOutDeg() const { return GetDeg(); }
    int GetInNId(const uint64_t& NodeN) const { return GetNbrNId(NodeN); }
    int GetOutNId(const uint64_t& NodeN) const { return GetNbrNId(NodeN); }
    int GetNbrNId(const uint64_t& NodeN) const { return NIdV[NodeN]; }
    bool IsNbrNId(const uint64_t& NId) const;
    bool IsInNId(const uint64_t& NId) const { return IsNbrNId(NId); }
    bool IsOutNId(const uint64_t& NId) const { return IsNbrNId(NId); }

    /*void PackOutNIdV() { NIdV.Pack(); }*/
    /*void PackNIdV() { NIdV.Pack(); }*/
    friend class TUNRCGraph;
    friend class TUNGraphMtx;
  };

  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    TNode Node;
    PUNRCGraph G;
    RAMCloud::TableEnumerator tableEnum;
    /*typedef THash<TInt, TNode>::TIter THashIter;*/
    /*THashIter NodeHI;*/
  public:
    /*TNodeI() : NodeHI() { }*/
    /*TNodeI(const THashIter& NodeHIter) : NodeHI(NodeHIter) { }*/
    /*TNodeI(const TNodeI& NodeI) : NodeHI(NodeI.NodeHI) { }*/
    /*TNodeI& operator = (const TNodeI& NodeI) { NodeHI = NodeI.NodeHI; return *this; }*/

    TNodeI(PUNRCGraph G);
    TNodeI(const TNodeI& TNodeI);
    /*TNodeI(PUNRCGraph G, const TNode& Node);*/
    /*TNodeI& operator= (const TNodeI& NodeI) { Node = NodeI.Node, G = NodeI.G, tableEnum = NodeI.tableEnum; return *this; }*/

    TNodeI& operator++ (int);

    /*bool operator < (const TNodeI& NodeI) const { return NodeHI < NodeI.NodeHI; }*/
    /*bool operator == (const TNodeI& NodeI) const { return Node == NodeI.Node; }*/

    /// Returns ID of the current node.
    int GetId() const { return Node.GetId(); }
    /// Returns degree of the current node.
    int GetDeg() const { return Node.GetDeg(); }
    /// Returns in-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetInDeg() const { return Node.GetInDeg(); }
    /// Returns out-degree of the current node (returns same as value GetDeg() since the graph is undirected).
    int GetOutDeg() const { return Node.GetOutDeg(); }
    /// Returns ID of NodeN-th in-node (the node pointing to the current node). ##TUNGraph::TNodeI::GetInNId
    int GetInNId(const int& NodeN) const { return Node.GetInNId(NodeN); }
    /// Returns ID of NodeN-th out-node (the node the current node points to). ##TUNGraph::TNodeI::GetOutNId
    int GetOutNId(const int& NodeN) const { return Node.GetOutNId(NodeN); }
    /// Returns ID of NodeN-th neighboring node. ##TUNGraph::TNodeI::GetNbrNId
    int GetNbrNId(const int& NodeN) const { return Node.GetNbrNId(NodeN); }
    /// Tests whether node with ID NId points to the current node.
    bool IsInNId(const int& NId) const { return Node.IsInNId(NId); }
    /// Tests whether the current node points to node with ID NId.
    bool IsOutNId(const int& NId) const { return Node.IsOutNId(NId); }
    /// Tests whether node with ID NId is a neighbor of the current node.
    bool IsNbrNId(const int& NId) const { return Node.IsNbrNId(NId); }
    friend class TUNRCGraph;
  };

  /// Edge iterator. Only forward iteration (operator++) is supported.
  /*
  class TEdgeI {
  private:
    TNodeI CurNode, EndNode;
    int CurEdge;
    PUNRCGraph G;
  public:
    TEdgeI() : CurNode(), EndNode(), CurEdge(0) { }
    TEdgeI(const TNodeI& NodeI, const TNodeI& EndNodeI, const int& EdgeN=0) : CurNode(NodeI), EndNode(EndNodeI), CurEdge(EdgeN) { }
    TEdgeI(const TEdgeI& EdgeI) : CurNode(EdgeI.CurNode), EndNode(EdgeI.EndNode), CurEdge(EdgeI.CurEdge) { }
    TEdgeI& operator = (const TEdgeI& EdgeI) { if (this!=&EdgeI) { CurNode=EdgeI.CurNode; EndNode=EdgeI.EndNode; CurEdge=EdgeI.CurEdge; } return *this; }
    /// Increment iterator.
    TEdgeI& operator++ (int) { do { CurEdge++; if (CurEdge >= CurNode.GetOutDeg()) { CurEdge=0; CurNode++; while (CurNode < EndNode && CurNode.GetOutDeg()==0) { CurNode++; } } } while (CurNode < EndNode && GetSrcNId()>GetDstNId()); return *this; }
    bool operator < (const TEdgeI& EdgeI) const { return CurNode<EdgeI.CurNode || (CurNode==EdgeI.CurNode && CurEdge<EdgeI.CurEdge); }
    bool operator == (const TEdgeI& EdgeI) const { return CurNode == EdgeI.CurNode && CurEdge == EdgeI.CurEdge; }
    /// Gets edge ID. Always returns -1 since only edges in multigraphs have explicit IDs.
    int GetId() const { return -1; }
    /// Gets the source of an edge. Since the graph is undirected this is the node with smaller ID of the edge endpoints.
    int GetSrcNId() const { return CurNode.GetId(); }
    /// Gets destination of an edge. Since the graph is undirected this is the node with greater ID of the edge endpoints.
    int GetDstNId() const { return CurNode.GetOutNId(CurEdge); }
    friend class TUNRCGraph;
  };
  */
private:
  TCRef CRef;
  uint64_t MxNId, NEdges;
  RAMCloud::RamCloud RamCloud;
  const string AdjTable_Name;
  const uint32_t AdjTable_ServerSpan;
  uint64_t AdjTable_Id;
private:
  /*TNode& GetNode(const int& NId) { return NodeH.GetDat(NId); }*/
  /*const TNode& GetNode(const int& NId) const { return NodeH.GetDat(NId); }*/
public:
  TUNRCGraph() : CRef(), MxNId(0), NEdges(0), RamCloud("infrc:host=192.168.1.101,port=12246"), AdjTable_Name("Adjacency Table"), AdjTable_ServerSpan(1) {
    AdjTable_Id = RamCloud.createTable(AdjTable_Name.c_str(), AdjTable_ServerSpan);
  }
  TUNRCGraph(const char* coordinatorLoc, const uint32_t serverSpan) : CRef(), MxNId(0), NEdges(0), RamCloud(coordinatorLoc), AdjTable_Name("Adjacency Table"), AdjTable_ServerSpan(serverSpan) {
    AdjTable_Id = RamCloud.createTable(AdjTable_Name.c_str(), AdjTable_ServerSpan);
  }
  /// Static constructor that returns a pointer to the graph. Call: PUNGraph Graph = TUNGraph::New().
  static PUNRCGraph New() { return new TUNRCGraph(); }
  static PUNRCGraph New(const char* coordinatorLoc, const uint32_t serverSpan) { return new TUNRCGraph(coordinatorLoc, serverSpan); }
  /// Allows for run-time checking the type of the graph (see the TGraphFlag for flags).
  bool HasFlag(const TGraphFlag& Flag) const;
  /*TUNRCGraph& operator = (const TUNRCGraph& Graph) {
    if (this!=&Graph) { MxNId=Graph.MxNId; NEdges=Graph.NEdges; RamCloud=Graph.RamCloud; } return *this; }*/
  
  /// Returns the number of nodes in the graph.
  // TODO: Currently no efficient way of finding the number of nodes in the graph (other than a table scan)
  /*int GetNodes() const { return NodeH.Len(); }*/
  /// Add a node with a unique ID to the graph.
  uint64_t AddNode();
  /// Adds a node of ID NId to the graph. ##TUNGraph::AddNode
  uint64_t AddNode(const uint64_t NId);
  /// Adds a node of ID NodeI.GetId() to the graph.
  /*int AddNode(const TNodeI& NodeI) { return AddNode(NodeI.GetId()); }*/
  /// Adds a node of ID NId to the graph and create edges to all nodes in vector NbrNIdV. ##TUNGraph::AddNode-1
  uint64_t AddNode(const uint64_t& NId, const std::vector<uint64_t>& Nbrs, const bool& safetyChecks = false);
  /// Adds a batch of nodes to the graph and create the specified edges.
  uint64_t AddNodes(const std::vector<uint64_t>& NodeIdVec, const std::vector<std::vector<uint64_t> >& NbrsVec);
  /// Adds a node of ID NId to the graph and create edges to all nodes in vector NIdVId in the vector pool Pool. ##TUNGraph::AddNode-2
  /*int AddNode(const int& NId, const TVecPool<TInt>& Pool, const int& NIdVId);*/
  /// Deletes node of ID NId from the graph. ##TUNGraph::DelNode
  /*void DelNode(const int& NId);*/
  /// Deletes node of ID NodeI.GetId() from the graph.
  /*void DelNode(const TNode& NodeI) { DelNode(NodeI.GetId()); }*/
  /// Tests whether ID NId is a node.
  bool IsNode(const uint64_t& NId);
  /// Returns an iterator referring to the first node in the graph.
  TNodeI BegNI() { return TUNRCGraph::TNodeI(this); }
  /// Returns an iterator referring to the past-the-end node in the graph.
  /*TNodeI EndNI() const { return TNodeI(NodeH.EndI()); }*/
  /// Returns an iterator referring to the node of ID NId in the graph.
  /*TNodeI GetNI(const int& NId) const { return TNodeI(NodeH.GetI(NId)); }*/
  /// Returns node of ID NId in the graph.
  TNode GetNode(const uint64_t& NId);
  /// Returns the maximum id of a any node in the graph.
  /*int GetMxNId() const { return MxNId; }*/

  /// Returns the number of edges in the graph.
  /*int GetEdges() const;*/
  /// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TUNGraph::AddEdge
  int AddEdge(const uint64_t& SrcNId, const uint64_t& DstNId);
  /// Adds an edge between EdgeI.GetSrcNId() and EdgeI.GetDstNId() to the graph.
  /*int AddEdge(const TEdgeI& EdgeI) { return AddEdge(EdgeI.GetSrcNId(), EdgeI.GetDstNId()); }*/
  /// Deletes an edge between node IDs SrcNId and DstNId from the graph. ##TUNGraph::DelEdge
  /*void DelEdge(const int& SrcNId, const int& DstNId);*/
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  /*bool IsEdge(const int& SrcNId, const int& DstNId) const;*/
  /// Returns an iterator referring to the first edge in the graph.
  /*TEdgeI BegEI() const { TNodeI NI = BegNI(); TEdgeI EI(NI, EndNI(), 0); if (GetNodes() != 0 && (NI.GetOutDeg()==0 || NI.GetId()>NI.GetOutNId(0))) { EI++; } return EI; }*/
  /// Returns an iterator referring to the past-the-end edge in the graph.
  /*TEdgeI EndEI() const { return TEdgeI(EndNI(), EndNI()); }*/
  /// Not supported/implemented!
  /*TEdgeI GetEI(const int& EId) const;*/
  /// Returns an iterator referring to edge (SrcNId, DstNId) in the graph. ##TUNGraph::GetEI
  /*TEdgeI GetEI(const int& SrcNId, const int& DstNId) const;*/

  /// Returns an ID of a random node in the graph.
  /*int GetRndNId(TRnd& Rnd=TInt::Rnd) { return NodeH.GetKey(NodeH.GetRndKeyId(Rnd, 0.8)); }*/
  /// Returns an interator referring to a random node in the graph.
  /*TNodeI GetRndNI(TRnd& Rnd=TInt::Rnd) { return GetNI(GetRndNId(Rnd)); }*/
  /// Gets a vector IDs of all nodes in the graph.
  /*void GetNIdV(TIntV& NIdV) const;*/

  /// Tests whether the graph is empty (has zero nodes).
  /*bool Empty() const { return GetNodes()==0; }*/
  /// Deletes all nodes and edges from the graph.
  /*void Clr() { MxNId=0; NEdges=0; NodeH.Clr(); }*/
  /// Reserves memory for a graph of Nodes nodes and Edges edges.
  void Reserve(const int& Nodes, const int& Edges) { return; }
  /// Reserves memory for node ID NId having Deg edges.
  /*void ReserveNIdDeg(const int& NId, const int& Deg) { GetNode(NId).NIdV.Reserve(Deg); }*/
  /// Defragments the graph. ##TUNGraph::Defrag
  /*void Defrag(const bool& OnlyNodeLinks=false);*/
  /// Checks the graph data structure for internal consistency. ##TUNGraph::IsOk
  /*bool IsOk(const bool& ThrowExcept=true) const;*/
  /// Print the graph in a human readable form to an output stream OutF.
  /*void Dump(FILE *OutF=stdout) const;*/
  /// Returns a small graph on 5 nodes and 5 edges. ##TUNGraph::GetSmallGraph
  /*static PUNRCGraph GetSmallGraph();*/

  void ClearRamCloudData() {
    RamCloud.dropTable(AdjTable_Name.c_str());
  }

  friend class TUNGraphMtx;
  friend class TPt<TUNRCGraph>;
};