// TODO ROK, Jure included basic documentation, finalize reference doc

/////////////////////////////////////////////////
// Graph Generators
namespace TSnap {

/////////////////////////////////////////////////
// Deterministic graphs
/// Generates a 2D-grid graph of Rows rows and Cols columns in ramcloud.
PUNRCGraph RCGenGrid(const char* GraphName, const uint64_t& Rows, const uint64_t& Cols, const char* coordinatorLoc="infrc:host=192.168.1.101,port=12246", const uint32_t serverSpan=1);
/// Generates a 2D-grid graph of Rows rows and Cols columns in ramcloud using multiwrites performance optimization.
PUNRCGraph RCGenGridMW(const char* GraphName, const uint64_t& Rows, const uint64_t& Cols, const uint64_t& batchSize=32, const char* coordinatorLoc="infrc:host=192.168.1.101,port=12246", const uint32_t serverSpan=1);
/// Generates a 2D-grid graph of Rows rows and Cols columns in ramcloud using multiwrites performance optimization and multithreading
PUNRCGraph RCGenGridMWMT(const char* GraphName, const uint64_t& Rows, const uint64_t& Cols, const uint64_t& batchSize=32, const uint64_t& threads=1, const char* coordinatorLoc="infrc:host=192.168.1.101,port=12246", const uint32_t serverSpan=1);

PUNRCGraph RCGenRndGnm(const char* GraphName, const char* coordinatorLoc, const uint32_t serverSpan, const uint64_t threads, const uint64_t& Nodes, const uint64_t& Edges, TRnd& Rnd=TInt::Rnd);

PUNRCGraph RCGenFull(const char* GraphName, const char* coordinatorLoc, const uint32_t serverSpan, const uint64_t threads, const uint64_t& Nodes);

}; // namespace TSnap
