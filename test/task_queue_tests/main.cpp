#include "../../lib/include/cpp_vk_lib/processing/task_queue.hpp"

#include <gtest/gtest.h>

namespace proc = vk::processing;

TEST(task_queue, future_tasks)
{
    proc::task_queue task_queue(2 /* num_workers */);

    std::vector<std::pair<bool, std::future<size_t>>> tasks;

    auto start_time = std::chrono::system_clock::now();

    for (size_t i = 0; i < 10; i++)
    {
        tasks.emplace_back(task_queue.push_future_task([_i = i] {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(100ms);
            return _i;
        }));
    }

    task_queue.start();
    task_queue.wait_for_completion();

    for (size_t i = 0; i < 10; i++)
    {
        ASSERT_EQ(tasks[i].second.get(), i);
    }

    auto finish_time = std::chrono::system_clock::now();

    ASSERT_LT(finish_time - start_time, std::chrono::milliseconds(510));
}

TEST(task_queue, void_tasks)
{
    proc::task_queue task_queue(2 /* num_workers */);

    auto start_time = std::chrono::system_clock::now();

    std::vector<bool> successfully_completed_tasks;

    for (size_t i = 0; i < 10; i++)
    {
        successfully_completed_tasks.push_back(task_queue.push_void_task([] {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(100ms);
        }));
    }

    task_queue.start();
    task_queue.wait_for_completion();

    for (bool task_flag : successfully_completed_tasks)
    {
        ASSERT_TRUE(task_flag);
    }

    auto finish_time = std::chrono::system_clock::now();

    ASSERT_LT(finish_time - start_time, std::chrono::milliseconds(510));
}


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
