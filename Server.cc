#include <string.h>
#include <omnetpp.h>
#include <random>
#include <ctime>
#include <fstream>

#include "TaskMes_m.h"  // Include the generated header file for your message class

using namespace omnetpp;

class Server : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual bool isMalicious();
    virtual int solveTask(std::string task);


    int currentRound = 0;

    int numServers;
    int numClients;
    int id;
};

Define_Module(Server);

void createLogFile(int Index){
    std::string filename = "ServerQueries_Log_" + std::to_string(Index) + ".txt";
    std::ofstream logfile(filename);
    if (logfile.is_open()) {
        std::cout << "Log file created: " << filename << std::endl;
        logfile.close();
    } else {
        std::cerr << "Error: Unable to create log file." << std::endl;
    }
}

void writeToLogFile(std::string output, int serverId){
    std::string filename = "ServerQueries_Log_" + std::to_string(serverId) + ".txt";
    std::ofstream logfile(filename, std::ios_base::app); // Open the file in append mode
    if (logfile.is_open()) {
        logfile << output << std::endl; // Write the output to the file
        logfile.close();
    } else {
        std::cerr << "Error: Unable to open log file for writing." << std::endl;
    }
}

int generateRandomNumber(int range) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, range - 1);
    return dis(gen);
}

bool Server::isMalicious(){
    int rand = generateRandomNumber(3);  // With Probability = 1/3
    if(rand == 0)
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
        return vowels+1;
    }else
        return vowels;
}

void Server::initialize()
{
    numServers = par("numServers").intValue();
    numClients = par("numClients").intValue();
    id = par("Id").intValue();

    createLogFile(id);

}

void Server::handleMessage(cMessage *msg)
{
    if (dynamic_cast<TaskMes*>(msg) != nullptr) {
        TaskMes *myMsg = check_and_cast<TaskMes*>(msg);
        int serverId = id;

        cGate *arrivalGate = myMsg->getArrivalGate();
        int clientId = arrivalGate->getIndex();

        int taskId = myMsg->getTaskId();
        int subTaskId = myMsg->getSubTaskId();

        std::string logMessage = "";

        logMessage = "Query, From Client-" + std::to_string(clientId) + ", To Server-" + std::to_string(serverId) + ", Task-" + std::to_string(taskId) + ", SubTask-" + std::to_string(subTaskId) + ", Query:" + myMsg->getText() + ", Timestamp:" + myMsg->getTimestamp();

        EV_INFO << logMessage <<endl;

        writeToLogFile(logMessage, serverId);

        if(taskId != currentRound){
            currentRound++;
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
