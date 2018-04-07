/*
 *  This routine is the initialization task for this test program.
 *  It is called from init_exec and has the responsibility for creating
 *  and starting the tasks that make up the test.  If the time of day
 *  clock is required for the test, it should also be set to a known
 *  value by this function.
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *  Based in part on OAR works.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>

#include <rtems.h>
#include <stdio.h>
#include <stdlib.h>

#if !defined(BSP_SMALL_MEMORY)
#define RTEMS_TEST_IO_STREAM
#ifdef RTEMS_TEST_IO_STREAM
  #include <iostream>
#endif

extern "C" {
  extern rtems_task main_task(rtems_task_argument);
}

static int num_inst = 0;

class A {
public:
    A(void)
    {
        num_inst++;
        printf(
          "Hey I'm in base class constructor number %d for %p.\n",
          num_inst,
          this
        );

 /*
  * Make sure we use some space
  */

        string = new char[50];
 sprintf(string, "Instantiation order %d", num_inst);
    };

    virtual ~A()
    {
        printf(
          "Hey I'm in base class destructor number %d for %p.\n",
          num_inst,
          this
        );
 print();
        num_inst--;
    };

    virtual void print()  { printf("%s\n", string); };

protected:
    char  *string;
};

class B : public A {
public:
    B(void)
    {
        num_inst++;
        printf(
          "Hey I'm in derived class constructor number %d for %p.\n",
          num_inst,
          this
        );

 /*
  * Make sure we use some space
  */

        string = new char[50];
 sprintf(string, "Instantiation order %d", num_inst);
    };

    ~B()
    {
        printf(
          "Hey I'm in derived class destructor number %d for %p.\n",
          num_inst,
          this
        );
       print();
        num_inst--;
    };

    void print()  { printf("Derived class - %s\n", string); }
};


A foo;
B foobar;

void
cdtest(void)
{
    A bar, blech, blah;
    B bleak;

#ifdef RTEMS_TEST_IO_STREAM
    std::cout << "Testing a C++ I/O stream" << std::endl;
#else
    printf("IO Stream not tested\n");
#endif

    bar = blech;

 printf( "before try block\n" );
 try
 {
    throw "Raising this";
 }
 catch( const char *e )
 {
    printf( "Got it: %s\n", e );
 }
 printf( "catch got called, exception handling worked !!!\n" );
}
#endif



extern "C" {
  rtems_task Init(
    rtems_task_argument arg
  );
};

rtems_task Init(
  rtems_task_argument
)
{
    printf( "\n\n*** CONSTRUCTOR/DESTRUCTOR TEST ***\n" );

#if !defined(BSP_SMALL_MEMORY)
    cdtest();
#endif

    printf( "*** END OF CONSTRUCTOR/DESTRUCTOR TEST ***\n\n\n" );

    exit(0);
}

/* configuration information */

#include <bsp.h>

#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

#define CONFIGURE_MAXIMUM_TASKS 1
/*
 * GCC C++ support requires Classic Semaphores but this could change to 
 * POSIX mutexes at some point in the future. When that happens, this will
 * need to change.
 */
#define CONFIGURE_MAXIMUM_SEMAPHORES 1

#define CONFIGURE_INIT

#include <rtems/confdefs.h>
