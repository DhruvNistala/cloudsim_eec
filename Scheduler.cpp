//
//  Scheduler.cpp
//  CloudSim
//
//  Created by ELMOOTAZBELLAH ELNOZAHY on 10/20/24.
//

#include "Scheduler.hpp"
#include <utility>
#include <iostream>
#include <algorithm>
#include <cassert>

static bool migrating = false;
static unsigned active_machines = 16;

void Scheduler::Init()
{
    // Find the parameters of the clusters
    // Get the total number of machines
    // For each machine:
    //      Get the type of the machine
    //      Get the memory of the machine
    //      Get the number of CPUs
    //      Get if there is a GPU or not
    //
    SimOutput("Scheduler::Init(): Total number of machines is " + to_string(Machine_GetTotal()), 0);
    SimOutput("Scheduler::Init(): Initializing scheduler", 0);
    active_machines = Machine_GetTotal();

    for (unsigned i = 0; i < active_machines; i++)
    {
        MachineInfo_t machine = Machine_GetInfo(MachineId_t(i));
        assert(Machine_GetInfo(MachineId_t(i)).s_state == S0);

        switch (machine.cpu)
        {
        case RISCV:
        {
            VMId_t new_vm = VM_Create(LINUX, RISCV);
            linux.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));

            new_vm = VM_Create(LINUX_RT, RISCV);
            linux_rt.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));
            break;
        }
        case POWER:
        {
            VMId_t new_vm = VM_Create(LINUX, POWER);
            linux.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));

            new_vm = VM_Create(LINUX_RT, POWER);
            linux_rt.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));

            new_vm = VM_Create(AIX, POWER);
            aix.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));
            break;
        }
        case ARM:
        {
            VMId_t new_vm = VM_Create(LINUX, ARM);
            linux.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));

            new_vm = VM_Create(LINUX_RT, ARM);
            linux_rt.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));

            new_vm = VM_Create(WIN, ARM);
            win.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));
            break;
        }
        case X86:
        {
            VMId_t new_vm = VM_Create(LINUX, X86);
            linux.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));

            new_vm = VM_Create(LINUX_RT, X86);
            linux_rt.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));

            new_vm = VM_Create(WIN, X86);
            win.push_back(new_vm);
            VM_Attach(new_vm, MachineId_t(i));
            break;
        }
        default:
            break;
        }

        machines.push_back(MachineId_t(i));
    }

    vms.insert(vms.end(), linux.begin(), linux.end());
    vms.insert(vms.end(), linux_rt.begin(), linux_rt.end());
    vms.insert(vms.end(), win.begin(), win.end());
    vms.insert(vms.end(), aix.begin(), aix.end());

    SimOutput("Scheduler::Init(): Created " + to_string(vms.size()) + " VMs across " + to_string(active_machines) + " machines", 3);
}

void Scheduler::MigrationComplete(Time_t time, VMId_t vm_id)
{
    // Update your data structure. The VM now can receive new tasks

    migrating_vms.erase(vm_id);
    SimOutput("MigrationComplete(): Migration of VM " + to_string(vm_id) + " completed at time " + to_string(time), 4);
}

#include <climits>

double Scheduler::CalculateTaskCPUUtilization(TaskId_t task_id)
{
    TaskInfo_t task = GetTaskInfo(task_id);
    // Compute demand based on total instructions and expected runtime.
    double runtime = double(task.target_completion - task.arrival); // in microseconds
    if (runtime <= 0)
        runtime = 1; // avoid division by zero
    double runtime_sec = runtime / 1000000.0;
    // Demand in MIPS = (total instructions in millions) / runtime (in seconds)
    double demand = (double(task.total_instructions) / 1e6) / runtime_sec;
    return demand;
}

vector<MachineId_t> Scheduler::SortMachinesByUtilization()
{
    vector<MachineId_t> sortedMachines = machines; // copy all machine IDs
    // Sort in ascending order based on the maximum of CPU and memory utilization
    sort(sortedMachines.begin(), sortedMachines.end(), [&](MachineId_t a, MachineId_t b)
         {
        double cpuA = CalculateCPUUtilization(a);
        double memA = CalculateMemoryUtilization(a);
        double utilA = std::max(cpuA, memA);
        double cpuB = CalculateCPUUtilization(b);
        double memB = CalculateMemoryUtilization(b);
        double utilB = std::max(cpuB, memB);
        return utilA < utilB; });
    return sortedMachines;
}

