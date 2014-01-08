namespace TSnap {

/////////////////////////////////////////////////
// Triads and clustering coefficient

void RCGetTriadsNode(const PUNRCGraph& Graph, uint64_t NId, uint64_t& ClosedTriad, uint64_t& OpenTriads);
void RCGetTriads(const PUNRCGraph& Graph, TUInt64TrV& NIdCOTriadV, int SampleNodes);

double RCGetClustCf(const PUNRCGraph& Graph, int SampleNodes=-1);

}

