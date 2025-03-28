// this file is only for referencing the structs. Not used anywhere in the code.

typedef enum { HIGH_PRIORITY, MID_PRIORITY, LOW_PRIORITY } Priority_t;
#define PRIORITY_LEVELS 3 , // System has 3 levels of priority,

typedef enum { LINUX, LINUX_RT, WIN, AIX } VMType_t;

typedef struct {
vector<TaskId_t> active_tasks;
CPUType_t cpu;
MachineId_t machine_id;
VMId_t vm_id;
VMType_t vm_type;
} VMInfo_t;

typedef struct {
    bool completed; // Set to true if the task has completed and is no longer active
    uint64_t total_instructions; // Total instructions necessary to run the task
    uint64_t remaining_instructions; // The instructions yet to execute to complete the task.
    Time_t arrival; // When did the task arrive?
    Time_t completion; // The time when the task completed. Invalid for active tasks
    Time_t target_completion; // The target completion for the task to satisfy the SLA
    bool gpu_capable; // Can the task benefit from a GPU (significant boost)
    Priority_t priority; // Task priority. One of three levels.
    CPUType_t required_cpu; // Specifies the expected CPU
    unsigned required_memory; // How much memory is required by the task.
    SLAType_t required_sla; // The type of service level agreements.
    VMType_t required_vm; // The type of VM that is required by the task
    TaskId_t task_id; // The task’s unique identifier
    } TaskInfo_t;

typedef enum {
    SLA0, // SLA requires 95% of tasks to finish within expected time
    SLA1, // SLA requires 90% of tasks to finish within expected time
    SLA2, // SLA requires 80% of tasks to finish within expected time
    SLA3 // Task to finish on a best effort basis
    } SLAType_t;
    #define NUM_SLAS 4

typedef enum {
    C0, // CPU is at state C0, in this case the power consumption is defined by the P-
    states
    C1, // CPU is at state C1 (halted but ready)
    C2, // CPU is clocked gated off
    C4 // CPU is powered gated off, note: C3 is not supported
    } CPUState_t;
    #define C_STATES 4 // For C0, the power consumption is defined by the P-states
    typedef enum {
    S0, // Machine is up. CPU's are at state C0 if running a task or C1
    S0i1, // Machine is up. CPU's are all in C1 state. Instantenous response.
    S1, // Machine is up. CPU's are in C2 state. Some delay in response time.
    S2, // S1 + CPUs are in C4 state. Delay in response time.
    S3, // S2 + DRAM in self-refresh. Serious delay in response time.
    S4, // S3 + DRAM is powered down. Large delay in response time.
    S5 // Machine is powered down.
    } MachineState_t;
    #define S_STATES 7

    typedef struct {
    unsigned num_cpus; // Number of CPU's on the machine
    CPUType_t cpu; // CPU type deployed in the machine
    unsigned memory_size; // Size of memory
    unsigned memory_used; // The memory currently in use
    unsigned active_tasks; // Number of tasks that are assigned to this machine
    unsigned active_vms; // Number of virtual machines attached to this machine
    bool gpus; // True if the processors are equipped with a GPU
    uint64_t energy_consumed; // How much energy has been consumed so far
    vector<unsigned> performance; // The MIPS ratings for the CPUs at different p-state
    vector<unsigned> c_states; // Power consumption under different C states
    vector<unsigned> p_states; // Power consumption for cores at different P states.
    Valid only when C-state is C0.
    vector<unsigned> s_states; // Machine power consumption under different S
    states
    MachineState_t s_state; // The current S state of the machine
    CPUPerformance_t p_state; // The current P state of the CPUs (all CPUs are set to
    the same P state to simplify scheduling
    MachineId_t machine_id; // The identifier of the machine
    } MachineInfo_t;