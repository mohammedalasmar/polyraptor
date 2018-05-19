#include "../FatTreeRQ/CentralScheduler.h"
//#include "/Volumes/LocalDataHD/m/ma/ma777/Desktop/omnetpp-5.2/src/envir/stopwatch.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include <fstream>

using namespace std;
using namespace std::chrono;

namespace inet {

Define_Module(CentralScheduler);

CentralScheduler::~CentralScheduler()
{
    cancelAndDelete(startManagerNode);
    cancelAndDelete(stopSimulation);

}

void CentralScheduler::initialize(int stage)
{

    // Record start time
    t1 = high_resolution_clock::now();

//    omnetpp::envir::Stopwatch stopwatch;
//    stopwatch.setRealTimeLimit(5);

    numCompletedShortFlows = par("numCompletedShortFlows");
    WATCH(numCompletedShortFlows);

    randMapShortFlowsVec.setName("randMapShortFlowsVec");
    permMapShortFlowsVector.setName("permMapShortFlowsVector");
    
    permMapLongFlowsVec.setName("permMapLongFlowsVec");
    permMapShortFlowsVec.setName("permMapShortFlowsVec");
    numTcpSessionAppsVec.setName("numTcpSessionAppsVec");
    numTcpSinkAppsVec.setName("numTcpSinkAppsVec");
    matSrc.setName("matSrc");
    matDest.setName("matDest");
    nodes.setName("nodes");
    
    std::cout << "\n\n Central flow scheduler \n";
    kValue = par("kValue");
    trafficMatrixType = par("trafficMatrixType");
    arrivalRate = par("arrivalRate");
    flowSize = par("flowSize");
    numShortFlows = par("numShortFlows");
    longFlowSize = par("longFlowSize");
    numServers = std::pow(kValue, 3) / 4;
    shuffle = par("shuffle");
    randomGroup = par("randomGroup");


    percentLongFlowNodes = par("percentLongFlowNodes");

    // multicast variables
    numReplica = par("numReplica");
    numRunningMulticastGroups = par("numRunningMulticastGroups");
    runMulticast = par("runMulticast");


    // multiSourcing
    runMultiSourcing = par("runMultiSourcing");
    numRunningMultiSourcingGroups = par("numRunningMultiSourcingGroups");
    numSourceNodes = par("numSourceNodes");






    std::cout << " =====  SIMULATION CONFIGURATIONS ========= " << "\n";
    std::cout << " =====  numServers   : " << numServers << "         ========= \n";
    std::cout << " =====  ShortflowSize: " << flowSize << "      ========= \n";
    std::cout << " =====  numShortFlows: " << numShortFlows << "          ========= \n";
    std::cout << " =====  arrivalRate  : " << arrivalRate << "       ========= \n";
    std::cout << " ========================================== " << "\n";
    stopSimulation=  new cMessage("stopSimulation");

    startManagerNode = new cMessage("startManagerNode");
    scheduleAt(0.0, startManagerNode);
}

void CentralScheduler::handleMessage(cMessage *msg)
{


    if (msg == stopSimulation) {
        std::cout << " All shortFlows COMPLETED  " << std::endl;
        totalSimTime=simTime();
//       cancelAndDelete   stopSimulation = nullptr;
//       cancelAndDelete(stopSimulation);
        endSimulation();
    }

    std::random_device rd; // uniformly-distributed integer random number generator
    std::mt19937 rng(rd()); // mt19937: Pseudo-random number generation
    std::cout << "\n\n\n ";
    std::cout << "******************** CentralScheduler::handleMessage .. ********************  \n";

    //  % of short flows and % of long flows
//    numlongflowsRunningServers = floor(numServers * 0.33); // 33% of nodes run long flows
    numlongflowsRunningServers = floor(numServers * percentLongFlowNodes); // 33% of nodes run long flows , TODO throw error as it shouldn't be 1
    numshortflowRunningServers = numServers - numlongflowsRunningServers;
    std::cout << "numshortflowRunningServers:  " << numshortflowRunningServers << std::endl;
    std::cout << "numlongflowsRunningServers:  " << numlongflowsRunningServers << std::endl;

//     cModuleType *moduleTypeSrc = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RQSinkApp");
//    std::cout << "aaaaaaaaa:  " << moduleTypeSrc->getFullPath() << std::endl;
//    std::cout << "aaaaaaaaa:  " <<this->getFullPath() << std::endl;
    generateTM();

    serversLocations();
    srand(time(NULL));
    std::string itsSrc;
    std::string newDest;

    deleteAllSubModuleApp("raptorQApp[0]");



    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    std::cout << "\n\n ******************** schedule Long flows .. ********************  \n";
    scheduleLongFlows();




    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    std::cout << "\n\n ******************** schedule Short flows .. ********************  \n";
    if (numShortFlows!= 0){
    for (unsigned int i = 1; i <= numShortFlows; i++) {
        std::cout << "\n\nShortflow ID: " << i << std::endl;

        if (strcmp(trafficMatrixType, "randTM") == 0) getNewDestRandTM(itsSrc, newDest);
        if (strcmp(trafficMatrixType, "permTM") == 0) getNewDestPremTM(itsSrc, newDest);
        // identifying the servers locations: FatTree.Pod[].racks[].servers[]
        scheduleNewShortFlow(itsSrc, newDest);
    }
    }
    std::cout << "\n\n\n";



    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    if (runMulticast == true){
        std::cout << "\n\n ******************** schedule Multicast flows .. ********************  \n";
        std::cout << "******************** generate all Multicast possible Groups .. ********************  \n";
        generateMulticastGroups() ;   // traffic load Type I. Generate combinations list


        std::cout << "\n\n ******************** numRunningMulticastGroups= " << numRunningMulticastGroups << "  ********************  \n";
        std::cout << " ******************** & assign sender for each selected group .. ********************  \n";

        if (numRunningMulticastGroups!= 0){
            for (unsigned int i = 1; i <= numRunningMulticastGroups; i++) {
                std::cout << "numRunningMulticastGroups: " << numRunningMulticastGroups << " , and  this is the " << i << " running group" <<  std::endl;
                getNewMulticastCombination(); // get one group from the combinations list
            }
        }

        std::cout << "\n\n **************   MulticastGroupConn  ..    *************  \n";
        int j = 0;int counterI=0;
        auto itCom = selectedCombinations.begin();
        while (itCom != selectedCombinations.end()) {
            std::cout << "[" << ++j << "] group index: " << itCom->groupIndex  << " ,,  "  ;
            std::cout << "multicastSender: " << itCom->multicastSender  << " ==> " << " Receivers GROUP =  [ ";

            for (auto iterSet = itCom->multicastReceivers.begin() ; iterSet!= itCom->multicastReceivers.end() ; ++iterSet){
                std::cout << *iterSet << "  "  ;
            }
            std::cout <<  " ] \n";

            std::cout << "\n\n ******************* updating MulticastGroup info (string)..'multicastGrTxRxList' ********************  \n";
            multicastGroupInfo(itCom->groupIndex, itCom->multicastSender ,  itCom->multicastReceivers );


            std::cout << "\n\n ******************* schedule MulticastGroupConn  .. ********************  \n";
            std::vector<int> receiversPortNumbers;


            scheduleMulticastGroupConn(itCom->groupIndex, itCom->multicastSender ,  itCom->multicastReceivers , receiversPortNumbers);
            std::cout << "\n\n ******************* explore the routing map in FatTree for this multicast group  .. ********************  \n";
            multicastRoutersLocation(itCom->groupIndex, itCom->multicastSender ,  itCom->multicastReceivers  );



             std::cout <<" counterI = " << ++counterI <<  "  \n\n\n" ;
            ++itCom;
        }

        std::cout << "\n\n test  \n" ;
        unsigned int a;
        unsigned int b;
        unsigned int c;
        getNodeRackPod(15, a,b,c);
        std::cout << a  << "  " << b << "  " << c << "\n\n\n";
    } // runMulticast == true


   // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
   // $$$$$$$$$$$$$$$$$$$$$$$  multiSourcing $$$$$$$$$$$$$$$$$$$$$$$$$  multiSourcing  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
   // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    if (runMultiSourcing == true) {
        std::cout << "\n\n ******************** schedule multiSourcing flows .. ********************  \n";
        std::cout << "******************** generate all multiSourcing possible Groups .. ********************  \n";
        generateMultiSourcingGroups(); // traffic load Type I. Generate combinations list (similar to generateMulticastGroups but we used numReplica)

        std::cout << "\n\n ******************** numRunningMultiSourcingGroups= " << numRunningMultiSourcingGroups
                << "  ********************  \n";
        std::cout << " ******************** & assign sender for each selected group .. ********************  \n";

        if (numRunningMultiSourcingGroups != 0) {
            for (unsigned int i = 1; i <= numRunningMultiSourcingGroups; i++) {
                std::cout << "numRunningMultiSourcingGroups: " << numRunningMultiSourcingGroups << " , and  this is the " << i << " running group"   << std::endl;
                getNewMultiSourcingCombination(); // get one group from the combinations list
            }
        }

        std::cout  << "\n\n **************  multiSourcing Group Conn  ..    *************  \n";
        int j = 0;
        auto itCom = multiSourcingSelectedCombinations.begin();
        while (itCom != multiSourcingSelectedCombinations.end()) {
            std::cout << "[" << ++j << "] group index: " << itCom->groupIndex << " ,,  ";
            std::cout << "multicastReceiver: " << itCom->multiSourcingReceiver << " ==> " << " Senders GROUP =  [ ";
            for (auto iterSet = itCom->multiSourcingSenders.begin();  iterSet != itCom->multiSourcingSenders.end(); ++iterSet) {
                std::cout << *iterSet << "  ";
            }
             std::cout << " ] \n";
             std::cout << "\n\n ******************* updating multiSourcingGroup info (string)..'multiSourcingGrTxRxList' ********************  \n";
             multiSourcingGroupInfo(itCom->groupIndex, itCom->multiSourcingReceiver,  itCom->multiSourcingSenders);
             std::cout << "\n\n ******************* schedule  MultiSourcingGroupConn  .. ********************  \n";
              std::vector<int> sendersPortNumbers;
              scheduleMultiSourcingGroupConn(itCom->groupIndex,  itCom->multiSourcingReceiver, itCom->multiSourcingSenders, sendersPortNumbers);

             //            std::cout << "\n\n ******************* explore the routing map in FatTree for this multicast group  .. ********************  \n";
             //            multicastRoutersLocation(itCom->groupIndex, itCom->multicastSender,itCom->multicastReceivers);
            std::cout << "\n\n\n";
            ++itCom;
        }




    } // runMultiSourcing == true






}



///////  multi sourcing  111111
void CentralScheduler::generateMultiSourcingGroups() {
    int n = numServers, k = numSourceNodes;
    nChooseK(0, n, k, false);
    numAllCombinations = combinations.size();
    cout << "numAllCombinations =  " << numAllCombinations << " \n\n";
    // combinations LIST:
    // vector 1 (index, receiver 1, receiver 2,..), vector 2 (index, receiver 1, receiver 2,..) , ..
    list<vector<int> >::iterator iterList;
    vector<int>::iterator iterVector;
    int combinationIndex = 0;
    cout << "id:" << " Rx \n";

    for (auto iterList = combinations.begin(); iterList != combinations.end();  ++iterList) {
        // add an index (at the beginning-NB no pushfront for vector so use insert)  for each combination vector in the list
        iterList->insert(iterList->begin(), combinationIndex);
        ++combinationIndex;
        for (iterVector = iterList->begin(); iterVector != iterList->end(); ++iterVector) {
            cout << *iterVector << "  ";
        }
        cout << " \n";
    }
    cout << "================ \n\n\n";
}