void Scheduler::NewTask(Time_t now, TaskId_t task_id)
{

    // Get the task parameters
    VMType_t vm_type = RequiredVMType(task_id);
    CPUType_t cpu_type = RequiredCPUType(task_id);
    unsigned memory = GetTaskMemory(task_id);
    bool gpu_capable = IsTaskGPUCapable(task_id);
    SLAType_t sla = RequiredSLA(task_id);

    Priority_t priority;
    if (sla == SLA0)
    {
        priority = HIGH_PRIORITY;
    }
    else if (sla == SLA1)
    {
        priority = MID_PRIORITY;
    }
    else
    {
        priority = LOW_PRIORITY;
    }

    // Compute the load factor for the new task.
    double taskLoad = CalculateTaskCPUUtilization(task_id);
    bool placed = false;

    vector<MachineId_t> sortedMachines = SortMachinesByUtilization();
    for (auto machine_id : sortedMachines)
    {
        double cpuUtil = CalculateCPUUtilization(machine_id);
        double memUtil = CalculateMemoryUtilization(machine_id);
        double currentUtil = std::max(cpuUtil, memUtil);
        SimOutput("NewTask(): Machine " + to_string(machine_id) +
                      " has utilization " + to_string(currentUtil),
                  3);

        SimOutput("Current util: " + to_string(currentUtil), 1);
        SimOutput("Task load: " + to_string(taskLoad), 1);

        double machineCapacity = (double)CalculateMachineMIPS(machine_id);
        double currentLoad = mips_util_map[machine_id];         // absolute MIPS demand already assigned
        double taskLoad = CalculateTaskCPUUtilization(task_id); // absolute MIPS demand for task
        double combinedUtil = (currentLoad + taskLoad) / machineCapacity;
        if (combinedUtil < 1.0)
        {
            MachineInfo_t machine_info = Machine_GetInfo(machine_id);
            // Check if machine is in a ready state (S0) before proceeding.
            if (machine_info.s_state != S0)
            {
                // If machine is powered off (S5) or in another sleep mode,
                // request it to wake up.
                Machine_SetState(machine_id, S0);
                // Optionally log that you are waking up the machine.
                SimOutput("NewTask(): Machine " + to_string(machine_id) + " is not ready (state: " +
                              to_string(machine_info.s_state) + "). Waking it up.",
                          3);
                // Skip this machine for now.
                continue;
            }

            // Try to find a compatible existing VM on this machine.
            bool vmFound = false;
            for (size_t i = 0; i < vms.size(); i++)
            {
                VMInfo_t vm_info = VM_GetInfo(vms[i]);
                if (vm_info.machine_id == machine_id &&
                    vm_info.vm_type == vm_type && vm_info.cpu == cpu_type)
                {
                    // Even if a VM exists, double-check that it’s ready.
                    if (!IsVMReady(vms[i]))
                    {
                        SimOutput("NewTask(): VM " + to_string(vms[i]) + " is not ready.", 3);
                        continue;
                    }
                    // Also check that the machine can support the memory requirement.
                    if ((!gpu_capable || machine_info.gpus) &&
                        machine_info.memory_size - machine_info.memory_used >= memory)
                    {
                        SimOutput("NewTask(): Assigning task " + to_string(task_id) +
                                      " to VM " + to_string(vms[i]) + " on machine " + to_string(machine_id),
                                  0);
                        VM_AddTask(vms[i], task_id, priority);
                        double demand = CalculateTaskCPUUtilization(task_id);
                        mips_util_map[machine_id] += demand;
                        machine_with_task[task_id] = machine_id;
                        vmFound = true;
                        placed = true;
                        break;
                    }
                }
            }
            if (!vmFound)
            {
                // Create a new VM only if the machine is ready.
                if (machine_info.cpu == cpu_type &&
                    ((!gpu_capable || machine_info.gpus) &&
                     (machine_info.memory_size - machine_info.memory_used >= memory)))
                {
                    // Since we already checked that the machine state is S0 above,
                    // we can safely proceed. If for some reason it becomes unready,
                    // the pending attachment mechanism will handle it.
                    VMId_t new_vm = VM_Create(vm_type, cpu_type);
                    PendingAttachment pa;
                    pa.vm = new_vm;
                    pa.machine_id = machine_id;
                    pa.task_id = task_id;
                    pa.priority = priority;
                    pa.demand = CalculateTaskCPUUtilization(task_id);
                    pendingAttachments.push_back(pa);
                    SimOutput("NewTask(): Created new VM " + to_string(new_vm) +
                                  " on machine " + to_string(machine_id) + " (pending attachment)",
                              0);
                    placed = true;
                    break;
                }
            }
            if (placed)
                break; // Stop after placing the task.
        }
    }

    // If still not placed, then mark as SLA violation.
    if (!placed)
    {
        SimOutput("NewTask(): Could not place task " + to_string(task_id) + " with load " + to_string(taskLoad) +
                      " due to insufficient capacity.",
                  0);
    }
}

