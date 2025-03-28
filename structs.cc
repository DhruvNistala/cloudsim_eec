// this file is only for referencing the structs

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