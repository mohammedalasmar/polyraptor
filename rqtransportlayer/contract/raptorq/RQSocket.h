
#ifndef __INET_RQSocket_H
#define __INET_RQSocket_H

#include "RQCommand_m.h"
#include "inet/common/INETDefs.h"

#include "inet/networklayer/common/L3Address.h"

namespace inet {

class RQStatusInfo;

/**
 * RQSocket is a convenience class, to make it easier to manage RQ connections
 * from your application models. You'd have one (or more) RQSocket object(s)
 * in your application simple module class, and call its member functions
 * (bind(), listen(), connect(), etc.) to open, close or abort a RQ connection.
 *
 * RQSocket chooses and remembers the connId for you, assembles and sends command
 * packets (such as OPEN_ACTIVE, OPEN_PASSIVE, CLOSE, ABORT, etc.) to RQ,
 * and can also help you deal with packets and notification messages arriving
 * from RQ.
 *
 * A session which opens a connection from local port 1000 to 10.0.0.2:2000,
 * sends 16K of data and closes the connection may be as simple as this
 * (the code can be placed in your handleMessage() or activity()):
 *
 * <pre>
 *   RQSocket socket;
 *   socket.connect(Address("10.0.0.2"), 2000);
 *
 *   msg = new cMessage("data1");
 *   msg->setByteLength(16*1024);  // 16K
 *   socket.send(msg);
 *
 *   socket.close();
 * </pre>
 *
 * Dealing with packets and notification messages coming from RQ is somewhat
 * more cumbersome. Basically you have two choices: you either process those
 * messages yourself, or let RQSocket do part of the job. For the latter,
 * you give RQSocket a callback object on which it'll invoke the appropriate
 * member functions: socketEstablished(), socketDataArrived(), socketFailure(),
 * socketPeerClosed(), etc (these are methods of RQSocket::CallbackInterface).,
 * The callback object can be your simple module class too.
 *
 * This code skeleton example shows how to set up a RQSocket to use the module
 * itself as callback object:
 *
 * <pre>
 * class MyModule : public cSimpleModule, public RQSocket::CallbackInterface
 * {
 *     RQSocket socket;
 *     virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);
 *     virtual void socketFailure(int connId, void *yourPtr, int code);
 *     ...
 * };
 *
 * void MyModule::initialize() {
 *     socket.setCallbackObject(this,nullptr);
 * }
 *
 * void MyModule::handleMessage(cMessage *msg) {
 *     if (socket.belongsToSocket(msg))
 *         socket.processMessage(msg); // dispatch to socketXXXX() methods
 *     else
 *         ...
 * }
 *
 * void MyModule::socketDataArrived(int, void *, cPacket *msg, bool) {
 *     EV << "Received RQ data, " << msg->getByteLength() << " bytes\\n";
 *     delete msg;
 * }
 *
 * void MyModule::socketFailure(int, void *, int code) {
 *     if (code==RQ_I_CONNECTION_RESET)
 *         EV << "Connection reset!\\n";
 *     else if (code==RQ_I_CONNECTION_REFUSED)
 *         EV << "Connection refused!\\n";
 *     else if (code==RQ_I_TIMEOUT)
 *         EV << "Connection timed out!\\n";
 * }
 * </pre>
 *
 * If you need to manage a large number of sockets (e.g. in a server
 * application which handles multiple incoming connections), the RQSocketMap
 * class may be useful. The following code fragment to handle incoming
 * connections is from the LDP module:
 *
 * <pre>
 * RQSocket *socket = socketMap.findSocketFor(msg);
 * if (!socket)
 * {
 *     // not yet in socketMap, must be new incoming connection: add to socketMap
 *     socket = new RQSocket(msg);
 *     socket->setOutputGate(gate("RQOut"));
 *     socket->setCallbackObject(this, nullptr);
 *     socketMap.addSocket(socket);
 * }
 * // dispatch to socketEstablished(), socketDataArrived(), socketPeerClosed()
 * // or socketFailure()
 * socket->processMessage(msg);
 * </pre>
 *
 * @see RQSocketMap
 */
class INET_API RQSocket
{
  public:
    /**
     * Abstract base class for your callback objects. See setCallbackObject()
     * and processMessage() for more info.
     *
     * Note: this class is not subclassed from cObject, because
     * classes may have both this class and cSimpleModule as base class,
     * and cSimpleModule is already a cObject.
     */
    class CallbackInterface
    {
      public:
        virtual ~CallbackInterface() {}
        virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent) = 0;
        virtual void socketEstablished(int connId, void *yourPtr) {}
        virtual void socketPeerClosed(int connId, void *yourPtr) {}
        virtual void socketClosed(int connId, void *yourPtr) {}
        virtual void socketFailure(int connId, void *yourPtr, int code) {}
        virtual void socketStatusArrived(int connId, void *yourPtr, RQStatusInfo *status) { delete status; }
        virtual void socketDeleted(int connId, void *yourPtr) {}
    };