bool Scheduler::IsVMReady(VMId_t vm)
{
    // Check if the VM is marked as migrating.
    if (migrating_vms.find(vm) != migrating_vms.end())
        return false;
    VMInfo_t info = VM_GetInfo(vm);
    // Check if the VM is attached to a valid machine.
    if (info.machine_id > Machine_GetTotal()) // or whatever the invalid ID is
        return false;

    MachineInfo_t machineInfo = Machine_GetInfo(info.machine_id);
    // Check if the machine is in an active state (for example, S0 or S0i1).
    if (machineInfo.s_state == S5) // S5 means powered off
        return false;
    return true;
}

void Scheduler::PeriodicCheck(Time_t now)
{
    const double overloadThreshold = 0.9; // Example threshold for overload

    // Iterate over all machines tracked in our machines vector.
    for (auto machine_id : machines)
    {
        unsigned machineMIPS = CalculateMachineMIPS(machine_id);
        double currentLoad = mips_util_map.count(machine_id) ? mips_util_map[machine_id] : 0.0;
        double utilization = currentLoad / double(machineMIPS);
        SimOutput("Machine " + to_string(machine_id) + " utilization: " + to_string(utilization), 3);

        // If the machine is overloaded, try to migrate an entire VM from it.
        if (utilization > overloadThreshold)
        {
            // Find a candidate VM on this machine.
            for (auto vm : vms)
            {
                VMInfo_t vmInfo = VM_GetInfo(vm);
                if (vmInfo.machine_id == machine_id && migrating_vms.find(vm) == migrating_vms.end())
                {
                    // Look for a target machine with the same CPU and OS and lower utilization.
                    vector<MachineId_t> sortedMachines = SortMachinesByUtilization();
                    for (auto target_machine : sortedMachines)
                    {
                        if (target_machine == machine_id)
                            continue;
                        MachineInfo_t targetInfo = Machine_GetInfo(target_machine);
                        // Check both CPU and OS compatibility.
                        if (targetInfo.cpu != vmInfo.cpu)
                            continue;

                        // (Assuming OS compatibility is inherent in vmInfo.vm_type and machine's CPU, or if needed, add a check here.)
                        unsigned targetMIPS = CalculateMachineMIPS(target_machine);
                        double targetLoad = mips_util_map.count(target_machine) ? mips_util_map[target_machine] : 0.0;
                        double targetUtil = targetLoad / double(targetMIPS);
                        if (targetUtil < utilization - 0.1) // margin for migration decision
                        {
                            // Migrate the entire VM.
                            MigrateVM(vm, target_machine);
                            // Once migration is initiated for this VM, break out of inner loops.
                            break;
                        }
                    }
                }
            }
        }

        // Power off machines with zero load.
        if (currentLoad <= 0)
        {
            MachineInfo_t minfo = Machine_GetInfo(machine_id);
            if (minfo.s_state != S5)
            {
                SimOutput("PeriodicCheck(): Powering off machine " + to_string(machine_id) + " (utilization 0)", 0);
                Machine_SetState(machine_id, S5);
            }
        }
    }
}

void Scheduler::Shutdown(Time_t time)
{
    // Do your final reporting and bookkeeping here.
    // Report about the total energy consumed
    // Report about the SLA compliance
    // Shutdown everything to be tidy :-)
    for (auto &vm : vms)
    {
        VM_Shutdown(vm);
    }
    SimOutput("SimulationComplete(): Finished!", 4);
    SimOutput("SimulationComplete(): Time is " + to_string(time), 4);
}

