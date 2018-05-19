#ifndef __INET_CentralScheduler_H
#define __INET_CentralScheduler_H
#include <chrono>  // for high_resolution_clock

#include <map>
#include <set>
#include <algorithm>    // std::random_shuffle
#include <vector>       // std::vector
#include <random>
#include <cmath>
#include <time.h>       /* time */

#include "inet/common/INETDefs.h"

#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/applications/tcpapp/TCPSessionApp.h"
#include "../application/raptorqapp/RaptorQBasicClientApp.h"
#include "../application/raptorqapp/RQSinkApp.h"
//#include "../rqtransportlayer/RaptorQ/RaptorQ.h"


namespace inet {
class INET_API CentralScheduler : public cSimpleModule, public ILifecycle
{
//private:
protected:
    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;
    simtime_t  totalSimTime;
    cOutVector permMapLongFlowsVec;
    cOutVector permMapShortFlowsVec;
    
    cOutVector randMapShortFlowsVec;
    cOutVector permMapShortFlowsVector;
    
    cOutVector numTcpSessionAppsVec;
    cOutVector numTcpSinkAppsVec;
    cOutVector nodes;
    cOutVector matSrc; // record all the source servers of the created short flows
    cOutVector matDest;// record all the dest servers of the created short flows
    

    cMessage *startManagerNode;
    int kValue;
    const char *trafficMatrixType;// either "permTM"  or "randTM"
    unsigned int test=0;
    unsigned int arrivalRate; // lamda of an exponential distribution (Morteza uses 256 and 2560)
    unsigned int flowSize;
    unsigned int numServers;
    unsigned int numShortFlows;
    unsigned int  longFlowSize;
    double percentLongFlowNodes;
//    int matrix[4][numServers];
    unsigned int numCompletedShortFlows = 0;
    cMessage *stopSimulation;
    std::vector<unsigned int> permServers;

    std::vector<unsigned int> permLongFlowsServers;
    std::vector<unsigned int> permShortFlowsServers;

    unsigned int numlongflowsRunningServers; // 33% of nodes run long flows
    unsigned int numshortflowRunningServers;

 ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

    // multicast variables
    struct coreAggMap
    {

        unsigned int aggIndex;
        std::vector<int> associatedCores; // associated

    };

    typedef std::list<coreAggMap> CoreAggMapList;
    CoreAggMapList coreAggMapList;


    unsigned int numReplica;
    unsigned int  numRunningMulticastGroups;
    bool runMulticast;
    std::vector<int> tempNode;
    std::vector<int> tempCombination;
    std::list< std::vector<int> > combinations; // all possible combinations
    unsigned int numAllCombinations ;
    std::set<int> alreadySelectedGroups; // contains index of the selected groups (set.insert does not add in order like in vector.pushback)


    struct multicastGroup{
        unsigned int groupIndex;
        unsigned int multicastSender;
        std::set<int> multicastReceivers;
    };

    typedef std::list< multicastGroup> SelectedCombinations;
    SelectedCombinations selectedCombinations;


    struct multicastGroupPortNumbers{
        unsigned int groupIndex;
        std::vector<int> multicastReceiversPortNumbers;
    };

    typedef std::list< multicastGroupPortNumbers> MulticastReceiversPortNumbers;
    MulticastReceiversPortNumbers multicastReceiversPortNumbersList;


//    struct multicastRouter{
//        unsigned int pod;
//        unsigned int rack;
//        unsigned int node;
//        unsigned int multicastGroupIndex;
//
//    };




//    struct multicastDistPodsRacks{
//        std::vector<int>  pod;
//        std::vector<int>  rack;
//        std::vector<int>  node;
//        unsigned int multicastGroupIndex;
//        std::string routeName;
//    };
//
//    typedef std::list< multicastDistPodsRacks> MulticastDistPodsRacks;
//    MulticastDistPodsRacks multicastDistPodsRacksList;

    struct MulticastInfo{
         unsigned int multicastGroupIndex;
        std::string routeName;
        std::vector<std::string> destinations;
    };
    typedef std::list< MulticastInfo> MulticastInfoList;
    MulticastInfoList multicastInfoList;



    struct multicastGrTxRx{
        unsigned int groupIndex;
        std::string multicastSender;
        std::vector<std::string> multicastReceivers;
    };

    typedef std::list< multicastGrTxRx> MulticastGrTxRxList;
    MulticastGrTxRxList multicastGrTxRxList;



    ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
    // MultiSourcing variables

   bool  runMultiSourcing;
   unsigned int numRunningMultiSourcingGroups;
   unsigned int numSourceNodes;


   struct multiSourcingGroup{
       unsigned int groupIndex;
       unsigned int multiSourcingReceiver;
       std::set<int> multiSourcingSenders;
   };
   typedef std::list< multiSourcingGroup> MultiSourcingSelectedCombinations;
   MultiSourcingSelectedCombinations multiSourcingSelectedCombinations;


