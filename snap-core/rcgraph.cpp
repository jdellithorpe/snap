
/// TNode
/// Tests whether NId is a neighbor.
bool TUNRCGraph::TNode::IsNbrNId(const uint64_t NId) const {
  for(uint64_t i = 0; i < NIdV.size(); i++) {
    if(NIdV[i] == NId)
      return true;
  }
  return false;
}

/// TNodeI
/// Constructor
TUNRCGraph::TNodeI::TNodeI(PUNRCGraph G) : G(G), tableEnum(G->RamCloud, G->AdjTable_Id, false)  {
  this->operator ++(1);
}

TUNRCGraph::TNodeI::TNodeI(const TNodeI& NodeI) : Node(NodeI.G), G(NodeI.G), tableEnum(NodeI.G->RamCloud, NodeI.G->AdjTable_Id, false) {
  this->operator ++(1);
}

int TUNRCGraph::TNodeI::operator++ (int) {
  uint32_t size = 0;
  const void* buffer = 0;

  tableEnum.next(&size, &buffer);
  if(buffer == NULL)
    return 0;

  RAMCloud::Object object(buffer, size);

  uint64_t* NIdPtr = (uint64_t*)object.getKey();
  uint64_t* NbrArr = (uint64_t*)object.getData();

  std::vector<uint64_t> NbrVec(NbrArr, NbrArr + (object.getDataLength()/sizeof(uint64_t)));
  Node = TUNRCGraph::TNode(G, *NIdPtr, NbrVec);

  return 1;
}

/// TUNRCGraph
/// Adds a node of ID NId to the graph. ##TUNGraph::AddNode
uint64_t TUNRCGraph::AddNode(const uint64_t NId, const bool safetyChecks) {
  if(safetyChecks)
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));

  std::vector<uint64_t> Nbrs(0);
  RamCloud.write(AdjTable_Id, (char*)&NId, sizeof(uint64_t), Nbrs.data(), Nbrs.size()*sizeof(uint64_t));
  return NId;
}

/// Adds a node of ID NId to the graph and create edges to all nodes in vector NbrNIdV. ##TUNGraph::AddNode-1
uint64_t TUNRCGraph::AddNode(const uint64_t NId, const std::vector<uint64_t>& Nbrs, const bool safetyChecks) {
  if(safetyChecks)
    IAssertR(!IsNode(NId), TStr::Fmt("NodeId %d already exists", NId));

  RamCloud.write(AdjTable_Id, (char*)&NId, sizeof(uint64_t), Nbrs.data(), Nbrs.size()*sizeof(uint64_t));
  return NId;
}

/// Adds a batch of nodes to the graph and create the specified edges.
uint64_t TUNRCGraph::AddNodes(const std::vector<uint64_t>& NodeIdVec, const std::vector<std::vector<uint64_t> >& NbrsVec, const bool safetyChecks) {
  RAMCloud::MultiWriteObject* requests[NodeIdVec.size()];
  RAMCloud::Tub<RAMCloud::MultiWriteObject> objects[NodeIdVec.size()];

  uint64_t totalBytesWritten = 0;

  for(uint64_t i = 0; i < NodeIdVec.size(); i++) {
    objects[i].construct( AdjTable_Id,
                          (char*)&NodeIdVec[i],
                          sizeof(uint64_t),
                          (char*)NbrsVec[i].data(),
                          NbrsVec[i].size()*sizeof(uint64_t) );
    requests[i] = objects[i].get();
    totalBytesWritten += requests[i]->keyLength + requests[i]->valueLength;
  }

  try {
    RamCloud.multiWrite(requests, NodeIdVec.size());
  } catch(RAMCloud::ClientException& e) {
    fprintf(stderr, "RAMCloud exception: %s\n", e.str().c_str());
    return -1;
  }

  return totalBytesWritten;
}

