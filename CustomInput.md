# Dhruv Nistala, Matthew Little
# Covers load spikes, system constraints, and heterogeneous machines
# Tests intense spikes (Task 5), moderate spikes (Tasks 6, 7, 9, 10), sparse loads (Tasks 8, 11), and baseline (Tasks 1-4) across 8 diverse machine classes with GPU/no-GPU options

machine class:
{
# machine class 1 Powerful x86 processors with GPUs
Number of machines: 8       # 8 high-end X86 servers in the cluster
CPU type: X86               # Compatible with LINUX, LINUX_RT, WIN VMs
Number of cores: 8          # 8 cores per machine, good for parallel tasks
Memory: 16384               # 16GB RAM, supports memory-intensive tasks
S-States: [120, 100, 100, 80, 40, 10, 0]  # Power (Watts): S0 (full) to S5 (off), high base power
P-States: [12, 8, 6, 4]     # Power/core (Watts): P0 (max) to P3 (min), scales performance
C-States: [12, 3, 1, 0]     # Idle power/core (Watts): C0 (active) to C4 (deep sleep)
MIPS: [1000, 800, 600, 400] # Performance: 1000 MIPS at P0, 400 at P3
GPUs: yes                   # GPU-enabled, boosts tasks like CRYPTO, HPC, AI
}
machine class:
{
# machine class 2 Less powerful x86 processors
Number of machines: 8       # 8 lower-end X86 servers
CPU type: X86               # Same VM compatibility as Class 1
Number of cores: 4          # Fewer cores, less parallelism
Memory: 8192                # 8GB RAM, half of Class 1, limits task capacity
S-States: [80, 60, 50, 40, 25, 10, 0]  # Lower base power than Class 1, energy-efficient
P-States: [8, 6, 4, 3]      # Lower power/core than Class 1, less dynamic range
C-States: [8, 2, 1, 0]      # Idle power lower, matches lower performance
MIPS: [500, 400, 300, 200]  # Half the performance of Class 1, slower tasks
GPUs: no                    
}
machine class:
{
# machine class 3 Powerful ARM processors
Number of machines: 8       # 8 high-end ARM servers
CPU type: ARM               # Supports LINUX, LINUX_RT, WIN VMs
Number of cores: 8          
Memory: 16384               
S-States: [120, 100, 100, 80, 40, 10, 0]  
P-States: [12, 8, 6, 4]     
C-States: [12, 3, 1, 0]     
MIPS: [1000, 800, 600, 400] 
GPUs: yes                  
}
machine class:
{
# machine class 4 Less powerful ARM processors
Number of machines: 8       # 8 lower-end ARM servers
CPU type: ARM               
Number of cores: 4          
Memory: 8192                
S-States: [80, 60, 50, 40, 25, 10, 0]  
P-States: [8, 6, 4, 3]      
C-States: [8, 2, 1, 0]      
MIPS: [500, 400, 300, 200] 
GPUs: no                    
}
machine class:
{
# machine class 5 Powerful RISCV processors
Number of machines: 8       # 8 high-end RISCV servers
CPU type: RISCV             
Number of cores: 8          
Memory: 16384               
S-States: [120, 100, 100, 80, 40, 10, 0]  
P-States: [12, 8, 6, 4]     
C-States: [12, 3, 1, 0]     
MIPS: [1000, 800, 600, 400] 
GPUs: yes                   
}
machine class:
{
# machine class 6 Less powerful RISCV processors
Number of machines: 8       # 8 lower-end RISCV servers
CPU type: RISCV             
Number of cores: 4          
Memory: 8192                
S-States: [80, 60, 50, 40, 25, 10, 0]  
P-States: [8, 6, 4, 3]      
C-States: [8, 2, 1, 0]      
MIPS: [500, 400, 300, 200]  
GPUs: no                   
}
machine class:
{
# machine class 7 Powerful POWER processors
Number of machines: 8       # 8 high-end POWER servers
CPU type: POWER             
Number of cores: 8          
Memory: 16384               
S-States: [120, 100, 100, 80, 40, 10, 0]  
P-States: [12, 8, 6, 4]     
C-States: [12, 3, 1, 0]     
MIPS: [1000, 800, 600, 400] 
GPUs: yes                   
}
machine class:
{
# machine class 8 Less powerful POWER processors
Number of machines: 8       # 8 lower-end POWER servers
CPU type: POWER             
Number of cores: 4          
Memory: 8192                
S-States: [80, 60, 50, 40, 25, 10, 0] 
P-States: [8, 6, 4, 3]     
C-States: [8, 2, 1, 0]     
MIPS: [500, 400, 300, 200]  
GPUs: no                   
}
task class:
{
# task 1 baseline AIX
Start time: 0             # Begins at 0ms
End time: 400000          # Ends at 400ms
Inter arrival: 8000       # ~50 tasks
Expected runtime: 1000000 # 1s at 1000 MIPS
Memory: 4
VM type: AIX
GPU enabled: no
SLA type: SLA0            # 95% within 1.2s
CPU type: POWER
Task type: WEB
Seed: 726775
}
task class:
{
# task 2 baseline Linux
Start time: 0             # Starts at 0ms
End time: 400000          # Ends at 400ms
Inter arrival: 8000       # ~50 tasks
Expected runtime: 1000000 # 1s at 1000 MIPS
Memory: 4
VM type: LINUX
GPU enabled: no
SLA type: SLA0            # 95% within 1.2s
CPU type: X86
Task type: WEB            # Short, frequent requests
Seed: 726775
}
task class:
{
# task 3 baseline Win
Start time: 0             # Starts at 0ms
End time: 400000          # Ends at 400ms
Inter arrival: 8000       # ~50 tasks
Expected runtime: 1000000 # 1s at 1000 MIPS
Memory: 4
VM type: WIN
GPU enabled: no
SLA type: SLA0            # 95% within 1.2s
CPU type: ARM
Task type: WEB
Seed: 726775
}
task class:
{
# task 4 baseline Linux_RT
Start time: 0             # Starts at 0ms
End time: 400000          # Ends at 400ms
Inter arrival: 8000       # ~50 tasks
Expected runtime: 1000000 # 1s at 1000 MIPS
Memory: 4
VM type: LINUX_RT
GPU enabled: no
SLA type: SLA0            # 95% within 1.2s
CPU type: RISCV
Task type: WEB
Seed: 726775
}
task class:
{
# task 5 temporary spike in load intensity (crypto)
Start time: 300000        # Starts at 300ms
End time: 310000          # Ends at 310ms
Inter arrival: 200        # ~50 tasks
Expected runtime: 10000000 # 10s at 1000 MIPS
Memory: 8
VM type: WIN
GPU enabled: yes
SLA type: SLA2            # 80% within 20s
CPU type: ARM
Task type: CRYPTO
Seed: 726775
}
task class:
{
# task 6 Less extreme spike in load intensity
Start time: 350000        # Starts at 350ms
End time: 400000          # Ends at 400ms
Inter arrival: 5000       # ~10 tasks
Expected runtime: 30000000 # 30s at 1000 MIPS
Memory: 8
VM type: AIX
GPU enabled: yes
SLA type: SLA0            # 95% within 36s
CPU type: POWER
Task type: HPC
Seed: 726775
}
task class:
{
# task 7 less extreme spike in load intensity of non-GPU enabled tasks
Start time: 600000        # Starts at 600ms
End time: 650000          # Ends at 650ms
Inter arrival: 5000       # ~10 tasks
Expected runtime: 3000000 # 3s at 1000 MIPS
Memory: 8
VM type: LINUX
GPU enabled: no
SLA type: SLA0            # 95% within 3.6s
CPU type: x86
Task type: STREAM
Seed: 726775
}
task class:
{
# task 8 sparse consecutive low-intensity tasks to test energy conservation in low workload
Start time: 3000000       # Starts at 3s
End time: 4000000         # Ends at 4s
Inter arrival: 100000     # ~10 tasks
Expected runtime: 100000  # 100ms at 1000 MIPS
Memory: 4
VM type: LINUX
GPU enabled: yes
SLA type: SLA2            # 80% within 200ms
CPU type: RISCV
Task type: WEB
Seed: 726775
}
task class:
{
# task 9 HPC spike
Start time: 600000        # Starts at 600ms
End time: 650000          # Ends at 650ms
Inter arrival: 2000       # ~25 tasks
Expected runtime: 50000   # 50ms at 1000 MIPS
Memory: 4096
VM type: AIX
GPU enabled: yes
SLA type: SLA0            # 95% within 60ms
CPU type: POWER
Task type: HPC
Seed: 726775
}
task class:
{
# task 10 STREAM spike
Start time: 900000        # Starts at 900ms
End time: 950000          # Ends at 950ms
Inter arrival: 2000       # ~25 tasks
Expected runtime: 50000   # 50ms at 1000 MIPS
Memory: 1024
VM type: LINUX
GPU enabled: no
SLA type: SLA0            # 95% within 60ms
CPU type: X86
Task type: STREAM
Seed: 726775
}
task class:
{
# task 11 sparse low intensity web
Start time: 1000000       # Starts at 1s
End time: 1500000         # Ends at 1.5s
Inter arrival: 50000      # ~10 tasks
Expected runtime: 100000  # 100ms at 1000 MIPS
Memory: 512
VM type: LINUX
GPU enabled: yes
SLA type: SLA2            # 80% within 200ms
CPU type: RISCV
Task type: WEB
Seed: 726775
}
task class:
{
# task 12 low-intensity with SLA3 for energy testing
Start time: 2000000       # Starts at 2s
End time: 2500000         # Ends at 2.5s
Inter arrival: 50000      # ~10 tasks
Expected runtime: 100000  # 100ms at 1000 MIPS
Memory: 256               
VM type: LINUX           
GPU enabled: no           
SLA type: SLA3            # Best effort, tests energy saving
CPU type: ARM             
Task type: STREAM
Seed: 726775
}
task class:
{
# task 13 quick AI burst to test GPUs 
Start time: 500000        # Starts at 500ms, between spikes
End time: 510000          # Ends at 510ms
Inter arrival: 1000       # ~10 tasks
Expected runtime: 20000   # 20ms at 1000 MIPS
Memory: 2048              
VM type: LINUX           
GPU enabled: yes
SLA type: SLA1            # 90% in 30ms
CPU type: X86
Task type: AI
Seed: 726775
}

