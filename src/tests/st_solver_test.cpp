#include <StraightTask.h>
#include <ofv_bga/st_solver_for_BGA.h>

Apoptoz::StraightTask get_straight_task() {
    std::map<std::string, double> coefs;
    std::vector<double> tau(4);
    NetFunctionElement<double> inital_data(5, 0);

    tau[0] = 1; tau[1] = 23; tau[2] = 46; tau[3] = 158;

    coefs["a1"] = 1.5;
    coefs["a2"] = 6*0.01;
    coefs["a3"] = 0.0001;
    coefs["a1r"] = 4.93;
    coefs["a2r"] = 3.81*0.1;
    coefs["b0"] = 0.001;
    coefs["b1"] = 1;
    coefs["b2"] = 2*0.001;
    coefs["b3"] = 0.01;
    coefs["kf"] = 270;
    coefs["kg"] = 47;
    coefs["km"] = 41;
    coefs["c1"] = 20;
    coefs["c2"] = 3;
    coefs["c3"] = 1.85;
    coefs["kp"] = 100;
    coefs["d0"] = 0.0001;
    coefs["d1"] = 5.7;
    coefs["d2"] = 5*0.001;
    coefs["d3"] = 29.8;
    coefs["kb"] = 28.9;
    coefs["k1"] = 2.61*0.1;
    coefs["k2"] = 7.71*0.1;
    coefs["k3"] = 14;
    coefs["k4"] = 9*0.1;

    Apoptoz::StraightTask task;
    task.coefs(coefs);
    task.initial_data(inital_data);
    task.tau(tau);
    task.T(800);
    task.N(100000);

    return task;
}

int main() {
    auto st_task = get_straight_task();
    double h = st_task.T()/st_task.N();

    std::vector<StraightTask::controlVarBaseCPtr> storage_bases;
    storage_bases.emplace_back(std::make_shared<StraightTask::synched_data_storage::base>("p53", "input/p53.txt", h));
    storage_bases.emplace_back(std::make_shared<StraightTask::synched_data_storage::base>("Sirt1", "input/Sirt1.txt", h));
    storage_bases.emplace_back(std::make_shared<StraightTask::synched_data_storage::base>("Bax", "input/Bax.txt", h));

    std::vector<StraightTask::featureBaseCPtr> feature_bases;
    feature_bases.emplace_back(std::make_shared<BGA::feature_t::base>("a1", 0.5, 2.5));
    feature_bases.emplace_back(std::make_shared<BGA::feature_t::base>("a2", 2*0.01, 0.1));
    feature_bases.emplace_back(std::make_shared<BGA::feature_t::base>("a3", 5*0.00001, 15*0.00001));
    feature_bases.emplace_back(std::make_shared<BGA::feature_t::base>("a1r", 4, 6));
    feature_bases.emplace_back(std::make_shared<BGA::feature_t::base>("a2r", 0.2, 0.6));
}