#ifndef LOCK_FREE_THREAD_POOL_HPP
#define LOCK_FREE_THREAD_POOL_HPP

#include <atomic>
#include <thread>
#include <functional>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cstddef>
#include <cassert>

// BoundedMPMCQueue  (Dmitry Vyukov's lock-free MPMC ring buffer)
//  This code logic is not mine , I used it for our use case of SPMC
// reference : 
// 1) https://sites.google.com/site/1024cores/home/lock-free-algorithms/queues/bounded-mpmc-queue
// 2) https://github.com/couchbase/phosphor/tree/master/thirdparty/dvyukov/include/dvyukov
// 3) https://github.com/erez-strauss/lockfree_mpmc_queue

/*
Earlier I had decided to go with simple thread pool with locks but since I had some time in my hand so I decide to go ahead with this
our system does not need this complexity but I just tried to expand out for our usecase
*/

// I did own modification but the core logic is not mine
template<typename T, std::size_t Cap>
class BoundedMPMCQueue {
    static_assert((Cap& (Cap - 1)) == 0, "Cap must be a power of 2");
    static constexpr std::size_t MASK = Cap - 1;

    // this is to prevent false sharing 
    struct alignas(64) Cell {
        std::atomic<std::size_t> sequence{ 0 };
        T                        data{};
    };

    alignas(64) Cell                     m_buffer[Cap];
    alignas(64) std::atomic<std::size_t> m_enqPos{ 0 };
    alignas(64) std::atomic<std::size_t> m_deqPos{ 0 };

public:
    BoundedMPMCQueue() noexcept {
        for (std::size_t i = 0; i < Cap; ++i)
            m_buffer[i].sequence.store(i, std::memory_order_relaxed);
    }

    BoundedMPMCQueue(const BoundedMPMCQueue&) = delete;
    BoundedMPMCQueue& operator=(const BoundedMPMCQueue&) = delete;

   // producer
    bool try_push(T val) noexcept {
        std::size_t pos = m_enqPos.load(std::memory_order_relaxed);
        Cell* cell;
        for (;;) {
            cell = &m_buffer[pos & MASK];
            const std::size_t seq = cell->sequence.load(std::memory_order_acquire);
            const std::ptrdiff_t diff =
                static_cast<std::ptrdiff_t>(seq) - static_cast<std::ptrdiff_t>(pos);

            if (diff == 0) {
                if (m_enqPos.compare_exchange_weak(
                    pos, pos + 1, std::memory_order_relaxed))
                    break;
            }
            else if (diff < 0) {
                return false; 
            }
            else {
                pos = m_enqPos.load(std::memory_order_relaxed);
            }
        }
        cell->data = std::move(val);
        cell->sequence.store(pos + 1, std::memory_order_release);
        return true;
    }
    
    // consumer pop
    bool try_pop(T& val) noexcept {
        std::size_t pos = m_deqPos.load(std::memory_order_relaxed);
        Cell* cell;
        for (;;) {
            cell = &m_buffer[pos & MASK];
            const std::size_t seq = cell->sequence.load(std::memory_order_acquire);
            const std::ptrdiff_t diff =
                static_cast<std::ptrdiff_t>(seq)
                - static_cast<std::ptrdiff_t>(pos + 1);

            if (diff == 0) {
                if (m_deqPos.compare_exchange_weak(
                    pos, pos + 1, std::memory_order_relaxed))
                    break;
            }
            else if (diff < 0) {
                return false; // Queue empty
            }
            else {
                pos = m_deqPos.load(std::memory_order_relaxed);
            }
        }
        val = std::move(cell->data);
        cell->sequence.store(pos + Cap, std::memory_order_release);
        return true;
    }

    bool empty() const noexcept {
        return m_enqPos.load(std::memory_order_relaxed)
            == m_deqPos.load(std::memory_order_relaxed);
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// LockFreeThreadPool
// this part of the code is also not mine, I just used and changed it for fucntion which was needed for our case
//  I do not templatetize it as it will make it even more complex to work with

class LockFreeThreadPool {
    using Task = std::function<void()>;
    static constexpr std::size_t QUEUE_CAP = 256;

public:
    explicit LockFreeThreadPool(int numThreads)
        : m_numWorkers(numThreads)
    {
        assert(numThreads > 0 && numThreads <= 64);
        m_workers.reserve(numThreads);
        for (int i = 0; i < numThreads; ++i)
            m_workers.emplace_back([this] { workerLoop(); });
    }

    ~LockFreeThreadPool() {
        // descorory waits for all
        m_running.store(false, std::memory_order_release);
        m_wakeCV.notify_all();
        for (auto& w : m_workers) {
            if (w.joinable()) w.join();
        }
    }

    LockFreeThreadPool(const LockFreeThreadPool&) = delete;
    LockFreeThreadPool& operator=(const LockFreeThreadPool&) = delete;
    
    // sumbit task
    void submit(Task task) {
        m_pendingCount.fetch_add(1, std::memory_order_relaxed);
        while (!m_queue.try_push(std::move(task))) {
            std::this_thread::yield(); // queue full — yield, not spin-burn
        }
        // Wake exactly one sleeping worker
        m_wakeCV.notify_one();
    }

    // waitAll: blocking so all can execture.
    void waitAll() {
        std::unique_lock<std::mutex> lock(m_doneMutex);
        m_doneCV.wait(lock, [this] {
            return m_pendingCount.load(std::memory_order_acquire) == 0;
            });
    }

    int workerCount() const noexcept { return m_numWorkers; }

private:
    void workerLoop() {
        while (m_running.load(std::memory_order_relaxed)) {
            Task task;

            if (m_queue.try_pop(task)) {
              // no locks in hot path
                task();
                const int remaining =
                    m_pendingCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
                if (remaining == 0) {
                    std::lock_guard<std::mutex> lock(m_doneMutex);
                    m_doneCV.notify_all();
                }
            }
            else {
               // cold path so this has mutex 
                std::unique_lock<std::mutex> lock(m_wakeMutex);
                m_wakeCV.wait(lock, [this] {
                    return !m_running.load(std::memory_order_relaxed)
                        || !m_queue.empty();
                    });
            }
        }
    }

    int                              m_numWorkers;
    BoundedMPMCQueue<Task, QUEUE_CAP> m_queue;
    std::vector<std::thread>         m_workers;

    std::atomic<bool>                m_running{ true };
    std::atomic<int>                 m_pendingCount{ 0 };

    // in cold path varaibles
    std::mutex               m_wakeMutex;
    std::condition_variable  m_wakeCV;

    // for waitall 
    std::mutex               m_doneMutex;
    std::condition_variable  m_doneCV;
};

#endif // LOCK_FREE_THREAD_POOL_HPP