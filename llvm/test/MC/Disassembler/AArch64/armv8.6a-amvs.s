// RUN: llvm-mc -triple aarch64 -show-encoding -disassemble -mattr=+amvs %s  | FileCheck %s
// RUN: llvm-mc -triple aarch64 -show-encoding -disassemble -mattr=+v8.6a -o - %s | FileCheck %s
// RUN: llvm-mc -triple aarch64 -show-encoding -disassemble -o - %s | FileCheck --check-prefix=NOAMVS %s
[0xc0,0xd2,0x1b,0xd5]
[0xc0,0xd2,0x3b,0xd5]
[0x00,0xd8,0x1c,0xd5]
[0x20,0xd8,0x1c,0xd5]
[0x40,0xd8,0x1c,0xd5]
[0x60,0xd8,0x1c,0xd5]
[0x80,0xd8,0x1c,0xd5]
[0xa0,0xd8,0x1c,0xd5]
[0xc0,0xd8,0x1c,0xd5]
[0xe0,0xd8,0x1c,0xd5]
[0x00,0xd9,0x1c,0xd5]
[0x20,0xd9,0x1c,0xd5]
[0x40,0xd9,0x1c,0xd5]
[0x60,0xd9,0x1c,0xd5]
[0x80,0xd9,0x1c,0xd5]
[0xa0,0xd9,0x1c,0xd5]
[0xc0,0xd9,0x1c,0xd5]
[0xe0,0xd9,0x1c,0xd5]
[0x00,0xd8,0x3c,0xd5]
[0x20,0xd8,0x3c,0xd5]
[0x40,0xd8,0x3c,0xd5]
[0x60,0xd8,0x3c,0xd5]
[0x80,0xd8,0x3c,0xd5]
[0xa0,0xd8,0x3c,0xd5]
[0xc0,0xd8,0x3c,0xd5]
[0xe0,0xd8,0x3c,0xd5]
[0x00,0xd9,0x3c,0xd5]
[0x20,0xd9,0x3c,0xd5]
[0x40,0xd9,0x3c,0xd5]
[0x60,0xd9,0x3c,0xd5]
[0x80,0xd9,0x3c,0xd5]
[0xa0,0xd9,0x3c,0xd5]
[0xc0,0xd9,0x3c,0xd5]
[0xe0,0xd9,0x3c,0xd5]
[0x00,0xda,0x1c,0xd5]
[0x20,0xda,0x1c,0xd5]
[0x40,0xda,0x1c,0xd5]
[0x60,0xda,0x1c,0xd5]
[0x80,0xda,0x1c,0xd5]
[0xa0,0xda,0x1c,0xd5]
[0xc0,0xda,0x1c,0xd5]
[0xe0,0xda,0x1c,0xd5]
[0x00,0xdb,0x1c,0xd5]
[0x20,0xdb,0x1c,0xd5]
[0x40,0xdb,0x1c,0xd5]
[0x60,0xdb,0x1c,0xd5]
[0x80,0xdb,0x1c,0xd5]
[0xa0,0xdb,0x1c,0xd5]
[0xc0,0xdb,0x1c,0xd5]
[0xe0,0xdb,0x1c,0xd5]
[0x00,0xda,0x3c,0xd5]
[0x20,0xda,0x3c,0xd5]
[0x40,0xda,0x3c,0xd5]
[0x60,0xda,0x3c,0xd5]
[0x80,0xda,0x3c,0xd5]
[0xa0,0xda,0x3c,0xd5]
[0xc0,0xda,0x3c,0xd5]
[0xe0,0xda,0x3c,0xd5]
[0x00,0xdb,0x3c,0xd5]
[0x20,0xdb,0x3c,0xd5]
[0x40,0xdb,0x3c,0xd5]
[0x60,0xdb,0x3c,0xd5]
[0x80,0xdb,0x3c,0xd5]
[0xa0,0xdb,0x3c,0xd5]
[0xc0,0xdb,0x3c,0xd5]
[0xe0,0xdb,0x3c,0xd5]
// CHECK:       msr     S3_3_C13_C2_6, x0       // encoding: [0xc0,0xd2,0x1b,0xd5]
// CHECK-NEXT:  mrs     x0, AMCG1IDR_EL0        // encoding: [0xc0,0xd2,0x3b,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF00_EL2, x0   // encoding: [0x00,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF01_EL2, x0   // encoding: [0x20,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF02_EL2, x0   // encoding: [0x40,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF03_EL2, x0   // encoding: [0x60,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF04_EL2, x0   // encoding: [0x80,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF05_EL2, x0   // encoding: [0xa0,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF06_EL2, x0   // encoding: [0xc0,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF07_EL2, x0   // encoding: [0xe0,0xd8,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF08_EL2, x0   // encoding: [0x00,0xd9,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF09_EL2, x0   // encoding: [0x20,0xd9,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF010_EL2, x0  // encoding: [0x40,0xd9,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF011_EL2, x0  // encoding: [0x60,0xd9,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF012_EL2, x0  // encoding: [0x80,0xd9,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF013_EL2, x0  // encoding: [0xa0,0xd9,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF014_EL2, x0  // encoding: [0xc0,0xd9,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF015_EL2, x0  // encoding: [0xe0,0xd9,0x1c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF00_EL2   // encoding: [0x00,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF01_EL2   // encoding: [0x20,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF02_EL2   // encoding: [0x40,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF03_EL2   // encoding: [0x60,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF04_EL2   // encoding: [0x80,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF05_EL2   // encoding: [0xa0,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF06_EL2   // encoding: [0xc0,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF07_EL2   // encoding: [0xe0,0xd8,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF08_EL2   // encoding: [0x00,0xd9,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF09_EL2   // encoding: [0x20,0xd9,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF010_EL2  // encoding: [0x40,0xd9,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF011_EL2  // encoding: [0x60,0xd9,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF012_EL2  // encoding: [0x80,0xd9,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF013_EL2  // encoding: [0xa0,0xd9,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF014_EL2  // encoding: [0xc0,0xd9,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF015_EL2  // encoding: [0xe0,0xd9,0x3c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF10_EL2, x0   // encoding: [0x00,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF11_EL2, x0   // encoding: [0x20,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF12_EL2, x0   // encoding: [0x40,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF13_EL2, x0   // encoding: [0x60,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF14_EL2, x0   // encoding: [0x80,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF15_EL2, x0   // encoding: [0xa0,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF16_EL2, x0   // encoding: [0xc0,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF17_EL2, x0   // encoding: [0xe0,0xda,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF18_EL2, x0   // encoding: [0x00,0xdb,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF19_EL2, x0   // encoding: [0x20,0xdb,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF110_EL2, x0  // encoding: [0x40,0xdb,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF111_EL2, x0  // encoding: [0x60,0xdb,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF112_EL2, x0  // encoding: [0x80,0xdb,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF113_EL2, x0  // encoding: [0xa0,0xdb,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF114_EL2, x0  // encoding: [0xc0,0xdb,0x1c,0xd5]
// CHECK-NEXT:  msr     AMEVCNTVOFF115_EL2, x0  // encoding: [0xe0,0xdb,0x1c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF10_EL2   // encoding: [0x00,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF11_EL2   // encoding: [0x20,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF12_EL2   // encoding: [0x40,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF13_EL2   // encoding: [0x60,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF14_EL2   // encoding: [0x80,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF15_EL2   // encoding: [0xa0,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF16_EL2   // encoding: [0xc0,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF17_EL2   // encoding: [0xe0,0xda,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF18_EL2   // encoding: [0x00,0xdb,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF19_EL2   // encoding: [0x20,0xdb,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF110_EL2  // encoding: [0x40,0xdb,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF111_EL2  // encoding: [0x60,0xdb,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF112_EL2  // encoding: [0x80,0xdb,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF113_EL2  // encoding: [0xa0,0xdb,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF114_EL2  // encoding: [0xc0,0xdb,0x3c,0xd5]
// CHECK-NEXT:  mrs     x0, AMEVCNTVOFF115_EL2  // encoding: [0xe0,0xdb,0x3c,0xd5]
// NOAMVS:       msr     S3_3_C13_C2_6, x0       // encoding: [0xc0,0xd2,0x1b,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_3_C13_C2_6       // encoding: [0xc0,0xd2,0x3b,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_0, x0       // encoding: [0x00,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_1, x0       // encoding: [0x20,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_2, x0       // encoding: [0x40,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_3, x0       // encoding: [0x60,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_4, x0       // encoding: [0x80,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_5, x0       // encoding: [0xa0,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_6, x0       // encoding: [0xc0,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C8_7, x0       // encoding: [0xe0,0xd8,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_0, x0       // encoding: [0x00,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_1, x0       // encoding: [0x20,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_2, x0       // encoding: [0x40,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_3, x0       // encoding: [0x60,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_4, x0       // encoding: [0x80,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_5, x0       // encoding: [0xa0,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_6, x0       // encoding: [0xc0,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C9_7, x0       // encoding: [0xe0,0xd9,0x1c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_0       // encoding: [0x00,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_1       // encoding: [0x20,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_2       // encoding: [0x40,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_3       // encoding: [0x60,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_4       // encoding: [0x80,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_5       // encoding: [0xa0,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_6       // encoding: [0xc0,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C8_7       // encoding: [0xe0,0xd8,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_0       // encoding: [0x00,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_1       // encoding: [0x20,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_2       // encoding: [0x40,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_3       // encoding: [0x60,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_4       // encoding: [0x80,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_5       // encoding: [0xa0,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_6       // encoding: [0xc0,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C9_7       // encoding: [0xe0,0xd9,0x3c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_0, x0      // encoding: [0x00,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_1, x0      // encoding: [0x20,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_2, x0      // encoding: [0x40,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_3, x0      // encoding: [0x60,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_4, x0      // encoding: [0x80,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_5, x0      // encoding: [0xa0,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_6, x0      // encoding: [0xc0,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C10_7, x0      // encoding: [0xe0,0xda,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_0, x0      // encoding: [0x00,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_1, x0      // encoding: [0x20,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_2, x0      // encoding: [0x40,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_3, x0      // encoding: [0x60,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_4, x0      // encoding: [0x80,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_5, x0      // encoding: [0xa0,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_6, x0      // encoding: [0xc0,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  msr     S3_4_C13_C11_7, x0      // encoding: [0xe0,0xdb,0x1c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_0      // encoding: [0x00,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_1      // encoding: [0x20,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_2      // encoding: [0x40,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_3      // encoding: [0x60,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_4      // encoding: [0x80,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_5      // encoding: [0xa0,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_6      // encoding: [0xc0,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C10_7      // encoding: [0xe0,0xda,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_0      // encoding: [0x00,0xdb,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_1      // encoding: [0x20,0xdb,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_2      // encoding: [0x40,0xdb,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_3      // encoding: [0x60,0xdb,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_4      // encoding: [0x80,0xdb,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_5      // encoding: [0xa0,0xdb,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_6      // encoding: [0xc0,0xdb,0x3c,0xd5]
// NOAMVS-NEXT:  mrs     x0, S3_4_C13_C11_7      // encoding: [0xe0,0xdb,0x3c,0xd5]