void Scheduler::TaskComplete(Time_t now, TaskId_t task_id)
{
    // Do any bookkeeping necessary for the data structures
    // Decide if a machine is to be turned off, slowed down, or VMs to be migrated according to your policy
    // This is an opportunity to make any adjustments to optimize performance/energy
    SimOutput("Scheduler::TaskComplete(): Task " + to_string(task_id) + " is complete at " + to_string(now), 4);

    MachineId_t machine_id = machine_with_task[task_id];
    double demand = CalculateTaskCPUUtilization(task_id);
    mips_util_map[machine_id] -= demand;
    machine_with_task.erase(task_id);
    if (mips_util_map[machine_id] < 0)
        mips_util_map[machine_id] = 0;
    // After task completion, if the machine now has 0 load, power it off.
    if (mips_util_map[machine_id] == 0)
    {
        MachineInfo_t minfo = Machine_GetInfo(machine_id);
        if (minfo.s_state != S5)
        {
            SimOutput("TaskComplete(): Powering off machine " + to_string(machine_id) + " (load now 0)", 0);
            Machine_SetState(machine_id, S5);
        }
    }
}

void Scheduler::SLAWarning(Time_t time, TaskId_t task_id)
{
    SimOutput("SLAWarning(): Task " + to_string(task_id) + " is violating SLA at time " + to_string(time), 2);

    // As a simple reaction, sort machines and select the most overloaded machine.
    vector<MachineId_t> sortedMachines = SortMachinesByUtilization(); // ascending order
    if (!sortedMachines.empty())
    {
        MachineId_t overloaded = sortedMachines.back();
        SimOutput("SLAWarning(): Overloaded machine " + to_string(overloaded) + " selected for migration", 3);
        // Find a candidate VM on the overloaded machine.
        for (auto vm : vms)
        {
            VMInfo_t vmInfo = VM_GetInfo(vm);
            if (vmInfo.machine_id == overloaded && !vmInfo.active_tasks.empty() &&
                migrating_vms.find(vm) == migrating_vms.end())
            {
                // Look for a target machine from sortedMachines with lower utilization.
                for (auto target : sortedMachines)
                {
                    if (target == overloaded)
                        continue;
                    MachineInfo_t targetInfo = Machine_GetInfo(target);
                    // Ensure CPU compatibility.
                    if (targetInfo.cpu != vmInfo.cpu)
                        continue;
                    double targetUtil = std::max(CalculateCPUUtilization(target), CalculateMemoryUtilization(target));
                    double overloadedUtil = std::max(CalculateCPUUtilization(overloaded), CalculateMemoryUtilization(overloaded));
                    if (targetUtil < overloadedUtil)
                    {
                        // Migrate the entire VM.
                        MigrateVM(vm, target);
                        return; // handle one migration per SLAWarning
                    }
                }
            }
        }
    }
}

void Scheduler::MigrateVM(VMId_t vm, MachineId_t target_machine)
{
    // This function migrates an entire VM to a target machine.
    // It updates the load mapping for all tasks in the VM and then calls VM_Migrate.
    VMInfo_t vmInfo = VM_GetInfo(vm);
    double vmTotalDemand = 0.0;
    for (auto t_id : vmInfo.active_tasks)
    {
        double tDemand = CalculateTaskCPUUtilization(t_id);
        vmTotalDemand += tDemand;
        // Update the machine mapping for each task to the target machine.
        machine_with_task[t_id] = target_machine;
    }
    // Update the load mapping: subtract from source and add to target.
    mips_util_map[vmInfo.machine_id] -= vmTotalDemand;
    mips_util_map[target_machine] += vmTotalDemand;

    migrating_vms.insert(vm);
    SimOutput("MigrateVM(): Migrating VM " + to_string(vm) +
                  " from machine " + to_string(vmInfo.machine_id) +
                  " to machine " + to_string(target_machine),
              3);
    assert(Machine_GetInfo(target_machine).s_state == S0);
    VM_Migrate(vm, target_machine);
}

