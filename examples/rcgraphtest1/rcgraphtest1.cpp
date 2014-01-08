#include "Snap.h"
#include <stdio.h>

/*
void exp01() {
  printf("exp01 start...\n");

  PUNRCGraph G = new TUNRCGraph("graph", "infrc:host=192.168.1.101,port=12246", 1);

  uint64_t a = G->AddNode();
  uint64_t b = G->AddNode();
  uint64_t c = G->AddNode();

  G->AddEdge(a,b);
  G->AddEdge(b,c);
  G->AddEdge(a,c);

  TUNRCGraph::TNode node = G->GetNode(a);

  for(int i = 0; i<node.GetDeg(); i++)
    printf("%d,", node.GetNbrNId(i));
  printf("\n");

  node = G->GetNode(b);

  for(int i = 0; i<node.GetDeg(); i++)
    printf("%d,", node.GetNbrNId(i));
  printf("\n");

  node = G->GetNode(c);

  for(int i = 0; i<node.GetDeg(); i++)
    printf("%d,", node.GetNbrNId(i));
  printf("\n");

  G->WipeOut();

  printf("exp01 end...\n");
}
*/

/*
void exp02() {
  printf("exp02 start...\n");

  PUNRCGraph G;
  uint64_t N = 10;
  uint64_t M = 10;
  G = TSnap::GenGrid<PUNRCGraph>(N, M, false);

  //TSnap::PrintInfo(G,"hoohaa","output.txt",false);

  G->WipeOut();

  printf("exp02 end...\n");
}
*/

/*
void exp03(uint64_t N, uint64_t M) {
  printf("exp03 start (SNAP GenGrid(%d, %d) on PUNRCGraph)...\n", N, M);

  PUNRCGraph G;

  printf("Generating graph...\n");
  uint64_t stat_gen_start = RAMCloud::Cycles::rdtsc();
  G = TSnap::GenGrid<PUNRCGraph>(N, M, false);
  uint64_t stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
  printf("Took: %f seconds\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed));
  printf("Perf: %f nodes/second\n", (float)N*M / RAMCloud::Cycles::toSeconds(stat_gen_elapsed));


  //TUNRCGraph::TNodeI nodeIter = G->BegNI();

  //for(uint64_t i = 0; i < N*M; i++) {
  //  printf("nodeId: %d\t", nodeIter.GetId());
  //  printf("neighbors: ");
  //  for(int i = 0; i<nodeIter.GetDeg(); i++)
  //      printf("%d,", nodeIter.GetNbrNId(i));
  //  printf("\n");
  //  nodeIter++;
  //}


  G->WipeOut();

  printf("exp03 end...\n");
}
*/

