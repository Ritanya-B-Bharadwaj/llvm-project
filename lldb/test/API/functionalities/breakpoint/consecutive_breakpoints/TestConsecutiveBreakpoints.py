"""
Test that we handle breakpoints on consecutive instructions correctly.
"""

import lldb
from lldbsuite.test.decorators import *
from lldbsuite.test.lldbtest import *
from lldbsuite.test import lldbutil


class ConsecutiveBreakpointsTestCase(TestBase):
    def prepare_test(self):
        self.build()

        (
            self.target,
            self.process,
            self.thread,
            bkpt,
        ) = lldbutil.run_to_source_breakpoint(
            self, "Set breakpoint here", lldb.SBFileSpec("main.cpp")
        )

        # Set breakpoint to the next instruction
        frame = self.thread.GetFrameAtIndex(0)

        address = frame.GetPCAddress()
        instructions = self.target.ReadInstructions(address, 2)
        self.assertEqual(len(instructions), 2)
        self.bkpt_address = instructions[1].GetAddress()
        self.breakpoint2 = self.target.BreakpointCreateByAddress(
            self.bkpt_address.GetLoadAddress(self.target)
        )
        self.assertTrue(
            self.breakpoint2 and self.breakpoint2.GetNumLocations() == 1,
            VALID_BREAKPOINT,
        )

    def finish_test(self):
        # Run the process until termination
        self.process.Continue()
        self.assertState(self.process.GetState(), lldb.eStateExited)

    @no_debug_info_test
    def test_continue(self):
        """Test that continue stops at the second breakpoint."""
        self.prepare_test()

        self.process.Continue()
        self.assertState(self.process.GetState(), lldb.eStateStopped)
        # We should be stopped at the second breakpoint
        self.thread = lldbutil.get_one_thread_stopped_at_breakpoint(
            self.process, self.breakpoint2
        )
        self.assertIsNotNone(
            self.thread, "Expected one thread to be stopped at breakpoint 2"
        )

        self.finish_test()

    @no_debug_info_test
    def test_single_step(self):
        """Test that single step stops at the second breakpoint."""
        self.prepare_test()

        # Instruction step to the next instruction
        # We haven't executed breakpoint2 yet, we're sitting at it now.
        step_over = False
        self.thread.StepInstruction(step_over)

        step_over = False
        self.thread.StepInstruction(step_over)

        # We've now hit the breakpoint and this StepInstruction has
        # been interrupted, it is still sitting on the thread plan stack.

        self.assertState(self.process.GetState(), lldb.eStateStopped)
        self.assertEqual(
            self.thread.GetFrameAtIndex(0).GetPCAddress().GetLoadAddress(self.target),
            self.bkpt_address.GetLoadAddress(self.target),
        )

        # One more instruction to complete the Step that was interrupted
        # earlier.
        self.thread.StepInstruction(step_over)
        strm = lldb.SBStream()
        self.thread.GetDescription(strm)
        self.assertIn("instruction step into", strm.GetData())
        self.assertIsNotNone(self.thread, "Expected to see that step-in had completed")

        self.finish_test()

    @no_debug_info_test
    @skipIf(
        oslist=["windows"],
        archs=["x86_64"],
        bugnumber="github.com/llvm/llvm-project/issues/138083",
    )
    def test_single_step_thread_specific(self):
        """Test that single step stops, even though the second breakpoint is not valid."""
        self.prepare_test()

        # Choose a thread other than the current one. A non-existing thread is
        # fine.
        thread_index = self.process.GetNumThreads() + 1
        self.assertFalse(self.process.GetThreadAtIndex(thread_index).IsValid())
        self.breakpoint2.SetThreadIndex(thread_index)

        step_over = False
        self.thread.StepInstruction(step_over)

        self.assertState(self.process.GetState(), lldb.eStateStopped)
        self.assertEqual(
            self.thread.GetFrameAtIndex(0).GetPCAddress().GetLoadAddress(self.target),
            self.bkpt_address.GetLoadAddress(self.target),
        )
        self.assertEqual(
            self.thread.GetStopReason(),
            lldb.eStopReasonPlanComplete,
            "Stop reason should be 'plan complete'",
        )

        # Hit our second breakpoint
        self.process.Continue()

        self.finish_test()
