simple Server
{
    parameters:
    //        @display("i=block/routing"); 
        int numServers;
        int numClients;
    gates:
        input in[];
        output out[];

    // You can access totalServers and totalClients in handleMessage method or other member functions
}

simple Client
{
    parameters:
        //        @display("i=block/routing");  
        int numServers;
        int numClients; 
    gates:
        input serverIn[];
        output serverOut[];
        input clientIn[];
        output clientOut[];

    // You can access totalServers and totalClients in handleMessage method or other member functions
}

network Net
{
    parameters:
        int numServers;
        int numClients;
        @display("bgb=400,300");
        
    submodules:
        s[numServers]: Server {
            @display("p=188,75");
            numServers = parent.numServers;
            numClients = parent.numClients;
        }
        c[numClients]: Client {
            @display("p=188,225");
            numServers = parent.numServers;
            numClients = parent.numClients;
        }
        
        
    connections:
        // Connect each server to each client
        
        
        for i=0..(numServers-1), for j=0..(numClients-1) {
            s[i].out++ --> {  delay = 100ms; } --> c[j].serverIn++;
            s[i].in++ <-- {  delay = 100ms; } <-- c[j].serverOut++;
        }

        // Connect each client to each other
        for i=0..(numClients-1), for j=0..(numClients-1) {
           	c[i].clientOut++ --> {  delay = 100ms; } --> c[j].clientIn++;
        }
}
