#ifndef snap_main_h
#define snap_main_h

/// ##mainpage

// ramcloud
#include "RamCloud.h"
#include "TableEnumerator.h"
#include "Object.h"
#include <boost/thread.hpp>

/////////////////////////////////////////////////
// SNAP library
#include "base.h"
#include "gnuplot.h"
#include "linalg.h"

#include "gbase.h"
#include "util.h"

// graph data structures
#include "graph.h"           // graphs
#include "rcgraph.h"         // ramcloud graphs
#include "network.h"         // networks
#include "bignet.h"          // large networks
#include "timenet.h"         // time evolving networks

// algorithms
#include "subgraph.h"        // subgraph manipulations
#include "anf.h"             // approximate diameter calculation
#include "bfsdfs.h"          // breadth and depth first search
#include "rcbfsdfs.h"        // ramcloud breadth and depth first search
#include "cncom.h"           // connected components
#include "kcore.h"           // k-core decomposition
#include "alg.h"             // misc graph algorithms
#include "triad.h"           // clustering coefficient and triads
#include "rctriad.h"         // ramcloud triad and cluster coefficient algorithms
#include "gsvd.h"            // SVD and eigenvector computations
#include "gstat.h"           // graph statistics
#include "centr.h"           // centrality measures
#include "cmty.h"            // community detection algorithms

// graph generators
#include "ggen.h"            // graph generators (preferential attachment, small-world, ...)
#include "rcggen.h"          // ramcloud graph generators
#include "ff.h"              // forest fire graph generator

#include "gio.h"
#include "gviz.h"
#include "ghash.h"
#include "statplot.h"

#endif
