# Dhruv Nistala, Matthew Little
# Covers load spikes, system constraints, and heterogeneous machines
# Tests intense spikes (Task 5), moderate spikes (Tasks 6, 7, 9, 10), sparse loads (Tasks 8, 11), and baseline (Tasks 1-4) across 8 diverse machine classes with GPU/no-GPU options

machine class:
{
# machine class 1 Powerful x86 processors with GPUs
Number of machines: 8
CPU type: X86
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
# machine class 2 Less powerful x86 processors
Number of machines: 8
CPU type: X86
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
# machine class 3 Powerful ARM processors
Number of machines: 8
CPU type: ARM
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
Number of machines: 8
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
Number of machines: 8
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
Number of machines: 8
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
Number of machines: 8
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
Number of machines: 8
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
Start time: 0
End time: 400000
Inter arrival: 8000
Expected runtime: 1000000
Memory: 4
VM type: AIX
GPU enabled: no
SLA type: SLA0
CPU type: POWER
Task type: WEB
Seed: 726775
}
task class:
{
Start time: 0
End time: 400000
Inter arrival: 8000
Expected runtime: 1000000
Memory: 4
VM type: LINUX
GPU enabled: no
SLA type: SLA0
CPU type: X86
Task type: WEB
Seed: 726775
}
task class:
{
Start time: 0
End time: 400000
Inter arrival: 8000
Expected runtime: 1000000
Memory: 4
VM type: WIN
GPU enabled: no
SLA type: SLA0
CPU type: ARM
Task type: WEB
Seed: 726775
}
task class:
{
Start time: 0
End time: 400000
Inter arrival: 8000
Expected runtime: 1000000
Memory: 4
VM type: LINUX_RT
GPU enabled: no
SLA type: SLA0
CPU type: RISCV
Task type: WEB
Seed: 726775
}
task class:
{
Start time: 300000
End time: 310000
Inter arrival: 200
Expected runtime: 10000000
Memory: 8
VM type: WIN
GPU enabled: yes
SLA type: SLA2
CPU type: ARM
Task type: CRYPTO
Seed: 726775
}
task class:
{
Start time: 350000
End time: 400000
Inter arrival: 5000
Expected runtime: 30000000
Memory: 8
VM type: AIX
GPU enabled: yes
SLA type: SLA0
CPU type: POWER
Task type: HPC
Seed: 726775
}
task class:
{
Start time: 600000
End time: 650000
Inter arrival: 5000
Expected runtime: 3000000
Memory: 8
VM type: LINUX
GPU enabled: no
SLA type: SLA0
CPU type: X86
Task type: STREAM
Seed: 726775
}
task class:
{
Start time: 3000000
End time: 4000000
Inter arrival: 100000
Expected runtime: 100000
Memory: 4
VM type: LINUX
GPU enabled: yes
SLA type: SLA2
CPU type: RISCV
Task type: WEB
Seed: 726775
}
task class:
{
Start time: 600000
End time: 650000
Inter arrival: 2000
Expected runtime: 50000
Memory: 4096
VM type: AIX
GPU enabled: yes
SLA type: SLA0
CPU type: POWER
Task type: HPC
Seed: 726775
}
task class:
{
Start time: 900000
End time: 950000
Inter arrival: 2000
Expected runtime: 50000
Memory: 1024
VM type: LINUX
GPU enabled: no
SLA type: SLA0
CPU type: X86
Task type: STREAM
Seed: 726775
}
task class:
{
Start time: 1000000
End time: 1500000
Inter arrival: 50000
Expected runtime: 100000
Memory: 512
VM type: LINUX
GPU enabled: yes
SLA type: SLA2
CPU type: RISCV
Task type: WEB
Seed: 726775
}
task class:
{
Start time: 2000000
End time: 2500000
Inter arrival: 50000
Expected runtime: 100000
Memory: 256
VM type: LINUX
GPU enabled: no
SLA type: SLA3
CPU type: ARM
Task type: STREAM
Seed: 726775
}
task class:
{
Start time: 500000
End time: 510000
Inter arrival: 1000
Expected runtime: 20000
Memory: 2048
VM type: LINUX
GPU enabled: yes
SLA type: SLA1
CPU type: X86
Task type: AI
Seed: 726775
}