/// Adds all node in range [startId, endId] inclusive
void AddNodeRange(const char* coordinatorLoc, const uint64_t tableId, const uint64_t startId, const uint64_t endId, const uint64_t batchSize) {
  RAMCloud::RamCloud RamCloud(coordinatorLoc);

  RAMCloud::MultiWriteObject* requests[batchSize];
  RAMCloud::Tub<RAMCloud::MultiWriteObject> objects[batchSize];

  char keys[batchSize][sizeof(uint64_t)];

  // Prep all the request objects
  for(uint64_t i=0; i<batchSize; i++) {
    memset(keys[i], 0, sizeof(uint64_t));

    objects[i].construct( tableId,
                          (char*)keys[i],
                          sizeof(uint64_t),
                          (char*)keys[i],
                          sizeof(uint64_t) );
    requests[i] = objects[i].get();
  }

  uint64_t count = 0;
  for(uint64_t key = startId; key<=endId; key++) {
    memcpy(keys[count], (char*)&key, sizeof(uint64_t));
    count++;
    if(count == batchSize) {
      try {
        RamCloud.multiWrite(requests, batchSize);
      } catch(RAMCloud::ClientException& e) {
        fprintf(stderr, "RAMCloud exception: %s\n", e.str().c_str());
        return;
      }
      count = 0;
    }
  }

  if(count) {
    try {
      RamCloud.multiWrite(requests, count);
    } catch(RAMCloud::ClientException& e) {
      fprintf(stderr, "RAMCloud exception: %s\n", e.str().c_str());
      return;
    }
  }
}

/// Adds nodes numbered 0 to Nodes-1 to the graph in bulk using multithreading
uint64_t TUNRCGraph::AddNodes(const uint64_t Nodes, uint64_t threads, uint64_t batchSize) {
  uint64_t sectionWidth = Nodes/threads;
  boost::thread_group tgroup;
  for(uint64_t i = 0; i<threads; i++) {
    if( i != threads-1 )
      tgroup.create_thread(boost::bind(&AddNodeRange, coordinatorLoc.c_str(), AdjTable_Id, i*sectionWidth, (i+1)*sectionWidth - 1, batchSize));
    else
      tgroup.create_thread(boost::bind(&AddNodeRange, coordinatorLoc.c_str(), AdjTable_Id, i*sectionWidth, Nodes-1, batchSize));
  }
  tgroup.join_all();
}

/// Deletes node of ID NId from the graph. ##TUNGraph::DelNode
void TUNRCGraph::DelNode(const uint64_t NId) {
  // TODO: Implement
}

/// Tests whether ID NId is a node.
bool TUNRCGraph::IsNode(const uint64_t NId) {
  RAMCloud::Buffer buffer;
  try {
    RamCloud.read(AdjTable_Id, (char*)&NId, sizeof(uint64_t), &buffer);
  } catch (RAMCloud::ClientException& e) {
    return false;
  }
  return true;
}

/// Fetches node of ID NId from the graph.
TUNRCGraph::TNode TUNRCGraph::GetNode(const uint64_t NId) {
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

/// Adds an edge between node IDs SrcNId and DstNId to the graph. ##TUNGraph::AddEdge
int TUNRCGraph::AddEdge(const uint64_t SrcNId, const uint64_t DstNId) {
  TUNRCGraph::TNode SrcNode(this);
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
    SrcNode = TUNRCGraph::TNode(this, SrcNId, neighborVec);
  }

  if(SrcNId != DstNId) {
    // Not a loop
    TUNRCGraph::TNode DstNode(this);
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
      DstNode = TUNRCGraph::TNode(this, DstNId, neighborVec);
    }

    // Check whether or not the edge already exists
    if(SrcNode.IsNbrNId(DstNId))
      return -2;

    DstNode.NIdV.push_back(SrcNId);
    RamCloud.write(AdjTable_Id, (char*)&DstNode.Id, sizeof(uint64_t), DstNode.NIdV.data(), DstNode.NIdV.size()*sizeof(uint64_t));
  }

  SrcNode.NIdV.push_back(DstNId);
  RamCloud.write(AdjTable_Id, (char*)&SrcNode.Id, sizeof(uint64_t), SrcNode.NIdV.data(), SrcNode.NIdV.size()*sizeof(uint64_t));

  return -1;
}

// helper function
bool IsInVector(const uint64_t NId, std::vector<uint64_t>& NIdV) {
  for(uint64_t i = 0; i < NIdV.size(); i++) {
    if(NIdV[i] == NId)
      return true;
  }
  return false;
}

