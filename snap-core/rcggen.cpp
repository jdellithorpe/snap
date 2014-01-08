/////////////////////////////////////////////////
// Graph Generators
namespace TSnap {

PUNRCGraph RCGenGrid(const char* GraphName, const uint64_t& Rows, const uint64_t& Cols, const char* coordinatorLoc, const uint32_t serverSpan) {
  PUNRCGraph GraphPt = new TUNRCGraph(GraphName, coordinatorLoc, serverSpan);
  PUNRCGraph::TObj& Graph = *GraphPt;
  uint64_t r, c;
  for (r = 0; r < Rows; r++) {
    for (c = 0; c < Cols; c++) {
      uint64_t nodeId = Cols*r + c;
      uint64_t left   = nodeId - 1;
      uint64_t right  = nodeId + 1;
      uint64_t top    = nodeId - Cols;
      uint64_t bottom = nodeId + Cols;

      if (r == 0) { // top row
        if(c == 0) { // top left corner
          Graph.AddNode(nodeId, std::vector<uint64_t>({right, bottom}));
        } else if(c == Cols-1) { // top right corner
          Graph.AddNode(nodeId, std::vector<uint64_t>({left, bottom}));
        } else { // top edge
          Graph.AddNode(nodeId, std::vector<uint64_t>({left, right, bottom}));
        }
      } else if(r == Rows-1) { // bottom row
        if(c == 0) { // bottom left corner
          Graph.AddNode(nodeId, std::vector<uint64_t>({right, top}));
        } else if(c == Cols-1) { // bottom right corner
          Graph.AddNode(nodeId, std::vector<uint64_t>({left, top}));
        } else { // bottom edge
          Graph.AddNode(nodeId, std::vector<uint64_t>({left, right, top}));
        }
      } else { // middle rows
        if(c == 0) { // left edge
          Graph.AddNode(nodeId, std::vector<uint64_t>({right, top, bottom}));
        } else if(c == Cols-1) { // right edge
          Graph.AddNode(nodeId, std::vector<uint64_t>({left, top, bottom}));
        } else { // center of grid
          Graph.AddNode(nodeId, std::vector<uint64_t>({left, right, top, bottom}));
        }
      }
    }
  }
  return GraphPt;
}

PUNRCGraph RCGenGridMW(const char* GraphName, const uint64_t& Rows, const uint64_t& Cols, const uint64_t& batchSize, const char* coordinatorLoc, const uint32_t serverSpan) {
  PUNRCGraph GraphPt = new TUNRCGraph(GraphName, coordinatorLoc, serverSpan);
  PUNRCGraph::TObj& Graph = *GraphPt;

  std::vector<uint64_t> NodeIdVec;
  std::vector<std::vector<uint64_t> > NbrsVec;

  uint64_t stat_addnodes_start = 0;
  uint64_t stat_addnodes_acc = 0;

  uint64_t r, c;
  for (r = 0; r < Rows; r++) {
    for (c = 0; c < Cols; c++) {
      uint64_t nodeId = Cols*r + c;
      uint64_t left   = nodeId - 1;
      uint64_t right  = nodeId + 1;
      uint64_t top    = nodeId - Cols;
      uint64_t bottom = nodeId + Cols;

      NodeIdVec.push_back(nodeId);

      if (r == 0) { // top row
        if(c == 0) { // top left corner
          NbrsVec.push_back(std::vector<uint64_t>({right, bottom}));
        } else if(c == Cols-1) { // top right corner
          NbrsVec.push_back(std::vector<uint64_t>({left, bottom}));
        } else { // top edge
          NbrsVec.push_back(std::vector<uint64_t>({left, right, bottom}));
        }
      } else if(r == Rows-1) { // bottom row
        if(c == 0) { // bottom left corner
          NbrsVec.push_back(std::vector<uint64_t>({right, top}));
        } else if(c == Cols-1) { // bottom right corner
          NbrsVec.push_back(std::vector<uint64_t>({left, top}));
        } else { // bottom edge
          NbrsVec.push_back(std::vector<uint64_t>({left, right, top}));
        }
      } else { // middle rows
        if(c == 0) { // left edge
          NbrsVec.push_back(std::vector<uint64_t>({right, top, bottom}));
        } else if(c == Cols-1) { // right edge
          NbrsVec.push_back(std::vector<uint64_t>({left, top, bottom}));
        } else { // center of grid
          NbrsVec.push_back(std::vector<uint64_t>({left, right, top, bottom}));
        }
      }

      if(NodeIdVec.size() == batchSize) {
        stat_addnodes_start = RAMCloud::Cycles::rdtsc();
        Graph.AddNodes(NodeIdVec, NbrsVec);
        stat_addnodes_acc += RAMCloud::Cycles::rdtsc() - stat_addnodes_start;
        NodeIdVec.clear();
        NbrsVec.clear();
      }
    }
  }

  if(NodeIdVec.size() != 0) {
    stat_addnodes_start = RAMCloud::Cycles::rdtsc();
    Graph.AddNodes(NodeIdVec, NbrsVec);
    stat_addnodes_acc += RAMCloud::Cycles::rdtsc() - stat_addnodes_start;
    NodeIdVec.clear();
    NbrsVec.clear();
  }

  printf("Total time for calling AddNodes: %f seconds\n", RAMCloud::Cycles::toSeconds(stat_addnodes_acc));

  return GraphPt;
}

PUNRCGraph RCGenGridStripMW(const char* GraphName, const uint64_t& RowStart, const uint64_t& RowEnd, const uint64_t& Rows, const uint64_t& Cols, const uint64_t& batchSize, const char* coordinatorLoc, const uint32_t serverSpan) {
  printf("started thread with RowStart: %d\tRowEnd: %d\n", RowStart, RowEnd);

  PUNRCGraph GraphPt = new TUNRCGraph(GraphName, coordinatorLoc, serverSpan);
  PUNRCGraph::TObj& Graph = *GraphPt;

  std::vector<uint64_t> NodeIdVec;
  std::vector<std::vector<uint64_t> > NbrsVec;

  uint64_t totalBytesWritten = 0;
  uint64_t totalNodesWritten = 0;

  uint64_t r, c;
  for (r = RowStart; r <= RowEnd; r++) {
    for (c = 0; c < Cols; c++) {
      uint64_t nodeId = Cols*r + c;
      uint64_t left   = nodeId - 1;
      uint64_t right  = nodeId + 1;
      uint64_t top    = nodeId - Cols;
      uint64_t bottom = nodeId + Cols;

      NodeIdVec.push_back(nodeId);
      //NbrsVec.push_back(std::vector<uint64_t>({nodeId, nodeId, nodeId, nodeId}));

      if (r == 0) { // top row
        if(c == 0) { // top left corner
          NbrsVec.push_back(std::vector<uint64_t>({right, bottom}));
        } else if(c == Cols-1) { // top right corner
          NbrsVec.push_back(std::vector<uint64_t>({left, bottom}));
        } else { // top edge
          NbrsVec.push_back(std::vector<uint64_t>({left, right, bottom}));
        }
      } else if(r == Rows-1) { // bottom row
        if(c == 0) { // bottom left corner
          NbrsVec.push_back(std::vector<uint64_t>({right, top}));
        } else if(c == Cols-1) { // bottom right corner
          NbrsVec.push_back(std::vector<uint64_t>({left, top}));
        } else { // bottom edge
          NbrsVec.push_back(std::vector<uint64_t>({left, right, top}));
        }
      } else { // middle rows
        if(c == 0) { // left edge
          NbrsVec.push_back(std::vector<uint64_t>({right, top, bottom}));
        } else if(c == Cols-1) { // right edge
          NbrsVec.push_back(std::vector<uint64_t>({left, top, bottom}));
        } else { // center of grid
          NbrsVec.push_back(std::vector<uint64_t>({left, right, top, bottom}));
        }
      }

      totalNodesWritten++;

      if(totalNodesWritten % 20000000 == 0)
        printf("Thread %d hit %f GB over %d Nodes\n", RowStart, (float)totalBytesWritten/1.0e9, totalNodesWritten);

      if(NodeIdVec.size() == batchSize) {
        totalBytesWritten += Graph.AddNodes(NodeIdVec, NbrsVec);
        NodeIdVec.clear();
        NbrsVec.clear();
      }
    }
  }

  printf("Thread with RowStart: %d\tRowEnd: %d\t is wrapping up...\n", RowStart, RowEnd);
  if(NodeIdVec.size() != 0) {
    totalBytesWritten += Graph.AddNodes(NodeIdVec, NbrsVec);
    NodeIdVec.clear();
    NbrsVec.clear();
  }

  return GraphPt;
}

PUNRCGraph RCGenGridMWMT(const char* GraphName, const uint64_t& Rows, const uint64_t& Cols, const uint64_t& batchSize, const uint64_t& threads, const char* coordinatorLoc, const uint32_t serverSpan) {
  PUNRCGraph GraphPt = new TUNRCGraph(GraphName, coordinatorLoc, serverSpan);
  uint64_t stripWidth = Rows/threads;
  boost::thread_group tgroup;
  for(uint64_t i = 0; i<threads; i++) {
    if( i != threads-1 )
      tgroup.create_thread(boost::bind(&RCGenGridStripMW, GraphName, i*stripWidth, (i+1)*stripWidth - 1, Rows, Cols, batchSize, coordinatorLoc, serverSpan));
    else
      tgroup.create_thread(boost::bind(&RCGenGridStripMW, GraphName, i*stripWidth, Rows-1, Rows, Cols, batchSize, coordinatorLoc, serverSpan));
  }
  tgroup.join_all();
  return GraphPt;
}



PUNRCGraph RCGenRndGnm(const char* GraphName, const char* coordinatorLoc, const uint32_t serverSpan, const uint64_t threads, const uint64_t& Nodes, const uint64_t& Edges, TRnd& Rnd) {
  PUNRCGraph GraphPt = new TUNRCGraph(GraphName, coordinatorLoc, serverSpan);

  uint64_t stat_addnodes_start = 0;
  uint64_t stat_addnodes_acc = 0;
  uint64_t stat_addedges_start = 0;
  uint64_t stat_addedges_acc = 0;

  stat_addnodes_start = RAMCloud::Cycles::rdtsc();
  GraphPt->AddNodes(Nodes, threads, 8192);
  stat_addnodes_acc += RAMCloud::Cycles::rdtsc() - stat_addnodes_start;

  /*
  stat_addnodes_start = RAMCloud::Cycles::rdtsc();
  for(uint64_t node = 0; node < Nodes; node++)
    GraphPt->AddNode(node);
  stat_addnodes_acc += RAMCloud::Cycles::rdtsc() - stat_addnodes_start;
  */

  printf("Add Node Took: %f seconds, %d nodes\n", RAMCloud::Cycles::toSeconds(stat_addnodes_acc), Nodes);

  uint64_t batchSize = 1024;
  std::vector<std::pair<uint64_t, uint64_t> > EdgeSet;
  uint64_t totalEdgesAdded = 0;
  while(totalEdgesAdded < Edges) {
    uint64_t addAmount = ((batchSize)<(Edges-totalEdgesAdded)?(batchSize):(Edges-totalEdgesAdded));
    for(uint64_t i = 0; i<addAmount; ) {
      const uint64_t SrcNId = Rnd.GetUniDevUInt64(Nodes);
      const uint64_t DstNId = Rnd.GetUniDevUInt64(Nodes);
      if (SrcNId != DstNId) {
        EdgeSet.push_back(std::pair<uint64_t,uint64_t>(SrcNId,DstNId));
        i++;
      }
    }
    stat_addedges_start = RAMCloud::Cycles::rdtsc();
    totalEdgesAdded += GraphPt->AddEdges(EdgeSet);
    stat_addedges_acc += RAMCloud::Cycles::rdtsc() - stat_addedges_start;
    EdgeSet.clear();
  }

  /*
  for(uint64_t edge = 0; edge < Edges; ) {
    const uint64_t SrcNId = Rnd.GetUniDevUInt64(Nodes);
    const uint64_t DstNId = Rnd.GetUniDevUInt64(Nodes);
    if (SrcNId != DstNId) {
      std::vector<std::pair<uint64_t, uint64_t> > Edges = {std::pair<uint64_t,uint64_t>(SrcNId,DstNId)};
      stat_addedges_start = RAMCloud::Cycles::rdtsc();
      if(GraphPt->AddEdges(Edges) != 0) { // is new edge
        edge++;
      }
      stat_addedges_acc += RAMCloud::Cycles::rdtsc() - stat_addedges_start;
    }
  }
  */

  /*
  for(uint64_t edge = 0; edge < Edges; ) {
    const uint64_t SrcNId = Rnd.GetUniDevUInt64(Nodes);
    const uint64_t DstNId = Rnd.GetUniDevUInt64(Nodes);
    if (SrcNId != DstNId) {
      stat_addedges_start = RAMCloud::Cycles::rdtsc();
      if(GraphPt->AddEdge(SrcNId, DstNId) != -2) { // is new edge
        edge++;
      }
      stat_addedges_acc += RAMCloud::Cycles::rdtsc() - stat_addedges_start;
    }
  }
   */

  printf("Add Edge Took: %f seconds, %d edges\n", RAMCloud::Cycles::toSeconds(stat_addedges_acc), Edges);
  return GraphPt;
}



PUNRCGraph RCGenFull(const char* GraphName, const char* coordinatorLoc, const uint32_t serverSpan, const uint64_t threads, const uint64_t& Nodes) {
  PUNRCGraph Graph = new TUNRCGraph(GraphName, coordinatorLoc, serverSpan);

  Graph->AddNodes(Nodes, threads, 8192);

  for (uint64_t n1 = 0; n1 < Nodes; n1++) {
    for (uint64_t n2 = 0; n2 < Nodes; n2++) {
      if (n1 != n2) { Graph->AddEdge(n1, n2); }
    }
  }
  return Graph;
}











} // namespace TSnap
