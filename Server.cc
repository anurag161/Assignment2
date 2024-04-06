#include <string.h>
#include <omnetpp.h>
#include <random>
#include <ctime>

#include "TaskMes_m.h"  // Include the generated header file for your message class

using namespace omnetpp;

class Server : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual bool isMalicious();
    virtual int solveTask(std::string task);


    int maliciousCount = 0;
    int currentRound = 0;

    int numServers;
    int numClients;
};

Define_Module(Server);

void createLogFile(int getIndex){
    // Server_Log_Index
    // int Index = getIndex();
}

void writeToLogFile(std::string output){

}

int generateRandomNumber(int range) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, range - 1);
    return dis(gen);
}

bool Server::isMalicious(){
    int rand = generateRandomNumber(2*numClients);
    if(rand == 0 && maliciousCount <= numServers/4)
        return true;
    else
        return false;
}

int Server::solveTask(std::string task){
    // Count Vowels
    int vowels = 0;
    for(int i = 0; i < task.length(); i++){
        if(task[i] == 'a' || task[i] == 'e' || task[i] == 'i' || task[i] == 'o' || task[i] == 'u')
            vowels++;
    }
    if(isMalicious()){
        maliciousCount++;
        return vowels+1;
    }else
        return vowels;
}

void Server::initialize()
{
    numServers = par("numServers").intValue();
    numClients = par("numClients").intValue();
}

void Server::handleMessage(cMessage *msg)
{
    if (dynamic_cast<TaskMes*>(msg) != nullptr) {
        TaskMes *myMsg = check_and_cast<TaskMes*>(msg);
        int serverId = getIndex();

        cGate *arrivalGate = myMsg->getArrivalGate();
        int clientId = arrivalGate->getIndex();

        int taskId = myMsg->getTaskId();
        int subTaskId = myMsg->getSubTaskId();

        EV_INFO << "Query, From Client-" << clientId << ", To Server-" << serverId << ", Task-" << taskId << ", SubTask-" << subTaskId << ", Query:" << myMsg->getText() << ", Timestamp:" << myMsg->getTimestamp() <<endl;

        // string output = "";
        // writeToLogFile(output, serverId);

        if(taskId != currentRound){
            currentRound++;
            maliciousCount = 0;
        }

        int ans = solveTask(myMsg->getText());


        TaskMes *resMes = new TaskMes("Response");


        resMes->setText(std::to_string(ans).c_str());
        resMes->setTaskId(taskId);
        resMes->setSubTaskId(subTaskId);

        auto currentTime = std::time(nullptr);
        struct tm *localTime = std::localtime(&currentTime);
        char time[9]; // Format: "hh:mm:ss\0"
        std::strftime(time, sizeof(time), "%H:%M:%S", localTime);
        resMes->setTimestamp(time);

        send(resMes, "out", clientId);
    }
    delete(msg);
}
