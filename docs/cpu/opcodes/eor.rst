.. -*- coding: utf-8 -*-
.. _eor:

EOR
---

.. module:: moro8

.. contents::
   :local:

Exclusive-OR Memory with Accumulator.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A EOR M -> A

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre

                MODE          SYNTAX        HEX LEN TIM
                Immediate     EOR #$44      $49  2   2
                Zero Page     EOR $44       $45  2   3
                Zero Page,X   EOR $44,X     $55  2   4
                Absolute      EOR $4400     $4D  3   4
                Absolute,X    EOR $4400,X   $5D  3   4+
                Absolute,Y    EOR $4400,Y   $59  3   4+
                Indirect,X    EOR ($44,X)   $41  2   6
                Indirect,Y    EOR ($44),Y   $51  2   5+

