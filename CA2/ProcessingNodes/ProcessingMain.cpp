#include "ProcessingNode.hpp"
#include "Shared.hpp"
#include <filesystem>

int sendProcInfoToLoader(const vector<ProcInfo>& children, const string& LoaderPipe) {
    int fd = open(LoaderPipe.c_str(), O_WRONLY);
    if (fd == -1) {
        perror("open failed");
        exit(1);
    }

    for (const auto& c : children) {
        ProcInfo info = c;
        if (write(fd, &info, sizeof(ProcInfo)) == -1) {
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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <num_processes>\n";
        return 1;
    }

    int numProcesses = stoi(argv[1]);
    string LoaderPipe = argv[2];
    vector<ProcInfo> children;

    std::filesystem::create_directory("pipes");

    for (int i = 0; i < numProcesses; ++i) {
        string pipePath = "pipes/node_" + to_string(i);
        unlink(pipePath.c_str());
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
            sleep(1);
            long usage = getCpuUsage(pid);
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
    for (const auto& c : children) {
        cout << "PID: " << c.pid << " - CPU Time: " << c.cpuUsage << " - Pipe: " << c.pipePath << endl;
    }

    // send Sorted ProcInfo to Loader
    sendProcInfoToLoader(children, LoaderPipe);
    cout << "Sent sorted process info to Loader.\n";

    for (int i = 0; i < numProcesses; ++i) {
        wait(NULL);
    }

    return 0;
}