double Scheduler::CalculateCPUUtilization(MachineId_t machine_id)
{
    double required_mips = mips_util_map[machine_id];
    // Use the actual MIPS capacity from the machine rather than p_state.
    double machine_capacity = (double)CalculateMachineMIPS(machine_id);
    // Avoid division by zero if capacity is zero.
    if (machine_capacity <= 0)
        machine_capacity = 1;
    double cpu_utilization = required_mips / machine_capacity;
    return cpu_utilization;
}
double Scheduler::CalculateMemoryUtilization(MachineId_t machine_id)
{
    MachineInfo_t machine_info = Machine_GetInfo(machine_id);
    double used_memory = machine_info.memory_used;
    double total_memory = machine_info.memory_size;
    return used_memory / total_memory;
}
unsigned Scheduler::CalculateMachineMIPS(MachineId_t machine_id)
{
    MachineInfo_t machine_info = Machine_GetInfo(machine_id);
    CPUPerformance_t p_state = machine_info.p_state;
    switch (p_state)
    {
    case P0:
        return machine_info.performance[0];
    case P1:
        return machine_info.performance[1];
    case P2:
        return machine_info.performance[2];
    default:
        return machine_info.performance[3];
    }
}

/*
 * This function gives us how much memory is required for a task
 */
double Scheduler::CalculateTaskMemoryUtilization(TaskId_t task_id)
{
    TaskInfo_t task = GetTaskInfo(task_id);
    return task.required_memory;
}

void Scheduler::StateChangeComplete(Time_t time, MachineId_t machine_id)
{
    // When a machine finishes a state change (wakes up), process pending attachments.
    for (auto it = pendingAttachments.begin(); it != pendingAttachments.end();)
    {
        if (it->machine_id == machine_id && Machine_GetInfo(machine_id).s_state == S0)
        {
            SimOutput("StateChangeComplete(): Attaching pending VM " + to_string(it->vm) +
                          " on machine " + to_string(machine_id),
                      3);
            assert(Machine_GetInfo(machine_id).s_state == S0);
            VM_Attach(it->vm, machine_id);
            VM_AddTask(it->vm, it->task_id, it->priority);
            mips_util_map[machine_id] += it->demand;
            machine_with_task[it->task_id] = machine_id;
            it = pendingAttachments.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

// Public interface below

static Scheduler Scheduler;

void InitScheduler()
{
    SimOutput("InitScheduler(): Initializing scheduler", 4);
    Scheduler.Init();
}

void HandleNewTask(Time_t time, TaskId_t task_id)
{
    SimOutput("HandleNewTask(): Received new task " + to_string(task_id) + " at time " + to_string(time), 4);
    Scheduler.NewTask(time, task_id);
}

void HandleTaskCompletion(Time_t time, TaskId_t task_id)
{
    SimOutput("HandleTaskCompletion(): Task " + to_string(task_id) + " completed at time " + to_string(time), 4);
    Scheduler.TaskComplete(time, task_id);
}

void MemoryWarning(Time_t time, MachineId_t machine_id)
{
    // The simulator is alerting you that machine identified by machine_id is overcommitted
    SimOutput("MemoryWarning(): Overflow at " + to_string(machine_id) + " was detected at time " + to_string(time), 0);
}

void MigrationDone(Time_t time, VMId_t vm_id)
{
    // The function is called on to alert you that migration is complete
    SimOutput("MigrationDone(): Migration of VM " + to_string(vm_id) + " was completed at time " + to_string(time), 4);
    Scheduler.MigrationComplete(time, vm_id);
    migrating = false;
}

void SchedulerCheck(Time_t time)
{
    // This function is called periodically by the simulator, no specific event
    SimOutput("SchedulerCheck(): SchedulerCheck() called at " + to_string(time), 4);
    Scheduler.PeriodicCheck(time);
}

void SimulationComplete(Time_t time)
{
    // This function is called before the simulation terminates Add whatever you feel like.
    cout << "SLA violation report" << endl;
    cout << "SLA0: " << GetSLAReport(SLA0) << "%" << endl;
    cout << "SLA1: " << GetSLAReport(SLA1) << "%" << endl;
    cout << "SLA2: " << GetSLAReport(SLA2) << "%" << endl; // SLA3 do not have SLA violation issues
    cout << "Total Energy " << Machine_GetClusterEnergy() << "KW-Hour" << endl;
    cout << "Simulation run finished in " << double(time) / 1000000 << " seconds" << endl;
    SimOutput("SimulationComplete(): Simulation finished at time " + to_string(time), 4);

    Scheduler.Shutdown(time);
}

void SLAWarning(Time_t time, TaskId_t task_id)
{
    Scheduler.SLAWarning(time, task_id);
}

void StateChangeComplete(Time_t time, MachineId_t machine_id)
{
    // Called in response to an earlier request to change the state of a machine
    Scheduler.StateChangeComplete(time, machine_id);
}
