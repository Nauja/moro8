.. -*- coding: utf-8 -*-
.. _ora:

ORA
---

.. module:: moro8

.. contents::
   :local:
      
OR Memory with Accumulator.

.. container:: moro8-opcode

    .. container:: moro8-header
        
        .. container:: moro8-pre

                ACTION
                A OR M -> A

        .. container:: moro8-pre

                FLAGS
                N Z C I D V
                + + - - - -

    .. container:: moro8-synopsis moro8-pre
                
                MODE          SYNTAX        HEX LEN TIM
                Immediate     ORA #$44      $09  2   2
                Zero Page     ORA $44       $05  2   3
                Zero Page,X   ORA $44,X     $15  2   4
                Absolute      ORA $4400     $0D  3   4
                Absolute,X    ORA $4400,X   $1D  3   4+
                Absolute,Y    ORA $4400,Y   $19  3   4+
                Indirect,X    ORA ($44,X)   $01  2   6
                Indirect,Y    ORA ($44),Y   $11  2   5+