    enum State { NOT_BOUND, BOUND, LISTENING, CONNECTING, CONNECTED, PEER_CLOSED, LOCALLY_CLOSED, CLOSED, SOCKERROR };

  protected:
    int connId;
    int sockstate;

    L3Address localAddr;
    int localPrt;
    L3Address remoteAddr;
    int remotePrt;

    CallbackInterface *cb;
    void *yourPtr;

    cGate *gateToRq;

    RQDataTransferMode dataTransferMode;
    std::string rqAlgorithmClass;

  protected:
    void sendToRQ(cMessage *msg);

    // internal: implementation behind listen() and listenOnce()
    void listen(bool fork);

  public:
    /**
     * Constructor. The getConnectionId() method returns a valid Id right after
     * constructor call.
     */
    RQSocket();

    /**
     * Constructor, to be used with forked sockets (see listen()).
     * The new connId will be picked up from the message: it should have
     * arrived from RQ and contain RQCommmand control info.
     */
    RQSocket(cMessage *msg);

    /**
     * Destructor
     */
    ~RQSocket();

    /**
     * Returns the internal connection Id. RQ uses the (gate index, connId) pair
     * to identify the connection when it receives a command from the application
     * (or RQSocket).
     */
    int getConnectionId() const { return connId; }

    /**
     * Returns the socket state, one of NOT_BOUND, CLOSED, LISTENING, CONNECTING,
     * CONNECTED, etc. Messages received from RQ must be routed through
     * processMessage() in order to keep socket state up-to-date.
     */
    int getState() { return sockstate; }

    /**
     * Returns name of socket state code returned by getState().
     */
    static const char *stateName(int state);

    void setState(enum State state) { sockstate = state; };

    /** @name Getter functions */
    //@{
    L3Address getLocalAddress() { return localAddr; }
    int getLocalPort() { return localPrt; }
    L3Address getRemoteAddress() { return remoteAddr; }
    int getRemotePort() { return remotePrt; }
    //@}

    /** @name Opening and closing connections, sending data */
    //@{

    /**
     * Sets the gate on which to send to RQ. Must be invoked before socket
     * can be used. Example: <tt>socket.setOutputGate(gate("raptorqOut"));</tt>
     */
    void setOutputGate(cGate *toRq) { gateToRq = toRq; }

    /**
     * Bind the socket to a local port number.
     */
    void bind(int localPort);

    /**
     * Bind the socket to a local port number and IP address (useful with
     * multi-homing).
     */
    void bind(L3Address localAddr, int localPort);

    /**
     * Returns the current dataTransferMode parameter.
     * @see RQCommand
     */
    RQDataTransferMode getDataTransferMode() const { return dataTransferMode; }

    /**
     * Returns the current rqAlgorithmClass parameter.
     */
    const char *getRqAlgorithmClass() const { return rqAlgorithmClass.c_str(); }

    /**
     * Convert a string to RQDataTransferMode enum.
     * Returns RQ_TRANSFER_UNDEFINED when string has an invalid value
     * Generate runtime error, when string is nullptr;
     */
    static RQDataTransferMode convertStringToDataTransferMode(const char *transferMode);

    /**
     * Sets the dataTransferMode parameter of the subsequent connect() or listen() calls.
     * @see RQCommand
     */
    void setDataTransferMode(RQDataTransferMode transferMode) { dataTransferMode = transferMode; }

    /**
     * Read "dataTransferMode" parameter from ini/ned, and set dataTransferMode member value
     *
     * Generate runtime error when parameter is missing or value is invalid.
     */
    void readDataTransferModePar(cComponent& component);

    /**
     * Sets the rqAlgorithmClass parameter of the next connect() or listen() call.
     */
    void setRqAlgorithmClass(const char *rqAlgorithmClass) { this->rqAlgorithmClass = rqAlgorithmClass; }

    /**
     * Initiates passive OPEN, creating a "forking" connection that will listen
     * on the port you bound the socket to. Every incoming connection will
     * get a new connId (and thus, must be handled with a new RQSocket object),
     * while the original connection (original connId) will keep listening on
     * the port. The new RQSocket object must be created with the
     * RQSocket(cMessage *msg) constructor.
     *
     * If you need to handle multiple incoming connections, the RQSocketMap
     * class can also be useful, and RQSrvHostApp shows how to put it all
     * together. See also RQOpenCommand documentation (neddoc) for more info.
     */
    void listen() { listen(true); }

