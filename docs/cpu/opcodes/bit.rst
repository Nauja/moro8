.. -*- coding: utf-8 -*-
.. _bit:

BIT
---

.. module:: moro8

.. contents::
   :local:

Test Bits in Memory with Accumulator.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A AND M, M7 -> N, M6 -> V

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
               M7 + - - - M6

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Zero Page     BIT $44       $24  2   3
                Absolute      BIT $4400     $2C  3   4

Bits 7 and 6 of operand are transfered to bit 7 and 6 of SR (N,V);
the zero-flag is set to the result of operand AND accumulator.