void exp04(uint64_t N, uint64_t M) {
  printf("exp04 start (SNAP GenGrid(%d, %d) on PUNGraph)...\n", N, M);

  PUNGraph G;

  printf("Generating graph...\n");
  uint64_t stat_gen_start = RAMCloud::Cycles::rdtsc();
  G = TSnap::GenGrid<PUNGraph>(N, M, false);
  uint64_t stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
  printf("Took: %f seconds\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed));
  printf("Perf: %f nodes/second\n", (float)N*M / RAMCloud::Cycles::toSeconds(stat_gen_elapsed));

  printf("exp04 end...\n");
}

void exp05(uint64_t N, uint64_t M, const char* coordinatorLoc="infrc:host=192.168.1.101,port=12246", const uint32_t serverSpan=1) {
  printf("exp05 start  (RamCloud GenGrid(%d, %d))...\n", N, M);

  PUNRCGraph G;

  printf("Generating graph...\n");
  uint64_t stat_gen_start = RAMCloud::Cycles::rdtsc();
  G = TSnap::RCGenGrid("graph", N, M, coordinatorLoc, serverSpan);
  uint64_t stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
  printf("Took: %f seconds\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed));
  printf("Perf: %f nodes/second\n", (float)N*M / RAMCloud::Cycles::toSeconds(stat_gen_elapsed));

  G->WipeOut();

  printf("exp05 end...\n");
}

void exp06(uint64_t N, uint64_t M, uint64_t batchSize=32, const char* coordinatorLoc="infrc:host=192.168.1.101,port=12246", const uint32_t serverSpan=1) {
  printf("exp06 start (RamCloud GenGrid(%d, %d) with Multiwrite batchSize %d with serverSpan %d)...\n", N, M, batchSize, serverSpan);

  PUNRCGraph G;

  printf("Generating graph...\n");
  uint64_t stat_gen_start = RAMCloud::Cycles::rdtsc();
  G = TSnap::RCGenGridMW("graph", N, M, batchSize, coordinatorLoc, serverSpan);
  uint64_t stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
  printf("Took: %f seconds\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed));
  printf("Perf: %f nodes/second\n", (float)N*M / RAMCloud::Cycles::toSeconds(stat_gen_elapsed));

  G->WipeOut();

  printf("exp06 end...\n");
}

void exp07(uint64_t N, uint64_t M, uint64_t batchSize=32, uint64_t threads=2, const char* coordinatorLoc="infrc:host=192.168.1.101,port=12246", const uint32_t serverSpan=1) {
  printf("exp07 start (RamCloud GenGrid(%d, %d) with Multiwrite batchSize %d and threads %d with serverSpan %d)...\n", N, M, batchSize, threads, serverSpan);

  PUNRCGraph G;

  printf("Generating graph...\n");
  uint64_t stat_gen_start = RAMCloud::Cycles::rdtsc();
  G = TSnap::RCGenGridMWMT("graph", N, M, batchSize, threads, coordinatorLoc, serverSpan);
  uint64_t stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
  printf("Took: %f seconds\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed));
  printf("Perf: %f nodes/second\n", (float)N*M / RAMCloud::Cycles::toSeconds(stat_gen_elapsed));

  G->WipeOut();

  printf("exp07 end...\n");
}

void exp08() {
  printf("exp08 start (Run RCGetTriadsNode)\n");
  PUNRCGraph G;
  uint64_t ClosedTriads, OpenTriads;
  G = TSnap::RCGenGridMWMT("graph", 100, 100, 1, 1, "infrc:host=192.168.1.101,port=12246", 1);
  TSnap::RCGetTriadsNode(G, 5005, ClosedTriads, OpenTriads);
  G->WipeOut();
  printf("ClosedTriads: %d, OpenTriads: %d\n", ClosedTriads, OpenTriads);
  printf("exp08 end...\n");
}

void exp09() {
  printf("exp09 start (Run RCGetTriadsNode)\n");
  PUNRCGraph G = new TUNRCGraph("graph", "infrc:host=192.168.1.101,port=12246", 1);
  std::vector<uint64_t> nbrs0 = {1,2,3,4};
  std::vector<uint64_t> nbrs1 = {0,2,3,4};
  std::vector<uint64_t> nbrs2 = {0,1,3,4};
  std::vector<uint64_t> nbrs3 = {0,1,2};
  std::vector<uint64_t> nbrs4 = {0,1,2};
  G->AddNode(0, nbrs0);
  G->AddNode(1, nbrs1);
  G->AddNode(2, nbrs2);
  G->AddNode(3, nbrs3);
  G->AddNode(4, nbrs4);

  uint64_t ClosedTriads, OpenTriads;
  TSnap::RCGetTriadsNode(G, 0, ClosedTriads, OpenTriads);
  G->WipeOut();
  printf("ClosedTriads: %d, OpenTriads: %d\n", ClosedTriads, OpenTriads);
  printf("exp09 end...\n");
}

void exp10() {
  printf("exp10 (enumerating the graph)\n");
  PUNRCGraph G;
  G = TSnap::RCGenGridMWMT("graph", 100, 100, 1, 1, "infrc:host=192.168.1.101,port=12246", 1);
  TUNRCGraph::TNodeI NI = G->BegNI();
  for(uint64_t i = 0; i<20000; i++) {
    TUNRCGraph::TNode node = NI.GetNode();
    printf("NodeID: %d\n", node.GetId());
    if(NI++ == 0)
      break;
  }
  printf("exp10 end...\n");
}

void exp11() {
  printf("exp11 start (Run RCGetTriads)\n");
  PUNRCGraph G;
  G = TSnap::RCGenGridMWMT("graph", 10, 10, 1, 1, "infrc:host=192.168.1.101,port=12246", 1);
  TUInt64TrV NIdCOTriadV;
  TSnap::RCGetTriads(G, NIdCOTriadV, -1);
  printf("printing it...\n");
  for(int i = 0; i<NIdCOTriadV.Len(); i++) {
    printf("%d\t%d, %d\n", (int)NIdCOTriadV[i].Val1, (int)NIdCOTriadV[i].Val2, (int)NIdCOTriadV[i].Val3);
  }

  G->WipeOut();
  printf("exp11 end...\n");
}

void exp12() {
  printf("exp12 start (Run RCGetClustCf)\n");
  PUNRCGraph G = new TUNRCGraph("graph", "infrc:host=192.168.1.101,port=12246", 1);
  std::vector<uint64_t> nbrs0 = {1,2,3,4};
  std::vector<uint64_t> nbrs1 = {0,2,3,4};
  std::vector<uint64_t> nbrs2 = {0,1,3,4};
  std::vector<uint64_t> nbrs3 = {0,1,2};
  std::vector<uint64_t> nbrs4 = {0,1,2};
  G->AddNode(0, nbrs0);
  G->AddNode(1, nbrs1);
  G->AddNode(2, nbrs2);
  G->AddNode(3, nbrs3);
  G->AddNode(4, nbrs4);
  double ClustCf = TSnap::RCGetClustCf(G, -1);
  printf("ClustCf: %f\n", ClustCf);

  G->WipeOut();
  printf("exp12 end...\n");
}

void exp13() {
  printf("exp13 start (Run RCGetBfsTree)\n");
  PUNRCGraph BfsTree = new TUNRCGraph("tree", "infrc:host=192.168.1.101,port=12246", 1);
  /*
  PUNRCGraph Graph = new TUNRCGraph("graph", "infrc:host=192.168.1.101,port=12246", 1);
  std::vector<uint64_t> nbrs0 = {1,2,3,4};
  std::vector<uint64_t> nbrs1 = {0,2,3,4};
  std::vector<uint64_t> nbrs2 = {0,1,3,4};
  std::vector<uint64_t> nbrs3 = {0,1,2};
  std::vector<uint64_t> nbrs4 = {0,1,2};
  Graph->AddNode(0, nbrs0);
  Graph->AddNode(1, nbrs1);
  Graph->AddNode(2, nbrs2);
  Graph->AddNode(3, nbrs3);
  Graph->AddNode(4, nbrs4);
  */
  PUNRCGraph Graph;
  Graph = TSnap::RCGenGridMWMT("graph", 10, 10, 1, 1, "infrc:host=192.168.1.101,port=12246", 1);

  TSnap::RCGetBfsTree(Graph, BfsTree, 0, true, false);

  std::queue<uint64_t> queue;
  std::set<uint64_t> seen_list;
  queue.push(0);
  seen_list.insert(0);
  while( !queue.empty() ) {
    uint64_t u = queue.front();
    queue.pop();

    printf("Parent: %d\tChildren: ", u);

    TUNRCGraph::TNode NodeU = BfsTree->GetNode(u);

    for(uint64_t e = 0; e<NodeU.GetDeg(); e++) {
      uint64_t nbr = NodeU.GetNbrNId(e);
      if(seen_list.count(nbr) == 0) {
        printf("%d, ", nbr);
        queue.push(nbr);
        seen_list.insert(nbr);
      }
    }

    printf("\n");
  }

  Graph->WipeOut();
  BfsTree->WipeOut();
  printf("exp13 end...\n");
}

void exp14() {
  printf("exp14 start (Run RCGenRndGnm)\n");

  PUNRCGraph Graph;
  Graph = TSnap::RCGenRndGnm("graph", "infrc:host=192.168.1.101,port=12246", 6, 6, 10000000, 1000000);

  PUNRCGraph BfsTree = new TUNRCGraph("tree", "infrc:host=192.168.1.101,port=12246", 1);
  TSnap::RCGetBfsTree(Graph, BfsTree, 0, true, false);

  std::queue<uint64_t> queue;
  std::set<uint64_t> seen_list;
  queue.push(0);
  seen_list.insert(0);
  while( !queue.empty() ) {
    uint64_t u = queue.front();
    queue.pop();

    printf("Parent: %d\tChildren: ", u);

    TUNRCGraph::TNode NodeU = BfsTree->GetNode(u);

    for(uint64_t e = 0; e<NodeU.GetDeg(); e++) {
      uint64_t nbr = NodeU.GetNbrNId(e);
      if(seen_list.count(nbr) == 0) {
        printf("%d, ", nbr);
        queue.push(nbr);
        seen_list.insert(nbr);
      }
    }

    printf("\n");
  }

  Graph->WipeOut();
  BfsTree->WipeOut();
  printf("exp14 end...\n");
}

/// PERFORMANCE SWEEP of RCGenRndGnm
void exp15() {
  printf("exp15 start (Run RCGenRndGnm)\n");

  uint64_t stat_gen_start, stat_gen_elapsed;

  for(uint64_t N = 9e6; N>=1e6; N -= 1e6) {
    PUNRCGraph G;
    printf("Generating graph...\n");
    stat_gen_start = RAMCloud::Cycles::rdtsc();
    G = TSnap::RCGenRndGnm("graph", "infrc:host=192.168.1.101,port=12246", 6, 6, N, N);
    stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
    printf("Took: %f seconds for %d nodes and edges\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed), N);
    G->WipeOut();
  }

  printf("exp15 end...\n");
}

// PERORMANCE SWEEP of GenRndGnm
void exp16() {
  printf("exp16 start (Run GenRndGnm)\n");
  PUNGraph G;
  uint64_t stat_gen_start, stat_gen_elapsed;

  for(uint64_t N = 1e7; N>=1e6; N -= 1e6) {
    printf("Generating graph...\n");
    stat_gen_start = RAMCloud::Cycles::rdtsc();
    G = TSnap::GenRndGnm<PUNGraph>(N, N, false, TInt::Rnd);
    stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
    printf("Took: %f seconds for %d nodes and edges\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed), N);
  }

  printf("exp16 end...\n");
}

/// PERFORMANCE SWEEP of RCGenGrid
void exp17() {
  printf("exp17 start (PERFORMANCE SWEEP of RCGenGrid)\n");

  uint64_t stat_gen_start, stat_gen_elapsed;

  uint64_t M = 10000;
  for(uint64_t N = 8e4; N<=9e4; N += 1e4) {
    printf("Generating graph %dx%d...\n", N, M);
    PUNRCGraph Graph;
    stat_gen_start = RAMCloud::Cycles::rdtsc();
    Graph = TSnap::RCGenGridMWMT("graph", N, M, 8192, 6, "infrc:host=192.168.1.101,port=12246", 6);
    stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
    printf("Took: %f seconds for %dx%d grid\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed), N, M);
    Graph->WipeOut();
  }

  printf("exp17 end...\n");
}

/// PERFORMANCE SWEEP of GenGrid
void exp18(){
  printf("exp18 start (PERFORMANCE SWEEP of GenGrid)\n");

  uint64_t stat_gen_start, stat_gen_elapsed;

  uint64_t M = 10000;
  for(uint64_t N = 1e4; N<=10e4; N += 1e4) {
    printf("Generating graph %dx%d...\n", N, M);
    PUNGraph Graph;
    stat_gen_start = RAMCloud::Cycles::rdtsc();
    Graph = TSnap::GenGrid<PUNGraph>(N, M, false);
    stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
    printf("Took: %f seconds for %dx%d grid\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed), N, M);
  }

  printf("exp18 end...\n");
}

/// PERFORMANCE SWEEP of RCGetClustCf
void exp19(){
  printf("exp19 start (PERFORMANCE SWEEP of RCGetClustCf)\n");

  uint64_t stat_gen_start, stat_gen_elapsed;
  uint64_t closedTriads, openTriads;

  for(uint64_t N = 10; N<=1e2; N += 10) {
    printf("Generating complete graph %d...\n", N);
    PUNRCGraph G;
    G = TSnap::RCGenFull("graph", "infrc:host=192.168.1.101,port=12246", 6, 6, N);
    printf("finished generating graph\n");
    stat_gen_start = RAMCloud::Cycles::rdtsc();
    TSnap::RCGetTriadsNode(G, 0, closedTriads, openTriads);
    stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
    printf("Took: %lu Microseconds for %d complete graph\n", RAMCloud::Cycles::toMicroseconds(stat_gen_elapsed), N);
    G->WipeOut();
  }

  printf("exp19 end...\n");
}

/// PERFORMANCE SWEEP of GetClustCf
void exp20(){
  printf("exp20 start (PERFORMANCE SWEEP of GetClustCf)\n");

  uint64_t stat_gen_start, stat_gen_elapsed;

  for(uint64_t N = 10; N<=1e2; N += 10) {
    printf("Generating complete graph %d...\n", N);
    PUNGraph G;
    G = TSnap::GenFull<PUNGraph>(N);
    printf("finished generating graph\n");
    stat_gen_start = RAMCloud::Cycles::rdtsc();
    TSnap::GetNodeClustCf(G, 0);
    stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;
    printf("Took: %lu Microseconds for %d complete graph\n", RAMCloud::Cycles::toMicroseconds(stat_gen_elapsed), N);
  }
  printf("exp20 end...\n");
}

void exp21() {
  printf("exp21 start (Run RCGetBfsTree)\n");

  uint64_t stat_gen_start, stat_gen_elapsed;

  uint64_t M = 100;
  for(uint64_t N = 1e3; N<=1e4; N += 1e3) {
    printf("Generating complete graph %dx%d...\n", N, M);
    PUNRCGraph Graph;
    Graph = TSnap::RCGenGridMWMT("graph", N, M, 8192, 6, "infrc:host=192.168.1.101,port=12246", 6);

    printf("finished gen...\n");
    PUNRCGraph BfsTree = new TUNRCGraph("tree", "infrc:host=192.168.1.101,port=12246", 1);

    printf("running bfs\n");
    stat_gen_start = RAMCloud::Cycles::rdtsc();
    TSnap::RCGetBfsTree(Graph, BfsTree, 0, true, false);
    stat_gen_elapsed = RAMCloud::Cycles::rdtsc() - stat_gen_start;

    printf("Took: %f seconds for %dx%d grid\n", RAMCloud::Cycles::toSeconds(stat_gen_elapsed), N, M);

    Graph->WipeOut();
    BfsTree->WipeOut();
  }

  printf("exp21 end...\n");
}

int main(int argc, char* argv[]) {
  printf("rcgraphtest1\n");

  //exp03(100, 1000); //GenGrid with TUNRCGraph
  //exp04(100000, 10000); //GenGrid with TUNGraph
  //exp05(1000, 10000, "infrc:host=192.168.1.101,port=12246", 1); //RCGenGrid
  //exp06(10000, 5000, 8192, "infrc:host=192.168.1.101,port=12246", 1); //RCGenGridMW
  //exp07(16000, 160000, 8192, 6, "infrc:host=192.168.1.101,port=12246", 6); //RCGenGridMWMT
  //exp08();
  //exp09();
  //exp10();
  //exp11();
  //exp12();
  //exp13();
  //exp14();
  //exp15();
  //exp16();
  //exp17();
  //exp18();
  //exp19();
  //exp20();
  exp21();

  printf("done.\n");

  return 0;
}
