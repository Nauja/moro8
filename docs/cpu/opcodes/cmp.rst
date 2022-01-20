.. -*- coding: utf-8 -*-
.. _cmp:

CMP
---

.. module:: moro8

.. contents::
   :local:

Compare Memory with Accumulator.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A - M

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Immediate     CMP #$44      $C9  2   2
                Zero Page     CMP $44       $C5  2   3
                Zero Page,X   CMP $44,X     $D5  2   4
                Absolute      CMP $4400     $CD  3   4
                Absolute,X    CMP $4400,X   $DD  3   4+
                Absolute,Y    CMP $4400,Y   $D9  3   4+
                Indirect,X    CMP ($44,X)   $C1  2   6
                Indirect,Y    CMP ($44),Y   $D1  2   5+

Bits 7 and 6 of operand are transfered to bit 7 and 6 of SR (N,V);
the zero-flag is set to the result of operand AND accumulator.
