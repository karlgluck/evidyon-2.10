#ifndef __DCXSCHEDULER_H__
#define __DCXSCHEDULER_H__



namespace dcx {

template <typename TIME_TYPE,
          const size_t NUMBER_OF_TASKS> class dcxSchedulerB {
public:
  typedef TIME_TYPE time_t;

public:

  dcxSchedulerB(time_t current_time, time_t period) : period_(period) {
    next_task_ = 0;
    time_t offset = (period_ / NUMBER_OF_TASKS) / 2;
    for (unsigned int i = 0; i < NUMBER_OF_TASKS; ++i) {
      last_task_times_[i] = current_time + offset + (i * period_) / NUMBER_OF_TASKS;
    }
  }

public:

  // Returns the next task to be executed, if it is necessary to do so.
  unsigned int poll(time_t time,
                    time_t* time_since_last_executed) {
    unsigned int task_index = next_task_;
    time_t delay = time - last_task_times_[task_index];
    if (delay < period_) return unsigned int(-1);
    last_task_times_[task_index] = time;
    *time_since_last_executed = delay;
    next_task_ = (task_index + 1) % NUMBER_OF_TASKS;
    return task_index;
  }

private:
  time_t last_task_times_[NUMBER_OF_TASKS];
  unsigned int next_task_;
  const time_t period_;
};



// Tasks in a scheduler group are always performed sequentially--no task
// will be executed more than once more than any other task.
template <typename TIME_TYPE> class dcxSchedulerGroup {
public:
  typedef TIME_TYPE time_t;

public:
  void configure(time_t period, unsigned int number_of_tasks) {
    period_ = period;
    task_ids_ = new int[number_of_tasks];
    last_task_times_ = new time_t[number_of_tasks];
    number_of_tasks_ = number_of_tasks;
    next_task_ = 0;
  }

  void assignTaskID(unsigned int index, int task_id) {
    task_ids_[index] = task_id;
    last_task_times_[index] = time_t(0);
  }

  void assignDefaultTaskTimes(time_t current_time) {
    time_t offset = period_ / number_of_tasks_ / 2;
    for (unsigned int i = 0; i < number_of_tasks_; ++i) {
      last_task_times_[i] = current_time + offset + (i * period_) / number_of_tasks_;
    }
  }

public:

  // Returns the next task to be executed, if it is necessary to do so.
  bool nextTask(time_t time,
                int* task_id,
                time_t* time_since_last_executed) {
    unsigned int task_index = next_task_;
    time_t delay = time - last_task_times_[task_index];
    if (delay < period_) return false;
    last_task_times_[task_index] = time;
    *task_id = task_ids_[task_index];
    *time_since_last_executed = delay;
    next_task_ = (task_index + 1) % number_of_tasks_;
    return true;
  }

private:
  time_t period_;
  int* task_ids_;
  time_t* last_task_times_;
  unsigned int number_of_tasks_;
  unsigned int next_task_;
};
#if 0
// A scheduler has the resolution of however often it is polled.
// To save processing time by creating low-resolution schedulers (ex. for
// tasks that only execute once every few minutes, instead of every
// few milliseconds) try creating scheduler hierarchies--i.e. schedule a
// scheduler!
template <typename TIME_TYPE> class dcxScheduler {
public:
  typedef TIME_TYPE time_t;

public:
  typedef dcxSchedulerGroup<TIME_TYPE> Group;

public:
  dcxScheduler() {
    default_task_id_ = 0;
    groups_ = 0;
    last_group_ = 0;
    number_of_groups_ = 0;
  }

  void create(unsigned int number_of_groups, unsigned int default_group, int default_task_id) {
    groups_ = new Group[number_of_groups];
    number_of_groups_ = number_of_groups;
    default_group_ = default_group;
    default_task_id_ = default_task_id;
    last_group_ = 0;
  }

  void destroy() {
    if (groups_ != NULL) {
      delete[] groups_;
      groups_ = NULL;
    }

    number_of_groups_ = 0;
    last_group_ = 0;
  }


  void configureGroup(unsigned int group, time_t period, unsigned int number_of_tasks) {
    groups_[group].configure(period, number_of_tasks);
  }


  void assignTaskID(unsigned int group, unsigned int index, int task_id) {
    groups_[group].assignTaskID(index, task_id);
  }


  void assignDefaultTaskTimes(time_t time) {
    for (unsigned int i = 0; i < number_of_groups_; ++i) {
      groups_[i].assignDefaultTaskTimes(time);
    }
  }


  // If the returned task group is negative, it means that the returned task 
  int nextTask(time_t time,
              unsigned int* task_group,
              time_t* time_since_last_executed) {
    unsigned int group = last_group_;
    int task_id;

    do {
      group = (group + 1) % number_of_groups_;
      if (groups_[group].nextTask(time, &task_id, time_since_last_executed)) {
        last_group_ = group;
        *task_group = group;
        return task_id;
      }
    } while (group != last_group_); // if we get back to the first group, we're done...

    *task_group = default_group_;
    *time_since_last_executed = 0;

    return task_id;
  }

private:
  unsigned int default_group_;
  int default_task_id_;
  Group* groups_;
  unsigned int last_group_;
  unsigned int number_of_groups_;
};

#endif

}

#endif
