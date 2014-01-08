namespace TSnap {

void RCGetBfsTree(const PUNRCGraph& Graph, const PUNRCGraph& BfsTree, const uint64_t& StartNId, const bool& FollowOut, const bool& FollowIn) {

  std::queue<uint64_t> queue;
  std::set<uint64_t> seen_list;

  queue.push(StartNId);
  seen_list.insert(StartNId);

  BfsTree->AddNode(StartNId);

  while( !queue.empty() ) {
    uint64_t u = queue.front();
    queue.pop();

    TUNRCGraph::TNode NodeU = Graph->GetNode(u);

    for(uint64_t e = 0; e<NodeU.GetDeg(); e++) {
      uint64_t nbr = NodeU.GetNbrNId(e);
      if(seen_list.count(nbr) == 0) {
        BfsTree->AddNode(nbr);
        BfsTree->AddEdge(u, nbr);
        queue.push(nbr);
        seen_list.insert(nbr);
      }
    }
  }

}


} // namespace TSnap
