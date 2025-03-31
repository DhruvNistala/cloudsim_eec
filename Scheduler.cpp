//
//  Scheduler.cpp
//  CloudSim
//
//  Created by ELMOOTAZBELLAH ELNOZAHY on 10/20/24.
//

#include "Scheduler.hpp"
#include <utility>
#include <iostream>

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
    SimOutput("Length of performance vector: " + to_string(Machine_GetInfo(MachineId_t(0)).performance.size()) + " and length of p-states vector: " + to_string(Machine_GetInfo(MachineId_t(0)).p_states.size()), 0);

    for (unsigned i = 0; i < active_machines; i++)
    {
        MachineInfo_t machine = Machine_GetInfo(MachineId_t(i));

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

    bool found = false;

    // First pass: Try to find an exact match for VM type and CPU type.
    for (size_t i = 0; i < vms.size(); i++)
    {
        // Skip VMs that are currently migrating
        if (migrating_vms.find(vms[i]) != migrating_vms.end())
            continue;

        VMInfo_t vm_info = VM_GetInfo(vms[i]);

        if (vm_info.vm_type == vm_type && vm_info.cpu == cpu_type)
        {
            MachineId_t machine_id = vm_info.machine_id;
            MachineInfo_t machine_info = Machine_GetInfo(machine_id);

            if ((!gpu_capable || machine_info.gpus) && machine_info.memory_size - machine_info.memory_used >= memory)
            {
                VM_AddTask(vms[i], task_id, priority);
                // update cpu util
                found = true;
                SimOutput("Scheduler::NewTask(): Added task " + to_string(task_id) + " to existing VM " + to_string(vms[i]) + " with exact match", 3);
                break;
            }
        }
    }

    // Second pass: CPU-only match if an exact match wasn't found.
    if (!found)
    {
        for (size_t i = 0; i < vms.size(); i++)
        {
            if (migrating_vms.find(vms[i]) != migrating_vms.end())
                continue;

            VMInfo_t vm_info = VM_GetInfo(vms[i]);

            if (vm_info.cpu == cpu_type)
            {
                MachineId_t machine_id = vm_info.machine_id;
                MachineInfo_t machine_info = Machine_GetInfo(machine_id);

                if ((!gpu_capable || machine_info.gpus) && machine_info.memory_size - machine_info.memory_used >= memory)
                {
                    VM_AddTask(vms[i], task_id, priority);
                    // update cpu util
                    found = true;
                    SimOutput("Scheduler::NewTask(): Added task " + to_string(task_id) + " to existing VM " + to_string(vms[i]) + " with CPU match", 3);
                    break;
                }
            }
        }
    }

    // Third pass: Create a new VM if no existing VM was found.
    if (!found)
    {
        VMId_t new_vm = VM_Create(vm_type, cpu_type);
        bool attached = false;

        // Try to attach the new VM to a machine with matching CPU and available resources.
        for (unsigned i = 0; i < Machine_GetTotal(); i++)
        {
            MachineInfo_t machine = Machine_GetInfo(MachineId_t(i));

            // Check for CPU compatibility and sufficient available memory and GPU (if needed)
            if (machine.cpu == cpu_type &&
                (!gpu_capable || machine.gpus) &&
                (machine.memory_size - machine.memory_used >= memory))
            {

                VM_Attach(new_vm, MachineId_t(i));

                // Add new_vm to the corresponding OS-specific vector.
                switch (vm_type)
                {
                case LINUX:
                    linux.push_back(new_vm);
                    break;
                case LINUX_RT:
                    linux_rt.push_back(new_vm);
                    break;
                case WIN:
                    win.push_back(new_vm);
                    break;
                case AIX:
                    aix.push_back(new_vm);
                    break;
                default:
                    break;
                }

                // Also add it to the global vms vector.
                vms.push_back(new_vm);

                // Assign the task to this new VM.
                VM_AddTask(new_vm, task_id, priority);
                // update cpu util
                found = true;
                attached = true;
                SimOutput("Scheduler::NewTask(): Created new VM for task " + to_string(task_id) +
                              " on machine " + to_string(i) + " with CPU match",
                          3);
                break;
            }
        }

        if (!attached)
        {
            // No machine with a compatible CPU (and required resources) was found.
            SimOutput("Scheduler::NewTask(): No compatible machine found for task " + to_string(task_id), 0);
            // Optionally, mark the task as unschedulable or throw an exception.
        }
    }

    if (!found && !vms.empty())
    {
        VM_AddTask(vms[0], task_id, priority);
        // update cpu util
        found = true;
        SimOutput("Scheduler::NewTask(): Added task " + to_string(task_id) + " to VM " + to_string(vms[0]) + " as absolute last resort", 3);
    }

    if (!found)
    {
        SimOutput("Scheduler::NewTask(): No compatible machine found for task " + to_string(task_id), 0);
    }
}

void Scheduler::PeriodicCheck(Time_t now)
{
    // This method should be called from SchedulerCheck()
    // SchedulerCheck is called periodically by the simulator to allow you to monitor, make decisions, adjustments, etc.
    // Unlike the other invocations of the scheduler, this one doesn't report any specific event
    // Recommendation: Take advantage of this function to do some monitoring and adjustments as necessary
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
}

void StateChangeComplete(Time_t time, MachineId_t machine_id)
{
    // Called in response to an earlier request to change the state of a machine
}

float Scheduler::CalculateCPUUtilization(MachineId_t machine_id)
{
    float required_mips = mips_util_map[machine_id];
    float cpu_utilization = required_mips / Machine_GetInfo(machine_id).p_state;
    return cpu_utilization;
}

float Scheduler::CalculateMachineUtilization(MachineId_t machine_id)
{
    MachineInfo_t machine_info = Machine_GetInfo(machine_id);
    float used_memory = machine_info.memory_used;
    float total_memory = machine_info.memory_size;
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
        break;
    case P1:
        return machine_info.performance[1];
        break;
    case P2:
        return machine_info.performance[2];
        break;
    case P3:
        return machine_info.performance[3];
        break;
    }
}