///////  multi sourcing  2222222
void CentralScheduler::getNewMultiSourcingCombination() {
    unsigned int selectedGroupIndex;
    if(randomGroup == true)   selectedGroupIndex =  std::rand() %  numAllCombinations;
    if(randomGroup == false)  selectedGroupIndex =   0;

    unsigned int selectedReceiver;
    if(randomGroup == true)   selectedReceiver = std::rand() %  numServers  ;
    if(randomGroup == false)  selectedReceiver =   0  ;


    // we need to make sure that each group will be selected once (Traffic Load Type I).
    bool isGroupAlreadyTaken = true ;
    while (isGroupAlreadyTaken == true) {
        auto it = alreadySelectedGroups.find(selectedGroupIndex);
        if (it != alreadySelectedGroups.end()) { // the selected group has already taken, its in alreadySelectedGroups==> select  another sender
            cout<< "This group has been selected, so let's get another group..... " << selectedGroupIndex << " \n";
            selectedGroupIndex = std::rand() % numAllCombinations;
        } else {    // the selected group is new ==> DONE
            isGroupAlreadyTaken = false;
            alreadySelectedGroups.insert(selectedGroupIndex);
        }
    }


    cout << "selectedGroupIndex "  << selectedGroupIndex <<"  \n";
    cout << "initial selectedReceiver: "  << selectedReceiver <<"  \n";
    std::set<int> selectedGroup ;
    auto iterList = combinations.begin();
    std::advance(iterList, selectedGroupIndex);
    int i=0;
    cout << "multiSourcing senders group:  " ;
    for (auto iterVector = iterList->begin(); iterVector != iterList->end(); ++iterVector) {
        if (i >0 ) { // just to ignore the index part
            cout << *iterVector << " ";
            selectedGroup.insert(*iterVector);
        }
        ++i;
    }

    cout << " \n";
    // finding a sender which is not in the selected group, as we don't want a node to send replica to itself in this traffic load
    bool isReceiverInTheSelectedGroup = true ;
    while (isReceiverInTheSelectedGroup == true) {
        auto it = selectedGroup.find(selectedReceiver);
        if (it != selectedGroup.end()) { // the selected receiver is in the selected group ==> select  another sender
            cout << "this sender node is in the selected group(it is a receiver), so let's get another sender for this group.."  << *it << " \n";
            selectedReceiver = std::rand() % numServers;
        } else {    // the selected sender is not in the selected group ==> DONE
            isReceiverInTheSelectedGroup = false;
        }
    }

    cout << " \n";
    cout << "selectedReceiver:  "  << selectedReceiver <<" \n";

    cout << "multiSourcing senders group:  " ;
    for ( auto iter = selectedGroup.begin() ; iter!=selectedGroup.end(); ++iter){
        cout << *iter << " ";
    }
    cout << "\n";

    multiSourcingGroup multiG;
    multiG.groupIndex =selectedGroupIndex ;
    multiG.multiSourcingReceiver = selectedReceiver;
    multiG.multiSourcingSenders = selectedGroup;
    multiSourcingSelectedCombinations.push_back(multiG);
    cout << "--------------- \n\n\n";

}


///////  multi sourcing  333333
void CentralScheduler::multiSourcingGroupInfo(unsigned int multicastGrIndex , unsigned int receiverId , std::set<int> sendersId ) {
    multiSourcingGrTxRx    mTxRx;
    mTxRx.groupIndex = multicastGrIndex;
    std::string  receiverString;
    findLocation(receiverId , receiverString ) ;
    mTxRx.multiSourcingReceiver  = receiverString;
    std::string rx;
    std::vector<std::string> thisGroupSenders;
    for (auto iter  = sendersId.begin(); iter  != sendersId.end();  ++iter ) {
        findLocation(*iter , rx ) ;
        thisGroupSenders.push_back(rx);
    }
    mTxRx.multiSourcingSenders =thisGroupSenders;
    multiSourcingGrTxRxList.push_back(mTxRx);


    // print out
    for (auto iter = multiSourcingGrTxRxList.begin();  iter != multiSourcingGrTxRxList.end(); ++iter) {
        std::cout << " Gr ID= " << iter->groupIndex << " \n";
        std::cout << " Gr Receiver= " << iter->multiSourcingReceiver << " \n";
        std::vector<std::string> senders = iter->multiSourcingSenders;
        for (auto it = senders.begin(); it != senders.end(); ++it) {
            std::cout << " Gr Senders= " <<  *it << " \n";
        }
    }
}

///////  multi sourcing  4444444
void CentralScheduler::getMultiSourcingGrReceiver(unsigned int multiSrcGrIndex, std::string& receiverName) {

    for (auto iter = multiSourcingGrTxRxList.begin();  iter != multiSourcingGrTxRxList.end(); ++iter) {
        if (iter->groupIndex == multiSrcGrIndex  ) {
            receiverName = iter->multiSourcingReceiver;
        }
    }

}

///////  multi sourcing  55555555
void CentralScheduler::scheduleMultiSourcingGroupConn(unsigned int groupIndex, unsigned int receiverId , std::set<int> sendersGroup , std::vector<int>& sendersPortNumbers)
{
    std::cout << "!!!!! scheduleMultiSourcingGroupConn .. !!!!!!  \n";
    std::string receiverName;
    std::string senderName;


    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    // ^^^^^^^^^^^^^^^^^^     multiSourcing  Receiver  configurations  ^^^^^^^^^
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    CentralScheduler::findLocation(receiverId, receiverName);

//    unsigned int receiverNodeId;
//    unsigned int receiverRackId;
//    unsigned int receiverPodId;
//    getNodeRackPod(receiverId, receiverNodeId, receiverRackId, receiverPodId);
//    std::string  initEdgeDest = "FatTree.Pod[" + std::to_string(senderPodId) + "].racks[" + std::to_string(senderRackId) + "].edgeSwitch";

    // get the src tcp module
    cModule* destModule = getModuleByPath(receiverName.c_str());
    cModule* tcpDestModule = destModule->getSubmodule("raptorQ");
    int newTCPGateOutSizeDest = tcpDestModule->gateSize("appOut") + 1;
    int newTCPGateInSizeDest = tcpDestModule->gateSize("appIn") + 1;
    tcpDestModule->setGateSize("appOut", newTCPGateOutSizeDest);
    tcpDestModule->setGateSize("appIn", newTCPGateInSizeDest);
    int newNumTcpSinkAppsDest = findNumSumbodules(destModule, "inet.raptorqimplementation.application.raptorqapp.RQSinkApp") + 1;
    cModuleType *moduleTypeDest = cModuleType::get( "inet.raptorqimplementation.application.raptorqapp.RQSinkApp");
    std::string nameTcpAppDest = "raptorQApp["  + std::to_string(newNumTcpSinkAppsDest - 1) + "]";

    std::cout << "Rx  NumRQSinkApp   =  " << newNumTcpSinkAppsDest << "\n";

    // DESTttttttt  RQSinkApp
    // create (possibly compound) module and build its submodules (if any)
    cModule *newDestAppModule = moduleTypeDest->create(nameTcpAppDest.c_str(), destModule);
    newDestAppModule->par("localPort").setDoubleValue( 80 + newNumTcpSinkAppsDest); //    to check if needed
//    newDestAppModule->par("localPort").setDoubleValue( 80 + groupIndex);
    newDestAppModule->par("multiSrcGoupId").setDoubleValue(groupIndex); //    to check if needed
    newDestAppModule->par("isMultiSourcingSink").setBoolValue(true); //    to check if needed





    //        newDestAppModule->par("localAddress").setStringValue(receiverName.c_str());

//    cout << " Rx port number= " << 80 + groupIndex << "\n";

    newDestAppModule->par("opcode").setDoubleValue(1);
//    newDestAppModule->par("isLongFlow").setBoolValue(true); // TODO
    //   --------<tcpIn         appOut[]<----------
    //     tcpApp                          tcp
    //   -------->tcpOut        appIn[] >----------
    cGate *gateTcpInDest = tcpDestModule->gate("appIn", newTCPGateOutSizeDest - 1);
    cGate *gateTcpOutDest = tcpDestModule->gate("appOut", newTCPGateOutSizeDest - 1);
    cGate *gateInDest = newDestAppModule->gate("raptorqIn");
    cGate *gateOutDest = newDestAppModule->gate("raptorqOut");
    gateTcpOutDest->connectTo(gateInDest);
    gateOutDest->connectTo(gateTcpInDest);
    newDestAppModule->finalizeParameters();
    newDestAppModule->buildInside();
    newDestAppModule->scheduleStart(simTime());
    newDestAppModule->callInitialize();

    // have been added few lines above ?? to remove
    //        newDestAppModule->par("localPort").setDoubleValue(80 + newNumTcpSessionAppsSrc);
    //        newDestAppModule->par("opcode").setDoubleValue(1); // receiver
    //

    //            multicastGroupPortNumbers mlticastGrPorts;
    //            mlticastGrPorts.multicastReceiversPortNumbers = receiversPortNumbers ;
    //            mlticastGrPorts.groupIndex = groupIndex;
    //            multicastReceiversPortNumbersList.push_back(mlticastGrPorts);






       // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
       // ^^^^^^^^^^^^^^^^^^     multiSourcing  Senders configurations  ^^^^^^^^^
       //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

       for (auto iter = sendersGroup.begin(); iter != sendersGroup.end(); ++iter) {
           cout << "NEW SENDER :)   ";
           cout << " Tx= " << *iter  << "\n";
           CentralScheduler::findLocation( *iter, senderName);

           cModule* srcModule = getModuleByPath(senderName.c_str());
           cModule* tcpSrcModule = srcModule->getSubmodule("raptorQ");
           int newTCPGateOutSizeSrc = tcpSrcModule->gateSize("appOut") + 1;
           int newTCPGateInSizeSrc = tcpSrcModule->gateSize("appIn") + 1;
           tcpSrcModule->setGateSize("appOut", newTCPGateOutSizeSrc);
           tcpSrcModule->setGateSize("appIn", newTCPGateInSizeSrc);
           int newNumTcpSessionAppsSrc = findNumSumbodules(srcModule, "inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp") + 1;
           cModuleType *moduleTypeSrc = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp");
           std::string nameTcpAppSrc = "raptorQApp[" + std::to_string(newNumTcpSessionAppsSrc - 1) + "]";

           std::cout << "Src  numRaptorQBasicClientApp =  " << newNumTcpSessionAppsSrc << "\n";


           // multiSourcing SENDER  RaptorQBasicClientApp
           cModule *newSrcAppModule = moduleTypeSrc->create(nameTcpAppSrc.c_str(), srcModule);


           newSrcAppModule->par("localAddress").setStringValue(senderName.c_str()); // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
           newSrcAppModule->par("connectAddress").setStringValue(receiverName.c_str()); // initEdgeDest initEdgeDest initEdgeDest initEdgeDest


           newSrcAppModule->par("connectPort").setDoubleValue(80 + newNumTcpSinkAppsDest);// this is not correct , use groupIndex
//           newSrcAppModule->par("connectPort").setDoubleValue(80 + groupIndex);



           sendersPortNumbers.push_back(80 + newNumTcpSessionAppsSrc); // ?? we might not need it

           std::random_device rd; // uniformly-distributed integer random number generator-- seed
            std::mt19937 rng(rd()); // mt19937: Pseudo-random number generation
            std::exponential_distribution<double> exp(arrivalRate);
            newArrivalTime = exp.operator()(rng);
            sumArrivalTimes = sumArrivalTimes + newArrivalTime;



           newSrcAppModule->par("startTime").setDoubleValue(sumArrivalTimes+simTime().dbl());
           newSrcAppModule->par("numSymbolsToSend").setDoubleValue(flowSize);
           newSrcAppModule->par("requestLength").setDoubleValue(10);
           newSrcAppModule->par("replyLength").setDoubleValue(1000);
           newSrcAppModule->par("opcode").setDoubleValue(2); // opcode=2 ==> Writer (sender)
           newSrcAppModule->par("fileId").setDoubleValue(2017);

           // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN multiSourcing Parameters
           newSrcAppModule->par("multicastGroupIndex").setDoubleValue(groupIndex);
           newSrcAppModule->par("isMultiSourcingConn").setBoolValue(true) ; // ToDOOOOOOOOOOOO


           //   --------<tcpIn         appOut[]<----------
           //     tcpApp                          tcp
           //   -------->tcpOut        appIn[] >----------
           cGate *gateTcpIn = tcpSrcModule->gate("appIn", newTCPGateInSizeSrc - 1);
           cGate *gateTcpOut = tcpSrcModule->gate("appOut", newTCPGateOutSizeSrc - 1);

           cGate *gateIn = newSrcAppModule->gate("raptorqIn");
           cGate *gateOut = newSrcAppModule->gate("raptorqOut");

           gateTcpOut->connectTo(gateIn);
           gateOut->connectTo(gateTcpIn);

           newSrcAppModule->finalizeParameters();
           newSrcAppModule->buildInside();
           newSrcAppModule->scheduleStart(simTime());
           newSrcAppModule->callInitialize();

           newSrcAppModule->par("startTime").setDoubleValue(simTime().dbl());

       }
}

















// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
// multicasting
bool CentralScheduler::isMulticastReceiver(unsigned int multicastGrIndex , std::string rx) {

    for (auto iter = multicastGrTxRxList.begin();  iter != multicastGrTxRxList.end(); ++iter) {
        if ( iter->groupIndex == multicastGrIndex){
        std::vector<std::string> dests = iter->multicastReceivers;
        for (auto it = dests.begin(); it != dests.end(); ++it) {
            std::string recv = *it;
            if ( strcmp( recv.c_str() , rx.c_str())  == 0){
                return true;
            }
         }
        }
    }
return false;
}

bool CentralScheduler::isMulticastSender(unsigned int multicastGrIndex, std::string tx) {

    for (auto iter = multicastGrTxRxList.begin();  iter != multicastGrTxRxList.end(); ++iter) {
        std::string senderName = iter->multicastSender;
        if (iter->groupIndex == multicastGrIndex    && strcmp(senderName.c_str(), tx.c_str()) == 0) {
            return true;
        }
    }
return false;
}

void CentralScheduler::getMulticastGrSender(unsigned int multicastGrIndex, std::string& senderName) {

    for (auto iter = multicastGrTxRxList.begin();  iter != multicastGrTxRxList.end(); ++iter) {
        if (iter->groupIndex == multicastGrIndex  ) {
               senderName = iter->multicastSender;
        }
    }

}

void CentralScheduler::getMulticastGrFirstEdgeDest(unsigned int multicastGrIndex, std::string  senderNode , std::string& firstEdge) {

    for (auto iter = multicastInfoList.begin();  iter != multicastInfoList.end(); ++iter) {
        std::string routeName = iter->routeName;
        if (iter->multicastGroupIndex == multicastGrIndex    && strcmp(senderNode.c_str(), routeName.c_str()) == 0) {
            firstEdge= iter->destinations.at(0);
        }
    }

}


void CentralScheduler::multicastGroupInfo(unsigned int multicastGrIndex , unsigned int senderId , std::set<int> receiversId ) {
    multicastGrTxRx    mTxRx;
    mTxRx.groupIndex = multicastGrIndex;
    std::string  senderString;
    findLocation(senderId , senderString ) ;
    mTxRx.multicastSender = senderString;
    std::string rx;
    std::vector<std::string> thisGroupReceivers;
    for (auto iter  = receiversId.begin(); iter  != receiversId.end();  ++iter ) {
        findLocation(*iter , rx ) ;
        thisGroupReceivers.push_back(rx);
    }
    mTxRx.multicastReceivers =thisGroupReceivers;
    multicastGrTxRxList.push_back(mTxRx);


    // print out
    for (auto iter = multicastGrTxRxList.begin();  iter != multicastGrTxRxList.end(); ++iter) {
//        std::cout << " Gr ID= " << iter->groupIndex << " \n";
//        std::cout << " Gr Sender= " << iter->multicastSender << " \n";
        std::vector<std::string> dests = iter->multicastReceivers;
        for (auto it = dests.begin(); it != dests.end(); ++it) {
//            std::cout << " Gr Receivers= " <<  *it << " \n";
        }
    }
}


void CentralScheduler::generateMulticastGroups() {
    int n = numServers, k = numReplica;
    nChooseK(0, n, k, false);
    numAllCombinations = combinations.size();
//    cout << "numAllCombinations =  " << numAllCombinations << " \n\n";
    // combinations LIST:
    // vector 1 (index, receiver 1, receiver 2,..), vector 2 (index, receiver 1, receiver 2,..) , ..
    list<vector<int> >::iterator iterList;
    vector<int>::iterator iterVector;
    int combinationIndex = 0;
//    cout << "id:" << " Rx \n";

    for (auto iterList = combinations.begin(); iterList != combinations.end();  ++iterList) {
        // add an index (at the beginning-NB no pushfront for vector so use insert)  for each combination vector in the list
        iterList->insert(iterList->begin(), combinationIndex);
        ++combinationIndex;
        for (iterVector = iterList->begin(); iterVector != iterList->end(); ++iterVector) {
//            cout << *iterVector << "  ";
        }
//        cout << " \n";
    }
//    cout << "================ \n\n\n";
}


// multicast combinations
void CentralScheduler::nChooseK(int offset, int n, int k , bool r) {
 if (r == false) {
     for (int i = 0; i < n; ++i) { tempNode.push_back(i); }
  }
  if (k == 0) {
      combinations.push_front (tempCombination); // push new combination vector(tempCombination) in the combination list(combinations)
    return;
  }
  for (int i = offset; i <= tempNode.size() - k; ++i) {
      tempCombination.push_back(tempNode[i]);
    nChooseK(i+1,n, k-1 , true);
    tempCombination.pop_back();
  }
}



void CentralScheduler::getNewMulticastCombination() {
    unsigned int selectedGroupIndex;
    if(randomGroup == true)   selectedGroupIndex =  std::rand() %  numAllCombinations;
    if(randomGroup == false)  selectedGroupIndex =   0;

    unsigned int selectedSender;
    if(randomGroup == true)   selectedSender = std::rand() %  numServers  ;
    if(randomGroup == false)  selectedSender =   0  ;


    // we need to make sure that each group will be selected once (Traffic Load Type I).
    bool isGroupAlreadyTaken = true ;
    while (isGroupAlreadyTaken == true) {
        auto it = alreadySelectedGroups.find(selectedGroupIndex);
        if (it != alreadySelectedGroups.end()) { // the selected group has already taken, its in alreadySelectedGroups==> select  another sender
//            cout<< "This group has been selected, so let's get another group..... " << selectedGroupIndex << " \n";
            selectedGroupIndex = std::rand() % numAllCombinations;
        } else {    // the selected group is new ==> DONE
            isGroupAlreadyTaken = false;
            alreadySelectedGroups.insert(selectedGroupIndex);
        }
    }



//     cout << "selectedGroupIndex "  << selectedGroupIndex <<"  \n";
//    cout << "initial selectedSender: "  << selectedSender <<"  \n";

    std::set<int> selectedGroup ;
    auto iterList = combinations.begin();
    std::advance(iterList, selectedGroupIndex);
    int i=0;
//    cout << "destinations group:  " ;
    for (auto iterVector = iterList->begin(); iterVector != iterList->end(); ++iterVector) {
        if (i >0 ) { // just to ignore the index part
            cout << *iterVector << " ";
            selectedGroup.insert(*iterVector);
        }
        ++i;
    }

//    cout << " \n";
    // finding a sender which is not in the selected group, as we don't want a node to send replica to itself in this traffic load
    bool isSenderInTheSelectedGroup = true ;
    while (isSenderInTheSelectedGroup == true){
    auto it=selectedGroup.find(selectedSender);
    if (it!=selectedGroup.end()) { // the selected sender is in the selected group ==> select  another sender
//        cout << "this sender node is in the selected group(it is a receiver), so let's get another sender for this group.." <<*it << " \n";
        selectedSender =    std::rand() %  numServers  ;
    } else {                     // the selected sender is not in the selected group ==> DONE
        isSenderInTheSelectedGroup = false;
    }
    }

//    cout << " \n";
//    cout << "selectedSender:  "  << selectedSender <<" \n";

//    cout << "destinations group:  " ;
    for ( auto iter = selectedGroup.begin() ; iter!=selectedGroup.end(); ++iter){
//        cout << *iter << " ";
    }
//    cout << "\n";

    multicastGroup multiG;
    multiG.groupIndex =selectedGroupIndex ;
    multiG.multicastSender= selectedSender;
    multiG.multicastReceivers= selectedGroup;
    selectedCombinations.push_back(multiG);
//    cout << "--------------- \n\n\n";

}




void CentralScheduler::getNodeRackPod(unsigned int nodeIndex,unsigned int &nodeId ,unsigned int &rackId , unsigned int &podId)
{
    Enter_Method("getNodeRackPod(unsigned int nodeIndex,unsigned int &nodeId ,unsigned int &rackId , unsigned int &podId)");

    auto itt = nodeLocationList.begin();
    while (itt != nodeLocationList.end()) {
        if (itt->index == nodeIndex) {
            nodeId=itt->node;
            rackId=itt->rack;
            podId=itt->pod;
//            nodePodRackLoc = "FatTree.Pod[" + std::to_string(itt->pod) + "].racks[" + std::to_string(itt->rack) + "].servers[" + std::to_string(itt->node) + "]";
        }
        ++itt;
    }
}

// return the set of receivers for the given  multicast group  index
void CentralScheduler::getMulticastGroupGivenIndex(unsigned int groupIndex,std::set<int>& multicastReceivers) {
    Enter_Method("getMulticastGroupGivenIndex(unsigned int groupIndex, std::set<int>& multicastReceivers )");
    auto itCom = selectedCombinations.begin();
    while (itCom != selectedCombinations.end()) {
        if (itCom->groupIndex == groupIndex) {
            multicastReceivers = itCom->multicastReceivers;
        }
        ++itCom;
    }
}

void CentralScheduler::getPodRackNodeForEachReceiverInMulticastGroup( std::set<int> multicastReceivers, std::set<std::string>& nodePodRackLoc) {
    Enter_Method("getPodRackNodeForEachReceiverInMulticastGroup(std::set<int> multicastReceivers , std::set<std::string>& nodePodRackLoc )");
    std::string receiverName;
    for (auto iter = multicastReceivers.begin(); iter!= multicastReceivers.end(); ++iter) {
        findLocation(*iter ,receiverName);
        nodePodRackLoc.insert(receiverName);
    }
}

void CentralScheduler::getMulticastGroupReceiversPortNumbers(unsigned int groupIndex,std::vector<int>& portNumbersVector) {
    Enter_Method("getMulticastGroupReceiversPortNumbers(unsigned int groupIndex,std::vector<int>& portNumbersVector) ");
    auto iter = multicastReceiversPortNumbersList.begin();
        while (iter != multicastReceiversPortNumbersList.end()) {
            if (iter->groupIndex == groupIndex) {
                portNumbersVector = iter->multicastReceiversPortNumbers;
            }
            ++iter;
        }

}