    /**
     * Initiates passive OPEN to create a non-forking listening connection.
     * Non-forking means that RQ will accept the first incoming
     * connection, and refuse subsequent ones.
     *
     * See RQOpenCommand documentation (neddoc) for more info.
     */
    void listenOnce() { listen(false); }

    /**
     * Active OPEN to the given remote socket.
     */
    void connect(L3Address remoteAddr, int remotePort,int fileID ,  unsigned int numSymbolsToSend, bool isLongFlow , bool isMulticastConn ,   unsigned int multicastGroupIndex , bool isMultiSourcingConn, unsigned int multiSourcingGroupIndex);

    /**
     * Sends data packet.
     */
    void send(cMessage *msg);

    /**
     * Sends command.
     */
    void sendCommand(cMessage *msg);

    /**
     * Closes the local end of the connection. With RQ, a CLOSE operation
     * means "I have no more data to send", and thus results in a one-way
     * connection until the remote RQ closes too (or the FIN_WAIT_1 timeout
     * expires)
     */
    void close();

    /**
     * Aborts the connection.
     */
    void abort();

    /**
     * Causes RQ to reply with a fresh RQStatusInfo, attached to a dummy
     * message as getControlInfo(). The reply message can be recognized by its
     * message kind RQ_I_STATUS, or (if a callback object is used)
     * the socketStatusArrived() method of the callback object will be
     * called.
     */
    void requestStatus();

    /**
     * Required to re-connect with a "used" RQSocket object.
     * By default, a RQSocket object is tied to a single RQ connection,
     * via the connectionId. When the connection gets closed or aborted,
     * you cannot use the socket to connect again (by connect() or listen())
     * unless you obtain a new connectionId by calling this method.
     *
     * BEWARE if you use RQSocketMap! RQSocketMap uses connectionId
     * to find RQSockets, so after calling this method you have to remove
     * the socket from your RQSocketMap, and re-add it. Otherwise RQSocketMap
     * will get confused.
     *
     * The reason why one must obtain a new connectionId is that RQ still
     * has to maintain the connection data structure (identified by the old
     * connectionId) internally for a while (2 maximum segment lifetimes = 240s)
     * after it reported "connection closed" to us.
     */
    void renewSocket();
    //@}

    /** @name Handling of messages arriving from RQ */
    //@{
    /**
     * Returns true if the message belongs to this socket instance (message
     * has a RQCommand as getControlInfo(), and the connId in it matches
     * that of the socket.)
     */
    bool belongsToSocket(cMessage *msg);

    /**
     * Returns true if the message belongs to any RQSocket instance.
     * (This basically checks if the message has a RQCommand attached to
     * it as getControlInfo().)
     */
    static bool belongsToAnyRQSocket(cMessage *msg);

    /**
     * Sets a callback object, to be used with processMessage().
     * This callback object may be your simple module itself (if it
     * multiply inherits from CallbackInterface too, that is you
     * declared it as
     * <pre>
     * class MyAppModule : public cSimpleModule, public RQSocket::CallbackInterface
     * </pre>
     * and redefined the necessary virtual functions; or you may use
     * dedicated class (and objects) for this purpose.
     *
     * RQSocket doesn't delete the callback object in the destructor
     * or on any other occasion.
     *
     * YourPtr is an optional pointer. It may contain any value you wish --
     * RQSocket will not look at it or do anything with it except passing
     * it back to you in the CallbackInterface calls. You may find it
     * useful if you maintain additional per-connection information:
     * in that case you don't have to look it up by connId in the callbacks,
     * you can have it passed to you as yourPtr.
     */
    void setCallbackObject(CallbackInterface *cb, void *yourPtr = nullptr);

    /**
     * Examines the message (which should have arrived from RQ),
     * updates socket state, and if there is a callback object installed
     * (see setCallbackObject(), class CallbackInterface), dispatches
     * to the appropriate method of it with the same yourPtr that
     * you gave in the setCallbackObject() call.
     *
     * The method deletes the message, unless (1) there is a callback object
     * installed AND (2) the message is payload (message kind RQ_I_DATA or
     * RQ_I_URGENT_DATA) when the responsibility of destruction is on the
     * socketDataArrived() callback method.
     *
     * IMPORTANT: for performance reasons, this method doesn't check that
     * the message belongs to this socket, i.e. belongsToSocket(msg) would
     * return true!
     */
    void processMessage(cMessage *msg);
    //@}
};

} // namespace inet

#endif // ifndef __INET_RQSocket_H

