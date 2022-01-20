.. -*- coding: utf-8 -*-
.. _sbc:

SBC
---

.. module:: moro8

.. contents::
   :local:
      
Subtract Memory from Accumulator with Borrow.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A - M - C -> A

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + + - - +

    .. container:: moro8-synopsis moro8-pre

        MODE          SYNTAX        HEX LEN TIM
        Immediate     SBC #$44      $E9  2   2
        Zero Page     SBC $44       $E5  2   3
        Zero Page,X   SBC $44,X     $F5  2   4
        Absolute      SBC $4400     $ED  3   4
        Absolute,X    SBC $4400,X   $FD  3   4+
        Absolute,Y    SBC $4400,Y   $F9  3   4+
        Indirect,X    SBC ($44,X)   $E1  2   6
        Indirect,Y    SBC ($44),Y   $F1  2   5+