void  CentralScheduler::multicastRoutersLocation(unsigned int groupIndex, unsigned int senderId , std::set<int> receiversGroup ) {
//      std::cout << " CentralScheduler::multicastRoutersLocation( )"   <<  "\n";

      std::string coreName ="none"  ;
      std::string aggDest1 = "none"  ;
      std::string srcNode ="none"  ;
      std::string initDest ="none"  ;

      std::string routerName1 ="none"  ;
      std::string routerName2 ="none"  ;
      std::string routerName3 ="none"  ;
      std::string routerName4 ="none"  ;
      std::string routerName5 ="none"  ;

      std::string  dest1 ="none"  ;
      std::string  dest2 ="none"  ;
      std::string  dest3 ="none"  ;
      std::string  dest4 ="none"  ;
      std::string  dest5 ="none"  ;

      // multicast sender
      unsigned int senderNodeId;
      unsigned int senderRackId;
      unsigned int senderPodId;

      getNodeRackPod(senderId,  senderNodeId , senderRackId ,  senderPodId);
//      std::cout  << "\n\n multicastRoutersLocation: groupIndex " << groupIndex  << "\n";
//      std::cout  << "Tx NODE INDEX " << senderNodeId  << "\n";
//      std::cout << "Tx node: " << senderNodeId  << " , rack: " <<senderRackId << " , pod: " << senderPodId<< "\n";

      initDest  = "FatTree.Pod[" + std::to_string(senderPodId) +"].racks["+ std::to_string(senderRackId) + "].edgeSwitch";
//      std::cout << "first dest: " << initDest  <<  "\n\n";
//      std::cout << "************************* NOW lets explore all the end-to-end path for each sender-->receiver in this multicast group  \n";


     unsigned int receiverNodeId;
     unsigned int receiverRackId;
     unsigned int receiverPodId;

     // extra information
//     std::vector<int> allPods;
//     std::vector<int> allRacks;
//     std::vector<int> allNodes;
//     for (auto iter = receiversGroup.begin(); iter != receiversGroup.end(); ++iter) {
//         getNodeRackPod(*iter, receiverNodeId, receiverRackId, receiverPodId);
//         allPods.push_back(receiverPodId);
//         allRacks.push_back(receiverRackId);
//         allNodes.push_back(receiverNodeId);
//     }
//     for (auto iter = allPods.begin(); iter != allPods.end(); ++iter) {
//         std::cout << "allPods podssssss= " << *iter << " \n";
//     }
//
//     for (auto iter = allRacks.begin(); iter != allRacks.end(); ++iter) {
//         std::cout << "allRacks racksssss = " << *iter << " \n";
//     }
//
//     for (auto iter = allNodes.begin(); iter != allNodes.end(); ++iter) {
//         std::cout << "allNodess nodessss = " << *iter << " \n";
//     }


     // FatTree
//     int totalNumberofPods = kValue;
//     int totalNumberofRacks = (kValue / 2) * kValue;
     //     int serversPerPod = pow(kValue, 2) / 4;
     //     int serversPerRack = kValue / 2;

     int totalNumberofCores = (kValue / 2) * (kValue / 2);
     int racksPerPod = (kValue / 2); // = number of agg in each pod

     // map each agg to its corresponding cores
     int step=0;
     for (int i=0; i< kValue/2 ; ++i ){
         coreAggMap coreAggPairs;
         for (int j= 0; j< kValue/2 ; ++j){
             coreAggPairs.aggIndex = i;
             coreAggPairs.associatedCores.push_back(step) ;
             ++step;
         }
         coreAggMapList.push_back(coreAggPairs);
     }

     // lets get a random agg and its core
    unsigned int whichAggAtSenderPod = std::rand() % racksPerPod; // pick one agg router to be used for this mulsticast group
//    std::cout << "\n   whichAgg " << whichAggAtSenderPod << " \n";

    unsigned int whichCore;
    for (auto iter = coreAggMapList.begin(); iter != coreAggMapList.end();  ++iter) {
//        std::cout << " ;;;;; selected agg index = " << iter->aggIndex << "\n";
        if (iter->aggIndex == whichAggAtSenderPod) {
            unsigned int tempCore =  std::rand() % racksPerPod;
            whichCore = iter->associatedCores.at(tempCore);
        }
    }
//    std::cout << "whichCore =  " <<  whichCore<<"\n\n";


 //    unsigned int whichCore = std::rand() % totalNumberofCores; // pick one core router to be used for this mulsticast group WRONG
    unsigned int whichAggAtReceiverPod = whichAggAtSenderPod;


     srcNode = "FatTree.Pod[" + std::to_string(senderPodId) +"].racks["+ std::to_string(senderRackId)+ "].servers[" + std::to_string(senderNodeId)+ "]";
     initDest  = "FatTree.Pod[" + std::to_string(senderPodId) +"].racks["+ std::to_string(senderRackId) + "].edgeSwitch";
     aggDest1 = "FatTree.Pod[" + std::to_string(senderPodId) +"].aggRouters["+ std::to_string(whichAggAtSenderPod) + "]";
     coreName =  "FatTree.CoreRouter[" + std::to_string(whichCore) +"]";
     std::vector<std::string> routingTree;


     bool samePodDiffRack = false;

     // iterate over all receivers in this group
    for (auto iter = receiversGroup.begin(); iter!= receiversGroup.end(); ++iter) {
//        std::cout  << "Rx NODE INDEX: " << *iter  << "\n";
        getNodeRackPod(*iter,  receiverNodeId , receiverRackId ,  receiverPodId);
//        std::cout << "Rx node: " << receiverNodeId << " , rack: " <<receiverRackId << " , pod: " << receiverPodId<< "\n";

        // CASE 1 same pod and same rack ==> edge router is used for routing
        if (receiverPodId == senderPodId && receiverRackId == senderRackId  ){
//            std::cout << "CASE A: same pod and same rack. Path: sender-->edge-->Receiver\n";

            routingTree.push_back("bbbbbb");  // bbbbbb is used as a break point between the routing details for each node in this multicast group

            // sender --> edge
            routingTree.push_back(srcNode);
            routingTree.push_back(initDest);

            // edge --> receiver
            dest1 = "FatTree.Pod[" + std::to_string(receiverPodId) +"].racks["+ std::to_string(receiverRackId) + "].servers[" + std::to_string(receiverNodeId)+ "]";
            routingTree.push_back(dest1);
        }

        // CASE 2 same pod but different rack
        if (receiverPodId == senderPodId && receiverRackId != senderRackId  ){
//            std::cout << "CASE B: same pod but different rack. Path: sender --> edge rack i --> agg --> edge rack ii --> receiver \n";

              samePodDiffRack = true;

            routingTree.push_back("bbbbbb");

            // sender --> edge rack i
            routingTree.push_back(srcNode);
            routingTree.push_back(initDest);

            // edge rack i ==> agg
            dest1 = "FatTree.Pod[" + std::to_string(receiverPodId) +"].aggRouters["+ std::to_string(whichAggAtSenderPod) + "]";
            routingTree.push_back(dest1);

            // agg  ==> edge rack ii
            dest2 = "FatTree.Pod[" + std::to_string(receiverPodId) +"].racks["+ std::to_string(receiverRackId) + "].edgeSwitch";
            routingTree.push_back(dest2);

            // edge rack ii ==> receiver
            dest3 = "FatTree.Pod[" + std::to_string(receiverPodId) +"].racks["+ std::to_string(receiverRackId) + "].servers[" + std::to_string(receiverNodeId)+ "]";
            routingTree.push_back(dest3);
        }

        // CASE 3 different pods
        if (receiverPodId != senderPodId ){
//            std::cout << "CASE 3: different pods. Path: sender --> edge Tx --> agg Tx --> Core --> agge Rx --> edge Rx --> receiver \n";
            routingTree.push_back("bbbbbb");

            //sender --> edge Tx
            routingTree.push_back(srcNode);
            routingTree.push_back(initDest);

            // edge Tx --> agg Tx
            dest1 = "FatTree.Pod[" + std::to_string(senderPodId) +"].aggRouters["+ std::to_string(whichAggAtSenderPod) + "]";
            routingTree.push_back(dest1);

            // agg Tx --> core
            dest2 =  "FatTree.CoreRouter[" + std::to_string(whichCore) +"]";
            routingTree.push_back(dest2);

            // core --> agg Rx
            dest3 =  "FatTree.Pod[" + std::to_string(receiverPodId) +"].aggRouters["+ std::to_string(whichAggAtReceiverPod) + "]";
            routingTree.push_back(dest3);

            // agg Rx --> edge Rx
            dest4 = "FatTree.Pod[" + std::to_string(receiverPodId) + "].racks["+ std::to_string(receiverRackId) + "].edgeSwitch";
            routingTree.push_back(dest4);

            // edge --> Rx
            dest5 = "FatTree.Pod[" + std::to_string(receiverPodId) +"].racks["+ std::to_string(receiverRackId) + "].servers[" + std::to_string(receiverNodeId)+ "]";
            routingTree.push_back(dest5);
        }
    } // end for all Rx nodes. routingTree is read now.

//    std::cout << "\n\n 111111111111111111111111111111111111111111111111111111111111111111111111111111111111111 \n";
//    std::cout << "i.ROUTING TREE that shows all the full paths for each connection in the multicast group    \n";
    for(auto iter= routingTree.begin(); iter!= routingTree.end(); ++iter){
//        std::cout << " " << *iter << " \n";
    }

//    std::cout << "\n\n 222222222222222222222222222222222222222222222222222222222222222222222222222222222222222 \n";
//    std::cout << "ii. updating ROUTING TREE. We are working at the sender pod now.    \n";
//    std::cout << "   filling multicastInfoList with destNodes for: initEdge, initAgg, initCore   \n";

//    for(auto iter= routingTree.begin(); iter!= routingTree.end(); ++iter){
//        if (*iter == srcNode) {
//            routingTree.erase(iter);
//        }
//    }
    std::vector<std::string> destinationNodesForInitEdge;
    std::vector<std::string> destinationNodesForInitAgg;
    std::vector<std::string> destinationNodesForCore;

    for (auto iter = routingTree.begin(); iter != routingTree.end();) {
        if (*iter == srcNode) {        // removing the sender node from all paths
            routingTree.erase(iter);
        }  else if (*iter == initDest) { // initDest=edge
            auto itNext = std::next(iter);
            destinationNodesForInitEdge.push_back(*itNext);
            if (*itNext != aggDest1) {
                routingTree.erase(itNext);
            }
            routingTree.erase(iter);
        } else if (*iter == aggDest1) {
            auto itNext = std::next(iter);
            destinationNodesForInitAgg.push_back(*itNext);
            routingTree.erase(iter);
        } else if (*iter == coreName) {
            auto itNext = std::next(iter);
            destinationNodesForCore.push_back(*itNext);
            routingTree.erase(iter);
        } else{
            ++iter;
        }
    }
//    removeSameConsecutiveElements
    getUniqueVector(destinationNodesForInitEdge);
    getUniqueVector(destinationNodesForInitAgg);
    getUniqueVector(destinationNodesForCore);

//    std::cout << "destNodes of (initEdge, initAgg, Core): \n";
//    std::cout << "edggggggg destinationNodesForInitEdge 0000000000   \n";
     for(auto iter= destinationNodesForInitEdge.begin(); iter!= destinationNodesForInitEdge.end(); ++iter){
//         std::cout << " " << *iter << " \n";
     }

//     std::cout << "agggggggg destinationNodesForInitAgg 00000000000   \n";
     for(auto iter= destinationNodesForInitAgg.begin(); iter!= destinationNodesForInitAgg.end(); ++iter){
//         std::cout << " " << *iter << " \n";
     }

//     std::cout << "coreeeee destinationNodesForCore 0000000000   \n";
     for(auto iter= destinationNodesForCore.begin(); iter!= destinationNodesForCore.end(); ++iter){
//         std::cout << " " << *iter << " \n";
     }

//     std::cout << "\n -- Reduced ROUTING TREE after removing destNodes of (initEdge, initAgg, Core)  \n";
//     std::cout << " currently  routingTree has reached agg router for each path \n";

     // to remove any Consecutive bbbbbb marks, and to remove the last bbbbbb in routingTree(if existed)
     removeSameConsecutiveElements(routingTree);
     auto iterBack = routingTree.back(); // back() returns a reference to the last element (so don't use *iterEnd)
     if (iterBack == "bbbbbb")  routingTree.pop_back();  // this fixed a bug of having bbbbbb as a last element in the vector


     for(auto iter= routingTree.begin(); iter!= routingTree.end(); ++iter){
//         std::cout << " " << *iter << " \n";
     }

    // filling multicastInfoList with destNodes for: initEdge, initAgg, initCore
    // fill in the list that contains all nodes that are involved in multicasting and their destinations
    MulticastInfo multicastInfoSender;
    std::vector<std::string> initDestinationNodesForSender;
    initDestinationNodesForSender.push_back(initDest);
    multicastInfoSender.multicastGroupIndex = groupIndex;
    multicastInfoSender.routeName = srcNode;
    multicastInfoSender.destinations = initDestinationNodesForSender;
    multicastInfoList.push_back(multicastInfoSender);

    if (destinationNodesForInitEdge.size() != 0) {
        MulticastInfo multicastInitEdge;
        multicastInitEdge.multicastGroupIndex = groupIndex;
        multicastInitEdge.routeName = initDest;
        multicastInitEdge.destinations = destinationNodesForInitEdge;
        multicastInfoList.push_back(multicastInitEdge);
    }
    if (destinationNodesForInitAgg.size() != 0) {
        MulticastInfo multicastInitAgg;
        multicastInitAgg.multicastGroupIndex = groupIndex;
        multicastInitAgg.routeName = aggDest1;
        multicastInitAgg.destinations = destinationNodesForInitAgg;
        multicastInfoList.push_back(multicastInitAgg);
    }
    if (destinationNodesForCore.size() != 0) {
        MulticastInfo multicastCore;
        multicastCore.multicastGroupIndex = groupIndex;
        multicastCore.routeName = coreName;
        multicastCore.destinations = destinationNodesForCore;
        multicastInfoList.push_back(multicastCore);
    }


    if (routingTree.size() == 0 ) samePodDiffRack= true;
    // if the sender and all receivers are in the same rack, then routingTree.size() will be zero, so we just finished here
    if ( samePodDiffRack == false) { // this fixed a bug when the sender and all receivers are in the same rack

//        std::cout << "\n\n 3333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333 " << routingTree.size() <<  "\n";
//        std::cout << "iii. more updates on ROUTING TREE  \n";
        std::vector<std::string> leadingNodes; // the nodes after the bbbbbb marking point in routingTree
        for (auto it = routingTree.begin(); it != routingTree.end(); ++it) {
            if (*it == "bbbbbb") {
                auto itNext = std::next(it);
                leadingNodes.push_back(*itNext);
            }
        }
        getUniqueVector(leadingNodes);

//        std::cout << "leadingNodes at the reduced ROUTING TREE =   \n";
        for (auto it = leadingNodes.begin(); it != leadingNodes.end(); ++it) {
//            std::cout << " " << *it << " \n";
        }

        for (auto it = leadingNodes.begin(); it != leadingNodes.end(); ++it) {
            MulticastInfo multicastInfo;
            std::vector<std::string> destinationNodesForAgg;
            multicastInfo.multicastGroupIndex = groupIndex;
            multicastInfo.routeName = *it;
            for (auto iter = routingTree.begin(); iter != routingTree.end();  ++iter) {
                if (*it == *iter) {
                    auto itNext = std::next(iter);
                    destinationNodesForAgg.push_back(*itNext);
                    routingTree.erase(iter); // this might case a bug if we erase last iter, as we have ++iter in the for loop ..> TODO
                }
            }
            getUniqueVector(destinationNodesForAgg); //  this line fixed the bug where we get multiple same dest nodes for the same leadingAggNode
            multicastInfo.destinations = destinationNodesForAgg;
            multicastInfoList.push_back(multicastInfo);
        }
//        std::cout << "sadasdasdasdasdasdas = " <<  routingTree.size()  <<  "  \n";

        // to remove any Consecutive bbbbbb marks, and to remove the last bbbbbb in routingTree(if existed)
        auto iterEnd = routingTree.back(); // back() returns a reference to the last element (so don't use *iterEnd)
        while (iterEnd == "bbbbbb") {
            routingTree.pop_back();
            iterEnd = routingTree.back();
        }


        // get rid of any extra bbbbbb
        for (auto iter = routingTree.begin(); iter != routingTree.end();) {
            auto nextIter = std::next(iter, 2);
            if (*iter == "bbbbbb" && *nextIter == "bbbbbb") {
                auto delIter = std::next(iter, 1);
                routingTree.erase(delIter);
            } else if (*iter == "bbbbbb" && nextIter >= routingTree.end()) {
                auto delIter = std::next(iter, 1);
                routingTree.erase(delIter);
            }
            if (nextIter < routingTree.end()) {
                ++iter;
            } else {
                iter = routingTree.end();
            }
        }

        // to remove any Consecutive bbbbbb marks, and to remove the last bbbbbb in routingTree(if existed)
        iterEnd = routingTree.back(); // back() returns a reference to the last element (so don't use *iterEnd)
        while (iterEnd == "bbbbbb") {
            routingTree.pop_back();
            iterEnd = routingTree.back();
        }

//        std::cout << "\n --  NEW Reduced ROUTING TREE \n";
        for (auto itt = routingTree.begin(); itt != routingTree.end(); ++itt) {
//            std::cout << " " << *itt << " \n";
        }

        removeSameConsecutiveElements(routingTree);

        ///// lasttt

//        std::cout << "\n\n 44444444444444444444444444444444444444444444444444444444444444444444444444444444444444444 \n";
//        std::cout << "iv. last stage in updating routingTree   \n";
        std::vector<std::string> moreLastNodes;
        for (auto it = routingTree.begin(); it != routingTree.end(); ++it) {
            if (*it == "bbbbbb") {
                auto itNext = std::next(it);
                moreLastNodes.push_back(*itNext);
            }
        }
        getUniqueVector(moreLastNodes);

        for (auto it = moreLastNodes.begin(); it != moreLastNodes.end(); ++it) {
            MulticastInfo multicastInfo;
            std::vector<std::string> destinationNodes;
            multicastInfo.multicastGroupIndex = groupIndex;
            multicastInfo.routeName = *it;
            for (auto iter = routingTree.begin(); iter != routingTree.end(); ++iter) {
                if (*it == *iter) {
                    auto itNext = std::next(iter);
                    destinationNodes.push_back(*itNext);
                    routingTree.erase(iter);
                }
            }
            multicastInfo.destinations = destinationNodes;
            multicastInfoList.push_back(multicastInfo);
        }


//        std::cout << "--  Final ROUTING TREE \n";
        for (auto itt = routingTree.begin(); itt != routingTree.end(); ++itt) {
//            std::cout << " " << *itt << " \n";
        }

    }  // close this if (routingTree.size() != 0 )
//    std::cout << "\n\n DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD \n";
//    std::cout << "DONE: printing out multicastInfoList –all routing nodes for this multicast group ,,,\n";
    ofstream myfile;
    myfile.open("multicastingRoute.csv");
    for (auto iter = multicastInfoList.begin(); iter != multicastInfoList.end();++iter) {
//        std::cout << " multicast ID= " << iter->multicastGroupIndex << " \n";
//        std::cout << iter->routeName << " \n";
        myfile << iter->routeName << ",";
        std::vector<std::string> destList = iter->destinations;
        for (auto it = destList.begin(); it != destList.end(); ++it) {
//            std::cout << *it << " \n";
            myfile << *it << ",";
        }
        myfile << "bbbbbb,";
//        std::cout << "\n\n";
    }
    myfile.close();

//////////////////////////
    std::cout << "    \n\n\n";
}