uint64_t TUNRCGraph::AddEdges(std::vector<std::pair<uint64_t,uint64_t> >& Edges) {
  // construct the read set
  std::map<uint64_t, std::vector<uint64_t> > NodeMap;
  std::map<uint64_t, std::vector<uint64_t> > ModifiedNodeMap;

  for(uint64_t i = 0; i<Edges.size(); i++) {
    NodeMap[Edges[i].first] = std::vector<uint64_t>({});
    NodeMap[Edges[i].second] = std::vector<uint64_t>({});
  }

  RAMCloud::MultiReadObject* read_requests[NodeMap.size()];
  RAMCloud::Tub<RAMCloud::Buffer> values[NodeMap.size()];
  RAMCloud::Tub<RAMCloud::MultiReadObject> read_objects[NodeMap.size()];

  // construct the multiread
  std::map<uint64_t, std::vector<uint64_t> >::iterator it;
  uint64_t count = 0;
  for(it = NodeMap.begin(); it!=NodeMap.end(); ++it) {
    read_objects[count].construct( AdjTable_Id,
                              (char*)&(it->first),
                              sizeof(uint64_t),
                              &values[count] );
    read_requests[count] = read_objects[count].get();
    count++;
  }

  // perform multiread
  try {
    RamCloud.multiRead(read_requests, NodeMap.size());
  } catch(RAMCloud::ClientException& e) {
  }

  // parse multiread
  for(uint64_t i = 0; i < NodeMap.size(); i++) {
    if(values[i].get()) {
      string adjlist(static_cast<const char*>(values[i].get()->getRange(0, values[i].get()->getTotalLength())), values[i].get()->getTotalLength());
      uint64_t* neighborArray = (uint64_t*)adjlist.c_str();
      std::vector<uint64_t> neighborVec(neighborArray, neighborArray + (values[i].get()->getTotalLength()/sizeof(uint64_t)));

      NodeMap[*(uint64_t*)read_requests[i]->key] = neighborVec;
    }
  }

  // add edges
  uint64_t edgesAdded = 0;
  for(uint64_t i = 0; i<Edges.size(); i++) {
    if(!IsInVector(Edges[i].second, NodeMap[Edges[i].first])) {
      NodeMap[Edges[i].first].push_back(Edges[i].second);
      NodeMap[Edges[i].second].push_back(Edges[i].first);
      ModifiedNodeMap[Edges[i].first] = NodeMap[Edges[i].first];
      ModifiedNodeMap[Edges[i].second] = NodeMap[Edges[i].second];
      edgesAdded++;
    }
  }

  RAMCloud::MultiWriteObject* write_requests[ModifiedNodeMap.size()];
  RAMCloud::Tub<RAMCloud::MultiWriteObject> write_objects[ModifiedNodeMap.size()];

  // construct the multiwrite
  count = 0;
  for(it = ModifiedNodeMap.begin(); it!=ModifiedNodeMap.end(); ++it) {
    write_objects[count].construct( AdjTable_Id,
                                    (char*)&(it->first),
                                    sizeof(uint64_t),
                                    (char*)(it->second.data()),
                                    it->second.size()*sizeof(uint64_t) );
    write_requests[count] = write_objects[count].get();
    count++;
  }

  // perform the multiwrite
  try {
    RamCloud.multiWrite(write_requests, ModifiedNodeMap.size());
  } catch(RAMCloud::ClientException& e) {
    fprintf(stderr, "RAMCloud exception: %s\n", e.str().c_str());
    return 0;
  }

  return edgesAdded;
}

/// Deletes an edge between node IDs SrcNId and DstNId from the graph. ##TUNGraph::DelEdge
void TUNRCGraph::DelEdge(const int SrcNId, const int DstNId) {
  // TODO: Implement
}

/// Tests whether an edge between node IDs SrcNId and DstNId exists in the graph.
bool TUNRCGraph::IsEdge(const int SrcNId, const int DstNId) const {
  // TODO: Implement
  return false;
}

/// Deletes all nodes and edges from the graph.
void TUNRCGraph::Clr() {
  // TODO: Implement
}