   struct multiSourcingGrTxRx{
       unsigned int groupIndex;
       std::string multiSourcingReceiver;
       std::vector<std::string> multiSourcingSenders;
   };

   typedef std::list< multiSourcingGrTxRx> MultiSourcingGrTxRxList;
   MultiSourcingGrTxRxList multiSourcingGrTxRxList;


////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////

      // multicast variables



    virtual bool handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) override
       { Enter_Method_Silent(); throw cRuntimeError("Unsupported lifecycle operation '%s'", operation->getClassName()); return true; }






    //  <dest, src>
    std::map<unsigned int, unsigned int> permMapLongFlows;
    std::map<unsigned int, unsigned int> permMapShortFlows;

    double sumArrivalTimes = 0;
    double newArrivalTime;
    bool shuffle = false;
    bool randomGroup = true;

    struct NodeLocation
    {
        unsigned int pod;
        unsigned int rack;
        unsigned int node;
        unsigned int index;

        unsigned int numTCPSink;
        unsigned int numTCPSession;
    };

    typedef std::list<NodeLocation> NodeLocationList;
    NodeLocationList nodeLocationList;

// std::random_device rd; // uniformly-distributed integer random number generator
// std::mt19937 rng(rd()); // mt19937: Pseudo-random number generation

    
    
    struct RecordMat
    {
        unsigned int recordSrc;
        unsigned int recordDest;
    };
    typedef std::list<RecordMat> RecordMatList;
    RecordMatList recordMatList;
    

    
public:
    CentralScheduler()
    {
    }
    virtual ~CentralScheduler();
    void getNodeRackPod(unsigned int nodeIndex,unsigned int &nodeId ,unsigned int &rackId , unsigned int &podId);
    void getMulticastGroupGivenIndex(unsigned int groupIndex, std::set<int>& multicastReceivers);
    void getPodRackNodeForEachReceiverInMulticastGroup(std::set<int> multicastReceivers , std::set<std::string>& nodePodRackLoc ) ;
    void getMulticastGroupReceiversPortNumbers(unsigned int groupIndex,std::vector<int>& portNumbersVector);

    void lookUpAtMulticastInfoList(unsigned int groupId , std::string currentNode,  std::vector<std::string>& destinations ) ;
    bool  isMulticastReceiver(unsigned int multicastGrIndex , std::string rx) ;
    bool  isMulticastSender(unsigned int multicastGrIndex, std::string tx);
    void  getMulticastGrSender(unsigned int multicastGrIndex, std::string& senderName);
    void  getMulticastGrFirstEdgeDest(unsigned int multicastGrIndex, std::string  senderNode , std::string& firstEdge);

    void getMultiSourcingGrReceiver(unsigned int multiSrcGrIndex, std::string& receiverName);


protected:
    virtual void initialize(int stage) override;
    //virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual void handleParameterChange(const char *parname) override;

    void serversLocations();
    void generateTM();

    void getNewDestRandTM(std::string& itsSrc, std::string& newDest);
    void getNewDestPremTM(std::string& itsSrc, std::string& newDest);

    void findLocation(unsigned int nodeIndex, std::string& nodePodRackLoc);
    void scheduleLongFlows();
    void deleteAllSubModuleApp(const char *subModuleToBeRemoved);
    int findNumSumbodules(cModule* nodeModule, const char *subModuleType);
    void scheduleNewShortFlow(std::string itsSrc, std::string newDest);

    void permTM(const char *longOrShortFlows);

    // multicast
    void generateMulticastGroups();
    void nChooseK(int offset, int n, int k , bool r);
    void getNewMulticastCombination();
    void scheduleMulticastGroupConn(unsigned int groupIndex , unsigned int senderId , std::set<int> receiversGroup , std::vector<int>& receiversPortNumbers);
    void   multicastRoutersLocation(unsigned int groupIndex, unsigned int senderId , std::set<int> receiversGroup ) ;
    void  getUniqueVector( std::vector<std::string>& uniqueVector)  ;
    void  removeSameConsecutiveElements( std::vector<std::string>& inVector) ;
    void multicastGroupInfo(unsigned int multicastGrIndex , unsigned int senderId , std::set<int> receiversId );

    // multiSourcing
    void generateMultiSourcingGroups();
    void getNewMultiSourcingCombination();
    void multiSourcingGroupInfo(unsigned int multicastGrIndex , unsigned int receiverId , std::set<int> sendersId );
    void  scheduleMultiSourcingGroupConn(unsigned int groupIndex, unsigned int receiverId , std::set<int> sendersGroup , std::vector<int>& sendersPortNumbers);


};

}

#endif // ifndef __INET_RQ_H