void CentralScheduler::lookUpAtMulticastInfoList(unsigned int groupId, std::string currentNode, std::vector<std::string>& destinations) {
//    std::cout << "lookUpAtMulticastInfoList \n";

    for (auto iter = multicastInfoList.begin(); iter != multicastInfoList.end();  ++iter) {
        if (iter->multicastGroupIndex == groupId && strcmp(iter->routeName.c_str(), currentNode.c_str()) == 0) {
            destinations = iter->destinations;
        }
    }

}




// remove any replicated values in the input vector, get a unique vector
void CentralScheduler::getUniqueVector(std::vector<std::string>& uniqueVector) {
    std::vector<std::string>::iterator r, w;
    std::set<std::string> tmpset;

    for (r = uniqueVector.begin(), w = uniqueVector.begin();
            r != uniqueVector.end(); ++r) {
        if (tmpset.insert(*r).second) {
            *w++ = *r;
        }
    }
    uniqueVector.erase(w, uniqueVector.end());

}


// remove any consecutive replications in the input vector
void CentralScheduler::removeSameConsecutiveElements( std::vector<std::string>& inVector) {
    auto last  = std::unique(inVector.begin() , inVector.end()); // unique forwards iterator to the new end of the range
    inVector.resize(std::distance(inVector.begin(), last));
}


void CentralScheduler::scheduleMulticastGroupConn(unsigned int groupIndex, unsigned int senderId , std::set<int> receiversGroup , std::vector<int>& receiversPortNumbers)
{
//    std::cout << "!!!!! scheduleMulticastGroupConn .. !!!!!!  \n";
    std::string receiverName;
    std::string senderName;


    CentralScheduler::findLocation(senderId, senderName);


    unsigned int senderNodeId;
    unsigned int senderRackId;
    unsigned int senderPodId;

    getNodeRackPod(senderId, senderNodeId, senderRackId, senderPodId);
    std::string  initEdgeDest = "FatTree.Pod[" + std::to_string(senderPodId) + "].racks[" + std::to_string(senderRackId) + "].edgeSwitch";


       // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
       // ^^^^^^^^^^^^^^^^^^     Multicast  Receivers configurations  ^^^^^^^^^
       //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

       for (auto iter = receiversGroup.begin(); iter != receiversGroup.end(); ++iter) {
        cout << "NEW RECEIVER :)   ";
//        cout << " Rx= " << *iter  << "\n";

        CentralScheduler::findLocation( *iter, receiverName);

        // get the src tcp module
        cModule* destModule = getModuleByPath(receiverName.c_str());
        cModule* tcpDestModule = destModule->getSubmodule("raptorQ");
        int newTCPGateOutSizeDest = tcpDestModule->gateSize("appOut") + 1;
        int newTCPGateInSizeDest = tcpDestModule->gateSize("appIn") + 1;
        tcpDestModule->setGateSize("appOut", newTCPGateOutSizeDest);
        tcpDestModule->setGateSize("appIn", newTCPGateInSizeDest);
        int newNumTcpSinkAppsDest = findNumSumbodules(destModule, "inet.raptorqimplementation.application.raptorqapp.RQSinkApp") + 1;
        cModuleType *moduleTypeDest = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RQSinkApp");
        std::string nameTcpAppDest = "raptorQApp[" + std::to_string(newNumTcpSinkAppsDest - 1) + "]";

//        std::cout << "Dest  NumRQSinkApp   =  " << newNumTcpSinkAppsDest << "\n";


        // DESTttttttt  RQSinkApp
        // create (possibly compound) module and build its submodules (if any)
        cModule *newDestAppModule = moduleTypeDest->create(nameTcpAppDest.c_str(), destModule);

        newDestAppModule->par("localPort").setDoubleValue(80 + newNumTcpSinkAppsDest);
//        newDestAppModule->par("localAddress").setStringValue(receiverName.c_str());

        receiversPortNumbers.push_back(80 + newNumTcpSinkAppsDest);
//        cout << " Rx port number= " << 80 + newNumTcpSinkAppsDest  << "\n";


        newDestAppModule->par("opcode").setDoubleValue(1);

        newDestAppModule->par("isLongFlow").setBoolValue(true); // TODO
        //   --------<tcpIn         appOut[]<----------
        //     tcpApp                          tcp
        //   -------->tcpOut        appIn[] >----------
        cGate *gateTcpInDest = tcpDestModule->gate("appIn", newTCPGateOutSizeDest - 1);
        cGate *gateTcpOutDest = tcpDestModule->gate("appOut", newTCPGateOutSizeDest - 1);
        cGate *gateInDest = newDestAppModule->gate("raptorqIn");
        cGate *gateOutDest = newDestAppModule->gate("raptorqOut");
        gateTcpOutDest->connectTo(gateInDest);
        gateOutDest->connectTo(gateTcpInDest);
        newDestAppModule->finalizeParameters();
        newDestAppModule->buildInside();
        newDestAppModule->scheduleStart(simTime());
        newDestAppModule->callInitialize();

        // have been added few lines above ?? to remove
//        newDestAppModule->par("localPort").setDoubleValue(80 + newNumTcpSessionAppsSrc);
//        newDestAppModule->par("opcode").setDoubleValue(1); // receiver


        } // Rx group

       multicastGroupPortNumbers mlticastGrPorts;
       mlticastGrPorts.multicastReceiversPortNumbers = receiversPortNumbers ;
       mlticastGrPorts.groupIndex = groupIndex;
       multicastReceiversPortNumbersList.push_back(mlticastGrPorts);

       // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
           // ^^^^^^^^^^^^^^^^^^     Multicast  Sender configurations  ^^^^^^^^^^^^
           //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
           // MULTICAST SENDER  NODE
           cout << "  Tx:  " <<  senderId  << "\n";
           cModule* srcModule = getModuleByPath(senderName.c_str());
           cModule* tcpSrcModule = srcModule->getSubmodule("raptorQ");
           int newTCPGateOutSizeSrc = tcpSrcModule->gateSize("appOut") + 1;
           int newTCPGateInSizeSrc = tcpSrcModule->gateSize("appIn") + 1;
           tcpSrcModule->setGateSize("appOut", newTCPGateOutSizeSrc);
           tcpSrcModule->setGateSize("appIn", newTCPGateInSizeSrc);
           int newNumTcpSessionAppsSrc = findNumSumbodules(srcModule, "inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp") + 1;
           cModuleType *moduleTypeSrc = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp");
           std::string nameTcpAppSrc = "raptorQApp[" + std::to_string(newNumTcpSessionAppsSrc - 1) + "]";

//           std::cout << "Src  numRaptorQBasicClientApp =  " << newNumTcpSessionAppsSrc << "\n";


           // MULTICAST SENDER  RaptorQBasicClientApp
           cModule *newSrcAppModule = moduleTypeSrc->create(nameTcpAppSrc.c_str(), srcModule);


           newSrcAppModule->par("localAddress").setStringValue(senderName.c_str()); // mmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm
           newSrcAppModule->par("connectAddress").setStringValue(initEdgeDest.c_str()); // initEdgeDest initEdgeDest initEdgeDest initEdgeDest

//           newSrcAppModule->par("connectAddress").setStringValue(std::to_string(groupIndex));

           std::random_device rd; // uniformly-distributed integer random number generator-- seed
           std::mt19937 rng(rd()); // mt19937: Pseudo-random number generation
           std::exponential_distribution<double> exp(arrivalRate);
           newArrivalTime = exp.operator()(rng);
           sumArrivalTimes = sumArrivalTimes + newArrivalTime;


           newSrcAppModule->par("connectPort").setDoubleValue(80 + newNumTcpSessionAppsSrc);// this is not correct , use groupIndex
           newSrcAppModule->par("startTime").setDoubleValue(simTime().dbl()+ sumArrivalTimes);
           newSrcAppModule->par("numSymbolsToSend").setDoubleValue(flowSize); // should be longFlowSize
           newSrcAppModule->par("requestLength").setDoubleValue(10);
           newSrcAppModule->par("replyLength").setDoubleValue(1000);
           newSrcAppModule->par("opcode").setDoubleValue(2); // opcode=2 ==> Writer (sender)
           newSrcAppModule->par("fileId").setDoubleValue(2017);

           // NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN MULTICAST Parameters
           newSrcAppModule->par("multicastGroupIndex").setDoubleValue(groupIndex);
           newSrcAppModule->par("isMulticastConn").setBoolValue(true) ;


           //   --------<tcpIn         appOut[]<----------
           //     tcpApp                          tcp
           //   -------->tcpOut        appIn[] >----------
           cGate *gateTcpIn = tcpSrcModule->gate("appIn", newTCPGateInSizeSrc - 1);
           cGate *gateTcpOut = tcpSrcModule->gate("appOut", newTCPGateOutSizeSrc - 1);

           cGate *gateIn = newSrcAppModule->gate("raptorqIn");
           cGate *gateOut = newSrcAppModule->gate("raptorqOut");

           gateTcpOut->connectTo(gateIn);
           gateOut->connectTo(gateTcpIn);

           newSrcAppModule->finalizeParameters();
           newSrcAppModule->buildInside();
           newSrcAppModule->scheduleStart(simTime());
           newSrcAppModule->callInitialize();

           newSrcAppModule->par("startTime").setDoubleValue(simTime().dbl());

}






















