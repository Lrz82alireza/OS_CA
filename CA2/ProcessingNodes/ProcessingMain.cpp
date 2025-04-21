#include "ProcessingNode.hpp"
#include "Shared.hpp"
#include <filesystem>
#include <thread>
#include <csignal>

int sendProcInfoToLoader(const vector<ProcInfo>& children, const string& LoaderPipe) {
    int fd = open(LoaderPipe.c_str(), O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }

    for (const auto& c : children) {
        if (write(fd, &c, sizeof(ProcInfo)) == -1) {
            perror("write failed");
            exit(1);
        }
    }

    ProcInfo endSignal;
    endSignal.pid = -1;
    endSignal.cpuUsage = -1;
    strcpy(endSignal.pipePath, "end");
    if (write(fd, &endSignal, sizeof(ProcInfo)) == -1) {
        perror("write end signal failed");
        exit(1);
    }

    close(fd);
    return 0;
}

long getCpuUsage(pid_t pid) {
    string statPath = "/proc/" + to_string(pid) + "/stat";
    ifstream file(statPath);
    if (!file.is_open()) return -1;
    string dummy;
    long utime, stime;
    for (int i = 0; i < 13; ++i) file >> dummy;
    file >> utime >> stime;
    return utime + stime;
}

long getTotalCPUTime() {
    std::ifstream file("/proc/stat");
    string line;
    std::getline(file, line);
    std::istringstream iss(line);
    string cpu;
    long val, total = 0;
    iss >> cpu; // skip "cpu"
    while (iss >> val) total += val;
    return total;
}

double getCpuUsagePercent(pid_t pid) {
    long p1 = getCpuUsage(pid);
    long t1 = getTotalCPUTime();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    long p2 = getCpuUsage(pid);
    long t2 = getTotalCPUTime();
    
    long deltaP = p2 - p1;
    long deltaT = t2 - t1;
    if (deltaT == 0) return 0.0;
    return 100.0 * deltaP / deltaT;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <num_processes> <loader_pipe>\n";
        return 1;
    }

    int numProcesses = stoi(argv[1]);
    string LoaderPipe = argv[2];
    vector<ProcInfo> children;
    vector<pid_t> childPIDs;

    std::filesystem::create_directory("pipes");

    for (int i = 0; i < numProcesses; ++i) {
        string pipePath = "pipes/node_" + to_string(i);
        unlink(pipePath.c_str());
        if (!std::filesystem::exists("pipes")) {
            std::filesystem::create_directory("pipes");
        }
        if (mkfifo(pipePath.c_str(), 0666) == -1) {
            perror("mkfifo failed");
            exit(EXIT_FAILURE);
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {
            ProcessingNode node(i, pipePath);
            node.listenAndCompute();
            _exit(0);
        } else {
            childPIDs.push_back(pid);
            sleep(1); // wait for child to start
            double usage = getCpuUsagePercent(pid);
            ProcInfo info;
            info.pid = pid;
            info.cpuUsage = usage;
            strcpy(info.pipePath, pipePath.c_str());
            children.push_back(info);
        }
    }

    sort(children.begin(), children.end(), [](const ProcInfo& a, const ProcInfo& b) {
        return a.cpuUsage < b.cpuUsage;
    });

    cout << "Sorted child processes by CPU usage:\n";
    // for (const auto& c : children) {
    //     cout << "PID: " << c.pid << " - CPU Usage: " << c.cpuUsage << " - Pipe: " << c.pipePath << endl;
    // }

    sendProcInfoToLoader(children, LoaderPipe);
    cout << "Sent sorted process info to Loader.\n";

    return 0;
}
