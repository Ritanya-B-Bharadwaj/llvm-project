! An initializer reading omp_orig must init the allocatable private copy from the
! original element, not a fabricated zero.

! RUN: %flang_fc1 -emit-hlfir -fopenmp -fopenmp-version=45 %s -o - | FileCheck %s

subroutine test_udr_orig_init()
  implicit none
  integer :: i
  integer, allocatable :: a

  !$omp declare reduction (myinit : integer : omp_out = omp_out + omp_in) &
  !$omp & initializer (omp_priv = omp_orig)

  allocate(a)
  a = 7

  !$omp parallel do reduction(myinit : a)
  do i = 1, 4
    a = a + i
  end do
end subroutine

! CHECK-LABEL: omp.declare_reduction @{{.*}}myinit_byref_box_heap_i32 : !fir.ref<!fir.box<!fir.heap<i32>>>
! CHECK:       init {
! CHECK:       ^bb0(%[[MOLD:.*]]: !fir.ref<!fir.box<!fir.heap<i32>>>, %[[ALLOC:.*]]: !fir.ref<!fir.box<!fir.heap<i32>>>):
! The original element is read (omp_orig), not replaced by a constant zero.
! CHECK:         %[[MBOX:.*]] = fir.load %[[MOLD]]
! CHECK:         %[[MADDR:.*]] = fir.box_addr %[[MBOX]]
! CHECK:         fir.load %[[MADDR]]
! CHECK:         omp.yield
! CHECK:       } combiner {
! CHECK:         fir.box_addr
! CHECK:         arith.addi
! CHECK:         fir.store
! CHECK:         omp.yield
! CHECK:       } cleanup {
! CHECK:         fir.freemem
! CHECK:         omp.yield

! CHECK-LABEL: func.func @_QPtest_udr_orig_init
! CHECK:         omp.wsloop {{.*}} reduction(byref @{{.*}}myinit_byref_box_heap_i32 %{{.*}} -> %{{.*}} : !fir.ref<!fir.box<!fir.heap<i32>>>)