void CentralScheduler::serversLocations()
{
    std::cout << "\n\n ******************** serversLocations .. ********************  \n";
    int totalNumberofPods = kValue;
    int totalNumberofRacks = (kValue / 2) * kValue;
    int racksPerPod = totalNumberofRacks / totalNumberofPods;
    int serversPerPod = pow(kValue, 2) / 4;
    int serversPerRack = kValue / 2;

    for (int m = 0; m < numServers; m++) {
        NodeLocation nodeLocation;
        nodeLocation.index = permServers.at(m);
        nodeLocation.pod = floor(permServers.at(m) / serversPerPod);
        nodeLocation.rack = floor((permServers.at(m) % serversPerPod) / serversPerRack);
        nodeLocation.node = permServers.at(m) % serversPerRack;
        nodeLocation.numTCPSink = 0;
        nodeLocation.numTCPSession = 0;
        nodeLocationList.push_back(nodeLocation);
    }

    std::list<NodeLocation>::iterator it;
    it = nodeLocationList.begin();
    while (it != nodeLocationList.end()) {
        std::cout << " index: " << it->index << " ==> " << " [pod, rack, node] =   ";
        std::cout << " [" << it->pod << ", " << it->rack << ", " << it->node << "] \n";
        it++;
    }
}


// random TM
void CentralScheduler::getNewDestRandTM(std::string& itsSrc, std::string& newDest) {
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  \n";
    std::cout << "******************** getNewDestination RandTM .. ********************  \n";
//    int newDestination = test;
    unsigned int newDestination = 0;
    unsigned int srcNewDestination = 0;
    while (newDestination == srcNewDestination) { // the dest should be different from the src
         newDestination = permServers.at( numlongflowsRunningServers+ (std::rand() % (numshortflowRunningServers)));
       //    int srcNewDestination = permMapShortFlows.find(newDestination)->second; // this line is used wit premTM not randTM
         srcNewDestination = permServers.at( numlongflowsRunningServers + (std::rand() % (numshortflowRunningServers)));
    }
    std::cout << "@@@ newDestination " << newDestination << " , its src   "<< srcNewDestination << "\n";

    CentralScheduler::findLocation(newDestination, newDest);
    CentralScheduler::findLocation(srcNewDestination, itsSrc);
    
    
    RecordMat recordMat;
    recordMat.recordSrc = srcNewDestination;
    recordMat.recordDest = newDestination;
    recordMatList.push_back(recordMat);
    

    
    // can be replaced by recordMatList ( see Finish())
    randMapShortFlowsVec.record(srcNewDestination);
    randMapShortFlowsVec.record(newDestination);
}


