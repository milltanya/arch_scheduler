#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

class Task
{
public:
    unsigned time_start; 	// время поступления задания
    unsigned priority; 		// приоритет задания
    unsigned exec_time;		// время выполнения задания
    unsigned res;			// требуемые ресурсы для выполнения
    std::vector<unsigned> starts;
    std::vector<unsigned> ends;
    unsigned time_left;
    Task(unsigned time_start_p, unsigned priority_p, unsigned exec_time_p, unsigned res_p)
    {
        time_start = time_start_p;
        priority = priority_p;
        exec_time = exec_time_p;
        res = res_p;
        starts = {};
        ends = {};
        time_left = exec_time;
    }
};

void read_data(const std::string& data, unsigned & max_res, std::vector<Task*>& tasks)
{
    int total_tasks;
    std::ifstream in;
    in.open(data);
    in >> max_res;
    in >> total_tasks;
    tasks.resize(total_tasks);
    unsigned s, p, e, r;
    for (int i = 0; i < total_tasks; ++i)
    {
        in >> s >> p >> e >> r;
        tasks[i] = new Task(s, p, e, r);
    }
    in.close();
}

class task_comp {
public:
    bool operator() (Task* left, Task* right) {
        return 1.0 * left->priority * left->time_left / left->exec_time > 1.0 * right->priority * right->time_left / right->exec_time;
    }
};

void print_tasks(const std::vector<Task*>& tasks) {
    std::cout << "------------------" << std::endl;
    std::cout << "time_start  | priority    |    exec_time |    res    |    time_left" << std::endl;
    for (Task* t : tasks) {
        std::cout <<
            t->time_start << "\t\t" <<
            t->priority << "\t\t" <<
            t->exec_time << "\t\t" <<
            t->res << "\t\t" <<
            t->time_left << "\t\t" <<
            std::endl;
    }
    std::cout << "------------------" << std::endl;
}

void schedule_and_write(unsigned max_res, const std::vector<Task*>& tasks, const std::string& res_filename) {
    std::vector<Task*> running = {};
    std::vector<Task*> waiting = {};
    std::vector<Task*> finished = {};
    unsigned resources_left;
    unsigned current_time = 1;
    int i = 0;

    for (;finished.size() < tasks.size(); ++current_time) {
        std::cout << "Time: " << current_time << std::endl;
        std::cout << "Running" << std::endl;
        print_tasks(running);
        std::cout << "Waiting" << std::endl;
        print_tasks(waiting);
        std::cout << "Finished" << std::endl;
        print_tasks(finished);

        for (Task* t : running) {
            t->time_left -= 1;
            t->ends.push_back(current_time);
            if (t->time_left == 0) {
                finished.push_back(t);
            } else {
                waiting.push_back(t);
            }
        }

        for (;i < tasks.size() && tasks[i]->time_start == current_time; ++i) {
            std::cout << "i = " << i << std::endl;
            waiting.push_back(tasks[i]);
        }

        std::cout << "Start scheduling" << std::endl;
        running.clear();
        resources_left = max_res;
        std::cout << "Sort" << std::endl;
        std::sort(waiting.begin(), waiting.end(), task_comp());
        std::cout << "Sorted" << std::endl;
        std::cout << "Resources " << max_res << std::endl;
        std::cout << "All not finished tasks" << std::endl;
        print_tasks(waiting);

        for (auto it = waiting.begin(); it != waiting.end();) {
            if ((*it)->res < resources_left) {
                resources_left -= (*it)->res;
                if (!(*it)->ends.empty() && (*it)->ends.back() == current_time) {
                    (*it)->ends.pop_back();
                } else {
                    (*it)->starts.push_back(current_time);
                }
                running.push_back((*it));
                it = waiting.erase(it);
            } else {
                ++it;
            }
        }
    }


    std::ofstream out;
    out.open(res_filename);
    for (Task* t : tasks) {
        out << t->starts.size();
        for (int j = 0; j < t->starts.size(); ++j) {
            out << " " << t->starts[j];
            out << " " << t->ends[j];
        }
        out << std::endl;
    }
    out.close();
}

int main() {
    std::string data;
    std::cout << "data_file:";
    std::cin >> data;
    unsigned max_res;
    std::vector<Task*> tasks;
    read_data(data, max_res, tasks);

    std::string res;
    std::cout << "res_file:";
    std::cin >> res;
    schedule_and_write(max_res, tasks, res);

    return 0;
}