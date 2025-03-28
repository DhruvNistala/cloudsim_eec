//
//  Interfaces.h
//  CloudSim
//
//  Created by ELMOOTAZBELLAH ELNOZAHY on 11/3/24.
//

#ifndef Interfaces_h
#define Interfaces_h

// This header defines the public interfaces between the various modules. We have the following modules:
// Debugging (messages and exceptions)
// Machines
// Scheduler
// Tasks
// VM (virtual machines)

#include <string>
#include <stdexcept>

#include "SimTypes.h"

// Debugging Interface – errors by simulator, can use try/catch
extern void             SimOutput(string msg, unsigned verbose_level); // can add messages to audit trail
extern void             ThrowException(string err_msg); // prints error message, stops simulation, only use verbosity levels 0-3. 
extern void             ThrowException(string err_msg, string further_input);
extern void             ThrowException(string err_msg, unsigned further_input);

// Machine Interface – Down calls, scheduler monitors energy
extern CPUType_t        Machine_GetCPUType(MachineId_t machine_id); // gets machine cpu type
extern uint64_t         Machine_GetEnergy(MachineId_t machine_id); // get total energy consumed for specific machine
extern double           Machine_GetClusterEnergy(); // gets energy for the whole cluster
extern MachineInfo_t    Machine_GetInfo(MachineId_t machine_id); // tells you number of cpus, cpu type, memory size and usage, number of tasks and vms, energy consumeed, etc
extern unsigned         Machine_GetTotal(); // gives total number of machines
extern void             Machine_SetCorePerformance(MachineId_t machine_id, unsigned core_id, CPUPerformance_t p_state);  // This is oriented toward dynamic energy -> sets P state for ALL cores on this machine, core_id ignored
extern void             Machine_SetState(MachineId_t machine_id, MachineState_t s_state); // main way to control machine energy, sets S states, Lower power is less energy, but longer wakeup time

// Scheduler Interface – up calls, info for scheduler to react to
extern void             InitScheduler();                                    // Called once at the beginning, initializes scheduler
extern void             HandleNewTask(Time_t time, TaskId_t task_id);       // Called every time a new task arrives to the system, sched. should decide which VM should host the task, and what priority
extern void             HandleTaskCompletion(Time_t time, TaskId_t task_id);// Called whenver a task finishes; scheduler might need to rebalance workload or free resources
extern void             MemoryWarning(Time_t time, MachineId_t machine_id); // Called to alert the scheduler of memory overcommitment -> if server overloaded
extern void             MigrationDone(Time_t time, VMId_t vm_id);           // Called to alert the scheduler that the VM has been migrated successfully, SYNCH MECHANISM
extern void             SchedulerCheck(Time_t time);                        // Called periodically. You may want to do some monitoring and adjustments. Lets scheduler make adjustments
extern void             SimulationComplete(Time_t time);                    // Called at the end of the simulation, used for reporting final results
extern void             SLAWarning(Time_t time, TaskId_t task_id);          // Called to alert the schedule of an SLA violation -> reactions: increase priority, migrate to faster server, or activate more resources
extern void             StateChangeComplete(Time_t time, MachineId_t machine_id);   // Called in response to an earlier request to change the state of a machine. tells scheduler state transition is done

// Statistics
extern double           GetSLAReport(SLAType_t sla);

// Simulator Interface
extern Time_t           Now();

// Task Interface – Down calls, lets scheduler find out information about tasks
extern unsigned         GetNumTasks(); // gets total number of tasks, including those that haven't yet arrived as well as completed
extern TaskInfo_t       GetTaskInfo(TaskId_t task_id); // returns detailed info about a specific task (completion status, needed processing, target completion time (SLA), etc.)
extern unsigned         GetTaskMemory(TaskId_t task_id); // individual task required memory
extern unsigned         GetTaskPriority(TaskId_t task_id); // task priority assigned to task
extern bool             IsSLAViolation(TaskId_t task_id); // if task is gonna miss deadline (SLA violation)
extern bool             IsTaskCompleted(TaskId_t task_id); // if task is finished
extern bool             IsTaskGPUCapable(TaskId_t task_id); // if task performance can be improved with GPU
extern CPUType_t        RequiredCPUType(TaskId_t task_id); // required type of cpu
extern SLAType_t        RequiredSLA(TaskId_t task_id); // sla0, sla1, sla2, sla3
extern VMType_t         RequiredVMType(TaskId_t task_id); // specific vm type required
extern void             SetTaskPriority(TaskId_t task_id, Priority_t priority); // scheduler can raise or lower priority

// VM Interface – Down calls, scheduler calls to tell simulator to do something
extern void             VM_Attach(VMId_t vm_id, MachineId_t machine_id); // link VM to physical server (check for CPU compatibility AND server has to be running)
extern void             VM_AddTask(VMId_t vm_id, TaskId_t task_id, Priority_t priority); // add task to specific VM, assign priority, check compatibility with VM, OS, CPU
extern VMId_t           VM_Create(VMType_t vm_type, CPUType_t cpu); // Creates VM (keep compatibility in mind)
extern VMInfo_t         VM_GetInfo(VMId_t vm_id); // get details like active tasks, CPU type, attached server, and VM type
extern void             VM_Migrate(VMId_t vm_id, MachineId_t machine_id); // moves VM and all tasks to different server, but resource intensive. Restrictions: CPU compatibility, destination server can't be sleeping
extern void             VM_RemoveTask(VMId_t vm_id, TaskId_t task_id); // Takes task off VM for load balancing
extern void             VM_Shutdown(VMId_t vm_id); // shuts down VM if no active tasks, frees up resources it takes on machine

#endif /* Interfaces_h */
