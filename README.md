*This project has been created as part of the 42 curriculum by mariaalm.*

# Codexion

## Description

Codexion is a concurrency simulation, modeled after the classic "dining philosophers" problem. A configurable number of *coders* sit in a circular co-working hub, sharing a set of *USB dongles* (one dongle placed between each pair of neighbors). Compiling quantum code requires two dongles held simultaneously (the coder's left and right dongle), so coders must compete for a scarce, shared resource without ever deadlocking, duplicating a dongle, or starving one another out.

Each coder is represented by its own thread and cycles through three phases — **compile**, **debug**, **refactor** — for as long as the simulation runs. A dedicated **scheduler thread** arbitrates access to the dongles according to a configurable policy (`fifo` or `edf`), and a dedicated **monitor thread** watches every coder's clock and stops the simulation the moment a coder fails to start compiling again within `time_to_burnout` milliseconds — or once every coder has completed `number_of_compiles_required` compiles.

The goal of the project is to practice real multithreaded programming in C using POSIX threads, mutexes and condition variables: correct synchronization, deadlock avoidance, fair scheduling, and precise, non-interleaved logging under heavy contention.

## Instructions

### Compilation

```bash
make
```

This builds the `codexion` binary at the root of the repository, compiling with `-Wall -Wextra -Werror -pthread` and no warnings.

Other available rules:

```bash
make clean    # remove object files
make fclean   # remove object files and the binary
make re       # fclean + all
```

### Running

```bash
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders (and dongles) in the simulation |
| `time_to_burnout` | Max delay (ms) between the start of one compile and the next before a coder burns out |
| `time_to_compile` | Duration (ms) of the compile phase |
| `time_to_debug` | Duration (ms) of the debug phase |
| `time_to_refactor` | Duration (ms) of the refactor phase |
| `number_of_compiles_required` | Number of compiles each coder must complete for the simulation to stop successfully |
| `dongle_cooldown` | Time (ms) a dongle stays unavailable after being released |
| `scheduler` | Arbitration policy: `fifo` or `edf` |

Example:

```bash
./codexion 5 2000 200 200 200 10 0 fifo
```

Every state change is logged as `timestamp_in_ms coder_id message`, e.g.:

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
```

## Resources

- POSIX Threads Programming (LLNL): https://hpc-tutorials.llnl.gov/posix/
- `man` pages for `pthread_create`, `pthread_mutex_lock`, `pthread_cond_wait`, `pthread_cond_timedwait`, `clock_gettime`, `gettimeofday`
- The Little Book of Semaphores, Allen B. Downey — background reading on the dining philosophers problem and classic synchronization patterns
- Wikipedia: Dining philosophers problem, Priority queue (binary heap) — for the min-heap implementation used by the scheduler

### AI usage

Claude (Anthropic) was used throughout this project as a learning and debugging aid, always with the student writing and typing every line of code herself. AI was used for:

- **Explaining concepts**: clarifying how condition variables, mutexes, binary heaps (array representation, sift-up/sift-down), and POSIX thread synchronization primitives work, before implementing them.
- **Code review**: after each function was written by the student, it was reviewed line-by-line for syntax errors, logic bugs, and Norm compliance (function length, line count), with explanations of *why* something was wrong rather than a corrected version being provided outright.
- **Debugging real concurrency bugs**: several real bugs were found and diagnosed this way over the course of the project, including:
  - a `compile_authorized` flag never being reset between compile cycles, causing out-of-order logs;
  - a copy-paste bug where the right dongle's cooldown check accidentally read the left dongle's timestamp, letting a dongle be re-acquired before its cooldown expired;
  - `pthread_cond_wait` used without a timeout, causing "lost wakeups" where the scheduler thread would sleep through a dongle's cooldown expiring;
  - the dispatch loop scanning the heap's raw array instead of always extracting the true minimum, breaking FIFO/EDF ordering guarantees;
  - a special case where `number_of_coders == 1` (only one physical dongle) was not detected, letting a lone coder "compile" with only one dongle instead of burning out;
  - unprotected shared fields (`simulation_over`, `last_compile_start`, `compiles_finish`) being read/written across threads without a mutex — data races found by manual code review and later confirmed clean under `valgrind --tool=helgrind`;
  - a starvation issue under heavy dongle contention, where two coders with no direct resource conflict could indefinitely "out-race" a third coder for shared dongles; resolved by adding an aging mechanism (`fail_count`) so a coder repeatedly denied a dongle gains scheduling priority.
- **Test design**: suggesting test scenarios (single coder, high contention, infeasible burnout, cooldown edge cases) and helper scripts used to sanity-check the logs (e.g. verifying every "is compiling" line is preceded by exactly two "has taken a dongle" lines for the same coder).

No code was generated wholesale by AI and pasted into the project; every function was written and typed by the student, then reviewed and corrected with AI assistance until the logic and syntax were understood and verified by the student herself.

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions)**: a coder never holds one dongle while waiting for the other. All dongle acquisition decisions are made atomically by a single scheduler thread, under one mutex (`scheduler_mutex`): a coder is granted *both* of its dongles at once, or neither. There is never a partial-acquisition state, so the classic "hold and wait" circular deadlock of the dining philosophers problem cannot occur.
- **Starvation prevention**: the scheduler processes pending requests strictly in priority order (via a hand-written binary min-heap), using a sequence counter as a deterministic tie-breaker when two requests have the same priority (avoiding a bias toward a fixed coder id). Under heavy contention, a coder that is repeatedly denied a dongle accumulates a `fail_count`, which is used as the highest-priority sort key on the next scheduling pass — guaranteeing that a coder waiting the longest cannot be indefinitely skipped by newer or unrelated requests.
- **Dongle cooldown handling**: a dongle records the timestamp at which it was released; it is only considered available once `now >= release_time + dongle_cooldown`. This is checked atomically alongside dongle availability when the scheduler considers granting it.
- **Precise burnout detection**: a dedicated monitor thread polls every coder's `last_compile_start` at a short, fixed interval (1 ms `usleep`), so burnout is detected and logged within a few milliseconds of the real deadline (well inside the 10 ms tolerance).
- **Log serialization**: every log line is written while holding a single dedicated `write_mutex`, so two threads can never interleave partial lines on `stdout`.
- **Clean shutdown**: when the simulation ends (burnout or success), the monitor thread wakes the scheduler thread and every individual coder thread that might be sleeping on a condition variable, so no thread is left blocked forever and every `pthread_join` in `main` eventually returns.

