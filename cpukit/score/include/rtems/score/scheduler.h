/**
 *  @file  rtems/score/scheduler.h
 *
 *  This include file contains all the constants and structures associated
 *  with the scheduler.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_SCHEDULER_H
#define _RTEMS_SCORE_SCHEDULER_H

#include <rtems/score/percpu.h>
#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/prioritybitmap.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreScheduler Scheduler Handler
 *
 *  This handler encapsulates functionality related to managing sets of threads
 *  that are ready for execution.
 */
/**@{*/

/*
 * These defines are used to set the scheduler_policy value. The values
 * must correspond directly with the order of the fields in the scheduler
 * table (Scheduler_Table_t), because the Configuration.scheduler_policy
 * field is used to index the scheduler table.
 */
#define _Scheduler_USER     (0)
#define _Scheduler_PRIORITY (1)

typedef struct Scheduler_Control_struct Scheduler_Control;

/*
 * The Scheduler_Table_t type defines the scheduler initialization table,
 * which is set up by confdefs.h based on the user's choice of scheduler
 * policy.
 */
typedef struct {
  void ( *scheduler_init )( Scheduler_Control * );
} Scheduler_Table_t;

/* instantiated and initialized in confdefs.h */
extern const Scheduler_Table_t          _Scheduler_Table[];

/**
 * The following Scheduler_Per_thread_xxx structures are used to
 * hold per-thread data used by the scheduler.  Thread_Control->scheduler is a
 * union of pointers, one for each of the following structures.  The
 * scheduler->xxx field points to an instantion of one of these structures,
 * which is allocated from the workspace during _Thread_Start.
 */

/**
 * Per-thread data related to the _Scheduler_PRIORITY scheduling policy.
 */
typedef struct {
  /** This field points to the Ready FIFO for this thread's priority. */
  Chain_Control                        *ready_chain;

  /** This field contains precalculated priority map indices. */
  Priority_bit_map_Information          Priority_map;
} Scheduler_priority_Per_thread;

/**
 * function jump table that holds pointers to the functions that
 * implement specific schedulers.
 */
typedef struct {
  /** Implements the scheduling decision logic (policy). */
  void ( *schedule ) ( Scheduler_Control * );

  /** Voluntarily yields the processor per the scheduling policy. */
  void ( *yield ) ( Scheduler_Control * );

  /** Removes the given thread from scheduling decisions. */
  void ( *block ) ( Scheduler_Control *, Thread_Control * );

  /** Adds the given thread to scheduling decisions. */
  void ( *unblock ) ( Scheduler_Control *, Thread_Control * );

  /** allocates the scheduler field of the given thread */
  void * ( *scheduler_allocate ) ( Scheduler_Control *, Thread_Control * );

  /** frees the scheduler field of the given thread */
  void ( *scheduler_free ) ( Scheduler_Control *, Thread_Control * );

  /** updates the scheduler field of the given thread -- primarily used
   * when changing the thread's priority. */
  void ( *scheduler_update ) ( Scheduler_Control *, Thread_Control * );
} Scheduler_Operations;

/**
 * This is the structure used to manage the scheduler.
 */
struct Scheduler_Control_struct {
  /**
   *  This union contains the pointer to the data structure used to manage
   *  the ready set of tasks. The pointer varies based upon the type of
   *  ready queue required by the scheduler.
   */
  union {
    /**
     * This is the set of lists (an array of Chain_Control) for
     * priority scheduling.
     */
    Chain_Control            *Priority;

  } ready_queues;

  /** The jump table for scheduler-specific functions */
  Scheduler_Operations                  operations;
};

/**
 *  The _Scheduler holds the structures used to manage the
 *  scheduler.
 *
 * @note Can we make this per-cpu? then _Scheduler will be a macro.
 */
SCORE_EXTERN Scheduler_Control          _Scheduler;

/**
 *  This routine initializes the scheduler to the policy chosen by the user
 *  through confdefs, or to the priority scheduler with ready chains by
 *  default.
 */
void _Scheduler_Handler_initialization( void );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/scheduler.inl>
#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */