/*
 * File: condvar3_3.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *
 *      Based upon Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@callisto.canberra.edu.au
 *
 *      The original list of contributors to the Pthreads-win32 project
 *      is contained in the file CONTRIBUTORS.ptw32 included with the
 *      source code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * --------------------------------------------------------------------------
 *
 * Test Synopsis:
 * - Test timeouts and lost signals on a CV.
 *
 * Test Method (Validation or Falsification):
 * - Validation
 *
 * Requirements Tested:
 * -
 *
 * Features Tested:
 * -
 *
 * Cases Tested:
 * -
 *
 * Description:
 * -
 *
 * Environment:
 * -
 *
 * Input:
 * - None.
 *
 * Output:
 * - File name, Line number, and failed expression on failure.
 * - No output on success.
 *
 * Assumptions:
 * -
 *
 * Pass Criteria:
 * - pthread_cond_timedwait returns ETIMEDOUT.
 * - Process returns zero exit status.
 *
 * Fail Criteria:
 * - pthread_cond_timedwait does not return ETIMEDOUT.
 * - Process returns non-zero exit status.
 */

/* Timur Aydin (taydin@snet.net) */

#include "test.h"


static pthread_cond_t cnd;
static pthread_mutex_t mtx;

int pthread_test_condvar3_3()
{
  int rc;

  struct timespec abstime =
    {
      0, 0
    };
  struct _timeb currSysTime;
  const unsigned int NANOSEC_PER_MILLISEC = 1000000;

  assert(pthread_cond_init(&cnd, 0) == 0);
  assert(pthread_mutex_init(&mtx, 0) == 0);

  /* get current system time */
  _ftime(&currSysTime);

  abstime.tv_sec = currSysTime.time;
  abstime.tv_nsec = NANOSEC_PER_MILLISEC * currSysTime.millitm;
  abstime.tv_sec += 1;

  /* Here pthread_cond_timedwait should time out after one second. */

  assert(pthread_mutex_lock(&mtx) == 0);

  assert((rc = pthread_cond_timedwait(&cnd, &mtx, &abstime)) == ETIMEDOUT);

  // Stop some compilers from reporting that rc is not used
  rc = rc;

  assert(pthread_mutex_unlock(&mtx) == 0);

  /* Here, the condition variable is signaled, but there are no
     threads waiting on it. The signal should be lost and
     the next pthread_cond_timedwait should time out too. */

//   assert(pthread_mutex_lock(&mtx) == 0);

  assert((rc = pthread_cond_signal(&cnd)) == 0);
//   assert(pthread_mutex_unlock(&mtx) == 0);

  assert(pthread_mutex_lock(&mtx) == 0);

  _ftime(&currSysTime);
  abstime.tv_sec = currSysTime.time;
  abstime.tv_nsec = NANOSEC_PER_MILLISEC * currSysTime.millitm;
  abstime.tv_sec += 1;

  assert((rc = pthread_cond_timedwait(&cnd, &mtx, &abstime)) == ETIMEDOUT);
  assert(pthread_mutex_unlock(&mtx) == 0);

  assert(pthread_cond_destroy(&cnd) == 0);
  assert(pthread_mutex_destroy(&mtx) == 0);

  return 0;
}
