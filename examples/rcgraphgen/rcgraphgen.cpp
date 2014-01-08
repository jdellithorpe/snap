#include "stdafx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Graph generators. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr OutFNm = Env.GetIfArgPrefixStr("-o:", "output.txt", "Output graph filename");
  const TStr Plot = Env.GetIfArgPrefixStr("-g:", "e", "Which generator to use:"
    "\n\tf: Complete graph. Required parameters: n (number of nodes)"
    "\n\ts: Star graph. Required parameters: n (number of nodes)"
    "\n\t2: 2D Grid. Required parameters: n (number of rows), m (number of columns)"
    "\n\te: Erdos-Renyi (G_nm). Required parameters: n (number of nodes), m (number of edges)"
    "\n\tk: Random k-regular graph. Required parameters: n (number of nodes), k (degree of every node)"
    "\n\tb: Albert-Barabasi Preferential Attachment. Required parameters: n (number of nodes), k (edges created by each new node)"
    "\n\tp: Random Power-Law graph. Required parameters: n (number of nodes), p (power-law degree exponent)"
    "\n\tc: Copying model by Kleinberg et al. Required parameters: n (number of nodes), p (copying probability Beta)"
    "\n\tw: Small-world model. Required parameters: n (number of nodes), k (each node is connected to k nearest neighbors in ring topology), p (rewiring probability)\n"
    );
  const int N = Env.GetIfArgPrefixInt("-n:", 1000, "Number of nodes");
  const int M = Env.GetIfArgPrefixInt("-m:", 5000, "Number of edges");
  const double P = Env.GetIfArgPrefixFlt("-p:", 0.1, "Probability/Degree-exponent");
  const int K = Env.GetIfArgPrefixInt("-k:", 3, "Degree");

  if (Env.IsEndOfRun()) { return 0; }
  TExeTm ExeTm;
  TInt::Rnd.PutSeed(0); // initialize random seed
  printf("Generating...\n");
  PUNRCGraph G;
  TStr DescStr;
  if (Plot == "f") {
    G = TSnap::GenFull<PUNRCGraph>(N);
    DescStr = TStr::Fmt("Undirected complete graph.");
  } else
  if (Plot == "s") {
    G = TSnap::GenStar<PUNRCGraph>(N, false);
    DescStr = TStr::Fmt("Undirected star graph (1 center node connected to all other nodes).");
  } else
  if (Plot == "2") {
    G = TSnap::GenGrid<PUNRCGraph>(N, M, false);
    DescStr = TStr::Fmt("Undirected 2D grid of %d rows and %d columns.", N, M);
  } else
  if (Plot == "e") {
    G = TSnap::GenRndGnm<PUNRCGraph>(N, M, false);
    DescStr = TStr::Fmt("Undirected Erdos-Renyi random graph.");
  } else
  if (Plot == "c") {
    G = TSnap::ConvertGraph<PUNRCGraph>(TSnap::GenCopyModel(N, P));
    DescStr = TStr::Fmt("Copying model by Kleinberg et al. Node u comes, selects a random v, and with prob P it links to v, with 1-P links u links to neighbor of v. Power-law degree slope is 1/(1-P).");
  }
  printf("done.\n");

  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