## Thread synchronization mechanisms

The project uses only local synchronization primitives passed by pointer through the `t_table`/`t_coder`/`t_dongle` structures — there are no global variables anywhere in the codebase.

- **`write_mutex`** (one, in `t_table`): protects every call to the logging function, so no two threads can print at the same time.
- **`scheduler_mutex`** (one, in `t_table`): protects the scheduler's pending-request heap and the shared state of every dongle (`in_use`, cooldown timestamp). All dongle acquisition/release logic runs under this single lock, which is what makes "grant both dongles or none" atomic and deadlock-free.
- **`scheduler_cond`** (one, in `t_table`): the scheduler thread sleeps on this condition variable (with `pthread_cond_timedwait`, not a plain `pthread_cond_wait`, so it periodically re-checks even if a wakeup was missed) until a coder pushes a new request or releases its dongles.
- **`mutex_compile` / `cond_compile`** (one pair per coder, in `t_coder`): protect that coder's own `compile_authorized` flag, `last_compile_start`, and `compiles_finish` counter. A coder blocks on its own `cond_compile` while waiting to be authorized to compile; the scheduler signals it directly (not a broadcast) once its two dongles have been granted.
- **`simulation_mutex` / `simulation_cond`** (one pair, in `t_table`): protect the `simulation_over` flag, which is written once by the monitor thread and read by every other thread to decide when to stop.

Race conditions are avoided by never reading or writing a field shared across threads outside of its owning mutex — for example, a coder's `last_compile_start` is written by that coder's own thread inside `mutex_compile` and read by the monitor thread inside the same mutex, rather than being accessed directly. Thread-safe communication between coders and the monitor is achieved entirely through this pattern: shared state is only ever touched while its corresponding mutex is held, and condition variables are used to avoid busy-waiting while still guaranteeing prompt wakeups (via `pthread_cond_signal`/`pthread_cond_broadcast` paired with `pthread_cond_wait`/`pthread_cond_timedwait`). The absence of data races and deadlocks was additionally verified with `valgrind` (`memcheck` and `helgrind`) across multiple runs.