// permutation TM
void CentralScheduler::generateTM()
{
    std::random_device rd; // uniformly-distributed integer random number generator-- seed
    std::mt19937 rng(rd());
    std::cout << "\n\n ******************** generate TM maps.. ********************  \n";
    for (int i = 0; i < numServers; ++i)
        permServers.push_back(i);
     if (shuffle)
       std::shuffle(permServers.begin(), permServers.end(), rng); ///////////// TODO ooooooooooooooooooo

    for (int i = 0; i < numServers; ++i) {
        if (i < numlongflowsRunningServers) {
            permLongFlowsServers.push_back(permServers.at(i));
            permMapLongFlows.insert(std::pair<int, int>(permServers.at((i + 1) % numlongflowsRunningServers), permServers.at(i)));  // < dest, src >
        }
        else if (i >= numlongflowsRunningServers && i < numServers - 1) {
            permShortFlowsServers.push_back(permServers.at(i));
            permMapShortFlows.insert(std::pair<int, int>(permServers.at(i + 1), permServers.at(i)));  // < dest, src >
        }
        else if (i == numServers - 1) {
//            permShortFlowsServers.push_back(permServers.at(numlongflowsRunningServers));
            permShortFlowsServers.push_back(permServers.at(i));
            permMapShortFlows.insert(std::pair<int, int>(permServers.at(numlongflowsRunningServers), permServers.at(i)));  // < dest, src >
        }
    }

    std::cout << "permServers:                ";
    for (std::vector<unsigned int>::iterator it = permServers.begin(); it != permServers.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';

    std::cout << "permLongFlowsServers:       ";
    for (std::vector<unsigned int>::iterator it = permLongFlowsServers.begin(); it != permLongFlowsServers.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';

    std::cout << "permShortFlowsServers:      ";
    for (std::vector<unsigned int>::iterator it = permShortFlowsServers.begin(); it != permShortFlowsServers.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';

    std::cout << "permMapLongFlows:                 \n";
    for (std::map<unsigned int, unsigned int>::iterator iter = permMapLongFlows.begin(); iter != permMapLongFlows.end(); ++iter) {
        cout << "  src " << iter->second << " ==> ";
        cout << "  dest " << iter->first << "\n";
        permMapLongFlowsVec.record(iter->second);
        permMapLongFlowsVec.record( iter->first);
    }

    std::cout << "permMapShortFlows:                 \n";
    for (std::map<unsigned int, unsigned int>::iterator iter = permMapShortFlows.begin(); iter != permMapShortFlows.end(); ++iter) {
        cout << "   src " << iter->second << " ==> ";
        cout << "   dest " << iter->first << "\n";
        permMapShortFlowsVec.record(iter->second );
        permMapShortFlowsVec.record(iter->first );
    }

}

void CentralScheduler::getNewDestPremTM(std::string& itsSrc, std::string& newDest)
{
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@  \n";
    std::cout << "******************** getNewDestination PremTM .. ********************  \n";

//    int newDestination = test ;
//    test++;
//    if(test == 16) test =0;

    int newDestination = permServers.at(numlongflowsRunningServers + (std::rand() % (numshortflowRunningServers)));

    int srcNewDestination = permMapShortFlows.find(newDestination)->second;
    std::cout << "@@@ newDestination " << newDestination << " , its src   " << srcNewDestination << "\n";

    CentralScheduler::findLocation(newDestination, newDest);
    CentralScheduler::findLocation(srcNewDestination, itsSrc);

//    std::cout << " newDest " << newDest << "\n";
//    std::cout << " itsSrc " << itsSrc << "\n";

    RecordMat recordMat;
    recordMat.recordSrc = srcNewDestination;
    recordMat.recordDest = newDestination;
    recordMatList.push_back(recordMat);
    
    permMapShortFlowsVector.record(srcNewDestination);
    permMapShortFlowsVector.record(newDestination);
}

void CentralScheduler::findLocation(unsigned int nodeIndex, std::string& nodePodRackLoc)
{
    std::list<NodeLocation>::iterator itt;
    itt = nodeLocationList.begin();
    while (itt != nodeLocationList.end()) {
        if (itt->index == nodeIndex) {
            nodePodRackLoc = "FatTree.Pod[" + std::to_string(itt->pod) + "].racks[" + std::to_string(itt->rack) + "].servers[" + std::to_string(itt->node) + "]";
        }
        itt++;
    }
}

void CentralScheduler::scheduleLongFlows()
{
    std::cout << "\n\n ******************** scheduleLongFlows .. ********************  \n";
    std::string dest;
    std::string source;

// iterate permMapLongFlows

    for (std::map<unsigned int, unsigned int>::iterator iter = permMapLongFlows.begin(); iter != permMapLongFlows.end(); ++iter) {
        cout << "\n\n NEW LONGFLOW :)   ";
        cout << "  host(SRC.)= " << iter->second << " ==> " << "  host(DEST.)= " << iter->first << "\n";

        
        RecordMat recordMat;
        recordMat.recordSrc = iter->second ;
        recordMat.recordDest = iter->first;
        recordMatList.push_back(recordMat);

        
        CentralScheduler::findLocation(iter->first, dest); // get dest value
        CentralScheduler::findLocation(iter->second, source);

        cout << "  nodePodRackLoc:  " << iter->second << " == " << source << " ==> " << iter->first << " == " << dest << "\n";

        cModule* srcModule = getModuleByPath(source.c_str());
        cModule* destModule = getModuleByPath(dest.c_str());

//        std::cout << "srcModule:  " << srcModule->getFullPath() << "  , destModule:  " << destModule->getFullPath() << std::endl << std::endl;

        // get the src tcp module
        cModule* tcpSrcModule = srcModule->getSubmodule("raptorQ");
        cModule* tcpDestModule = destModule->getSubmodule("raptorQ");

        int newTCPGateOutSizeSrc = tcpSrcModule->gateSize("appOut") + 1;
        int newTCPGateInSizeSrc = tcpSrcModule->gateSize("appIn") + 1;
        int newTCPGateOutSizeDest = tcpDestModule->gateSize("appOut") + 1;
        int newTCPGateInSizeDest = tcpDestModule->gateSize("appIn") + 1;

        tcpSrcModule->setGateSize("appOut", newTCPGateOutSizeSrc);
        tcpSrcModule->setGateSize("appIn", newTCPGateInSizeSrc);
        tcpDestModule->setGateSize("appOut", newTCPGateOutSizeDest);
        tcpDestModule->setGateSize("appIn", newTCPGateInSizeDest);

//        std::cout << "aaaaaaaaa tcpSrcModule->gateSize(appOut) =  " << tcpSrcModule->gateSize("appOut") << std::endl;
//        std::cout << "aaaaaaaaa tcpSrcModule->gateSize(appIn)  =  " << tcpSrcModule->gateSize("appIn") << std::endl;

        int newNumTcpSessionAppsSrc = findNumSumbodules(srcModule, "inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp") + 1;
        int newNumTcpSinkAppsDest = findNumSumbodules(destModule, "inet.raptorqimplementation.application.raptorqapp.RQSinkApp") + 1;

//        std::cout << "wwwwww 1111111  =  " << findNumSumbodules(srcModule, "inet.applications.tcpapp.TCPSessionApp") << std::endl;
//        std::cout << "wwwwww 22222222 =  " << findNumSumbodules(destModule, "inet.applications.tcpapp.TCPSinkApp") << std::endl;

        std::cout << "Src  numTCPSessionApp =  " << newNumTcpSessionAppsSrc << "\n";
        std::cout << "Dest  NumTCPSinkApp   =  " << newNumTcpSinkAppsDest << "\n";

        // find factory object
        cModuleType *moduleTypeSrc = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp");
        cModuleType *moduleTypeDest = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RQSinkApp");

        std::string nameTcpAppSrc = "raptorQApp[" + std::to_string(newNumTcpSessionAppsSrc - 1) + "]";
        std::string nameTcpAppDest = "raptorQApp[" + std::to_string(newNumTcpSinkAppsDest - 1) + "]";








        // DESTttttttt  RQSinkApp
        // create (possibly compound) module and build its submodules (if any)
        cModule *newDestAppModule = moduleTypeDest->create(nameTcpAppDest.c_str(), destModule);
        newDestAppModule->par("localPort").setDoubleValue(80 + newNumTcpSessionAppsSrc);
        newDestAppModule->par("isLongFlow").setBoolValue(true);
        //   --------<tcpIn         appOut[]<----------
        //     tcpApp                          tcp
        //   -------->tcpOut        appIn[] >----------
        cGate *gateTcpInDest = tcpDestModule->gate("appIn", newTCPGateOutSizeDest - 1);
        cGate *gateTcpOutDest = tcpDestModule->gate("appOut", newTCPGateOutSizeDest - 1);

        cGate *gateInDest = newDestAppModule->gate("raptorqIn");
        cGate *gateOutDest = newDestAppModule->gate("raptorqOut");

        gateTcpOutDest->connectTo(gateInDest);
        gateOutDest->connectTo(gateTcpInDest);

        newDestAppModule->finalizeParameters();
        newDestAppModule->buildInside();
        newDestAppModule->scheduleStart(simTime());
        newDestAppModule->callInitialize();

        // have been added few lines above ?? to remove
        newDestAppModule->par("localPort").setDoubleValue(80 + newNumTcpSessionAppsSrc);
        newDestAppModule->par("opcode").setDoubleValue(1);

//        **.server.raptorQApp[*].typename  = "RQSinkApp"
//        **.server.raptorQApp[*].localAddress = ""
//        **.server.raptorQApp[*].localPort = 80
//        **.server.raptorQApp[*].opcode = 1   # 1 read, 2 write




        // SOURCeeeeee  RaptorQBasicClientApp

        //  Can we change the numTcpApps in the standardHost compund module ? ( ans: no, To be confimed)
//        int currentNumTcpApp = srcModule->par("numTcpApps").doubleValue();
//        srcModule->par("numTcpApps").setDoubleValue(3);
        cModule *newSrcAppModule = moduleTypeSrc->create(nameTcpAppSrc.c_str(), srcModule);

        newSrcAppModule->par("connectAddress").setStringValue(dest.c_str());
        newSrcAppModule->par("connectPort").setDoubleValue(80 + newNumTcpSessionAppsSrc);
//        newSrcAppModule->par("sendBytes").setDoubleValue(longFlowSize);
//        newSrcAppModule->par("tOpen").setDoubleValue(simTime().dbl());
//        newSrcAppModule->par("tSend").setDoubleValue(simTime().dbl());
        newSrcAppModule->par("startTime").setDoubleValue(simTime().dbl());
        newSrcAppModule->par("numSymbolsToSend").setDoubleValue(longFlowSize); // should be longFlowSize
        newSrcAppModule->par("requestLength").setDoubleValue(10);
        newSrcAppModule->par("replyLength").setDoubleValue(1000);
        newSrcAppModule->par("opcode").setDoubleValue(2); // opcode=2 ==> Writer
        newSrcAppModule->par("fileId").setDoubleValue(2017);


//        **.server*.raptorQApp[0].connectAddress = "server1"
//        **.server*.raptorQApp[0].connectPort = 80
//        **.server*.raptorQApp[0].startTime =0s
//        **.server*.raptorQApp[0].idleInterval = 100s
//        **.server*.raptorQApp[0].numSymbolsToSend = 70
//        **.server*.raptorQApp[0].requestLength = 10B
//        **.server*.raptorQApp[0].replyLength = 1000B
//        **.server*.raptorQApp[0].opcode = 2   # 1 read, 2 write
//        **.server*.raptorQApp[0].fileId=2017



        //   --------<tcpIn         appOut[]<----------
        //     tcpApp                          tcp
        //   -------->tcpOut        appIn[] >----------
        cGate *gateTcpIn = tcpSrcModule->gate("appIn", newTCPGateInSizeSrc - 1);
        cGate *gateTcpOut = tcpSrcModule->gate("appOut", newTCPGateOutSizeSrc - 1);

        cGate *gateIn = newSrcAppModule->gate("raptorqIn");
        cGate *gateOut = newSrcAppModule->gate("raptorqOut");

        gateTcpOut->connectTo(gateIn);
        gateOut->connectTo(gateTcpIn);

        newSrcAppModule->finalizeParameters();
        newSrcAppModule->buildInside();
        newSrcAppModule->scheduleStart(simTime());
        newSrcAppModule->callInitialize();

//        std::cout << "wwwwww 22222222  =  " << findNumSumbodules(srcModule, "inet.applications.tcpapp.TCPSessionApp") << std::endl;

        newSrcAppModule->par("startTime").setDoubleValue(simTime().dbl());
//        std::cout << "mohModule->gateCount(); " << newSrcAppModule->gateCount() << std::endl;

//        std::cout << "gateTcpIn  " << gateTcpIn->getBaseName() << "  " << gateTcpIn->getIndex() << std::endl;
//        std::cout << "gateTcpOut  " << gateTcpOut->getBaseName() << " " << gateTcpIn->getIndex() << std::endl;
//        std::cout << "gateIn  " << gateIn->getBaseName() << std::endl;
//        std::cout << "gateOut  " << gateOut->getBaseName() << std::endl;

    }
}

void CentralScheduler::scheduleNewShortFlow(std::string itsSrc, std::string newDest)
{
    std::cout << "@@@@@@@@@@@@@@@@@@@@@@@@@ scheduleNewShortFlow .. @@@@@@@@@@@@@@@@@@@@@@@@@  \n";

    std::cout << " newDest " << newDest << "\n";
    std::cout << " itsSrc " << itsSrc << "\n";

    std::random_device rd; // uniformly-distributed integer random number generator-- seed
    std::mt19937 rng(rd()); // mt19937: Pseudo-random number generation

    //  arrivalRate = 2560; //lamda
    //  double averageArrivalTime = 1/arrivalRate;
    std::exponential_distribution<double> exp(arrivalRate);

    newArrivalTime = exp.operator()(rng);
    sumArrivalTimes = sumArrivalTimes + newArrivalTime;
//     appModule->par("tSend").setDoubleValue(simTime().dbl() + sumArrivalTimes);

    cModule* srcModule = getModuleByPath(itsSrc.c_str());  // const char* c_str Return pointer to the string.
    cModule* destModule = getModuleByPath(newDest.c_str());

     std::cout << "srcModule:  " << srcModule->getFullPath() << "  , destModule:  " << destModule->getFullPath() << std::endl << std::endl;

    // get the src tcp module
    cModule* tcpSrcModule = srcModule->getSubmodule("raptorQ");
    cModule* tcpDestModule = destModule->getSubmodule("raptorQ");

    unsigned int newTCPGateOutSizeSrc = tcpSrcModule->gateSize("appOut") + 1;
    unsigned int newTCPGateInSizeSrc = tcpSrcModule->gateSize("appIn") + 1;
    unsigned int newTCPGateOutSizeDest = tcpDestModule->gateSize("appOut") + 1;
    unsigned int newTCPGateInSizeDest = tcpDestModule->gateSize("appIn") + 1;

    tcpSrcModule->setGateSize("appOut", newTCPGateOutSizeSrc);
    tcpSrcModule->setGateSize("appIn", newTCPGateInSizeSrc);
    tcpDestModule->setGateSize("appOut", newTCPGateOutSizeDest);
    tcpDestModule->setGateSize("appIn", newTCPGateInSizeDest);

//     std::cout << "aaaaaaaaa tcpSrcModule->gateSize(appOut) =  " << tcpSrcModule->gateSize("appOut") << std::endl;
//     std::cout << "aaaaaaaaa tcpSrcModule->gateSize(appIn)  =  " << tcpSrcModule->gateSize("appIn") << std::endl;

    unsigned int newNumTcpSessionAppsSrc = findNumSumbodules(srcModule, "inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp") + 1;
    unsigned int newNumTcpSinkAppsDest = findNumSumbodules(destModule, "inet.raptorqimplementation.application.raptorqapp.RQSinkApp") + 1;

//     std::cout << "wwwwww 1111111  =  " << findNumSumbodules(srcModule, "inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp") << std::endl;
//     std::cout << "wwwwww 22222222 =  " << findNumSumbodules(destModule, "inet.raptorqimplementation.application.raptorqapp.RQSinkApp") << std::endl;
    std::cout << "Src  numTCPSessionApp =  " << newNumTcpSessionAppsSrc << "\n";
    std::cout << "Dest  NumTCPSinkApp   =  " << newNumTcpSinkAppsDest << "\n";

    // find factory object
    cModuleType *moduleTypeSrc = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp");
    cModuleType *moduleTypeDest = cModuleType::get("inet.raptorqimplementation.application.raptorqapp.RQSinkApp");

    std::string nameTcpAppSrc = "raptorQApp[" + std::to_string(newNumTcpSessionAppsSrc - 1) + "]";
    std::string nameTcpAppDest = "raptorQApp[" + std::to_string(newNumTcpSinkAppsDest - 1) + "]";




    // DESTttttttt         RQSinkApp
    // create (possibly compound) module and build its submodules (if any)
    cModule *newDestAppModule = moduleTypeDest->create(nameTcpAppDest.c_str(), destModule);
    newDestAppModule->par("localPort").setDoubleValue(80 +newNumTcpSinkAppsDest);
    newDestAppModule->par("opcode").setDoubleValue(1);
    newDestAppModule->par("isLongFlow").setBoolValue(false);


//    std::cout << "Dest dasdasdasdasdasdadas   =  " << newNumTcpSinkAppsDest << "\n";

    //   --------<tcpIn         appOut[]<----------
    //     tcpApp                          tcp
    //   -------->tcpOut        appIn[] >----------
    cGate *gateTcpInDest = tcpDestModule->gate("appIn", newTCPGateOutSizeDest - 1);
//    std::cout << "Dest eeeeeee   =  " << newNumTcpSinkAppsDest << "\n";


    cGate *gateTcpOutDest = tcpDestModule->gate("appOut", newTCPGateOutSizeDest - 1);

    cGate *gateInDest = newDestAppModule->gate("raptorqIn");
    cGate *gateOutDest = newDestAppModule->gate("raptorqOut");

    gateTcpOutDest->connectTo(gateInDest);
    gateOutDest->connectTo(gateTcpInDest);

    newDestAppModule->finalizeParameters();
    newDestAppModule->buildInside();
    newDestAppModule->scheduleStart(simTime());
    newDestAppModule->callInitialize();

//    newDestAppModule->par("localPort").setDoubleValue(80 + newNumTcpSessionAppsSrc+newNumTcpSinkAppsDest); //??? to be checked





    // SOURCeeeeee  RaptorQBasicClientApp

    //  Can we change the numTcpApps in the standardHost compound module ? ( ans: no, To be confimed)
    //        int currentNumTcpApp = srcModule->par("numTcpApps").doubleValue();
    //        srcModule->par("numTcpApps").setDoubleValue(3);
    cModule *newSrcAppModule = moduleTypeSrc->create(nameTcpAppSrc.c_str(), srcModule);

    newSrcAppModule->par("connectAddress").setStringValue(newDest);
    newSrcAppModule->par("connectPort").setDoubleValue(80 + newNumTcpSinkAppsDest);  //??? to be checked
//    newSrcAppModule->par("sendBytes").setDoubleValue(flowSize);
//    newSrcAppModule->par("tOpen").setDoubleValue(simTime().dbl() + sumArrivalTimes);
//    newSrcAppModule->par("tSend").setDoubleValue(simTime().dbl() + sumArrivalTimes);

    newSrcAppModule->par("startTime").setDoubleValue(simTime().dbl()+ sumArrivalTimes);
    newSrcAppModule->par("numSymbolsToSend").setDoubleValue(flowSize); //
    newSrcAppModule->par("requestLength").setDoubleValue(10);
    newSrcAppModule->par("replyLength").setDoubleValue(1000);
    newSrcAppModule->par("opcode").setDoubleValue(2);
    newSrcAppModule->par("fileId").setDoubleValue(2017);



    //   --------<tcpIn         appOut[]<----------
    //     tcpApp                          tcp
    //   -------->tcpOut        appIn[] >----------
    cGate *gateTcpIn = tcpSrcModule->gate("appIn", newTCPGateInSizeSrc - 1);
    cGate *gateTcpOut = tcpSrcModule->gate("appOut", newTCPGateOutSizeSrc - 1);

    cGate *gateIn = newSrcAppModule->gate("raptorqIn");
    cGate *gateOut = newSrcAppModule->gate("raptorqOut");

    gateTcpOut->connectTo(gateIn);
    gateOut->connectTo(gateTcpIn);

    newSrcAppModule->finalizeParameters();
    newSrcAppModule->buildInside();
    newSrcAppModule->scheduleStart(simTime());
    newSrcAppModule->callInitialize();

//            std::cout << "wwwwww 233222222222  =  " << findNumSumbodules(srcModule, "inet.applications.tcpapp.TCPSessionApp") << std::endl;

    newSrcAppModule->par("startTime").setDoubleValue(simTime().dbl()+ sumArrivalTimes);

}

int CentralScheduler::findNumSumbodules(cModule* nodeModule, const char *subModuleType)
{
    unsigned int rep = 0;
    for (cModule::SubmoduleIterator iter(nodeModule); !iter.end(); iter++) {
        cModule *subModule = *iter;
        if (strcmp(subModule->getModuleType()->getFullName(), subModuleType) == 0) {
            rep++;
            //             std::cout << "subModule->getModuleType()->getFullName()  hhhh " << subModule->getModuleType()->getFullName() << std::endl;
        }
    }
    return rep;
}

void CentralScheduler::deleteAllSubModuleApp(const char *subModuleToBeRemoved)
{
    std::cout << "\n\n ******************** deleteAll temp SubModuleApp  .. ********************  \n";
    std::string node;

    for (int i = 0; i < numServers; i++) {

        CentralScheduler::findLocation(i, node);
        cModule* nodeModule = getModuleByPath(node.c_str());
        // delete raptorQApp[0]

        cModule* tempTcpAppModule = nullptr;
        for (cModule::SubmoduleIterator iter(nodeModule); !iter.end(); iter++) {
            cModule *subModule = *iter;
            if (strcmp(subModule->getFullName(), subModuleToBeRemoved) == 0) {
                tempTcpAppModule = subModule;
//                 std::cout << "tempTcpAppModule  " << tempTcpAppModule->getFullPath() << std::endl;
            }
        }
        tempTcpAppModule->deleteModule();

        cModule* tcpSrcModule = nodeModule->getSubmodule("raptorQ");
        tcpSrcModule->setGateSize("appOut", 0);
        tcpSrcModule->setGateSize("appIn", 0);
    }
    std::cout << " Done.. \n";

}

// permutation Traffic Matrix
void CentralScheduler::permTM(const char *longOrShortFlows)
{

//    std::random_device rd; // uniformly-distributed integer random number generator-- seed
//    std::mt19937 rng(rd()); // mt19937: Pseudo-random number generation
//
//    //  arrivalRate = 2560; //lamda
//    //  double averageArrivalTime = 1/arrivalRate;
//    std::exponential_distribution<double> exp(arrivalRate);
//
//    int selectTxNode = 0;
//    int selectRxNode = 0;
//    int startIndex = 0;
//    int lastIndex = 0;
//    int next = 0;
//    int firstLongflowsNodeIndex = 0;
//    int lastLongflowsNodeIndex = numlongflowsRunningServers - 1;
//    int firstShortflowsNodeIndex = numlongflowsRunningServers;
//    int lastShortflowsNodeIndex = numlongflowsRunningServers + numshortflowRunningServers - 1;
//
//    if (strcmp(longOrShortFlows, "longFlows") == 0) {
//        selectTxNode = firstLongflowsNodeIndex - 1;
//        selectRxNode = firstLongflowsNodeIndex;
//        startIndex = firstLongflowsNodeIndex;
//        lastIndex = lastLongflowsNodeIndex;
//    }
//    else if (strcmp(longOrShortFlows, "shortFlows") == 0) {
//        selectTxNode = firstShortflowsNodeIndex - 1;
//        selectRxNode = firstShortflowsNodeIndex;
//        startIndex = firstShortflowsNodeIndex;
//        lastIndex = lastShortflowsNodeIndex;
//    }
//
//    std::cout << "startIndex  startIndex  startIndex  startIndex  " << startIndex << std::endl;
//    std::cout << "lastIndex  lastIndex  lastIndex  lastIndex  " << lastIndex << std::endl;
//
//    for (int i = startIndex; i <= lastIndex; i++) {
//        selectTxNode++;
//        selectRxNode++;
//        next = i + 1;
//
//        if (i == lastIndex) {
//            selectTxNode = i;
//            selectRxNode = startIndex;
//            next = startIndex;
//        }
//
//        std::string conn = "host " + std::to_string(permServers.at(selectTxNode)) + " ==> " + "host " + std::to_string(permServers.at(selectRxNode));
//        std::cout << "conn:  " << conn << std::endl;
//
//        std::string src = "FatTree.Pod[" + std::to_string(podLocationForEachServer.at(i)) + "].racks[" + std::to_string(rackLocationForEachServer.at(i)) + "].servers["
//                + std::to_string(LocationForEachServerInTheRack.at(i)) + "]";
//
//        std::string dest = "FatTree.Pod[" + std::to_string(podLocationForEachServer.at(next)) + "].racks[" + std::to_string(rackLocationForEachServer.at(next)) + "].servers["
//                + std::to_string(LocationForEachServerInTheRack.at(next)) + "]";
//
//        //    FatTree.Pod[].racks[].servers[]
//        cModule* srcModule = getModuleByPath(src.c_str());
//        cModule* destModule = getModuleByPath(dest.c_str());
//
////        std::cout << "src:        " << src << std::endl;
//        std::cout << "srcModule:  " << srcModule->getFullPath() << std::endl << std::endl;
//
////        std::cout << "dest:        " << dest << std::endl;
//        std::cout << "destModule:  " << destModule->getFullPath() << std::endl;
//
//        cModule* appModule = nullptr;
//        for (cModule::SubmoduleIterator iter(srcModule); !iter.end(); iter++) {
//            cModule *subModule = *iter;
//            if (strcmp(subModule->getFullName(), "tcpApp[1]") == 0) {
//                //   std::cout << "get the modulde " << subModule->getFullPath() << std::endl;
//                appModule = subModule;
//                break;
//            }
//        }
//
//        //moh note: whenever a parameter changes, the TCPSessionApp::handleParameterChange  is called
//        //if we change three parameters, then handleParameterChange will be called three times
//        //so we need to add an if condition in this method to take actions whenever all the parameters are changed not just one
//        appModule->par("connectAddress").setStringValue(dest);
//
//        if (strcmp(longOrShortFlows, "longFlows") == 0) {
//            appModule->par("sendBytes").setDoubleValue(1000); // lllllllllllllllllllllllllll
//            appModule->par("tSend").setDoubleValue(simTime().dbl());
//        }
//        else if (strcmp(longOrShortFlows, "shortFlows") == 0) {
//
//            permMap.insert(std::pair<int, int>(permServers.at(selectRxNode), permServers.at(selectTxNode)));  // < dest, src >
////            std::cout << "selectRxNode  " <<selectRxNode <<  "  selectTxNode  " << selectTxNode  << std::endl;
//
//            newArrivalTime = exp.operator()(rng);
//            sumArrivalTimes = sumArrivalTimes + newArrivalTime;
//            appModule->par("sendBytes").setDoubleValue(flowSize);
//            appModule->par("tSend").setDoubleValue(simTime().dbl() + sumArrivalTimes);
//        }
//
//        //            appModule->par("tClose").setDoubleValue(1000000000);
//        std::cout << "simTime().dbl()  " << simTime().dbl() << std::endl;
//        // std::cout << "tClose  " <<  appModule->par("tClose").doubleValue() << std::endl;
//        std::cout << "long flows: tSend  " << appModule->par("tSend").doubleValue() << std::endl;
//        std::cout << "************\n\n\n\n ";
//    }
}

void CentralScheduler::finish()
{

    for (std::vector<unsigned int>::iterator iter = permServers.begin(); iter != permServers.end(); ++iter) {

        cout << "  NODE= " << *iter << "  ";
        nodes.record(*iter);

        std::string source;
        CentralScheduler::findLocation(*iter, source); // get dest value
        //
//        cout << "  nodePodRackLoc:  " << iter->second << " == " << source << " ==> " << iter->first << " == " << dest << "\n";
//
        cModule* srcModule = getModuleByPath(source.c_str());

        int finalNumTcpSessionApps = findNumSumbodules(srcModule, "inet.raptorqimplementation.application.raptorqapp.RaptorQBasicClientApp");
        int finalNumTcpSinkApps = findNumSumbodules(srcModule, "inet.raptorqimplementation.application.raptorqapp.RQSinkApp");

        std::cout << "  finalNumTcpSessionApps:  " << finalNumTcpSessionApps << ",  finalNumTcpSinkApps: " << finalNumTcpSinkApps << "\n";
        numTcpSessionAppsVec.record(finalNumTcpSessionApps);
        numTcpSinkAppsVec.record(finalNumTcpSinkApps);
    }



    std::cout << "numshortflowRunningServers:  " << numshortflowRunningServers << std::endl;
    std::cout << "numlongflowsRunningServers:  " << numlongflowsRunningServers << std::endl;

    std::cout << "permLongFlowsServers:       ";
    for (std::vector<unsigned int>::iterator it = permLongFlowsServers.begin(); it != permLongFlowsServers.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';




    // Record end time
    t2 = high_resolution_clock::now();
    auto duration = duration_cast<minutes>(t2 - t1).count();
    std::cout << "=================================================== "  << std::endl;
    std::cout << " total Wall Clock Time (Real Time) = " << duration <<  " minutes" << std::endl;
    std::cout << " total Simulation Time      = " << totalSimTime <<  " sec" << std::endl;
    std::cout << "=================================================== "  << std::endl;
    std::cout << " num completed shortflows = " << numCompletedShortFlows << std::endl;


    recordScalar("simTimeTotal=", totalSimTime);
    recordScalar("numShortFlows=", numShortFlows);
    recordScalar("flowSize=", flowSize);
    recordScalar("percentLongFlowNodes=", percentLongFlowNodes);
    recordScalar("arrivalRate=", arrivalRate);
    if (strcmp(trafficMatrixType, "permTM") == 0)  recordScalar("permTM", 0);
    if (strcmp(trafficMatrixType, "randTM") == 0)  recordScalar("randTM", 1);
    recordScalar("wallClockTime=", duration);


    recordScalar("bbbbbbbbbbbbbbb=", flowSize);

    //   int i=0;
    std::list<RecordMat>::iterator itt;
    itt = recordMatList.begin();
    while (itt != recordMatList.end()) {
        matSrc.record(itt->recordSrc);
        matDest.record(itt->recordDest);
        //        std::cout << " flowNumber = " << ++i  << " src: " << itt->recordSrc << " , destttttt= " << itt->recordDest << std::endl;
        itt++;
    }
}

void CentralScheduler::handleParameterChange(const char *parname)
{
//    std::cout << " CentralScheduler num completed shortflows = " << numCompletedShortFlows << std::endl;

    if (parname && strcmp(parname, "numCompletedShortFlows") == 0) {
//        numCompletedShortFlows = par("numCompletedShortFlows");
        ++numCompletedShortFlows;
        std::cout << " num completed shortflows = " << numCompletedShortFlows << "\n\n\n\n";

        if (numCompletedShortFlows == numShortFlows) {
//             getSimulation()->callFinish();
            scheduleAt(simTime(), stopSimulation);
        }
    }
}

} //namespace inet

//    int matrix[4][numServers];
//    for (int n = 0; n < 4; n++) {
//        for (int m = 0; m < numServers; m++) {
//            if (n == 0)
//                matrix[n][m] = permServers.at(m);
//            if (n == 1)
//                matrix[n][m] = podLocationForEachServer.at(m);
//            if (n == 2)
//                matrix[n][m] = rackLocationForEachServer.at(m);
//            if (n == 3)
//                matrix[n][m] = LocationForEachServerInTheRack.at(m);
//        }
//    }
//    //    Matrix= [permutationServers ;  podLocationForEachServer ; rackLocationForEachServer; LocationForEachServerInTheRack];

//        cModule* tcpSrcModule = nullptr;
//        for (cModule::SubmoduleIterator iter(srcModule); !iter.end(); iter++) {
//            cModule *subModule = *iter;
//            if (strcmp(subModule->getFullName(), "tcp") == 0) {
//                std::cout << "get the modulde 'tcp'  " << subModule->getFullPath() << std::endl;
//                tcpSrcModule = subModule;
//                break;
//            }
//        }
