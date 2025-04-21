#include "ProcessingNode.hpp"
#include "Shared.hpp"

struct ProcInfo {
    pid_t pid;
    long cpuUsage;
};

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
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <num_processes>" << endl;
        return 1;
    }

    int numProcesses = stoi(argv[1]);
    vector<ProcInfo> children;

    for (int i = 0; i < numProcesses; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }
        if (pid == 0) {
            // ProcessingNode node(getpid());
            // node.run();
            _exit(0);
        } else {
            sleep(1);
            long usage = getCpuUsage(pid);
            children.push_back({pid, usage});
        }
    }

    sort(children.begin(), children.end(), [](const ProcInfo& a, const ProcInfo& b) {
        return a.cpuUsage < b.cpuUsage;
    });

    cout << "Sorted child processes by CPU usage:\n";
    for (const auto& c : children) {
        cout << "PID: " << c.pid << " - CPU Time: " << c.cpuUsage << endl;
    }

    
    
    // Wait for all child processes to finish
    for (int i = 0; i < numProcesses; ++i) {
        wait(NULL);
    }

    return 0;
}
