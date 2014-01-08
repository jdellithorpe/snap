namespace TSnap {

void RCGetTriadsNode(const PUNRCGraph& Graph, uint64_t NId, uint64_t& ClosedTriads, uint64_t& OpenTriads) {
  TUNRCGraph::TNode Node = Graph->GetNode(NId);
  TVec<TUNRCGraph::TNode> Nbrs;
  for(uint64_t e = 0; e<Node.GetDeg(); e++) {
    Nbrs.Add(Graph->GetNode(Node.GetNbrNId(e)));
  }
  ClosedTriads = 0;
  OpenTriads = 0;
  for(uint64_t src = 0; src<Nbrs.Len(); src++) {
    for(uint64_t dst = src + 1; dst<Nbrs.Len(); dst++) {
      if(Nbrs[src].IsNbrNId(Nbrs[dst].GetId()))
        ClosedTriads++;
      else
        OpenTriads++;
    }
  }
}

void RCGetTriads(const PUNRCGraph& Graph, TUInt64TrV& NIdCOTriadV, int SampleNodes) {
  TUNRCGraph::TNodeI NI = Graph->BegNI();
  uint64_t ClosedTriads, OpenTriads;

  NIdCOTriadV.Clr(false);
  if(SampleNodes == -1) {
    do {
      RCGetTriadsNode(Graph, NI.GetNode().GetId(), ClosedTriads, OpenTriads);
      NIdCOTriadV.Add(TUInt64Tr((TUInt64)NI.GetNode().GetId(), (TUInt64)ClosedTriads, (TUInt64)OpenTriads));
    } while(NI++);
  } else {
    int count = 0;
    NIdCOTriadV.Reserve(SampleNodes);
    do {
      RCGetTriadsNode(Graph, NI.GetNode().GetId(), ClosedTriads, OpenTriads);
      NIdCOTriadV.Add(TUInt64Tr((TUInt64)NI.GetNode().GetId(), (TUInt64)ClosedTriads, (TUInt64)OpenTriads));
    } while(NI++ && (++count < SampleNodes));
  }
}

double RCGetClustCf(const PUNRCGraph& Graph, int SampleNodes) {
  TUInt64TrV NIdCOTriadV;
  RCGetTriads(Graph, NIdCOTriadV, SampleNodes);

  if (NIdCOTriadV.Empty()) { return 0.0; }
  double SumCcf = 0.0;
  for (int i = 0; i < NIdCOTriadV.Len(); i++) {
    const double OpenCnt = NIdCOTriadV[i].Val2+NIdCOTriadV[i].Val3;
    if (OpenCnt > 0) {
      SumCcf += (double)NIdCOTriadV[i].Val2 / OpenCnt; }
  }
  return SumCcf / double(NIdCOTriadV.Len());
}



} // namespace TSnap
