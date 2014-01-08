/// New RamCloud Undirected Graph (reworking the API)

class TUNRCGraph;

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
    TNode() : Id(), NIdV() { }
    TNode(PUNRCGraph G) : Id(), NIdV(), G(G) { }
    TNode(PUNRCGraph G, const uint64_t NId) : Id(NId), NIdV(), G(G) { }
    TNode(PUNRCGraph G, const uint64_t NId, std::vector<uint64_t>& Nbrs) : Id(NId), NIdV(Nbrs), G(G) { }
    TNode(PUNRCGraph G, const TNode& Node) : Id(Node.Id), NIdV(Node.NIdV), G(G) { }

    uint64_t GetId() const { return Id; }
    uint64_t GetDeg() const { return NIdV.size(); }
    uint64_t GetInDeg() const { return GetDeg(); }
    uint64_t GetOutDeg() const { return GetDeg(); }
    uint64_t GetInNId(const uint64_t NodeN) const { return GetNbrNId(NodeN); }
    uint64_t GetOutNId(const uint64_t NodeN) const { return GetNbrNId(NodeN); }
    uint64_t GetNbrNId(const uint64_t NodeN) const { return NIdV[NodeN]; }
    bool IsNbrNId(const uint64_t NId) const;
    bool IsInNId(const uint64_t NId) const { return IsNbrNId(NId); }
    bool IsOutNId(const uint64_t NId) const { return IsNbrNId(NId); }

    friend class TUNRCGraph;
  };

  /// Node iterator. Only forward iteration (operator++) is supported.
  class TNodeI {
  private:
    TNode Node;
    PUNRCGraph G;
    RAMCloud::TableEnumerator tableEnum;
  public:
    TNodeI(PUNRCGraph G);
    TNodeI(const TNodeI& NodeI);

    int operator++ (int);

    TNode GetNode() { return Node; }

    friend class TUNRCGraph;
  };

private:
  TCRef CRef;
  const string GraphName;
  RAMCloud::RamCloud RamCloud;
  const string AdjTable_Name;
  const string coordinatorLoc;
  const uint32_t AdjTable_ServerSpan;
  uint64_t AdjTable_Id;
public:
  TUNRCGraph(const char* GraphName, const char* coordinatorLoc, const uint32_t serverSpan=1) : CRef(), GraphName(GraphName), coordinatorLoc(coordinatorLoc), RamCloud(coordinatorLoc), AdjTable_Name(GraphName), AdjTable_ServerSpan(serverSpan) {
    AdjTable_Id = RamCloud.createTable(AdjTable_Name.c_str(), AdjTable_ServerSpan);
  }

  /// Adds a node of ID NId to the graph. ##TUNGraph::AddNode
  uint64_t AddNode(const uint64_t NId, const bool safetyChecks=false);
  /// Adds a node of ID NId to the graph and create edges to all nodes in vector NbrNIdV. ##TUNGraph::AddNode-1
  uint64_t AddNode(const uint64_t NId, const std::vector<uint64_t>& Nbrs, const bool safetyChecks=false);
  /// Adds a batch of nodes to the graph and create the specified edges.
  uint64_t AddNodes(const std::vector<uint64_t>& NodeIdVec, const std::vector<std::vector<uint64_t> >& NbrsVec, const bool safetyChecks=false);
  /// Adds nodes numbered 0 to Nodes-1 to the graph in bulk using multithreading
  uint64_t AddNodes(const uint64_t Nodes, uint64_t threads, uint64_t batchSize);
  /// Get a Node Iterator
  TNodeI BegNI() { return TUNRCGraph::TNodeI(this); }
  /// Deletes node of ID NId from the graph. ##TUNGraph::DelNode
  void DelNode(const uint64_t NId);
  /// Tests whether ID NId is a node.
  bool IsNode(const uint64_t NId);
  /// Fetches node of ID NId from the graph.
  TNode GetNode(const uint64_t NId);

  /// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TUNGraph::AddEdge
  int AddEdge(const uint64_t SrcNId, const uint64_t DstNId);
  /// Add edges in bulk
  uint64_t AddEdges(std::vector<std::pair<uint64_t,uint64_t> >& Edges);
  /// Deletes an edge between node IDs SrcNId and DstNId from the graph. ##TUNGraph::DelEdge
  void DelEdge(const int SrcNId, const int DstNId);
  /// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
  bool IsEdge(const int SrcNId, const int DstNId) const;

  /// Deletes all nodes and edges from the graph.
  void Clr();
  /// Completely eliminate the graph from RAMCloud.
  void WipeOut() {
    RamCloud.dropTable(AdjTable_Name.c_str());
  }

  friend class TUNGraphMtx;
  friend class TPt<TUNRCGraph>;
};

