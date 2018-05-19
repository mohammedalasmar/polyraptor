



package inet.raptorqimplementation.application.contract;

//
// Template for RQ applications. It shows what gates a RQ app
// needs, to be able to be used in ~StandardHost etc.
//
moduleinterface IRaptorQApp
{
    parameters:
        @display("i=block/app");

    gates:
        input raptorqIn @labels(RQCommand/up);
        output raptorqOut @labels(RQCommand/down);
}

