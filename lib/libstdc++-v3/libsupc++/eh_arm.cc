// -*- C++ -*- ARM specific Exception handling support routines.
// Copyright (C) 2004, 2005, 2008, 2009, 2010 Free Software Foundation, Inc.
//
// This file is part of GCC.
//
// GCC is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// GCC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.
//
// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#include "cxxabi.h"
#include "unwind-cxx.h"

#ifdef __ARM_EABI_UNWINDER__

using namespace __cxxabiv1;

// Do the work for __cxa_end_cleanup.  Returns the currently propagating
// exception object.
extern "C" _Unwind_Exception *
__gnu_end_cleanup(void)
{
  /*__cxa_exception *header;
  __cxa_eh_globals *globals = __cxa_get_globals();

  header = globals->propagatingExceptions;

  // Check something hasn't gone horribly wrong.
  if (!header)
    std::terminate();

  if (__is_gxx_exception_class(header->unwindHeader.exception_class))
    {
      header->propagationCount--;
      if (header->propagationCount == 0)
	{
	  // Remove exception from chain.
	  globals->propagatingExceptions = header->nextPropagatingException;
	  header->nextPropagatingException = NULL;
	}
    }
  else
    globals->propagatingExceptions = NULL;
  
  return &header->unwindHeader;*/
  return NULL;
}

extern "C" void _Unwind_Resume(_Unwind_Control_Block* b)
{
    return NULL;
}

extern "C" _Unwind_Reason_Code
__gxx_personality_v0 (int version,
		      _Unwind_Action actions,
		      _Unwind_Exception_Class exception_class,
		      struct _Unwind_Exception *ue_header,
		      struct _Unwind_Context *context)
{
    return 0;
}

// Assembly wrapper to call __gnu_end_cleanup without clobbering r1-r3.
// Also push r4 to preserve stack alignment.
#ifdef __thumb__
asm ("  .pushsection .text.__cxa_end_cleanup\n"
"	.global __cxa_end_cleanup\n"
"	.type __cxa_end_cleanup, \"function\"\n"
"	.thumb_func\n"
"__cxa_end_cleanup:\n"
"	push\t{r1, r2, r3, r4}\n"
"	bl\t__gnu_end_cleanup\n"
"	pop\t{r1, r2, r3, r4}\n"
"	bl\t_Unwind_Resume @ Never returns\n"
"	.popsection\n");
#else
asm ("  .pushsection .text.__cxa_end_cleanup\n"
"	.global __cxa_end_cleanup\n"
"	.type __cxa_end_cleanup, \"function\"\n"
"__cxa_end_cleanup:\n"
"	stmfd\tsp!, {r1, r2, r3, r4}\n"
"	bl\t__gnu_end_cleanup\n"
"	ldmfd\tsp!, {r1, r2, r3, r4}\n"
"	bl\t_Unwind_Resume @ Never returns\n"
"	.popsection\n");
#endif

#endif
